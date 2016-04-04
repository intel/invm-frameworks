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

#include "ObjectListResult.h"
#include <sstream>
#include <iostream>
#include <iomanip>


cli::framework::ObjectListResult::ObjectListResult() : m_root("Root")
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}

unsigned int cli::framework::ObjectListResult::getColumnWidth(const std::string key) const
{
	unsigned int width = key.length();
	for (propertyObjects_t::const_iterator iter = m_objects.begin();
		iter != m_objects.end(); iter ++)
	{
		PropertyListResult item = iter->second;
		unsigned int itemWidth = item[key].length();
		if (itemWidth > width)
		{
			width = itemWidth;
		}
	}
	return width;
}

/*
 * Output the list of PropertyLists in table format
 */
std::string cli::framework::ObjectListResult::outputTextTable() const
{

	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	propertyObjects_t::const_iterator iter = m_objects.begin();
	std::vector<unsigned int> columnWidths;
	for (; iter != m_objects.end(); iter ++)
	{
		PropertyListResult item = iter->second;
		properties_t::const_iterator propIter = item.propertiesBegin();
		// print the headers
		if (iter == m_objects.begin())
		{
			for (; propIter != item.propertiesEnd(); propIter ++)
			{
				int columnWidth = getColumnWidth(propIter->key);
				columnWidths.push_back(columnWidth);
				result << std::left << std::setw(columnWidth) << propIter->key << " ";
			}
			result << std::endl;
			propIter = item.propertiesBegin();
		}
		// print the values
		int i = 0;
		for (; propIter != item.propertiesEnd(); propIter ++)
		{
			result << std::left << std::setw(columnWidths[i]) << propIter->value->output() << " ";
			i++;
		}
		result << std::endl;
	}

	if (result.str().empty())
	{
		result << NoResultsStr;
	}
	return result.str();
}

/*
 * Output the list of PropertyLists.
 */
std::string cli::framework::ObjectListResult::outputText() const
{

	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::string result("");
	propertyObjects_t::const_iterator iter = m_objects.begin();
	for (; iter != m_objects.end(); iter ++)
	{
		std::string key = iter->first;
		std::string value = iter->second.output();
		result += value + "\n";
	}

	if (result.empty())
	{
		result = NoResultsStr;
	}
	return result;
}

std::string cli::framework::ObjectListResult::outputXml() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;
	// start tag
	result << "<" << m_root << ObjectListResultListXmlTag << ">";
	if (m_objects.size() > 0)
	{
		result << "\n";
	}

	// object list
	propertyObjects_t::const_iterator iter = m_objects.begin();
	for (; iter != m_objects.end(); iter++)
	{
		result << iter->second.outputXml();
	}
	// end tag
	result << "</" << m_root << ObjectListResultListXmlTag << ">\n";
	return result.str();
}

std::string cli::framework::ObjectListResult::outputJson() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;
	// start brace
	result << "{\n";
	
	// object list
	propertyObjects_t::const_iterator iter = m_objects.begin();
	for (; iter != m_objects.end(); iter++)
	{
		//Use the key value as the Object Name.
		result << "\t\"" << iter->first << "\":\n";
		result << iter->second.outputJson();

		//Check the next iterator to see if we have reached the end
		//If the end has not been reached, add a comma
		propertyObjects_t::const_iterator temp_iter = iter + 1;
		if (temp_iter != m_objects.end())
		{
			result << ",";
		}
		result << "\n";
	}
	// end brace
	result << "}\n";
	return result.str();
}

std::string cli::framework::ObjectListResult::outputEsxXml() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;

	// if there's only one then return it as a single property list result
	if (getCount() == 1)
	{
		result << m_objects.begin()->second.outputEsxXml();
	}
	else
	{
		result << ESX_BEGIN_ROOTTAG << ESX_BEGIN_STRUCTLIST;
		for (propertyObjects_t::const_iterator iter = m_objects.begin();
				iter != m_objects.end(); iter++)
		{
			result << stringFromArgList(ESX_BEGIN_STRUCT.c_str(), m_root.c_str());



			for(properties_t::const_iterator propIter = iter->second.propertiesBegin();
					propIter != iter->second.propertiesEnd(); propIter++)
			{
				result << stringFromArgList(ESX_BEGIN_FIELD.c_str(), propIter->key.c_str());
				result << ESX_BEGIN_STRING << propIter->value->output() << ESX_END_STRING;
				result << ESX_END_FIELD;
			}
			result << ESX_END_STRUCT;
		}
		result << ESX_END_LIST << ESX_END_ROOTTAG;
	}
	return result.str();
}

std::string cli::framework::ObjectListResult::outputEsxXmlTable() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;

	// if there's only one then return it as a single property list result
	if (getCount() == 1)
	{
		result << m_objects.begin()->second.outputEsxXmlTable();
	}
	else
	{
		result << outputEsxXml();
	}
	return result.str();
}

/*
 * Add a new key/value pair to the internal collection
 */
void cli::framework::ObjectListResult::insert(const std::string& key, PropertyListResult& value)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	value.setName(m_root);
	m_objects.push_back(std::pair<std::string, PropertyListResult>(key, value));
}

void cli::framework::ObjectListResult::setRoot(const std::string& root)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_root = root;
}

/*!
 * Retrieve the number of objects
 */
int cli::framework::ObjectListResult::getCount() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_objects.size();
}

/*
 * Retrieve the beginning of the property objects list.
 */
cli::framework::propertyObjects_t::iterator cli::framework::ObjectListResult::objectsBegin()
{
	return m_objects.begin();
}

/*
 * Retrieve the end of the property objects list.
 */
cli::framework::propertyObjects_t::iterator cli::framework::ObjectListResult::objectsEnd()
{
	return m_objects.end();
}
