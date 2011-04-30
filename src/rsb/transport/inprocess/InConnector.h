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

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Properties.h>

#include "../Connector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace inprocess {

/**
 * @author jmoringe
 */
class RSB_EXPORT InConnector: public rsb::transport::InConnector,
        public boost::enable_shared_from_this<InConnector> {
public:
    InConnector();
    virtual ~InConnector();

    Scope getScope() const;
    void setScope(const Scope& scope);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    void push(EventPtr e);

    static rsb::transport::InConnector *create(
            const rsc::runtime::Properties &args);
private:
    rsc::logging::LoggerPtr logger;

    Scope scope;

    bool active;
};

typedef boost::shared_ptr<InConnector> InConnectorPtr;

}
}
