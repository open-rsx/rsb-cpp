/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "InPullConnector.h"

#include "Bus.h"
#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

InPullConnector::InPullConnector() :
    logger(Logger::getLogger("rsb.inprocess.InPullConnector")), active(false) {
}

transport::InPullConnector* InPullConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.InPullConnector");
    RSCDEBUG(logger, "Creating InPullConnector with properties " << args);
    return new InPullConnector();
}

InPullConnector::~InPullConnector() {
    if (this->active) {
        deactivate();
    }
}

string InPullConnector::getClassName() const {
    return "InPullConnector";
}

void InPullConnector::printContents(ostream& stream) const {
    stream << "scope = " << scope;
}

Scope InPullConnector::getScope() const {
    return this->scope;
}

void InPullConnector::setScope(const Scope& scope) {
    if (this->active) {
        throw std::runtime_error("Cannot set scope while active");
    }

    this->scope = scope;
}

void InPullConnector::activate() {
    RSCDEBUG(logger, "Activating");
    /* TODO(jmoringe, 2012-07-03): implement once we have a common
     * base-class for In{Push,Pull}Connectors */
    /*Bus::getInstance().addSink(boost::dynamic_pointer_cast<InConnector>(
      shared_from_this()));*/
    this->active = true;
}

void InPullConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");
    //Bus::getInstance().removeSink(this);
    /* TODO(jmoringe, 2012-07-03): implement once we have a common
     * base-class for In{Push,Pull}Connectors */
}

void InPullConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

void InPullConnector::handle(EventPtr event) {

    /** TODO(jmoringe, 2011-11-07): This ensures not overwriting
     * earlier receive timestamp added by different
     * connector. However, thread-safety issue remains.  */
    if (event->getMetaData().getReceiveTime() == 0) {
        event->mutableMetaData().setReceiveTime();
    }
    /* TODO(jmoringe, 2012-07-03): queue event for later pull-retrieval */
}

EventPtr InPullConnector::raiseEvent(bool /*block*/) {
}

}
}
