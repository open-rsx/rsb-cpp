/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include "Assembly.h"

#include <boost/format.hpp>
#include <boost/date_time/microsec_time_clock.hpp>

#include "../../protocol/ProtocolException.h"

using namespace std;

using namespace boost;
using namespace boost::posix_time;

using namespace rsc::logging;
using namespace rsc::threading;

using namespace rsb::protocol;

namespace rsb {
namespace transport {
namespace spread {

Assembly::Assembly(rsb::protocol::FragmentedNotificationPtr n) :
        logger(
                Logger::getLogger(
                        str(
                                format("rsb.transport.spread.Assembly[%1%]")
                                        % n->notification().event_id().sequence_number()))), receivedParts(
                0), birthTime(microsec_clock::local_time()) {
    store.resize(n->num_data_parts());
    add(n);
}

Assembly::~Assembly() {
}

NotificationPtr Assembly::getCompleteNotification() const {
    RSCTRACE(logger, "Joining fragments");
    assert(isComplete());

    NotificationPtr notification(
            store[0]->mutable_notification(),
            rsc::misc::ParentSharedPtrDeleter
                    < rsb::protocol::FragmentedNotification > (store[0]));

    // Concatenate data parts
    string* resultData = notification->mutable_data();
    for (unsigned int i = 1; i < this->store.size(); ++i) {
        resultData->append(store[i]->notification().data());
    }
    return notification;
}

bool Assembly::add(FragmentedNotificationPtr n) {
    RSCTRACE(
            logger,
            "Adding notification " << n->notification().event_id().sequence_number() << " (part " << n->data_part() << "/" << this->store.size() << ") to assembly");
    assert(n->num_data_parts() == store.size());
    //assert(!store[n->data_part()]);
    if (store[n->data_part()]) {
        throw ProtocolException(
                boost::str(
                        boost::format(
                                "Received fragment (%d/%d) of notification for event with sender id %x and sequence number %d twice!.")
                                % n->data_part() % n->num_data_parts()
                                % n->notification().event_id().sender_id()
                                % n->notification().event_id().sequence_number()));
    }
    store[n->data_part()] = n;
    ++receivedParts;
    return isComplete();
}

bool Assembly::isComplete() const {
    return this->receivedParts == this->store.size();
}

unsigned int Assembly::age() const {
    return (microsec_clock::local_time() - this->birthTime).total_seconds();
}

AssemblyPool::PruningTask::PruningTask(Pool& pool,
        boost::recursive_mutex& poolMutex, const unsigned& ageS,
        const unsigned int& pruningIntervalMs) :
        PeriodicTask(pruningIntervalMs), logger(
                Logger::getLogger("rsb.transport.spread.AssemblyPool.PruningTask")), pool(
                pool), poolMutex(poolMutex), maxAge(ageS) {
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

AssemblyPool::AssemblyPool(const unsigned int& ageS,
        const unsigned int& pruningIntervalMs) :
        logger(Logger::getLogger("rsb.transport.spread.AssemblyPool")), pruningAgeS(ageS), pruningIntervalMs(
                pruningIntervalMs) {
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

void AssemblyPool::setPruning(const bool& prune) {
    boost::recursive_mutex::scoped_lock lock(pruningMutex);

    if (!isPruning() && prune) {
        RSCDEBUG(logger, "Starting Assembly pruning");
        pruningTask.reset(
                new PruningTask(this->pool, this->poolMutex, pruningAgeS,
                        pruningIntervalMs));
        this->executor.schedule(this->pruningTask);
    } else if (isPruning() && !prune) {
        RSCDEBUG(logger, "Stopping Assembly pruning");
        assert(pruningTask);
        pruningTask->cancel();
        pruningTask->waitDone();
        RSCDEBUG(logger, "Assembly pruning stopped");
    }

}

rsb::protocol::NotificationPtr AssemblyPool::add(
        FragmentedNotificationPtr notification) {
    boost::recursive_mutex::scoped_lock lock(this->poolMutex);

    string key = notification->notification().event_id().sender_id();
    key.push_back(
            notification->notification().event_id().sequence_number()
                    & 0x000000ff);
    key.push_back(
            notification->notification().event_id().sequence_number()
                    & 0x0000ff00);
    key.push_back(
            notification->notification().event_id().sequence_number()
                    & 0x00ff0000);
    key.push_back(
            notification->notification().event_id().sequence_number()
                    & 0xff000000);
    Pool::iterator it = this->pool.find(key);
    NotificationPtr result;
    AssemblyPtr assembly;
    if (it != this->pool.end()) {
        // Push message to existing Assembly
        assembly = it->second;
        RSCTRACE(
                logger,
                "Adding notification " << notification->notification().event_id().sequence_number() << " to existing assembly " << assembly);
        assembly->add(notification);
    } else {
        // Create new Assembly
        RSCTRACE(
                logger,
                "Creating new assembly for notification " << notification->notification().event_id().sequence_number());
        assembly.reset(new Assembly(notification));
        it = this->pool.insert(make_pair(key, assembly)).first;
    }

    if (assembly->isComplete()) {
        result = assembly->getCompleteNotification();
        this->pool.erase(it);
    }

    RSCTRACE(logger, "dataPool size: " << this->pool.size());

    return result;

}

}
}
}
