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
     * Creates a new scope that is a subscope of this one with the subordinated
     * scope described by the given argument. E.g. "/this/is/".concat("/a/test/")
     * results in "/this/is/a/test".
     *
     * @param childScope child to concatenate to the current scope for forming a
     *                   subscope
     * @return new scope instance representing the created subscope
     */
    Scope concat(const Scope &childScope) const;

    bool operator==(const Scope &other) const;
    bool operator<(const Scope &other) const;

private:

    std::vector<std::string> components;

    static const std::string COMPONENT_SEPARATOR;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const Scope &scope);

}

