/*
 * Copyright (c) 2015 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Intel Corporation nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file contains definition of the NvmCimXml class which
 * converts a CIM Instance to and from XML representation.
 */


#ifndef	_WBEM_FRAMEWORK_CIMXML_H_
#define	_WBEM_FRAMEWORK_CIMXML_H_

#include <map>
#include <string>
#include <sstream>
#include <rapidxml.hpp>
#include "Attribute.h"
#include "Exception.h"

#include "ObjectPath.h"


namespace wbem
{
namespace framework
{

// CIM XML Schema constants (node names, attribute names, ...)
// CIMXML is defined here:
// http://dmtf.org/sites/default/files/standards/documents/CIM_XML_Mapping20.html
static const std::string CX_CLASSNAME = "CLASSNAME";
static const std::string CX_PROPERTY = "PROPERTY";
static const std::string CX_PROPERTYARRAY = "PROPERTY.ARRAY";
static const std::string CX_NAME = "NAME";
static const std::string CX_TYPE = "TYPE";
static const std::string CX_VALUE = "VALUE";
static const std::string CX_VALUEARRAY = "VALUE.ARRAY";
static const std::string CX_TRUE = "TRUE"; // should be case insensitive
static const std::string CX_FALSE = "FALSE"; // should be case insensitive
static const std::string CX_INSTANCE = "INSTANCE";


/*!
 * CIM XML to Instance converter.
 */
class CimXml
{
	public:

		/*!
		 * Constructor initializes the XML document from the string.
		 * @throw NvmException on invalid XML string
		 * @param[in] cimXml
		 * 		The CIM XML string representing an NvmInstance.
		 */
		CimXml(std::string cimXml);

		/*!
		 * Retrieve the name of the NvmInstance.
		 * @throw NvmException if class name is not found or invalid.
		 * @return The NvmInstance class name.
		 */
		std::string getClass();

		/*!
		 * Retrieve the list of NvmAttributes for the NvmInstance.
		 * @throw NvmException if properties are not found or invalid.
		 * @return The list of NvmAttributes.
		 */
		attributes_t getProperties();

		/*!
		 * Convert an attribute type from a CIM XML type string to an enumeration.
		 * @param[in] str
		 * 		The attribute type as a string.
		 * @throw NvmException on invalid attribute type string.
		 * @return The attribute type as an enumeration.
		 * @note Does not account for lists.
		 */
		static enum DataType stringToEnum(std::string str);

		/*!
		 * Convert from a CIM XML type string to an attribute type.
		 * @param[in] str
		 * 		The CIM XML type string.
		 * @param[in] isArray
		 * 		True if the attribute is an array of values of the type.
		 * 	@return The attribute type as an enumeration.
		 */
		static enum DataType stringToAttributeType(std::string str, bool isArray);

		/*!
		 * Convert an attribute type from an enumeration to a string.
		 * @param[in] type
		 * 		The attribute type as an enumeration.
		 * @throw NvmException on invalid attribute type enumeration.
		 * @return The attribute type as a string.
		 */
		static std::string enumToString(enum DataType type);


	private:
		std::string m_classname;
		attributes_t m_attributes;
		template<class T>
		void notNull(T *pObj, const std::string &function, const std::string &item);
		void generateAttributes(const rapidxml::xml_node<> *pInstance, const std::string propertyTag,
				const std::string valueTag);
		Attribute createAttribute(std::string typeStr, rapidxml::xml_node<> *pValue, bool isKey,
				bool isArray);
		template<class T> std::vector<T> createList(rapidxml::xml_node<> *pValueArray);
		template<class T> T toType(std::string value);
		bool toBoolean(std::string value);

		static std::map<enum DataType, std::string> m_enumStringMap;
		static void setupMap();

		typedef std::pair<enum DataType, std::string> enumString_t;
		typedef std::map<enum DataType, std::string>::iterator enumStringIterator_t;
};

}  // framework
}  // wbem

#endif  // #ifndef _WBEM_FRAMEWORK_CIMXML_H_
