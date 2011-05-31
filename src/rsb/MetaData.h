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

#include <map>
#include <set>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/operators.hpp>

#include <rsc/runtime/Printable.h>
#include <rsc/misc/UUID.h>

#include "rsb/rsbexports.h"

namespace rsb {

/**
 * Framework-supplied meta data attached to each event that give information
 * e.g. about timing issues.
 *
 * @author jwienke
 */
class RSB_EXPORT MetaData: public virtual rsc::runtime::Printable,
        boost::equality_comparable<MetaData> {
public:

    MetaData();
    virtual ~MetaData();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    rsc::misc::UUID getSenderId() const;
    void setSenderId(const rsc::misc::UUID &senderId);

    boost::uint64_t getEventCreationTime() const;
    void setEventCreationTime(const boost::uint64_t &time = 0);
    boost::uint64_t getSendTime() const;
    void setSendTime(const boost::uint64_t &time = 0);
    boost::uint64_t getRawReceiveTime() const;
    void setRawReceiveTime(const boost::uint64_t &time = 0);
    boost::uint64_t getReceiveTime() const;
    void setReceiveTime(const boost::uint64_t &time = 0);

    std::set<std::string> userTimeKeys() const;
    bool hasUserTime(const std::string &key) const;
    boost::uint64_t getUserTime(const std::string &key) const;
    void setUserTime(const std::string &key, const boost::uint64_t &time = 0);

    std::set<std::string> userInfoKeys() const;
    bool hasUserInfo(const std::string &key) const;
    std::string getUserInfo(const std::string &key) const;
    void setUserInfo(const std::string &key, const std::string &value);
    std::map<std::string, std::string>::const_iterator userInfosBegin() const;
    std::map<std::string, std::string>::const_iterator userInfosEnd() const;

    bool operator==(const MetaData &other) const;

private:

    rsc::misc::UUID senderId;

    boost::uint64_t eventCreationTime;
    boost::uint64_t sendTime;
    boost::uint64_t rawReceiveTime;
    boost::uint64_t receiveTime;

    std::map<std::string, boost::uint64_t> userTimes;
    std::map<std::string, std::string> userInfos;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const MetaData &meta);

}
