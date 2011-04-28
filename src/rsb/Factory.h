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
 */
class RSB_EXPORT Factory: public rsc::patterns::Singleton<Factory> {
public:

    virtual ~Factory();

    template<class DataType>
    typename Informer<DataType>::Ptr createInformer(
            const Scope &scope,
            const std::string &dataType = rsc::runtime::typeName(
                    typeid(DataType))) {
        return typename Informer<DataType>::Ptr(
                new Informer<DataType> (scope, dataType));
    }

    ListenerPtr createListener(const Scope &scope);

    patterns::ServerPtr createServer(const Scope &scope);

    patterns::RemoteServerPtr createRemoteServer(const Scope &scope);

    friend class rsc::patterns::Singleton<Factory>;

private:

    /**
     * Singleton constructor.
     */
    Factory();

};

}
