/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

// mark-start::body
#include <stdlib.h>

#include <rsb/Factory.h>

using namespace rsb;
using namespace rsb::patterns;

int main(int /*argc*/, char** /*argv*/) {
    // Use the RSB factory to create a RemoteServer instance for the
    // server at scope /example/server.
    Factory& factory = Factory::getInstance();
    RemoteServerPtr remoteServer
        = factory.createRemoteServer("/example/server");

    // Call the method "echo", passing it a string value as argument
    // and accepting a string value as result. Note that the types of
    // arguments and return values are defined by the server providing
    // the respective methods and have to be matched in method calls.
    boost::shared_ptr<std::string> request(new std::string("bla"));
    boost::shared_ptr<std::string> result
        = remoteServer->call<std::string>("echo", request);
    std::cout << "Server replied: " << *result << std::endl;

    return EXIT_SUCCESS;
}
// mark-end::body
