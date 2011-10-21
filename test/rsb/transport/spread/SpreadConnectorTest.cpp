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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/converter/Repository.h"
#include "rsb/transport/spread/InConnector.h"
#include "rsb/transport/spread/OutConnector.h"

#include "testconfig.h"
#include "../ConnectorTest.h"

using namespace std;
using namespace rsb::spread;
using namespace rsb::transport;
using namespace rsb::converter;
using namespace testing;

static int dummy = pullInConnectorTest();

InPushConnectorPtr createSpreadInConnector() {
    return InPushConnectorPtr(new rsb::spread::InConnector(stringConverterRepository()->getConvertersForDeserialization(),
						       defaultHost(),
						       SPREAD_PORT));
}

OutConnectorPtr createSpreadOutConnector() {
    return OutConnectorPtr(new rsb::spread::OutConnector(stringConverterRepository()->getConvertersForSerialization(),
							 defaultHost(),
							 SPREAD_PORT));
}

const ConnectorTestSetup spreadSetup(createSpreadInConnector,
        createSpreadOutConnector);

INSTANTIATE_TEST_CASE_P(SpreadConnector,
        ConnectorTest,
        ::testing::Values(spreadSetup))
;
