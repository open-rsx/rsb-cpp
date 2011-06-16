/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <rsb/Version.h>
#include <rsb/Factory.h>
#include <rsb/transport/Factory.h>
#include <rsb/converter/Repository.h>

using namespace std;
using namespace rsb;

int main(int argc, char **argv) {
    bool verbose = false;
    if (argc == 2 && string(argv[1]) == "--verbose") {
        verbose = true;
    }

    cout << "Version: " << Version::string() << ", build "
            << Version::buildString() << ", abi " << Version::abi() << endl;

    if (verbose) {
        rsb::Factory::getInstance();

        cout << endl;

        cout << endl << "Connectors" << endl;

        cout << rsb::transport::InFactory::getInstance() << endl;
        cout << rsb::transport::OutFactory::getInstance() << endl;

        cout << endl << "Converters" << endl;

        cout << *rsb::converter::stringConverterRepository() << endl;
    }

    return EXIT_SUCCESS;

}
