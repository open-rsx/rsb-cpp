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

#include "strategies.h"

#include <boost/thread/mutex.hpp>

#include "EventReceivingStrategyFactory.h"
#include "DirectEventReceivingStrategy.h"
#include "ParallelEventReceivingStrategy.h"

#include "EventSendingStrategyFactory.h"
#include "DirectEventSendingStrategy.h"

namespace rsb {
namespace eventprocessing {

bool registered = false;
boost::mutex registrationMutex;

void registerDefaultEventProcessingStrategies() {
    boost::mutex::scoped_lock lock(registrationMutex);
    if (registered) {
        return;
    }
    registered = true;

    {
        EventReceivingStrategyFactory& factory =
                EventReceivingStrategyFactory::getInstance();

        factory.impls().register_("direct",
                &DirectEventReceivingStrategy::create);
        factory.impls().register_("parallel",
                &ParallelEventReceivingStrategy::create);
    }

    {
        EventSendingStrategyFactory& factory
            = EventSendingStrategyFactory::getInstance();

        factory.impls().register_("direct", &DirectEventSendingStrategy::create);
    }
}

}
}
