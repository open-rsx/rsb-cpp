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

#include "../OutConnector.h"
#include "../ConverterSelectingConnector.h"
#include "SpreadConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * @author jmoringe
 */
class RSB_EXPORT OutConnector: public transport::OutConnector,
        public rsb::transport::ConverterSelectingConnector<std::string> {
public:
    OutConnector(ConverterSelectionStrategyPtr converters,
            const std::string& host = defaultHost(), unsigned int port =
                    defaultPort(), unsigned int maxFragmentSize = 100000);
    virtual ~OutConnector();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void handle(rsb::EventPtr e);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    static rsb::transport::OutConnector *create(
            const rsc::runtime::Properties &args);
private:

    void fillMandatoryNotificationFields(protocol::Notification &notification,
            const EventPtr &event);

    /**
     * Fills a protocol::Notification with header contents.
     *
     * @param notification notification to fill
     * @param event the event that generated the notification
     * @param wireSchema wire schema of the serialized data
     */
    void fillNotificationHeader(protocol::Notification &notification,
            const EventPtr &event, const std::string &wireSchema);

    void fillEventId(protocol::EventId &id, const EventId &realId);

    rsc::logging::LoggerPtr logger;
    bool active;
    SpreadConnectorPtr connector;
    unsigned int maxFragmentSize;
    /**
     * The number of bytes minimally required to successfully serialize the
     * notification with the limited size for each fragment.
     */
    unsigned int minDataSpace;

};

}
}
