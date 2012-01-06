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

#include "rsb/util/Task.h"
#include "rsb/util/PeriodicTask.h"
#include "rsb/transport/Connector.h"
#include "rsb/transport/inprocess/InProcessConnector.h"
#include "BTL2StringConverter.h"
#include "InformerTask.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

using namespace std;
using namespace rsb;
using namespace rsb::test;
using namespace rsb::util;
using namespace rsb::transport;
using namespace rsb::inprocess;

int main(void) {
	// task execution service
	TaskExecutorVoidPtr exec(new TaskExecutor<void> ());

	// in-process port
	ConnectorPtr p(new InProcessConnector());

	// domain objects
	boost::shared_ptr<InformerTask> source(new InformerTask(p));
	p->setObserver(boost::bind(&InformerTask::handler, source, _1));

	// activate port and schedule informer
	p->activate();
	TaskPtr task_source = exec->schedulePeriodic<InformerTask> (source, 0);

	// wait *here* for shutdown as this is not known to the Port
	{
		boost::recursive_mutex::scoped_lock lock(source->m);
		while (source->c != 10) {
			source->cond.wait(lock);
		}
	}

	// stop and join all threads
	exec->join(task_source); // no cancel needed as done already locally, see execute()
	// port is deactivated through dtr
	cerr << "InProcessTest finished" << endl;

	return EXIT_SUCCESS;
}
