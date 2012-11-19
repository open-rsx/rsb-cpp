/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <boost/thread.hpp>

#include <rsb/Informer.h>
#include <rsb/Listener.h>
#include <rsb/Factory.h>
#include <rsb/Handler.h>

using namespace std;
using namespace rsb;
using namespace rsb::filter;

void printData(const std::string& name, boost::shared_ptr<string> e) {
    cout << "[" << name << "] received: " << *e << endl;
}

int main(int /*argc*/, char** /*argv*/) {

    Factory& factory = getFactory();

    for (int i = 0; i < 3; ++i) {

        Scope scope1("/test/dummyOne");
        Scope scope2("/test/dummyTwo");

        Informer<string>::Ptr informer1 = factory.createInformer<string> (
                scope1);
        Informer<string>::Ptr informer2 = factory.createInformer<string> (
                scope2);

        ListenerPtr listener1 = factory.createListener(scope1);
        listener1->addHandler(
                HandlerPtr(
                        new DataFunctionHandler<string> (
                                boost::bind(&printData, "sub1", _1))));

        ListenerPtr listener2 = factory.createListener(scope2);
        listener2->addHandler(
                HandlerPtr(
                        new DataFunctionHandler<string> (
                                boost::bind(&printData, "sub2", _1))));

        informer1->publish(
                Informer<string>::DataPtr(new string("informer1 first message")));
        informer1->publish(
                Informer<string>::DataPtr(
                        new string("informer1 second message")));

        informer2->publish(
                Informer<string>::DataPtr(new string("informer2 first message")));
        informer2->publish(
                Informer<string>::DataPtr(
                        new string("informer2 second message")));

        boost::this_thread::sleep(boost::posix_time::seconds(2));

    }

    return EXIT_SUCCESS;

}
