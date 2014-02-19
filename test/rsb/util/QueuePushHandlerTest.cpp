/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2014 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "rsb/util/QueuePushHandler.h"

using namespace std;
using namespace testing;
using namespace rsb;
using namespace rsb::util;

TEST(QueuePushHandlerTest, testPush)
{

    boost::shared_ptr<rsc::threading::SynchronizedQueue<boost::shared_ptr<string> > > queue(
            new rsc::threading::SynchronizedQueue<boost::shared_ptr<string> >);
    QueuePushHandler<string> handler(queue);
    EventPtr event(new Event);
    boost::shared_ptr<string> data(new string("test"));
    event->setData(data);
    handler.handle(event);
    EXPECT_EQ(data, queue->pop());

}

TEST(QueuePushHandlerTest, testEventWarning)
{
    boost::shared_ptr<rsc::threading::SynchronizedQueue<EventPtr> > queue(
                new rsc::threading::SynchronizedQueue<EventPtr>);
    QueuePushHandler<Event> handler(queue);
    EventPtr event(new Event);
    boost::shared_ptr<Event> data(new Event);
    event->setData(data);
    handler.handle(event);
    EXPECT_EQ(data, queue->pop());
}
