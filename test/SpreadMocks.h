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

#ifndef SPREADMOCKS_H_
#define SPREADMOCKS_H_

#include <gmock/gmock.h>

#include "transport/spread/SpreadGroup.h"
#include "transport/spread/SpreadConnection.h"

using namespace std;
using namespace rsb::spread;
using namespace testing;

class SpreadGroupMock : public SpreadGroup {
	MOCK_METHOD1(join, void(SpreadConnectionPtr con));
	MOCK_METHOD1(leave, void(SpreadConnectionPtr con));
};

class SpreadConnectionMock : public SpreadConnection {
	MOCK_METHOD1(isActive, bool());
	// it definitely sucks that Spread Low-Level API calls are now
	// cluttered over the code, e.g., in the SpreadGroup Object
};

#endif /* SPREADMOCKS_H_ */
