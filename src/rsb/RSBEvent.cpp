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

#include "RSBEvent.h"
#include <ostream>

using namespace std;

namespace rsb {

  RSBEvent::~RSBEvent() {
  }

  void RSBEvent::setUUID(const rsc::misc::UUID &id) {
    this->id = id;
  }

  rsc::misc::UUID RSBEvent::getUUID() {
    return id;
  }

  void RSBEvent::setURI(string s) {
    uri = s;
  }

  string RSBEvent::getURI() {
    return uri;
  }

  void RSBEvent::setData(VoidPtr d) {
    content = d;
  }

  VoidPtr RSBEvent::getData() {
    return content;
  }

  string RSBEvent::getType() {
    return type;
  }

  void RSBEvent::setType(string t) {
    type = t;
  }

  bool RSBEvent::hasMetaInfo(const string &key) const {
    return metaInfos.count(key);
  }

  string RSBEvent::getMetaInfo(const string &key) const {
    if (metaInfos.count(key)) {
      return metaInfos.find(key)->second;
    } else {
      throw runtime_error("No meta info registered under key '" + key + "'");
    }
  }

  void RSBEvent::addMetaInfo(const string &key, const string &value,
                             bool override) {

    if (metaInfos.count(key) && !override) {
      throw runtime_error("There already is meta info registered under key '"
                          + key + "'");
    }
    metaInfos[key] = value;

  }

  map<string, string>::const_iterator RSBEvent::metaInfoBegin() const {
    return metaInfos.begin();
  }

  map<string, string>::const_iterator RSBEvent::metaInfoEnd() const {
    return metaInfos.end();
  }

  ostream& operator<<(ostream& out, const RSBEvent& e) {
    //out.precision(3);
    out << "RSBEvent[id=" << e.id.getIdAsString() << " type=" << e.type
        << " uri=" << e.uri << "] ";
    return out;
  }

}
