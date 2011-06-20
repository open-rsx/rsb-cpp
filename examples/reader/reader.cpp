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

#include <stdlib.h>
#include <math.h>

#include <rsc/logging/Logger.h>
#include <rsc/logging/LoggerFactory.h>

#include <rsb/Factory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;

int main(int argc, char **argv) {
    LoggerFactory::getInstance()->reconfigure(Logger::TRACE);
    LoggerPtr logger = Logger::getLogger("reader");

    Scope scope;
    if (argc > 1) {
        scope = Scope(argv[1]);
    } else {
        scope = Scope("/example/informer");
    }

    Factory &factory = Factory::getInstance();
    ReaderPtr reader = factory.createReader(scope);
    RSCINFO(logger, "Reader setup finished. Waiting for messages on scope " << scope);

    while (true) {
        EventPtr event = reader->read();
	RSCINFO(logger, "Received event " << event);
    }

    return EXIT_SUCCESS;
}
