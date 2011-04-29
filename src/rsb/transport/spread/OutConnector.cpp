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

#include "OutConnector.h"

#include "../../protocol/ProtocolException.h"

using namespace std;

using namespace rsc::runtime;
using namespace rsc::logging;

using namespace rsb::protocol;

namespace rsb {
namespace spread {

OutConnector::OutConnector(const string& host, unsigned int port) :
    logger(Logger::getLogger("rsb.spread.OutConnector")),
    active(false),
    connector(new SpreadConnector(host, port)) {
}

rsb::transport::OutConnector* OutConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.spread.OutConnector");
    RSCDEBUG(logger, "creating OutConnector with properties " << args);

    return new OutConnector(args.get<string>      ("host", defaultHost()),
                            args.get<unsigned int>("port", defaultPort()));
}

OutConnector::~OutConnector() {
    if (this->active) {
        deactivate();
    }
}

void OutConnector::activate() {
    this->connector->activate();
}

void OutConnector::deactivate() {
    this->connector->deactivate();
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
    this->connector->setQualityOfServiceSpecs(specs);
}

void OutConnector::push(EventPtr e) {
    // TODO Remove "data split" information from notification
    // TODO Read max spread message len from config file
    // get matching converter
    string wire;
    int spreadMaxLen = 100000;

    boost::shared_ptr<void> obj = e->getData();
    // TODO exception handling if converter is not available
    boost::shared_ptr<converter::Converter<string> > c =
        this->connector->getConverters()->getConverterByDataType(e->getType());
    string wireType = c->serialize(make_pair(e->getType(), obj), wire);

    // ---- Begin split message implementation ----
    unsigned int numDataParts = 0;
    RSCDEBUG(logger, "Whole message size (data only): " << wire.size());
    numDataParts = wire.size() / spreadMaxLen;
    RSCDEBUG(logger, "Number of message parts (data only): " << numDataParts+1);

    string dataPart;
    size_t curPos = 0;
    size_t maxPartSize = 100000;
    for (unsigned int i = 0; i <= numDataParts; i++) {

        Notification n;
        n.set_eid(e->getId().getIdAsString());
        n.set_sequence_length(0);
        n.set_standalone(false);
        n.set_uri(e->getScope().toString());
        n.set_type_id(wireType);
        for (map<string, string>::const_iterator it = e->metaInfoBegin(); it
                 != e->metaInfoEnd(); ++it) {
            MetaInfo *info = n.mutable_metainfos()->Add();
            info->set_key(it->first);
            info->set_value(it->second);
        }
        n.set_num_data_parts(numDataParts);
        n.set_data_part(i);

        if (curPos < (wire.size() - (wire.size() % maxPartSize))) {
            dataPart = wire.substr(curPos, maxPartSize);
            curPos = (i * maxPartSize) + maxPartSize;
        } else {
            dataPart = wire.substr(curPos, wire.size() % maxPartSize);
        }

        n.mutable_data()->set_binary(dataPart);
        n.mutable_data()->set_length(dataPart.size());
        RSCTRACE(logger, "Size of message[" << i << "] (data only): " << dataPart.size());

        string sm;
        if (!n.SerializeToString(&sm)) {
            throw ProtocolException("Failed to write notification to stream");
        }

        SpreadMessage msg(sm);

        // TODO convert URI to group name
        // TODO check if it is necessary to join spread groups when only sending to them

        msg.addGroup(e->getScope().toString());
        msg.setQOS(this->connector->getMessageQoS());

        RSCTRACE(logger, "This is the serialized message size before send: " << msg.getSize());

        if (!this->connector->send(msg)) {
            //        for (list<string>::const_iterator n = msg->getGroupsBegin(); n != msg->getGroupsEnd(); ++n) {
            //           cout << "Sending msg to following groups: " << *n << endl;
            //        }
            // TODO implement queing or throw messages away?
            // TODO maybe return exception with msg that was not sent
            RSCWARN(logger, "Spread Connection inactive -> could not send message");
        } else {
            RSCDEBUG(logger, "event sent to spread");
        }
    }
}

}
}
