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

#include "OutConnector.h"

#include "Bus.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport {
namespace socket {

transport::OutConnector *OutConnector::create(const Properties &args) {
    LoggerPtr logger = Logger::getLogger("rsb.transport.socket.OutConnector");
    RSCDEBUG(logger, "Creating OutConnector with properties " << args);

    return new OutConnector(args.get<ConverterSelectionStrategyPtr>("converters"),
                            args.get<string>                       ("host",   "localhost"),
                            args.getAs<unsigned int>               ("port",   9999),
                            args.getAs<bool>                       ("server", false));
}

OutConnector::OutConnector(ConverterSelectionStrategyPtr  converters,
                           const string                  &host,
                           unsigned int                   port,
                           bool                           server) :
    ConnectorBase(converters, host, port, server),
    logger(Logger::getLogger("rsb.transport.socket.OutConnector")){
}

void OutConnector::activate() {
    // nothing to do
}

void OutConnector::deactivate() {
    // nothing to do
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {
    RSCWARN(logger, "Quality of service not implemented");
}

void OutConnector::handle(EventPtr event) {
    event->mutableMetaData().setSendTime();

    EventPtr busEvent(new Event(*event));
    boost::shared_ptr<string> wireData(new string());
    converter::AnnotatedData d(busEvent->getType(), busEvent->getData());
    string wireSchema = getConverter(busEvent->getType())->serialize(d, *wireData);
    busEvent->setData(wireData);
    busEvent->mutableMetaData().setUserInfo("rsb.wire-schema", wireSchema);

    getBus()->handle(busEvent);
}

}
}
}
