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

#ifndef PORT_H_
#define PORT_H_

#include <map>

#include <boost/shared_ptr.hpp>

#include <rsc/misc/UUID.h>

#include "../RSBEvent.h"
#include "../Action.h"
#include "../filter/FilterObserver.h"
#include "../QualityOfServiceSpec.h"

namespace rsb {
namespace transport {

typedef boost::shared_ptr<std::map<void *, rsb::Action> > ActionListPtr;

/**
 * @author swrede
 */
class Port: public rsb::filter::FilterObserver {
public:
	Port();
	virtual ~Port();

	virtual void activate() = 0;
	virtual void deactivate() = 0;

	virtual void push(rsb::RSBEventPtr e) = 0;

	// Observer may implement complex event matching
	// or be just directly the user-level event handlers
	virtual void setObserver(rsb::Action a);
	virtual void removeObserver(rsb::Action a);

	/**
	 * Requests new QoS settings for publishing events. Does not influence the
	 * receiving part.
	 *
	 * @param specs QoS specification
	 * @throw UnsupportedQualityOfServiceException requirements cannot be met
	 */
	virtual void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) = 0;

protected:
	rsc::misc::UUID id;
	rsb::Action observer;
	//ActionListPtr actions;

};

typedef boost::shared_ptr<Port> PortPtr;

}
}

#endif /* PORT_H_ */
