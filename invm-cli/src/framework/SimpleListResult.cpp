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

#include "SimpleListResult.h"
#include <sstream>
#include <iostream>


cli::framework::SimpleListResult::SimpleListResult()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}

cli::framework::SimpleListResult::~SimpleListResult()
{
	// clean up the individual simple results
	for (simpleResults_t::iterator iter = m_results.begin();
				iter != m_results.end(); iter ++)
	{
		delete (*iter);
	}
	m_results.clear();
}

/*
 * Output the list of PropertyLists.
 */
std::string cli::framework::SimpleListResult::outputText() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::string results;
	for (simpleResults_t::const_iterator iter = m_results.begin();
			iter != m_results.end(); iter ++)
	{
		// if not the first one, add a new line
		if (iter != m_results.begin())
		{
			results += "\n";
		}
		results += (*iter)->outputText();
	}
	return results;
}

std::string cli::framework::SimpleListResult::outputXml() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	// start tag
	result << "<" << simpleListResultXmlTag << ">";
	if (m_results.size() > 0)
	{
		result << "\n";
	}

	// list of simple results
	for (simpleResults_t::const_iterator iter = m_results.begin();
				iter != m_results.end(); iter ++)
	{
		result << "\t" << (*iter)->outputXml();
	}
	// end tag
	result << "</" << simpleListResultXmlTag << ">\n";
	return result.str();
}

std::string cli::framework::SimpleListResult::outputJson() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	// start brace
	result << "{\n";
	
	// list of simple results
	for (simpleResults_t::const_iterator iter = m_results.begin();
		iter != m_results.end(); iter++)
	{
		result << "\t\"" << simpleListResultXmlTag << "\":";
		result << (*iter)->outputJson();

		//Check the next iterator to see if we have reached the end
		//If the end has not been reached, add a comma
		simpleResults_t::const_iterator temp_iter = iter + 1;
		if (temp_iter != m_results.end())
		{
			result << ",";
		}
		result << "\n";
	}
	
	// end brace
	result << "\n}\n";
	return result.str();
}

std::string cli::framework::SimpleListResult::outputEsxXml() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	result << ESX_BEGIN_ROOTTAG;
	result << ESX_BEGIN_STRINGLIST;
	for (simpleResults_t::const_iterator iter = m_results.begin(); iter != m_results.end(); iter ++)
	{
		result << ESX_BEGIN_STRING << (*iter)->output() << ESX_END_STRING;
	}
	result << ESX_END_LIST;
	result << ESX_END_ROOTTAG;
	return result.str();
}


/*
 * Add a new SimpleResult to the internal collection
 */
void cli::framework::SimpleListResult::insert(cli::framework::SimpleResult *pResult)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_results.push_back(pResult);
}

/*
 * Turn the string into a SimpleResult then add it to the internal collection
 */
void cli::framework::SimpleListResult::insert(const std::string& stringResult)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_results.push_back(new SimpleResult(stringResult));
}


/*
 * Retrieve the number of results
 */
int cli::framework::SimpleListResult::getCount()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_results.size();
}

/*
 * Retrieve the beginning of the resuls list.
 */
cli::framework::simpleResults_t::iterator cli::framework::SimpleListResult::resultsBegin()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_results.begin();
}

/*
 * Retrieve the end of the results list.
 */
cli::framework::simpleResults_t::iterator cli::framework::SimpleListResult::resultsEnd()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_results.end();
}
