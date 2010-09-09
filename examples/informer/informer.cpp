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

#include <stdlib.h>
#include <iostream>
#include <rsc/logging/Logger.h>
//#include <rsc/logging/propertyconfigurator.h>
#include <math.h>

#include <rsb/Publisher.h>
#include <rsc/misc/Timer.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::util;

int main(void) {

//    std::ostringstream confpath;
//    char *log4cxxPropsEnv = getenv("LOG4CXXPROPS");
//
//    if (log4cxxPropsEnv != NULL) {
//
//        confpath << log4cxxPropsEnv;
//        cout << "Trying log4cxx configuration from file " << confpath.str()
//                << endl;
//
//        try {
//            log4cxx::PropertyConfigurator::configure(confpath.str());
//        } catch (const std::exception& e) {
//            cout << "Trying log4cxx configuration from file " << confpath.str()
//                    << " failed. Using BasicConfigurator." << endl;
//        }
//    }

    LoggerPtr l = Logger::getLogger("informer");

    Publisher<string>::Ptr informer(new Publisher<string>("rsb://example/informer","string"));
    Publisher<string>::DataPtr s(new string("blub"));

    TimerPtr t(new Timer("prototype"));
    t->start();

    for (int j = 0; j < 1200; j++) {
            informer->publish(s);
    }

    t->stop();
    cout << "Elapsed time for " << 1200 << " messages sent: " << t->getElapsed() << " ms" << endl;

    return (EXIT_SUCCESS);
}

