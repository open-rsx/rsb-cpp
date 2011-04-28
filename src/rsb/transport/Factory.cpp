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

#include "Factory.h"

#include <rsc/logging/Logger.h>

#include "../Factory.h"

#include "spread/SpreadConnector.h"
#include "inprocess/InProcessConnector.h"

using namespace rsc::logging;

// TODO wtf? Why no class member
namespace {
LoggerPtr logger(Logger::getLogger("rsb.transport.Factory"));
}

namespace rsb {
namespace transport {

ConnectorPtr Factory::createConnector(ConnectorTypes type) {
        // FIXME this forces init of RSB factory which triggers registration
        // of transport implementations.
        rsb::Factory::getInstance();

	ConnectorPtr p;
	//		ConverterRegistryPtr r = boost::shared_ptr<ConverterRegistry>();
	//		Converter ac = boost::shared_ptr<Converter>(new UCharConverter());
	//		r->addConverter("uchar.person",ac);
	switch (type) {
	case LOCAL:
		RSCINFO(logger, "TransportFactory instantiating new InProcessConnector")
		;
		p = boost::shared_ptr<Connector>(InFactory::getInstance().createInst("local"));
		break;
	case SPREAD:
		RSCINFO(logger, "TransportFactory instantiating new SpreadConnector")
		;
		p = boost::shared_ptr<Connector>(InFactory::getInstance().createInst("spread"));
		break;
	case NONE:
		RSCINFO(logger, "TransportFactory not instantiating any Connector implementation")
		;
		p = ConnectorPtr();
		break;
	default:
		RSCWARN(logger, "TransportType not supported by this TransportFactory implementation")
		;
		break;
	}
	//		p.setTypeConverters(r);
	return p; //, new btl::ucharConverter());
}

}

}
