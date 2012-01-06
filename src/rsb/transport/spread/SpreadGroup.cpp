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

#include "SpreadGroup.h"

#include <stdexcept>

#include <sp.h>

#include <rsc/logging/Logger.h>

#include "../../CommException.h"

using namespace std;
using namespace rsc::logging;

namespace rsb {
namespace spread {

SpreadGroup::SpreadGroup(const string& n) :
    name(n), logger(Logger::getLogger("rsb.spread.SpreadGroup")) {
    RSCDEBUG(logger, "new spread group object, group name: " << n);
}

SpreadGroup::~SpreadGroup() {
    RSCDEBUG(logger, "destructor called for group object: " << name);
}

string SpreadGroup::getName() const {
    return name;
}

void SpreadGroup::join(SpreadConnectionPtr con) {

    if (!con->isActive()) {
        throw runtime_error("Spread connection is not active");
    }

    int retCode = SP_join(*con->getMailbox(), name.c_str());
    handleRetCode(retCode, "joining");

}

void SpreadGroup::leave(SpreadConnectionPtr con) {

    if (!con->isActive()) {
        throw CommException("Connection is not active, cannot leave " + name);
    }

    int retCode = SP_leave(*con->getMailbox(), name.c_str());
    handleRetCode(retCode, "leaving");

}

void SpreadGroup::handleRetCode(const int& retCode,
        const std::string& actionName) {

    if (!retCode) {
        RSCDEBUG(logger, actionName << " spread group with name " << name << " successful");
    } else {
        stringstream msg;
        msg << "Got error while " << actionName << " spread group '" << name
                << "': ";
        switch (retCode) {
        case ILLEGAL_GROUP:
            msg << "ILLEGAL_GROUP";
            break;
        case ILLEGAL_SESSION:
            msg << "ILLEGAL_SESSION";
            break;
        case CONNECTION_CLOSED:
            msg << "CONNECTION_CLOSED";
            break;
        default:
            msg << "Unknown spread error with code " << retCode;
            break;
        }
        RSCERROR(logger, "Error " << actionName << " spread group: " << msg.str());
        throw CommException(msg.str());
    }

}

}
}
