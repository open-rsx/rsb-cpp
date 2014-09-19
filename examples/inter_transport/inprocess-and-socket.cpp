/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2013, 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/thread.hpp>

#include <rsc/misc/SignalWaiter.h>

#include <rsb/Factory.h>
#include <rsb/Handler.h>

using namespace std;

using namespace boost;

using namespace rsb;

void printEvent(boost::shared_ptr<string> data) {
    cout << "received " << *data << endl;
}

int main() {

    rsc::misc::initSignalWaiter();

    // Adjust default configuration such that the inprocess and socket
    // transports are enabled.
    ParticipantConfig config =
            getFactory().getDefaultParticipantConfig();
    ParticipantConfig::Transport inprocess = config.getTransport("inprocess");
    inprocess.setEnabled(true);
    config.addTransport(inprocess);
    getFactory().setDefaultParticipantConfig(config);

    // Setup a listener. This listener uses the modified default
    // configuration above which amounts to one connector for the
    // inprocess transport and one connector for the socket
    // transport. These connectors cause the listener to receive all
    // events which are sent by the informer below twice.
    //
    // The expected output is therefore (modulo order of events):
    // "received foo     [via socket]
    //  received foo"    [via inprocess]
    ListenerPtr listener
        = getFactory().createListener(Scope("/tutorial/transports"));
    listener->addHandler(
            HandlerPtr(new DataFunctionHandler<string> (&printEvent)));

    // Setup an informer. This informer uses the same modified default
    // configuration as the listener above. It therefore broadcasts
    // events via one connector for the inprocess transport and one
    // connector for the socket transport. The socket connector
    // enables participants in other processes to receive the events.
    Informer<string>::Ptr informer
        = getFactory().createInformer<string>(Scope("/tutorial/transports"));

    boost::shared_ptr<string> data(new string("foo"));
    informer->publish(data);

    return rsc::misc::suggestedExitCode(rsc::misc::waitForSignal());

}
