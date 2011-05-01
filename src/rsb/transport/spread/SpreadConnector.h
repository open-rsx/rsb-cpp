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
#include <map>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/misc/UUID.h>

#include "../../converter/Repository.h"
#include "../Connector.h"
#include "ReceiverTask.h"
#include "MembershipManager.h"
#include "SpreadConnection.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * @author swrede
 * @todo do we really want to expose this through dlls only for the unit tests
 */
class RSB_EXPORT SpreadConnector {
public:
    SpreadConnector(const std::string &host = defaultHost(),
                    unsigned int port = defaultPort(),
        rsb::converter::Repository<std::string>::Ptr converters =
                    converter::stringConverterRepository());

    virtual ~SpreadConnector();

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    void join(const std::string &name);
    void leave(const std::string &name);

    bool send(const SpreadMessage &msg);

    void init(const std::string& host, unsigned int port);

    SpreadConnectionPtr getConnection();

    rsb::converter::Repository<std::string>::Ptr getConverters();

    SpreadMessage::QOS getMessageQoS() const;

    std::string makeGroupName(const Scope &scope) const;

private:

    rsc::logging::LoggerPtr logger;

    rsc::misc::UUID id;

    volatile bool activated;
    SpreadConnectionPtr con;

    MembershipManagerPtr memberships;

    rsb::converter::Repository<std::string>::Ptr converters;

    /**
     * The message type applied to every outgoing message.
     */
    SpreadMessage::QOS messageQoS;

    typedef std::map<QualityOfServiceSpec::Ordering, std::map<
            QualityOfServiceSpec::Reliability, SpreadMessage::QOS> > QoSMap;

    /**
     * Map from 2D input space defined in QualitOfServiceSpec to 1D spread message
     * types. First dimension is ordering, second is reliability.
     */
    static const QoSMap qosMapping;

    static QoSMap buildQoSMapping();

};

typedef boost::shared_ptr<SpreadConnector> SpreadConnectorPtr;

}
}
