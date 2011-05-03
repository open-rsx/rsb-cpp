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

#include <boost/shared_ptr.hpp>

#include <rsc/runtime/Printable.h>

#include "../QualityOfServiceSpec.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

/**
 * @author swrede
 */
class RSB_EXPORT Connector: public virtual rsc::runtime::Printable {
public:
    virtual ~Connector();

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    /**
     * Requests new QoS settings for publishing events. Does not
     * influence the receiving part.
     *
     * @param specs QoS specification
     * @throw UnsupportedQualityOfServiceException requirements cannot be met
     */
    virtual void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) = 0;
};

typedef boost::shared_ptr<Connector> ConnectorPtr;

}
}
