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

#include "Handler.h"

using namespace std;

namespace rsb {

Handler::Handler(const string& method) {
    if (!method.empty()) {
        methods.insert(method);
    }
}

Handler::Handler(const set<string>& methods) :
    methods(methods) {
}

Handler::~Handler() {
}

set<string> Handler::getMethods() const {
    return methods;
}

bool Handler::acceptsMethod(const string& method) const {
    return this->methods.empty()
        || (this->methods.find(method) != this->methods.end());
}

EventFunctionHandler::EventFunctionHandler(const EventFunction& function,
        const string& method) :
    Handler(method), function(function) {
}

string EventFunctionHandler::getClassName() const {
    return "EventFunctionHandler";
}

void EventFunctionHandler::printContents(ostream& stream) const {
    stream << "function = " << function;
}

void EventFunctionHandler::handle(EventPtr event) {
    this->function(event);
}

}
