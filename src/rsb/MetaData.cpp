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

#include "MetaData.h"

#include <stdexcept>

#include <rsc/misc/langutils.h>

using namespace std;

namespace rsb {

MetaData::MetaData() :
    eventCreationTime(rsc::misc::currentTimeMicros()), sendTime(0),
            rawReceiveTime(0), receiveTime(0) {
}

MetaData::~MetaData() {
}

string MetaData::getSenderId() const {
    return senderId;
}

void MetaData::setSenderId(const string &senderId) {
    this->senderId = senderId;
}

boost::uint64_t MetaData::getEventCreationTime() const {
    return eventCreationTime;
}

void MetaData::setEventCreationTime(const boost::uint64_t &time) {
    if (time == 0) {
        eventCreationTime = rsc::misc::currentTimeMicros();
    } else {
        eventCreationTime = time;
    }
}

boost::uint64_t MetaData::getSendTime() const {
    return sendTime;
}

void MetaData::setSendTime(const boost::uint64_t &time) {
    if (time == 0) {
        sendTime = rsc::misc::currentTimeMicros();
    } else {
        sendTime = time;
    }
}

boost::uint64_t MetaData::getRawReceiveTime() const {
    return rawReceiveTime;
}

void MetaData::setRawReceiveTime(const boost::uint64_t &time) {
    if (time == 0) {
        rawReceiveTime = rsc::misc::currentTimeMicros();
    } else {
        rawReceiveTime = time;
    }
}

boost::uint64_t MetaData::getReceiveTime() const {
    return receiveTime;
}

void MetaData::setReceiveTime(const boost::uint64_t &time) {
    if (time == 0) {
        receiveTime = rsc::misc::currentTimeMicros();
    } else {
        receiveTime = time;
    }
}

set<string> MetaData::userTimeKeys() const {
    set<string> keys;
    for (map<string, boost::uint64_t>::const_iterator it = userTimes.begin(); it
            != userTimes.end(); ++it) {
        keys.insert(it->first);
    }
    return keys;
}

bool MetaData::hasUserTime(const std::string &key) const {
    return userTimes.count(key) > 0;
}

boost::uint64_t MetaData::getUserTime(const std::string &key) const {
    map<string, boost::uint64_t>::const_iterator it = userTimes.find(key);
    if (it == userTimes.end()) {
        throw invalid_argument("There is no user time with key '" + key + "'.");
    } else {
        return it->second;
    }
}

void MetaData::setUserTime(const std::string &key, const boost::uint64_t &time) {
    userTimes.erase(key);
    if (time == 0) {
        userTimes[key] = rsc::misc::currentTimeMicros();
    } else {
        userTimes[key] = time;
    }
}

}
