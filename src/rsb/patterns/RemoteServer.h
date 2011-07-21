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

#pragma once

#include <stdexcept>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include <rsc/logging/Logger.h>

#include "../Event.h"
#include "../Informer.h"
#include "../Listener.h"
#include "../Exception.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace patterns {

class WaitingEventHandler;

/**
 * The client side of a request-reply-based communication channel.
 *
 * @author jwienke
 */
class RSB_EXPORT RemoteServer: public boost::noncopyable {
public:

    /**
     * Thrown if a remote calle timed out.
     *
     * @author jwienke
     */
    class TimeoutException: public Exception {
    public:
        explicit TimeoutException(const std::string &message);
    };

    /**
     * Thrown if a remote method call resulted in an exception.
     *
     * @author jwienke
     */
    class RemoteTargetInvocationException: public Exception {
    public:
        explicit RemoteTargetInvocationException(const std::string &message);
    };

    /**
     * Construct a new @c RemoteServer object which can be used to
     * call methods of the server at @a scope.
     *
     * @param scope The base scope of the server the methods of which
     * will be called.
     * @param maxReplyWaitTime Maximum number of seconds to wait for a
     * reply from the server when calling a method.
     */
    RemoteServer(const Scope &scope,
                 unsigned int maxReplyWaitTime = 25);
    virtual ~RemoteServer();

    EventPtr callMethod(const std::string &methodName, EventPtr data);

private:

    rsc::logging::LoggerPtr logger;

    Scope scope;

    class MethodSet {
    public:
        std::string methodName;
        std::string sendType;
        boost::shared_ptr<WaitingEventHandler> handler;
        ListenerPtr replyListener;
        Informer<void>::Ptr requestInformer;
    };

    boost::mutex methodSetMutex;
    std::map<std::string, MethodSet> methodSets;

    unsigned int maxReplyWaitTime;

    MethodSet getMethodSet(const std::string &methodName,
                           const std::string &sendType);

};

typedef boost::shared_ptr<RemoteServer> RemoteServerPtr;

}
}
