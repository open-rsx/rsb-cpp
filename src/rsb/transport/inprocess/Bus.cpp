#include "Bus.h"

using namespace std;

using namespace rsc::logging;

using namespace rsb::transport;

namespace rsb {
namespace inprocess {

Bus::Bus() :
    logger(Logger::getLogger("rsb.inprocess.Bus")) {
}

Bus::~Bus() {
    if (!this->sinks.empty()) {
        RSCWARN(logger, "" << this->sinks.size() << " non-empty scopes when destructing");
    }
}

void Bus::addSink(InConnectorPtr sink) {
    boost::recursive_mutex::scoped_lock lock(this->mutex);

    Scope scope = sink->getScope();
    RSCDEBUG(logger, "Adding sink " << sink << " to scope " << scope);
    SinkList& connectors = this->sinks[scope];
    connectors.push_back(sink);
}

void Bus::removeSink(InConnector* sink) {
    boost::recursive_mutex::scoped_lock lock(this->mutex);

    Scope scope = sink->getScope();
    RSCDEBUG(logger, "Removing sink " << sink << " from scope " << scope);
    SinkList& connectors = this->sinks[scope];
    RSCDEBUG(logger, "Scope " << scope << " has " << connectors.size() << " connectors");
    for (SinkList::iterator it = connectors.begin(); it != connectors.end(); ++it) {
        // If the weak pointer is dangling, we found our
        // sink. Otherwise, we can just check the pointer.
        InConnectorPtr ptr = it->lock();
        if (!ptr || (ptr.get() == sink)) {
            RSCDEBUG(logger, "Found connector " << sink << " in scope " << scope);
            connectors.erase(it);
            break;
        }
    }
    RSCDEBUG(logger, "Scope " << scope << " has " << connectors.size() << " connectors");
    if (connectors.empty()) {
        RSCDEBUG(logger, "Remove empty scope " << scope);
        this->sinks.erase(scope);
    }
}

void Bus::push(EventPtr event) {
    boost::recursive_mutex::scoped_lock lock(this->mutex);

    RSCDEBUG(logger, "Delivering event " << event);

    vector<Scope> scopes = event->getScope().superScopes(true);
    RSCDEBUG(logger, "Relevant scopes " << scopes);

    for (vector<Scope>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
        SinkMap::const_iterator it_ = this->sinks.find(*it);
        if (it_ != this->sinks.end()) {
            const SinkList& connectors = it_->second;

            for (SinkList::const_iterator it__ = connectors.begin(); it__ != connectors.end(); ++it__) {
                InConnectorPtr connector = it__->lock();
                if (connector) {
                    RSCDEBUG(logger, "Delivering to " << connector << " in " << *it);
                    connector->push(event);
                }
            }
        }
    }
}

}
}
