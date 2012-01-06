/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#pragma once

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

