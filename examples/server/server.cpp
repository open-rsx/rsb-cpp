/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2011, 2012, 2013, 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

// mark-start::body
#include <boost/thread.hpp>

#include <rsc/misc/SignalWaiter.h>

#include <rsb/Factory.h>

using namespace rsb;
using namespace rsb::patterns;

class EchoCallback: public LocalServer::Callback<std::string, std::string> {
public:
    boost::shared_ptr<std::string> call(const std::string& /*methodName*/,
                                        boost::shared_ptr<std::string> input) {
        return input;
    }
};

class VoidVoidCallback: public LocalServer::Callback<void, void> {
    void call(const std::string& /*methodName*/) {
        std::cout << "void-void method called" << std::endl;
    }
};

int main(int /*argc*/, char** /*argv*/) {

    rsc::misc::initSignalWaiter();

    // Use the RSB factory to create a Server instance that provides
    // callable methods under the scope /example/server.
    Factory& factory = getFactory();
    LocalServerPtr server = factory.createLocalServer("/example/server");

    // Register method with name and implementing callback object.
    server->registerMethod("echo", LocalServer::CallbackPtr(new EchoCallback()));
    server->registerMethod("void", LocalServer::CallbackPtr(new VoidVoidCallback()));

    // Wait here so incoming method calls can be processed.
    return rsc::misc::suggestedExitCode(rsc::misc::waitForSignal());

}
// mark-end::body
