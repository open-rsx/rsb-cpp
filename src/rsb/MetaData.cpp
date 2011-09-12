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
#include <rsc/runtime/ContainerIO.h>

using namespace std;

namespace rsb {

MetaData::MetaData() :
    senderId(false),
    createTime(rsc::misc::currentTimeMicros()), sendTime(0), receiveTime(0),
    deliverTime(0) {
}

MetaData::~MetaData() {
}

string MetaData::getClassName() const {
    return "MetaData";
}

void MetaData::printContents(std::ostream &stream) const {
    stream << "senderId = " << senderId << ", creationTime = " << createTime
            << ", sendTime = " << sendTime << ", receiveTime = " << receiveTime
            << ", deliverTime = " << deliverTime << ", userTimes = "
            << userTimes << ", userInfos = " << userInfos;
}

rsc::misc::UUID MetaData::getSenderId() const {
    return senderId;
}

void MetaData::setSenderId(const rsc::misc::UUID &senderId) {
    this->senderId = senderId;
}

boost::uint64_t MetaData::getCreateTime() const {
    return createTime;
}

void MetaData::checkedTimeStampSet(boost::uint64_t &timeStamp,
        const double &proposedValue) {
    if (proposedValue < 0) {
        throw invalid_argument("Timestamps must be >= 0");
    }
    if (proposedValue < 0.000001) {
        timeStamp = rsc::misc::currentTimeMicros();
    } else {
        timeStamp = boost::uint64_t(proposedValue * 1000000);
    }
}

void MetaData::checkedTimeStampSet(boost::uint64_t &timeStamp,
        const boost::uint64_t &proposedValue) {
    if (proposedValue == 0) {
        timeStamp = rsc::misc::currentTimeMicros();
    } else {
        timeStamp = proposedValue;
    }
}

void MetaData::setCreateTime(const boost::uint64_t &time) {
    checkedTimeStampSet(createTime, time);
}

void MetaData::setCreateTime(const double &time) {
    checkedTimeStampSet(createTime, time);
}

boost::uint64_t MetaData::getSendTime() const {
    return sendTime;
}

void MetaData::setSendTime(const boost::uint64_t &time) {
    checkedTimeStampSet(sendTime, time);
}

void MetaData::setSendTime(const double &time) {
    checkedTimeStampSet(sendTime, time);
}

boost::uint64_t MetaData::getReceiveTime() const {
    return receiveTime;
}

void MetaData::setReceiveTime(const boost::uint64_t &time) {
    checkedTimeStampSet(receiveTime, time);
}

void MetaData::setReceiveTime(const double &time) {
    checkedTimeStampSet(receiveTime, time);
}

boost::uint64_t MetaData::getDeliverTime() const {
    return deliverTime;
}

void MetaData::setDeliverTime(const boost::uint64_t &time) {
    checkedTimeStampSet(deliverTime, time);
}

void MetaData::setDeliverTime(const double &time) {
    checkedTimeStampSet(deliverTime, time);
}

set<string> MetaData::userTimeKeys() const {
    set<string> keys;
    for (map<string, boost::uint64_t>::const_iterator it = userTimes.begin(); it
            != userTimes.end(); ++it) {
        keys.insert(it->first);
    }
    return keys;
}

bool MetaData::hasUserTime(const string &key) const {
    return userTimes.count(key) > 0;
}

boost::uint64_t MetaData::getUserTime(const string &key) const {
    map<string, boost::uint64_t>::const_iterator it = userTimes.find(key);
    if (it == userTimes.end()) {
        throw invalid_argument("There is no user time with key '" + key + "'.");
    } else {
        return it->second;
    }
}

void MetaData::setUserTime(const string &key, const boost::uint64_t &time) {
    userTimes.erase(key);
    checkedTimeStampSet(userTimes[key], time);
}

void MetaData::setUserTime(const string &key, const double &time) {
    userTimes.erase(key);
    checkedTimeStampSet(userTimes[key], time);
}

map<string, boost::uint64_t>::const_iterator MetaData::userTimesBegin() const {
    return userTimes.begin();
}

map<string, boost::uint64_t>::const_iterator MetaData::userTimesEnd() const {
    return userTimes.end();
}

set<string> MetaData::userInfoKeys() const {
    set<string> keys;
    for (map<string, string>::const_iterator it = userInfos.begin(); it
            != userInfos.end(); ++it) {
        keys.insert(it->first);
    }
    return keys;
}

bool MetaData::hasUserInfo(const string &key) const {
    return userInfos.count(key);
}

string MetaData::getUserInfo(const string &key) const {
    if (userInfos.count(key)) {
        return userInfos.find(key)->second;
    } else {
        throw invalid_argument("No meta info registered under key '" + key
                + "'");
    }
}

void MetaData::setUserInfo(const string &key, const string &value) {
    userInfos.erase(key);
    userInfos[key] = value;
}

map<string, string>::const_iterator MetaData::userInfosBegin() const {
    return userInfos.begin();
}

map<string, string>::const_iterator MetaData::userInfosEnd() const {
    return userInfos.end();
}

bool MetaData::operator==(const MetaData &other) const {
    return (senderId == other.senderId) && (createTime == other.createTime)
            && (sendTime == other.sendTime) && (receiveTime
            == other.receiveTime) && (deliverTime == other.deliverTime)
            && (userTimes == other.userTimes) && (userInfos == other.userInfos);
}

ostream &operator<<(ostream &stream, const MetaData &meta) {
    meta.print(stream);
    return stream;
}

}
