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

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread.hpp>

#include <rsc/runtime/Properties.h>
#include <rsc/threading/TaskExecutor.h>

#include "../Connector.h"
#include "StatusTask.h"

namespace rsb {
namespace inprocess {

/**
 * @todo use logging instead of cout
 * @author swrede
 */
class InProcessConnector: public rsb::transport::InConnector,
			  public rsb::transport::OutConnector {
public:
	InProcessConnector();
	virtual ~InProcessConnector();

	void activate();
	void deactivate();
	void push(rsb::EventPtr e);

	void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

	static rsb::transport::Connector* create(const rsc::runtime::Properties& args);
private:
	volatile bool shutdown;

	rsc::threading::TaskExecutorPtr exec;
	boost::shared_ptr<StatusTask>	st;
};

}
}
