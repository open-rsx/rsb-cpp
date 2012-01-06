/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include <boost/format.hpp>

#include <boost/thread.hpp>

#include <rsb/Factory.h>

using namespace std;

using namespace boost;

using namespace rsb;
using namespace rsb::patterns;

int main(int /*argc*/, char** /*argv*/) {
    // Use the RSB factory to create a RemoteServer instance for the
    // server at scope /example/server.
    Factory& factory = Factory::getInstance();
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
            boost::shared_ptr<string> request(new string(str(format("This is request 1 in iteration %1%")
                                                      % iteration)));
            boost:: shared_ptr<string> result
                = remoteServer->call<string>(methodName1, request, 10);
            cout << "Got result: " << *result << endl;
        } catch (const std::exception& e) {
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
        } catch (const std::exception& e) {
            cerr << "Error calling method: " << e.what() << endl;
        }

        // Call the method "methodError" which signals an error when
        // called. This remote error causes an exception to be thrown
        // locally.
        cout << "Calling method " << methodName3 << endl;
        try {
            remoteServer->call<string>(methodName3, boost::shared_ptr<string>(new string("bla")));
            cout << "Method call succeeded; This should not happen" << endl;
        } catch (const std::exception& e) {
            cout << "Got error: " << e.what() << endl;
        }

        // Wait a little bit.
        this_thread::sleep(posix_time::seconds(10));
    }

    return EXIT_SUCCESS;

}
