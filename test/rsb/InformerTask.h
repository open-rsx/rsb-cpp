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

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>

#include <rsc/threading/RepetitiveTask.h>

#include "rsb/transport/Connector.h"

namespace rsb {
namespace test {

/**
 * @author swrede
 */
class InformerTask: public rsc::threading::RepetitiveTask {
public:
	InformerTask(rsb::transport::OutConnectorPtr p, const unsigned int &numEvents);
	virtual ~InformerTask();

	void execute();

	std::vector<EventPtr> getEvents();

private:
	unsigned int numEvents;
	unsigned int sentEvents;
	rsb::transport::OutConnectorPtr port;
	std::vector<EventPtr> events;

};

/**
 * @author jwienke
 */
class WaitingObserver {
public:

	WaitingObserver(const unsigned int &desiredEvents);

	void handler(EventPtr e);

	void waitReceived();

	std::vector<EventPtr> getEvents();

private:

	unsigned int desiredEvents;
	unsigned int receivedEvents;
	boost::recursive_mutex m;
	boost::condition condition;
	std::vector<EventPtr> events;

};

}
}
