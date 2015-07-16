/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "InformerTask.h"

#include <stdlib.h>
#include <time.h>

#include <rsc/misc/langutils.h>
#include <rsc/runtime/TypeStringTools.h>

using namespace std;
using namespace rsb;
using namespace rsb::transport;
using namespace rsc::threading;

namespace rsb {
namespace test {

InformerTask::InformerTask(OutConnectorPtr p, const Scope& scope,
        const unsigned int& numEvents, const unsigned int& dataSizeInBytes) :
        scope(scope), numEvents(numEvents), dataSizeInBytes(dataSizeInBytes), sentEvents(
                0), connector(p), data(
                new string(rsc::misc::randAlnumStr(dataSizeInBytes))) {
}

InformerTask::~InformerTask() {
}

void InformerTask::execute() {

    ++sentEvents;
    //cout << "sending event: " << sentEvents << endl;
    Scope thisScope = scope;
    if (sentEvents % 2 == 0) {
        // should be filtered
        thisScope = Scope("/other").concat(scope);
    }
    EventPtr p(new Event(thisScope, data, rsc::runtime::typeName<string>()));
    p->setId(id, sentEvents);

    // add causing events
    unsigned int numCauses = rand() % 10;
    for (unsigned int i = 0; i < numCauses; ++i) {
        p->addCause(EventId(rsc::misc::UUID(), rand()));
    }

    connector->handle(p);
    if (sentEvents % 2 == 1) {
        events.push_back(p);
    }
    if (sentEvents == 2 * numEvents) {
        //cout << endl;
        cancel();
    }

}

std::vector<EventPtr> InformerTask::getEvents() {
    return events;
}

// ------

WaitingObserver::WaitingObserver(const unsigned int& desiredEvents,
        const Scope& scope) :
        desiredEvents(desiredEvents), scope(scope), receivedEvents(0) {

}

void WaitingObserver::handler(EventPtr e) {
    boost::recursive_mutex::scoped_lock lock(m);
    ++receivedEvents;
    events.push_back(e);
    //cout << "WaitingObserver #" << receivedEvents << "/" << desiredEvents
    //        << " received on " << scope << ". Metadata: " << *e << endl;
    if (receivedEvents == desiredEvents) {
        condition.notify_all();
    }
}

vector<EventPtr> WaitingObserver::getEvents() {
    boost::recursive_mutex::scoped_lock lock(m);
    return events;
}

bool WaitingObserver::waitReceived(const unsigned int& timeoutMs) {
    boost::recursive_mutex::scoped_lock lock(m);
    while (receivedEvents < desiredEvents) {
        if (timeoutMs == 0) {
            condition.wait(lock);
        } else {
            bool normalWakeup = condition.timed_wait(lock,
                    boost::posix_time::milliseconds(timeoutMs));
            if (!normalWakeup) {
                return false;
            }
        }
    }
    return true;
}

Scope WaitingObserver::getScope() const {
    return scope;
}

}
}
