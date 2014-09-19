/* ============================================================
 *
 * This file is part of the RSB project.
 *
 * Copyright (C) 2011, 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <string>

#include <boost/thread/thread.hpp>

#include <rsc/misc/langutils.h>

#include <rsb/Event.h>
#include <rsb/Factory.h>
#include <rsb/MetaData.h>

using namespace std;

using namespace rsb;

int main() {
    // Create an Informer object for string data for the channel of
    // this example.
    Informer<string>::Ptr informer
        = getFactory().createInformer<string>(Scope("/example/metadata"));

    // Create and send data in loop.
    while (true) {
        // We simulate some expensive data-creation process here, so
        // we get distinct timestamps at the start and end of the
        // processing.
        boost::uint64_t processingStart = rsc::misc::currentTimeMicros();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
        string data = "result of processing";
        boost::uint64_t processingDone = rsc::misc::currentTimeMicros();

        // We manually create a new Event object and add the desired
        // meta-data:
        // + processing-start timestamp
        // + processing-done timestamp
        // + creator meta-info item
        EventPtr event(new Event(Scope("/example/metadata"),
                                 Informer<string>::DataPtr(new string(data)),
                                 "std::string"));

        MetaData& metaData = event->mutableMetaData();
        metaData.setUserTime("processing-start", processingStart);
        metaData.setUserTime("processing-done",  processingDone);

        metaData.setUserInfo("creator", "example-sender-program");

        // When the data members of the Event object are populated, we
        // publish it using the Informer object.
        informer->publish(event);
    }

    return EXIT_SUCCESS;
}
