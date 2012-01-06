/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <rsb/Factory.h>

#include <boost/thread.hpp>

#include <rsc/runtime/TypeStringTools.h>
#include <rsc/logging/LoggerFactory.h>

using namespace std;

using namespace rsb;
using namespace rsb::patterns;

class TestCallback: public Server::Callback<string, string> {
public:
    boost::shared_ptr<string> call(const string& methodName,
                                   boost::shared_ptr<string> input) {
        return boost::shared_ptr<string>(
                new string("reply to '" + *input + "' (method is '" + methodName + "')"));
    }
};

class ErrorProducingCallback: public Server::Callback<string, string> {
public:
    boost::shared_ptr<string> call(const string& /*methodName*/,
                                   boost::shared_ptr<string> /*input*/) {
        throw runtime_error("Intentionally failing.");
    }
};

int main(int /*argc*/, char** /*argv*/) {
    // Use the RSB factory to create a Server instance that provides
    // callable methods under the scope /example/server.
    Factory& factory = Factory::getInstance();
    ServerPtr server = factory.createServer(Scope("/example/server"));

    // Register callable methods which dispatch method calls to
    // instances of TestCallback and ErrorProducingCallback.
    server->registerMethod("methodOne",
                           Server::CallbackPtr(new TestCallback()));
    server->registerMethod("methodTwo",
                           Server::CallbackPtr(new TestCallback()));
    server->registerMethod("methodError",
                           Server::CallbackPtr(new ErrorProducingCallback()));

    // Wait here so incoming method calls can be processed.
    boost::this_thread::sleep(boost::posix_time::seconds(1000));

    return EXIT_SUCCESS;
}
