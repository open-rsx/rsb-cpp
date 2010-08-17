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

#include "filter/AbstractFilter.h"
#include "filter/ScopeFilter.h"
#include "filter/FilterObserver.h"
#include "filter/FilterObservable.h"
#include "filter/FilterActionTypes.h"

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace rsb::filter;
using namespace testing;

class Observable : public FilterObservable {
public:
	void raise(AbstractFilterPtr f) {
		// p AbstractPort implements FilterObserver
	//		Sub s;
	//		ep->register(s);
	//		{
	//			for f in s.filter:
	//			  notifyObservers(f, ADD);
	//		}
		notifyObservers(f,FilterAction::ADD);
	}
};

class Observer : public FilterObserver {
public:
	void notify(AbstractFilter* f, FilterAction::Types at) {
		ASSERT_FALSE(1) << "notify AbstractFilter triggered";
	}

	void notify(ScopeFilter* sf, FilterAction::Types at) {
		ASSERT_TRUE(sf!=NULL);
		cerr << sf->getURI() << endl;
	}
};

TEST(FilterObserverTest, testDoubleDispatch)
{
	// ConverterRegistryPtr r = boost::shared_ptr<ConverterRegistry>();
	// AbstractConverter ac = boost::shared_ptr<AbstractConverter>(new UCharConverter());
	AbstractFilterPtr f = AbstractFilterPtr(new ScopeFilter("xcf://blah"));
	boost::shared_ptr<Observable> observable = boost::shared_ptr<Observable>(new Observable());
	boost::shared_ptr<Observer> observer = boost::shared_ptr<Observer>(new Observer());
	observable->addObserver(observer);
	ASSERT_NO_THROW(observable->raise(f));
	// p.setTypeConverters(r);
}

