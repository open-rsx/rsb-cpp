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

#include "socket/InPushConnector.h"
#include "socket/OutConnector.h"

#include "transports.h"

using namespace std;

namespace rsb {
namespace transport {

static bool registered = false;
static boost::mutex registrationMutex;

void registerDefaultTransports() {
    boost::mutex::scoped_lock lock(registrationMutex);

    if (registered) {
        return;
    }
    registered = true;

    // In-direction, push-style connectors
    {
        InPushFactory& factory = InPushFactory::getInstance();
        factory.registerConnector("inprocess",
                                  &inprocess::InConnector::create,
                                  "inprocess");

        {
            set<string> options;
            options.insert("host");
            options.insert("port");

            factory.registerConnector("spread",
                                      &spread::InConnector::create,
                                      "spread",
                                      options);
        }

        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("server");

            factory.registerConnector("socket",
                                      &socket::InPushConnector::create,
                                      "socket",
                                      options);
        }
    }

    // In-direction, pull-style connectors
    {
        InPullFactory& factory = InPullFactory::getInstance();

        {
            set<string> options;
            options.insert("host");
            options.insert("port");

            factory.registerConnector("spread",
                                      &spread::InPullConnector::create,
                                      "spread",
                                      options);
        }
    }

    // Out-direction connectors
    {
        OutFactory& factory = OutFactory::getInstance();
        factory.registerConnector("inprocess",
                                  &inprocess::OutConnector::create,
                                  "inprocess");

        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("maxfragmentsize");

            factory.registerConnector("spread",
                                      &spread::OutConnector::create,
                                      "spread",
                                      options);
        }

        {
            set<string> options;
            options.insert("host");
            options.insert("port");
            options.insert("server");

            factory.registerConnector("socket",
                                      &socket::OutConnector::create,
                                      "socket");
        }
    }

}

}
}
