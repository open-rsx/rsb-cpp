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

#include "MetaData.h"

#include <stdexcept>

#include <rsc/misc/langutils.h>
#include <rsc/runtime/ContainerIO.h>

using namespace std;

namespace rsb {

const boost::posix_time::ptime MetaData::UNIX_EPOCH = boost::posix_time::ptime(
        boost::gregorian::date(1970, boost::date_time::Jan, 1));

MetaData::MetaData() :
        senderId(false), createTime(rsc::misc::currentTimeMicros()), sendTime(
                0), receiveTime(0), deliverTime(0) {
}

MetaData::~MetaData() {
}

string MetaData::getClassName() const {
    return "MetaData";
}

void MetaData::printContents(std::ostream& stream) const {
    stream << "senderId = " << senderId << ", creationTime = " << createTime
            << ", sendTime = " << sendTime << ", receiveTime = " << receiveTime
            << ", deliverTime = " << deliverTime << ", userTimes = "
            << userTimes << ", userInfos = " << userInfos;
}

rsc::misc::UUID MetaData::getSenderId() const {
    return senderId;
}

void MetaData::setSenderId(const rsc::misc::UUID& senderId) {
    this->senderId = senderId;
}

boost::uint64_t MetaData::getCreateTime() const {
    return createTime;
}

void MetaData::checkedTimeStampSet(boost::uint64_t& timestamp,
        const double& proposedValue) {
    if (proposedValue < 0) {
        throw invalid_argument("Timestamps must be >= 0");
    }
    if (proposedValue < 0.000001) {
        timestamp = rsc::misc::currentTimeMicros();
    } else {
        timestamp = boost::uint64_t((proposedValue * 1000000) + 0.5);
    }
}

void MetaData::checkedTimeStampSet(boost::uint64_t& timestamp,
        const boost::uint64_t& proposedValue) {
    if (proposedValue == 0) {
        timestamp = rsc::misc::currentTimeMicros();
    } else {
        timestamp = proposedValue;
    }
}

void MetaData::checkedTimeStampSet(boost::uint64_t& timestamp,
        const boost::posix_time::ptime& proposedValue) {
    boost::posix_time::time_duration delta = proposedValue - UNIX_EPOCH;
    // TODO convert time if not UTC time zone
    if (delta.is_negative()) {
        throw invalid_argument("Specified time is not a unix timestamp");
    }
    boost::uint64_t converted = delta.total_microseconds();
    timestamp = converted;
}

void MetaData::setCreateTime(const boost::uint64_t& time) {
    checkedTimeStampSet(createTime, time);
}

void MetaData::setCreateTime(const double& time) {
    checkedTimeStampSet(createTime, time);
}

void MetaData::setCreateTime(const boost::posix_time::ptime& time) {
    checkedTimeStampSet(createTime, time);
}

boost::uint64_t MetaData::getSendTime() const {
    return sendTime;
}

void MetaData::setSendTime(const boost::uint64_t& time) {
    checkedTimeStampSet(sendTime, time);
}

void MetaData::setSendTime(const double& time) {
    checkedTimeStampSet(sendTime, time);
}

void MetaData::setSendTime(const boost::posix_time::ptime& time) {
    checkedTimeStampSet(sendTime, time);
}

boost::uint64_t MetaData::getReceiveTime() const {
    return receiveTime;
}

void MetaData::setReceiveTime(const boost::uint64_t& time) {
    checkedTimeStampSet(receiveTime, time);
}

void MetaData::setReceiveTime(const double& time) {
    checkedTimeStampSet(receiveTime, time);
}

void MetaData::setReceiveTime(const boost::posix_time::ptime& time) {
    checkedTimeStampSet(receiveTime, time);
}

boost::uint64_t MetaData::getDeliverTime() const {
    return deliverTime;
}

void MetaData::setDeliverTime(const boost::uint64_t& time) {
    checkedTimeStampSet(deliverTime, time);
}

void MetaData::setDeliverTime(const double& time) {
    checkedTimeStampSet(deliverTime, time);
}

void MetaData::setDeliverTime(const boost::posix_time::ptime& time) {
    checkedTimeStampSet(deliverTime, time);
}

set<string> MetaData::userTimeKeys() const {
    set<string> keys;
    for (map<string, boost::uint64_t>::const_iterator it = userTimes.begin();
            it != userTimes.end(); ++it) {
        keys.insert(it->first);
    }
    return keys;
}

bool MetaData::hasUserTime(const string& key) const {
    return this->userTimes.find(key) != this->userTimes.end();
}

boost::uint64_t MetaData::getUserTime(const string& key) const {
    map<string, boost::uint64_t>::const_iterator it
        = this->userTimes.find(key);
    if (it == this->userTimes.end()) {
        throw invalid_argument("There is no user time with key '" + key + "'.");
    }
    return it->second;
}

void MetaData::setUserTime(const string& key, const boost::uint64_t& time) {
    userTimes.erase(key);
    checkedTimeStampSet(userTimes[key], time);
}

void MetaData::setUserTime(const string& key, const double& time) {
    userTimes.erase(key);
    checkedTimeStampSet(userTimes[key], time);
}

void MetaData::setUserTime(const string& key,
        const boost::posix_time::ptime& time) {
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
    for (map<string, string>::const_iterator it = userInfos.begin();
            it != userInfos.end(); ++it) {
        keys.insert(it->first);
    }
    return keys;
}

bool MetaData::hasUserInfo(const string& key) const {
    return this->userInfos.find(key) != this->userInfos.end();
}

string MetaData::getUserInfo(const string& key) const {
    map<string, string>::const_iterator it = this->userInfos.find(key);
    if (it == this->userInfos.end()) {
        throw invalid_argument(
                "No meta info registered under key '" + key + "'");
    }
    return it->second;
}

void MetaData::setUserInfo(const string& key, const string& value) {
    userInfos.erase(key);
    userInfos[key] = value;
}

map<string, string>::const_iterator MetaData::userInfosBegin() const {
    return userInfos.begin();
}

map<string, string>::const_iterator MetaData::userInfosEnd() const {
    return userInfos.end();
}

bool MetaData::operator==(const MetaData& other) const {
    return (senderId == other.senderId) && (createTime == other.createTime)
            && (sendTime == other.sendTime)
            && (receiveTime == other.receiveTime)
            && (deliverTime == other.deliverTime)
            && (userTimes == other.userTimes) && (userInfos == other.userInfos);
}

ostream& operator<<(ostream& stream, const MetaData& meta) {
    meta.print(stream);
    return stream;
}

}
