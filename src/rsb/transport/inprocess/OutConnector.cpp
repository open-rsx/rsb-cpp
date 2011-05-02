#include "OutConnector.h"

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

OutConnector::OutConnector() :
    bus(Bus::getInstance()) {
}

rsb::transport::OutConnector* OutConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.OutConnector");
    RSCDEBUG(logger, "Creating OutConnector with properties " << args);

    return new OutConnector();
}

void OutConnector::activate() {
}

void OutConnector::deactivate() {
}

void OutConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {
}

void OutConnector::handle(EventPtr event) {
    this->bus.handle(event);
}

}
}
