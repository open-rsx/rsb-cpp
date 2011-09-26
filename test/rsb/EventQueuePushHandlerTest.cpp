/* ============================================================
 *
 * This file is a part of RSB project
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

#include <stdexcept>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/EventQueuePushHandler.h"

using namespace std;
using namespace testing;
using namespace rsb;

TEST(EventQueuePushHandlerTest, testPush)
{

    boost::shared_ptr<rsc::threading::SynchronizedQueue<EventPtr> > queue(
            new rsc::threading::SynchronizedQueue<EventPtr>);
    EventQueuePushHandler handler(queue);
    EventPtr event(new Event);
    handler.handle(event);
    EXPECT_EQ(event, queue->pop());

}
