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
#include <rsc/threading/TaskExecutor.h>

#include "../../filter/ScopeFilter.h"
#include "../InPushConnector.h"
#include "../ConverterSelectingConnector.h"
#include "SpreadConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * @author jmoringe
 */
class RSB_EXPORT InConnector: public transport::InPushConnector,
                              public transport::ConverterSelectingConnector<std::string> {
    friend class ReceiverTask;
public:
    InConnector(converter::ConverterSelectionStrategy<std::string>::Ptr converters,
                const std::string &host = defaultHost(),
                unsigned int port = defaultPort());
    virtual ~InConnector();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void setScope(const Scope &scope);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    void addHandler(eventprocessing::HandlerPtr handler);
    void removeHandler(eventprocessing::HandlerPtr handler);

    static transport::InPushConnector *create(
            const rsc::runtime::Properties &args);
private:
    rsc::logging::LoggerPtr logger;

    bool active;

    SpreadConnectorPtr connector;
    boost::shared_ptr<Scope> activationScope;

    rsc::threading::TaskExecutorPtr exec;
    //   boost::shared_ptr<StatusTask> st;
    boost::shared_ptr<ReceiverTask> rec;
};

}
}
