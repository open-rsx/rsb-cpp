/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <string>

#include <boost/shared_ptr.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/patterns/Singleton.h>

#include "rsb/rsbexports.h"
#include "Informer.h"
#include "Listener.h"
#include "patterns/Server.h"
#include "patterns/RemoteServer.h"

namespace rsb {

/**
 * Factory for RSB user-level domain objects for communication patterns.
 *
 * @author jwienke
 * @todo integrate model and validate requests against it
 * @todo port type selection missing
 */
class RSB_EXPORT Factory: public rsc::patterns::Singleton<Factory> {
public:

	virtual ~Factory();

	template<class DataType>
	typename Informer<DataType>::Ptr createInformer(const std::string &uri,
			const std::string &dataType = rsc::runtime::typeName<DataType>()) {
		return typename Informer<DataType>::Ptr(new Informer<DataType> (uri,
				dataType));
	}

	ListenerPtr createListener(const std::string &uri);

	patterns::ServerPtr createServer(const std::string &uri);

	patterns::RemoteServerPtr createRemoteServer(const std::string &uri);

	friend class rsc::patterns::Singleton<Factory>;

private:

	/**
	 * Singleton constructor.
	 */
	Factory();

};

}
