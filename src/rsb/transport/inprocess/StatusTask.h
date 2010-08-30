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

#ifndef STATUSTASK_H_
#define STATUSTASK_H_

#include "../../util/Task.h"

namespace rsb {

namespace inprocess {

class InProcessPort;
typedef boost::shared_ptr<InProcessPort> InProcessPortPtr;

class StatusTask {
public:
	StatusTask(InProcessPort* p) : port(p) {};
	~StatusTask() {
		std::cout << "dtr ~StatusTask() " << std::endl;
	}

	boost::shared_ptr<void> execute(rsb::util::Task<void>* t) {
		std::cout << "InProcessPort alive. Status: undefined" << std::endl;
		return boost::shared_ptr<void>();
	}

private:
	// TODO how to replace this ptr by some kind of a smart ptr
	InProcessPort* port;
};

}

}

#endif /* STATUSTASK_H_ */
