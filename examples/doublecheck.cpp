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

#include <rsb/Publisher.h>
#include <rsb/Subscriber.h>
#include <rsb/Subscription.h>

using namespace std;
using namespace rsb;
using namespace rsb::filter;

class PrintHandler: public DataHandler<string> {
private:
	string name;
public:
	PrintHandler(const string &name) :
		name(name) {
	}

	void notify(boost::shared_ptr<string> e) {
		cout << "[" << name << "] received: " << *e << endl;
	}
};

int main(int argc, char **argv) {

	for (int i = 0; i < 3; ++i) {

		string uri1 = "rsb;//test/dummy1";
		string uri2 = "rsb;//test/dummy2";

		Publisher<string> publisher1(uri1, "string");
		Publisher<string> publisher2(uri2, "string");

		SubscriberPtr subscriber1(new Subscriber(uri1));
		SubscriberPtr subscriber2(new Subscriber(uri2));

		SubscriptionPtr sub1(new Subscription);
		sub1->appendFilter(AbstractFilterPtr(new ScopeFilter(uri1)));
		sub1->appendHandler(HandlerPtr(new PrintHandler("sub1")));
		subscriber1->addSubscription(sub1);

		SubscriptionPtr sub2(new Subscription);
		sub2->appendFilter(AbstractFilterPtr(new ScopeFilter(uri2)));
		sub2->appendHandler(HandlerPtr(new PrintHandler("sub2")));
		subscriber2->addSubscription(sub2);

		publisher1.publish(Publisher<string>::DataPtr(new string(
				"publisher1 first message")));
		publisher1.publish(Publisher<string>::DataPtr(new string(
				"publisher1 second message")));

		publisher2.publish(Publisher<string>::DataPtr(new string(
				"publisher2 first message")));
		publisher2.publish(Publisher<string>::DataPtr(new string(
				"publisher2 second message")));

		sleep(1);

	}

	return 0;

}
