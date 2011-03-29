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
#include <fstream>

#include <stdlib.h>
#include <math.h>
#include <ctime>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Subscriber.h>
#include <rsb/Subscription.h>
#include <rsb/Handler.h>
#include <rsb/filter/ScopeFilter.h>
#include <rsb/RSBFactory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;
using namespace boost::posix_time;

class MyDataHandler: public EventHandler {
public:
	MyDataHandler() {
    counter = 0;
	}
  
	void notify(RSBEventPtr e) {

    counter++;
    ptime receiveTime = microsec_clock::local_time();
    string file = e->getMetaInfo("file");
    string fileLoc = "/tmp/" + file;
    
    boost::shared_ptr<string> data = boost::static_pointer_cast<string>(e->getData());
    ptime sendTime = from_iso_string(e->getMetaInfo("startTime"));
    cout << "Received message [" << counter << "] - " << file << endl;
    cout << "... elapsed time between SEND -> RECEIVE: " << to_simple_string(receiveTime - sendTime) << endl;

    ofstream out(fileLoc.c_str(), ios::binary);
    for (it=data->begin();it<data->end();++it) { out << *it; }
    out.close();
	}
  
  long counter;
  string::iterator it;

};

int main(int argc, char **argv) {

	RSBFactory factory;

	LoggerPtr l = Logger::getLogger("receiver");

	boost::timer t;

	SubscriberPtr s = factory.createSubscriber("blub");
	SubscriptionPtr sub(new Subscription());
	string uri;
	if (argc > 1) {
		uri = argv[1];
	} else {
		uri = "rsb://example/informer";
	}
	sub->appendFilter(AbstractFilterPtr(new ScopeFilter(uri)));
  
	boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());
  
	// register event handler
	sub->appendHandler(dh);
	s->addSubscription(sub);
  
	cout << "Subscriber setup finished. Waiting for messages on uri " << uri
       << endl;
	while (true) {
		boost::this_thread::sleep(boost::posix_time::seconds(1000));
	}
  
	return (EXIT_SUCCESS);
}
