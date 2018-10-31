/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *               2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "ParallelEventReceivingStrategy.h"

#include <rsc/debug/DebugTools.h>
#include <rsc/runtime/ContainerIO.h>
#include <rsc/misc/langutils.h>

#include "../MetaData.h"
#include "../filter/Filter.h"

using namespace std;

using namespace rsc::runtime;
using namespace rsc::logging;
using namespace rsc::debug;

namespace rsb {
namespace eventprocessing {

EventReceivingStrategy* ParallelEventReceivingStrategy::create(const Properties& props) {
    return new ParallelEventReceivingStrategy(props.getAs<unsigned int>("threads", 5),
                                              props.getAs<bool>("parallelhandlercalls", false));
}

ParallelEventReceivingStrategy::ParallelEventReceivingStrategy(unsigned int numThreads,
        bool parallelHandlerCalls) :
    logger(Logger::getLogger("rsb.eventprocessing.ParallelEventReceivingStrategy")),
    pool(numThreads,
         boost::bind(&ParallelEventReceivingStrategy::deliver, this, _1, _2),
         boost::bind(&ParallelEventReceivingStrategy::filter, this, _1, _2)),
    errorStrategy(ParticipantConfig::ERROR_STRATEGY_LOG) {
    pool.setParallelCalls(parallelHandlerCalls);
    pool.start();
}

ParallelEventReceivingStrategy::~ParallelEventReceivingStrategy() {
    pool.stop();
}

string ParallelEventReceivingStrategy::getClassName() const {
    return "ParallelEventReceivingStrategy";
}

void ParallelEventReceivingStrategy::printContents(ostream& stream) const {
    boost::shared_lock<boost::shared_mutex> filtersLock(filtersMutex);
    boost::recursive_mutex::scoped_lock errorLock(errorStrategyMutex);
    stream << "filters = " << filters << ", errorStrategy = " << errorStrategy;
}

bool ParallelEventReceivingStrategy::filter(rsb::HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Matching event " << e << " for handler " << handler);

    // match event
    try {

        if (!handler->acceptsMethod(e->getMethod())) {
            return false;
        }

        boost::shared_lock<boost::shared_mutex> lock(filtersMutex);
        for (set<filter::FilterPtr>::const_iterator filterIt = filters.begin(); filterIt
                != filters.end(); ++filterIt) {
            if (!(*filterIt)->match(e)) {
                return false;
            }
        }

        return true;

    } catch (const std::exception& ex) {

        stringstream s;
        s << "Exception matching event " << e << " for handler " << handler
                << ":" << endl;
        s << ex.what() << endl;
        s << DebugTools::newInstance()->exceptionInfo(ex);

        handleDispatchError(s.str());

    } catch (...) {

        stringstream s;
        s << "Catch-all handler called matching event " << e << " for handler "
                << handler << endl;
        DebugToolsPtr tool = DebugTools::newInstance();
        vector<string> trace = tool->createBacktrace();
        s << tool->formatBacktrace(trace);

        handleDispatchError(s.str());

    }

    return false;

}

void ParallelEventReceivingStrategy::handleDispatchError(const string& message) {

    boost::recursive_mutex::scoped_lock strategyLock(errorStrategyMutex);
    switch (errorStrategy) {
    case ParticipantConfig::ERROR_STRATEGY_LOG:
        RSCERROR(logger, message)
        ;
        break;
    case ParticipantConfig::ERROR_STRATEGY_PRINT:
        cerr << message << endl;
        break;
    case ParticipantConfig::ERROR_STRATEGY_EXIT:
        cerr << message << endl;
        exit(1);
        break;
    default:
        RSCWARN(logger, "Unknown error strategy: " << errorStrategy)
        ;
        RSCERROR(logger, message)
        ;
        break;
    }

}

void ParallelEventReceivingStrategy::deliver(rsb::HandlerPtr handler, EventPtr e) {
    RSCDEBUG(logger, "Delivering event " << e << " to handler " << handler);

    try {

        handler->handle(e);

    } catch (const std::exception& ex) {

        stringstream s;
        s << "Exception dispatching event " << e << " to handler " << handler
                << ":" << endl;
        s << ex.what() << endl;
        s << DebugTools::newInstance()->exceptionInfo(ex);

        handleDispatchError(s.str());

    } catch (...) {

        stringstream s;
        s << "Catch-all handler called dispatching event " << e
                << " to handler " << handler << endl;
        DebugToolsPtr tool = DebugTools::newInstance();
        vector<string> trace = tool->createBacktrace();
        s << tool->formatBacktrace(trace);

        handleDispatchError(s.str());

    }

}

void ParallelEventReceivingStrategy::handle(EventPtr event) {
    event->mutableMetaData().setDeliverTime(rsc::misc::currentTimeMicros());
    pool.push(event);
}

void ParallelEventReceivingStrategy::addHandler(rsb::HandlerPtr handler,
        const bool& /*wait*/) {
    // wait can be ignored because the pool always ensures this
    pool.registerReceiver(handler);
}

void ParallelEventReceivingStrategy::removeHandler(rsb::HandlerPtr handler,
        const bool& /*wait*/) {
    // wait can be ignored because the pool always ensures this
    pool.unregisterReceiver(handler);
}

void ParallelEventReceivingStrategy::addFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);
    filters.insert(filter);
}

void ParallelEventReceivingStrategy::removeFilter(filter::FilterPtr filter) {
    boost::unique_lock<boost::shared_mutex> lock(filtersMutex);
    filters.erase(filter);
}


void ParallelEventReceivingStrategy::setHandlerErrorStrategy(
        const ParticipantConfig::ErrorStrategy& strategy) {
    boost::recursive_mutex::scoped_lock lock(errorStrategyMutex);
    this->errorStrategy = strategy;
}

}
}
