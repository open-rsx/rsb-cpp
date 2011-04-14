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

#include <iostream>

#include <string.h>
#include <math.h>

#include <rsc/misc/Registry.h>
#include <rsc/threading/ThreadedTaskExecutor.h>

#include "../../protocol/Notification.h"
#include "SpreadConnector.h"
#include "SpreadConnection.h"
#include "../../util/Configuration.h"
#include "../../CommException.h"
#include "../Converter.h"
#include "../../protocol/ProtocolException.h"
#include "../../UnsupportedQualityOfServiceException.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::util;
using namespace rsb::transport;
using namespace rsb::protocol;
using namespace rsc::threading;

namespace rsb {
namespace spread {

const SpreadConnector::QoSMap SpreadConnector::qosMapping = SpreadConnector::buildQoSMapping();

SpreadConnector::SpreadConnector(
		rsb::transport::ConverterCollection<std::string>::Ptr converters) :
	converters(converters) {
	init();
}

SpreadConnector::SpreadConnector() :
	converters(stringConverterCollection()) {
	init();
}

void SpreadConnector::init() {
	logger = rsc::logging::Logger::getLogger("rsb.spread.SpreadConnector");
	RSCDEBUG(logger, "SpreadConnector() entered, port id: " << id.getIdAsString());
	shutdown = false;
	exec = TaskExecutorPtr(new ThreadedTaskExecutor);
	// TODO ConnectionPool for SpreadConnections?!?
	// TODO Send Message over Managing / Introspection Channel
	// TODO Generate Unique-IDs for SpreadConnectors
	con = SpreadConnectionPtr(new SpreadConnection(id.getIdAsString()));
	// TODO check if it makes sense and is possible to provide a weak_ptr to the ctr of StatusTask
	//st = boost::shared_ptr<StatusTask>(new StatusTask(this));
	rec = boost::shared_ptr<ReceiverTask>(
			new ReceiverTask(con, converters, observer));
	memberships = MembershipManagerPtr(new MembershipManager());
	setQualityOfServiceSpecs(QualityOfServiceSpec());
}

void SpreadConnector::activate() {
	// connect to spread
	con->activate();
	// (re-)start threads
	exec->schedule(rec);
	//exec->schedule(st);
}

void SpreadConnector::setObserver(Action a) {
	observer = a;
	rec->setAction(a);
}

void SpreadConnector::removeObserver(Action /*a*/) {
	observer.clear();
	rec->setAction(Action());
}

void SpreadConnector::deactivate() {
	shutdown = true;
	RSCDEBUG(logger, "deactivate() entered"); // << *id);
	rec->cancel();
	// killing spread connection, exception thrown to rec thread which
	// should be handled specifically as the cancel flag was set
	// memberships->leaveAll();
	con->deactivate();
	RSCDEBUG(logger, "deactivate() stopping qad task object");
	//	cout << "stopping st task" << endl;
	//	st->cancel();
	RSCTRACE(logger, "deactivate() finished"); // << *id);
}

SpreadConnector::~SpreadConnector() {
	if (!shutdown) {
		deactivate();
	}
}

void SpreadConnector::notify(rsb::filter::ScopeFilter* f,
		const rsb::filter::FilterAction::Types &at) {
	// join or leave groups
	// TODO evaluate success
	RSCDEBUG(logger, "notify(rsb::filter::ScopeFilter*, ...) entered"); // << *id);
	switch (at) {
	case rsb::filter::FilterAction::ADD:
		RSCINFO(logger, "ScopeFilter URI is " << f->getURI()
				<< " ,now going to join Spread group")
		;
		memberships->join(f->getURI(), con);
		break;
	case rsb::filter::FilterAction::REMOVE:
		RSCINFO(logger, "ScopeFilter URI is " << f->getURI()
				<< " ,now going to leave Spread group")
		;
		memberships->leave(f->getURI(), con);
		break;
	default:
		RSCWARN(logger,
				"ScopeFilter Action not supported by this Connector implementation")
		;
		break;
	}

}

void SpreadConnector::push(EventPtr e) {
	// TODO Remove "data split" information from notification
	// TODO Read max spread message len from config file
	// get matching converter
	string wire;
	int spreadMaxLen = 100000;

	boost::shared_ptr<void> obj = e->getData();
	// TODO exception handling if converter is not available
	boost::shared_ptr<Converter<string> > c =
			converters->getConverterByDataType(e->getType());
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
		n.set_eid(e->getUUID().getIdAsString());
		n.set_sequence_length(0);
		n.set_standalone(false);
		n.set_uri(e->getURI());
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

		msg.addGroup(e->getURI());
		msg.setQOS(messageQoS);

		RSCTRACE(logger, "This is the serialized message size before send: " << msg.getSize());

		if (!con->send(msg)) {
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

SpreadConnector::QoSMap SpreadConnector::buildQoSMapping() {

	map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> unorderedMap;
	unorderedMap.insert(
			make_pair(QualityOfServiceSpec::UNRELIABLE,
					SpreadMessage::UNRELIABLE));
	unorderedMap.insert(
			make_pair(QualityOfServiceSpec::RELIABLE, SpreadMessage::RELIABLE));

	map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> orderedMap;
	orderedMap.insert(
			make_pair(QualityOfServiceSpec::UNRELIABLE, SpreadMessage::FIFO));
	orderedMap.insert(
			make_pair(QualityOfServiceSpec::RELIABLE, SpreadMessage::FIFO));

	map<QualityOfServiceSpec::Ordering, map<QualityOfServiceSpec::Reliability,
			SpreadMessage::QOS> > table;
	table.insert(make_pair(QualityOfServiceSpec::UNORDERED, unorderedMap));
	table.insert(make_pair(QualityOfServiceSpec::ORDERED, orderedMap));

	return table;

}

void SpreadConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {

	QoSMap::const_iterator orderMapIt = qosMapping.find(specs.getOrdering());
	if (orderMapIt == qosMapping.end()) {
		throw UnsupportedQualityOfServiceException("Unknown ordering", specs);
	}
	map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS>::const_iterator
			mapIt = orderMapIt->second.find(specs.getReliability());
	if (mapIt == orderMapIt->second.end()) {
		throw UnsupportedQualityOfServiceException("Unknown reliability", specs);
	}

	messageQoS = mapIt->second;

	RSCDEBUG(logger, "Selected new message type " << messageQoS);

}

}
}
