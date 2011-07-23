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

#include "Assembly.h"

#include <boost/format.hpp>
#include <boost/date_time/microsec_time_clock.hpp>

using namespace std;

using namespace boost;
using namespace boost::posix_time;

using namespace rsc::logging;
using namespace rsc::threading;

using namespace rsb::protocol;

namespace rsb {
namespace spread {

Assembly::Assembly(rsb::protocol::NotificationPtr n) :
    logger(Logger::getLogger(str(format("rsb.spread.Assembly[%1%]")
                                 % n->sequence_number()))),
    receivedParts(0), birthTime(microsec_clock::local_time()) {
    store.resize(n->num_data_parts());
    add(n);
}

Assembly::~Assembly() {
}

string Assembly::getData(const unsigned int &i) const {
    return this->store[i]->data();
}

string *Assembly::getCompleteData() const {
    RSCTRACE(logger, "Joining data parts");
    assert(isComplete());
    // Concatenate data parts
    string* result = new string(getData(0));
    for (unsigned int i = 1; i < this->store.size(); ++i) {
        result->append(getData(i));
    }
    return result;
}

unsigned int Assembly::add(NotificationPtr n) {
    RSCTRACE(logger, "Adding notification " << n->sequence_number()
            << " (part " << n->data_part() << "/" << this->store.size()
            << ") to assembly");
    store[n->data_part()] = n;
    return receivedParts++;
}

bool Assembly::isComplete() const {
    return this->receivedParts == this->store.size();
}

unsigned int Assembly::age() const {
    return (microsec_clock::local_time() - this->birthTime).total_seconds();
}

AssemblyPool::PruningTask::PruningTask(Pool &pool,
        boost::recursive_mutex &poolMutex, const unsigned &ageS,
        const unsigned int &pruningIntervalMs) :
    PeriodicTask(pruningIntervalMs), logger(Logger::getLogger(
            "rsb.spread.AssemblyPool.PruningTask")), pool(pool), poolMutex(
            poolMutex), maxAge(ageS) {
}

void AssemblyPool::PruningTask::execute() {
    boost::recursive_mutex::scoped_lock lock(this->poolMutex);

    RSCDEBUG(logger, "Scanning for old assemblies");
    Pool::iterator it = this->pool.begin();
    while (it != this->pool.end()) {
        if (it->second->age() > maxAge) {
            RSCDEBUG(logger, "Pruning old assembly " << it->second);
            this->pool.erase(it++);
        } else {
            ++it;
        }
    }

}

AssemblyPool::AssemblyPool(const unsigned int &ageS,
        const unsigned int &pruningIntervalMs) :
    logger(Logger::getLogger("rsb.spread.AssemblyPool")), pruningAgeS(ageS),
            pruningIntervalMs(pruningIntervalMs) {
    if (ageS == 0) {
        throw domain_error("Age must not be 0.");
    }
    if (pruningIntervalMs == 0) {
        throw domain_error("Pruning interval must not be 0");
    }
}

AssemblyPool::~AssemblyPool() {
    setPruning(false);
}

bool AssemblyPool::isPruning() const {
    boost::recursive_mutex::scoped_lock lock(pruningMutex);
    return pruningTask;
}

void AssemblyPool::setPruning(const bool &prune) {
    boost::recursive_mutex::scoped_lock lock(pruningMutex);

    if (!isPruning() && prune) {
        RSCDEBUG(logger, "Starting Assembly pruning");
        pruningTask.reset(new PruningTask(this->pool, this->poolMutex,
                pruningAgeS, pruningIntervalMs));
        this->executor.schedule(this->pruningTask);
    } else if (isPruning() && !prune) {
        RSCDEBUG(logger, "Stopping Assembly pruning");
        assert(pruningTask);
        pruningTask->cancel();
        pruningTask->waitDone();
        RSCDEBUG(logger, "Assembly pruning stopped");
    }

}

shared_ptr<string> AssemblyPool::add(NotificationPtr notification) {
    boost::recursive_mutex::scoped_lock lock(this->poolMutex);

    string key = notification->sender_id();
    key.push_back(notification->sequence_number() & 0x000000ff);
    key.push_back(notification->sequence_number() & 0x0000ff00);
    key.push_back(notification->sequence_number() & 0x00ff0000);
    key.push_back(notification->sequence_number() & 0xff000000);
    Pool::iterator it = this->pool.find(key);
    string *result = 0;
    AssemblyPtr assembly;
    if (it != this->pool.end()) {
        // Push message to existing Assembly
        assembly = it->second;
        RSCTRACE(logger, "Adding notification " << notification->sequence_number() << " to existing assembly "
                << assembly);
        assembly->add(notification);
    } else {
        // Create new Assembly
        RSCTRACE(logger, "Creating new assembly for notification " << notification->sequence_number());
        assembly.reset(new Assembly(notification));
        it = this->pool.insert(make_pair(key, assembly)).first;
    }

    if (assembly->isComplete()) {
        result = assembly->getCompleteData();
        this->pool.erase(it);
    }

    RSCTRACE(logger, "dataPool size: " << this->pool.size());

    return shared_ptr<string> (result);
}

}
}
