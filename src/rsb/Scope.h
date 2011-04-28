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

#include <string>
#include <vector>
#include <ostream>

#include <boost/operators.hpp>

#include "rsb/rsbexports.h"

namespace rsb {

/**
 * Scope is a descriptor for a hierarchical channel of the unified bus. It can
 * be described through a syntax like "/a/parent/scope/".
 *
 * @author jwienke
 */
class RSB_EXPORT Scope: boost::totally_ordered<Scope> {
public:

    /**
     * Constructs scope from a string syntax.
     *
     * @param scope string representation of the desired scope
     * @throw std::invalid_argument invalid syntax
     */
    explicit Scope(const std::string &scope);

    /**
     * Creates a scope representing "/". Use this wisely!
     */
    Scope();

    /**
     * Destructor.
     */
    virtual ~Scope();

    /**
     * Returns all components of the scope as an ordered list. Components are
     * the names between the separator character '/'. The first entry in the
     * list is the highest level of hierarchy. The scope '/' returns an empty
     * list.
     *
     * @return components of the represented scope as ordered list with highest
     *         level as first entry
     */
    std::vector<std::string> getComponents() const;

    /**
     * Reconstructs a fully formal string representation of the scope with
     * leading an trailing slashes.
     *
     * @return string representation of the scope
     */
    std::string toString() const;

    /**
     * Creates a new scope that is a sub-scope of this one with the subordinated
     * scope described by the given argument. E.g. "/this/is/".concat("/a/test/")
     * results in "/this/is/a/test".
     *
     * @param childScope child to concatenate to the current scope for forming a
     *                   sub-scope
     * @return new scope instance representing the created sub-scope
     */
    Scope concat(const Scope &childScope) const;

    /**
     * Tests whether this scope is a sub-scope of the given other scope, which
     * means that the other scope is a prefix of this scope. E.g. "/a/b/" is a
     * sub-scope of "/a/".
     *
     * @param other other scope to test
     * @return @c true if this is a sub-scope of the other scope, equality gives
     *         @c false, too
     */
    bool isSubScopeOf(const Scope &other) const;

    /**
     * Inverse operation of #isSubScopeOf.
     *
     * @param other other scope to test
     * @return @c true if this scope is a strict super scope of the other scope.
     *         equality also gives @c false.
     */
    bool isSuperScopeOf(const Scope &other) const;

    /**
     * Generates all super scopes of this scope including the root scope "/".
     * The returned list of scopes is ordered by hierarchy with "/" being the
     * first entry.
     *
     * @param includeSelf if set to @true, this scope is also included as last
     *                    element of the returned list
     * @return list of all super scopes ordered by hierarchy, "/" being first
     */
    std::vector<Scope> superScopes(const bool &includeSelf = false) const;

    bool operator==(const Scope &other) const;
    bool operator<(const Scope &other) const;

private:

    std::vector<std::string> components;

    static const std::string COMPONENT_SEPARATOR;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const Scope &scope);

}

