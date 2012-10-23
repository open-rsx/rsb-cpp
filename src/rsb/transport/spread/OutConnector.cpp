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

#include "OutConnector.h"

#include <rsc/misc/langutils.h>

#include "../../MetaData.h"
#include "../../EventId.h"
#include "../../Scope.h"

#include "../../protocol/ProtocolException.h"
#include "../../protocol/FragmentedNotification.h"


using namespace std;

using namespace rsc::runtime;
using namespace rsc::logging;

using namespace rsb::protocol;
using namespace rsb::converter;

namespace rsb {
namespace transport {
namespace spread {

transport::OutConnector* OutConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.transport.spread.OutConnector");
    RSCDEBUG(logger, "creating OutConnector with properties " << args);

    return new OutConnector(
            args.get<ConverterSelectionStrategyPtr>("converters"),
            args.get<string>("host", defaultHost()),
            args.getAs<unsigned int>("port", defaultPort()),
            args.getAs<unsigned int>("maxfragmentsize", 100000));
}

OutConnector::OutConnector(ConverterSelectionStrategyPtr converters,
        const string& host, unsigned int port, unsigned int maxFragmentSize) :
        transport::ConverterSelectingConnector<string>(converters), logger(
                Logger::getLogger("rsb.transport.spread.OutConnector")), active(false), connector(
                new SpreadConnector(host, port)), maxFragmentSize(
                maxFragmentSize), minDataSpace(5) {
}

OutConnector::~OutConnector() {
    if (this->active) {
        deactivate();
    }
}

void OutConnector::printContents(ostream& stream) const {
    stream << "connector = " << connector << ", active = " << active;
}

void OutConnector::setScope(const Scope& /*scope*/) {
}

void OutConnector::activate() {
    this->connector->activate();
    this->active = true;
}

void OutConnector::deactivate() {
    this->connector->deactivate();
    this->active = false;
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& specs) {
    this->connector->setQualityOfServiceSpecs(specs);
}

void OutConnector::handle(EventPtr event) {

    // TODO exception handling if converter is not available
    ConverterPtr c = getConverter(event->getType());
    string wire;
    string wireSchema = c->serialize(
            make_pair(event->getType(), event->getData()), wire);

    event->mutableMetaData().setSendTime(rsc::misc::currentTimeMicros());

    // create a list of all fragments required to send this event
    vector<FragmentedNotificationPtr> fragments;

    size_t curPos = 0;
    unsigned int currentDataPart = 0;
    // "currentDataPart == 0" is required for the case when wire.size() == 0
    // This can happen, for example, with the "void" wire-schema.
    while (curPos < wire.size() || currentDataPart == 0) {

        FragmentedNotificationPtr notification(new FragmentedNotification);
        fillNotificationId(*(notification->mutable_notification()),
                event);

        // when sending the first time, we need to transmit all meta data.
        if (currentDataPart == 0) {
            fillNotificationHeader(*(notification->mutable_notification()),
                    event, wireSchema);
        }
        // Scale the data for this message with the size of the generated header
        // and mandatory id fields in the notification
        unsigned int headerByteSize = notification->ByteSize();
        assert(headerByteSize <= maxFragmentSize - minDataSpace);
        if (headerByteSize >= maxFragmentSize - minDataSpace) {
            throw ProtocolException(
                    "The meta data of this event are too big for spread!");
        }
        unsigned int maxDataPartSize = maxFragmentSize - headerByteSize;

        // finally set the data information
        string dataPart = wire.substr(curPos, maxDataPartSize);
        curPos += maxDataPartSize;

        notification->mutable_notification()->set_data(dataPart);
        notification->set_data_part(currentDataPart);
        // optimistic guess for the number of required fragments
        notification->set_num_data_parts(1);

        fragments.push_back(notification);

        ++currentDataPart;

    }

    // adapt num_data_parts field of each FragmentedNotification if we need more
    // than one fragment
    assert(!fragments.empty());
    if (fragments.size() > 1) {
        for (vector<FragmentedNotificationPtr>::iterator fragmentIt =
                fragments.begin(); fragmentIt != fragments.end();
                ++fragmentIt) {
            (*fragmentIt)->set_num_data_parts(fragments.size());
        }
    }

    // finally send all fragments

    for (vector<FragmentedNotificationPtr>::const_iterator fragmentIt =
            fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt) {

        // serialize to spread message
        string serializedMessageData;
        if (!(*fragmentIt)->SerializeToString(&serializedMessageData)) {
            throw ProtocolException("Failed to write notification to stream");
        }
        SpreadMessage spreadMessage(serializedMessageData);

        // send message to appropriate groups
        const vector<string>& groupNames = connector->makeGroupNames(
                *event->getScopePtr());
        for (vector<string>::const_iterator groupIt = groupNames.begin();
                groupIt != groupNames.end(); ++groupIt) {
            spreadMessage.addGroup(*groupIt);
        }
        spreadMessage.setQOS(this->connector->getMessageQoS());

        RSCTRACE(
                logger,
                "This is the serialized message size before send: " << spreadMessage.getSize());

        this->connector->send(spreadMessage);
        // TODO implement queuing or throw messages away?
        // TODO maybe return exception with msg that was not sent
        // TODO especially important to fulfill QoS specs
        RSCDEBUG(logger, "event sent to spread");

    }

}

}
}
}
