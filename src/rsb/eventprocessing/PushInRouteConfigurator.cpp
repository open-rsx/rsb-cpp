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

#include "PushInRouteConfigurator.h"

#include <boost/bind.hpp>

#include "EventReceivingStrategyFactory.h"

using namespace std;

using namespace boost;

using namespace rsc::runtime;
using namespace rsc::logging;

using namespace rsb::transport;

namespace rsb {
namespace eventprocessing {

PushInRouteConfigurator::PushInRouteConfigurator(const Scope&             scope,
                                                 const ParticipantConfig& config) :
    InRouteConfigurator(scope, config),
    logger(Logger::getLogger("rsb.eventprocessing.PushInRouteConfigurator")),
    errorStrategy(ParticipantConfig::ERROR_STRATEGY_LOG) {
}

PushInRouteConfigurator::~PushInRouteConfigurator() {
}

string PushInRouteConfigurator::getClassName() const {
    return "PushInRouteConfigurator";
}

void PushInRouteConfigurator::printContents(ostream& stream) const {
    InRouteConfigurator::printContents(stream);
    stream << ", errorStrategy = " << this->errorStrategy;
}

void PushInRouteConfigurator::activate() {
    InRouteConfigurator::activate();

    // The base class has instantiated an
    // EventReceivingStrategy. Retrieve it and install our error
    // handling strategy.
    this->eventReceivingStrategy
        = dynamic_pointer_cast<PushEventReceivingStrategy>(getEventReceivingStrategy());
    this->eventReceivingStrategy->setHandlerErrorStrategy(this->errorStrategy);

    // Retrieve the set of connectors and ourselves to the list of
    // handlers of each connector.
    InRouteConfigurator::ConnectorSet connectors = getConnectors();
    for (InRouteConfigurator::ConnectorSet::const_iterator it =
            connectors.begin(); it != connectors.end(); ++it) {
        (*it)->setErrorStrategy(this->errorStrategy);
        (*it)->addHandler(
                HandlerPtr(
                        new EventFunctionHandler(
                                boost::bind(&PushEventReceivingStrategy::handle,
                                        this->eventReceivingStrategy, _1))));
    }
}

void PushInRouteConfigurator::handlerAdded(rsb::HandlerPtr handler, const bool& wait) {
    this->eventReceivingStrategy->addHandler(handler, wait);
}

void PushInRouteConfigurator::handlerRemoved(rsb::HandlerPtr handler, const bool& wait) {
    this->eventReceivingStrategy->removeHandler(handler, wait);
}

void PushInRouteConfigurator::setErrorStrategy(const ParticipantConfig::ErrorStrategy& strategy) {
    if (this->eventReceivingStrategy) {
        this->eventReceivingStrategy->setHandlerErrorStrategy(strategy);
    }
    InRouteConfigurator::ConnectorSet connectors = getConnectors();
    for (InRouteConfigurator::ConnectorSet::const_iterator it =
            connectors.begin(); it != connectors.end(); ++it) {
        (*it)->setErrorStrategy(this->errorStrategy);
    }
    this->errorStrategy = strategy;
}

EventReceivingStrategyPtr PushInRouteConfigurator::createEventReceivingStrategy() {
    string     impl    = getReceivingStrategyConfig().getName();
    Properties options = getReceivingStrategyConfig().getOptions();
    RSCDEBUG(logger, "Instantiating event receiving strategy with config "
             << getReceivingStrategyConfig());
    return EventReceivingStrategyPtr(getEventReceivingStrategyFactory().createInst(impl, options));
}

}
}
