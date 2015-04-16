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

#include "AsioServiceContext.h"

using namespace std;
using namespace boost::asio;
using namespace rsc::logging;

namespace rsb {
namespace transport {

AsioServiceContext::AsioServiceContext() :
        logger(Logger::getLogger("rsb.transport.socket.AsioServiceContext")), service(
                new io_service), keepAlive(new io_service::work(*service)), thread(
                boost::bind(&boost::asio::io_service::run, service)) {
    RSCINFO(logger, "Started service thread");
}

AsioServiceContext::~AsioServiceContext() {
    RSCINFO(logger, "Stopping service thread");
    this->keepAlive.reset();
    if (boost::this_thread::get_id() != this->thread.get_id()) {
        this->thread.join();
    }
    RSCINFO(logger, "Stopped service thread");
}

AsioServiceContext::ServicePtr AsioServiceContext::getService() {
    return service;
}

}
}
