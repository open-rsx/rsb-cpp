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

#include "Bus.h"

#include <rsc/runtime/ContainerIO.h>

using namespace std;

using namespace rsc::logging;

using namespace rsb::transport;

namespace rsb {
namespace transport{
namespace inprocess {

Bus::Bus() :
    logger(Logger::getLogger("rsb.transport.inprocess.Bus")), singleThreaded(false) {
}

Bus::~Bus() {
    if (!this->sinks.empty()) {
        RSCWARN(
                logger,
                "" << this->sinks.size()
                        << " non-empty scopes when destructing: " << this->sinks);
    }
}

/*void Bus::printContents(ostream& stream) const {
    stream << "sinks = " << this->sinks;
    }*/

void Bus::addSink(InConnectorPtr sink) {
    boost::recursive_mutex::scoped_lock lock(this->mutex);

    RSCDEBUG(logger, "Adding sink " << sink);

    SinkMap::iterator it = this->sinks.find(sink->getScope());
    if (it == this->sinks.end()) {
        RSCDEBUG(logger,
                "No entry in sink map for event scope " << sink->getScope());

        set<boost::weak_ptr<InConnector> > connectors;
        for (SinkMap::iterator it_ = this->sinks.begin(); it_
                != this->sinks.end(); ++it_) {
            RSCDEBUG(
                    logger,
                    "Adding " << it_->second.size() << " connectors from "
                            << it_->first);

            if (it_->first == sink->getScope() || it_->first.isSuperScopeOf(
                    sink->getScope())) {
                copy(it_->second.begin(), it_->second.end(),
                        inserter(connectors, connectors.begin()));
            }
        }
        copy(connectors.begin(), connectors.end(),
                back_inserter(this->sinks[sink->getScope()]));

        RSCDEBUG(
                logger,
                "Created entry in sink map for scope " << sink->getScope()
                        << " with " << connectors.size() << " connectors");

        it = this->sinks.find(sink->getScope());
    }
    it->second.push_back(sink);

    for (SinkMap::iterator it = this->sinks.begin(); it != this->sinks.end(); ++it) {
        if (it->first.isSubScopeOf(sink->getScope())) {
            SinkList& connectors = it->second;
            connectors.push_back(sink);
        }
    }
}

void Bus::removeSink(InConnector* sink) {
    boost::recursive_mutex::scoped_lock lock(this->mutex);

    vector<Scope> scopes = sink->getScope().superScopes(true);
    RSCDEBUG(logger, "Removing sink " << sink);

    for (SinkMap::iterator it = this->sinks.begin(); it != this->sinks.end(); ++it) {
        SinkList& connectors = it->second;
        RSCDEBUG(
                logger,
                "Scope " << it->first << " has " << connectors.size()
                        << " connectors");

        for (SinkList::iterator it_ = connectors.begin(); it_
                != connectors.end(); ++it_) {
            // If the weak pointer is dangling, we found our
            // sink. Otherwise, we can just check the pointer.
            InConnectorPtr ptr = it_->lock();
            if (!ptr || (ptr.get() == sink)) {
                RSCDEBUG(logger,
                        "Found connector " << sink << " in scope " << it->first);
                it_ = connectors.erase(it_);
                break;
            }
        }

        RSCDEBUG(
                logger,
                "Scope " << it->first << " has " << connectors.size()
                        << " connectors");
        if (connectors.empty()) {
            RSCDEBUG(logger, "Removing empty scope " << it->first);
            //this->sinks.erase(it);
        }
    }
}

void Bus::handle(EventPtr event) {
    //    RSCDEBUG(logger, "Delivering event " << event);

    if (singleThreaded) {
        this->handleNoLock(event);
    } else {
        boost::recursive_mutex::scoped_lock lock(this->mutex);
        this->handleNoLock(event);
    }

}

void Bus::handleNoLock(EventPtr event) {

    SinkMap::const_iterator it = this->sinks.find(*event->getScopePtr());
    if (it == this->sinks.end()) {
        RSCDEBUG(logger,
                "No entry in sink map for event scope " << *event->getScopePtr());

        set<boost::weak_ptr<InConnector> > connectors;
        for (SinkMap::iterator it_ = this->sinks.begin(); it_
                != this->sinks.end(); ++it_) {
            RSCDEBUG(
                    logger,
                    "Adding " << it_->second.size() << " connectors from "
                            << it_->first);

            if (it_->first == *event->getScopePtr() || it_->first.isSuperScopeOf(
                    *event->getScopePtr())) {
                copy(it_->second.begin(), it_->second.end(),
                        inserter(connectors, connectors.begin()));
            }
        }
        copy(connectors.begin(), connectors.end(),
                back_inserter(this->sinks[*event->getScopePtr()]));

        RSCDEBUG(
                logger,
                "Created entry in sink map for scope " << *event->getScopePtr()
                        << " with " << connectors.size() << " connectors");

        it = this->sinks.find(*event->getScopePtr());
    }

    const SinkList& connectors = it->second;
    for (SinkList::const_iterator it__ = connectors.begin(); it__
            != connectors.end(); ++it__) {
        InConnectorPtr connector = it__->lock();
        if (connector) {
            // RSCDEBUG(logger, "Delivering to " << connector << " in " << *it);
            connector->handle(event);
        }
    }
}

}
}
}
