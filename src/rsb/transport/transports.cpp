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

#include "Factory.h"

#include "inprocess/InConnector.h"
#include "inprocess/OutConnector.h"

#include "spread/InConnector.h"
#include "spread/InPullConnector.h"
#include "spread/OutConnector.h"

#include "transports.h"

using namespace std;

namespace rsb {
namespace transport {

static bool registered = false;
static boost::mutex registrationMutex;

void registerDefaultTransports() {

    boost::mutex::scoped_lock lock(registrationMutex);
    if (!registered) {

        {
            InPushFactory &factory = InPushFactory::getInstance();

            factory.registerConnector("inprocess",
				      &rsb::inprocess::InConnector::create,
				      "inprocess");

	    std::set<std::string> options;
	    options.insert("host");
	    options.insert("port");
	    factory.registerConnector("spread",
				      &rsb::spread::InConnector::create,
				      "spread",
				      options);
        }

        {
            InPullFactory &factory = InPullFactory::getInstance();

            set<string> options;
            options.insert("host");
            options.insert("port");
            factory.registerConnector("spread",
                                      &spread::InPullConnector::create,
                                      "spread",
                                      options);
        }

        {
            OutFactory &factory = OutFactory::getInstance();
            factory.registerConnector("inprocess",
				      &rsb::inprocess::OutConnector::create,
				      "inprocess");

	    std::set<std::string> options;
	    options.insert("host");
	    options.insert("port");
            factory.registerConnector("spread",
				      &rsb::spread::OutConnector::create,
				      "spread",
				      options);
        }

        registered = true;
    }

}

}
}
