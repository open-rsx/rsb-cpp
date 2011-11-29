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

#include <stdexcept>

#include <boost/thread.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsb/EventCollections.h>
#include <rsb/EventId.h>
#include <rsb/MetaData.h>

#include "rsb/converter/EventsByScopeMapConverter.h"

using namespace std;
using namespace testing;
using namespace rsb;

EventPtr createRandomEvent(const Scope& scope) {

    EventPtr event(new Event);
    event->setScope(scope);
    if (rand() > RAND_MAX / 2) {
        event->setType(rsc::runtime::typeName<boost::uint64_t>());
        event->setData(
                boost::shared_ptr<boost::uint64_t>(
                        new boost::uint64_t(rand())));
        event->addCause(EventId(rsc::misc::UUID(), rand()));
    } else {
        event->setType(rsc::runtime::typeName<string>());
        event->setData(
                boost::shared_ptr<string>(
                        new string(rsc::misc::randAlnumStr(30))));
    }

    event->setEventId(rsc::misc::UUID(), rand());
    event->setMethod(rsc::misc::randAlnumStr(5));
    event->mutableMetaData().setCreateTime(boost::uint64_t(rand()));
    event->mutableMetaData().setSendTime(boost::uint64_t(rand()));
    event->mutableMetaData().setReceiveTime(boost::uint64_t(rand()));
    event->mutableMetaData().setDeliverTime(boost::uint64_t(rand()));
    event->mutableMetaData().setUserInfo(rsc::misc::randAlnumStr(10),
            rsc::misc::randAlnumStr(10));
    event->mutableMetaData().setUserInfo(rsc::misc::randAlnumStr(10),
            rsc::misc::randAlnumStr(10));
    event->mutableMetaData().setUserTime(rsc::misc::randAlnumStr(5),
            boost::uint64_t(rand()));

    return event;

}

TEST(EventsByScopeMapConverterTest, testRoundtrip) {

    boost::shared_ptr<EventsByScopeMap> message(new EventsByScopeMap);
    const unsigned int numScopes = 5;
    for (unsigned int scopeNum = 0; scopeNum < numScopes; ++scopeNum) {

        const Scope scope(
                "/" + rsc::misc::randAlnumStr(10) + "/"
                        + rsc::misc::randAlnumStr(5));

        const unsigned int numEvents = rand() / (RAND_MAX / 5) + 1;
        for (unsigned int eventNum = 0; eventNum < numEvents; ++eventNum) {

            EventPtr event = createRandomEvent(scope);
            (*message)[scope].push_back(event);

        }
    }

    converter::EventsByScopeMapConverter converter;
    string wire;
    string wireSchema = converter.serialize(
            make_pair(rsc::runtime::typeName<EventsByScopeMap>(), message),
            wire);
    converter::AnnotatedData data = converter.deserialize(wireSchema, wire);

    boost::shared_ptr<EventsByScopeMap> convertedData =
            boost::static_pointer_cast<EventsByScopeMap>(data.second);

    ASSERT_EQ(message->size(), convertedData->size());

    for (EventsByScopeMap::iterator originalScopeIt = message->begin();
            originalScopeIt != message->end(); ++originalScopeIt) {

        Scope scope = originalScopeIt->first;
        ASSERT_TRUE(convertedData->count(scope));
        ASSERT_EQ((*message)[scope].size(), (*convertedData)[scope].size());

        vector<EventPtr>::const_iterator originalIt = (*message)[scope].begin();
        vector<EventPtr>::const_iterator convertedIt =
                (*convertedData)[scope].begin();
        while (originalIt != (*message)[scope].end()) {

            EventPtr originalEvent = *originalIt;
            EventPtr convertedEvent = *convertedIt;

            EXPECT_EQ(originalEvent->getCauses(), convertedEvent->getCauses());
            EXPECT_EQ(originalEvent->getEventId(),
                    convertedEvent->getEventId());
            EXPECT_EQ(originalEvent->getMetaData(),
                    convertedEvent->getMetaData());
            EXPECT_EQ(originalEvent->getMethod(), convertedEvent->getMethod());
            EXPECT_EQ(originalEvent->getScope(), convertedEvent->getScope());
            EXPECT_EQ(originalEvent->getType(), convertedEvent->getType());
            if (originalEvent->getType() == rsc::runtime::typeName<string>()) {
                EXPECT_EQ(
                        *boost::static_pointer_cast<string>(originalEvent->getData()),
                        *boost::static_pointer_cast<string>(convertedEvent->getData()));
            } else {
                EXPECT_EQ(
                        *boost::static_pointer_cast<boost::uint64_t>(originalEvent->getData()),
                        *boost::static_pointer_cast<boost::uint64_t>(convertedEvent->getData()));
            }

            ++originalIt;
            ++convertedIt;
        }

    }

}
