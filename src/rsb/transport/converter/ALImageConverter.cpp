/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "rsb/transport/converter/ALImageMessage.pb.h"

using namespace std;
using namespace AL;

namespace rsb {
namespace transport {

ALImageConverter::ALImageConverter() {
}

ALImageConverter::~ALImageConverter() {
}

void ALImageConverter::serialize(const std::string &type, boost::shared_ptr<
		void> data, string &m) {
	if (type == "ALImage") {

		boost::shared_ptr<ALImage> image = boost::static_pointer_cast<ALImage>(
				data);
		ALImageMessage message;

		message.set_data((char *) image->getData(), image->getSize());
		message.set_resolution(image->getResolution());
		cout << "resolution before: " << image->getResolution() << endl;
		message.set_colorspace(image->getColorSpace());
		message.SerializeToString(&m);

		//		cout << "ALImageConverter::serialize after cast" << endl;
		//		cout << "ALImage allocated size:" << image->getAllocatedSize() << endl;
		//		cout << "ALImage size:" << image->getSize() << endl;
		//		cout << "ALImage allocated sizeof:" << sizeof(image->getData()) << endl;
		//		cout << "ALImageConverter::serialize after assign" << endl;

	} else {
		// TODO error handling!
		assert(false);
	}
}

boost::shared_ptr<void> ALImageConverter::deserialize(const std::string &type,
		const std::string &d) {
	boost::shared_ptr<void> p;
	if (type == "ALImage") {

		ALImageMessage message;
		message.ParseFromString(d);

		cout << "resolution after: " << message.resolution() << endl;
		ALImage *image = new ALImage(message.resolution(),
				message.colorspace(), false);
		// data needs to be copied here because the message is removed at the
		// end of this function
		memcpy(image->getData(), message.data().c_str(), image->getSize());

		//		cout << "ALImageConverter::deserialize after c_str()" << endl;
		//		cout << "ALImage allocated size:" << image->getAllocatedSize() << endl;
		//		cout << "ALImage size:" << image->getSize() << endl;
		//		cout << "ALImage allocated sizeof:" << sizeof(image->getData()) << endl;

		boost::shared_ptr<ALImage> s(image);
		p = boost::static_pointer_cast<void>(s);

	} else {
		// TODO better error handling
		throw("No such type registered at TypeFactory!");
	}
	return p;
}
;

}

}
