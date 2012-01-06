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

#include <iostream>

#include <stdlib.h>
#include <math.h>

#include <boost/timer.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;

int main(void) {

    // first get a factory instance that is used to create RSB domain objects
    Factory& factory = Factory::getInstance();

    // create an informer that is capable of sending events containing any kind
    // of data on the scope "/example/informer".
    Informer<AnyType>::Ptr informer = factory.createInformer<AnyType> (
            Scope("/example/informer"));

    // create data to send over the informer. Data is always maintained in
    // shared_ptr instances. Here we use typed informer to create a shared_ptr
    // of the data type we want to send
    Informer<string>::DataPtr s(new string("blub"));

    boost::timer t;

    // really send the data several times
    for (int j = 0; j < 1200; j++) {
        informer->publish(s);
    }

    cout << "Elapsed time for " << 1200 << " messages sent: " << t.elapsed()
            << " s" << endl;

    return EXIT_SUCCESS;
}
