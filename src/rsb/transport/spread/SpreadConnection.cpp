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

#include "SpreadConnection.h"

#include <iostream>
#include <string.h>

#include <sp.h>

#include "../../util/Configuration.h"
#include "../../CommException.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb::util;

namespace rsb {
namespace spread {

#define SPREAD_MAX_GROUPS   100
#define SPREAD_MAX_MESSLEN  180000

// TODO make port a numerical attribute
SpreadConnection::SpreadConnection(const string& id, const string& h,
		const string& p) :
	logger(Logger::getLogger("rsb.spread.SpreadConnection")), connected(false),
			host(h), port(p), conId(id), msgCount(0) {
	spreadhost = port + "@" + host;
	RSCDEBUG(logger, "instantiated spread connection with id " << conId
			<< " to spread daemon at " << spreadhost);
}

SpreadConnection::SpreadConnection(const string& id) :
	logger(Logger::getLogger("rsb.spread.SpreadConnection")), connected(false),
			conId(id), msgCount(0) {
	host = Configuration::getInstance()->getProperty("Spread.Host");
	port = Configuration::getInstance()->getProperty("Spread.Port");
	spreadhost = port + "@" + host;
	RSCDEBUG(logger, "instantiated spread connection with id " << conId
			<< " to spread daemon at " << spreadhost);
}

SpreadConnection::~SpreadConnection() {
	// this does not work with XcfAppender...
	RSCDEBUG(logger, "destroying SpreadConnection object");
}

void SpreadConnection::activate() {
	// spread init and group join - not threadsafe
	if (!connected) {
		RSCDEBUG(logger, "connecting to spread daemon at " << spreadhost);
		// TODO store connection ID
		char spreadPrivateGroup[MAX_GROUP_NAME];
		int ret = SP_connect(spreadhost.c_str(), 0, 0, 0, &con,
				spreadPrivateGroup);
		spreadpg = string(spreadPrivateGroup);
		if (ret != ACCEPT_SESSION) {
			switch (ret) {
			case ILLEGAL_SPREAD:
				// TODO throw initialization exception
				RSCFATAL(logger,
						"spread connect error: connection to spread daemon at "
						<< spreadhost
						<< " failed, check port and hostname")
				;
				break;
			case COULD_NOT_CONNECT:
				// TODO throw initialization exception
				RSCFATAL(logger,
						"spread connect error: connection to spread daemon failed due to socket errors")
				;
				break;
			case CONNECTION_CLOSED:
				// CHECK throw initialize exception?
				RSCFATAL(
						logger,
						"spread connect error: communication errors occurred during setup of connection")
				;
				throw CommException(
						"spread connect error: communication errors occurred during setup of connection");
			case REJECT_VERSION:
				// TODO throw initialization exception
				RSCFATAL(logger,
						"spread connect error: daemon or library version mismatch")
				;
				break;
			case REJECT_NO_NAME:
				// TODO throw initialization exception
				RSCFATAL(logger,
						"spread connect error: protocol error during setup")
				;
				break;
			case REJECT_ILLEGAL_NAME:
				// TODO throw initialization exception
				RSCFATAL(
						logger,
						"spread connect error: name provided violated requirement, length or illegal character")
				;
				break;
			case REJECT_NOT_UNIQUE:
				// TODO throw name exists exception
				RSCFATAL(logger,
						"spread connect error: name provided is not unique on this daemon")
				;
				break;
			default:
				RSCFATAL(logger, "unknown spread connect error")
				;
			}
			// TODO throw exception
			SP_error(ret);
			throw CommException("Error during connection to spread daemon");
		} else {
			// TODO return private group id as result of this function
			RSCDEBUG(logger, "success, private group id is " << spreadpg);
		}
		RSCINFO(logger, "connected to spread daemon");

		connected = true;

	}
}

void SpreadConnection::deactivate() {
	if (isActive()) {
		//SP_disconnect(con);
		SP_multicast(con, RELIABLE_MESS, spreadpg.c_str(), 0, 0, 0);
		RSCDEBUG(logger, "Spread connection disconnected, id " << conId
				<< " private group " << spreadpg);
		connected = false;
	} else {
		RSCWARN(logger, "Trying to deactivate a connection that was not active");
	}
}

bool SpreadConnection::isActive() {
	return connected;
}

void SpreadConnection::receive(SpreadMessagePtr sm) {
	// read from Spread multicast group
	// TODO add something like sm->reset
	int service_type;
	int num_groups;
	char sender[MAX_GROUP_NAME];
	char ret_groups[SPREAD_MAX_GROUPS][MAX_GROUP_NAME];
	int16 mess_type;
	int dummy_endian_mismatch;
	char buf[SPREAD_MAX_MESSLEN];
	int ret = SP_receive(con, &service_type, sender, SPREAD_MAX_GROUPS,
			&num_groups, ret_groups, &mess_type, &dummy_endian_mismatch,
			SPREAD_MAX_MESSLEN, buf);
	if (ret >= 0) {
		if (Is_regular_mess(service_type)) {
			RSCINFO(logger, "regular spread message received");

			// cancel if requested
			if (num_groups == 1 && string(ret_groups[0]) == string(spreadpg)) {
				SP_disconnect(con);
				connected = false;
				throw CommException("Cancelled!");
			}

			sm->setType(SpreadMessage::REGULAR);
			sm->setData(string(buf, ret));
			if (num_groups < 0) {
				// TODO check whether we shall implement a best effort strategy here
				RSCWARN(logger,
						"error during message receival, group array too large, requested size "
						<< " configured size " << SPREAD_MAX_GROUPS);
			}
			for (int i = 0; i < num_groups; i++) {
				if (ret_groups[i] != NULL) {
					string group = string(ret_groups[i]);
					RSCDEBUG(logger,
							"received message, addressed at group with name "
							<< group);
					sm->addGroup(group);
				}
			}
		} else if (Is_membership_mess(service_type)) {
			RSCINFO(logger, "received spread membership message type");
			sm = SpreadMessagePtr(new SpreadMessage(SpreadMessage::MEMBERSHIP));
		} else {
			RSCWARN(logger, "received unknown spread message type");
		}
		RSCDEBUG(logger, "before returning spread message with content: "
				+ sm->getDataAsString());
	} else {
		string err;
		switch (ret) {
		case ILLEGAL_SESSION:
			err = "spread receive error: mbox given to receive on was illegal";
			break;
		case ILLEGAL_MESSAGE:
			err = "spread receive error: message had an illegal structure";
			break;
		case CONNECTION_CLOSED:
			err = "spread receive error: message communication errors occurred";
			break;
		case GROUPS_TOO_SHORT:
			err
					= "spread receive error: groups array too short to hold list of groups";
			break;
		case BUFFER_TOO_SHORT:
			err
					= "spread receive error: message body buffer too short to hold the message received";
			break;
		default:
			err = "unknown spread receive error";
		}
		throw CommException("Spread communication error. Reason: " + err);
	}
	if (!Is_regular_mess(service_type)) {
		// set flag to not process message - shouldn't be a data packet
		throw CommException("received unknown type of spread message");
	}

}

bool SpreadConnection::send(const SpreadMessage& msg) {
	// TODO check message size, if larger than ~100KB throw exception
	// TODO add mutex, enque or send directly?
	int groupCount = msg.getGroupCount();
	if (groupCount == 0) {
		throw CommException("group information missing in message");
	}
	if (isActive()) {
		// TODO add error handling
		int ret;
		if (groupCount == 1) {
			// use SP_multicast
			string group = *msg.getGroupsBegin();
			RSCDEBUG(logger, "sending message to group with name " << group);
			ret = SP_multicast(con, RELIABLE_MESS, group.c_str(), 0,
					msg.getSize(), msg.getData());
			msgCount++;
		} else {
			// use SP_multigroup_multicast
			char groups[groupCount][MAX_GROUP_NAME];
			int j = 0;
			for (list<string>::const_iterator it = msg.getGroupsBegin(); it
					!= msg.getGroupsEnd(); ++it) {
				string group = *it;
				strcpy(groups[j], group.c_str());
				j++;
			}
			ret = SP_multigroup_multicast(con, RELIABLE_MESS, groupCount,
					groups, RELIABLE_MESS, msg.getSize(), msg.getData());
			msgCount++;
		}
		// FIXME: check return code of the above call
		if (ret >= 0) {
			// cerr << "Message sent successfully!" << endl;
			return true;
		} else {
			switch (ret) {
			case ILLEGAL_SESSION:
				// TODO throw exception
				RSCWARN(logger, "Send: Illegal Session! ")
				;
				break;
			case ILLEGAL_MESSAGE:
				RSCWARN(logger, "Send: Illegal Message! ")
				;
				break;
			case CONNECTION_CLOSED:
				RSCWARN(logger, "Send: Connection Closed! ")
				;
				break;
			}
			return false;
		}

	} else {
		return false;
	}
}

string SpreadConnection::generateId(const string& prefix) {
	// TODO generate meaningful and unique Id according to MAX_PRIVATE_NAME
	//            uuid_t id;
	//            uuid_generate(id);
	//            char buf[37];
	//            uuid_unparse(id,buf);
	return string(prefix);
}

unsigned long SpreadConnection::getMsgCount() {
	return msgCount;
}

mailbox *SpreadConnection::getMailbox() {
	return &con;
}

}
}
