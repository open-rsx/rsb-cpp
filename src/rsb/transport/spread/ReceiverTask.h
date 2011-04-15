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

#pragma once

#include <map>

#include <rsc/logging/Logger.h>
#include <rsc/threading/RepetitiveTask.h>
#include <rsc/misc/UUID.h>

#include "../../protocol/Notification.h"
#include "../../converter/Repository.h"
#include "../../Event.h"
#include "../../Handler.h"
#include "../Connector.h"
#include "../../converter/Converter.h"
#include "SpreadConnection.h"

namespace rsb {
namespace spread {

/**
 * @author swrede
 */
class DataStore {
public:

	DataStore(rsb::protocol::NotificationPtr n);

	~DataStore();

	std::string getData(const unsigned int &i);

	unsigned int add(rsb::protocol::NotificationPtr n);

private:
	rsc::logging::LoggerPtr logger;
	unsigned int receivedParts;
	std::vector<rsb::protocol::NotificationPtr> store;

};

typedef boost::shared_ptr<DataStore> DataStorePtr;

/**
 * @author swrede
 * @todo does action need locking if it is set while the task is running?
 */
class ReceiverTask: public rsc::threading::RepetitiveTask {
public:

	ReceiverTask(SpreadConnectionPtr s,
                     converter::Repository<std::string>::Ptr converters,
                     HandlerPtr handler);
	virtual ~ReceiverTask();

	void execute();
	void setHandler(HandlerPtr handler);

private:
	rsc::logging::LoggerPtr logger;
	volatile bool cancelRequested;
	SpreadConnectionPtr con;
	converter::Repository<std::string>::Ptr converters;
	HandlerPtr handler;
	std::map<std::string, boost::shared_ptr<DataStore> > dataPool;
	std::map<std::string, boost::shared_ptr<DataStore> >::iterator it;

};

}
}
