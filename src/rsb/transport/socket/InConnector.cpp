/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2012, 2013, 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "../../MetaData.h"

#include "Factory.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport {
namespace socket {

transport::InConnector* InConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.transport.socket.InConnector");
    RSCDEBUG(logger, "Creating InConnector with properties " << args);

    return new InConnector(getDefaultFactory(),
                           args.get<ConverterSelectionStrategyPtr>("converters"),
                           args.get<string>                       ("host",       DEFAULT_HOST),
                           args.getAs<unsigned int>               ("port",       DEFAULT_PORT),
                           args.getAs<Server>                     ("server",     SERVER_AUTO),
                           args.getAs<bool>                       ("tcpnodelay", true),
                           args.getAs<bool>                       ("wait",       true));
}

InConnector::InConnector(FactoryPtr                    factory,
                         ConverterSelectionStrategyPtr converters,
                         const string&                 host,
                         unsigned int                  port,
                         Server                        server,
                         bool                          tcpnodelay,
                         bool                          waitForClientDisconnects) :
    ConnectorBase(factory, converters, host, port, server, tcpnodelay,
                  waitForClientDisconnects),
    logger(Logger::getLogger("rsb.transport.socket.InConnector")) {
}

InConnector::~InConnector() {
    if (this->active) {
        deactivate();
    }
}

void InConnector::activate() {
    ConnectorBase::activate();

    RSCDEBUG(logger, "Activating");

    getBus()->addSink(dynamic_pointer_cast<InConnector>(enable_shared_from_this<rsb::transport::InConnector>::shared_from_this()));
}

void InConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");

    BusPtr bus = getBus();
    if (bus) {
        RSCDEBUG(logger, "Removing ourselves from sink list of bus " << bus);
        bus->removeSink(this);
    }

    ConnectorBase::deactivate();
}

void InConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
    RSCDEBUG(logger, "Quality of service not implemented");
}

void InConnector::printContents(ostream& stream) const {
    stream << "scope = " << getScope();
}

void InConnector::setScope(const Scope& scope) {
    ConnectorBase::setScope(scope);
}

void InConnector::handle(EventPtr busEvent) {
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

const std::string InConnector::getTransportURL() const {
    return ConnectorBase::getTransportURL();
}

}
}
}
