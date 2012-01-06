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

#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Factory.h>
#include <rsb/Informer.h>
#include <rsb/MetaData.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace boost::posix_time;

int main(void) {

    Factory& factory = Factory::getInstance();
    unsigned int numMsg;
    string* binDoc = new string();
    string fileLoc;

    LoggerPtr l = Logger::getLogger("informer");
    cout << "Please enter valid filename: ";
    getline(cin, fileLoc);
    cout << "Please enter number of messages: ";
    cin >> numMsg;

    // Split into filename
    size_t found = fileLoc.find_last_of("/");
    string file = fileLoc.substr(found + 1);

    // Read file
    try {
        ifstream in(fileLoc.c_str(), ios::binary);

        char ch;
        while (in.get(ch)) {
            binDoc->push_back(ch);
        }
        in.close();
    } catch (std::exception& ex) {
        cerr << ex.what() << endl;
    }

    // Create Informer and Event
    Informer<string>::Ptr informer = factory.createInformer<string> (Scope(
            "/example/informer"));
    Informer<string>::DataPtr data(binDoc);

    cout << "sending " << data->length() << " bytes" << endl;

    EventPtr event = EventPtr(new Event(Scope("/example/informer"),
            boost::static_pointer_cast<void>(data), rsc::runtime::typeName<
                    string>()));
    event->mutableMetaData().setUserInfo("file", file);
    for (unsigned int j = 0; j < numMsg; j++) {
        // Get current time
        event->mutableMetaData().setUserInfo("startTime", to_iso_string(
                microsec_clock::local_time()));

        // Send event
        informer->publish(event);
        boost::this_thread::sleep(boost::posix_time::milliseconds(6));
    }
    cout << "READY" << endl;
    return EXIT_SUCCESS;
}
