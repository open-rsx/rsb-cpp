/* ============================================================
 *
 * This file is part of the RSB project
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

#pragma once

#include <ostream>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <boost/signal.hpp>

#include <rsc/misc/UUID.h>
#include <rsc/runtime/Printable.h>

#include "rsb/rsbexports.h"

namespace rsb {

class Participant;

typedef boost::signal1<void, Participant*> SignalParticipantDestroyed;
typedef boost::shared_ptr<SignalParticipantDestroyed> SignalParticipantDestroyedPtr;

class ParticipantConfig;
class Scope;
typedef boost::shared_ptr<Scope> ScopePtr;

/**
 * Objects of this class participate in the exchange of notifications
 * on one channel of the bus. Since each channel is identified by
 * scope, each @ref rsb::Participant has an associated scope. On the
 * other hand, scopes do not uniquely identify @ref rsb::Participant s
 * since zero, one or more of these can participate in the exchange of
 * any given scope. For this reason, each @ref rsb::Participant has a
 * unique id.
 *
 *  @author jmoringe
 */
class RSB_EXPORT Participant: public virtual rsc::runtime::Printable,
        public boost::noncopyable {
    friend class Factory;
public:

    virtual ~Participant();

    void printContents(std::ostream& stream) const;

    /**
     * Return the kind of the participant.
     *
     * Examples include @c "informer", @c "listener" and @c "reader".
     *
     * @return a string describing the kind of the participant.
     */
    virtual std::string getKind() const = 0;

    /**
     * Returns the unique id of the participant.
     *
     * @return unique id
     */
    rsc::misc::UUID getId() const;

    /**
     * Returns the scope of this participant.
     *
     * @return scope
     */
    ScopePtr getScope() const;

    /**
     * Returns a copy of the participant's configuration.
     *
     * @return copy of this participant's config
     */
    ParticipantConfig getConfig() const;

protected:

    Participant(const Scope& scope, const ParticipantConfig& config);

private:

    class Impl;
    boost::scoped_ptr<Impl> d;

    void setSignalParticipantDestroyed(SignalParticipantDestroyedPtr signal);

};

typedef boost::shared_ptr<Participant> ParticipantPtr;

}
