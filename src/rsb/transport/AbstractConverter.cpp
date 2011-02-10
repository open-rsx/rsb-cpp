/*
 * AbstractConverter.cpp
 *
 *  Created on: 03.08.2010
 *      Author: swrede
 */

#include "AbstractConverter.h"

namespace rsb {
namespace transport {

rsc::misc::Registry<AbstractConverter<std::string> > *stringConverterRegistry() {
	static rsc::misc::Registry<AbstractConverter<std::string> > *registry =
			new ::rsc::misc::Registry<AbstractConverter<std::string> >;
	return registry;
}

}
}
