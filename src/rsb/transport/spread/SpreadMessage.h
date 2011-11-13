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

#pragma once

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * Default message QOS for sending is RELIABLE.
 *
 * @author swrede
 */
class RSB_EXPORT SpreadMessage {
public:

    enum Type {
        REGULAR = 0x0001, MEMBERSHIP = 0x0002, OTHER = 0xFFFF,
    };

    /**
     * Message reliability and QoS types. For some strange reasons the int
     * values directly resemble the sp.h defines. ;)
     *
     * @author jwienke
     */
    enum QOS {
        UNRELIABLE = 0x00000001,
        RELIABLE = 0x00000002,
        FIFO = 0x00000004,
        CASUAL = 0x00000008,
        AGREED = 0x00000010,
        SAFE = 0x00000020
    };

    /**
     * Creates a new empty message with undefined type #OTHER and QoS
     * #UNRELIABLE.
     */
    SpreadMessage();
    /**
     * Creates a new message with the specified type and QoS #UNRELIABLE.
     *
     * @param mt message type
     */
    SpreadMessage(const Type& mt);
    /**
     * Creates a message with the specified data and message type #OTHER and QoS
     * #UNRELIABLE.
     *
     * @param d data to set
     */
    SpreadMessage(const std::string& d);
    /**
     * Creates a message with the specified data and message type #OTHER and QoS
     * #UNRELIABLE.
     *
     * @param d data to set
     */
    SpreadMessage(const char* d);
    virtual ~SpreadMessage();

    void setData(const std::string& doc);
    void setData(const char* d);
    std::string getDataAsString() const;
    const char* getData() const;
    int getSize() const;
    SpreadMessage::Type getType() const;
    void setType(Type mt);
    QOS getQOS() const;
    void setQOS(const QOS& qos);

    void addGroup(const std::string& name);
    unsigned int getGroupCount() const;
    std::list<std::string>::const_iterator getGroupsBegin() const;
    std::list<std::string>::const_iterator getGroupsEnd() const;

    /**
     * Resets this message to a message of type #OTHER with no contents and
     * groups.
     */
    void reset();

private:
    std::string data;
    std::list<std::string> groups;
    Type type;
    QOS qos;
};

typedef boost::shared_ptr<SpreadMessage> SpreadMessagePtr;

}
}

