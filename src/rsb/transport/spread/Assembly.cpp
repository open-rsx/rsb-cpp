#include "Assembly.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

using namespace rsb::protocol;

namespace rsb {
namespace spread {

Assembly::Assembly(rsb::protocol::NotificationPtr n) :
    logger(Logger::getLogger("rsb.spread.Assembly[" + n->id() + "]")), receivedParts(0) {
    store.resize(n->num_data_parts() + 1);
    add(n);
}

Assembly::~Assembly() {
}

string Assembly::getData(const unsigned int &i) const {
    return this->store[i]->data().binary();
}

string *Assembly::getCompleteData() const {
    RSCTRACE(logger, "Joining data parts");
    assert(isComplete());
    // Concatenate data parts
    string* result = new string(getData(0));
    for (unsigned int i = 1; i < this->store.size(); ++i) {
        result->append(getData(i));
    }
    return result;
}

unsigned int Assembly::add(NotificationPtr n) {
    RSCTRACE(logger, "Adding notification " << n->id()
             << " (part " << n->data_part() << "/" << this->store.size()
             << ") to assembly");

    store[n->data_part()] = n;
    return receivedParts++;
}

bool Assembly::isComplete() const {
    return this->receivedParts == this->store.size();
}

AssemblyPool::AssemblyPool() :
    logger(Logger::getLogger("rsb.spread.AssemblyPool")) {
}

shared_ptr<string> AssemblyPool::add(NotificationPtr notification) {
    Pool::iterator it = this->pool.find(notification->id());
    string *result = 0;
    if (it != this->pool.end()) {
        // Push message to existing Assembly
        AssemblyPtr assembly = it->second;
        RSCTRACE(logger, "Adding notification " << notification->id() << " to existing assembly "
                 << assembly);
        assembly->add(notification);
        if (assembly->isComplete()) {
            result = assembly->getCompleteData();
            this->pool.erase(it);
        }
    } else {
        // Create new Assembly
        RSCTRACE(logger, "Creating new assembly for notification " << notification->id());
        this->pool.insert(make_pair(notification->id(), AssemblyPtr(new Assembly(notification))));
    }
    RSCTRACE(logger, "dataPool size: " << this->pool.size());

    return shared_ptr<string>(result);
}


}
}
