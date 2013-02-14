/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2013 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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
#include <boost/shared_ptr.hpp>

#include <rsb/Factory.h>
#include <rsb/converter/Repository.h>

#include "SimpleImage.h"
#include "SimpleImageConverter.h"

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

using namespace converter_tutorial;

int main() {
    // Register our converter within the collection of converters for
    // the string wire-type (which is used for arrays of octets in
    // C++).
    //
    // Try senderNoConverter.cpp to see what happens, if the converter
    // is not registered.
    shared_ptr<SimpleImageConverter> converter(new SimpleImageConverter());
    converterRepository<std::string>()->registerConverter(converter);

    // Create an Informer object that is parametrized with the
    // data-type SimpleImage.
    Informer<SimpleImage>::Ptr informer =
            getFactory().createInformer<SimpleImage> (
                    Scope("/tutorial/converter"));

    // Construct and send a SimpleImage object.
    shared_ptr<SimpleImage> image(new SimpleImage());
    image->width = 10;
    image->height = 10;
    image->data = new unsigned char[100];
    informer->publish(image);

    return EXIT_SUCCESS;
}
// mark-end::body
