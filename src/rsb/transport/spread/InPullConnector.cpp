/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "../../Scope.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::converter;

namespace rsb {
namespace transport {
namespace spread {

transport::InPullConnector* InPullConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.transport.spread.InPullConnector");
    RSCDEBUG(logger, "creating InPullConnector with properties " << args);

    return new InPullConnector(args.get<ConverterSelectionStrategyPtr> ("converters"),
                               args.get<string>                        ("host", defaultHost()),
                               args.getAs<unsigned int>                ("port", defaultPort()));
}

InPullConnector::InPullConnector(ConverterSelectionStrategyPtr converters,
                                 const string&                 host,
                                 unsigned int                  port) :
    logger(Logger::getLogger("rsb.transport.spread.InPullConnector")),
    active(false),
    connector(new SpreadConnector(host, port)),
    processor(converters) {
}

InPullConnector::~InPullConnector() {
    if (this->active) {
        deactivate();
    }
}

void InPullConnector::printContents(ostream& stream) const {
    stream << "active = " << this->active
           << ", connector = " << this->connector;
}

void InPullConnector::activate() {
    this->connector->activate();
    this->active = true;

    // check that scope is applied
    if (activationScope) {
        setScope(*activationScope);
        activationScope.reset();
    }

}

void InPullConnector::deactivate() {
    this->connector->deactivate();
}

void InPullConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& specs) {
    this->connector->setQualityOfServiceSpecs(specs);
}

void InPullConnector::setScope(const Scope& scope) {
    if (!active) {
        this->activationScope.reset(new Scope(scope));
    } else {
        this->connector->join(this->connector->makeGroupName(scope));
    }
}

EventPtr InPullConnector::raiseEvent(bool block) {
    assert(block);

    SpreadMessagePtr message(new SpreadMessage());
    EventPtr event;
    while (true) {
        this->connector->receive(message);
        assert(message);
        if (message->getType() != SpreadMessage::REGULAR) {
            continue;
        }
        event = this->processor.processMessage(message);
        if (event) {
            return event;
        }
    };
    // This should never happen so far unless non-blocking (not implemented so far)
    return EventPtr();
}

}
}
}
