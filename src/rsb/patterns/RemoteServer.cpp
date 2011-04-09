/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "RemoteServer.h"

#include <stdexcept>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include <rsc/misc/UUID.h>

using namespace std;

namespace rsb {
namespace patterns {

class WaitingEventHandler: public EventHandler {
private:

	rsc::logging::LoggerPtr logger;

	boost::mutex mutex;
	boost::condition condition;

	set<string> waitForEvents;
	map<string, RSBEventPtr> storedEvents;

public:

	WaitingEventHandler(rsc::logging::LoggerPtr logger) :
		logger(logger) {
	}

	void notify(RSBEventPtr event) {
		{
			boost::mutex::scoped_lock lock(mutex);
			if (event && event->hasMetaInfo("ServerRequestId")
					&& waitForEvents.count(
							event->getMetaInfo("ServerRequestId"))) {
				RSCDEBUG(logger, "Received reply event " << *event);
				waitForEvents.erase(event->getMetaInfo("ServerRequestId"));
				storedEvents[event->getMetaInfo("ServerRequestId")] = event;
			} else {
				RSCTRACE(logger, "Received uninteresting event " << *event);
			}
		}
		condition.notify_all();
	}

	void expectReply(const string &requestId) {
		boost::mutex::scoped_lock lock(mutex);
		waitForEvents.insert(requestId);
	}

	RSBEventPtr getReply(const string &requestId) {

		RSCTRACE(logger, "Waiting for reply with id " << requestId);

		boost::mutex::scoped_lock lock(mutex);
		// TODO duplicated string with Server
		while (!storedEvents.count(requestId)) {
			boost::xtime xt;
			xtime_get(&xt, boost::TIME_UTC);
			// TODO make max time a configuration property
			xt.sec += 25;
			if (!condition.timed_wait(lock, xt)) {
				RSCERROR(logger, "Timeout while waiting");
				throw RemoteServer::TimeoutException(
						"Error calling method and waiting for reply with id "
								+ requestId + ". Waited 25 seconds.");
			}
		}

		RSBEventPtr reply = storedEvents[requestId];
		storedEvents.erase(requestId);

		RSCDEBUG(logger, "Received correct reply event " << *reply);
		return reply;
	}

};

RemoteServer::TimeoutException::TimeoutException(const string &message) :
	runtime_error(message) {
}

RemoteServer::RemoteTargetInvocationException::RemoteTargetInvocationException(
		const string &message) :
	runtime_error(message) {
}

RemoteServer::RemoteServer(const string &uri) :
			logger(
					rsc::logging::Logger::getLogger(
							"rsc.patterns.RemoteServer." + uri)), uri(uri) {
	// TODO check that this server is alive...
	// TODO probably it would be a good idea to request some method infos from
	//      the server, e.g. for type checking
}

RemoteServer::~RemoteServer() {
}

RemoteServer::MethodSet RemoteServer::getMethodSet(const string &methodName,
		const string &sendType) {

	boost::mutex::scoped_lock lock(methodSetMutex);

	if (!methodSets.count(methodName)) {

		// start a subscriber to wait for the reply
		const string replyUri = uri + "-reply-" + methodName;
		SubscriberPtr subscriber(new Subscriber(replyUri));
		SubscriptionPtr subscription(new Subscription);
		subscription->appendFilter(
				filter::AbstractFilterPtr(new filter::ScopeFilter(replyUri)));
		boost::shared_ptr<WaitingEventHandler> handler(
				new WaitingEventHandler(logger));
		subscription->appendHandler(handler);
		subscriber->addSubscription(subscription);

		// publisher for requests
		Publisher<void>::Ptr publisher(
				new Publisher<void> (uri + "-request-" + methodName, sendType));

		MethodSet set;
		set.methodName = methodName;
		set.sendType = sendType;
		set.handler = handler;
		set.replySubscriber = subscriber;
		set.replySubscription = subscription;
		set.requestPublisher = publisher;

		methodSets[methodName] = set;

	}

	if (methodSets[methodName].sendType != sendType) {
		throw runtime_error(
				"Illegal send type. Method previously accepted "
						+ methodSets[methodName].sendType + " but now "
						+ sendType + " was requested");
	}

	return methodSets[methodName];

}

RSBEventPtr RemoteServer::callMethod(const string &methodName, RSBEventPtr data) {

	RSCDEBUG(logger, "Calling method " << methodName << " with data " << data);

	// TODO check that the desired method exists

	string requestId = rsc::misc::UUID().getIdAsString();
	// TODO duplicated string from Server
	data->addMetaInfo("ServerRequestId", requestId, true);

	MethodSet methodSet = getMethodSet(methodName, data->getType());
	methodSet.handler->expectReply(requestId);

	methodSet.requestPublisher->publish(data);

	// wait for the reply
	RSBEventPtr result = methodSet.handler->getReply(requestId);
	if (result->hasMetaInfo("isException")) {
		assert(result->getType() == "string");
		throw RemoteTargetInvocationException(
				"Error calling remote method '" + methodName + "': "
						+ *(boost::static_pointer_cast<string>(
								result->getData())));
	} else {
		return result;
	}

}

}
}
