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

#include "SpreadConnection.h"

#include <iostream>
#include <string.h>

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <rsc/misc/IllegalStateException.h>

#include <sp.h>

#include "../../CommException.h"

using namespace std;

using namespace boost;

using namespace rsc::logging;

namespace rsb {
namespace spread {

#define SPREAD_MAX_GROUPS   100
#define SPREAD_MAX_MESSLEN  180000

SpreadConnection::SpreadConnection(const string& id, const string& host,
        unsigned int port) :
    logger(Logger::getLogger("rsb.transport.spread.SpreadConnection")), connected(false),
    host(host), port(port),
#ifdef WIN32
    spreadname(str(format("%1%@%2%") % port % host)),
#else
    spreadname((host == defaultHost())
               ? lexical_cast<string>(port)
               : str(format("%1%@%2%") % port % host)),
#endif
    conId(id), msgCount(0) {
    RSCDEBUG(logger, "instantiated spread connection with id " << conId
             << " to spread daemon at " << spreadname);
}

SpreadConnection::~SpreadConnection() {
    RSCDEBUG(logger, "destroying SpreadConnection object");
    if (connected) {
        deactivate();
    }
}

void SpreadConnection::activate() {
    // XXX spread init and group join - not threadsafe, what to do about this?
    if (connected) {
        throw rsc::misc::IllegalStateException("Connection with id " + conId
                + " is already active.");
    }

    RSCDEBUG(logger, "connecting to spread daemon at " << spreadname);
    char spreadPrivateGroup[MAX_GROUP_NAME];
    int ret = SP_connect(spreadname.c_str(), 0, 0, 0, &con, spreadPrivateGroup);
    spreadpg = string(spreadPrivateGroup);
    if (ret != ACCEPT_SESSION) {
        switch (ret) {
        case ILLEGAL_SPREAD:
            RSCFATAL(
                    logger,
                    "spread connect error: connection to spread daemon at " << spreadname << " failed, check port and hostname");
            break;
        case COULD_NOT_CONNECT:
            RSCFATAL(
                    logger,
                    "spread connect error: connection to spread daemon failed due to socket errors");
            break;
        case CONNECTION_CLOSED:
            RSCFATAL(
                    logger,
                    "spread connect error: communication errors occurred during setup of connection");
            break;
        case REJECT_VERSION:
            RSCFATAL(logger,
                    "spread connect error: daemon or library version mismatch");
            break;
        case REJECT_NO_NAME:
            RSCFATAL(logger,
                    "spread connect error: protocol error during setup");
            break;
        case REJECT_ILLEGAL_NAME:
            RSCFATAL(
                    logger,
                    "spread connect error: name provided violated requirement, length or illegal character");
            break;
        case REJECT_NOT_UNIQUE:
            RSCFATAL(
                    logger,
                    "spread connect error: name provided is not unique on this daemon");
            break;
        default:
            RSCFATAL(logger, "unknown spread connect error, value: " << ret);
        }
        SP_error(ret);
        throw CommException("Error during connection to spread daemon");
    } else {
        RSCDEBUG(logger, "success, private group id is " << spreadpg);
    }
    RSCINFO(logger, "connected to spread daemon");

    connected = true;

}

void SpreadConnection::deactivate() {

    if (!connected) {
        throw rsc::misc::IllegalStateException("Connection with id " + conId
                + " is not active.");
    }

    connected = false;

}

bool SpreadConnection::isActive() {
    return connected;
}

void SpreadConnection::receive(SpreadMessagePtr sm) {

    if (!connected) {
        throw rsc::misc::IllegalStateException("Connection with id " + conId
                + " is not active.");
    }

    sm->reset();

    // read from Spread multicast group
    int serviceType;
    int numGroups;
    char sender[MAX_GROUP_NAME];
    char retGroups[SPREAD_MAX_GROUPS][MAX_GROUP_NAME];
    int16 messType;
    int dummyEndianMismatch;
    char buf[SPREAD_MAX_MESSLEN];
    int ret = SP_receive(con, &serviceType, sender, SPREAD_MAX_GROUPS,
            &numGroups, retGroups, &messType, &dummyEndianMismatch,
            SPREAD_MAX_MESSLEN, buf);

    // check for errors
    if (ret < 0) {

        string err;
        switch (ret) {
        case ILLEGAL_SESSION:
            err = "spread receive error: mbox given to receive on was illegal";
            break;
        case ILLEGAL_MESSAGE:
            err = "spread receive error: message had an illegal structure";
            break;
        case CONNECTION_CLOSED:
            err = "spread receive error: message communication errors occurred";
            break;
        case GROUPS_TOO_SHORT:
            err
                    = "spread receive error: groups array too short to hold list of groups";
            break;
        case BUFFER_TOO_SHORT:
            err
                    = "spread receive error: message body buffer too short to hold the message received";
            break;
        default:
            err = "unknown spread receive error";
        }
        throw CommException("Spread communication error. Reason: " + err);

    }

    // handle normal messages
    if (Is_regular_mess(serviceType)) {

        RSCDEBUG(logger, "regular spread message received");

        // cancel if requested
        if (numGroups == 1 && string(retGroups[0]) == string(spreadpg)) {
            throw boost::thread_interrupted();
        }

        sm->setType(SpreadMessage::REGULAR);
        sm->setData(string(buf, ret));
        if (numGroups < 0) {
            // TODO check whether we shall implement a best effort strategy here
            RSCWARN(logger,
                    "error during message reception, group array too large, requested size "
                    << " configured size " << SPREAD_MAX_GROUPS);
        }
        for (int i = 0; i < numGroups; i++) {
            if (retGroups[i] != NULL) {
                string group = string(retGroups[i]);
                RSCDEBUG(logger,
                        "received message, addressed at group with name "
                        << group);
                sm->addGroup(group);
            }
        }

    } else if (Is_membership_mess(serviceType)) {
        // this will currently never happen as we do not want to have membership messages
        // and this message does not contain any contents

        RSCINFO(logger, "received spread membership message type");
        sm.reset(new SpreadMessage(SpreadMessage::MEMBERSHIP));

    } else {

        RSCFATAL(logger, "received unknown spread message type with code " << serviceType);
        assert(false);
        throw CommException(
                "Received a message that is neither membership nor data message. "
                    "This should never happen according to the spread documentation.");

    }

}

void SpreadConnection::send(const SpreadMessage& msg) {

    // TODO check message size, if larger than ~100KB throw exception
    if (!connected) {
        throw rsc::misc::IllegalStateException("Connection with id " + conId
                + " is not active.");
    }

    const unsigned int groupCount = msg.getGroupCount();
    if (groupCount == 0) {
        throw CommException("Group information missing in message");
    }

    int ret;
    if (groupCount == 1) {
        // use SP_multicast
        string group = *msg.getGroupsBegin();
        RSCDEBUG(logger, "sending message to group with name " << group);
        ret = SP_multicast(con, msg.getQOS(), group.c_str(), 0, msg.getSize(),
                msg.getData());
    } else {
        // use SP_multigroup_multicast
        char* groups = new char[groupCount * MAX_GROUP_NAME];
        memset(groups, 0, groupCount * MAX_GROUP_NAME);
        int j = 0;
        for (list<string>::const_iterator it = msg.getGroupsBegin(); it
                != msg.getGroupsEnd(); ++it) {
            string group = *it;
            strcpy(groups + j * MAX_GROUP_NAME, group.c_str());
            j++;
        }
        ret = SP_multigroup_multicast(con, msg.getQOS(), groupCount,
                (const char(*)[MAX_GROUP_NAME]) groups, 0, msg.getSize(),
                msg.getData());
        delete[] groups;
    }

    // TODO shouldn't msgCount be incremented only in case of success?
    ++msgCount;

    if (ret < 0) {

        stringstream err;
        switch (ret) {
        case ILLEGAL_SESSION:
            err << "Illegal Session";
            break;
        case ILLEGAL_MESSAGE:
            err << "Illegal Message";
            break;
        case CONNECTION_CLOSED:
            err << "Connection Closed";
            break;
        default:
            err << "Unknown spread error with code " << ret;
            break;
        }

        throw CommException(err.str());

    }

}

void SpreadConnection::interruptReceive() {

    if (!connected) {
        throw rsc::misc::IllegalStateException("Connection with id " + conId
                + " is not active.");
    }

    SP_multicast(con, RELIABLE_MESS, spreadpg.c_str(), 0, 0, 0);

}

string SpreadConnection::generateId(const string& prefix) {
    // TODO generate meaningful and unique Id according to MAX_PRIVATE_NAME
    //            uuid_t id;
    //            uuid_generate(id);
    //            char buf[37];
    //            uuid_unparse(id,buf);
    return prefix;
}

unsigned long SpreadConnection::getMsgCount() {
    return msgCount;
}

mailbox* SpreadConnection::getMailbox() {
    if (!connected) {
        throw rsc::misc::IllegalStateException("Connection with id " + conId
                + " is not active.");
    }
    return& con;
}

string defaultHost() {
    return "localhost";
}

unsigned int defaultPort() {
    return DEFAULT_SPREAD_PORT;
}

}
}
