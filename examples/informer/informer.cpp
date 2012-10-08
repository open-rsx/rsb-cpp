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

// mark-start::body
#include <stdlib.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsb;

int main(void) {

    // First get a factory instance that is used to create RSB domain
    // objects.
    Factory& factory = Factory::getInstance();

    // Create an informer that is capable of sending events containing
    // string data on the scope "/example/informer".
    Informer<string>::Ptr informer
        = factory.createInformer<string> ("/example/informer");

    // Create data to send over the informer. Data is always
    // maintained in shared_ptr instances. Informer provides a typedef
    // DataPtr of the appropriate type according to its template
    // parameter
    Informer<string>::DataPtr s(new string("example payload"));

    // Send the data.
    informer->publish(s);

    return EXIT_SUCCESS;
}
// mark-end::body
