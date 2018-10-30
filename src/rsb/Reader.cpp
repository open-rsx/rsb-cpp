/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Reader.h"

#include <boost/bind.hpp>

#include <rsc/threading/SynchronizedQueue.h>

#include "Handler.h"

#include "eventprocessing/EventReceivingStrategy.h"
#include "eventprocessing/InRouteConfigurator.h"

using namespace std;

using namespace boost;

using namespace rsb::eventprocessing;
using namespace rsb::transport;

namespace rsb {

Reader::Reader(const vector<InConnectorPtr>& connectors,
               const Scope&                  scope,
               const ParticipantConfig&      config) :
    Participant(scope, config) {
    initialize(connectors, scope, config);
}

std::string Reader::getKind() const {
    return "reader";
}

const std::set<std::string> Reader::getTransportURLs() const {
    return this->configurator->getTransportURLs();
}

void Reader::initialize(const std::vector<InConnectorPtr>& connectors,
                        const Scope&                       scope,
                        const ParticipantConfig&           config) {
    this->configurator.reset(new eventprocessing::InRouteConfigurator(scope, config));
    this->configurator->setErrorStrategy(getConfig().getErrorStrategy());
    for (std::vector<InConnectorPtr>::const_iterator it = connectors.begin(); it
           != connectors.end(); ++it) {
      this->configurator->addConnector(*it);
    }

    this->configurator->setQualityOfServiceSpecs(config.getQualityOfServiceSpec());

    this->configurator->activate();

    this->configurator->handlerAdded(
            HandlerPtr(new EventFunctionHandler(boost::bind(&Reader::handle, this, _1))),
            true);
}


EventPtr Reader::read(bool block) {
    if (block) {
        return this->queue.pop();
    } else {
        try {
            return this->queue.tryPop();
        } catch (const rsc::threading::QueueEmptyException&) {
            return EventPtr();
        }
    }
}

void Reader::handle(EventPtr event) {
    this->queue.push(event);
}

}
