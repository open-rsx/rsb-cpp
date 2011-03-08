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
#include <rsc/threading/RepetitiveTask.h>

#include "../ConverterCollection.h"
#include "../../RSBEvent.h"
#include "../QueueAndDispatchTask.h"
#include "../Port.h"
#include "../AbstractConverter.h"
#include "SpreadConnection.h"

namespace rsb {
namespace spread {

typedef boost::shared_ptr<
		rsb::transport::QueueAndDispatchTask<rsb::RSBEventPtr> > QADPtr;

/**
 * @author swrede
 */
class ReceiverTask: public rsc::threading::RepetitiveTask {
public:
	ReceiverTask(SpreadConnectionPtr s, transport::ConverterCollection<
			std::string>::Ptr converters, QADPtr q);
	virtual ~ReceiverTask();

	void execute();

private:
	rsc::logging::LoggerPtr logger;
	volatile bool cancelRequested;
	SpreadConnectionPtr con;
	transport::ConverterCollection<std::string>::Ptr converters;
	QADPtr qad;
};

}
}

#endif /* RECEIVERTASK_H_ */
