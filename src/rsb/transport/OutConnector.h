/* ============================================================
 *
 * This file is part of the RSB project
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

#pragma once

#include <boost/shared_ptr.hpp>

#include "../eventprocessing/Handler.h"
#include "Connector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

/** Objects of classes which implement this interface can be used to
 * send events by means of one transport mechanism.
 *
 * @author jmoringe
 */
class RSB_EXPORT OutConnector : public Connector,
                                public eventprocessing::Handler {
};

typedef boost::shared_ptr<OutConnector> OutConnectorPtr;

}
}
