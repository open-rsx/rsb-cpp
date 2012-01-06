/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "SpreadMessage.h"

#include <stdexcept>

#include <string.h>

#include <rsc/logging/Logger.h>

#include <sp.h>

using namespace std;
using namespace rsc::logging;

namespace {
LoggerPtr logger(Logger::getLogger("rsb.spread.SpreadMessage"));
}

namespace rsb {
namespace spread {

SpreadMessage::SpreadMessage() :
    type(OTHER), qos(UNRELIABLE) {
}

SpreadMessage::SpreadMessage(const Type& mt) :
    type(mt), qos(UNRELIABLE) {
}

SpreadMessage::SpreadMessage(const string& d) :
    data(d), type(OTHER), qos(UNRELIABLE) {
}

SpreadMessage::SpreadMessage(const char* buf) :
    data(buf), type(OTHER), qos(UNRELIABLE) {
}

SpreadMessage::~SpreadMessage() {
}

void SpreadMessage::setData(const std::string& doc) {
    data = doc;
}

void SpreadMessage::setData(const char* buf) {
    data = string(buf);
}

string SpreadMessage::getDataAsString() const {
    return data;
}

const char* SpreadMessage::getData() const {
    return data.c_str();
}

int SpreadMessage::getSize() const {
    return data.length();
}

void SpreadMessage::setType(Type mt) {
    type = mt;
}

SpreadMessage::Type SpreadMessage::getType() const {
    return type;
}

void SpreadMessage::addGroup(const std::string& name) {
    if (strlen(name.c_str()) > MAX_GROUP_NAME - 1) {
        throw invalid_argument(
                "Group name '" + name + "' is too long for spread.");
    }
    groups.push_back(name);
}

unsigned int SpreadMessage::getGroupCount() const {
    return groups.size();
}

list<string>::const_iterator SpreadMessage::getGroupsBegin() const {
    return groups.begin();
}

list<string>::const_iterator SpreadMessage::getGroupsEnd() const {
    return groups.end();
}

SpreadMessage::QOS SpreadMessage::getQOS() const {
    return qos;
}

void SpreadMessage::setQOS(const QOS& qos) {
    this->qos = qos;
}

void SpreadMessage::reset() {
    type = OTHER;
    data.clear();
    qos = UNRELIABLE;
    groups.clear();
}

}
}
