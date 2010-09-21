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

#ifndef SERVER_H_
#define SERVER_H_

#include <map>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "../Publisher.h"
#include "../Subscriber.h"

namespace rsb {
namespace patterns {

/**
 * The server side of a request-reply-based communication channel.
 *
 * @author jwienke
 */
class Server: public boost::noncopyable {
public:

	class IntlCallback {
	public:

		virtual std::string getRequestType() const = 0;
		virtual std::string getReplyType() const = 0;

		virtual boost::shared_ptr<void>
				intlCall(const std::string &methodName,
						boost::shared_ptr<void> input) = 0;

	};

	template<class RequestType, class ReplyType>
	class Callback: public IntlCallback {
	public:

		virtual boost::shared_ptr<ReplyType> call(
				const std::string &methodName,
				boost::shared_ptr<RequestType> input) = 0;

	private:

		boost::shared_ptr<void> intlCall(const std::string &methodName,
				boost::shared_ptr<void> input) {
			return call(methodName, boost::static_pointer_cast<RequestType>(
					input));
		}

	};
	typedef boost::shared_ptr<IntlCallback> CallbackPtr;

	Server(const std::string &uri);
	virtual ~Server();

	void registerMethod(const std::string &methodName, CallbackPtr callback);

private:

	std::string uri;

	SubscriberPtr requestSubscriber;

	std::map<std::string, std::pair<SubscriptionPtr, Publisher<void>::Ptr> >
			methods;

};

typedef boost::shared_ptr<Server> ServerPtr;

}
}

#endif /* SERVER_H_ */
