/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2016 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <rsb/Informer.h>
#include <rsb/Factory.h>
#include <rsb/ParticipantConfig.h>

int main(void) {

    // Get the factory instance.
    rsb::Factory& factory = rsb::getFactory();

    // Retrieve a copy of the default ParticipantConfig.
    rsb::ParticipantConfig config = factory.getDefaultParticipantConfig();

    // Iterate over all available transports, finding and enabling the
    // Spread transport.
    std::set<rsb::ParticipantConfig::Transport> transports
        = config.getTransports(true);
    for (std::set<rsb::ParticipantConfig::Transport>::const_iterator it
             = transports.begin(); it != transports.end(); ++it) {
        if (it->getName() == "spread") {
            config.mutableTransport(it->getName()).setEnabled(true);
        }
    }

    // Create an informer using the non-default (transport)
    // configuration.
    rsb::Informer<std::string>::Ptr informer
        = factory.createInformer<std::string> ("/example/informer", config);

    informer->publish(boost::shared_ptr<std::string>(new std::string("foo")));

    return EXIT_SUCCESS;

}
