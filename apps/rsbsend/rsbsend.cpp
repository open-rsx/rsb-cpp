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
#include <sstream>
#include <fstream>

#include <stdlib.h>
#include <math.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsb;

int main(int argc, char** argv) {

    Factory& factory = Factory::getInstance();

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " [scope] [file with contents]" << endl;
        return EXIT_FAILURE;
    }

    // open file
    ifstream in;
    in.open(argv[2]);

    if (!in.is_open()) {
        cerr << "Unable to open " << argv[2] << endl;
        return EXIT_FAILURE;
    }

    stringstream contents;
    while (in.good()) {
        string line;
        getline(in, line);
        contents << line;
    }
    in.close();

    // publish
    Informer<string>::Ptr informer = factory.createInformer<string> (
            Scope(argv[1]));
    informer->publish(boost::shared_ptr<string>(new string(contents.str())));

    return EXIT_SUCCESS;
}

