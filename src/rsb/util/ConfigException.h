/*!
 * \file Configuration.h
 *
 * \author Sebastian Wrede
 * \date 28.07.2010
 *
 * Exception thrown upon configuration errors.
 */

#ifndef CONFIGEXCEPTION_H_
#define CONFIGEXCEPTION_H_

#include "../RSBException.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace util {

/**
 * General exception thrown upon configuration errors.
 *
 * @author: swrede
 * @todo remove header implementation
 */
class RSB_EXPORT ConfigException: public rsb::RSBException {
public:
	ConfigException(const std::string& m) : RSBException(m) {};
	virtual ~ConfigException() throw () {};
};

}

}

#endif /* CONFIGEXCEPTION_H_ */
