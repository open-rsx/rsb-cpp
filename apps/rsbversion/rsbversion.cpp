/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <stdlib.h>

#include <rsb/Version.h>
#include <rsb/Factory.h>

#include <rsb/eventprocessing/EventReceivingStrategyFactory.h>
#include <rsb/eventprocessing/EventSendingStrategyFactory.h>

#include <rsb/transport/Factory.h>

#include <rsb/converter/Repository.h>

using namespace std;
using namespace rsb;

int main(int argc, char** argv) {
    bool verbose = false;
    if (argc == 2 && string(argv[1]) == "--verbose") {
        verbose = true;
    }

    cout << "Version: " << Version::string() << ", build "
         << Version::buildString() << ", abi " << Version::abi()
         << endl;

    if (verbose) {
        rsb::Factory::getInstance();

        cout << endl;

        cout << endl << "Default Configuration" << endl;

        cout << rsb::Factory::getInstance().getDefaultParticipantConfig() << endl;

        cout << endl << "Event Processing" << endl;

        cout << eventprocessing::EventReceivingStrategyFactory::getInstance() << endl;
        cout << eventprocessing::EventSendingStrategyFactory::getInstance() << endl;

        cout << endl << "Connectors" << endl;

        cout << rsb::transport::InPullFactory::getInstance() << endl;
        cout << rsb::transport::InPushFactory::getInstance() << endl;
        cout << rsb::transport::OutFactory::getInstance() << endl;

        cout << endl << "Converters" << endl;

        cout << *rsb::converter::stringConverterRepository() << endl;
    }

    return EXIT_SUCCESS;

}
