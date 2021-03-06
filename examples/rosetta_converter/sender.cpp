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

#include <rsb/Factory.h>
#include <rsb/converter/Repository.h>
#include <rsb/converter/ProtocolBufferConverter.h>

// See ../CMakeLists.txt for the generation of this file.
// The generated file can be found in ${BUILD_DIR}/protobuf_converter
#include <protobuf_converter/SimpleImage.pb.h>

using namespace boost;

using namespace rsb;
using namespace rsb::converter;

// The generated protocol buffer class is in this namespace.
using namespace tutorial::protobuf_converter;

int main() {
    // Register a specific template instantiation of the
    // ProtocolBufferConverter for our SimpleImage protocol buffer
    // message.
    shared_ptr<ProtocolBufferConverter<SimpleImage> > converter(
            new ProtocolBufferConverter<SimpleImage> ());
    converterRepository<std::string>()->registerConverter(converter);

    // Create an informer which has the SimpleImage protocol buffer
    // message as its data type.
    Informer<SimpleImage>::Ptr informer =
            getFactory().createInformer<SimpleImage> (
                    Scope("/tutorial/converter"));

    // Create and publish an instance of SimpleImage. To see the
    // event, you can, for example use the RSB logger utility or the
    // receiver program in this directory.
    Informer<SimpleImage>::DataPtr data(new SimpleImage());
    data->set_width(10);
    data->set_height(10);
    data->set_data(new char[100], 100);
    informer->publish(data);

    return EXIT_SUCCESS;
}
