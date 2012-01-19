/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "InConnector.h"

#include "Bus.h"
#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

InConnector::InConnector() :
    logger(Logger::getLogger("rsb.inprocess.InConnector")), active(false) {
}

transport::InPushConnector* InConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.InConnector");
    RSCDEBUG(logger, "Creating InConnector with properties " << args);

    // Seems to have confused some users.
    // See https://code.cor-lab.de/issues/649
    // if (args.has("converters")) {
    //   RSCWARN(logger, "`converters' property found when constructing inprocess::InConnector. This connector does not support (or require) converters.");
    // }
    return new InConnector();
}

InConnector::~InConnector() {
    if (this->active) {
        deactivate();
    }
}

string InConnector::getClassName() const {
    return "InConnector";
}

void InConnector::printContents(ostream& stream) const {
    stream << "scope = " << scope;
}

Scope InConnector::getScope() const {
    return this->scope;
}

void InConnector::setScope(const Scope& scope) {
    if (this->active) {
        throw std::runtime_error("Cannot set scope while active");
    }

    this->scope = scope;
}

void InConnector::activate() {
    RSCDEBUG(logger, "Activating");
    Bus::getInstance().addSink(boost::dynamic_pointer_cast<InConnector>(
            shared_from_this()));
    this->active = true;
}

void InConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");
    Bus::getInstance().removeSink(this);
}

void InConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

void InConnector::handle(EventPtr event) {

    /** TODO(jmoringe, 2011-11-07): This ensures not overwriting
     * earlier receive timestamp added by different
     * connector. However, thread-safety issue remains.  */
    if (event->getMetaData().getReceiveTime() == 0) {
        event->mutableMetaData().setReceiveTime();
    }
    for (HandlerList::iterator it = this->handlers.begin(); it
            != this->handlers.end(); ++it) {
        (*it)->handle(event);
    }
}

}
}
