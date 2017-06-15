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
 * This file contains the base class for Intel CIM instances.
 */

#include <sstream>

#include "StringUtil.h"
#include <string/s_str.h>
#include "Instance.h"
#include "CimXml.h"
#include "ExceptionBadParameter.h"

/*
 * Default constructor --creates an empty attribute map.
 */
wbem::framework::Instance::Instance()
	: m_Class(""), m_Host(""), m_Namespace(""), m_InstanceAttributes()
{}

/*
 * Constructor that initializes attributes from an object path
 */
wbem::framework::Instance::Instance(ObjectPath& path)
	: m_Class(path.getClass()), m_Host(path.getHost()),
	m_Namespace(path.getNamespace()), m_InstanceAttributes(path.getKeys())
{}

wbem::framework::Instance::Instance(std::string cimXml)
{
	try
	{
		CimXml xml(cimXml);
		m_Class = xml.getClass();

		attributes_t properties = xml.getProperties();

		for (attributes_t::iterator property = properties.begin(); property != properties.end(); property++)
		{
			setAttribute((const std::string&)(*property).first, (*property).second);
		}
	}
	catch (std::exception &)
	{
		throw ExceptionBadParameter("CIM-XML");
	}
}

/*
 * Retrieve an attribute value from this instance.
 */
int wbem::framework::Instance::getAttribute(const std::string& key, wbem::framework::Attribute& value) const
{
	wbem::framework::attributes_t::const_iterator iter = m_InstanceAttributes.find(key);
	if (iter != m_InstanceAttributes.end())
	{
		value = iter->second;
		return wbem::framework::SUCCESS;
	}
	return wbem::framework::FAIL;
}

/*
 * Retrieve an attribute value from this instance.
 */
int wbem::framework::Instance::getAttributeI(std::string &key, wbem::framework::Attribute& value) const
{
	// case insensitive search
	for (attributes_t::const_iterator iter = m_InstanceAttributes.begin();
			iter != m_InstanceAttributes.end(); ++iter)
	{
		// case insensitive compare (strings must be same length to be equal)
		if (key.length() == iter->first.length() &&
				s_strncmpi(iter->first.c_str(), key.c_str(), iter->first.length()) == 0)
		{
			key = iter->first; // update the key to the proper case
			value = iter->second;
			return wbem::framework::SUCCESS;
		}
	}

	return wbem::framework::FAIL;
}


/*
 * Get the object path for this instance
 */
wbem::framework::ObjectPath wbem::framework::Instance::getObjectPath() const
{
	// get all the keys
	framework::attributes_t keys;
	for (attributes_t::const_iterator iter = m_InstanceAttributes.begin();
		iter != m_InstanceAttributes.end(); iter++)
	{
		if (iter->second.isKey())
		{
			keys.insert(std::pair<std::string,
				framework::Attribute>(iter->first, iter->second));
		}
	}

	// create the object path
	return framework::ObjectPath(m_Host, m_Namespace, m_Class, keys);
}

int wbem::framework::Instance::setAttribute(const std::string &key, const framework::Attribute &value)
{
	m_InstanceAttributes.erase(key);
	m_InstanceAttributes.insert(std::pair<std::string, framework::Attribute>(key, value));
	return wbem::framework::SUCCESS;
}

/*
 * check attribute list before deciding to load an attribute.
 */
int wbem::framework::Instance::setAttribute(const std::string &key,
		const framework::Attribute &value,
		const framework::attribute_names_t &attributes)
{
	wbem::framework::Attribute newAttr;
	getAttribute(key, newAttr);

	// if the attribute list is empty or the attribute is specified, set it
	bool found = false;
	found = attributes.size() == 0;
	for (unsigned int i = 0; (i < attributes.size() && !found); i++)
	{
		if (StringUtil::stringCompareIgnoreCase(attributes[i], key))
		{
			found = true;
		}
	}
	if (found)
	{
		setAttribute(key, value);
	}

	return wbem::framework::SUCCESS;
}

/*
 * Allows the caller to iterate over the attributes.
 */
wbem::framework::attributes_t::const_iterator wbem::framework::Instance::attributesBegin() const
{
	return m_InstanceAttributes.begin();
}

/*
 * Allows the caller to get a pointer to the last item in the attribute list.  Mostly this is for
 * loop control.
 */
wbem::framework::attributes_t::const_iterator wbem::framework::Instance::attributesEnd() const
{
	return m_InstanceAttributes.end();
}

size_t wbem::framework::Instance::attributesCount() const
{
	return m_InstanceAttributes.size();
}

/*
 * Get the XML string for this instance
 * See below for An example encoded in CIM-XML (whitespace for readability only).
 *
 */
std::string wbem::framework::Instance::getCimXml() const
{
	std::stringstream xml;

	std::string value_xml_open = "<" + CX_VALUE + ">";
	std::string value_xml_close = "</" + CX_VALUE + ">";

	xml << "<" << CX_INSTANCE << " " + CX_CLASSNAME + "=\"" << m_Class << "\">";
	for(attributes_t::const_iterator iter = m_InstanceAttributes.begin();
			iter != m_InstanceAttributes.end();
			iter ++)
	{
		std::stringstream value;
		Attribute attribute = (*iter).second;

		std::string propertyStr = CX_PROPERTY;
		if (attribute.isArray())
		{
			propertyStr = CX_PROPERTYARRAY;
			value << "<" << CX_VALUEARRAY << ">";
		}
		value << attribute.asStr(value_xml_open, value_xml_close, "");
		if (attribute.isArray())
		{
			 value << "</" << CX_VALUEARRAY << ">";
		}

		xml << "<" << propertyStr << " " <<
				CX_NAME << "=\"" << (*iter).first << "\" " <<
				CX_TYPE << "=\"" << CimXml::enumToString(attribute.getType()) << "\"" <<
				">" <<
				value.str() <<
				"</" << propertyStr <<  ">"
				;
	}

	xml << "</" + CX_INSTANCE + ">";
	return xml.str();
}

/*
 * Get the Instance class.
 */
std::string wbem::framework::Instance::getClass() const
{
	return m_Class;
}

/*
 * Get the Instance host.
 */
std::string wbem::framework::Instance::getHost() const
{
	return m_Host;
}

/*
 * Get the Instance namespace.
 */
std::string wbem::framework::Instance::getNamespace() const
{
	return m_Namespace;
}

/*
 * Inequality operator
 */
bool wbem::framework::Instance::operator != (const wbem::framework::Instance& rhs) const
{
	return (*this == rhs ? false : true);
}

/*
 * Equality operator
 */
bool wbem::framework::Instance::operator == (const wbem::framework::Instance& rhs) const
{
	bool result = true;
	wbem::framework::attributes_t::const_iterator rhsIter = rhs.attributesBegin();
	for (; rhsIter != rhs.attributesEnd(); rhsIter++)
	{
		std::string rhsKey = (*rhsIter).first;
		wbem::framework::Attribute rhsAttr = (*rhsIter).second;

		wbem::framework::Attribute lhsAttr;
		if ((this->getAttribute(rhsKey, lhsAttr)) != wbem::framework::SUCCESS)
		{
			result = false;
			break;
		}

		if (lhsAttr != rhsAttr)
		{
			result = false;
			break;
		}
	}
	return result;
}

