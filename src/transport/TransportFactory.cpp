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

#include "TransportFactory.h"

#include <log4cxx/logger.h>

using namespace log4cxx;

namespace {
	LoggerPtr logger(Logger::getLogger("rsb.transport.TransportFactory"));
}

namespace rsb {

namespace transport {

TransportFactory::TransportFactory() {
	// TODO Auto-generated constructor stub

}

TransportFactory::~TransportFactory() {
	// TODO Auto-generated destructor stub
}

PortPtr TransportFactory::createPort(PortTypes type) {
	PortPtr p;
//		ConverterRegistryPtr r = boost::shared_ptr<ConverterRegistry>();
//		AbstractConverter ac = boost::shared_ptr<AbstractConverter>(new UCharConverter());
//		r->addConverter("uchar.person",ac);
	switch (type) {
		case LOCAL:
			LOG4CXX_INFO(logger, "TransportFactory instantiating new InProcessPort");
			p = boost::shared_ptr<Port>(new rsb::inprocess::InProcessPort());
			break;
		case SPREAD:
			LOG4CXX_INFO(logger, "TransportFactory instantiating new SpreadPort");
			p = boost::shared_ptr<Port>(new rsb::spread::SpreadPort());
			break;
		default:
			LOG4CXX_WARN(logger, "TransportType not supported by this TransportFactory implementation");
			break;
	}
//		p.setTypeConverters(r);
	return p; //, new btl::ucharConverter());
}

}

}
