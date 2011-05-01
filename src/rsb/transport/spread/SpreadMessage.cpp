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

#include "SpreadMessage.h"

#include <stdexcept>

#include <string.h>

#include <rsc/logging/Logger.h>

using namespace std;
using namespace rsc::logging;

namespace {
LoggerPtr logger(Logger::getLogger("rsb.spread.SpreadMessage"));
}

namespace rsb {
namespace spread {

SpreadMessage::SpreadMessage() :
    qos(UNRELIABLE) {
}

SpreadMessage::SpreadMessage(const Type &mt) :
    type(mt), qos(UNRELIABLE) {
}

SpreadMessage::SpreadMessage(const string &d) :
    data(d), qos(UNRELIABLE) {
}

SpreadMessage::SpreadMessage(const char *buf) :
    qos(UNRELIABLE) {
    data = string(buf);
}

SpreadMessage::~SpreadMessage() {
    RSCTRACE(logger, "spread message destructor called");
}

void SpreadMessage::setData(const std::string& doc) {
    data = doc;
}

void SpreadMessage::setData(const char* buf) {
    data = string(buf);
}

string SpreadMessage::getDataAsString() const {
    return data;
}

const char* SpreadMessage::getData() const {
    return data.c_str();
}

int SpreadMessage::getSize() const {
    return data.length();
}

void SpreadMessage::setType(Type mt) {
    type = mt;
}

SpreadMessage::Type SpreadMessage::getType() const {
    return type;
}

void SpreadMessage::addGroup(const std::string &name) {
    if (strlen(name.c_str()) > 31) {
        throw invalid_argument(
                "Group name '" + name + "' is too long for spread.");
    }
    groups.push_back(name);
}

int SpreadMessage::getGroupCount() const {
    return groups.size();
}

list<string>::const_iterator SpreadMessage::getGroupsBegin() const {
    return groups.begin();
}

list<string>::const_iterator SpreadMessage::getGroupsEnd() const {
    return groups.end();
}

SpreadMessage::QOS SpreadMessage::getQOS() const {
    return qos;
}

void SpreadMessage::setQOS(const QOS &qos) {
    this->qos = qos;
}

}
}
