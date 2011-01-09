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

#include "rsb/Handler.h"
#include "rsb/Action.h"

#include <boost/bind.hpp>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace rsb;
using namespace testing;

class MyEventHandler: public EventHandler {
public:

	void notify(RSBEventPtr e) {
		cout << "Event received: " << e->getUUID().getIdAsString() << endl;
	}
};

class MyDataHandler: public DataHandler<string> {
public:

	void notify(boost::shared_ptr<string> e) {
		cout << "Data received: " << *e << endl;
	}
};

TEST(HandlerTest, testDispatch)
{
	EventHandler* eh = new MyEventHandler();
	DataHandler<string>* dh = new MyDataHandler();
	rsb::Action ea = boost::bind(&Handler::internal_notify, eh, _1);
	rsb::Action da = boost::bind(&Handler::internal_notify, dh, _1);
	RSBEventPtr e(new RSBEvent());
	e->setData(boost::shared_ptr<string>(new string("blub")));
	e->setURI("blah");
	// TODO Check that exception is thrown if no converter available!
	e->setType("string");
	ea(e);
	da(e);
}
