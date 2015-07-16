/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
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

#include "rsb/Handler.h"

#include <iostream>

#include <boost/bind.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/misc/UUID.h>

#include "rsb/MetaData.h"
#include "rsb/EventId.h"
#include "rsb/Scope.h"

using namespace std;
using namespace rsb;
using namespace testing;

void printEvent(EventPtr e) {
    cout << "Event received: " << e << endl;
}

void printData(boost::shared_ptr<string> e) {
    cout << "Data received: " << *e << endl;
}

TEST(HandlerTest, testDispatch)
{
    HandlerPtr eh(new EventFunctionHandler(boost::bind(&printEvent, _1)));
    HandlerPtr
            dh(new DataFunctionHandler<string> (boost::bind(&printData, _1)));

    EventPtr e(new Event());
    e->setData(boost::shared_ptr<string>(new string("blub")));
    e->setScope(Scope("/blah"));
    e->setType("string");

    eh->handle(e);
    dh->handle(e);
}
