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

#include "RSBEvent.h"
#include "transport/TransportFactory.h"
#include "transport/Port.h"
#include "transport/spread/SpreadConnection.h"
#include "transport/spread/SpreadMessage.h"
#include "util/Timer.h"

using namespace std;
using namespace log4cxx;
using namespace rsb;
using namespace rsb::transport;
using namespace rsb::util;

int main(int argc, char** argv) {

    BasicConfigurator::configure();
    LoggerPtr l = Logger::getLogger("informer");

    PortPtr p = TransportFactory::createPort(TransportFactory::SPREAD);

    RSBEventPtr e(new RSBEvent("rsb://example/informer",boost::static_pointer_cast<void>(boost::shared_ptr<string>(new string("Hello!"))), "Hello World!"));

    TimerPtr t(new Timer("prototype"));
    t->start();

    for (int j = 0; j < 1200; j++) {
            p->push(e);
    }

    t->stop();
    cout << "Elapsed time for " << 1200 << " messages sent: " << t->getElapsed() << " ms" << endl;

    return (EXIT_SUCCESS);
}

