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

#include "InPushConnector.h"

#include <rsc/threading/ThreadedTaskExecutor.h>

#include "../../Scope.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;
using namespace rsc::threading;

using namespace rsb::eventprocessing;
using namespace rsb::converter;

namespace rsb {
namespace transport {
namespace spread {

transport::InPushConnector* InPushConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.transport.spread.InConnector");
    RSCDEBUG(logger, "creating InConnector with properties " << args);

    return new InPushConnector(args.get<ConverterSelectionStrategyPtr>("converters"),
                               args.get<string>                       ("host", defaultHost()),
                               args.getAs<unsigned int>               ("port", defaultPort()));
}

InPushConnector::InPushConnector(const ConverterSelectionStrategyPtr converters,
                                 const string&                       host,
                                 unsigned int                        port) :
    transport::ConverterSelectingConnector<string>(converters), logger(
            Logger::getLogger("rsb.transport.spread.InPushConnector")), active(false),
            connector(new SpreadConnector(host, port)) {
    this->exec = TaskExecutorPtr(new ThreadedTaskExecutor);
    this->rec = boost::shared_ptr<ReceiverTask>(new ReceiverTask(
            this->connector->getConnection(), HandlerPtr(), this));
}

InPushConnector::~InPushConnector() {
    if (this->active) {
        deactivate();
    }
}

void InPushConnector::printContents(ostream& stream) const {
    stream << "active = " << this->active
           << "connector = " << this->connector;
}

void InPushConnector::activate() {
    this->connector->activate();

    // (re-)start threads
    this->exec->schedule(rec);
    //this->exec->schedule(st);
    this->active = true;

    // check that scope is applied
    if (activationScope) {
        setScope(*activationScope);
        activationScope.reset();
    }

}

void InPushConnector::deactivate() {
    this->rec->cancel();
    if (this->connector->getConnection()->isActive()) {
        this->connector->getConnection()->interruptReceive();
        this->rec->waitDone();
    }
    this->connector->deactivate();
    this->active = false;
}

void InPushConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& specs) {
    this->connector->setQualityOfServiceSpecs(specs);
    if (specs.getReliability() >= QualityOfServiceSpec::RELIABLE) {
        this->rec->setPruning(false);
    } else {
        this->rec->setPruning(true);
    }
}

void InPushConnector::addHandler(HandlerPtr handler) {
    assert(this->handlers.empty());
    transport::InPushConnector::addHandler(handler);
    this->rec->setHandler(this->handlers.front());
}

void InPushConnector::removeHandler(HandlerPtr handler) {
    transport::InPushConnector::addHandler(handler);
    this->rec->setHandler(HandlerPtr());
}

void InPushConnector::setScope(const Scope& scope) {
    if (!active) {
        activationScope.reset(new Scope(scope));
    } else {
        connector->join(connector->makeGroupName(scope));
    }
}

}
}
}
