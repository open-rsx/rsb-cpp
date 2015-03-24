/* ============================================================
 *
 * This file is part of the RSB project.
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

#include "CreateFunctions.h"

using namespace std;

namespace rsb {

InformerBasePtr crateInformerBase(const Scope&             scope,
                                  const string&            dataType,
                                  const ParticipantConfig& config) {
    return getFactory().createInformerBase(scope, dataType, config);
}

ListenerPtr createListener(const Scope&             scope,
                           const ParticipantConfig& config) {
    return getFactory().createListener(scope, config);
}

ReaderPtr createReader(const Scope&             scope,
                       const ParticipantConfig& config) {
    return getFactory().createReader(scope, config);
}

patterns::LocalServerPtr createLocalServer(const Scope& scope) {
    return getFactory().createLocalServer(scope);
}

patterns::RemoteServerPtr createRemoteServer(const Scope& scope) {
    return getFactory().createRemoteServer(scope);
}

}
