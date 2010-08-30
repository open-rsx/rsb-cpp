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

#ifndef WIN32
#define RSB_API
#endif

#include "../RSBException.h"

namespace rsb {

namespace util {

/** \brief General exception thrown upon configuration errors.
 *
 * @author: swrede
 */
class RSB_API ConfigException: public rsb::RSBException {
public:
	ConfigException(const std::string& m) : RSBException(m) {};
	virtual ~ConfigException() throw () {};
};

}

}

#endif /* CONFIGEXCEPTION_H_ */
