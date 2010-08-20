/*
 * producer.cpp
 *
 *  Created on: 28.07.2010
 *      Author: swrede
 */

#include <stdlib.h>
#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <math.h>

#include "transport/spread/SpreadConnection.h"
#include "transport/spread/SpreadMessage.h"
#include "util/Timer.h"

using namespace std;
using namespace log4cxx;
using namespace rsb::spread;
using namespace rsb::util;

int main(int argc, char** argv) {

    BasicConfigurator::configure();
    LoggerPtr l = Logger::getLogger("producer");

    SpreadConnectionPtr con(new SpreadConnection("producer"));
    con->activate();

//    SpreadGroupPtr sg_ins = con->join("insert");
//    SpreadGroupPtr sg_rem = con->join("remove");
//    SpreadGroupPtr sg_upd = con->join("update");
//    SpreadGroupPtr sg_que = con->join("query");

    SpreadMessage msg("Hello World!");
    msg.addGroup("insert");
    msg.addGroup("remove");
    msg.addGroup("update");
    msg.addGroup("query");
//    msg->addGroup("udpate");
//    msg->addGroup("query");

//    for (list<string>::const_iterator n = msg->getGroupsBegin(); n != msg->getGroupsEnd(); ++n) {
//        cout << *n << endl;
//    }

    TimerPtr t(new Timer("prototype"));
    t->start();

    for (int j = 0; j < 1200; j++) {
            con->send(msg);
    }

    t->stop();
    cout << "Elapsed time for " << con->getMsgCount() << " messages sent: " << t->getElapsed() << " ms" << endl;

//    con->leave(sg_ins);
//    con->leave(sg_rem);
//    con->leave(sg_upd);
//    con->leave(sg_que);

    return (EXIT_SUCCESS);
}

