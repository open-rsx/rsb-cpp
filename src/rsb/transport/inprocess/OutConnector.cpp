#include "OutConnector.h"

using namespace std;

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

    if (args.has("converters")) {
        RSCWARN(logger, "`converters' property found when constructing inprocess::OutConnector. This connector does not support (or require) converters.");
    }
    return new OutConnector();
}

string OutConnector::getClassName() const {
    return "OutConnector";
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
