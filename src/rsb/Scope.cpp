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

#include "Scope.h"

#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using namespace std;

namespace rsb {

const string Scope::COMPONENT_SEPARATOR = "/";

Scope::Scope(const string &s) {

    string scope = s;

    // allow the shortcut syntax with a missing slash in the end
    if (!scope.empty() && (scope.substr(scope.size() - 1, 1)
            != COMPONENT_SEPARATOR)) {
        scope.append(COMPONENT_SEPARATOR);
    }

    // validate fully formal scope syntax
    boost::regex expression(
            COMPONENT_SEPARATOR + "([a-zA-Z]+" + COMPONENT_SEPARATOR + ")*");
    if (!boost::regex_match(scope, expression)) {
        throw invalid_argument("Invalid scope syntax for '" + scope + "'");
    }

    // split also creates empty components in front of the leading slash and
    // after the trailing one, so skip them for processing
    string withoutSlahses = scope.substr(1, scope.size() - 2);

    // splitting an empty string would result in one component, which we do not
    // want to have
    if (!withoutSlahses.empty()) {
        boost::split(components, withoutSlahses,
                boost::is_any_of(COMPONENT_SEPARATOR));
    }

}

Scope::~Scope() {
}

vector<string> Scope::getComponents() const {
    return components;
}

string Scope::toString() const {
    stringstream s;
    s << COMPONENT_SEPARATOR;
    for (vector<string>::const_iterator it = components.begin(); it
            != components.end(); ++it) {
        s << *it << COMPONENT_SEPARATOR;
    }
    return s.str();
}

bool Scope::operator==(const Scope &other) const {
    return components == other.components;
}

bool Scope::operator<(const Scope &other) const {
    return toString() < other.toString();
}

ostream &operator<<(ostream &stream, const Scope &scope) {
    return stream << "Scope[" << scope.toString() << "]";
}

}
