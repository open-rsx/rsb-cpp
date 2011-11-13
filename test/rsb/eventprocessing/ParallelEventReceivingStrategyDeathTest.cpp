/* ============================================================
 *
 * This file is a part of the RSB project.
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

#include <boost/thread.hpp>

#include "rsb/eventprocessing/ParallelEventReceivingStrategy.h"
#include "rsb/QueuePushHandler.h"
#include "rsb/filter/ScopeFilter.h"

using namespace std;
using namespace rsb;
using namespace rsb::eventprocessing;
using namespace rsc::threading;
using namespace rsb::filter;

using namespace std;

class ErrorGeneratingHandler: public rsb::Handler {
public:

    ErrorGeneratingHandler(const bool& catchAll) :
        catchAll(catchAll) {
    }

    virtual ~ErrorGeneratingHandler() {

    }

    string getClassName() const {
        return "ErrorGeneratingHandler";
    }

    void handle(EventPtr /*event*/) {
        if (catchAll) {
            throw "A random string";
        } else {
            throw runtime_error("A random error");
        }
    }

private:
    bool catchAll;

};

int main(int argc, char** argv) {

    if (argc != 2) {
        cerr << "expected argument" << endl;
        return EXIT_SUCCESS;
    }

    bool catchAll = false;
    if (string(argv[1]) == "exception") {
        catchAll = false;
    } else if (string(argv[1]) == "catchall") {
        catchAll = true;
    } else {
        cerr << "unexpected argument: " << argv[1] << endl;
        return EXIT_SUCCESS;
    }

    ParallelEventReceivingStrategy processor(1);
    rsb::HandlerPtr exceptionHandler(new ErrorGeneratingHandler(catchAll));
    processor.addHandler(exceptionHandler, true);
    processor.setHandlerErrorStrategy(ParticipantConfig::EXIT);

    EventPtr event(new Event);
    event->setData(boost::shared_ptr<string>(new string("hello")));
    processor.handle(event);

    boost::this_thread::sleep(boost::posix_time::milliseconds(10000));

    return EXIT_SUCCESS;

}

