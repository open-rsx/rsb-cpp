/* ============================================================
 *
 * This file is part of the RSB project
 *
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

#include <iostream>

#include <boost/thread.hpp>

#include <rsb/Factory.h>

using namespace std;

using namespace boost;

using namespace rsb;

void printEvent(shared_ptr<string> data) {
    cout << "received " << *data << endl;
}

int main() {
    // Setup a listener. This listener uses the default configuration
    // which usually amounts to a sole connector for the spread
    // transport. This connector enables the listener to receive
    // events send by the inprocessAndSpread program.
    //
    // The expected output is:
    // "received foo"
    ListenerPtr listener = Factory::getInstance().createListener(
            Scope("/tutorial/transports"));
    listener->addHandler(
            HandlerPtr(new DataFunctionHandler<string> (&printEvent)));

    boost::this_thread::sleep(boost::posix_time::seconds(10));

    return EXIT_SUCCESS;
}
