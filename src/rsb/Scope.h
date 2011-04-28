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

#include "rsb/rsbexports.h"

namespace rsb {

/**
 * Scope is a descriptor for a hierarchical channel of the unified bus. It can
 * be described through a syntax like "/a/parent/scope/".
 *
 * @author jwienke
 */
class RSB_EXPORT Scope {
public:

    /**
     * Constructs scope from a string syntax.
     *
     * @param scope string representation of the desired scope
     * @throw std::invalid_argument invalid syntax
     */
    explicit Scope(const std::string &scope);

    virtual ~Scope();

    std::vector<std::string> getComponents() const;

    std::string toString() const;

private:

    std::vector<std::string> components;

    static const std::string COMPONENT_SEPARATOR;

};

RSB_EXPORT std::ostream &operator<<(std::ostream &stream, const Scope &scope);

}

