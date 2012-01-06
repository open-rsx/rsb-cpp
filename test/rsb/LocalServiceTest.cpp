/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <stdexcept>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/LocalService.h"
#include "rsb/ParticipantConfig.h"

#include "mocks.h"

using namespace std;
using namespace testing;
using namespace rsb;

/**
 * A dummy for testing with the Participant class.
 *
 * @author jwienke
 */
class DummyParticipant: public Participant {
public:
    DummyParticipant(const Scope& scope) :
            Participant(scope, ParticipantConfig()) {
    }
    virtual ~DummyParticipant() {
    }
    string getClassName() const {
        return "DummyParticipant";
    }
};

TEST(LocalServiceTest, testConstruction)
{

    Scope scope("/this/is/a/test");
    LocalService service(scope);
    EXPECT_EQ(scope, *service.getScope());
    EXPECT_TRUE(service.getParticipants().empty());

}

TEST(LocalServiceTest, testParticipants)
{

    Scope scope("/this/is/a/test");
    LocalService service(scope);

    EXPECT_THROW(service.addParticipant(ParticipantPtr(new DummyParticipant(Scope("/this/is/")))), invalid_argument);
    EXPECT_THROW(service.addParticipant(ParticipantPtr(new DummyParticipant(Scope("/not/a/prefix")))), invalid_argument);
    EXPECT_THROW(service.addParticipant(ParticipantPtr(new DummyParticipant(scope))), invalid_argument);

    ParticipantPtr directChild(new DummyParticipant(scope.concat(Scope("/child"))));
    ParticipantPtr indirectChild(new DummyParticipant(scope.concat(Scope("/child/further/"))));

    service.addParticipant(directChild);
    service.addParticipant(indirectChild);

    set<ParticipantPtr> participants = service.getParticipants();
    EXPECT_EQ(size_t(2), participants.size());
    EXPECT_EQ(size_t(1), participants.count(directChild));
    EXPECT_EQ(size_t(1), participants.count(indirectChild));

    service.removeParticipant(indirectChild);
    participants = service.getParticipants();
    EXPECT_EQ(size_t(1), participants.size());
    EXPECT_EQ(size_t(1), participants.count(directChild));

}
