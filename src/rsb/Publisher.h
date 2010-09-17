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

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include "RSBEvent.h"
#include "transport/Router.h"
#include "transport/Port.h"

#include <string>
#include <rsc/logging/Logger.h>
#include <boost/shared_ptr.hpp>

namespace rsb {

/**
 * @todo check thread-safety, e.g. setting router to active and setting the
 *       passive flag must be atomic
 * @todo does it make sense that publishers are copyable?
 */
template<class T>
class Publisher {
public:
	typedef boost::shared_ptr<Publisher<T> > Ptr;
	typedef boost::shared_ptr<T> DataPtr;

	Publisher(std::string uri, std::string type) :
		logger(rsc::logging::Logger::getLogger("rsb.Publisher." + uri)), uri(uri),
				passive(false), defaultType(type) {
		// TODO evaluate configuration
		router = transport::RouterPtr(new transport::Router(
				transport::TransportFactory::NONE,
				transport::TransportFactory::SPREAD));
		activate();
	}

	Publisher(transport::TransportFactory::PortTypes out, std::string uri,
			std::string type) :
		logger(rsc::logging::Logger::getLogger("rsb.Publisher")), uri(uri),
				passive(false), defaultType(type) {
		// TODO evaluate configuration
		router = transport::RouterPtr(new transport::Router(
				transport::TransportFactory::NONE, out));
		activate();
	}
	virtual ~Publisher() {
	}
	;

	// publish data via RSB directly according to
	// publisher configuration
	void publish(boost::shared_ptr<T> data) {
		VoidPtr p = boost::static_pointer_cast<void>(data);
		publish(p, defaultType);
	}

	//	void publish(const T &data) {
	//		// TODO check how this could work
	//		boost::weak_ptr<T> w(data);
	//		VoidPtr v = boost::static_pointer_cast<void>(w);
	//		publish(v,defaultType);
	//	}

	//	template< class T1 >
	//	void publish(const T1 &data, std::string type) {
	//		// TODO check this!
	//		boost::weak_ptr<T1> w(data);
	//		VoidPtr v = boost::static_pointer_cast<void>(w);
	//		publish(v,type);
	//	}

	template<class T1>
	void publish(boost::shared_ptr<T1> data, std::string type) {
		VoidPtr p = boost::static_pointer_cast<void>(data);
		publish(p, type);
	}

	// if some metadata needs to be set externally
	// TODO assumption is that data and type field alredy set externally
	//      throw exception if not the case
	void publish(RSBEventPtr event) {
		// TODO Check that exception is thrown if no converter available!
		event->setURI(uri);
		RSCDEBUG(logger, "Publishing event");
		router->publish(event);
	}

	void activate() {
		router->activate();
		passive = false;
	}

	void deactivate() {
		if (!passive)
			router->deactivate();
		passive = true;
	}

protected:
	Publisher() { /* forbidden */
	}

	void publish(VoidPtr p, std::string type) {
		RSBEventPtr e(new RSBEvent());
		e->setData(p);
		e->setURI(uri);
		// TODO Check that exception is thrown if no converter available!
		e->setType(type);
		RSCDEBUG(logger, "Publishing event");
		router->publish(e);
	}

private:
	rsc::logging::LoggerPtr logger;
	std::string uri;
	volatile bool passive;
	std::string defaultType;
	rsb::transport::RouterPtr router;

};

}

#endif /* PUBLISHER_H_ */
