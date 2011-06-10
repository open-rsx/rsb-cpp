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

    /**
     * Returns the ID (a UUID) of the sending participant.
     *
     * @return UUID
     */
    rsc::misc::UUID getSenderId() const;
    /**
     * Sets the ID (a UUID) of the sending participant.
     *
     * @param senderId id of the sending participant
     */
    void setSenderId(const rsc::misc::UUID &senderId);

    /**
     * @name framework time stamps
     *
     * Timestamps supplied by the rrameowrk itself.
     */
    //@{
    /**
     * Returns a time stamp that is automatically filled with the time the event
     * instance was created by the language binding. This should usually reflect
     * the time at which the notified condition most likely occurred in the
     * sender. If event instances are reused, it has to be reset manually by the
     * client.
     *
     * This timestamp is initially set to the creating time stamp of this
     * instance.
     *
     * @return timestamp in microseconds
     */
    boost::uint64_t getCreateTime() const;
    /**
     * Sets the time stamp that is automatically filled with the time the event
     * instance was created by the language binding. This should usually reflect
     * the time at which the notified condition most likely occurred in the
     * sender. If event instances are reused, it has to be reset manually by the
     * client.
     *
     * @param time timestamp in microseconds or 0 to use current system time
     */
    void setCreateTime(const boost::uint64_t &time = 0);

    /**
     * Returns the time at which the generated notification for an event was
     * sent on the bus (after serialization).
     *
     * @return timestamp in microseconds
     */
    boost::uint64_t getSendTime() const;
    /**
     * Sets the time at which the generated notification for an event was
     * sent on the bus (after serialization).
     *
     * @param time timestamp in microseconds or 0 to use current system time
     */
    void setSendTime(const boost::uint64_t &time = 0);

    /**
     * Returns the time at which an event is received by listener in its encoded
     * form.
     *
     * @return timestamp in microseconds
     */
    boost::uint64_t getReceiveTime() const;
    /**
     * Sets the time at which an event is received by listener in its encoded
     * form.
     *
     * @param time timestamp in microseconds or 0 to use current system time
     */
    void setReceiveTime(const boost::uint64_t &time = 0);

    /**
     * Returns the time at which an event was decoded and will be dispatched to
     * the client as soon as possible (set directly before passing it to the
     * client handler).
     *
     * @return timestamp in microseconds
     */
    boost::uint64_t getDeliverTime() const;
    /**
     * Sets the time at which an event was decoded and will be dispatched to
     * the client as soon as possible (set directly before passing it to the
     * client handler).
     *
     * @param time timestamp in microseconds or 0 to use current system time
     */
    void setDeliverTime(const boost::uint64_t &time = 0);
    //@}

    /**
     * @name user timestamps
     *
     * Additional timestamps that can be filled by the framework client. Keys
     * are unique.
     */
    //@{
    /**
     * Returns the keys of all available user times.
     *
     * @return set of all keys
     */
    std::set<std::string> userTimeKeys() const;
    /**
     * Checks whether a user-provided timestamp with the given key exists
     *
     * @param key the key to check
     * @return @c true if a timestamp for the given key exists, else @c false
     */
    bool hasUserTime(const std::string &key) const;
    /**
     * Returns the user timestamp stored under the provided key.
     *
     * @param key key of the user-provided timestamp
     * @return timetamp
     * @throw std::invalid_argument no timestamp stored und the provided key
     */
    boost::uint64_t getUserTime(const std::string &key) const;
    /**
     * Sets a user timestamp and replaces existing entries.
     *
     * @param key the key for the timestamp
     * @param time time in microseconds or 0 to use current system time
     */
    void setUserTime(const std::string &key, const boost::uint64_t &time = 0);

    std::map<std::string, boost::uint64_t>::const_iterator userTimesBegin() const;
    std::map<std::string, boost::uint64_t>::const_iterator userTimesEnd() const;
    //@}

    /**
     * @name user infos
     *
     * A set of key-value style string infos that can be used by the client.
     * Keys are unique.
     */
    //@{
    /**
     * Returns all keys of user-defined infos.
     *
     * @return set of all defined keys
     */
    std::set<std::string> userInfoKeys() const;
    /**
     * Checks whether a user info exists under the provided key.
     *
     * @param key key to check
     * @return @c true if an info for the key is defined, else @c false
     */
    bool hasUserInfo(const std::string &key) const;
    /**
     * Returns the user-defined string for the given key.
     *
     * @param key key to look up
     * @return user info given for this key
     * @throw std::invalid_argument no info set for the specified key
     */
    std::string getUserInfo(const std::string &key) const;
    /**
     * Sets a user info with the specified key and value or replaces and already
     * existing one
     *
     * @param key the key to set
     * @param value the user value
     */
    void setUserInfo(const std::string &key, const std::string &value);
    std::map<std::string, std::string>::const_iterator userInfosBegin() const;
    std::map<std::string, std::string>::const_iterator userInfosEnd() const;
    //@}

    bool operator==(const MetaData &other) const;

private:

    rsc::misc::UUID senderId;

    boost::uint64_t createTime;
    boost::uint64_t sendTime;
    boost::uint64_t receiveTime;
    boost::uint64_t deliverTime;

    std::map<std::string, boost::uint64_t> userTimes;
    std::map<std::string, std::string> userInfos;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const MetaData &meta);

}
