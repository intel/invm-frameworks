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
 * This file contains a class to convert a CIM Object path to an ObjectPath
 */

#include <string>
#include "ObjectPathBuilder.h"

wbem::framework::ObjectPathBuilder::ObjectPathBuilder(const std::string &cimPath)
	: m_cimPath(cimPath)
{
	// convert back slashes to forward slashes
	size_t slashPos = m_cimPath.find("\\");
	while (slashPos != std::string::npos)
	{
		m_cimPath.replace(slashPos, 1, "/");
		slashPos = m_cimPath.find("\\", slashPos+1);
	}

	// remove quotes
	size_t quotePos = m_cimPath.find("\"");
	while (quotePos != std::string::npos)
	{
		m_cimPath.replace(quotePos, 1, ""); // trim all "'s
		quotePos = m_cimPath.find("\"", quotePos+1);
	}
	Parse();

}

bool wbem::framework::ObjectPathBuilder::Parse()
{
	size_t position = m_cimPath.find_first_of(':');
	if (position != std::string::npos) // namespace path found
	{
		ParseNamespacePath(m_cimPath.substr(0, position));
		ParseObjectPath(m_cimPath.substr(position + 1, m_cimPath.length() - position - 1));
	}
	else
	{
		ParseObjectPath(m_cimPath);
	}



	return true;
}

bool wbem::framework::ObjectPathBuilder::Build(wbem::framework::ObjectPath *pPath)
{
	bool result = false;

	if (!m_className.empty() && m_properties.size() > 0)
	{
		wbem::framework::attributes_t keys;

		for(std::map<std::string, std::string>::iterator prop = m_properties.begin();
			prop!= m_properties.end(); prop++)
		{
			wbem::framework::Attribute attribute(prop->second, true);
			keys[prop->first] = attribute;
		}
		pPath->setObjectPath(m_host, m_namespace, m_className, keys);
		result = true;
	}

	return result;
}

/*
 * Parse the namespace portion of the CIM object path in the format:
 * [\\machine name\]namespace
 */
void wbem::framework::ObjectPathBuilder::ParseNamespacePath(std::string value)
{
	if (!value.empty())
	{
		// machine name is optional
		if (value.compare(0, 2, "//") == 0)
		{
			// split the machine name and namespace
			// NOTE that machine name is not valid without namespace
			size_t position = value.find('/', 2);
			if (position != std::string::npos && position != value.length())
			{
				m_host = value.substr(2, position-2);
				m_namespace = value.substr(position+1);
			}
		}
		// no machine name, just namespace
		else
		{
			m_namespace = value;
		}
	}
}


/*
 * Parse the object path portion of the CIM object path in the format:
 * ClassName.PropertyName=Value[,PropertyName=Value...]
 */
void wbem::framework::ObjectPathBuilder::ParseObjectPath(std::string value)
{
	if (!value.empty())
	{
		// get the class name
		size_t dotPos = value.find('.');
		// if it doesn't exist, just bail, because the object path isn't valid without at
		// least a class name and one key/value pair
		if (dotPos != std::string::npos)
		{
			m_className = value.substr(0, dotPos);
			std::string properties = value.substr(dotPos+1);

			// parse the properties
			size_t startPos = 0;
			do
			{
				// find the next key/value pair
				size_t commaPos = properties.find(',', startPos);
				std::string property;
				if (commaPos != std::string::npos)
				{
					property = properties.substr(startPos, commaPos-startPos);
					startPos = commaPos+1;
				}
				else
				{
					property = properties.substr(startPos);
					startPos = std::string::npos;
				}

				// split into key=value
				size_t equalPos = property.find('=');
				if (equalPos != std::string::npos)
				{
					m_properties[property.substr(0, equalPos)] =
							property.substr(equalPos+1);
				}
			}
			while (startPos != std::string::npos);
		}
	}
}
