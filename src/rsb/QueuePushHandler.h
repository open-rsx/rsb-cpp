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

#ifndef QUEUEPUSHHANDLER_H_
#define QUEUEPUSHHANDLER_H_

#include <boost/shared_ptr.hpp>

#include <rsc/threading/SynchronizedQueue.h>

#include "Handler.h"

namespace rsb {

template<class T>
class QueuePushHandler: public DataHandler<T> {
private:
	boost::shared_ptr<rsc::threading::SynchronizedQueue<boost::shared_ptr<T> > >
			queue;
public:
	QueuePushHandler(boost::shared_ptr<rsc::threading::SynchronizedQueue<
			boost::shared_ptr<T> > > queue) :
		queue(queue) {
	}

	void notify(boost::shared_ptr<T> e) {
		queue->push(e);
	}
};

}

#endif /* QUEUEPUSHHANDLER_H_ */
