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
using namespace rsc::threading;

namespace rsb {
namespace transport{
namespace inprocess {

InPullConnector::InPullConnector() :
    logger(Logger::getLogger("rsb.transport.inprocess.InPullConnector")) {
}

transport::InPullConnector* InPullConnector::create(const Properties& args) {
    return new InPullConnector();
}

InPullConnector::~InPullConnector() {
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

    this->queue.push(event);
}

EventPtr InPullConnector::raiseEvent(bool block) {
    if (block) {
        return this->queue.pop();
    } else {
        try {
            return this->queue.tryPop();
        } catch (const QueueEmptyException&) {
            return EventPtr();
        }
    }
}

}
}
}
