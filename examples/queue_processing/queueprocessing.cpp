/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <iostream>

#include <stdlib.h>

#include <rsc/misc/SignalWaiter.h>

#include <rsc/threading/SynchronizedQueue.h>

#include <rsb/Factory.h>
#include <rsb/Informer.h>
#include <rsb/Listener.h>
#include <rsb/util/QueuePushHandler.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::util;

int main(int argc, char** argv) {

    rsc::misc::initSignalWaiter();

    // first get a factory instance that is used to create RSB domain objects
    Factory& factory = getFactory();

    // set up the scope to receive the required data for processing on, either
    // from the command line argument or use the default scope of the informer
    // example
    Scope inputScope;
    if (argc > 1) {
        inputScope = Scope(argv[1]);
    } else {
        inputScope = Scope("/example/informer");
    }
    // also setup a scope which is used to publish results from this component
    Scope outputScope = Scope("/processed").concat(inputScope);

    // create a listener that *listens* for incoming data on the scope specified
    // before
    ListenerPtr listener = factory.createListener(inputScope);

    // in this example we assume that the data type we receive is std::string

    // Listeners receive incoming data asynchronously each time another
    // component publishes new data. As we are working in a loop we need a way
    // to get the new data synchronized with the loop. For this purpose we use
    // a synchronized queue which is filled asynchronously from the listener
    // and will be read synchronously in our main loop.
    boost::shared_ptr<rsc::threading::SynchronizedQueue<boost::shared_ptr<
            string> > > queue(
            new rsc::threading::SynchronizedQueue<boost::shared_ptr<string> >);
    // we need to use a shared pointer holding the queue as we have to pass it
    // to another object that fills the queue asynchronously and the contents of
    // the queue are also managed by shared pointers, i.e.
    // boost::shared_ptr<string>
    // Note that we could have also initialized the queue with a number as the
    // constructor that indicates the size of the queue to receive only recent
    // data if processing in the main loop is too slow to handle all incoming
    // data. Old data would be removed from the queue automatically in this
    // case.

    // The final missing step is to connect the created queue with the listener.
    // This is accomplished by installing a special handler which is called by
    // the Listener each time new data is available. The handler fills the
    // queue with the new data.
    listener->addHandler(HandlerPtr(new QueuePushHandler<string> (queue)));

    // for being able to publish processing results (which means informing other
    // components in the system) we also need an RSB object that can inform
    Informer<string>::Ptr informer = factory.createInformer<string> (
            outputScope);

    // Now we start the (endless) main loop that processes the data
    while (rsc::misc::lastArrivedSignal() == rsc::misc::NO_SIGNAL) {

        // receive new data. This operation blocks until new data is available
        // if the queue was empty
        boost::shared_ptr<string> data = queue->pop();

        cout << "received: " << *data << endl;

        // do some heavy processing with the data to generate new results to
        // publish on RSB
        boost::shared_ptr<string> resultData(
                new string("heavily processed: " + *data));

        // and inform the world about the newly generated data by sending them
        // over the informer
        informer->publish(resultData);

    }

    return EXIT_SUCCESS;
}
