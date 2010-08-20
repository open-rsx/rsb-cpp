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

#include "../RSBEvent.h"
#include "../Action.h"
#include "../filter/FilterObserver.h"
#include "../util/UUID.h"

#include <map>
#include <boost/shared_ptr.hpp>

namespace rsb {

namespace transport {

// TODO extract converter map into its own class
// TODO put specific converter into each specific port, otherwise casting needed
// TODO converters need to be specific for
//      different transports
typedef boost::shared_ptr<std::map< std::string, boost::shared_ptr<void> > > ConverterMapPtr;
typedef boost::shared_ptr<std::map< void *, rsb::Action > > ActionListPtr;

class Port : public rsb::filter::FilterObserver {
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

protected:
	rsb::util::UUID id;
	rsb::Action observer;
	//ActionListPtr actions;
	ConverterMapPtr converters;

};

typedef boost::shared_ptr<Port> PortPtr;

}

}

#endif /* PORT_H_ */
