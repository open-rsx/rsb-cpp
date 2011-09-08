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

#include <rsb/transport/Factory.h>

#include <gtest/gtest.h>

using namespace std;

using namespace rsc::runtime;

using namespace rsb;
using namespace rsb::transport;

class TestConnector: public InConnector {
public:
    static InConnector* create(const Properties&) {
        return new TestConnector();
    }

    virtual ~TestConnector() {
    }

    void activate() {
    }

    void deactivate() {
    }

    void setQualityOfServiceSpecs(const QualityOfServiceSpec&) {}

    void setScope(const Scope&) {}

    string getClassName() const {
        return "TestConnector";
    }
};

TEST(ConnectorFactoryTest, testRegister)
{
    ConnectorFactory<InConnector> &factory = ConnectorFactory<InConnector>::getInstance();
    factory.registerConnector("bla", &TestConnector::create, "bla");
    factory.getConnectorInfo("bla");
}
