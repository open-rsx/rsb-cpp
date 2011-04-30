/* ============================================================
 *
 * This file is a part of the RSB project.
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

#pragma once

#include <gmock/gmock.h>

#include "rsb/Service.h"

class MockService: public rsb::Service {
public:
    MockService() : rsb::Service(rsb::Scope("/")) {
    }
    MOCK_CONST_METHOD0(getScope, rsb::Scope());
    MOCK_CONST_METHOD0(getParticipants, std::set<rsb::ParticipantPtr>());
    MOCK_METHOD1(addParticipant, void(rsb::ParticipantPtr participant));
    MOCK_METHOD1(removeParticipant, void(rsb::ParticipantPtr participant));
    MOCK_CONST_METHOD0(getSubServices, std::set<rsb::ServicePtr>());
    MOCK_METHOD1(addSubService, void(rsb::ServicePtr subService));
    MOCK_METHOD1(removeSubService, void(rsb::ServicePtr subService));
};
