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

#include <ostream>

#include "rsb/rsbexports.h"

namespace rsb {

/**
 * Specification of desired quality of service settings for sending and
 * receiving events. Specification given here are required "at least". This
 * means concrete port instances can implement "better" QoS specs without any
 * notification to the clients. Better is decided by the integer value of the
 * specification enums. Higher values mean better services.
 *
 * @author jwienke
 */
class RSB_EXPORT QualityOfServiceSpec {
public:

	/**
	 * Specifies the ordering of events for subscribers on a publisher.
	 *
	 * @author jwienke
	 */
	enum Ordering {
		/**
		 * The events are delivered in arbitrary order.
		 */
		UNORDERED = 10,
		/**
		 * Every subscriber receives the events of one publisher in the order
		 * the publisher sent the events. No guarantees are given for events of
		 * multiple publishers.
		 */
		ORDERED = 20
	};

	/**
	 * Specifies the reliability of messages.
	 *
	 * @author jwienke
	 */
	enum Reliability {
		/**
		 * Events may be dropped and not be visible to a subscriber.
		 */
		UNRELIABLE = 10,
		/**
		 * Messages are guaranteed to be delivered. Otherwise an error is
		 * raised.
		 */
		RELIABLE = 20
	};

	/**
	 * Constructs the default QoS specs for every publishers. Messages are
	 * unordered but reliably.
	 */
	QualityOfServiceSpec();

	/**
	 * Constructs a new QoS specification with desired details.
	 *
	 * @param ordering desired ordering type
	 * @param reliability desired reliability type
	 */
	QualityOfServiceSpec(Ordering ordering, Reliability reliability);

	/**
	 * Destructor.
	 */
	virtual ~QualityOfServiceSpec();

	/**
	 * Returns the desired ordering settings.
	 *
	 * @return ordering requirements
	 */
	Ordering getOrdering() const;

	/**
	 * Returns the desired reliability settings.
	 *
	 * @return reliability requirements
	 */
	Reliability getReliability() const;

	friend RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const QualityOfServiceSpec &spec);

private:

	Ordering ordering;
	Reliability reliability;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const QualityOfServiceSpec &spec);

}

