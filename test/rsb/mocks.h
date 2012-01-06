/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "rsb/Service.h"

class MockService: public rsb::Service {
public:
    MockService() :
        rsb::Service(rsb::Scope("/")) {
    }
    std::string getClassName() const {
        return "MockService";
    }
    MOCK_CONST_METHOD0(getScope, rsb::Scope());
    MOCK_CONST_METHOD0(getParticipants, std::set<rsb::ParticipantPtr>());
    MOCK_METHOD1(addParticipant, void(rsb::ParticipantPtr participant));
    MOCK_METHOD1(removeParticipant, void(rsb::ParticipantPtr participant));
    MOCK_CONST_METHOD0(getSubServices, std::set<rsb::ServicePtr>());
    MOCK_METHOD1(addSubService, void(rsb::ServicePtr subService));
    MOCK_METHOD1(removeSubService, void(rsb::ServicePtr subService));
};
