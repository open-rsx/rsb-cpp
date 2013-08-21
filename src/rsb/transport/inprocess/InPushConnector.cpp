/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012, 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "InPushConnector.h"

#include "../../Event.h"
#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport{
namespace inprocess {

InPushConnector::InPushConnector() :
    logger(Logger::getLogger("rsb.transport.inprocess.InPushConnector")) {
}

transport::InPushConnector* InPushConnector::create(const Properties& /*args*/) {
    // Seems to have confused some users.
    // See https://code.cor-lab.de/issues/649
    // if (args.has("converters")) {
    //   RSCWARN(logger, "`converters' property found when constructing inprocess::InPushConnector. This connector does not support (or require) converters.");
    // }
    return new InPushConnector();
}

InPushConnector::~InPushConnector() {
}

void InPushConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

void InPushConnector::handle(EventPtr event) {
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
}
