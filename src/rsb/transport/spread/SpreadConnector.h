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

#include <map>

#include <rsc/runtime/Properties.h>
#include <rsc/logging/Logger.h>
#include <rsc/threading/TaskExecutor.h>

#include "../../converter/Repository.h"
#include "../Connector.h"
#include "../../filter/ScopeFilter.h"
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
class RSB_EXPORT SpreadConnector: public rsb::transport::Connector {
public:
        SpreadConnector(const std::string& host = defaultHost(),
                        unsigned int port = defaultPort());
	explicit SpreadConnector(
			rsb::converter::Repository<std::string>::Ptr converters);
	virtual ~SpreadConnector();

	void push(rsb::EventPtr e);

	void activate();
	void deactivate();

	void setObserver(HandlerPtr handler);

	void notify(rsb::filter::ScopeFilter* f,
			const rsb::filter::FilterAction::Types &at);

	void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

	static rsb::transport::Connector*
        create(const rsc::runtime::Properties& args);
private:
	rsc::logging::LoggerPtr logger;

	void init(const std::string& host, unsigned int port);

	volatile bool activated;
	SpreadConnectionPtr con;

	rsc::threading::TaskExecutorPtr exec;
	//   boost::shared_ptr<StatusTask> st;
	boost::shared_ptr<ReceiverTask> rec;

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

	HandlerPtr observer;
};

}

}
