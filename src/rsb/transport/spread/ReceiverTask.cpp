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

#include "ReceiverTask.h"

#include <iostream>

#include "../../protocol/Notification.h"
#include "../../converter/Converter.h"
#include "../../CommException.h"
#include "SpreadConnection.h"

using namespace std;
using namespace rsb;
using namespace rsb::protocol;
using namespace rsb::transport;
using namespace rsc::logging;

namespace rsb {
namespace spread {

DataStore::DataStore(rsb::protocol::NotificationPtr n) :
	logger(Logger::getLogger("rsb.spread.DataStore")), receivedParts(0) {
	store.resize(n->num_data_parts() + 1);
	add(n);
}

DataStore::~DataStore() {
}

string DataStore::getData(const unsigned int &i) {
	return store[i]->data().binary();
}

unsigned int DataStore::add(rsb::protocol::NotificationPtr n) {
	RSCTRACE(logger, "Add message " << n->eid() << " (part " << n->data_part() << ") to DataStore");
	store[n->data_part()] = n;
	return receivedParts++;
}

ReceiverTask::ReceiverTask(SpreadConnectionPtr s,
		converter::Repository<string>::Ptr converters, const Action &action) :
	logger(rsc::logging::Logger::getLogger("rsb.spread.ReceiverTask")),
			cancelRequested(false), con(s), converters(converters),
			action(action) {
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	//RSBINFO(logger, "Times (last cycle = " << timer->getElapsed()-timestamp << "ms)");
	RSCTRACE(logger, "ReceiverTask::RecieverTask, SpreadConnection: " << con);
	GOOGLE_PROTOBUF_VERIFY_VERSION;

}

ReceiverTask::~ReceiverTask() {
}

void ReceiverTask::execute() {
	// TODO Do performance optimization for data joining
	// TODO Think about old data in dataPool map
	//logger->setLevel(Logger::TRACE);
	NotificationPtr n(new Notification());
	bool bigMsgComplete = false;
	boost::shared_ptr<string> s;
	try {
		SpreadMessagePtr sm(new SpreadMessage(SpreadMessage::REGULAR));
		con->receive(sm);
		RSCDEBUG(logger, "ReceiverTask::execute new SpreadMessage received " << sm);
		// TODO think about how to deal with non-data messages, e.g., membership
		if (SpreadMessage::REGULAR == sm->getType()) {
			if (!sm || !n) {
				throw CommException("Ptr Null");
			}
			if (!n->ParseFromString(sm->getDataAsString())) {
				throw CommException(
						"Failed to parse notification in pbuf format");
			}
			RSCTRACE(logger, "Parsed event ID: " << n->eid());
			RSCTRACE(logger, "Binary length: " << n->data().length());
			RSCTRACE(logger, "Number of split message parts: " << n->num_data_parts());
			RSCTRACE(logger, "... received message part    : " << n->data_part());

			//Build data from parts
			if (n->num_data_parts() > 0) {
				it = dataPool.find(n->eid());
				if (it != dataPool.end()) {
					// Push message to existing DataQueue
					RSCTRACE(logger, "Add message to existing data store");
					if (n->num_data_parts() == it->second->add(n)) {
						RSCTRACE(logger, "Join data parts");
						// Concatenate data parts
						s = boost::shared_ptr<string>(
								new string(it->second->getData(0)));
						for (unsigned int i = 1; i <= n->num_data_parts(); ++i) {
							s->append(it->second->getData(i));
						}
						dataPool.erase(it);
						bigMsgComplete = true;
					}
				} else {
					// Create new DataStore
					RSCTRACE(logger, "Create new data store for message: " << n->eid());
					dataPool.insert(
							pair<string, DataStorePtr> (n->eid(),
									DataStorePtr(new DataStore(n))));
				}
			} else {
				s = boost::shared_ptr<string>(new string(n->data().binary()));
			}
			RSCTRACE(logger, "dataPool size: " << dataPool.size());
			if ((n->num_data_parts() == 0) || bigMsgComplete) {
				// Send message as single spread event
				EventPtr e(new Event());
				e->setUUID(n->eid());
				e->setURI(n->uri());
				for (int i = 0; i < n->metainfos_size(); ++i) {
					e->addMetaInfo(n->metainfos(i).key(),
							n->metainfos(i).value());
				}
				// TODO refactor converter handling and conversion
				// TODO error handling
				converter::Converter<string>::Ptr c =
						converters->getConverterByWireSchema(n->type_id());
				converter::AnnotatedData deserialized = c->deserialize(
						n->type_id(), *s);
				e->setType(deserialized.first);
				e->setData(deserialized.second);
				if (action) {
					action(e);
				}
				bigMsgComplete = false;
			}
		}
	} catch (rsb::CommException &e) {
		if (!isCancelRequested()) {
			cout << "SpreadConnector error: " << e.what() << endl;
		} else {
			// safely ignore, invalid mbox just signals in this context
			// that the connection to spread was deactivated
		}
	}

}

void ReceiverTask::setAction(const Action &action) {
	this->action = action;
}

}
}
