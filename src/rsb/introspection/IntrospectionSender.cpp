/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2014, 2015, 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "IntrospectionSender.h"

#include <boost/format.hpp>

#include <rsc/misc/UUID.h>

#include <rsb/protocol/introspection/Hello.pb.h>
#include <rsb/protocol/introspection/Bye.pb.h>

#include "../EventId.h"
#include "../MetaData.h"
#include "../Handler.h"
#include "../Factory.h"

#include "Types.h"
#include "Model.h"

namespace rsb {
namespace introspection {

// Internal
// @author jmoringe
struct QueryHandler : public Handler {
    QueryHandler(IntrospectionSender& sender)
        : sender(sender) {
    }

    void handle(EventPtr query) {
        RSCDEBUG(this->sender.logger, "Processing introspection query " << query);

        boost::mutex::scoped_lock lock(sender.mutex);

        if (query->getType() == rsc::runtime::typeName<void>()) {
            if (query->getMethod() == "SURVEY") { // TODO check scope
                handleSurvey(query);
            } else if (query->getMethod() == "REQUEST") {
                handleRequest(query);
            } else {
                RSCWARN(this->sender.logger, "Introspection query not understood: " << query);
            }
        } else if ((query->getType() == rsc::runtime::typeName<std::string>())
                   && (*boost::static_pointer_cast<std::string>(query->getData())
                       == "ping")) {
            handlePing(query);
        }
    }

    void handleSurvey(EventPtr query) {
        for (IntrospectionSender::ParticipantList::const_iterator it
                 = this->sender.participants.begin();
             it != this->sender.participants.end();
             ++it) {
            this->sender.sendHello(*it, query);
        }
    }

    void handleRequest(EventPtr query) {
        std::string idString
            = (query->getScope().getComponents()
               [query->getScope().getComponents().size() - 1]);
        rsc::misc::UUID id(idString);
        for (IntrospectionSender::ParticipantList::const_iterator it
                 = this->sender.participants.begin();
             it != this->sender.participants.end(); ++it) {
            if (it->getId() == id) {
                this->sender.sendHello(*it, query);
                break;
            }
        }
    }

    void handlePing(EventPtr query) {
        for (IntrospectionSender::ParticipantList::const_iterator it
                 = this->sender.participants.begin();
             it != this->sender.participants.end();
             ++it) {
            sendPong(*it, query);
        }
    }

    void sendPong(const ParticipantInfo& participant, EventPtr /*query*/) {
        EventPtr pongEvent(new Event());
        pongEvent->setScope(this->sender.informer->getScope()->concat(participant.getId().getIdAsString()));
        pongEvent->setType(rsc::runtime::typeName<std::string>());
        pongEvent->setData(boost::shared_ptr<std::string>(new std::string("pong")));
        this->sender.informer->publish(pongEvent);
    }

    IntrospectionSender& sender;
};

// Internal
// @author jmoringe
struct EchoCallback : public patterns::LocalServer::EventCallback {
    EventPtr call(const std::string& /*methodName*/, EventPtr request) {
        EventPtr reply(new Event(request->getScope(),
                                 request->getData(),
                                 request->getType()));
        reply->mutableMetaData().setUserTime("request.send",
                                             request->getMetaData().getSendTime());
        reply->mutableMetaData().setUserTime("request.receive",
                                             request->getMetaData().getReceiveTime());
        return reply;
    }
};

IntrospectionSender::IntrospectionSender(boost::shared_ptr<std::string> processDisplayName,
                                         const ParticipantConfig& listenerConfig,
                                         const ParticipantConfig& informerConfig)
    : logger(rsc::logging::Logger::getLogger("rsb.introspection.IntrospectionSender")),
      processDisplayName(processDisplayName),
      listener(getFactory().createListener(INTROSPECTION_PARTICIPANTS_SCOPE,
                                           listenerConfig)),
      informer(getFactory().createInformerBase(INTROSPECTION_PARTICIPANTS_SCOPE,
                                               "", informerConfig)),
      server(getFactory().createLocalServer(INTROSPECTION_HOSTS_SCOPE
                                            .concat(boost::str(boost::format("/%1%/%2%")
                                                               % host.getId() % process.getPid())),
                                            listenerConfig, informerConfig)) {
    listener->addHandler(HandlerPtr(new QueryHandler(*this)));
    server->registerMethod("echo", patterns::LocalServer::CallbackPtr(new EchoCallback()));
}

void IntrospectionSender::addParticipant(ParticipantPtr participant,
                                         Participant*   parent) {
    RSCDEBUG(this->logger, boost::format("Adding participant %1%") % participant);

    boost::mutex::scoped_lock lock(this->mutex);

    ParticipantInfo info(participant->getKind(),
                         participant->getId(),
                         (parent ? parent->getId() : rsc::misc::UUID(false)),
                         *participant->getScope(),
                         "TODO", // TODO type
                         participant->getTransportURLs());
    this->participants.push_back(info);

    sendHello(info);
}

bool IntrospectionSender::removeParticipant(const Participant& participant) {
    RSCDEBUG(this->logger, boost::format("Removing participant %1%") % participant);

    boost::mutex::scoped_lock lock(this->mutex);

    ParticipantList::iterator it;
    for (it = this->participants.begin(); it != this->participants.end(); ++it) {
        if (it->getId() == participant.getId()) {
            break;
        }
    }
    if (it == this->participants.end()) {
        RSCWARN(this->logger, boost::format("Trying to remove unknown participant %1%")
                % participant);
    } else {
        sendBye(*it);

        this->participants.erase(it);
    }

    RSCDEBUG(this->logger, boost::format("%1% participant(s) remain(s)")
             % this->participants.size());

    return !this->participants.empty();
}

const boost::posix_time::ptime UNIX_EPOCH
  = boost::posix_time::ptime(boost::gregorian::date(1970, boost::date_time::Jan, 1));

void IntrospectionSender::sendHello(const ParticipantInfo& participant,
                                    EventPtr               query) {
    boost::shared_ptr<rsb::protocol::introspection::Hello> hello(
        new rsb::protocol::introspection::Hello());

    // Add participant information.
    hello->set_id(participant.getId().getId().data,
                  participant.getId().getId().size());
    if (participant.getParentId() != rsc::misc::UUID(false)) {
        hello->set_parent(participant.getParentId().getId().data,
                          participant.getParentId().getId().size());
    }
    hello->set_kind(participant.getKind());
    hello->set_scope(participant.getScope().toString());
    for (std::set<std::string>::const_iterator it
             = participant.getTransportURLs().begin();
         it != participant.getTransportURLs().end(); ++it) {
        hello->add_transport(*it);
    }

    // Add process information.
    rsb::protocol::operatingsystem::Process* process
        = hello->mutable_process();
    process->set_id(boost::lexical_cast<std::string>(this->process.getPid()));
    process->set_program_name(this->process.getProgramName());
    std::vector<std::string> arguments = this->process.getArguments();
    for (std::vector<std::string>::const_iterator it = arguments.begin();
         it != arguments.end(); ++it) {
        process->add_commandline_arguments(*it);
    }
    process->set_start_time((this->process.getStartTime() - UNIX_EPOCH)
                            .total_microseconds());
    process->set_rsb_version(this->process.getRSBVersion());
    if (!this->process.getExecutingUser().empty()) {
        process->set_executing_user(this->process.getExecutingUser());
    }
    if (this->processDisplayName) {
        process->set_display_name(*this->processDisplayName);
    }

    // Add host information.
    rsb::protocol::operatingsystem::Host* host = hello->mutable_host();
    if (!this->host.getId().empty()) {
        host->set_id(this->host.getId());
    }
    host->set_hostname(this->host.getHostname());
    if (!this->host.getMachineType().empty()) {
        host->set_machine_type(this->host.getMachineType());
    }
    if (!this->host.getMachineVersion().empty()) {
        host->set_machine_version(this->host.getMachineVersion());
    }
    if (!this->host.getSoftwareType().empty()) {
        host->set_software_type(this->host.getSoftwareType());
    }
    if (!this->host.getSoftwareVersion().empty()) {
        host->set_software_version(this->host.getSoftwareVersion());
    }

    // Construct event.
    EventPtr helloEvent(new Event());
    helloEvent->setScope(this->informer->getScope()->concat(boost::str(boost::format("/%1%")
                                                                       % participant.getId().getIdAsString())));
    helloEvent->setData(hello);
    helloEvent->setType(rsc::runtime::typeName(*hello.get()));
    if (query) {
        helloEvent->addCause(query->getId());
    }

    this->informer->publish(helloEvent);
}

void IntrospectionSender::sendBye(const ParticipantInfo& participant) {
    boost::shared_ptr<rsb::protocol::introspection::Bye> bye(
        new rsb::protocol::introspection::Bye());
    bye->set_id(participant.getId().getId().data,
                participant.getId().getId().size());
    EventPtr byeEvent(new Event());
    byeEvent->setScope(this->informer->getScope()
                       ->concat(boost::str(boost::format("/%1%")
                                           % participant.getId().getIdAsString())));
    byeEvent->setData(bye);
    byeEvent->setType(rsc::runtime::typeName(*bye.get()));
    this->informer->publish(byeEvent);
}

}
}
