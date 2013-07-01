/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <stdlib.h>

#include <stdexcept>
#include <iostream>

#include <rsb/Factory.h>

using namespace std;

int main() {

    // This test makes a failing attempt to contact a socket transport
    // server. This used to lead to failed assertion in the socket
    // transport code. Here, we make sure such an attempt produces an
    // exception which can be handled normally.
    rsb::Factory &factory = rsb::getFactory();
    try {
        rsb::ParticipantConfig config = factory.getDefaultParticipantConfig();
        rsc::runtime::Properties options;
        options.set<string>("port",   "12345");
        options.set<string>("server", "0");
        config.mutableTransport("socket").setOptions(options);
        factory.createListener("/dummy", config);
    }
    catch(const exception& e) {
        cout << "caught exception at toplevel: " << e.what() << endl;
    }

    return EXIT_SUCCESS;
}
