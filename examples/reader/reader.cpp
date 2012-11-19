/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <iostream>

#include <rsb/Factory.h>

using namespace rsb;

int main(int argc, char** argv) {
    // Set up the scope to receive on either from the command line
    // argument or use the default scope of the informer example.
    Scope scope(argc > 1 ? argv[1] : "/example/informer");

    // Create a reader which synchronously receives events on the
    // specified scope.
    Factory& factory = getFactory();
    ReaderPtr reader = factory.createReader(scope);

    // Print events as they are received.
    while (true) {
        EventPtr event = reader->read();
        std::cout << event << std::endl;
    }

    return EXIT_SUCCESS;
}
// mark-end::body
