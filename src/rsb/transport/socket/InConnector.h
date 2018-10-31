/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2012, 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <string>

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Properties.h>

#include "../../eventprocessing/Handler.h"

#include "../InConnector.h"

#include "ConnectorBase.h"
#include "Factory.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace transport {
namespace socket {

/**
 * Instances of this class receive events from a bus that is accessed
 * via a socket connection.
 *
 * The receiving and dispatching of events is done in push mode: each
 * instance has a @ref BusConnection which pushes appropriate events
 * into the instance. The connector deserializes event payloads and
 * pushes the events into handlers (usually objects which implement
 * @ref EventReceivingStrategy).
 *
 * @author jmoringe
 */
class RSB_EXPORT InConnector: public virtual ConnectorBase,
                              public virtual transport::InConnector,
                              public virtual eventprocessing::Handler {
public:
    static rsb::transport::InConnector* create(const rsc::runtime::Properties& args);

    /**
     * @copydoc ConnectorBase::ConnectorBase()
     */
    InConnector(FactoryPtr                    factory,
                ConverterSelectionStrategyPtr converters,
                const std::string&            host,
                unsigned int                  port,
                Server                        server,
                bool                          tcpnodelay,
                bool                          waitForClientDisconnects);

    virtual ~InConnector();

    virtual void activate();
    virtual void deactivate();

    virtual void setScope(const Scope& scope);

    void setQualityOfServiceSpecs(const QualityOfServiceSpec& specs);

    void handle(EventPtr event);

    // Overwrites method in ConnectorBase.
    virtual const std::string getTransportURL() const;
private:
    rsc::logging::LoggerPtr logger;

    void printContents(std::ostream& stream) const;
};

typedef boost::shared_ptr<InConnector> InConnectorPtr;

}
}
}
