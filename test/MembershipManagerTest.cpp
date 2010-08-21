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

#include "transport/spread/MembershipManager.h"

#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace rsb::spread;
using namespace testing;

TEST(MembershipManagerTest, testRoundtrip) {
	MembershipManagerPtr mm(new MembershipManager());
	// TODO convert this to a mock-only test case
	SpreadConnectionPtr sp(new SpreadConnection("blub"));
	sp->activate();

	ASSERT_NO_THROW(mm->join("a",sp));
	mm->join("a",sp);
	// join a different group
	mm->join("b",sp);
	// leave it
	mm->leave("b",sp);
	// leave a
	mm->leave("a",sp);
	mm->leave("a",sp);
	// re-join a previously left group
	mm->join("b",sp);
	// left b as well
	ASSERT_NO_THROW(mm->leave("b",sp));
}
