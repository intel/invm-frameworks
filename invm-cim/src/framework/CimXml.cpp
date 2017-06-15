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
 * This file contains the implementation of the NvmCimXml class which
 * converts a CIM Instance to and from XML representation.
 */

#include <algorithm>
#include <sstream>
#include <map>
#include <iomanip>
#include <string/s_str.h>
#include <rapidxml.hpp>
#include <stdexcept>

#include "CimXml.h"

#include "ExceptionBadParameter.h"
#include "ExceptionNotSupported.h"
#include "StringUtil.h"

/*
 * Constructor requires CIM-XML string.  Initialize the the XML document.
 */
wbem::framework::CimXml::CimXml(std::string cimXml)
{
	try
	{
		// xml string being parsed must be modifiable. rapidxml does inline whitespace
		// normalization and other destructive changes
		if (cimXml.empty())
		{
			throw ExceptionBadParameter("cimXml");
		}
		char xml[cimXml.length() + 1];
		s_strcpy(xml, cimXml.c_str(), cimXml.length() + 1);
		rapidxml::xml_document<> doc;
		doc.parse<0>(xml);

		rapidxml::xml_node<> *pInstance = doc.first_node(CX_INSTANCE.c_str());

		notNull(pInstance, __FUNCTION__, CX_INSTANCE);
		rapidxml::xml_attribute<> *classnameAttribute = pInstance->first_attribute(CX_CLASSNAME.c_str());
		notNull(classnameAttribute, __FUNCTION__, CX_CLASSNAME);
		m_classname = classnameAttribute->value();

		generateAttributes(pInstance, CX_PROPERTY, CX_VALUE); // generate non-array attributes
		generateAttributes(pInstance, CX_PROPERTYARRAY, CX_VALUEARRAY); // generate array attributes
	}
	catch (const std::exception &)
	{
		throw ExceptionBadParameter("cimXml");
	}
}

void wbem::framework::CimXml::generateAttributes(const rapidxml::xml_node<> *pInstance,
		const std::string propertyTag, const std::string valueTag)
{
	rapidxml::xml_attribute<> *name;
	rapidxml::xml_attribute<> *type;
	rapidxml::xml_node<> *pValue;

	// If we expect an array of values, treat this as an array
	bool isArray = false;
	if (valueTag == CX_VALUEARRAY)
	{
		isArray = true;
	}

	for (rapidxml::xml_node<> *node = pInstance->first_node(propertyTag.c_str());
			node; node = node->next_sibling(propertyTag.c_str()))
	{
		name = node->first_attribute(CX_NAME.c_str());
		type = node->first_attribute(CX_TYPE.c_str());
		notNull(name, __FUNCTION__, CX_NAME);
		notNull(type, __FUNCTION__, CX_TYPE);

		pValue = node->first_node(valueTag.c_str());

		m_attributes[name->value()] = createAttribute(type->value(), pValue, true, isArray);
	}
}

/*
 * Get the class name for the instance
 */
std::string wbem::framework::CimXml::getClass()
{
	return m_classname;
}

/*
 * Get the list of all NvmAttributes (keys and not keys) from the CIM-XML Properties
 */
wbem::framework::attributes_t wbem::framework::CimXml::getProperties()
{
	return m_attributes;
}

/*
 * Helper function to quickly test if an object is null.  If it is throw an exception.
 */
template<class T>
void wbem::framework::CimXml::notNull(T* pObj, const std::string &function, const std::string &item)
{
	if (pObj == NULL)
	{
		throw ExceptionBadParameter(item.c_str());
	}
}

/*
 * Creates the appropriate NvmAttribute based on a string representation of the type and the value.
 */
wbem::framework::Attribute wbem::framework::CimXml::createAttribute(std::string typeStr, rapidxml::xml_node<> *pValue,
		bool isKey, bool isArray)
{
	setupMap();
	enum DataType type = stringToAttributeType(typeStr, isArray);

	// For non-array attributes, we will use this
	std::string value;
	if (pValue) // value not required
	{
		value = pValue->value();
	}

	switch(type)
	{
		case BOOLEAN_T:
			return Attribute(toBoolean(value), isKey);
		case UINT8_T:
			/*
			 * A note about NVM_UINT8 and NVM_SINT8 conversion. The way the toType template function
			 * works, if a char (NVM_INT8) is used as the template type, then it is converted to
			 * a char, not a number that fits into 8 bits. So converting to a short, then casting
			 * to a char is used.
			 */
			return Attribute((UINT8)toType<UINT16>(value), isKey);
		case UINT16_T:
			return Attribute(toType<UINT16>(value), isKey);
		case UINT32_T:
			return Attribute(toType<UINT32>(value), isKey);
		case UINT64_T:
			return Attribute(toType<UINT64>(value), isKey);
		case SINT8_T:
			return Attribute((SINT8)toType<SINT16>(value), isKey);
		case SINT16_T:
			return Attribute(toType<SINT16>(value), isKey);
		case SINT32_T:
			return Attribute(toType<SINT32>(value), isKey);
		case SINT64_T:
			return Attribute(toType<SINT64>(value), isKey);
		case REAL32_T:
			return Attribute(toType<REAL32>(value), isKey);
		case STR_T:
			return Attribute(value, isKey);
		case DATETIME_T:
			return Attribute(toType<UINT64>(value), DATETIME_SUBTYPE_DATETIME, isKey);
		case DATETIME_INTERVAL_T:
			return Attribute(toType<UINT64>(value), DATETIME_SUBTYPE_INTERVAL, isKey);
		case UINT16_LIST_T:
			return Attribute(createList<UINT16>(pValue), isKey);
		case UINT32_LIST_T:
			return Attribute(createList<UINT32>(pValue), isKey);
		case UINT64_LIST_T:
			return Attribute(createList<UINT64>(pValue), isKey);
		case STR_LIST_T:
			return Attribute(createList<STR>(pValue), isKey);
		case BOOLEAN_LIST_T:
			return Attribute(createList<BOOLEAN>(pValue), isKey);
		case UINT8_LIST_T:
			return Attribute(createList<UINT8>(pValue), isKey);
		case ENUM_T:
		case ENUM16_T:
			return Attribute();
	}
	return Attribute();
}

/*
 * Walk through VALUE properties and build them into a vector of type T objects.
 */
template <class T>
std::vector<T> wbem::framework::CimXml::createList(rapidxml::xml_node<> *pValueArray)
{
	std::vector<T> result;
	if (pValueArray)
	{
		for (rapidxml::xml_node<> *node = pValueArray->first_node(CX_VALUE.c_str());
				node; node = node->next_sibling())
		{
			std::string value;
			value = node->value();

			result.push_back(toType<T>(value));
		}
	}

	return result;
}

// **********************************************************************
// static members
// **********************************************************************

/*
 * Static function to setup the static map of nvm_data_type enum to string
 */
void wbem::framework::CimXml::setupMap()
{
	if(m_enumStringMap.size() == 0)
	{
		m_enumStringMap[BOOLEAN_T] = "boolean";
		m_enumStringMap[UINT8_T] = "uint8";
		m_enumStringMap[UINT16_T] = "uint16";
		m_enumStringMap[UINT32_T] = "uint32";
		m_enumStringMap[UINT64_T] = "uint64";
		m_enumStringMap[SINT8_T] = "int8";
		m_enumStringMap[SINT16_T] = "int16";
		m_enumStringMap[SINT32_T] = "int32";
		m_enumStringMap[SINT64_T] = "int64";
		m_enumStringMap[STR_T] = "string";
		m_enumStringMap[ENUM_T] = "enum";
		m_enumStringMap[ENUM16_T] = "enum";
		m_enumStringMap[DATETIME_T] = "datetime";
		m_enumStringMap[DATETIME_INTERVAL_T] = "datetime_interval";
	}
}

/*
 * Static map of nvm_data_type enum to string to help in serializing enums to string value
 */
std::map<enum wbem::framework::DataType, std::string> wbem::framework::CimXml::m_enumStringMap;

/*
 * Helper function to get the CIM type string version of an enum
 */
std::string wbem::framework::CimXml::enumToString(enum DataType type)
{
	setupMap();
	try
	{
		// Convert list types back to their base type to get their CIM type string
		switch (type)
		{
		case UINT8_LIST_T:
			type = UINT8_T;
			break;
		case UINT16_LIST_T:
			type = UINT16_T;
			break;
		case UINT32_LIST_T:
			type = UINT32_T;
			break;
		case UINT64_LIST_T:
			type = UINT64_T;
			break;
		case STR_LIST_T:
			type = STR_T;
			break;
		case BOOLEAN_LIST_T:
			type = BOOLEAN_T;
			break;
		default:
			break;
		}
		return m_enumStringMap.at(type);
	}
	catch (std::out_of_range &e)
	{
		throw ExceptionBadParameter("type");
	}
}

/*
 * Helper function to get the enum from the string version
 */
enum wbem::framework::DataType wbem::framework::CimXml::stringToEnum(std::string str)
{
	setupMap();
	bool found = false;
	enum DataType result = BOOLEAN_T;
	for (enumStringIterator_t iter = m_enumStringMap.begin();
			iter != m_enumStringMap.end() && !found;
			iter++)
	{
		if((*iter).second == str)
		{
			result = (*iter).first;
			found = true;
		}
	}
	if (!found)
	{
		throw ExceptionBadParameter("string type");
	}
	return result;
}

enum wbem::framework::DataType wbem::framework::CimXml::stringToAttributeType(std::string str,
		bool isArray)
{
	DataType type = stringToEnum(str);
	if (isArray)
	{
		switch (type)
		{
		case UINT8_T:
			type = UINT8_LIST_T;
			break;
		case UINT16_T:
			type = UINT16_LIST_T;
			break;
		case UINT32_T:
			type = UINT32_LIST_T;
			break;
		case UINT64_T:
			type = UINT64_LIST_T;
			break;
		case STR_T:
			type = STR_LIST_T;
			break;
		case BOOLEAN_T:
			type = BOOLEAN_LIST_T;
			break;
		default:
			throw ExceptionBadParameter("string array type");
		}
	}
	return type;
}

/*
 * Helper function to convert a string to a specific type
 */
template<typename T>
inline T wbem::framework::CimXml::toType(std::string value)
{
	T result;
	std::istringstream convert(value);
	if(value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) // is hex
	{
		convert >> std::hex >> result;
	}
	else if (StringUtil::stringCompareIgnoreCase(value, CX_TRUE) ||
			 StringUtil::stringCompareIgnoreCase(value, CX_FALSE))
	{
		convert >> std::boolalpha >> result;
	}
	else
	{
		convert >> result;
	}
	return result;
}

bool wbem::framework::CimXml::toBoolean(std::string value)
{
	bool result;

	if (StringUtil::stringCompareIgnoreCase(value, CX_TRUE))
	{
		result = true;
	}
	else if (StringUtil::stringCompareIgnoreCase(value, CX_FALSE))
	{
		result = false;
	}
	else
	{
		throw ExceptionBadParameter("boolean:value");
	}
	return result;
}
