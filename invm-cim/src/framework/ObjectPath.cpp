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
 * This file defines an Intel CIM object path. Object paths are essentially a
 * multi-part key that uniquely identifies a given CIM instance.
 */

#include <logger/logging.h>
#include "ExceptionBadParameter.h"
#include "ObjectPath.h"


wbem::framework::ObjectPath::ObjectPath()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	m_Host = "";
	m_Namespace = "";
	m_Class = "";
}

wbem::framework::ObjectPath::ObjectPath(std::string host, std::string wbem_namespace,
	std::string wbem_class, attributes_t keys)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	m_Host = host;
	m_Namespace = wbem_namespace;
	m_Class = wbem_class;
	m_Keys = keys;
}

void wbem::framework::ObjectPath::setObjectPath(std::string host, std::string wbem_namespace,
	std::string wbem_class, attributes_t keys)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	m_Host = host;
	m_Namespace = wbem_namespace;
	m_Class = wbem_class;
	m_Keys = keys;
}

wbem::framework::ObjectPath::~ObjectPath()
{
}

const wbem::framework::Attribute& wbem::framework::ObjectPath::getKeyValue(const std::string &key) const
	throw (wbem::framework::Exception)
{
	COMMON_LOG_DEBUG_F("Searching for key: %s", key.c_str());
	attributes_t::const_iterator item = m_Keys.find(key);
	if (item != m_Keys.end())
	{
		return item->second;
	}
	COMMON_LOG_ERROR_F("Key not found: %s", key.c_str());
	throw ExceptionBadParameter(key.c_str());
}

void wbem::framework::ObjectPath::checkKey(std::string const &key,
		std::string const &expected) const
throw (Exception)
{
	if (getKeyValue(key).asStr() != expected)
	{
		throw framework::ExceptionBadParameter(key.c_str());
	}
}

const wbem::framework::attributes_t& wbem::framework::ObjectPath::getKeys() const
{
	return m_Keys;
}

const std::string& wbem::framework::ObjectPath::getHost() const
{
	return m_Host;
}

const std::string& wbem::framework::ObjectPath::getNamespace() const
{
	return m_Namespace;
}

const std::string& wbem::framework::ObjectPath::getClass() const
{
	return m_Class;
}

std::string wbem::framework::ObjectPath::asString(bool ignoreHostName) const
{
	std::string path;
	bool first = true;

	if(ignoreHostName)
	{
		path = "//./" + m_Namespace + ":" + m_Class + ".";
	}
	else
	{
		path = "//" + m_Host + "/" + m_Namespace + ":" + m_Class + ".";
	}

	for (attributes_t::const_iterator iterator = m_Keys.begin();
			iterator != m_Keys.end(); iterator++)
	{
		if (iterator->second.isKey())
		{
			// separate keys with commas
			if (first)
			{
				first = false;
			}
			else
			{
				path += ",";
			}

			// put the key name in
			path += iterator->first + "=";

			// add the attribute
			path += "\"" + iterator->second.asStr() + "\"";
		}
	}
	return path;
}

/*
 * Empty object path
 */
bool wbem::framework::ObjectPath::empty()
{
	return (m_Host.empty() && m_Namespace.empty() && m_Class.empty() && m_Keys.empty());
}

bool wbem::framework::ObjectPath::operator==(const ObjectPath &rhs)
{
	return (rhs.asString() == this->asString());
}

bool wbem::framework::ObjectPath::operator!=(const ObjectPath &rhs)
{
	return (rhs.asString() != this->asString());
}

