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

#pragma once

#include <string>
#include <vector>
#include <map>

#include <rsc/runtime/NoSuchObject.h>
#include <rsc/runtime/TypeStringTools.h>
#include <rsc/runtime/Printable.h>
#include <rsc/patterns/Factory.h>

#include "InPullConnector.h"
#include "InPushConnector.h"
#include "OutConnector.h"

namespace rsb {
namespace transport {

/**
 * Objects of this class are specialized factories that construct @ref
 * Connector objects and provide introspection for connector
 * implementations without instantiating them.
 *
 * @author jmoringe
 */
template <typename Interface>
class ConnectorFactory: public rsc::patterns::Factory<std::string, Interface>,
			public rsc::patterns::Singleton< ConnectorFactory<Interface> >,
			public rsc::runtime::Printable {
public:
    /**
     * Instances of this class describe capabilities and properties of
     * connector implementations.
     *
     * @author jmoringe
     */
    class ConnectorInfo: public rsc::runtime::Printable {
    public:
	typedef std::set<std::string> SchemaList;
	typedef std::set<std::string> OptionList;

	ConnectorInfo(const std::string &name,
		      const SchemaList &schemas,
		      const OptionList &options) :
	    name(name), schemas(schemas), options(options) {
	    this->options.insert("enabled");
	}

	/**
	 * Return the name of the implementation.
	 */
	std::string getName() const {
	    return this->name;
	}

	/**
	 * Return the set of schemas supported by the connector
	 * implementation.
	 *
	 * @return A @ref std::set containing the supported schemas as
	 * strings.
	 */
	SchemaList getSchemas() const {
	    return this->schemas;
	}

	/**
	 * Return a list of option names describing configurations
	 * options recognized by the implementation.
	 *
	 * @return A @ref std::set containing the names of recognized
	 * options.
	 */
	OptionList getOptions() const {
	    return this->options;
	}

        bool operator<(const ConnectorInfo &other) const {
            if (this->name < other.name) {
                return true;
            } else if (this->name == other.name) {
                if (this->schemas < other.schemas) {
                    return true;
                } else if (this->schemas == other.schemas) {
                    return this->options < other.options;
                }
            }
            return false;
        }
    private:
	std::string name;
	SchemaList schemas;
	OptionList options;

	std::string getClassName() const {
	    return "ConnectorInfo";
	}

	void printContents(std::ostream &stream) const {
	    stream << this->name
		   << ", schemas = " << this->schemas
		   << ", options = " << this->options;
	}
    };

private:
    typedef rsc::patterns::Factory<std::string, Interface> Factory;
    typedef typename Factory::CreateFunction CreateFunction;
    typedef typename Factory::ImplMapProxy ImplMapProxy;
    typedef std::map<std::string, ConnectorInfo> InfoMap; // forward
public:

    /** Return information regarding the connector implementation
     * named @a name.
     *
     * @param name Name of the implementation for which information
     * should be returned.
     * @return A @ref ConnectorInfo object.
     * @throw rsc::runtime::NoSuchObject If a record for @a name
     * cannot be found.
     */
    ConnectorInfo getConnectorInfo(const std::string &name) const {
	typename InfoMap::const_iterator it = this->infos.find(name);
	if (it == this->infos.end()) {
	    throw rsc::runtime::NoSuchObject(name);
	}
	return it->second;
    }

    std::set<ConnectorInfo> getConnectorInfos() const {
        std::set<ConnectorInfo> result;

        for (typename InfoMap::const_iterator it = this->infos.begin();
             it != this->infos.end(); ++it) {
            result.insert(it->second);
        }
        return result;
    }

    /**
     * For the connector implementation named @a name, register the
     * construct function @a constructor, supported schemas @a schemas
     * and recognized configuration options @a options.
     *
     * @param name Name of the connector implementation.
     * @param constructor Construct function.
     * @param schemas A list of strings designating schemas supported
     * by the implementation.
     * @param options A list of strings describing configuration
     * options recognized by the implementation.
     */
    void registerConnector(const std::string &name,
			   const CreateFunction &constructor,
			   const std::set<std::string> &schemas = std::set<std::string>(),
			   const std::set<std::string> &options = std::set<std::string>()) {
	Factory::impls().register_(name, constructor);

	ConnectorInfo info(name, schemas, options);
	this->infos.insert(std::make_pair(name, info));
    }

    void registerConnector(const std::string &name,
			   const CreateFunction &constructor,
			   const std::string &schema,
			   const std::set<std::string> &options = std::set<std::string>()) {
	std::set<std::string> schemas;
	schemas.insert(schema);
	registerConnector(name, constructor, schemas, options);
    }
private:
    InfoMap infos;

    std::string getClassName() const {
	return "ConnectorFactory<" + rsc::runtime::typeName<Interface>() + ">";
    }

    void printContents(std::ostream &stream) const {
	const ImplMapProxy &implementations = Factory::impls();
	stream << std::endl;
	for (typename ImplMapProxy::const_iterator it = implementations.begin(); it
		 != implementations.end(); ++it) {
	    stream << "\t" << getConnectorInfo(it->first) << std::endl;
	}
    }
};

typedef ConnectorFactory<InPullConnector> InPullFactory;

typedef ConnectorFactory<InPushConnector> InPushFactory;

typedef ConnectorFactory<OutConnector> OutFactory;

}
}
