/*
 * consumer.cpp
 *
 *  Created on: 28.07.2010
 *      Author: swrede
 */

#include <stdlib.h>

#include <iostream>
#include <log4cxx/logger.h>
//#include <log4cxx/stream.h>
#include <log4cxx/basicconfigurator.h>
#include <math.h>

#include "transport/spread/SpreadConnection.h"
#include "transport/spread/SpreadMessage.h"
#include "util/Timer.h"

using namespace std;
using namespace log4cxx;
using namespace rsb::spread;
using namespace rsb::util;

void printGroups(std::string s) { cout << s << endl; }

int main(int argc, char** argv) {

    BasicConfigurator::configure();
    LoggerPtr l = Logger::getLogger("receiver");
  //  logstream l_debug(l,Level::getDebug());

    SpreadConnectionPtr con(new SpreadConnection("receiver"));
    con->activate();

    SpreadGroupPtr sg_ins = con->join("insert");
    SpreadGroupPtr sg_rem = con->join("remove");
    SpreadGroupPtr sg_upd = con->join("update");
    SpreadGroupPtr sg_que = con->join("query");

    TimerPtr t(new Timer("protopype"));

    SpreadMessagePtr msg(new SpreadMessage());
    bool counting = false;
    long count = 0;
    do {
        if (count==0 && !counting) {
            cout << "Consumer setup finished. Waiting for messages..." << endl;
        }
        con->receive(msg);
        switch (msg->getType()) {
            case SpreadMessage::MEMBERSHIP:
                cout << "Received membership message." << endl;
                break;
            case SpreadMessage::REGULAR:
                if (!counting) {
                    cout << "Received first message, starting timer..." << endl;
                    counting = true;
                    t->start();
                } else {
                    count++;
                }
                break;
            case SpreadMessage::OTHER:
                cout << "Received unknown message. " << endl;
                break;
        }
//        l_debug << "received spread message nr " << count << ": " << msg->getDataAsString() << LOG4CXX_ENDMSG;
    } while (count < 1000);

    t->stop();

    cout << "Last message was sent to the following groups: " << endl;

    for_each(msg->getGroupsBegin(),msg->getGroupsEnd(),&printGroups);

    cout << "Elapsed time per message (" << count << " messages received): " << t->getElapsed()/count << " ms" << endl;

    return (EXIT_SUCCESS);
}
