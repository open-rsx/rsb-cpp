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

#include "strategies.h"

#include <boost/thread/mutex.hpp>

#include "EventReceivingStrategyFactory.h"
#include "DirectEventReceivingStrategy.h"
#include "ParallelEventReceivingStrategy.h"

#include "EventSendingStrategyFactory.h"
#include "DirectEventSendingStrategy.h"

namespace rsb {
namespace eventprocessing {

bool registered = false;
boost::mutex registrationMutex;

void registerDefaultEventProcessingStrategies() {
    boost::mutex::scoped_lock lock(registrationMutex);
    if (registered) {
        return;
    }
    registered = true;

    {
        EventReceivingStrategyFactory& factory =
                EventReceivingStrategyFactory::getInstance();

        factory.impls().register_("direct",
                &DirectEventReceivingStrategy::create);
        factory.impls().register_("parallel",
                &ParallelEventReceivingStrategy::create);
    }

    {
        EventSendingStrategyFactory& factory
            = EventSendingStrategyFactory::getInstance();

        factory.impls().register_("direct", &DirectEventSendingStrategy::create);
    }
}

}
}
