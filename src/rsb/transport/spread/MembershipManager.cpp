/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "MembershipManager.h"

#include <iostream>

using namespace std;
using namespace rsc::logging;

namespace rsb {
namespace spread {

MembershipManager::MembershipManager() :
    logger(Logger::getLogger("rsb.spread.MembershipManager")) {
    RSCTRACE(logger, "MembershipManager() entered");
    groups = boost::shared_ptr<GroupMap>(new GroupMap());
}

MembershipManager::~MembershipManager() {
}

void MembershipManager::join(string group, SpreadConnectionPtr c) {
    RSCDEBUG(logger, "Trying to join group with id " << group << " on SpreadConnection " << c);
    boost::recursive_mutex::scoped_lock lock(groupsMutex);
    GroupMap::iterator i = groups->find(group);
    if (groups->end() != i) {
        int refs = i->second.second;
        RSCDEBUG(logger, "Group object already existing, ref count: " << refs);
        if (0 == refs) {
            // if count == 0 we re-join the group
            i->second.first->join(c);
            RSCINFO(logger, "Re-joined SpreadGroup with id " << group);
        }
        i->second.second = ++refs;
        RSCTRACE(logger, "New group ref count: " << i->second.second);
    } else {
        RSCDEBUG(logger, "Group object not found, creating new one");
        SpreadGroupPtr sg(new SpreadGroup(group));
        sg->join(c);
        RSCINFO(logger, "Joined SpreadGroup with id " << group);
        (*groups)[group] = make_pair(sg, 1);
    }
}

void MembershipManager::leave(string group, SpreadConnectionPtr c) {
    RSCDEBUG(logger, "Checking if we want to leave group with id " << group << " on SpreadConnection " << c);
    boost::recursive_mutex::scoped_lock lock(groupsMutex);
    GroupMap::iterator i = groups->find(group);
    if (groups->end() != i) {
        int refs = i->second.second;
        RSCDEBUG(logger, "Group object found, ref count: " << refs);
        // if count-1 == 0 sg->leave
        i->second.second = --refs;
        RSCTRACE(logger, "New group ref count: " << i->second.second);
        if (0 == refs) {
            RSCTRACE(logger, "Count is 0, leaving group");
            i->second.first->leave(c);
            RSCINFO(logger, "Left SpreadGroup with id " << group);
        }
    } else {
        RSCWARN(logger, "SpreadGroup with id " << group << " not found in GroupMap");
    }
}

}
}
