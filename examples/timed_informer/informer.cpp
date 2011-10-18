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
#include <math.h>

#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <rsc/threading/PeriodicTask.h>
#include <rsc/threading/ThreadedTaskExecutor.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsc::misc;
using namespace rsb;
namespace po = boost::program_options;

class FloodingTask: public rsc::threading::PeriodicTask {
public:

    FloodingTask(Informer<string>::Ptr informer, const unsigned int &ms = 2) :
        rsc::threading::PeriodicTask(ms), iteration(1), informer(informer) {
    }

    virtual ~FloodingTask() {
    }

    virtual void execute() {

        string temp = boost::str(
                boost::format("this is iteration %d") % iteration++);
        boost::shared_ptr<string> data(new string(temp));
        informer->publish(data);

    }

private:

    unsigned long long iteration;
    Informer<string>::Ptr informer;

};

int main(int argc, char *argv[]) {

    po::options_description desc("Allowed options");
    desc.add_options()("int,i", po::value<unsigned int>()->required(),
            "interval in ms")("scope,s", po::value<string>()->required(),
            "destination scope")("help,h", "help message");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    po::notify(vm);

    unsigned int intervalMs = vm["int"].as<unsigned int> ();
    Scope scope = vm["scope"].as<string> ();

    cout << "sending on " << scope << " with interval " << intervalMs << endl;

    // ---

    Factory &factory = Factory::getInstance();

    Informer<string>::Ptr informer = factory.createInformer<string> (scope);

    rsc::threading::ThreadedTaskExecutor exec;

    exec.schedule(
            rsc::threading::TaskPtr(new FloodingTask(informer, intervalMs)));

    while (true) {
        boost::this_thread::sleep(boost::posix_time::seconds(1000));
    }

    return EXIT_SUCCESS;

}
