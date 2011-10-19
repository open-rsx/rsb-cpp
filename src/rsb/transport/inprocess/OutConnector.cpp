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

#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

OutConnector::OutConnector() :
    bus(Bus::getInstance()) {
}

rsb::transport::OutConnector* OutConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.OutConnector");
    RSCDEBUG(logger, "Creating OutConnector with properties " << args);

    // Seems to have confused some users.
    // See https://code.cor-lab.de/issues/649
    // if (args.has("converters")) {
    //     RSCWARN(logger, "`converters' property found when constructing inprocess::OutConnector. This connector does not support (or require) converters.");
    // }
    return new OutConnector();
}

string OutConnector::getClassName() const {
    return "OutConnector";
}

void OutConnector::activate() {
}

void OutConnector::deactivate() {
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {
}

void OutConnector::handle(EventPtr event) {
    event->mutableMetaData().setSendTime();
    this->bus.handle(event);
}

}
}
