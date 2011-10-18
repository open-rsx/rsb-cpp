/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "DirectEventReceivingStrategy.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#include <rsc/debug/DebugTools.h>
#include <rsc/runtime/ContainerIO.h>
#include <rsc/misc/langutils.h>

#include "../MetaData.h"
#include "../filter/Filter.h"

using namespace std;

using namespace boost;

using namespace rsc::runtime;
using namespace rsc::logging;

namespace rsb {
namespace eventprocessing {

EventReceivingStrategy* DirectEventReceivingStrategy::create(const Properties &props) {
    return new DirectEventReceivingStrategy(props.getAs<bool>("singlethreaded"));
}

DirectEventReceivingStrategy::DirectEventReceivingStrategy(bool singleThreaded) :
    logger(Logger::getLogger("rsb.eventprocessing.DirectEventReceivingStrategy")),
    errorStrategy(ParticipantConfig::LOG),
    singleThreaded(singleThreaded) {
}

DirectEventReceivingStrategy::~DirectEventReceivingStrategy() {
}

void DirectEventReceivingStrategy::printContents(ostream &stream) const {
    boost::shared_lock<boost::shared_mutex> filtersLock(this->filtersMutex);
    boost::shared_lock<boost::shared_mutex> errorLock(this->errorStrategyMutex);
    stream << "filters = " << this->filters
           << ", errorStrategy = " << this->errorStrategy
           << ", singleThreaded = " << this->singleThreaded;
}

void DirectEventReceivingStrategy::setHandlerErrorStrategy(
        const ParticipantConfig::ErrorStrategy &strategy) {
    boost::shared_lock<boost::shared_mutex> lock(errorStrategyMutex);
    this->errorStrategy = strategy;
}

bool DirectEventReceivingStrategy::filter(EventPtr e) {
    // match event
    try {
        if (this->singleThreaded) {
            return filterNoLock(e);
        } else {
            boost::shared_lock<boost::shared_mutex> lock(filtersMutex);

            return filterNoLock(e);
        }
    } catch (const std::exception& ex) {

        stringstream s;
        s << "Exception matching event " << e << ":" << endl;
        s << ex.what() << endl;
        s << rsc::debug::DebugTools::newInstance()->exceptionInfo(ex);

        handleDispatchError(s.str());

    } catch (...) {

        stringstream s;
        s << "Catch-all handler called matching event " << endl;
        rsc::debug::DebugToolsPtr tool = rsc::debug::DebugTools::newInstance();
        vector<string> trace = tool->createBacktrace();
        s << tool->formatBacktrace(trace);

        handleDispatchError(s.str());

    }

    return false;

}

bool DirectEventReceivingStrategy::filterNoLock(EventPtr e) {
    for (set<filter::FilterPtr>::const_iterator filterIt =
             filters.begin(); filterIt != filters.end(); ++filterIt) {
        if (!(*filterIt)->match(e)) {
            return false;
        }
    }
    return true;
}

void DirectEventReceivingStrategy::handleDispatchError(const string &message) {
    boost::shared_lock<boost::shared_mutex> strategyLock(errorStrategyMutex);

    switch (errorStrategy) {
    case ParticipantConfig::LOG:
        RSCERROR(logger, message);
        break;
    case ParticipantConfig::PRINT:
        cerr << message << endl;
        break;
    case ParticipantConfig::EXIT:
        cerr << message << endl;
        exit(1);
        break;
    default:
        RSCWARN(logger, "Unknown error strategy: " << errorStrategy);
        RSCERROR(logger, message);
        break;
    }

}

void DirectEventReceivingStrategy::deliver(rsb::HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Delivering event " << e << " to handler " << handler);

    try {

        handler->handle(e);

    } catch (const std::exception& ex) {

        stringstream s;
        s << "Exception dispatching event " << e << " to handler " << handler
                << ":" << endl;
        s << ex.what() << endl;
        s << rsc::debug::DebugTools::newInstance()->exceptionInfo(ex);

        handleDispatchError(s.str());

    } catch (...) {

        stringstream s;
        s << "Catch-all handler called dispatching event " << e
                << " to handler " << handler << endl;
        rsc::debug::DebugToolsPtr tool = rsc::debug::DebugTools::newInstance();
        vector<string> trace = tool->createBacktrace();
        s << tool->formatBacktrace(trace);

        handleDispatchError(s.str());

    }

}

void DirectEventReceivingStrategy::handle(EventPtr event) {
    if (this->singleThreaded) {
        handleNoLock(event);
    } else {
        boost::shared_lock<boost::shared_mutex> lock(this->handlerMutex);

        handleNoLock(event);
    }
}

void DirectEventReceivingStrategy::handleNoLock(EventPtr event) {
    event->mutableMetaData().setDeliverTime(rsc::misc::currentTimeMicros());

    if (filter(event)) {
        for (HandlerList::const_iterator it = this->handlers.begin(); it
                 != this->handlers.end(); ++it)
            deliver(*it, event);
    }
}

void DirectEventReceivingStrategy::addHandler(rsb::HandlerPtr handler,
        const bool &/*wait*/) {
    boost::shared_lock<boost::shared_mutex> lock(this->handlerMutex);

    this->handlers.push_back(handler);
}

void DirectEventReceivingStrategy::removeHandler(rsb::HandlerPtr handler,
        const bool &/*wait*/) {
    boost::shared_lock<boost::shared_mutex> lock(this->handlerMutex);

    this->handlers.remove(handler);
}

void DirectEventReceivingStrategy::addFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);

    this->filters.insert(filter);
}

void DirectEventReceivingStrategy::removeFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);

    this->filters.erase(filter);
}

}
}
