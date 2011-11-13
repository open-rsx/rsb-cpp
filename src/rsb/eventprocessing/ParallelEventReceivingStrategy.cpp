/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *               2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "ParallelEventReceivingStrategy.h"

#include <rsc/debug/DebugTools.h>
#include <rsc/runtime/ContainerIO.h>
#include <rsc/misc/langutils.h>

#include "../MetaData.h"
#include "../filter/Filter.h"

using namespace std;

using namespace boost;

using namespace rsc::runtime;
using namespace rsc::logging;
using namespace rsc::debug;

namespace rsb {
namespace eventprocessing {

EventReceivingStrategy* ParallelEventReceivingStrategy::create(const Properties& props) {
    return new ParallelEventReceivingStrategy(props.getAs<unsigned int>("threads", 5));
}

ParallelEventReceivingStrategy::ParallelEventReceivingStrategy(unsigned int numThreads) :
    logger(Logger::getLogger("rsb.eventprocessing.ParallelEventReceivingStrategy")),
    pool(numThreads,
         boost::bind(&ParallelEventReceivingStrategy::deliver, this, _1, _2),
         boost::bind(&ParallelEventReceivingStrategy::filter, this, _1, _2)),
    errorStrategy(ParticipantConfig::LOG) {
    pool.start();
}

ParallelEventReceivingStrategy::~ParallelEventReceivingStrategy() {
    pool.stop();
}

string ParallelEventReceivingStrategy::getClassName() const {
    return "ParallelEventReceivingStrategy";
}

void ParallelEventReceivingStrategy::printContents(ostream& stream) const {
    shared_lock<shared_mutex> filtersLock(filtersMutex);
    recursive_mutex::scoped_lock errorLock(errorStrategyMutex);
    stream << "filters = " << filters << ", errorStrategy = " << errorStrategy;
}

void ParallelEventReceivingStrategy::setHandlerErrorStrategy(
        const ParticipantConfig::ErrorStrategy& strategy) {
    recursive_mutex::scoped_lock lock(errorStrategyMutex);
    this->errorStrategy = strategy;
}

bool ParallelEventReceivingStrategy::filter(rsb::HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Matching event " << e << " for handler " << handler);

    // match event
    try {

        if (!handler->acceptsMethod(e->getMethod())) {
            return false;
        }

        shared_lock<shared_mutex> lock(filtersMutex);
        for (set<filter::FilterPtr>::const_iterator filterIt = filters.begin(); filterIt
                != filters.end(); ++filterIt) {
            if (!(*filterIt)->match(e)) {
                return false;
            }
        }

        return true;

    } catch (const std::exception& ex) {

        stringstream s;
        s << "Exception matching event " << e << " for handler " << handler
                << ":" << endl;
        s << ex.what() << endl;
        s << DebugTools::newInstance()->exceptionInfo(ex);

        handleDispatchError(s.str());

    } catch (...) {

        stringstream s;
        s << "Catch-all handler called matching event " << e << " for handler "
                << handler << endl;
        DebugToolsPtr tool = DebugTools::newInstance();
        vector<string> trace = tool->createBacktrace();
        s << tool->formatBacktrace(trace);

        handleDispatchError(s.str());

    }

    return false;

}

void ParallelEventReceivingStrategy::handleDispatchError(const string& message) {

    recursive_mutex::scoped_lock strategyLock(errorStrategyMutex);
    switch (errorStrategy) {
    case ParticipantConfig::LOG:
        RSCERROR(logger, message)
        ;
        break;
    case ParticipantConfig::PRINT:
        cerr << message << endl;
        break;
    case ParticipantConfig::EXIT:
        cerr << message << endl;
        exit(1);
        break;
    default:
        RSCWARN(logger, "Unknown error strategy: " << errorStrategy)
        ;
        RSCERROR(logger, message)
        ;
        break;
    }

}

void ParallelEventReceivingStrategy::deliver(rsb::HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Delivering event " << e << " to handler " << handler);

    try {

        handler->handle(e);

    } catch (const std::exception& ex) {

        stringstream s;
        s << "Exception dispatching event " << e << " to handler " << handler
                << ":" << endl;
        s << ex.what() << endl;
        s << DebugTools::newInstance()->exceptionInfo(ex);

        handleDispatchError(s.str());

    } catch (...) {

        stringstream s;
        s << "Catch-all handler called dispatching event " << e
                << " to handler " << handler << endl;
        DebugToolsPtr tool = DebugTools::newInstance();
        vector<string> trace = tool->createBacktrace();
        s << tool->formatBacktrace(trace);

        handleDispatchError(s.str());

    }

}

void ParallelEventReceivingStrategy::handle(EventPtr event) {
    event->mutableMetaData().setDeliverTime(rsc::misc::currentTimeMicros());
    pool.push(event);
}

void ParallelEventReceivingStrategy::addHandler(rsb::HandlerPtr handler,
        const bool& /*wait*/) {
    // wait can be ignored because the pool always ensures this
    pool.registerReceiver(handler);
}

void ParallelEventReceivingStrategy::removeHandler(rsb::HandlerPtr handler,
        const bool& /*wait*/) {
    // wait can be ignored because the pool always ensures this
    pool.unregisterReceiver(handler);
}

void ParallelEventReceivingStrategy::addFilter(filter::FilterPtr filter) {
    unique_lock<shared_mutex> lock(filtersMutex);
    filters.insert(filter);
}

void ParallelEventReceivingStrategy::removeFilter(filter::FilterPtr filter) {
    unique_lock<shared_mutex> lock(filtersMutex);
    filters.erase(filter);
}

}
}
