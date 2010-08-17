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

#ifndef INFORMERTASK_H_
#define INFORMERTASK_H_

#include "transport/Port.h"
#include "util/Task.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>

namespace rsb {

namespace test {

class InformerTask {
public:
	InformerTask(rsb::transport::PortPtr p);
	virtual ~InformerTask();

	int c;
	boost::recursive_mutex m;
	boost::condition cond;

	void execute(rsb::util::Task<void>* t);
	void handler(rsb::RSBEventPtr e);

private:
	int i;
	rsb::transport::PortPtr port;

};

}

}

#endif /* INFORMERTASK_H_ */
