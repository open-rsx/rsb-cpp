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

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/transport/Port.h"
#include "rsb/transport/spread/SpreadPort.h"
#include "rsb/filter/AbstractFilter.h"

#include "../../InformerTask.h"

#include "testhelpers.h"

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::filter;
using namespace rsb::transport;
using namespace rsb::spread;
using namespace testing;
using namespace rsc::threading;

TEST(SpreadPortTest, testConstruction)
{
	// ConverterRegistryPtr r = boost::shared_ptr<ConverterRegistry>();
	// AbstractConverter ac = boost::shared_ptr<AbstractConverter>(new UCharConverter());
	ASSERT_NO_THROW(PortPtr p = boost::shared_ptr<Port>(new rsb::spread::SpreadPort()));
	// p.setTypeConverters(r);
}

TEST(SpreadPortTest, testConnnection)
{
	// ConverterRegistryPtr r = boost::shared_ptr<ConverterRegistry>();
	// AbstractConverter ac = boost::shared_ptr<AbstractConverter>(new UCharConverter());
	PortPtr p = boost::shared_ptr<Port>(new rsb::spread::SpreadPort());
	ASSERT_NO_THROW(p->activate());
	// p.setTypeConverters(r);
}

TEST(SpreadPortTest, testRoundtrip)
{
	// task execution service
	TaskExecutorVoidPtr exec(new TaskExecutor<void> ());

	// in-process port
	PortPtr p(new SpreadPort());
	ASSERT_NO_THROW(p->activate());

	// filter for joining test group
	AbstractFilterPtr f = AbstractFilterPtr(new ScopeFilter("xcf://blah"));
	FilterObserverPtr fo = boost::static_pointer_cast<FilterObserver>(p);
	f->notifyObserver(fo,FilterAction::ADD);

	// domain objects
	boost::shared_ptr<InformerTask> source(new InformerTask(p));
	p->setObserver(boost::bind(&InformerTask::handler, source.get(), _1));

	// activate port and schedule informer
	p->activate();
	TaskPtr task_source = exec->schedulePeriodic<InformerTask> (source, 0);

	// wait *here* for shutdown as this is not known to the Port
	{
		boost::recursive_mutex::scoped_lock lock(source->m);
		while (source->c != 10) {
			source->cond.wait(lock);
		}
	}

	// stop and join all threads
	exec->join(task_source); // no cancel needed as done already locally, see execute()
	// port is deactivated through dtr
	cerr << "SpreadProcessTest finished" << endl;
}

int main(int argc, char* argv[]) {
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

	SpreadStarter spread;

	InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();

}
