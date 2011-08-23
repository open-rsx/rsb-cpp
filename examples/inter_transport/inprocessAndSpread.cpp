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

#include <iostream>

#include <boost/thread.hpp>

#include <rsb/Factory.h>

using namespace std;

using namespace boost;

using namespace rsb;

void printEvent(boost::shared_ptr<string> data) {
    cout << "received " << *data << endl;
}

int main() {
    // Adjust default configuration such that the inprocess and spread
    // transports are enabled.
    ParticipantConfig config =
            Factory::getInstance().getDefaultParticipantConfig();
    ParticipantConfig::Transport inprocess = config.getTransport("inprocess");
    inprocess.setEnabled(true);
    config.addTransport(inprocess);
    Factory::getInstance().setDefaultParticipantConfig(config);

    // Setup a listener. This listener uses the modified default
    // configuration above which amounts to one connector for the
    // inprocess transport and one connector for the spread
    // transport. These connectors cause the listener to receive all
    // events which are sent by the informer below twice.
    //
    // The expected output is therefore (modulo order of events):
    // "received foo     [via spread]
    //  received foo"    [via inprocess]
    ListenerPtr listener = Factory::getInstance().createListener(
            Scope("/tutorial/transports"));
    listener->addHandler(
            HandlerPtr(new DataFunctionHandler<string> (&printEvent)));

    // Setup an informer. This informer uses the same modified default
    // configuration as the listener above. It therefore broadcasts
    // events via one connector for the inprocess transport and one
    // connector for the spread transport. The spread connector
    // enables participants in other processes to receive the events.
    Informer<string>::Ptr informer = Factory::getInstance().createInformer<
            string> (Scope("/tutorial/transports"));

    boost::shared_ptr<string> data(new string("foo"));
    informer->publish(data);

    boost::this_thread::sleep(boost::posix_time::seconds(1));

    return EXIT_SUCCESS;
}
