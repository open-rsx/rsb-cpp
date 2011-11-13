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

#include <boost/timer.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;

int main(void) {

    // first get a factory instance that is used to create RSB domain objects
    Factory& factory = Factory::getInstance();

    // create an informer that is capable of sending events containing any kind
    // of data on the scope "/example/informer".
    Informer<AnyType>::Ptr informer = factory.createInformer<AnyType> (
            Scope("/example/informer"));

    // create data to send over the informer. Data is always maintained in
    // shared_ptr instances. Here we use typed informer to create a shared_ptr
    // of the data type we want to send
    Informer<string>::DataPtr s(new string("blub"));

    boost::timer t;

    // really send the data several times
    for (int j = 0; j < 1200; j++) {
        informer->publish(s);
    }

    cout << "Elapsed time for " << 1200 << " messages sent: " << t.elapsed()
            << " s" << endl;

    return EXIT_SUCCESS;
}
