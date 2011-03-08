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

#include <rsc/misc/Registry.h>
#include <rsc/threading/ThreadedTaskExecutor.h>

#include "../../protocol/Notification.h"
#include "SpreadPort.h"
#include "SpreadConnection.h"
#include "../../util/Configuration.h"
#include "../../CommException.h"
#include "../AbstractConverter.h"
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

const SpreadPort::QoSMap SpreadPort::qosMapping = SpreadPort::buildQoSMapping();

SpreadPort::SpreadPort(
		rsb::transport::ConverterCollection<std::string>::Ptr converters) :
	converters(converters) {
	init();
}

SpreadPort::SpreadPort() :
	converters(stringConverterCollection()) {
	init();
}

void SpreadPort::init() {
	logger = rsc::logging::Logger::getLogger("rsb.spread.SpreadPort");
	RSCDEBUG(logger, "SpreadPort() entered, port id: " << id.getIdAsString());
	shutdown = false;
	exec = TaskExecutorPtr(new ThreadedTaskExecutor);
	// TODO ConnectionPool for SpreadConnections?!?
	// TODO Send Message over Managing / Introspection Channel
	// TODO Generate Unique-IDs for SpreadPorts
	con = SpreadConnectionPtr(new SpreadConnection(id.getIdAsString()));
	// TODO check if it makes sense and is possible to provide a weak_ptr to the ctr of StatusTask
	//st = boost::shared_ptr<StatusTask>(new StatusTask(this));
	qad = boost::shared_ptr<QueueAndDispatchTask<RSBEventPtr> >(
			new QueueAndDispatchTask<RSBEventPtr> ());
	rec = boost::shared_ptr<ReceiverTask>(
			new ReceiverTask(con, converters, qad));
	memberships = MembershipManagerPtr(new MembershipManager());
	setQualityOfServiceSpecs(QualityOfServiceSpec());
}

void SpreadPort::activate() {
	// connect to spread
	con->activate();
	// (re-)start threads
	exec->schedule(rec);
	exec->schedule(qad);
	//exec->schedule(st);
}

// TODO think about extracting qad functions into QADPort?!?
void SpreadPort::setObserver(Action a) {
	qad->setObserver(a);
}

void SpreadPort::removeObserver(Action /*a*/) {
	qad->setObserver(NULL);
}

void SpreadPort::deactivate() {
	shutdown = true;
	RSCDEBUG(logger, "deactivate() entered"); // << *id);
	rec->cancel();
	// killing spread connection, exception thrown to rec thread which
	// should be handled specifically as the cancel flag was set
	// memberships->leaveAll();
	con->deactivate();
	RSCDEBUG(logger, "deactivate() stopping qad task object");
	qad->cancel();
	//	cout << "stopping st task" << endl;
	//	st->cancel();
	RSCTRACE(logger, "deactivate() finished"); // << *id);
}

SpreadPort::~SpreadPort() {
	if (!shutdown) {
		deactivate();
	}
}

void SpreadPort::notify(rsb::filter::ScopeFilter* f,
		rsb::filter::FilterAction::Types at) {
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
				"ScopeFilter Action not supported by this Port implementation")
		;
		break;
	}

}

void SpreadPort::push(RSBEventPtr e) {
	// get matching converter
	string wire;
	boost::shared_ptr<void> obj = e->getData();
	// TODO exception handling if converter is not available
	boost::shared_ptr<AbstractConverter<string> > c =
			converters->getConverterByDataType(e->getType());
	string wireType = c->serialize(make_pair(e->getType(), obj), wire);

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
	n.mutable_data()->set_binary(wire);
	n.mutable_data()->set_length(wire.length());
	string sm;

	if (!n.SerializeToString(&sm)) {
		throw ProtocolException("Failed to write notification to stream");
	}

	SpreadMessage msg(sm);
	// TODO convert URI to group name
	// TODO check if it is necessary to join spread groups when only sending to them

	msg.addGroup(e->getURI());
	msg.setQOS(messageQoS);

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

SpreadPort::QoSMap SpreadPort::buildQoSMapping() {

	map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> unorderedMap;
	unorderedMap.insert(make_pair(QualityOfServiceSpec::UNRELIABLE,
			SpreadMessage::UNRELIABLE));
	unorderedMap.insert(make_pair(QualityOfServiceSpec::RELIABLE,
			SpreadMessage::RELIABLE));

	map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> orderedMap;
	orderedMap.insert(make_pair(QualityOfServiceSpec::UNRELIABLE,
			SpreadMessage::FIFO));
	orderedMap.insert(make_pair(QualityOfServiceSpec::RELIABLE,
			SpreadMessage::FIFO));

	map<QualityOfServiceSpec::Ordering, map<QualityOfServiceSpec::Reliability,
			SpreadMessage::QOS> > table;
	table.insert(make_pair(QualityOfServiceSpec::UNORDERED, unorderedMap));
	table.insert(make_pair(QualityOfServiceSpec::ORDERED, orderedMap));

	return table;

}

void SpreadPort::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {

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
