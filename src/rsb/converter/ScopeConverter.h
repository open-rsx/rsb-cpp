/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <string>

#include <boost/shared_ptr.hpp>

#include "Converter.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

/**
 * (De)serializes @ref rsb::Scope objects.
 *
 * @author jmoringe
 */
class RSB_EXPORT ScopeConverter: public Converter<std::string> {
 public:
    ScopeConverter();
    virtual ~ScopeConverter();

    std::string serialize(const AnnotatedData& data, std::string& wire);

    AnnotatedData deserialize(const std::string& wireSchema,
                              const std::string& wire);

 private:
    static const std::string WIRE_SCHEMA;
};

}
}
