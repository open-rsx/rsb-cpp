/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "QualityOfServiceSpec.h"

using namespace std;

namespace rsb {

QualityOfServiceSpec::QualityOfServiceSpec() :
    ordering(UNORDERED), reliability(RELIABLE) {
}

QualityOfServiceSpec::QualityOfServiceSpec(Ordering ordering,
        Reliability reliability) :
    ordering(ordering), reliability(reliability) {
}

QualityOfServiceSpec::~QualityOfServiceSpec() {
}

QualityOfServiceSpec::Ordering QualityOfServiceSpec::getOrdering() const {
    return ordering;
}

QualityOfServiceSpec::Reliability QualityOfServiceSpec::getReliability() const {
    return reliability;
}

bool QualityOfServiceSpec::operator==(const QualityOfServiceSpec &other) const {
    return (ordering == other.ordering && reliability == other.reliability);
}

ostream &operator<<(ostream &stream, const QualityOfServiceSpec &spec) {
    return stream << "QualityOfServiceSpec[ordering = " << spec.ordering
            << ", reliability = " << spec.reliability << "]";
}

}
