/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *               2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Informer.h"

#include <boost/format.hpp>

#include "Scope.h"

using namespace std;

namespace rsb {

InformerBase::InformerBase(const vector<transport::OutConnectorPtr>& connectors,
                           const Scope&                              scope,
                           const ParticipantConfig&                  config,
                           const string&                             defaultType) :
    Participant(scope, config), defaultType(defaultType),
    configurator(new eventprocessing::OutRouteConfigurator(scope)),
    currentSequenceNumber(0) {
    // TODO evaluate configuration
    for (vector<transport::OutConnectorPtr>::const_iterator it =
            connectors.begin(); it != connectors.end(); ++it) {
        this->configurator->addConnector(*it);
    }

    this->configurator->activate();
}

InformerBase::~InformerBase() {
}

void InformerBase::printContents(ostream& stream) const {
    Participant::printContents(stream);
    stream << ", type = " << defaultType;
}

std::string InformerBase::getKind() const {
    return "informer";
}

string InformerBase::getType() const {
    return this->defaultType;
}

void InformerBase::setQualityOfSerivceSpecs(const QualityOfServiceSpec& specs) {
    configurator->setQualityOfServiceSpecs(specs);
}

EventPtr InformerBase::createEvent() const {
    EventPtr event(new Event());
    event->setScopePtr(getScope());
    return event;
}

EventPtr InformerBase::publish(VoidPtr data, const std::string& type) {
    EventPtr event = createEvent();
    event->setData(data);
    event->setType(type);
    checkedPublish(event);
    return event;
}

EventPtr InformerBase::uncheckedPublish(VoidPtr data, const std::string& type) {
    EventPtr event = createEvent();
    event->setData(data);
    event->setType(type);
    uncheckedPublish(event);
    return event;
}

EventPtr InformerBase::publish(EventPtr event) {
    checkedPublish(event);
    return event;
}

void InformerBase::checkedPublish(EventPtr event) {
    if (event->getType().empty()) {
        throw invalid_argument(
                boost::str(
                        boost::format("Event type cannot be empty: %1%")
                                % event));
    }
    // Check event type against informer's declared type.
    if (!getType().empty() && event->getType() != getType()) {
        throw invalid_argument(
                boost::str(
                        boost::format(
                                "Specified event type %1% does not match informer type %2%.")
                                % event->getType() % getType()));
    }
    // Check event scope against informer's declared scope.
    if (*event->getScopePtr() != *getScope() && !event->getScopePtr()->isSubScopeOf(
            *getScope())) {
        throw invalid_argument(
                boost::str(
                        boost::format(
                                "Specified event scope %1% does not match informer scope %2%.")
                                % event->getScopePtr() % getScope()));
    }

    this->uncheckedPublish(event);
}

void InformerBase::uncheckedPublish(EventPtr event) {
    event->setEventId(getId(), nextSequenceNumber());
    configurator->publish(event);
}

boost::uint32_t InformerBase::nextSequenceNumber() {
    boost::mutex::scoped_lock lock(this->sequenceNumberMutex);
    return this->currentSequenceNumber++;
}

}
