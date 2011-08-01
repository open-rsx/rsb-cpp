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
#include <iterator>

#include <boost/format.hpp>

using namespace std;

using namespace boost;

namespace rsb {

const char Scope::COMPONENT_SEPARATOR = '/';

/**
 * Validate that @a s satisfies the regular expression
 * /([a-zA-Z0-9]+/)* and split at '/' characters.
 *
 * @param s String representation that should be verified and split.
 * @param components A vector in which the extracted scope components
 * should be stored
 */
inline void verifyAndSplit(const string &s, vector<string> &components) {
    string::const_iterator prev = s.begin();
    // The scope string has to start with a '/'.
    if (*prev != Scope::COMPONENT_SEPARATOR)
        throw invalid_argument(str(format("Invalid scope syntax for '%1%': has to begin with '%2%'")
                                   % s % Scope::COMPONENT_SEPARATOR));

    // Process remainder of s.  If we are at the end of s already, it
    // denotes the root scope and we do not have to create any
    // components.
    string::const_iterator next = prev + 1;
    for (; next != s.end(); ++next) {
        // If we encounter a '/', make sure that we accumulated at
        // least one character in the current scope component.
        if (*next == Scope::COMPONENT_SEPARATOR) {
            if (distance(prev,next) == 1) {
                throw invalid_argument(str(format("Invalid scope syntax for '%1%' at char %2%: zero-length component between two '%3%'")
                                           % s % distance(s.begin(), next) % Scope::COMPONENT_SEPARATOR));
            }
            components.push_back(string(prev + 1, next));
            prev = next;
        }
        // The current character is not a '/' and we want to append it
        // to the current scope component. Verify that it is a legal
        // scope component character.
        else if (!(('a' <= *next && *next <= 'z')
                     || ('A' <= *next && *next <= 'Z')
                     || ('0' <= *next && *next <= '9'))) {
            throw invalid_argument(str(format("Invalid scope syntax for '%1%' at char %2%: invalid character '%3%'")
                                       % s % distance(s.begin(), next) % *next));
        }
        // The current character is a valid scope component
        // character. Append it to the current scope component.
    }
    if (prev + 1 != next) {
        components.push_back(string(prev + 1, next));
    }
}

Scope::Scope(const string &s) {
    verifyAndSplit(s, this->components);
}

Scope::Scope() {
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

Scope Scope::concat(const Scope &childScope) const {
    Scope result = *this;
    for (vector<string>::const_iterator it = childScope.components.begin(); it
            != childScope.components.end(); ++it) {
        result.components.push_back(*it);
    }
    return result;
}

bool Scope::isSubScopeOf(const Scope &other) const {

    if (components.size() <= other.components.size()) {
        return false;
    }

    for (size_t i = 0; i < other.components.size(); ++i) {
        if (components[i] != other.components[i]) {
            return false;
        }
    }

    return true;

}

bool Scope::isSuperScopeOf(const Scope &other) const {

    if (components.size() >= other.components.size()) {
        return false;
    }

    for (size_t i = 0; i < components.size(); ++i) {
        if (components[i] != other.components[i]) {
            return false;
        }
    }

    return true;

}

vector<Scope> Scope::superScopes(const bool &includeSelf) const {

    vector<Scope> result;

    if (!components.empty()) {

        // this math only works for scopes that are not the root scope
        for (size_t requiredComponents = 0; requiredComponents
                <= components.size() - 1; ++requiredComponents) {

            Scope super;
            for (size_t i = 0; i < requiredComponents; ++i) {
                super.components.push_back(components[i]);
            }
            result.push_back(super);

        }

    }

    if (includeSelf) {
        result.push_back(*this);
    }

    return result;

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
