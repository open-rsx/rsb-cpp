/* ============================================================
 *
 * This file is part of the RSB project
 *
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

#include <boost/thread/thread.hpp>

#include <rsb/Event.h>
#include <rsb/Listener.h>
#include <rsb/MetaData.h>
#include <rsb/Factory.h>
#include <rsb/Handler.h>

using namespace std;

using namespace boost;

using namespace rsb;

// This function is called for each received event by the Listener
// object created below.
void printEvent(EventPtr event) {
    cout << "Received event with meta-data:" << endl;

    // Read-only access to the meta-data of the event.
    MetaData metaData = event->getMetaData();
    // Iterate over all user-supplied timestamps. There are accessor
    // methods like getCreateTime() for the system-supplied
    // timestamps.
    for (map<string, uint64_t>::const_iterator it = metaData.userTimesBegin();
         it != metaData.userTimesEnd(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    // Iterate over all meta-data items.
    for (map<string, string>::const_iterator it = metaData.userInfosBegin();
         it != metaData.userInfosEnd(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    cout << endl;
}

int main() {
    // Create a Listener object for the channel of this example and
    // connect the printEvent() function using an EventFunctionHandler
    // adapter.
    ListenerPtr listener
        = Factory::getInstance().createListener(Scope("/example/metadata"));
    listener->addHandler(HandlerPtr(new EventFunctionHandler(printEvent)));

    // Wait here, while the Listener object asynchronously receives
    // and processes events.
    while (true) {
        boost::this_thread::sleep(boost::posix_time::seconds(1000));
    };

    return EXIT_SUCCESS;
}
