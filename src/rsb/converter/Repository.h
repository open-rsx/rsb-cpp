/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#pragma once

#include <string>
#include <stdexcept>
#include <set>
#include <iomanip>

#include <boost/format.hpp>

#include <rsc/runtime/Printable.h>
#include <rsc/runtime/TypeStringTools.h>
#include <rsc/runtime/NoSuchObject.h>
#include <rsc/logging/Logger.h>

#include "Converter.h"
#include "ConverterSelectionStrategy.h"
#include "UnambiguousConverterMap.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace converter {

/**
 * Maintains a collection of converters for a specific wire format. Each
 * converter has a wire type describing the actual message that is written on
 * the wire and a data type that indicates which data it can serialize on the
 * wire.
 *
 * @author jwienke
 * @tparam WireType the wire-type of the collected converters.
 */
template<class WireType>
class Repository: public rsc::runtime::Printable {
public:
    typedef typename Converter<WireType>::Ptr ConverterPtr;

    /** WireSchema and DataType */
    typedef std::pair<std::string, std::string> ConverterSignature;

    typedef std::map<std::string, std::string> ConverterSelectionMap;

    Repository() :
        logger(rsc::logging::Logger::getLogger("rsb.converter.Repository")) {
    }

    typename ConverterSelectionStrategy<WireType>::Ptr getConvertersForSerialization(const ConverterSelectionMap& selection
                                                                    = ConverterSelectionMap()) const {
        UnambiguousConverterMap<WireType>* result = new UnambiguousConverterMap<WireType>();
        for (typename ConverterMap::const_iterator it = this->converters.begin();
             it != this->converters.end(); ++it) {
            std::string wireSchema = it->first.first;
            std::string dataType   = it->first.second;
            // The data-type is not mentioned in the explicit
            // selection. Try to add the converter. This may throw in
            // case of ambiguity.
            if (selection.find(dataType) == selection.end()) {
                try {
                    result->addConverter(dataType, it->second);
                } catch (const std::invalid_argument& e) {
                    std::set<std::string> wireSchemas;
                    for (typename ConverterMap::const_iterator it_ = this->converters.begin();
                         it_ != this->converters.end(); ++it_) {
                        if (dataType == it_->first.second)
                            wireSchemas.insert(it_->first.first);
                    }
                    throw std::runtime_error(boost::str(boost::format("Ambiguous converter set for wire-type `%1%' and data-type `%2%': candidate wire-schemas are %3%; hint: add a configuration option `transport.<name>.converter.cpp.<one of %3%> = %2%' to resolve the ambiguity.")
                                                        % rsc::runtime::typeName<WireType>()
                                                        % dataType
                                                        % wireSchemas));
                }
            }
            // There is an entry for data-type in the explicit
            // selection. Add the converter if the wire-schema matches.
            else if (wireSchema == selection.find(dataType)->second) {
                RSCDEBUG(this->logger, "Found configured converter signature " << it->first);
                result->addConverter(dataType, it->second);
            }
        }
        return typename ConverterSelectionStrategy<WireType>::Ptr(result);
    }

    typename ConverterSelectionStrategy<WireType>::Ptr getConvertersForDeserialization(const ConverterSelectionMap& selection
                                                                      = ConverterSelectionMap()) const {
        UnambiguousConverterMap<WireType>* result = new UnambiguousConverterMap<WireType>();
        for (typename ConverterMap::const_iterator it = this->converters.begin();
             it != this->converters.end(); ++it) {
            std::string wireSchema = it->first.first;
            std::string dataType   = it->first.second;
            // The wire-schema is not mentioned in the explicit
            // selection. Try to add the converter. This may throw in
            // case of ambiguity.
            if (selection.find(wireSchema) == selection.end()) {
                try {
                    result->addConverter(wireSchema, it->second);
                } catch (const std::invalid_argument& e) {
                    std::set<std::string> dataTypes;
                    for (typename ConverterMap::const_iterator it_ = this->converters.begin();
                         it_ != this->converters.end(); ++it_) {
                        if (wireSchema == it_->first.first)
                            dataTypes.insert(it_->first.second);
                    }
                    throw std::runtime_error(boost::str(boost::format("Ambiguous converter set for wire-type `%1%' and wire-schema `%2%': candidate data-types are %3%; hint: add a configuration option `transport.<name>.converter.cpp.%2% = <one of %3%>' to resolve the ambiguity.")
                                                        % rsc::runtime::typeName<WireType>()
                                                        % wireSchema
                                                        % dataTypes));
                }
            }
            // There is an entry for wire-schema in the explicit
            // selection. Add the converter if the data-type matches.
            else if (dataType == selection.find(wireSchema)->second) {
                RSCDEBUG(this->logger, "Found configured converter signature " << it->first);
                result->addConverter(wireSchema, it->second);
            }
        }
        return typename ConverterSelectionStrategy<WireType>::Ptr(result);
    }

    /**
     * Registers the given converter in the collection.
     *
     * @param converter the converter to register
     * @throw std::invalid_argument if there is already a converter registered
     *                              with the same wire type or data type
     */
    void registerConverter(ConverterPtr converter) {
        std::string wireSchema = converter->getWireSchema();
        std::string dataType   = converter->getDataType();
        if (this->converters.find(std::make_pair(wireSchema, dataType))
            != this->converters.end()) {
            // TODO use RSB execption; but do we have one for invalid argument?
            throw std::invalid_argument(boost::str(boost::format("There already is a converter for wire-schema `%1%' and data-type `%2%'")
                                                   % wireSchema % dataType));
        }
        this->converters[std::make_pair(wireSchema, dataType)]
            = converter;
    }

    ConverterPtr getConverter(const std::string& wireSchema, const std::string& dataType) const {
        typename ConverterMap::const_iterator it
            = this->converters.find(std::make_pair(wireSchema, dataType));
        if (it == this->converters.end()) {
            throw rsc::runtime::NoSuchObject(boost::str(boost::format("Could not find a converter for wire-schema `%1%' and data-type `%2%'")
                                                        % wireSchema % dataType));
        }
        return it->second;
    }

    ConverterPtr getConverter(const ConverterSignature& signature) const {
        return getConverter(signature.first, signature.second);
    }

    void clear() {
        this->converters.clear();
    }

    typedef boost::shared_ptr<Repository<WireType> > Ptr;

private:
    typedef std::map<ConverterSignature, ConverterPtr> ConverterMap;

    rsc::logging::LoggerPtr logger;
    ConverterMap converters;

    std::string getClassName() const {
	return "Repository<" + rsc::runtime::typeName<WireType>() + ">";
    }

    void printContents(std::ostream& stream) const {
	stream << std::endl;
	for (typename ConverterMap::const_iterator it = this->converters.begin();
	     it != this->converters.end(); ++it) {
	    stream << "\t"
		   << std::setw(16) << std::left << it->first.first
		   << " <-> "
		   << std::setw(16) << std::left << it->first.second
		   << ": " << *it->second << std::endl;
	}
    }
};

RSB_EXPORT Repository<std::string>::Ptr stringConverterRepository();

}
}
