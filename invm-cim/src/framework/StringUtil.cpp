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
 * This file contains string helper functions.
 */


#include <sstream>
#include "StringUtil.h"

bool wbem::framework::StringUtil::stringCompareIgnoreCase(std::string str1, std::string str2)
{
	if (str1.size() != str2.size())
	{
		return false;
	}
	for (unsigned int i = 0; i < str1.length(); i++)
	{
		if (std::tolower(str1[i]) != std::tolower(str2[i]))
		{
			return false;
		}
	}
	return true;
}


COMMON_UINT64 wbem::framework::StringUtil::stringToUint64(const std::string& str)
{
	COMMON_UINT64 result;

	std::stringstream ss(str);
	ss >> result;

	return result;
}

COMMON_INT64 wbem::framework::StringUtil::stringToInt64(const std::string& str)
{
	COMMON_INT64 result;

	std::stringstream ss(str);
	ss >> result;

	return result;
}


std::string wbem::framework::StringUtil::removeStrings(
		const std::string& fkValue, std::vector<std::string> strList)
{
	// avoid log tracing in this function, it is called a lot.
	std::string ret = fkValue;
	for (std::vector<std::string>::const_iterator itr = strList.begin(); itr != strList.end(); itr++)
	{
		size_t pos = ret.find(*itr);
		if (pos!= std::string::npos)
		{
			ret.replace(pos, (*itr).length(), "");
		}
	}
	return ret;
}

std::string wbem::framework::StringUtil::removeString(
		const std::string& fkValue, const std::string& str)
{
	std::vector<std::string> filter;
	filter.push_back(str);
	return removeStrings(fkValue, filter);

}