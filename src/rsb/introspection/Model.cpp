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

ParticipantInfo::ParticipantInfo(const std::string&     kind,
                                 const rsc::misc::UUID& id,
                                 const rsc::misc::UUID& parentId,
                                 const Scope&           scope,
                                 const std::string&     type)
    : kind(kind), id(id), parentId(parentId), scope(scope), type(type) {
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

ProcessInfo::ProcessInfo(unsigned int                    pid,
                         const std::string&              programName,
                         const std::vector<std::string>& arguments)
    : pid(pid), programName(programName), arguments(arguments) {
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

std::string tryCurrentHostname() {
    try {
        return rsc::os::currentHostname();
    } catch (const std::exception& e) {
        RSCERROR(logger, boost::str(boost::format("tryCurrentHostname failed: %1%")
                                    % e.what()));
        return "<unknown host>";
    }
}

HostInfo::HostInfo(const std::string& id,
                   const std::string& hostname)
    : id(id), hostname(hostname) {
}

HostInfo::~HostInfo() {
}

const std::string& HostInfo::getId() const {
    return this->id;
}

const std::string& HostInfo::getHostname() const {
    return this->hostname;
}

}
}
