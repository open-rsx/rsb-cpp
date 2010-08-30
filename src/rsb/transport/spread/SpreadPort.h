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

#ifndef SPREADPORT_H_
#define SPREADPORT_H_

#include "../Port.h"
#include "../../filter/ScopeFilter.h"
#include "../../util/TaskExecutor.h"
#include "../QueueAndDispatchTask.h"
#include "ReceiverTask.h"
#include "MembershipManager.h"
#include "SpreadConnection.h"

#include <rsc/logging/Logger.h>

namespace rsb {

namespace spread {

class SpreadPort: public rsb::transport::Port {
public:
	SpreadPort();
	virtual ~SpreadPort();

	void push(rsb::RSBEventPtr e);

	void activate();
	void deactivate();

	void setObserver(rsb::Action a);
	void removeObserver(rsb::Action a);

	void notify(rsb::filter::ScopeFilter* f, rsb::filter::FilterAction::Types at);

private:
	rsc::logging::LoggerPtr logger;

	volatile bool shutdown;
	SpreadConnectionPtr con;

	rsb::util::TaskExecutorVoidPtr exec;
 //   boost::shared_ptr<StatusTask> st;
	boost::shared_ptr<ReceiverTask> rec;
    boost::shared_ptr<rsb::transport::QueueAndDispatchTask<RSBEventPtr> > qad;

    rsb::util::TaskPtr recTask;
	rsb::util::TaskPtr qadTask;
//	rsb::util::TaskPtr staTask;

	MembershipManagerPtr memberships;
};

}

}

#endif /* SPREADPORT_H_ */
