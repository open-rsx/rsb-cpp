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

#include <string>

#include <rsc/runtime/Properties.h>

#include "../InPullConnector.h"
#include "../ConverterSelectingConnector.h"
#include "SpreadConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * This class implements pull-style event receiving for the Spread
 * transport.
 *
 * @author jmoringe
 */
class RSB_EXPORT InPullConnector: public transport::InPullConnector,
                                  public transport::ConverterSelectingConnector<std::string> {
public:
    InPullConnector(ConverterSelectionStrategyPtr converters,
                    const std::string &host = defaultHost(),
                    unsigned int port = defaultPort());
    virtual ~InPullConnector();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void setScope(const Scope &scope);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    EventPtr emit(bool block);

    static transport::InPullConnector *create(const rsc::runtime::Properties &args);
private:
    rsc::logging::LoggerPtr logger;

    bool active;

    SpreadConnectorPtr connector;
    boost::shared_ptr<Scope> activationScope;
};

}
}
