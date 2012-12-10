/* ============================================================
 *
 * This file is a part of the RSB project
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "rsb/converter/ProtocolBufferConverter.h"
#include "rsb/converter/TestMessage.pb.h"

using namespace std;
using namespace rsb;
using namespace rsb::converter;
using namespace testing;

// This test tests linking errors with subclassing from a template-based class.
// It is only effective if there is a second file doing exactly the same thing.
// That's the reason why this file is duplicated in the source tree with
// slightly different names.
// For an explanation of the potential linking problems please refer to:
// http://www.codesynthesis.com/~boris/blog/2010/01/18/dll-export-cxx-templates/
TEST(ProtocolBufferConverterTest, testWindowsLinking)
{
    ProtocolBufferConverter<rsb::converter::TestMessage> converter;
}
