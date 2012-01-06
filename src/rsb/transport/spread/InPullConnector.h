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

#pragma once

#include <string>

#include <rsc/runtime/Properties.h>

#include "../InPullConnector.h"
#include "../ConverterSelectingConnector.h"
#include "SpreadConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * This class implements pull-style event receiving for the Spread
 * transport.
 *
 * @author jmoringe
 */
class RSB_EXPORT InPullConnector: public transport::InPullConnector,
                                  public transport::ConverterSelectingConnector<std::string> {
public:
    InPullConnector(ConverterSelectionStrategyPtr converters,
                    const std::string& host = defaultHost(),
                    unsigned int port = defaultPort());
    virtual ~InPullConnector();

    std::string getClassName() const;
    void printContents(std::ostream& stream) const;

    void setScope(const Scope& scope);

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec& specs);

    EventPtr raiseEvent(bool block);

    static transport::InPullConnector* create(const rsc::runtime::Properties& args);
private:
    rsc::logging::LoggerPtr logger;

    bool active;

    SpreadConnectorPtr connector;
    boost::shared_ptr<Scope> activationScope;
};

}
}
