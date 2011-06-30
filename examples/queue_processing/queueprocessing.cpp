/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <iostream>

#include <stdlib.h>

#include <rsc/threading/SynchronizedQueue.h>

#include <rsb/Factory.h>
#include <rsb/Informer.h>
#include <rsb/Listener.h>
#include <rsb/QueuePushHandler.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;

int main(int argc, char **argv) {

    // first get a factory instance that is used to create RSB domain objects
    Factory &factory = Factory::getInstance();

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
    while (true) {

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
