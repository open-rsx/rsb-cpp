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

#include "../../protocol/Notification.h"
#include "SpreadPort.h"
#include "SpreadConnection.h"
#include "../../util/Configuration.h"
#include "../../CommException.h"

#include <iostream>
#include <string.h>

#include "../AbstractConverter.h"
#include "../../protocol/ProtocolException.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::util;
using namespace rsb::transport;
using namespace rsb::protocol;

namespace rsb {

namespace spread {

SpreadPort::SpreadPort() : logger(rsc::logging::Logger::getLogger("rsb.spread.SpreadPort")) {
	RSBDEBUG(logger, "SpreadPort() entered, port id: " << id.getIdAsString());
	shutdown = false;
	exec = TaskExecutorVoidPtr(new TaskExecutor<void>());
	// TODO ConnectionPool for SpreadConnections?!?
	// TODO Send Message over Managing / Introspection Channel
	// TODO Generate Unique-IDs for SpreadPorts
	con = SpreadConnectionPtr(new SpreadConnection(id.getIdAsString()));
	// TODO check if it makes sense and is possible to provide a weak_ptr to the ctr of StatusTask
	//st = boost::shared_ptr<StatusTask>(new StatusTask(this));
	qad = boost::shared_ptr<QueueAndDispatchTask<RSBEventPtr > >(new QueueAndDispatchTask<RSBEventPtr>());
	rec = boost::shared_ptr<ReceiverTask>(new ReceiverTask(con,converters,qad));
	memberships = MembershipManagerPtr(new MembershipManager());
}

void SpreadPort::activate() {
	// connect to spread
	con->activate();
	// (re-)start threads
	recTask = exec->schedulePeriodic<ReceiverTask>(rec,0);
	qadTask = exec->schedulePeriodic<QueueAndDispatchTask<RSBEventPtr> >(qad,0);
	//staTask = exec->schedulePeriodic<StatusTask>(st,500);
}

// TODO think about extracting qad functions into QADPort?!?
void SpreadPort::setObserver(Action a) {
	qad->setObserver(a);
}

void SpreadPort::removeObserver(Action a) {
	qad->setObserver(NULL);
}

void SpreadPort::deactivate() {
	shutdown = true;
	RSBDEBUG(logger, "deactivate() entered"); // << *id);
	rec->cancel();
	// killing spread connection, exception thrown to rec thread which
	// should be handled specifically as the cancel flag was set
	// memberships->leaveAll();
	con->deactivate();
	RSBDEBUG(logger, "deactivate() stopping qad task object");
	qad->cancel();
	RSBDEBUG(logger, "deactivate() stopping qad thread");
	exec->join(qadTask);
	RSBDEBUG(logger, "deactivate() stopping receiver thread");
	exec->join(recTask);
//	cout << "stopping st task" << endl;
//	staTask->cancel();
//	cout << "stopping st thread" << endl;
//	exec->join(staTask);
	RSBTRACE(logger, "deactivate() finished"); // << *id);
};

SpreadPort::~SpreadPort() {
	if (!shutdown) deactivate();
}

void SpreadPort::notify(rsb::filter::ScopeFilter* f, rsb::filter::FilterAction::Types at) {
	// join or leave groups
	// TODO evaluate success
	RSBDEBUG(logger, "notify(rsb::filter::ScopeFilter*, ...) entered"); // << *id);
	switch (at) {
		case rsb::filter::FilterAction::ADD:
			RSBINFO(logger, "ScopeFilter URI is "<< f->getURI() << " ,now going to join Spread group");
			memberships->join(f->getURI(),con);
			break;
		case rsb::filter::FilterAction::REMOVE:
			RSBINFO(logger, "ScopeFilter URI is "<< f->getURI() << " ,now going to leave Spread group");
			memberships->leave(f->getURI(),con);
			break;
		default:
			RSBWARN(logger, "ScopeFilter Action not supported by this Port implementation");
			break;
	}

}

void SpreadPort::push(RSBEventPtr e) {
	// get matching converter
	string s;
	boost::shared_ptr<void> obj = boost::static_pointer_cast<void>(e->getData());
	cerr << "SpreadPort::push Type: " << e->getType() << endl;
	boost::shared_ptr<AbstractConverter<string> > c = boost::static_pointer_cast<AbstractConverter<string> >((*converters)[e->getType()]);
	c->serialize(e->getType(),obj,s);
	cerr << "SpreadPort::push after serialize" << endl;
	Notification n;
	n.set_eid("not set yet");
	n.set_s_length(0);
	n.set_standalone(false);
	n.set_uri(e->getURI());
	n.set_type_id(e->getType());
	cerr << "SpreadPort::push after set type" << endl;
	n.mutable_data()->set_binary(s);
	// TODO fix this, think about whether this is needed
	n.mutable_data()->set_length(s.length());
	cerr << "SpreadPort::push after notification" << endl;
	string sm;

	if (!n.SerializeToString(&sm)) {
		throw ProtocolException("Failed to write notification to stream");
	}

	SpreadMessage msg(sm);
    // TODO convert URI to group name
    // TODO check if it is necessary to join spread groups when only sending to them

	msg.addGroup(e->getURI());

	if (!con->send(msg)) {
//        for (list<string>::const_iterator n = msg->getGroupsBegin(); n != msg->getGroupsEnd(); ++n) {
//           cout << "Sending msg to following groups: " << *n << endl;
//        }
    	// TODO implement queing or throw messages away?
    	// TODO maybe return exception with msg that was not sent
    	RSBWARN(logger, "Spread Connection inactive -> could not send message");
    } else {
    	RSBDEBUG(logger, "event sent to spread");
    }
}

}

}
