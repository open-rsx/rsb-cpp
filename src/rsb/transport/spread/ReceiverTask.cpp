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
#include "ReceiverTask.h"
#include "../AbstractConverter.h"
#include "../../CommException.h"

#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::protocol;
using namespace rsb::transport;
using namespace rsc::logging;

namespace rsb {

namespace spread {

ReceiverTask::ReceiverTask(SpreadConnectionPtr s, rsc::misc::Registry<
		rsb::transport::AbstractConverter<std::string> > *converters, QADPtr q) :
	logger(rsc::logging::Logger::getLogger("rsb.spread.ReceiverTask")),
			cancelRequested(false), con(s), converters(converters), qad(q) {
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	//RSBINFO(logger, "Times (last cycle = " << timer->getElapsed()-timestamp << "ms)");
	RSCTRACE(logger, "ReceiverTask::RecieverTask, SpreadConnection: " << con);
	GOOGLE_PROTOBUF_VERIFY_VERSION;

}

ReceiverTask::~ReceiverTask() {
	// TODO Auto-generated destructor stub
}

void ReceiverTask::execute(rsb::util::Task<void>* t) {
	NotificationPtr n(new Notification());

	try {
		SpreadMessagePtr sm(new SpreadMessage(SpreadMessage::REGULAR));
		con->receive(sm);
		RSCDEBUG(logger, "ReceiverTask::execute new SpreadMessage received "
				<< sm);
		// TODO think about how to deal with non-data messages, e.g., membership
		if (SpreadMessage::REGULAR == sm->getType()) {
			if (!sm || !n) {
				throw CommException("Ptr Null");
			}
			if (!n->ParseFromString(sm->getDataAsString())) {
				throw CommException(
						"Failed to parse notification in pbuf format");
			}
			//	cout << "Parsed event ID: " << n->eid() << endl;
			//	cout << "Binary length: " << n->data().length() << endl;
			RSBEventPtr e(new RSBEvent());
			e->setURI(n->uri());
			e->setType(n->type_id());
			for (int i = 0; i < n->metainfos_size(); ++i) {
				e->addMetaInfo(n->metainfos(i).key(), n->metainfos(i).value());
			}
			// TODO refactor converter handling and conversion
			boost::shared_ptr<AbstractConverter<string> > c =
					converters->getRegistree(e->getType());
			e->setData(c->deserialize(e->getType(), n->data().binary()));
			qad->addElement(e);
		}
	} catch (rsb::CommException &e) {
		if (!cancelRequested) {
			cout << "SpreadPort error: " << e.what() << endl;
		} else {
			// safely ignore, invalid mbox just signals in this context
			// that the connection to spread was deactivated
		}
	}
	if (cancelRequested) {
		t->cancel();
	}
}

}

}
