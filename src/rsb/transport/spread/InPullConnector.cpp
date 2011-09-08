/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "InPullConnector.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::eventprocessing;
using namespace rsb::converter;

namespace rsb {
namespace spread {

transport::InPullConnector *InPullConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.spread.InPullConnector");
    RSCDEBUG(logger, "creating InPullConnector with properties " << args);

    return new InPullConnector(args.get<ConverterSelectionStrategyPtr> ("converters"),
                               args.get<string>                        ("host", defaultHost()),
                               args.getAs<unsigned int>                ("port", defaultPort()));
}

InPullConnector::InPullConnector(ConverterSelectionStrategyPtr  converters,
                                 const string		       &host,
                                 unsigned int                   port):
    transport::ConverterSelectingConnector<string>(converters), logger(
            Logger::getLogger("rsb.spread.InPullConnector")), active(false),
            connector(new SpreadConnector(host, port)) {
}

InPullConnector::~InPullConnector() {
    if (this->active) {
        deactivate();
    }
}

string InPullConnector::getClassName() const {
    return "InPullConnector";
}

void InPullConnector::printContents(ostream &stream) const {
    stream << "active = " << this->active
           << ", connector = " << this->connector;
}

void InPullConnector::activate() {
    this->connector->activate();
    this->active = true;

    // check that scope is applied
    if (activationScope) {
        setScope(*activationScope);
        activationScope.reset();
    }

}

void InPullConnector::deactivate() {
    this->connector->deactivate();
}

void InPullConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
    this->connector->setQualityOfServiceSpecs(specs);
}

void InPullConnector::setScope(const Scope &scope) {
    if (!active) {
        this->activationScope.reset(new Scope(scope));
    } else {
        this->connector->join(this->connector->makeGroupName(scope));
    }
}

EventPtr InPullConnector::raiseEvent(bool block) {
    SpreadMessagePtr message(new SpreadMessage());
    if (block) {
        this->connector->receive(message);
    }
    // TODO implement
    return EventPtr();
}

}
}
