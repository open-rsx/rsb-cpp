/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012, 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/program_options.hpp>

#include <rsc/Version.h>
#include <rsc/plugins/Manager.h>

#include <rsb/Version.h>
#include <rsb/Factory.h>

#include <rsb/eventprocessing/EventReceivingStrategyFactory.h>
#include <rsb/eventprocessing/EventSendingStrategyFactory.h>

#include <rsb/transport/Factory.h>

#include <rsb/converter/Repository.h>

using namespace std;

using namespace boost::program_options;

using namespace rsb;

bool version         = true;
bool verbose         = false;
bool configuration   = false;
bool eventProcessing = false;
bool connectors      = false;
bool converters      = false;
bool plugins         = false;

options_description options("Allowed options");

bool handleCommandline(int argc, char *argv[]) {
    options.add_options()
        ("help",
         "Display a help message.")
        ("version",
         bool_switch(&version)->default_value(true),
         "Display version information?")
        ("verbose",
         bool_switch(&verbose),
         "Display all available information? This is equivalent to specifying "
"all other flags simultaneously.")
        ("configuration",
         bool_switch(&configuration),
         "Display configuration information?")
        ("event-processing",
         bool_switch(&eventProcessing),
         "Display available event processing strategies?")
        ("connectors",
         bool_switch(&connectors),
         "Display available connectors?")
        ("converters",
         bool_switch(&converters),
         "Display available converters?")
        ("plugins",
         bool_switch(&plugins),
         "Display available plugins?")
        ;

    variables_map map;
    store(command_line_parser(argc, argv)
          .options(options)
          .run(), map);
    notify(map);
    if (map.count("help"))
        return true;

    return false;
}

void usage() {
    cout << "usage: info [OPTIONS]" << endl;
    cout << options << endl;
}

int main(int argc, char** argv) {

    // Handle commandline arguments.
    try {
        if (handleCommandline(argc, argv)) {
            usage(); // --help
            return EXIT_SUCCESS;
        }
    } catch (const std::exception& e) {
        cerr << "Error parsing command line: " << e.what() << endl;
        usage();
        return EXIT_FAILURE;
    }

    // Initialize everything.
    rsb::getFactory();

    if (version || verbose) {
        cout << "Version: " << Version::string() << ", build "
             << Version::buildString() << ", abi " << Version::abi()
             << endl
             << endl
             << "RSC Version: " << rsc::Version::string() << ", build "
             << rsc::Version::buildString() << ", abi " << rsc::Version::abi()
             << endl;
    }

    if (configuration || verbose) {
        cout << endl << "Default Configuration" << endl;

        cout << rsb::getFactory().getDefaultParticipantConfig() << endl;
    }

    if (eventProcessing || verbose) {
        cout << endl << "Event Processing" << endl;

        cout << eventprocessing::getEventReceivingStrategyFactory() << endl;
        cout << eventprocessing::getEventSendingStrategyFactory() << endl;
    }

    if (connectors || verbose) {
        cout << endl << "Connectors" << endl;

        cout << rsb::transport::getInPullFactory() << endl;
        cout << rsb::transport::getInPushFactory() << endl;
        cout << rsb::transport::getOutFactory() << endl;
    }

    if (converters || verbose)  {
        cout << endl << "Converters" << endl;

        cout << *rsb::converter::converterRepository<string>() << endl;
    }

    if (plugins || verbose) {
        cout << endl << "Plugins" << endl;

        rsc::plugins::ManagerPtr manager
            = getFactory().getPluginManager();
        cout << "  Search path: " << manager->getPath() << endl;
        cout << "  Available plugins: " << endl;
        std::set<rsc::plugins::PluginPtr> plugins = manager->getPlugins();
        
        for (std::set<rsc::plugins::PluginPtr>::const_iterator it
                 = plugins.begin(); it != plugins.end(); ++it) {
            cout << "  * " << (*it)->getName() << endl;
        }
    }
    
    return EXIT_SUCCESS;

}
