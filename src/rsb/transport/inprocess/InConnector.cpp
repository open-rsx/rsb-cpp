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

#include "InConnector.h"

#include "Bus.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

InConnector::InConnector() :
    logger(Logger::getLogger("rsb.inprocess.InConnector")), active(false) {
}

rsb::transport::InConnector* InConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.InConnector");
    RSCDEBUG(logger, "Creating InConnector with properties " << args);

    if (args.has("converters")) {
      RSCWARN(logger, "`converters' property found when constructing inprocess::InConnector. This connector does not support (or require) converters.");
    }
    return new InConnector();
}

InConnector::~InConnector() {
    if (this->active) {
        deactivate();
    }
}

string InConnector::getClassName() const {
    return "InConnector";
}

void InConnector::printContents(ostream &stream) const {
    stream << "scope = " << scope;
}

Scope InConnector::getScope() const {
    return this->scope;
}

void InConnector::setScope(const Scope& scope) {
    if (this->active)
        throw std::runtime_error("Cannot set scope while active");

    this->scope = scope;
}

void InConnector::activate() {
    RSCDEBUG(logger, "Activating");
    Bus::getInstance().addSink(boost::dynamic_pointer_cast<InConnector>(
            shared_from_this()));
    this->active = true;
}

void InConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");
    Bus::getInstance().removeSink(this);
}

void InConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {
}

void InConnector::handle(EventPtr event) {

    for (HandlerList::iterator it = this->handlers.begin(); it
            != this->handlers.end(); ++it) {
        (*it)->handle(event);
    }
}

}
}
