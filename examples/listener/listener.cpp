/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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
#include <math.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/timer.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Listener.h>
#include <rsb/Subscription.h>
#include <rsb/Handler.h>
#include <rsb/filter/ScopeFilter.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;

void printData(boost::shared_ptr<string> e) {
    cout << *e << endl;
}

int main(int argc, char **argv) {

    Factory &factory = Factory::getInstance();

    LoggerPtr l = Logger::getLogger("receiver");

    boost::timer t;

    Scope scope;
    if (argc > 1) {
        scope = Scope(argv[1]);
    } else {
        scope = Scope("/example/informer");
    }
    ListenerPtr s = factory.createListener(scope);
    s->addHandler(HandlerPtr(new DataFunctionHandler<string> (&printData)));

    cerr << "Listener setup finished. Waiting for messages on scope " << scope
            << endl;

    while (true) {
        boost::this_thread::sleep(boost::posix_time::seconds(1000));
    }

    return (EXIT_SUCCESS);
}
