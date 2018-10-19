/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012, 2013, 2015 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "Factory.h"
#include "../../MetaData.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport {
namespace socket {

transport::InPushConnector* InPushConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.transport.socket.InPushConnector");
    RSCDEBUG(logger, "Creating InPushConnector with properties " << args);

    return new InPushConnector(getDefaultFactory(),
                               args.get<ConverterSelectionStrategyPtr>("converters"),
                               args.get<string>                       ("host",       DEFAULT_HOST),
                               args.getAs<unsigned int>               ("port",       DEFAULT_PORT),
                               args.getAs<Server>                     ("server",     SERVER_AUTO),
                               args.getAs<bool>                       ("tcpnodelay", true),
                               args.getAs<bool>                       ("wait", true));
}

InPushConnector::InPushConnector(FactoryPtr                    factory,
                                 ConverterSelectionStrategyPtr converters,
                                 const string&                 host,
                                 unsigned int                  port,
                                 Server                        server,
                                 bool                          tcpnodelay,
                                 bool                          waitForClientDisconnects) :
    ConnectorBase(factory, converters, host, port, server, tcpnodelay,
                  waitForClientDisconnects),
    InConnector(factory, converters, host, port, server, tcpnodelay,
                waitForClientDisconnects),
    logger(Logger::getLogger("rsb.transport.socket.InPushConnector")) {
}

InPushConnector::~InPushConnector() {
}

void InPushConnector::setScope(const Scope& scope) {
    ConnectorBase::setScope(scope);
}

void InPushConnector::handle(EventPtr busEvent) {
    if (!this->active) {
        throw std::runtime_error("Cannot handle events when not active");
    }

    // busEvent is an intermediate object. The deserialization of the
    // payload still has to be performed.
    EventPtr event(new Event(*busEvent));

    event->mutableMetaData().setReceiveTime();

    // Extract the serialized data and wire-schema from the
    // intermediate event.
    boost::shared_ptr<string> wireData = static_pointer_cast<string>(event->getData());
    string wireSchema = event->getMetaData().getUserInfo("rsb.wire-schema");

    // Apply the configured converter.
    AnnotatedData d
        = getConverter(wireSchema)->deserialize(wireSchema, *wireData);
    event->setData(d.second);
    event->setType(d.first);

    // Dispatch the final result to all handlers (typically a single
    // object implementing the EventReceivingStrategy interface).
    for (HandlerList::iterator it = this->handlers.begin(); it
            != this->handlers.end(); ++it) {
        (*it)->handle(event);
    }
}

const std::string InPushConnector::getTransportURL() const {
    return ConnectorBase::getTransportURL();
}

}
}
}
