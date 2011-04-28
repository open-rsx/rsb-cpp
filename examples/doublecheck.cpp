/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <boost/thread.hpp>

#include <rsb/Informer.h>
#include <rsb/Listener.h>
#include <rsb/Subscription.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsb;
using namespace rsb::filter;

void printData(const std::string& name, boost::shared_ptr<string> e) {
    cout << "[" << name << "] received: " << *e << endl;
}

int main(int /*argc*/, char **/*argv*/) {

    Factory &factory = Factory::getInstance();

    for (int i = 0; i < 3; ++i) {

        Scope scope1("/test/dummy1");
        Scope scope2("/test/dummy2");

        Informer<string>::Ptr informer1 = factory.createInformer<string> (
                scope1);
        Informer<string>::Ptr informer2 = factory.createInformer<string> (
                scope2);

        ListenerPtr listener1 = factory.createListener(scope1);
        listener1->appendHandler(
                HandlerPtr(
                        new DataFunctionHandler<string> (
                                boost::bind(&printData, "sub1", _1))));

        ListenerPtr listener2 = factory.createListener(scope2);
        listener2->appendHandler(
                HandlerPtr(
                        new DataFunctionHandler<string> (
                                boost::bind(&printData, "sub2", _1))));

        informer1->publish(
                Informer<string>::DataPtr(new string("informer1 first message")));
        informer1->publish(
                Informer<string>::DataPtr(
                        new string("informer1 second message")));

        informer2->publish(
                Informer<string>::DataPtr(new string("informer2 first message")));
        informer2->publish(
                Informer<string>::DataPtr(
                        new string("informer2 second message")));

        boost::this_thread::sleep(boost::posix_time::seconds(2));

    }

    return EXIT_SUCCESS;

}
