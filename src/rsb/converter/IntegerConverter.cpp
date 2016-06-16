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

#include "IntegerConverter.h"

namespace rsb {
namespace converter {

Uint32Converter::Uint32Converter()
    : IntegerConverter<boost::uint32_t>("uint32") {
}

Uint64Converter::Uint64Converter()
    : IntegerConverter<boost::uint64_t>("uint64") {
}

Int32Converter::Int32Converter()
    : IntegerConverter<boost::int32_t>("int32") {
}

Int64Converter::Int64Converter()
    : IntegerConverter<boost::int64_t>("int64") {
}

}
}
