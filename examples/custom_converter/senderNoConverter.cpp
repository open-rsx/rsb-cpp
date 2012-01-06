/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/shared_ptr.hpp>

#include <rsb/Factory.h>

#include "SimpleImage.h"

using namespace boost;

using namespace rsb;

using namespace converter_tutorial;

// This program demonstrates the effect of using a data-type for which
// no converter is available (or at least not registered).
int main() {
    Informer<SimpleImage>::Ptr informer =
            Factory::getInstance().createInformer<SimpleImage> (
                    Scope("/tutorial/converter"));

    shared_ptr<SimpleImage> image(new SimpleImage());
    image->width = 10;
    image->height = 10;
    image->data = new unsigned char[100];
    informer->publish(image);

    return EXIT_SUCCESS;
}
