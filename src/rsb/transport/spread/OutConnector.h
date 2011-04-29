/* ============================================================
 *
 * This file is a part of the RSB project
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

#pragma once

#include <string>

#include <rsc/runtime/Properties.h>

#include "../Connector.h"
#include "SpreadConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * @author jmoringe
 */
class RSB_EXPORT OutConnector : public rsb::transport::OutConnector {
public:
        OutConnector(const std::string& host = defaultHost(),
                     unsigned int port = defaultPort());

        void push(rsb::EventPtr e);

        void activate();
        void deactivate();

        void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

        static rsb::transport::OutConnector* create(const rsc::runtime::Properties& args);
private:
	rsc::logging::LoggerPtr logger;

        SpreadConnectorPtr connector;
};

}
}
