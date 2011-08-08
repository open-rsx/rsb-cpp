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

#include <boost/format.hpp>

#include <boost/thread.hpp>

#include <rsb/Factory.h>

using namespace std;

using namespace boost;

using namespace rsb;
using namespace rsb::patterns;

int main(int /*argc*/, char **/*argv*/) {
    // Use the RSB factory to create a RemoteServer instance for the
    // server at scope /example/server.
    Factory &factory = Factory::getInstance();
    RemoteServerPtr remoteServer
        = factory.createRemoteServer(Scope("/example/server"));

    string methodName1 = "methodOne";
    string methodName2 = "methodTwo";
    string methodName3 = "methodError";

    for (int iteration = 1; iteration < 5; ++iteration) {
        // Call the method "methodOne", passing it a string value as
        // argument and accepting a string value as result. Note that
        // the types of arguments and return values are defined by the
        // server providing the respective methods and have to be
        // matched in method calls.
        //
        // Specify a timeout of 10 seconds for the method call.
        cout << "Calling method " << methodName1 << endl;
        try {
            shared_ptr<string> request(new string(str(format("This is request 1 in iteration %1%")
                                                      % iteration)));
            shared_ptr<string> result
                = remoteServer->call<string>(methodName1, request, 10);
            cout << "Got result: " << *result << endl;
        } catch (std::exception &e) {
            cerr << "Error calling method: " << e.what() << endl;
        }

        // Call the method "methodTwo", passing it an event as
        // argument and accepting an event as result. Note that the
        // types of argument and return value still have to match the
        // types defined by the providing server.
        EventPtr request2(new Event);
        request2->setType("std::string");
        request2->setData(VoidPtr(new string(str(format("This is request 2 in iteration %1%")
                                                 % iteration))));
        cout << "Calling method " << methodName2 << endl;
        try {
            EventPtr result = remoteServer->call(methodName2, request2);
            cout << "Got result: " << *result << ": "
                    << *(static_pointer_cast<string>(result->getData()))
                    << endl;
        } catch (std::exception &e) {
            cerr << "Error calling method: " << e.what() << endl;
        }

        // Call the method "methodError" which signals an error when
        // called. This remote error causes an exception to be thrown
        // locally.
        cout << "Calling method " << methodName3 << endl;
        try {
            remoteServer->call<string>(methodName3, shared_ptr<string>(new string("bla")));
            cout << "Method call succeeded; This should not happen" << endl;
        } catch (std::exception &e) {
            cout << "Got error: " << e.what() << endl;
        }

        // Wait a little bit.
        this_thread::sleep(posix_time::seconds(10));
    }

    return EXIT_SUCCESS;

}
