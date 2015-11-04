/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2014, 2015 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <vector>
#include <string>
#include <set>

#include <boost/date_time/posix_time/ptime.hpp>

#include <rsc/misc/UUID.h>
#include <rsc/os/ProcessInfo.h>
#include <rsc/os/HostInfo.h>

#include <rsb/Version.h>
#include <rsb/Scope.h>

namespace rsb {
namespace introspection {

// ParticipantInfo

/**
 * Instances of this class store information about participants in the
 * current process.
 *
 * @author jmoringe
 */
class ParticipantInfo {
public:
    ParticipantInfo(const std::string&           kind,
                    const rsc::misc::UUID&       id,
                    const rsc::misc::UUID&       parentId,
                    const Scope&                 scope,
                    const std::string&           type,
                    const std::set<std::string>& transportURLs);
    virtual ~ParticipantInfo();

    const std::string& getKind() const;

    const rsc::misc::UUID& getId() const;

    const rsc::misc::UUID& getParentId() const;

    const Scope& getScope() const;

    const std::string& getType() const;

    const std::set<std::string>& getTransportURLs() const;
private:
    std::string           kind;
    rsc::misc::UUID       id;
    rsc::misc::UUID       parentId;
    Scope                 scope;
    std::string           type;
    std::set<std::string> transportURLs;
};

// ProcessInfo

// Wrap non-essential RSC function which can throw exceptions.
std::string tryCurrentProgramName();

std::vector<std::string> tryCurrentCommandlineArguments();

boost::posix_time::ptime tryCurrentProcessStartTime();

std::string tryCurrentExecutingUser();

/**
 * Instances of this class store information about the current
 * process.
 *
 * @author jmoringe
 */
class ProcessInfo {
public:
    ProcessInfo(unsigned int                    pid
                = rsc::os::currentProcessId(),
                const std::string&              programName
                = tryCurrentProgramName(),
                const std::vector<std::string>& arguments
                = tryCurrentCommandlineArguments(),
                const boost::posix_time::ptime& startTime
                = tryCurrentProcessStartTime(),
                const std::string&              rsbVersion
                = rsb::Version::string() + "-" + rsb::Version::buildId(),
                const std::string&              executingUser
                = tryCurrentExecutingUser());
    virtual ~ProcessInfo();

    unsigned int getPid() const;

    const std::string& getProgramName() const;

    const std::vector<std::string>& getArguments() const;

    const boost::posix_time::ptime& getStartTime() const;

    const std::string& getRSBVersion() const;

    const std::string& getExecutingUser() const;
private:
    unsigned int             pid;
    std::string              programName;
    std::vector<std::string> arguments;
    boost::posix_time::ptime startTime;
    std::string              rsbVersion;
    std::string              executingUser;
};

// HostInfo

std::string tryCurrentHostId();

std::string currentCompatibleHostId();

std::string tryCurrentHostname();

std::string tryCurrentMachineType();

std::string tryCurrentMachineVersion();

std::string tryCurrentSoftwareType();

std::string tryCurrentSoftwareVersion();

/**
 * Instances of this class store information about the local host.
 *
 * @author jmoringe
 */
class HostInfo {
public:
    HostInfo(const std::string& id              = currentCompatibleHostId(),
             const std::string& hostname        = tryCurrentHostname(),
             const std::string& machineType     = tryCurrentMachineType(),
             const std::string& machineVersion  = tryCurrentMachineVersion(),
             const std::string& softwareType    = tryCurrentSoftwareType(),
             const std::string& softwareVersion = tryCurrentSoftwareVersion());
    virtual ~HostInfo();

    const std::string& getId() const;

    const std::string& getHostname() const;

    const std::string& getMachineType() const;

    const std::string& getMachineVersion() const;

    const std::string& getSoftwareType() const;

    const std::string& getSoftwareVersion() const;
private:
    std::string id;
    std::string hostname;
    std::string machineType;
    std::string machineVersion;
    std::string softwareType;
    std::string softwareVersion;
};

}
}
