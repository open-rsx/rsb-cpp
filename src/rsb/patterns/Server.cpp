/* ============================================================
 *
 * This file is part of the RSB project.
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

#include "Server.h"

#include "../Factory.h"

namespace rsb {
namespace patterns {

Method::Method(const Scope&             scope,
               const std::string&       name,
               const ParticipantConfig& listenerConfig,
               const ParticipantConfig& informerConfig)
    : Participant(scope, listenerConfig), name(name), // TODO do config properly
      listenerConfig(listenerConfig), informerConfig(informerConfig) {
}

Method::~Method() {
    deactivate();
}

void Method::activate() {
    getListener(); // Force listener creation
}

void Method::deactivate() {
    if (this->listener) {
        this->listener.reset();
    }
    if (this->informer) {
        this->informer.reset();
    }
}

const std::string& Method::getName() const {
    return this->name;
}

const std::set<std::string> Method::getTransportURLs() const {
    return std::set<std::string>();
}


ListenerPtr Method::getListener() {
    if (!this->listener) {
        this->listener = makeListener();
    }

    return this->listener;
}

const ParticipantConfig& Method::getListenerConfig() const {
    return this->listenerConfig;
}

ListenerPtr Method::makeListener() {
    return getFactory().createListener(*getScope(), getListenerConfig(), this);
}

InformerBasePtr Method::getInformer() {
    if (!this->informer) {
        this->informer = makeInformer();
    }

    return this->informer;
}

const ParticipantConfig& Method::getInformerConfig() const {
    return this->informerConfig;
}

InformerBasePtr Method::makeInformer() {
    return getFactory().createInformerBase(*getScope(),
                                           "", // TODO
                                           getInformerConfig(),
                                           this);
}

}
}
