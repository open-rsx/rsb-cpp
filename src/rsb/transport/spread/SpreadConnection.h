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

#ifndef SPREADCONNECTION_H_
#define SPREADCONNECTION_H_

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "SpreadMessage.h"
#include "rsb/rsbexports.h"

// forward declaration to avoid exposing sp.h with strange defines that prevent
// other code from compiling
typedef int mailbox;

namespace rsb {
namespace spread {

/**
 * @author swrede
 * @todo really expose this through dlls only for the unit tests?
 */
class RSB_EXPORT SpreadConnection {
public:
	SpreadConnection(const std::string &prefix, const std::string &host,
			const std::string& port);
	SpreadConnection(const std::string &prefix);
	virtual ~SpreadConnection();

	// connection state management
	// CHECK: this is similar to the other active objects, but is it
	// really necessary in the spread case?
	void activate();
	void deactivate();

	// fundamental message exchange
	bool send(const SpreadMessage &msg);
	void receive(SpreadMessagePtr sm);

	// is connected to spread daemon
	bool isActive();

	// return number of message sent
	unsigned long getMsgCount();

	// return mailbox for other low-level functions
	mailbox *getMailbox();

private:
	std::string generateId(const std::string &prefix);

	rsc::logging::LoggerPtr logger;
	bool connected; // flag to indicate whether we are connected to spread
	mailbox con; // Handle to the returned spread connection
	std::string host; // host
	std::string port; // and port
	std::string spreadhost; // and name of spread daemon, e.g., 4803@localhost
	std::string spreadpg; // private name of this connection
	std::string conId; // user name to be used for spread connection
	unsigned long msgCount; // nr of message sent via this connection

};

typedef boost::shared_ptr<SpreadConnection> SpreadConnectionPtr;

}

}

#endif /* SPREADCONNECTION_H_ */
