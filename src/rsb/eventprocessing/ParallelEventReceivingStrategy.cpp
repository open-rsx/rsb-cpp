/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include <rsc/runtime/ContainerIO.h>

using namespace std;

namespace rsb {
namespace eventprocessing {

ParallelEventReceivingStrategy::ParallelEventReceivingStrategy() :
    logger(
            rsc::logging::Logger::getLogger(
                    "rsb.ParallelEventReceivingStrategy")),
            pool(5, boost::bind(&ParallelEventReceivingStrategy::deliver, this,
                    _1, _2), boost::bind(
                    &ParallelEventReceivingStrategy::filter, this, _1, _2)) {
    pool.start();
}

ParallelEventReceivingStrategy::ParallelEventReceivingStrategy(
        unsigned int num_threads) :
    logger(
            rsc::logging::Logger::getLogger(
                    "rsb.ParallelEventReceivingStrategy")),
            pool(num_threads, boost::bind(
                    &ParallelEventReceivingStrategy::deliver, this, _1, _2),
                    boost::bind(&ParallelEventReceivingStrategy::filter, this,
                            _1, _2)) {
    pool.start();
}

ParallelEventReceivingStrategy::~ParallelEventReceivingStrategy() {
    pool.stop();
}

string ParallelEventReceivingStrategy::getClassName() const {
    return "ParallelEventReceivingStrategy";
}

void ParallelEventReceivingStrategy::printContents(ostream &stream) const {
    stream << "filters = " << filters;
}

bool ParallelEventReceivingStrategy::filter(HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Matching event " << e << " for handler " << handler);

    // match event
    try {

        boost::shared_lock<boost::shared_mutex> lock(filtersMutex);
        for (set<filter::FilterPtr>::const_iterator filterIt = filters.begin(); filterIt
                != filters.end(); ++filterIt) {
            if (!(*filterIt)->match(e)) {
                return false;
            }
        }

        return true;

    } catch (const exception& ex) {
        RSCFATAL(logger, "Exception matching event " << *e
                << " for handler " << handler << ":" << ex.what());
    } catch (...) {
        RSCFATAL(logger, "Catch-all exception matching event " << e
                << " for handler " << handler);
    }

    return false;

}

void ParallelEventReceivingStrategy::deliver(HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Delivering event " << e << " for subscription " << handler);

    try {

        handler->handle(e);

    } catch (const exception &ex) {
        // TODO probably disable this subscription
        RSCFATAL(logger, "Exception delivering event " << e
                << " to handler " << handler << ":" << ex.what());
    } catch (...) {
        RSCFATAL(logger, "Catch-all exception delivering event " << e
                << " to handler " << handler);
    }

}

void ParallelEventReceivingStrategy::handle(EventPtr event) {
    pool.push(event);
}

void ParallelEventReceivingStrategy::addHandler(HandlerPtr handler) {
    pool.registerReceiver(handler);
}

void ParallelEventReceivingStrategy::removeHandler(HandlerPtr handler) {
    pool.unregisterReceiver(handler);
}

void ParallelEventReceivingStrategy::addFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);
    filters.insert(filter);
}

void ParallelEventReceivingStrategy::removeFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);
    filters.erase(filter);
}

}
}
