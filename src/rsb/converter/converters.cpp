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

#include "converters.h"

#include <boost/thread.hpp>

#include "Repository.h"
#include "BoolConverter.h"
#include "ByteArrayConverter.h"
#include "EventsByScopeMapConverter.h"
#include "StringConverter.h"
#include "Uint64Converter.h"
#include "VoidConverter.h"

namespace rsb {
namespace converter {

static bool registered = false;
static boost::mutex registrationMutex;

void registerDefaultConverters() {

    boost::mutex::scoped_lock lock(registrationMutex);
    if (!registered) {
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new BoolConverter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new ByteArrayConverter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new StringConverter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new Uint64Converter));
        stringConverterRepository()->registerConverter(
                Converter<std::string>::Ptr(new VoidConverter));
        registered = true;
    }

}

}
}
