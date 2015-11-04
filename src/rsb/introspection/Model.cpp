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

#include "Model.h"

#include <boost/format.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <rsc/logging/Logger.h>

namespace {

rsc::logging::LoggerPtr logger
= rsc::logging::Logger::getLogger("rsb.introspection");

}

namespace rsb {
namespace introspection {

// ParticipantInfo

ParticipantInfo::ParticipantInfo(const std::string&           kind,
                                 const rsc::misc::UUID&       id,
                                 const rsc::misc::UUID&       parentId,
                                 const Scope&                 scope,
                                 const std::string&           type,
                                 const std::set<std::string>& transportURLs)
    : kind(kind), id(id), parentId(parentId), scope(scope), type(type),
      transportURLs(transportURLs) {
}

ParticipantInfo::~ParticipantInfo() {
}

const std::string& ParticipantInfo::getKind() const {
    return this->kind;
}

const rsc::misc::UUID& ParticipantInfo::getId() const {
    return this->id;
}

const rsc::misc::UUID& ParticipantInfo::getParentId() const {
    return this->parentId;
}

const Scope& ParticipantInfo::getScope() const {
    return this->scope;
}

const std::string& ParticipantInfo::getType() const {
    return this->type;
}

const std::set<std::string>& ParticipantInfo::getTransportURLs() const {
    return this->transportURLs;
}


// ProcessInfo

std::string tryCurrentProgramName() {
    try {
        return rsc::os::currentProgramName();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentProgramName failed: %1%")
                                    % e.what()));
        return "<unknown program>";
    }
}

std::vector<std::string> tryCurrentCommandlineArguments() {
    try {
        return rsc::os::currentCommandlineArguments();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentCommandlineArguments failed: %1%")
                                    % e.what()));
        return std::vector<std::string>();
    }
}

boost::posix_time::ptime tryCurrentProcessStartTime() {
    try {
        return rsc::os::currentProcessStartTime();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentProcessStartTime failed: %1%")
                                    % e.what()));
        static boost::posix_time::ptime firstCall
            = boost::posix_time::microsec_clock::universal_time();
        return firstCall;
    }
}

std::string tryCurrentExecutingUser() {
    try {
        return rsc::os::currentExecutingUser();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentExecutingUser failed: %1%")
                                    % e.what()));
        return "";
    }
}

ProcessInfo::ProcessInfo(unsigned int                    pid,
                         const std::string&              programName,
                         const std::vector<std::string>& arguments,
                         const boost::posix_time::ptime& startTime,
                         const std::string&              rsbVersion,
                         const std::string&              executingUser)
    : pid(pid), programName(programName), arguments(arguments),
      startTime(startTime), rsbVersion(rsbVersion),
      executingUser(executingUser) {
}

ProcessInfo::~ProcessInfo() {
}

unsigned int ProcessInfo::getPid() const {
    return this->pid;
}

const std::string& ProcessInfo::getProgramName() const {
    return this->programName;
}

const std::vector<std::string>& ProcessInfo::getArguments() const {
    return this->arguments;
}

const boost::posix_time::ptime& ProcessInfo::getStartTime() const {
    return this->startTime;
}

const std::string& ProcessInfo::getRSBVersion() const {
    return this->rsbVersion;
}

const std::string& ProcessInfo::getExecutingUser() const {
    return this->executingUser;
}

// HostInfo

std::string tryCurrentHostId() {
    try {
        return rsc::os::currentHostId();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentHostId failed: %1%")
                                    % e.what()));
        return "";
    }
}

std::string currentCompatibleHostId() {
    // For compatibility with languages that have no easy way of
    // calling C API functions, use hostname instead of calling a host
    // id C function provided by respective operating system.
    std::string softwareType = tryCurrentSoftwareType();
    if (softwareType == "win32" || softwareType == "darwin") {
        return tryCurrentHostname();
    } else {
        return tryCurrentHostId();
    }
}

std::string tryCurrentHostname() {
    try {
        return rsc::os::currentHostname();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentHostname failed: %1%")
                                    % e.what()));
        return "<unknown host>";
    }
}

std::string tryCurrentMachineType() {
    try {
        return rsc::os::currentMachineType();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentMachineType failed: %1%")
                                    % e.what()));
        return "";
    }
}

std::string tryCurrentMachineVersion() {
    try {
        return rsc::os::currentMachineVersion();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentMachineVersion failed: %1%")
                                    % e.what()));
        return "";
    }
}

std::string tryCurrentSoftwareType() {
    try {
        return rsc::os::currentSoftwareType();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentSoftwareType failed: %1%")
                                    % e.what()));
        return "";
    }
}

std::string tryCurrentSoftwareVersion() {
    try {
        return rsc::os::currentSoftwareVersion();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentSoftwareVersion failed: %1%")
                                    % e.what()));
        return "";
    }
}

HostInfo::HostInfo(const std::string& id,
                   const std::string& hostname,
                   const std::string& machineType,
                   const std::string& machineVersion,
                   const std::string& softwareType,
                   const std::string& softwareVersion)
    : id(id), hostname(hostname),
      machineType(machineType), machineVersion(machineVersion),
      softwareType(softwareType), softwareVersion(softwareVersion) {
}

HostInfo::~HostInfo() {
}

const std::string& HostInfo::getId() const {
    return this->id;
}

const std::string& HostInfo::getHostname() const {
    return this->hostname;
}

const std::string& HostInfo::getMachineType() const {
    return this->machineType;
}

const std::string& HostInfo::getMachineVersion() const {
    return this->machineVersion;
}

const std::string& HostInfo::getSoftwareType() const {
    return this->softwareType;
}

const std::string& HostInfo::getSoftwareVersion() const {
    return this->softwareVersion;
}

}
}
