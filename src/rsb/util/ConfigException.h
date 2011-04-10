/*!
 * \file Configuration.h
 *
 * \author Sebastian Wrede
 * \date 28.07.2010
 *
 * Exception thrown upon configuration errors.
 */

#pragma once

#include "../RSBException.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace util {

/**
 * General exception thrown upon configuration errors.
 *
 * @author: swrede
 */
class RSB_EXPORT ConfigException: public rsb::RSBException {
public:
	ConfigException(const std::string& m);
	virtual ~ConfigException() throw ();
};

}
}

