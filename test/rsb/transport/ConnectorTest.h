/* ============================================================
 *
 * This file is a part of the RSB project
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

#include <boost/function.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/transport/InPullConnector.h"
#include "rsb/transport/InPushConnector.h"
#include "rsb/transport/OutConnector.h"

/**
 * Test configuration for different conector types.
 *
 * @author jwienke
 */
class ConnectorTestSetup {
public:
    ConnectorTestSetup(
            boost::function<rsb::transport::InPushConnectorPtr()> createInConnector,
            boost::function<rsb::transport::OutConnectorPtr()> createOutConnector) :
        createInConnector(createInConnector),
                createOutConnector(createOutConnector) {

    }
    boost::function<rsb::transport::InPushConnectorPtr()> createInConnector;
    boost::function<rsb::transport::OutConnectorPtr()> createOutConnector;
};

/**
 * Value-parametrized test class for different connector implementations.
 * For each implementation provide a ConnectorTestSetup instance that specifies
 * factory functions to use for creating connectors.
 *
 * @author jwienke
 */
class ConnectorTest: public ::testing::TestWithParam<ConnectorTestSetup> {
};
