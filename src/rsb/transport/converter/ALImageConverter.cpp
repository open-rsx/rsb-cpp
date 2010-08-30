/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ALImageConverter.h"

using namespace std;
using namespace AL;

namespace rsb {

namespace transport {

ALImageConverter::ALImageConverter() {
	// TODO Auto-generated constructor stub

}

ALImageConverter::~ALImageConverter() {
	// TODO Auto-generated destructor stub
}

void ALImageConverter::serialize(const std::string &type, boost::shared_ptr<void> data, string &m) {
	cout << "ALImageConverter::serialize: " << type << endl;
	if (type=="ALImage") {
		cout << "ALImageConverter::serializing ALImage" << endl;
		boost::shared_ptr<ALImage> image = boost::static_pointer_cast<ALImage>(data);
		cout << "ALImageConverter::serialize after cast" << endl;
		cout << "ALImage allocated size:" << image->getAllocatedSize() << endl;
		cout << "ALImage size:" << image->getSize() << endl;
		cout << "ALImage allocated sizeof:" << sizeof(image->getData()) << endl;
		m.assign((char *) image->getData(),image->getSize());
		cout << "ALImageConverter::serialize after assign" << endl;
	}
}


boost::shared_ptr<void> ALImageConverter::deserialize(const std::string &type, const std::string &d) {
	boost::shared_ptr<void> p;
	if (type=="ALImage") {
		ALImage *image = new ALImage(kQQVGA, kRGBColorSpace, true);

		image->setData((unsigned char*) d.c_str());
		cout << "ALImageConverter::deserialize after c_str()" << endl;
		cout << "ALImage allocated size:" << image->getAllocatedSize() << endl;
		cout << "ALImage size:" << image->getSize() << endl;
		cout << "ALImage allocated sizeof:" << sizeof(image->getData()) << endl;

		boost::shared_ptr<ALImage> s(image);
		p = boost::static_pointer_cast<void>(s);

	} else {
		throw("No such type registered at TypeFactory!");
	}
	return p;
};

}

}
