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

#include <rsb/Handler.h>
#include <rsb/Listener.h>
#include <rsb/Factory.h>

using namespace rsb;

void printData(boost::shared_ptr<std::string> e) {
    std::cout << "Received event: " << *e << std::endl;
}

int main(int argc, char** argv) {

    // First get a factory instance that is used to create RSB
    // objects.
    Factory& factory = getFactory();

    // Set up the scope to receive on either from the command line
    // argument or use the default scope of the informer example.
    Scope scope((argc > 1) ? argv[1] : "/example/informer");

    // Create a listener that asynchronously receives events from the
    // bus and dispatches them to registered handlers.
    ListenerPtr listener = factory.createListener(scope);

    // Add a handler that is notified about every new event.  This
    // time a special handler instance is used that wraps a function
    // pointer of a function that is only interested in the received
    // data contained in the event and not the additional meta data
    // provided by the event instance. Other handlers exist that also
    // receive Event instances, either as class instances or by
    // wrapping function pointers.
    listener->addHandler(HandlerPtr(new DataFunctionHandler<std::string> (&printData)));

    // As events are received asynchronously we have to wait here for
    // them.
    while (true) {
        boost::this_thread::sleep(boost::posix_time::seconds(1000));
    }

    return EXIT_SUCCESS;
}
// mark-end::body
