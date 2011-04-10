/*!
 * \file Configuration.h
 *
 * \author Sebastian Wrede
 * \date 28.07.2010
 *
 * Provides configuration mechanisms for RSB. Currently XCF-Style.
 * @todo Should be able to read XSRAD configs. (jwienke: really?)
 * @todo missing license headers
 */

#pragma once

#include <string>
#include <vector>
#include <map>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include "rsb/rsbexports.h"

namespace rsb {
namespace util {

// a table of properties that can be referenced by name
typedef std::map<std::string, std::string> PropertyDic;

class Configuration;
typedef boost::shared_ptr<Configuration> ConfigurationPtr;

/**
 * @class Configuration Configuration.h
 *
 * This class implements the configuration scheme for RSB applications.
 * It can be used to retrieve properties in a key/value style. Those
 * properties are read from the environment, the home folder of the
 * current user and the working directory of the current process. As
 * this class implements a Singleton, only one instance is available in
 * any one process.
 *
 * @author: swrede
 * @date: 28.12.2007
 */
class RSB_EXPORT Configuration {
	// TODO think about support for user defined properties
public:
	static ConfigurationPtr getInstance();
	::std::string getProperty(const std::string& key);
	int getPropertyAsInt(const std::string& key);
	void setProperty(const std::string& key, const ::std::string& value);
	~Configuration();

protected:
	Configuration();
	void loadSettings();
	void load(const std::string& file);
	void loadEnv();
	Configuration(const Configuration&); // not implemented -- linker will complain if used

private:
	// the singleton instance of the properties object
	static ConfigurationPtr propertiesSingleton;
	// flag to signal, whether the properties are initialized
	static bool propertiesInitialized;
	// mutex to protect properties-initialization
	static boost::mutex initMtx;
	// recursive mutex to protect property access
	boost::recursive_mutex mtx;
	// map of all properties
	std::map<std::string, std::string> properties;
	// vector of all valid property-names
	std::vector<std::string> keys;

	// this method pares a single line to set a property (if applicable)
	void parseLine(const std::string&);

	// initialize internal map with default values for XCF properties
	void setXcfDefaults();

	void setCommonProperties();
	void setJavaProperties();
	void setIceProperties();
	void setSpreadProperties();
	void setLoggingProperties();
};

}

}

