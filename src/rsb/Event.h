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

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <rsc/misc/UUID.h>
#include <rsc/runtime/Printable.h>

#include "rsb/rsbexports.h"
#include "Scope.h"

namespace rsb {

typedef boost::shared_ptr<void> VoidPtr;

/**
 * Basic message that is exchanged between informers and listeners. It is a
 * combination of metadata and the actual data to publish / subscribe as
 * payload.
 *
 * @author swrede
 */
class RSB_EXPORT Event: public virtual rsc::runtime::Printable {
public:

    Event();

    /**
     * Constructor.
     *
     * @param scope scope of the event
     * @param payload payload of the event
     * @param type type identifier to serialize / deserialize the event payload
     */
    Event(const Scope &scope, boost::shared_ptr<void> payload,
            const std::string &type);

    virtual ~Event();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    void setId(const rsc::misc::UUID &id);
    rsc::misc::UUID getId();

    Scope getScope();
    void setScope(const Scope &scope);

    // TODO REFACTOR: introduce domain object for type (hierarchy?)
    std::string getType();
    void setType(const std::string &type);

    VoidPtr getData();
    void setData(VoidPtr d);

    bool hasMetaInfo(const std::string &key) const;
    std::string getMetaInfo(const std::string &key) const;
    void addMetaInfo(const std::string &key, const std::string &value,
            bool override = false);
    std::map<std::string, std::string>::const_iterator metaInfoBegin() const;
    std::map<std::string, std::string>::const_iterator metaInfoEnd() const;

private:
    rsc::misc::UUID id;
    Scope scope;

    VoidPtr content;

    // is this a single type, a hierarchy or a set?
    std::string type;

    std::map<std::string, std::string> metaInfos;

};

typedef boost::shared_ptr<Event> EventPtr;

}

