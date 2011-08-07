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

    virtual ~TestCallback() {
    }

    string methodName;

    TestCallback(const string &methodName) :
        methodName(methodName) {
    }

    boost::shared_ptr<string> call(const string &methodName,
                                   boost::shared_ptr<string> input) {
        assert(methodName == this->methodName);

        if (methodName == "methodError") {
            throw runtime_error("Intentionally failing.");
        }

        return boost::shared_ptr<string>(
                new string("reply to '" + *input + "' (method is '" + methodName + "')"));
    }

};

int main(int /*argc*/, char **/*argv*/) {
    // Use the RSB factory to create a Server instance that provides
    // callable methods under the scope /example/server.
    Factory &factory = Factory::getInstance();
    const Scope scope("/example/server");
    ServerPtr server = factory.createServer(scope);

    // Register callable methods which dispatch method calls to
    // instances of TestCallback.
    const string methodName1 = "methodOne";
    Server::CallbackPtr m1(new TestCallback(methodName1));
    server->registerMethod(methodName1, m1);

    const string methodName2 = "methodTwo";
    Server::CallbackPtr m2(new TestCallback(methodName2));
    server->registerMethod(methodName2, m2);

    const string methodName3 = "methodError";
    Server::CallbackPtr m3(new TestCallback(methodName3));
    server->registerMethod(methodName3, m3);

    // Wait here so incoming method calls can be processed.
    boost::this_thread::sleep(boost::posix_time::seconds(1000));

    return EXIT_SUCCESS;
}
