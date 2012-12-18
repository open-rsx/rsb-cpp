/* ============================================================
 *
 * This file is a part of the rsb-cpp project.
 *
 * Copyright (C) 2012 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#pragma once

#include "rsb/converter/Converter.h"

/**
 * This converter is compiled into the unit tests to test windows compilation
 * issues with the templatized Converter base class.
 *
 * @ref https://code.cor-lab.org/issues/1285
 * @author jwienke
 */
class TestConverter: public rsb::converter::Converter<std::string> {
public:
    TestConverter();
    virtual ~TestConverter();

    virtual std::string serialize(const rsb::AnnotatedData& data,
            std::string& wire);

    virtual rsb::AnnotatedData deserialize(const std::string& wireSchema,
            const std::string& wire);

};

