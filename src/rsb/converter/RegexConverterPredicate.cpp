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

#include "RegexConverterPredicate.h"

using namespace std;
using namespace boost;

namespace rsb {
namespace converter {

RegexConverterPredicate::RegexConverterPredicate(const string &regex):
    regex(regex) {
}

RegexConverterPredicate::RegexConverterPredicate(const boost::regex &regex):
    regex(regex) {
}

bool RegexConverterPredicate::match(const string& key) const {
    return boost::regex_match(key, this->regex);
}

string RegexConverterPredicate::getClassName() const {
    return "RegexConverterPredicate";
}

void RegexConverterPredicate::printContents(ostream &stream) const {
    stream << "pattern = " << this->regex;
}

}
}
