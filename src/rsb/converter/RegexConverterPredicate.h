/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/regex.hpp>

#include "PredicateConverterList.h"

namespace rsb {
namespace converter {

/** 
 * Objects of this class select @ref Converter s by matching the query
 * wire-schema or data-type against a sotred reqular expression.
 *
 * @author jmoringe
 */
class RegexConverterPredicate: public ConverterPredicate {
public:
    /** 
     * Construct a new RegexConverterPredicate that matches queries
     * against the regular expression @a regex.
     * 
     * @param regex The regular expression the new predicate should
     * use.
     */
    RegexConverterPredicate(const std::string &regex);

    /** 
     * Construct a new RegexConverterPredicate that matches queries
     * against the regular expression @a regex.
     * 
     * @param regex The regular expression the new predicate should
     * use.
     */
    RegexConverterPredicate(const boost::regex &regex);

    bool match(const std::string& key) const;
private:
    boost::regex regex;

    std::string getClassName() const;

    void printContents(std::ostream &/*stream*/) const;
};

}
}
