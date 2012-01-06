/* ============================================================
 *
 * This file is a part of the RSB-trunk project.
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

#include "ProtocolBufferConverter.h"

namespace rsb {
namespace converter {

namespace detail {

std::string typeNameToProtoName(const std::string& type_name) {
	bool skip = false;
#ifdef WIN32
	if (type_name.size() >= 6 && type_name.substr(0, 6) == "class ") {
		skip = true;
	}
#endif
    std::string result = ".";
    bool colon = false;
    for (std::string::const_iterator it 
		     = type_name.begin() + (skip ? 6 : 0); 
			 it != type_name.end(); ++it) {
        // Consume two (hopefully adjacent) ':', emit one '.'
        if (*it == ':') {
            if (colon) {
                colon = false;
            } else {
                result.push_back('.');
                colon = true;
            }
        } else {
            result.push_back(*it);
        }
    }
    return result;
}

std::string typeNameToWireSchema(const std::string& type_name) {
    // See https://code.cor-lab.de/projects/rsb/wiki/Types/diff?version=6&version_from=4&commit=View+differences
    // return "protocol-buffer:" + typeNameToProtoName(type_name);
    return typeNameToProtoName(type_name);
}

}

}
}
