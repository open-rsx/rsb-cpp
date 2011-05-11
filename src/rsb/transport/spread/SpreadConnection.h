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

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>

#include "SpreadMessage.h"
#include "rsb/rsbexports.h"

// forward declaration to avoid exposing sp.h with strange defines that prevent
// other code from compiling
typedef int mailbox;

namespace rsb {
namespace spread {

RSB_EXPORT std::string defaultHost();

RSB_EXPORT unsigned int defaultPort();

/**
 * @author swrede
 * @todo really expose this through dlls only for the unit tests?
 */
class RSB_EXPORT SpreadConnection {
public:
    SpreadConnection(const std::string &prefix,
            const std::string &host = defaultHost(),
            unsigned int port = defaultPort());
    virtual ~SpreadConnection();

    /**
     * @name connection state management
     * @todo is this really necesary?
     */
    //@{
    void activate();
    void deactivate();
    //@}

    /**
     * @name fundamental message exchange
     */
    //@{
    bool send(const SpreadMessage &msg);
    void receive(SpreadMessagePtr sm);
    //@}

    /**
     * Tells if this instance is connected to spread daemon.
     *
     * @return @c true if connected
     */
    bool isActive();

    /**
     * Returns number of messages sent.
     *
     * @return number of sent messages
     */
    unsigned long getMsgCount();

    /**
     * Returns the internally used mailbox for other low-level functions.
     *
     * @return mailbox
     * @todo why pointer? mailbox is a typedef to int? If pointer is required
     *       use a shared ptr
     */
    mailbox *getMailbox();

private:
    std::string generateId(const std::string &prefix);

    rsc::logging::LoggerPtr logger;
    /**
     * A flag to indicate whether we are connected to spread.
     */
    bool connected;
    /**
     * Handle to the internal spread connection.
     */
    mailbox con;
    /**
     * Host for the spread daemon.
     */
    std::string host;
    /**
     * Port for the spread daemon.
     */
    unsigned int port;
    /**
     * The composd name of the daemon made up from port and host, e.g.
     * 4803@localhost,
     */
    std::string spreadhost;
    /**
     * Private name of this connection.
     */
    std::string spreadpg;
    /**
     * User-defined name to be used for this spread connection.
     */
    std::string conId;
    /**
     * Number of message sent via this connection.
     */
    unsigned long msgCount;
};

typedef boost::shared_ptr<SpreadConnection> SpreadConnectionPtr;

}

}
