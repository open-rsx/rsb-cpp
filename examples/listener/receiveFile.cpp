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

#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <math.h>
#include <ctime>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <rsc/misc/SignalWaiter.h>

#include <rsc/logging/Logger.h>

#include <rsb/Listener.h>
#include <rsb/Handler.h>
#include <rsb/filter/ScopeFilter.h>
#include <rsb/Factory.h>
#include <rsb/MetaData.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace rsb::filter;
using namespace boost::posix_time;

class MyDataHandler: public Handler {
public:
    MyDataHandler() {
        counter = 0;
    }

    string getClassName() const {
        return "MyDataHandler";
    }

    void handle(EventPtr e) {

        counter++;
        ptime receiveTime = microsec_clock::local_time();
        string file = e->mutableMetaData().getUserInfo("file");
        string fileLoc = "/tmp/" + file;

        boost::shared_ptr<string> data = boost::static_pointer_cast<string>(
                e->getData());
        ptime sendTime = from_iso_string(e->mutableMetaData().getUserInfo(
                "startTime"));
        cout << "Received message [" << counter << "] - " << file << endl;
        cout << "... elapsed time between SEND -> RECEIVE: "
                << to_simple_string(receiveTime - sendTime) << endl;

        ofstream out(fileLoc.c_str(), ios::binary);
        for (it = data->begin(); it < data->end(); ++it) {
            cout << "write..." << endl;
            out << *it;
        }
        out.close();
    }

    long counter;
    string::iterator it;

};

int main(int argc, char** argv) {

    rsc::misc::initSignalWaiter();

    Factory& factory = getFactory();

    LoggerPtr l = Logger::getLogger("receiver");

    boost::timer t;

    Scope scope;
    if (argc > 1) {
        scope = Scope(argv[1]);
    } else {
        scope = Scope("/example/informer");
    }
    ListenerPtr s = factory.createListener(scope);
    s->addHandler(HandlerPtr(new MyDataHandler()));

    cout << "Listener setup finished. Waiting for messages on scope " << scope
            << endl;

    return rsc::misc::suggestedExitCode(rsc::misc::waitForSignal());

}
