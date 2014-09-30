/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2014 Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <rsc/logging/Logger.h>

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {

/**
 * A class that keeps a boost asio service alive as long as this class lives.
 * So it is best maintained in shared_ptr instances
 * (@ref AsioServiceContextPtr).
 *
 * @author jwienke
 */
class RSB_EXPORT AsioServiceContext {
public:
    AsioServiceContext();
    virtual ~AsioServiceContext();

    typedef boost::shared_ptr<boost::asio::io_service> ServicePtr;

    ServicePtr getService();

private:
    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

    rsc::logging::LoggerPtr logger;
    ServicePtr service;
    WorkPtr keepAlive;
    boost::thread thread;
};

typedef boost::shared_ptr<AsioServiceContext> AsioServiceContextPtr;

}
}
