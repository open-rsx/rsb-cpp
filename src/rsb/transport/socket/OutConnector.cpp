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

#include "OutConnector.h"

#include "Bus.h"
#include "../../MetaData.h"
#include "../../EventId.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace transport {
namespace socket {

transport::OutConnector* OutConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.transport.socket.OutConnector");
    RSCDEBUG(logger, "Creating OutConnector with properties " << args);

    return new OutConnector(getDefaultFactory(),
                            args.get<ConverterSelectionStrategyPtr>("converters"),
                            args.get<string>                       ("host",       DEFAULT_HOST),
                            args.getAs<unsigned int>               ("port",       DEFAULT_PORT),
                            args.getAs<Server>                     ("server",     SERVER_AUTO),
                            args.getAs<bool>                       ("tcpnodelay", true));
}

OutConnector::OutConnector(FactoryPtr                    factory,
                           ConverterSelectionStrategyPtr  converters,
                           const string&                  host,
                           unsigned int                   port,
                           Server                         server,
                           bool                           tcpnodelay) :
    ConnectorBase(factory, converters, host, port, server, tcpnodelay),
    logger(Logger::getLogger("rsb.transport.socket.OutConnector")){
}

OutConnector::~OutConnector() {

}

void OutConnector::setScope(const Scope& scope) {
    ConnectorBase::setScope(scope);
}

void OutConnector::activate() {
    ConnectorBase::activate();
}

void OutConnector::deactivate() {
    ConnectorBase::deactivate();
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
    RSCWARN(logger, "Quality of service not implemented");
}

void OutConnector::handle(EventPtr event) {
    event->mutableMetaData().setSendTime();

    EventPtr busEvent(new Event(*event));
    boost::shared_ptr<string> wireData(new string());
    AnnotatedData d(busEvent->getType(), busEvent->getData());
    string wireSchema = getConverter(busEvent->getType())->serialize(d, *wireData);
    busEvent->setData(wireData);
    busEvent->mutableMetaData().setUserInfo("rsb.wire-schema", wireSchema);

    getBus()->handle(busEvent);
}

}
}
}
