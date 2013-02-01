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
 * /([-_a-zA-Z0-9]+/)* and splits it at '/' characters.
 *
 * @param s String representation that should be verified and split.
 * @param components A vector in which the extracted scope components should be
 *                   stored
 * @param normalizedString returns a normalized version of the scope's string
 *                         representation
 */
inline void verifyAndSplit(const string& s, vector<string>& components,
        string& normalizedString) {
    if (s.empty()) {
        throw invalid_argument("Empty scope string given.");
    }
    string::const_iterator prev = s.begin();
    // The scope string has to start with a '/'.
    if (*prev != Scope::COMPONENT_SEPARATOR) {
        throw invalid_argument(
                str(
                        format(
                                "Invalid scope syntax for '%1%': has to begin with '%2%'")
                                % s % Scope::COMPONENT_SEPARATOR));
    }

    // Process remainder of s.  If we are at the end of s already, it
    // denotes the root scope and we do not have to create any
    // components.
    string::const_iterator next = prev + 1;
    for (; next != s.end(); ++next) {
        // If we encounter a '/', make sure that we accumulated at
        // least one character in the current scope component.
        if (*next == Scope::COMPONENT_SEPARATOR) {
            if (distance(prev, next) == 1) {
                throw invalid_argument(
                        str(
                                format(
                                        "Invalid scope syntax for '%1%' at char %2%: zero-length component between two '%3%'")
                                        % s % distance(s.begin(), next)
                                        % Scope::COMPONENT_SEPARATOR));
            }
            components.push_back(string(prev + 1, next));
            prev = next;
        }
        // The current character is not a '/' and we want to append it
        // to the current scope component. Verify that it is a legal
        // scope component character.
        else if (!(('a' <= *next && *next <= 'z')
                   || ('A' <= *next && *next <= 'Z')
                   || ('0' <= *next && *next <= '9')
                   || (*next == '_') || (*next == '-'))) {
            throw invalid_argument(
                    str(
                            format(
                                    "Invalid scope syntax for '%1%' at char %2%: invalid character '%3%'")
                                    % s % distance(s.begin(), next) % *next));
        }
        // The current character is a valid scope component
        // character. Append it to the current scope component.
    }
    if (prev + 1 != next) {
        components.push_back(string(prev + 1, next));
    }

    // now build the normalized string. ATM we only need to check for the
    // trailing slash, All other things are handled by the checks before
    normalizedString = s;
    if (*(--s.end()) != Scope::COMPONENT_SEPARATOR) {
        normalizedString += Scope::COMPONENT_SEPARATOR;
    }

}

Scope::Scope(const string& s) :
        scopestring() {
    // reserve a number of vector components that should be enough for most
    // realistic scopes. This speeds up parsing.
    components.reserve(10);
    verifyAndSplit(s, this->components, this->scopestring);
}

Scope::Scope(const char *scope) :
        scopestring() {
    // reserve a number of vector components that should be enough for most
    // realistic scopes. This speeds up parsing.
    components.reserve(10);
    verifyAndSplit(string(scope), this->components, this->scopestring);
}

Scope::Scope() :
        scopestring("/") {
}

Scope::~Scope() {
}

const vector<string>& Scope::getComponents() const {
    return components;
}

const std::string& Scope::toString() const {
    return this->scopestring;
}

Scope Scope::concat(const Scope& childScope) const {
    Scope result; // start with empty string cache
    result.components = this->components;

    for (vector<string>::const_iterator it = childScope.components.begin();
            it != childScope.components.end(); ++it) {
        result.components.push_back(*it);
    }
    result.updateStringCache();

    return result;
}

bool Scope::isSubScopeOf(const Scope& other) const {

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

bool Scope::isSuperScopeOf(const Scope& other) const {

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

void Scope::updateStringCache() {
    unsigned int scopesize = 1;
    for (vector<string>::const_iterator it = components.begin();
            it != components.end(); ++it) {
        scopesize += it->size() + 1;
    }

    this->scopestring.resize(scopesize);
    this->scopestring[0] = COMPONENT_SEPARATOR;
    string::iterator cursor = scopestring.begin() + 1;
    for (vector<string>::const_iterator it = components.begin();
            it != components.end(); ++it) {
        std::copy(it->begin(), it->end(), cursor);
        cursor += it->size();
        *cursor++ = COMPONENT_SEPARATOR;
    }
}

vector<Scope> Scope::superScopes(const bool& includeSelf) const {

    vector<Scope> result;

    if (!components.empty()) {

        // this math only works for scopes that are not the root scope
        for (size_t requiredComponents = 0;
                requiredComponents <= components.size() - 1;
                ++requiredComponents) {

            Scope super;
            for (size_t i = 0; i < requiredComponents; ++i) {
                super.components.push_back(components[i]);
            }
            super.updateStringCache();
            result.push_back(super);

        }

    }

    if (includeSelf) {
        result.push_back(*this);
    }

    return result;
}

bool Scope::operator==(const Scope& other) const {
    return toString() == other.toString();
}

bool Scope::operator<(const Scope& other) const {
    return toString() < other.toString();
}

ostream& operator<<(ostream& stream, const Scope& scope) {
    return stream << "Scope[" << scope.toString() << "]";
}

}
