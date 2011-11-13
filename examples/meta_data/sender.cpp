/* ============================================================
 *
 * This file is part of the RSB project.
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

#include <string>

#include <boost/thread/thread.hpp>

#include <rsc/misc/langutils.h>

#include <rsb/Event.h>
#include <rsb/Factory.h>
#include <rsb/MetaData.h>

using namespace std;

using namespace rsb;

int main() {
    // Create an Informer object for string data for the channel of
    // this example.
    Informer<string>::Ptr informer
        = Factory::getInstance().createInformer<string>(Scope("/example/metadata"));

    // Create and send data in loop.
    while (true) {
        // We simulate some expensive data-creation process here, so
        // we get distinct timestamps at the start and end of the
        // processing.
        boost::uint64_t processingStart = rsc::misc::currentTimeMicros();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
	string data = "result of processing";
        boost::uint64_t processingDone = rsc::misc::currentTimeMicros();

        // We manually create a new Event object and add the desired
        // meta-data:
        // + processing-start timestamp
        // + processing-done timestamp
        // + creator meta-info item
	EventPtr event(new Event(Scope("/example/metadata"),
                                 Informer<string>::DataPtr(new string(data)),
                                 "std::string"));

        MetaData& metaData = event->mutableMetaData();
        metaData.setUserTime("processing-start", processingStart);
        metaData.setUserTime("processing-done",  processingDone);

        metaData.setUserInfo("creator", "example-sender-program");

        // When the data members of the Event object are populated, we
        // publish it using the Informer object.
        informer->publish(event);
    }

    return EXIT_SUCCESS;
}
