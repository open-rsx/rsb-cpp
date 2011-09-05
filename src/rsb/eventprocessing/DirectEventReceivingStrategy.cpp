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

#include <rsc/debug/DebugTools.h>
#include <rsc/runtime/ContainerIO.h>
#include <rsc/misc/langutils.h>

using namespace std;

using namespace rsc::logging;

namespace rsb {
namespace eventprocessing {

DirectEventReceivingStrategy::DirectEventReceivingStrategy() :
            logger(
                    Logger::getLogger(
                            "rsb.eventprocessing.DirectEventReceivingStrategy")),
            errorStrategy(ParticipantConfig::LOG), singleThreaded(false) {
}

DirectEventReceivingStrategy::~DirectEventReceivingStrategy() {
}

void DirectEventReceivingStrategy::printContents(ostream &stream) const {
    boost::shared_lock<boost::shared_mutex> filtersLock(filtersMutex);
    boost::recursive_mutex::scoped_lock errorLock(errorStrategyMutex);
    stream << "filters = " << filters << ", errorStrategy = " << errorStrategy;
}

void DirectEventReceivingStrategy::setHandlerErrorStrategy(
        const ParticipantConfig::ErrorStrategy &strategy) {
    boost::recursive_mutex::scoped_lock lock(errorStrategyMutex);
    this->errorStrategy = strategy;
}

bool DirectEventReceivingStrategy::filter(EventPtr e) {
    // match event
    try {

        if (singleThreaded) {
            for (set<filter::FilterPtr>::const_iterator filterIt =
                    filters.begin(); filterIt != filters.end(); ++filterIt) {
                if (!(*filterIt)->match(e)) {
                    return false;
                }
            }
        } else {
            boost::shared_lock<boost::shared_mutex> lock(filtersMutex);
            for (set<filter::FilterPtr>::const_iterator filterIt =
                    filters.begin(); filterIt != filters.end(); ++filterIt) {
                if (!(*filterIt)->match(e)) {
                    return false;
                }
            }
        }

        return true;

    } catch (const exception& ex) {

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

void DirectEventReceivingStrategy::handleDispatchError(const string &message) {

    boost::recursive_mutex::scoped_lock strategyLock(errorStrategyMutex);
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

    } catch (const exception& ex) {

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
    event->mutableMetaData().setDeliverTime(rsc::misc::currentTimeMicros());

    if (singleThreaded) {
        if (filter(event)) {
            for (HandlerList::const_iterator it = this->handlers.begin(); it
                    != this->handlers.end(); ++it)
                deliver(*it, event);
        }
    } else {
        boost::shared_lock<boost::shared_mutex> lock(this->handlerMutex);
        if (filter(event)) {
            for (HandlerList::const_iterator it = this->handlers.begin(); it
                    != this->handlers.end(); ++it)
                deliver(*it, event);
        }
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
    filters.insert(filter);
}

void DirectEventReceivingStrategy::removeFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);
    filters.erase(filter);
}

}
}
