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

#ifndef REMOTESERVER_H_
#define REMOTESERVER_H_

#include <stdexcept>
#include <string>

#include <boost/noncopyable.hpp>

#include <rsc/logging/Logger.h>

#include "../RSBEvent.h"
#include "../Publisher.h"
#include "../Subscriber.h"

namespace rsb {
namespace patterns {

class WaitingEventHandler;

/**
 * The client side of a request-reply-based communication channel.
 *
 * @author jwienke
 */
class RemoteServer: public boost::noncopyable {
public:

	/**
	 * Thrown if a remote called timed out.
	 *
	 * @author jwienke
	 */
	class TimeoutException: public std::runtime_error {
	public:
		explicit TimeoutException(const std::string &message) :
			std::runtime_error(message) {
		}
	};

	/**
	 * Thrown if a remote method call resulted in an exception.
	 *
	 * @author jwienke
	 */
	class RemoteTargetInvocationException: public std::runtime_error {
	public:
		explicit RemoteTargetInvocationException(const std::string &message) :
			std::runtime_error(message) {
		}
	};

	RemoteServer(const std::string &uri);
	virtual ~RemoteServer();

	RSBEventPtr callMethod(const std::string &methodName, RSBEventPtr data);

private:

	rsc::logging::LoggerPtr logger;

	std::string uri;

	class MethodSet {
	public:
		std::string methodName;
		std::string sendType;
		boost::shared_ptr<WaitingEventHandler> handler;
		SubscriberPtr replySubscriber;
		SubscriptionPtr replySubscription;
		Publisher<void>::Ptr requestPublisher;
	};

	boost::mutex methodSetMutex;
	std::map<std::string, MethodSet> methodSets;

	MethodSet getMethodSet(const std::string &methodName,
			const std::string &sendType);

};

typedef boost::shared_ptr<RemoteServer> RemoteServerPtr;

}
}

#endif /* REMOTESERVER_H_ */
