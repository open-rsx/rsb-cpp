/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <boost/shared_ptr.hpp>

#include <rsc/threading/SynchronizedQueue.h>

#include "Handler.h"

namespace rsb {

/**
 * A DataHandler for RSB Subscriptions that pushes all received data on a
 * rsc::SynchronizedQueue. This queue must handle shared pointers of the data
 * type.
 *
 * @author jwienke
 * @tparam T data type received by the handler. All data is handeled as a shared
 *           pointer of this type
 */
template<class T>
class QueuePushHandler: public DataHandler<T> {
private:
	boost::shared_ptr<rsc::threading::SynchronizedQueue<boost::shared_ptr<T> > >
			queue;
public:

	/**
	 * Constructs a new instance.
	 *
	 * @param queue the queue to push received data on
	 */
	QueuePushHandler(boost::shared_ptr<rsc::threading::SynchronizedQueue<
			boost::shared_ptr<T> > > queue) :
		queue(queue) {
	}

	void notify(boost::shared_ptr<T> e) {
		queue->push(e);
	}
};

}

