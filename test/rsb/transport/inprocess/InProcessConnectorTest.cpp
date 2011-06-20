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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/transport/inprocess/InConnector.h"
#include "rsb/transport/inprocess/OutConnector.h"

#include "testconfig.h"
#include "../ConnectorTest.h"

using namespace std;
using namespace rsb::inprocess;
using namespace rsb::transport;
using namespace testing;

rsb::transport::InPushConnectorPtr createInProcessInConnector() {
    return rsb::transport::InPushConnectorPtr(new rsb::inprocess::InConnector());
}

rsb::transport::OutConnectorPtr createInProcessOutConnector() {
    return rsb::transport::OutConnectorPtr(new rsb::inprocess::OutConnector());
}

const ConnectorTestSetup inProcessSetup(createInProcessInConnector,
        createInProcessOutConnector);

INSTANTIATE_TEST_CASE_P(InProcessConnector,
        ConnectorTest,
        ::testing::Values(inProcessSetup))
;
