/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <rsb/protocol/introspection/Hello.pb.h>
#include <rsb/protocol/introspection/Bye.pb.h>

#include "../EventId.h"
#include "../MetaData.h"
#include "../Handler.h"
#include "../Factory.h"

#include "Model.h"

namespace rsb {
namespace introspection {

// IntrospectionSender

struct QueryHandler : public Handler {
    QueryHandler(IntrospectionSender& sender)
        : sender(sender) {
    }

    void handle(EventPtr query) {
        RSCDEBUG(this->sender.logger, "Processing introspection query " << query);
        if (query->getType() == rsc::runtime::typeName<void>()) {
            if (query->getMethod() == "SURVEY") { // TODO check scope
                for (IntrospectionSender::ParticipantList::const_iterator it
                         = this->sender.participants.begin();
                     it != this->sender.participants.end(); ++it) {
                    this->sender.sendHello(*it, query);
                }
            } else if (query->getMethod() == "REQUEST") {
                std::string idString
                    = (query->getScope().getComponents()
                       [query->getScope().getComponents().size() - 1]);
                rsc::misc::UUID id(idString);
                for (IntrospectionSender::ParticipantList::const_iterator it
                         = this->sender.participants.begin();
                     it != this->sender.participants.end(); ++it) {
                    if (it->getId() == id) {
                        // TODO this->sender.sendHello(*it, query);
                        break;
                    }
                }
            } else {
                RSCWARN(this->sender.logger, "Introspection query not understood: " << query);
            }
        } else if ((query->getType() == rsc::runtime::typeName<std::string>())
                   && (*boost::static_pointer_cast<std::string>(query->getData())
                       == "ping")) {
            for (IntrospectionSender::ParticipantList::const_iterator it
                     = this->sender.participants.begin();
                 it != this->sender.participants.end();
                 ++it) {
                sendPong(*it, query);
            }
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

struct EchoCallback : public patterns::LocalServer::EventCallback {
    EventPtr call(const std::string& /*methodName*/, EventPtr request) {
        request->mutableMetaData()
            .setUserTime("request.send",
                         request->getMetaData().getSendTime());
        request->mutableMetaData()
            .setUserTime("request.receive",
                         request->getMetaData().getReceiveTime());
        return request;
    }
};

IntrospectionSender::IntrospectionSender()
    : logger(rsc::logging::Logger::getLogger("rsb.introspection.IntrospectionSender")),
      listener(getFactory().createListener("/__rsb/introspection/participants/")),
      informer(getFactory().createInformerBase("/__rsb/introspection/participants/")),
      server(getFactory().createLocalServer(boost::str(boost::format("/__rsb/introspection/hosts/%1%/%2%")
                                                       % host.getId() % process.getPid()))) {
    listener->addHandler(HandlerPtr(new QueryHandler(*this)));
    server->registerMethod("echo", patterns::LocalServer::CallbackPtr(new EchoCallback()));
}

void IntrospectionSender::addParticipant(ParticipantPtr participant) {
    RSCDEBUG(this->logger, boost::format("Adding participant %1%") % participant);

    ParticipantInfo info(participant->getKind(),
                         participant->getId(),
                         *participant->getScope(),
                         "TODO"); // TODO type
    this->participants.push_back(info);

    sendHello(info);
}

bool IntrospectionSender::removeParticipant(const Participant& participant) {
    RSCDEBUG(this->logger, boost::format("Removing participant %1%") % participant);

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

void IntrospectionSender::sendHello(const ParticipantInfo& participant,
                                    EventPtr               query) {
    boost::shared_ptr<rsb::protocol::introspection::Hello> hello(
        new rsb::protocol::introspection::Hello());

    // Add participant information.
    hello->set_id(participant.getId().getId().data,
                  participant.getId().getId().size());
    hello->set_kind(participant.getKind());
    hello->set_scope(participant.getScope().toString());

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

    // Add host information.
    rsb::protocol::operatingsystem::Host* host = hello->mutable_host();
    if (!this->host.getId().empty()) {
        host->set_id(this->host.getId());
    }
    host->set_hostname(this->host.getHostname());

    // Construct event.
    EventPtr helloEvent(new Event());
    helloEvent->setScope(this->informer->getScope()->concat(boost::str(boost::format("/%1%")
                                                                       % participant.getId().getIdAsString())));
    helloEvent->setData(hello);
    helloEvent->setType(rsc::runtime::typeName(*hello.get()));
    if (query) {
        helloEvent->addCause(query->getEventId());
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
