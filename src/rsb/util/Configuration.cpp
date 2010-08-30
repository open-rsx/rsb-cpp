/*
 * Configuration.cpp
 *
 *  Created on: 28.07.2010
 *      Author: swrede
 */

#include "Configuration.h"

#ifdef WIN32
#include "rsb_config.hpp"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rsc/logging/Logger.h>

#include "ConfigException.h"

#ifndef WIN32
extern char **environ;
#endif

using namespace std;
using namespace rsc::logging;

namespace {
LoggerPtr logger(Logger::getLogger("rsb.util.Configuration"));
}

namespace rsb {

namespace util {

// just for compatibility
enum RecoverLevel {
	NONE, LIMITED, FULL
};

// define static members
bool Configuration::propertiesInitialized = false;
ConfigurationPtr Configuration::propertiesSingleton;
boost::mutex Configuration::initMtx;

string convertFromEnviron(char* text) {
	char buf[2000];
	strncpy(buf, text, 1999);

	int i = 0;
	while ((buf[i] != 0) && (buf[i] != '=')) {
		if (buf[i] == '_') {
			buf[i] = '.';
		}
		i++;
	}
	string s(buf);

	return s;
}

void openWinsockDll() {
	// windows only: initializes winsock dll
#ifdef WIN32
	WSADATA WSAData;
	if(::WSAStartup(MAKEWORD(1, 0), &WSAData))
	cerr << "error initializing winsock dll" << endl;
#endif
}

string getHomeDir() {
#ifndef WIN32
	string home = getenv("HOME") + string("/");
	;
#else
	string home = string(getenv("HOMEDRIVE")) + string(getenv("HOMEPATH")) + string("\\");
#endif
	return home;
}

void Configuration::setCommonProperties() {
	// basic Configuration
	keys.push_back("XCF.Initial.Host");
	char buf[200];
	gethostname(buf, 200);
	setProperty("XCF.Initial.Host", buf);

	// COUT, CERR, NONE, FILE
	keys.push_back("XCF.LogAppender");
	setProperty("XCF.LogAppender", "CERR");

	keys.push_back("XCF.Logfile");
	setProperty("XCF.Logfile", "xcf.log");

	keys.push_back("XCF.Subscriber.TimeOutTime");
	setProperty("XCF.Subscriber.TimeOutTime", "1000");

	// advanced Configuration
	keys.push_back("XCF.Network.Interface");
	setProperty("XCF.Network.Interface", buf);

	keys.push_back("XCF.Initial.Port");
	setProperty("XCF.Initial.Port", "10000");

	keys.push_back("XCF.ThreadPool.Size");
	setProperty("XCF.ThreadPool.Size", "5");

	keys.push_back("XCF.ThreadPool.SizeMax");
	setProperty("XCF.ThreadPool.SizeMax", "100");

	keys.push_back("XCF.Publisher.QueueMaxSize");
	setProperty("XCF.Publisher.QueueMaxSize", "10000");

	keys.push_back("XCF.Subscriber.QueueMaxSize");
	setProperty("XCF.Subscriber.QueueMaxSize", "100000");

	keys.push_back("XCF.XCFAppender.Name");
	setProperty("XCF.XCFAppender.Name", "XCFAppLogger");

	keys.push_back("XCF.Events.DispatchTimeout");
	setProperty("XCF.Events.DispatchTimeout", "100");

	keys.push_back("XCF.Events.DispatchTimeout");
	setProperty("XCF.Events.DispatchTimeout", "20");

	// deprecated Configuration
	// NONE, LIMITED, FULL
	keys.push_back("XCF.Global.RecoverLevel");
	setProperty("XCF.Global.RecoverLevel", "NONE");

	keys.push_back("XCF.Global.RecoverTimeout");
	setProperty("XCF.Global.RecoverTimeout", "600");
}

void Configuration::setJavaProperties() {
	// not used so far in C++ XCF, taken into account here
	// to prevent warning about unknown property
	keys.push_back("XCF.Global.PingInterval");
	setProperty("XCF.Global.PingInterval", "1000");

	keys.push_back("XCF.Publisher.MessageRetrievalBundleSize");
	setProperty("XCF.Publisher.MessageRetrievalBundleSize", "10");

	keys.push_back("XCF.Subscriber.MessageRetrievalInterval");
	setProperty("XCF.Subscriber.MessageRetrievalInterval", "100");

}

void Configuration::setIceProperties() {
	keys.push_back("XCF.ObjectAdapter.Name");
	setProperty("XCF.ObjectAdapter.Name", "XCFAdapter");

	keys.push_back("XCF.ObjectAdapter.Port");
	setProperty("XCF.ObjectAdapter.Port", "2000");

	keys.push_back("XCF.Dispatcher.ObjectAdapter.Name");
	setProperty("XCF.Dispatcher.ObjectAdapter.Name", "DispatcherOA");

	keys.push_back("Ice.Trace.Network");
	setProperty("Ice.Trace.Network", "1");

	keys.push_back("Ice.Trace.Protocol");
	setProperty("Ice.Trace.Protocol", "0");

	keys.push_back("Ice.Trace.Retry");
	setProperty("Ice.Trace.Retry", "2");

	keys.push_back("Ice.Trace.Slicing");
	setProperty("Ice.Trace.Slicing", "1");

	keys.push_back("Ice.Warn.Connections");
	setProperty("Ice.Warn.Connections", "1");

	keys.push_back("Ice.Warn.Dispatch");
	setProperty("Ice.Warn.Dispatch", "2");

	keys.push_back("Ice.Warn.AMICallback");
	setProperty("Ice.Warn.AMICallback", "1");

	keys.push_back("Ice.Warn.Leaks");
	setProperty("Ice.Warn.Leaks", "0");

	//IMPORTANT: IF CHANGED, XCF WILL NOT FUNCTION PROPERLY
	keys.push_back("Ice.MonitorConnections");
	setProperty("Ice.MonitorConnections", "1");

	keys.push_back("Ice.Logger.Timestamp");
	setProperty("Ice.Logger.Timestamp", "0");

	keys.push_back("Ice.MessageSizeMax");
	setProperty("Ice.MessageSizeMax", "10240");

	keys.push_back("Ice.Default.Package");
	setProperty("Ice.Default.Package", "net.sf.xcf.ice");
}

void Configuration::setSpreadProperties() {
	keys.push_back("Spread.Port");
	setProperty("Spread.Port", "4803");

	keys.push_back("Spread.Host");
	setProperty("Spread.Host", "localhost");

	keys.push_back("Spread.StartDaemon");
	setProperty("Spread.StartDaemon", "1");

	keys.push_back("Spread.Path");
	setProperty("Spread.Path", "");
}

void Configuration::setLoggingProperties() {
	// TODO adapt or add current java Configuration
	// DebugLevels for framework components
	//NONE, DEBUG, WARN, ERROR
	keys.push_back("XCF.DebugLevel.XCFLogger");
	setProperty("XCF.DebugLevel.XCFLogger", "WARN");

	keys.push_back("XCF.DebugLevel.ServerLogger");
	setProperty("XCF.DebugLevel.ServerLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.RemoteServerLogger");
	setProperty("XCF.DebugLevel.RemoteServerLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.PublisherLogger");
	setProperty("XCF.DebugLevel.PublisherLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.SubscriberLogger");
	setProperty("XCF.DebugLevel.SubscriberLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.InterceptorLogger");
	setProperty("XCF.DebugLevel.InterceptorLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.SubscriberInterceptDistributorLogger");
	setProperty("XCF.DebugLevel.SubscriberInterceptDistributorLogger",
			"DERIVED");

	keys.push_back("XCF.DebugLevel.PublisherInterceptDistributorLogger");
	setProperty("XCF.DebugLevel.PublisherInterceptDistributorLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.DispatcherInterceptDistributorLogger");
	setProperty("XCF.DebugLevel.DispatcherInterceptDistributorLogger",
			"DERIVED");

	keys.push_back("XCF.DebugLevel.ServerInterceptDistributorLogger");
	setProperty("XCF.DebugLevel.ServerInterceptDistributorLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.LoggingInterceptor");
	setProperty("XCF.DebugLevel.LoggingInterceptor", "DERIVED");

	keys.push_back("XCF.DebugLevel.TerminateLogger");
	setProperty("XCF.DebugLevel.TerminateLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.UnexpectedLogger");
	setProperty("XCF.DebugLevel.UnexpectedLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.IceLogger");
	setProperty("XCF.DebugLevel.IceLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.DefaultDispatcherLogger");
	setProperty("XCF.DebugLevel.DefaultDispatcherLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.SimpleDispatcherLogger");
	setProperty("XCF.DebugLevel.SimpleDispatcherLogger", "DERIVED");

	keys.push_back("XCF.DebugLevel.XCFAppenderLogger");
	setProperty("XCF.DebugLevel.XCFAppenderLogger", "WARN");
}

Configuration::Configuration() {
	setXcfDefaults();
}

Configuration::~Configuration() {
}

void Configuration::setXcfDefaults() {
	setCommonProperties();
	setIceProperties();
	setSpreadProperties();
	setJavaProperties();
	setLoggingProperties();
}

ConfigurationPtr Configuration::getInstance() {
	boost::mutex::scoped_lock scoped_lock(initMtx);

	if (propertiesInitialized == false) {
		RSBDEBUG(logger,"creating single Configuration instance");
		openWinsockDll();
		// create single instance
		propertiesSingleton = ConfigurationPtr(new Configuration());
		RSBDEBUG(logger,"now parsing configuration");
		propertiesSingleton->loadSettings();
		RSBDEBUG(logger,"property singleton constructed");
		propertiesInitialized = true;
		return propertiesSingleton;
	} else {
		return propertiesSingleton;
	}
}

string Configuration::getProperty(const string& key) {
	boost::recursive_mutex::scoped_lock scoped_lock(mtx);

	map<string, string>::const_iterator p = properties.find(key);
	if (p != properties.end()) {
		return p->second;
	} else {
		RSBWARN(logger, "property not found with key: " + key);
		throw ConfigException(string("Can't find property with key: ")
				+ string(key));
	}
}

int Configuration::getPropertyAsInt(const string& key) {
	boost::recursive_mutex::scoped_lock scoped_lock(mtx);

	int value = 0;
	map<string, string>::const_iterator p = properties.find(key);
	if (p != properties.end()) {
		istringstream v(p->second);
		if (!(v >> value) || !v.eof()) {
			RSBWARN(logger, "could not conert property with key " + key
					+ " to int type");
			throw ConfigException(string(
					"Can't convert property to integer value with key: ")
					+ string(key));
		}
	} else {
		RSBWARN(logger, "property not found with key: " + key);
		throw ConfigException(string("Can't find property with key: ")
				+ string(key));
	}

	return value;
}

void Configuration::setProperty(const string& key, const string& value) {
	boost::recursive_mutex::scoped_lock scoped_lock(mtx);

	// Is property to be set contained in list of allowed properties?
	std::vector<std::string>::iterator k;
	bool valid = false;
	for (k = keys.begin(); k != keys.end(); k++) {
		if (*k == key)
			valid = true;
	}

	if (!valid) {
		RSBWARN(logger, "property not found with key: " + key);
		throw ConfigException("Property " + key + " doesn't exist.");
	}

	if (key == "XCF.Global.RecoverLevel") {
		if (value == "NONE") {
			ostringstream is;
			is << (int) NONE;
			setProperty("XCF.Global.RecoverLevel", is.str());
			return;
		} else if (value == "LIMITED") {
			ostringstream is;
			is << (int) LIMITED;
			setProperty("XCF.Global.RecoverLevel", is.str());
			return;
		} else if (value == "FULL") {
			ostringstream is;
			is << (int) FULL;
			setProperty("XCF.Global.RecoverLevel", is.str());
			return;
		}
	}

	if (!key.empty()) {
		if (!value.empty()) {
			properties[key] = value;
		} else {
			properties.erase(key);
		}
	}
}

void Configuration::loadSettings() {
	// read settings
	RSBDEBUG(logger,"reading user defined settings");
	string home = getHomeDir();
	// TODO how to handle additional user defined loggers?
	// first, parse user's home folder .xcfrc file
	load(home + ".xcfrc");
	// second, parse the .xcfrc file in the working directory
	load(".xcfrc");
	// third, parse the user's environment variables
	loadEnv();
}

void Configuration::load(const string& file) {
	try {
		ifstream in(file.c_str());
		if (!in.is_open()) {
			// Ignored. No config file present at given location.
			RSBDEBUG(logger,"no .xcfrc found at location: " + file);
		} else {
			// parse contents
			char line[1024];
			while (in.getline(line, 1024)) {
				parseLine(line);
			}
		}
	} catch (const std::exception& ex) {
		RSBERROR(logger,
				"catched standard exception during parsing of file at location: "
						+ file);
		RSBERROR(logger,"catched exception message is: " + string(ex.what()));
	} catch (...) {
		RSBERROR(logger,
				"catched unknown exception during parsing of config file in home folder");
	}
}

void Configuration::loadEnv() {
#ifndef WIN32
	int i;
	for (i = 0; environ[i] != NULL; i++) {
		try {
			// parse only relevant environment variables
			if ((strncmp(environ[i], "XCF", 3) == 0) || (strncmp(environ[i],
					"Ice", 3) == 0) || (strncmp(environ[i], "Spr", 3) == 0)) {
				parseLine( convertFromEnviron(environ[i]));
			}
		} catch (...) {
			RSBERROR(logger,
					"catched unknown exception during parsing of environment variables");
		}
	}
#endif
}

void Configuration::parseLine(const std::string& line) {
	const string delim = " \t\r\n";
	string s = line;

	string::size_type idx = s.find('#');
	if (idx != string::npos) {
		s.erase(idx);
	}

	idx = s.find_last_not_of(delim);
	if (idx != string::npos && idx + 1 < s.length()) {
		s.erase(idx + 1);
	}

	string::size_type beg = s.find_first_not_of(delim);
	if (beg == string::npos) {
		return;
	}

	string::size_type end = s.find_first_of(delim + "=", beg);
	if (end == string::npos) {
		return;
	}

	string key = s.substr(beg, end - beg);

	end = s.find('=', end);
	if (end == string::npos) {
		return;
	}
	++end;

	string value;
	beg = s.find_first_not_of(delim, end);
	if (beg != string::npos) {
		end = s.length();
		value = s.substr(beg, end - beg);
	}

	try {
		setProperty(key, value);
	} catch (const ConfigException &ex) {
		// warn but ignore invalid property
		RSBWARN(logger, "Exception catched during parsing of XCF config: " + string(
				ex.what()));
	}
}

}

}
