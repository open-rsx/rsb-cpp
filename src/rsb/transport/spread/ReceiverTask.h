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

#ifndef RECEIVERTASK_H_
#define RECEIVERTASK_H_

#include <rsc/logging/Logger.h>
#include <rsc/misc/Registry.h>
#include <rsc/threading/Task.h>

#include "../../RSBEvent.h"
#include "../QueueAndDispatchTask.h"
#include "../Port.h"
#include "../AbstractConverter.h"

namespace rsb {
namespace spread {

typedef boost::shared_ptr<
		rsb::transport::QueueAndDispatchTask<rsb::RSBEventPtr> > QADPtr;

// use a forward declaration because the spread header, which is available
// through SpreadConnection.h makes the protocol buffers code unusable by
// defining something in global scope...
class SpreadConnection;
typedef boost::shared_ptr<SpreadConnection> SpreadConnectionPtr;

/**
 * @todo remove header implementations
 */
class ReceiverTask {
public:
	ReceiverTask(SpreadConnectionPtr s, rsc::misc::Registry<
			rsb::transport::AbstractConverter<std::string> > *converters,
			QADPtr q);
	virtual ~ReceiverTask();

	void execute(rsc::threading::Task<void>* t);

	void cancel() {
		cancelRequested = true;
	}

private:
	rsc::logging::LoggerPtr logger;
	volatile bool cancelRequested;
	SpreadConnectionPtr con;
	rsc::misc::Registry<rsb::transport::AbstractConverter<std::string> >
			*converters;
	QADPtr qad;
};

}

}

#endif /* RECEIVERTASK_H_ */
