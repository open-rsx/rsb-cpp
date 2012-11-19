/* ============================================================
 *
 * This file is a part of the RSB project
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
#include <sstream>
#include <fstream>

#include <stdlib.h>
#include <math.h>

#include <rsb/Informer.h>
#include <rsb/Factory.h>

using namespace std;
using namespace rsb;

int main(int argc, char** argv) {

    Factory& factory = getFactory();

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " [scope] [file with contents]" << endl;
        return EXIT_FAILURE;
    }

    // open file
    ifstream in;
    in.open(argv[2]);

    if (!in.is_open()) {
        cerr << "Unable to open " << argv[2] << endl;
        return EXIT_FAILURE;
    }

    stringstream contents;
    while (in.good()) {
        string line;
        getline(in, line);
        contents << line;
    }
    in.close();

    // publish
    Informer<string>::Ptr informer = factory.createInformer<string> (
            Scope(argv[1]));
    informer->publish(boost::shared_ptr<string>(new string(contents.str())));

    return EXIT_SUCCESS;
}

