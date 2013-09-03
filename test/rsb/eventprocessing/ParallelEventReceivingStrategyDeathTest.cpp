/* ============================================================
 *
 * This file is a part of the RSB project.
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

#include <iostream>

#include <stdlib.h>

#include <boost/thread.hpp>

#include "rsb/eventprocessing/ParallelEventReceivingStrategy.h"
#include "rsb/util/QueuePushHandler.h"
#include "rsb/filter/ScopeFilter.h"

using namespace std;
using namespace rsb;
using namespace rsb::eventprocessing;
using namespace rsc::threading;
using namespace rsb::filter;
using namespace rsb::util;

using namespace std;

class ErrorGeneratingHandler: public rsb::Handler {
public:

    ErrorGeneratingHandler(const bool& catchAll) :
        catchAll(catchAll) {
    }

    virtual ~ErrorGeneratingHandler() {

    }

    string getClassName() const {
        return "ErrorGeneratingHandler";
    }

    void handle(EventPtr /*event*/) {
        if (catchAll) {
            throw "A random string";
        } else {
            throw runtime_error("A random error");
        }
    }

private:
    bool catchAll;

};

int main(int argc, char** argv) {

    if (argc != 2) {
        cerr << "expected argument" << endl;
        return EXIT_SUCCESS;
    }

    bool catchAll = false;
    if (string(argv[1]) == "exception") {
        catchAll = false;
    } else if (string(argv[1]) == "catchall") {
        catchAll = true;
    } else {
        cerr << "unexpected argument: " << argv[1] << endl;
        return EXIT_SUCCESS;
    }

    ParallelEventReceivingStrategy processor(1);
    rsb::HandlerPtr exceptionHandler(new ErrorGeneratingHandler(catchAll));
    processor.addHandler(exceptionHandler, true);
    processor.setHandlerErrorStrategy(ParticipantConfig::ERROR_STRATEGY_EXIT);

    EventPtr event(new Event);
    event->setData(boost::shared_ptr<string>(new string("hello")));
    processor.handle(event);

    boost::this_thread::sleep(boost::posix_time::milliseconds(10000));

    return EXIT_SUCCESS;

}

