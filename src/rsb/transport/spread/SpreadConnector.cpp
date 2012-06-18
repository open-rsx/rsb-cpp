/* ============================================================
 *
 * This file is a part of the RSB project
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

#include <string.h>
#include <math.h>

#include <rsc/misc/Registry.h>

#include "../../CommException.h"
#include "../../UnsupportedQualityOfServiceException.h"
#include "../../util/MD5.h"
#include "../../Scope.h"

#include "../../converter/Converter.h"

#include "SpreadConnection.h"
#include "SpreadConnector.h"

#include <sp.h>

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb;
using namespace rsb::util;
using namespace rsb::transport;

namespace rsb {
namespace spread {

const SpreadConnector::QoSMap SpreadConnector::qosMapping =
        SpreadConnector::buildQoSMapping();

SpreadConnector::SpreadConnector(const string& host, unsigned int port) {
    init(host, port);
}

void SpreadConnector::init(const string& host, unsigned int port) {
    this->logger = Logger::getLogger("rsb.spread.SpreadConnector");
    RSCDEBUG(logger, "SpreadConnector::init() entered");
    this->activated = false;
    // TODO ConnectionPool for SpreadConnections?!?
    // TODO Send Message over Managing / Introspection Channel
    this->con = SpreadConnectionPtr(
            new SpreadConnection(id.getIdAsString(), host, port));
    this->memberships = MembershipManagerPtr(new MembershipManager());
    setQualityOfServiceSpecs(QualityOfServiceSpec());
}

void SpreadConnector::activate() {
    // connect to spread
    this->con->activate();
    this->activated = true;
}

void SpreadConnector::deactivate() {
    RSCDEBUG(logger, "deactivate() entered");
    if (this->con->isActive()) {
        this->con->deactivate();
    }
    // memberships->leaveAll();
    RSCTRACE(logger, "deactivate() finished"); // << *id);
    this->activated = false;
}

void SpreadConnector::join(const string& name) {
    this->memberships->join(name, this->con);
}

void SpreadConnector::leave(const string& name) {
    this->memberships->leave(name, this->con);
}

void SpreadConnector::send(const SpreadMessage& msg) {
    this->con->send(msg);
}

void SpreadConnector::receive(SpreadMessagePtr msg) {
    this->con->receive(msg);
}

SpreadConnector::~SpreadConnector() {
    if (this->activated) {
        deactivate();
    }
}

SpreadConnectionPtr SpreadConnector::getConnection() {
    return this->con;
}

SpreadMessage::QOS SpreadConnector::getMessageQoS() const {
    return this->messageQoS;
}

SpreadConnector::QoSMap SpreadConnector::buildQoSMapping() {
    map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> unorderedMap;
    unorderedMap.insert(
            make_pair(QualityOfServiceSpec::UNRELIABLE,
                    SpreadMessage::UNRELIABLE));
    unorderedMap.insert(
            make_pair(QualityOfServiceSpec::RELIABLE, SpreadMessage::RELIABLE));

    map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS> orderedMap;
    orderedMap.insert(
            make_pair(QualityOfServiceSpec::UNRELIABLE, SpreadMessage::FIFO));
    orderedMap.insert(
            make_pair(QualityOfServiceSpec::RELIABLE, SpreadMessage::FIFO));

    map<QualityOfServiceSpec::Ordering, map<QualityOfServiceSpec::Reliability,
            SpreadMessage::QOS> > table;
    table.insert(make_pair(QualityOfServiceSpec::UNORDERED, unorderedMap));
    table.insert(make_pair(QualityOfServiceSpec::ORDERED, orderedMap));

    return table;
}

void SpreadConnector::setQualityOfServiceSpecs(
        const QualityOfServiceSpec& specs) {

    QoSMap::const_iterator orderMapIt = qosMapping.find(specs.getOrdering());
    if (orderMapIt == qosMapping.end()) {
        throw UnsupportedQualityOfServiceException("Unknown ordering", specs);
    }
    map<QualityOfServiceSpec::Reliability, SpreadMessage::QOS>::const_iterator
            mapIt = orderMapIt->second.find(specs.getReliability());
    if (mapIt == orderMapIt->second.end()) {
        throw UnsupportedQualityOfServiceException("Unknown reliability", specs);
    }

    messageQoS = mapIt->second;

    RSCDEBUG(logger, "Selected new message type " << messageQoS);
}

const vector<string>& SpreadConnector::makeGroupNames(
        const Scope& scope) const {

    boost::upgrade_lock<boost::shared_mutex> lock(groupNameCacheMutex);

    GroupNameCache::const_iterator it = this->groupNameCache.find(scope);
    if (it != this->groupNameCache.end()) {
        return it->second;
    }

    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

    // avoid flooding the cache
    // rationale: normally there is only a limited amount of group names used in
    // a system. In other cases we assume that the group names are created
    // dynamically and in this case the cache won't help at all
    if (groupNameCache.size() > 300) {
        RSCDEBUG(logger, "Flushing group name cache");
        groupNameCache.clear();
    }

    // Warm-up cache
    vector<string>& cacheItem = this->groupNameCache[scope];
    vector<Scope> scopes = scope.superScopes(true);
    for (vector<Scope>::const_iterator scopeIt = scopes.begin(); scopeIt
            != scopes.end(); ++scopeIt) {
        cacheItem.push_back(this->makeGroupName(*scopeIt));
    }

    return cacheItem;

}

std::string SpreadConnector::makeGroupName(const Scope& scope) const {
    return MD5(scope.toString()).toHexString().substr(0, MAX_GROUP_NAME - 1);
}

}
}
