/* ============================================================
 *
 * This file is part of the RSB project.
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

#pragma once

#include <boost/shared_ptr.hpp>

#include "../Scope.h"
#include "../Participant.h"
#include "../ParticipantConfig.h"
#include "../Listener.h"
#include "../Informer.h"

#include "rsb/rsbexports.h"

namespace rsb {
namespace patterns {

/**
 * Base class for method classes.
 *
 * Manages a listener and an informer participant and their respective
 * configurations.
 *
 * @author jmoringe
 */
class Method : public Participant {
public:
    Method(const Scope&             scope,
           const std::string&       name,
           const ParticipantConfig& listenerConfig,
           const ParticipantConfig& informerConfig);
    virtual ~Method();

    // Will override method in Participant.
    void activate();

    // Will override method in Participant.
    void deactivate();

    /**
     * Returns the name of the method.
     */
    const std::string& getName() const;
protected:
    /**
     * Returns the @ref Listener participant, creating it if necessary.
     */
    ListenerPtr getListener();

    /**
     * Returns the configuration for the @ref Listener participant.
     */
    const ParticipantConfig& getListenerConfig() const;

    /**
     * Creates and returns the @ref Listener participant.
     *
     * Derived class can override this method to adapt the created
     * instance.
     */
    virtual ListenerPtr makeListener();

    /**
     * Returns the @ref Informer participant, creating it if necessary.
     */
    InformerBasePtr getInformer();

    /**
     * Returns the configuration for the @ref Informer participant.
     */
    const ParticipantConfig& getInformerConfig() const;

    /**
     * Creates and returns the @ref Informer participant.
     *
     * Derived class can override this method to adapt the created
     * instance.
     */
    virtual InformerBasePtr makeInformer();
private:
    std::string       name;

    ParticipantConfig listenerConfig;
    ParticipantConfig informerConfig;

    ListenerPtr       listener;
    InformerBasePtr   informer;
};

typedef boost::shared_ptr<Method> MethodPtr;

}
}
