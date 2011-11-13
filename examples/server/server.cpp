/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <rsb/Factory.h>

#include <boost/thread.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/logging/LoggerFactory.h>

using namespace std;

using namespace rsb;
using namespace rsb::patterns;

class TestCallback: public Server::Callback<string, string> {
public:
    boost::shared_ptr<string> call(const string& methodName,
                                   boost::shared_ptr<string> input) {
        return boost::shared_ptr<string>(
                new string("reply to '" + *input + "' (method is '" + methodName + "')"));
    }
};

class ErrorProducingCallback: public Server::Callback<string, string> {
public:
    boost::shared_ptr<string> call(const string& /*methodName*/,
                                   boost::shared_ptr<string> /*input*/) {
        throw runtime_error("Intentionally failing.");
    }
};

int main(int /*argc*/, char** /*argv*/) {
    // Use the RSB factory to create a Server instance that provides
    // callable methods under the scope /example/server.
    Factory& factory = Factory::getInstance();
    ServerPtr server = factory.createServer(Scope("/example/server"));

    // Register callable methods which dispatch method calls to
    // instances of TestCallback and ErrorProducingCallback.
    server->registerMethod("methodOne",
                           Server::CallbackPtr(new TestCallback()));
    server->registerMethod("methodTwo",
                           Server::CallbackPtr(new TestCallback()));
    server->registerMethod("methodError",
                           Server::CallbackPtr(new ErrorProducingCallback()));

    // Wait here so incoming method calls can be processed.
    boost::this_thread::sleep(boost::posix_time::seconds(1000));

    return EXIT_SUCCESS;
}
