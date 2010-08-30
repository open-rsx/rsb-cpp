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
	if (type=="ALImage") {
		boost::shared_ptr<ALImage> image = boost::static_pointer_cast<ALImage>(data);
		m.assign((char *) image->getData(),image->getSize());
	}
}


boost::shared_ptr<void> ALImageConverter::deserialize(const std::string &type, const std::string &d) {
	boost::shared_ptr<void> p;
	if (type=="ALImage") {
		ALImage *image = new ALImage(kQQVGA, kRGBColorSpace, false);

		image->setData((unsigned char*) d.c_str());

		boost::shared_ptr<ALImage> s(image);
		p = boost::static_pointer_cast<void>(s);

	} else {
		throw("No such type registered at TypeFactory!");
	}
	return p;
};

}

}
