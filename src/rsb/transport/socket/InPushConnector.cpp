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

#include "InPushConnector.h"

#include "Factory.h"
#include "../../MetaData.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport {
namespace socket {

transport::InPushConnector* InPushConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.transport.socket.InPushConnector");
    RSCDEBUG(logger, "Creating InPushConnector with properties " << args);

    return new InPushConnector(args.get<ConverterSelectionStrategyPtr>("converters"),
                               args.get<string>                       ("host",   "localhost"),
                               args.getAs<unsigned int>               ("port",   9999),
                               args.get<string>                       ("server", "auto"));
                               args.getAs<bool>                       ("tcpnodelay", false));
}

InPushConnector::InPushConnector(ConverterSelectionStrategyPtr converters,
                                 const string&                 host,
                                 unsigned int                  port,
                                 const string&                 server,
                                 bool                          tcpnodelay) :
    ConnectorBase(converters, host, port, server, tcpnodelay),
    logger(Logger::getLogger("rsb.transport.socket.InPushConnector")),
    active(false) {
}

InPushConnector::~InPushConnector() {
    if (this->active) {
        deactivate();
    }
}

Scope InPushConnector::getScope() const {
    return this->scope;
}

void InPushConnector::setScope(const Scope& scope) {
    if (this->active)
        throw std::runtime_error("Cannot set scope while active");

    this->scope = scope;
}

void InPushConnector::activate() {
    ConnectorBase::activate();

    RSCDEBUG(logger, "Activating");

    getBus()->addSink(dynamic_pointer_cast<InPushConnector>(enable_shared_from_this<InConnector>::shared_from_this()));

    this->active = true;
}

void InPushConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    this->active = false;

    RSCDEBUG(logger, "Removing ourselves from sink list of bus " << getBus());
    getBus()->removeSink(this);

    ConnectorBase::deactivate();
}

void InPushConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
    RSCWARN(logger, "Quality of service not implemented");
}

void InPushConnector::handle(EventPtr busEvent) {
    // busEvent is an intermediate object. The deserialization of the
    // payload still has to be performed.
    EventPtr event(busEvent);

    event->mutableMetaData().setReceiveTime();

    // Extract the serialized data and wire-schema from the
    // intermediate event.
    boost::shared_ptr<string> wireData = static_pointer_cast<string>(event->getData());
    string wireSchema = event->getMetaData().getUserInfo("rsb.wire-schema");

    // Apply the configured converter.
    converter::AnnotatedData d
        = getConverter(wireSchema)->deserialize(wireSchema, *wireData);
    event->setData(d.second);
    event->setType(d.first);

    // Dispatch the final result to all handlers (typically a single
    // object implementing the EventReceivingStrategy interface).
    for (HandlerList::iterator it = this->handlers.begin(); it
            != this->handlers.end(); ++it) {
        (*it)->handle(event);
    }
}

void InPushConnector::printContents(ostream& stream) const {
    stream << "scope = " << scope;
}

}
}
}
