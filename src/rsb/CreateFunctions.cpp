/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "CreateFunctions.h"

using namespace std;

namespace rsb {

InformerBasePtr crateInformerBase(const Scope&             scope,
                                  const string&            dataType,
                                  const ParticipantConfig& config) {
    return Factory::getInstance().createInformerBase(scope, dataType, config);
}

ListenerPtr createListener(const Scope&             scope,
                           const ParticipantConfig& config) {
    return Factory::getInstance().createListener(scope, config);
}

ReaderPtr createReader(const Scope&             scope,
                       const ParticipantConfig& config) {
    return Factory::getInstance().createReader(scope, config);
}

ServicePtr createService(const Scope& scope) {
    return Factory::getInstance().createService(scope);
}

patterns::ServerPtr createServer(const Scope& scope) {
    return Factory::getInstance().createServer(scope);
}

patterns::RemoteServerPtr createRemoteServer(const Scope& scope) {
    return Factory::getInstance().createRemoteServer(scope);
}

}
