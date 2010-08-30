/*
 * TypeFactoryRegistry.h
 *
 *  Created on: 03.08.2010
 *      Author: swrede
 */

#ifndef TYPEFACTORYREGISTRY_H_
#define TYPEFACTORYREGISTRY_H_

namespace rsb {

namespace transport {

class TypeFactoryRegistry {
public:
	TypeFactoryRegistry();
	virtual ~TypeFactoryRegistry();

	// TODO add pair spec (type, format)
	TypeFactory getTypeFactory(std::string format) {
		if (format=="binary") {
			return LocalTypeConverter();
		} else {
			return SpreadTypeConverter();
		}
	}
};

}

}

#endif /* TYPEFACTORYREGISTRY_H_ */
