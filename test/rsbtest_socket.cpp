/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2012 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <gmock/gmock.h>

#include "testhelpers.h"

#include "rsb/converter/converters.h"

using namespace std;
using namespace testing;

int main(int argc, char* argv[]) {

    srand(time(NULL));

    disableExternalConfigFiles();

    rsb::converter::registerDefaultConverters();

    InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();

}
