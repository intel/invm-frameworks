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
 * This file contains a class to process WQL strings into values
 * meaningful for the Intel WBEM library.
 */

#include <string/s_str.h>
#include <string/x_str.h>
#include <logger/logging.h>
#include "ExceptionInvalidWqlQuery.h"
#include "ExceptionNoMemory.h"
#include "WqlQuery.h"

namespace wbem
{
namespace framework
{

WqlQuery::WqlQuery(const std::string &query) throw (Exception) :
		m_query(query),
		m_className(""),
		m_attributes(),
		m_pConditional(NULL)
{
	initFromQuery(query);
}


WqlQuery::WqlQuery(const WqlQuery& query)
{
	m_query = query.m_query;
	m_className = query.m_className;
	m_attributes = query.m_attributes;
	if (query.m_pConditional)
	{
		m_pConditional = new WqlConditional(*(query.m_pConditional));
	}
	else
	{
		m_pConditional = NULL;
	}
}

WqlQuery::~WqlQuery()
{
	if (m_pConditional)
	{
		delete m_pConditional;
	}
}

WqlQuery& WqlQuery::operator=(const WqlQuery& query)
{
	// avoid memory errors that could result from self-assignment
	if (&query != this)
	{
		m_query = query.m_query;
		m_className = query.m_className;
		m_attributes = query.m_attributes;

		// Memory housekeeping
		if (m_pConditional)
		{
			delete m_pConditional;
			m_pConditional = NULL;
		}
		// Need to create a new copy of the conditional
		if (query.m_pConditional)
		{
			m_pConditional = new WqlConditional(*(query.m_pConditional));
		}
	}

	return *this;
}

void WqlQuery::initFromQuery(const std::string &query) throw (Exception)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);

	// values
	std::string className;
	std::vector<std::string> selectTokens;
	std::vector<std::string> conditionalTokens;

	// parse the query string
	parse(query, className, selectTokens, conditionalTokens);

	// process the parsed values
	processClassName(className);
	processSelectAttributes(selectTokens);
	processConditional(conditionalTokens);
}

/*
 * This method does some basic grammatical checking on the query while
 * populating the outputs, but more in-depth parsing of outputs occurs
 * elsewhere.
 */
void WqlQuery::parse(const std::string &query,
		std::string &className,
		std::vector<std::string> &selectTokens,
		std::vector<std::string> &conditionalTokens) const
	throw (Exception)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);

	// reject empty strings
	if (query.empty())
	{
		COMMON_LOG_ERROR("empty WQL string");
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
	}

	// create a copy we can mangle
	size_t querySize = query.size() + 1;
	char tempStr[querySize];
	s_strcpy(tempStr, query.c_str(), sizeof (tempStr));
	s_strtrim(tempStr, sizeof (tempStr)); // trim off whitespace

	// remember the sizes of keyword strings
	// we'll use them for string comparisons
	static size_t selectSize = WQL_SELECT.size() + 1;
	static size_t fromSize = WQL_FROM.size() + 1;
	static size_t whereSize = WQL_WHERE.size() + 1;

	// walk through the string
	// expect the form:
	// 		SELECT <attr> FROM <classname> [WHERE <conditional>]
	enum ParseState
	{
		PARSE_SELECT,
		PARSE_SELECT_VALUE,
		PARSE_FROM,
		PARSE_FROM_VALUE,
		PARSE_WHERE,
		PARSE_CONDITIONAL,
		PARSE_DONE
	};
	enum ParseState state = PARSE_SELECT;

	// Tokenize and parse out arguments
	bool openQuotes = false; // track open quotes
	char quoteType = '\0'; // track open quotes
	const char *delim = " \t\n"; // delimiters - whitespace
	char *pNext = tempStr; // where to start search for next token
	char *pCh = x_strtok(&pNext, delim);
	while (pCh != NULL)
	{
		std::string token = pCh;
		switch (state)
		{
		case PARSE_SELECT:
			if (s_strncmpi(pCh, WQL_SELECT.c_str(),
					selectSize) == 0)
			{
				state = PARSE_SELECT_VALUE;
			}
			else
			{
				COMMON_LOG_ERROR_F("Expected '%s', first token was '%s'",
						WQL_SELECT.c_str(),
						pCh);
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
			}
			break;
		case PARSE_SELECT_VALUE:
			// make sure it's not a keyword
			if (!isWqlKeyword(token))
			{
				selectTokens.push_back(token);
				// If there's a comma at the end, we expect more values
				if (*(token.end() - 1) != ',')
				{
					state = PARSE_FROM;
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("Expected an attribute, got keyword '%s'",
						pCh);
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
			}
			break;
		case PARSE_FROM:
			if (s_strncmpi(pCh, WQL_FROM.c_str(), fromSize) == 0)
			{
				state = PARSE_FROM_VALUE;
			}
			else
			{
				COMMON_LOG_ERROR_F("Expected keyword '%s', token was '%s'",
						WQL_FROM.c_str(),
						pCh);
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
			}
			break;
		case PARSE_FROM_VALUE:
			// make sure it's not a keyword
			if (!isWqlKeyword(token))
			{
				className = token;
				state = PARSE_WHERE;
			}
			else
			{
				COMMON_LOG_ERROR_F("Expected a value, got keyword '%s'",
						pCh);
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
			}
			break;
		case PARSE_WHERE:
			if (s_strncmpi(pCh, WQL_WHERE.c_str(), whereSize) == 0)
			{
				state = PARSE_CONDITIONAL;
			}
			else
			{
				COMMON_LOG_ERROR_F("Expected keyword '%s', token was '%s'",
						WQL_WHERE.c_str(),
						pCh);
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
			}
			break;
		case PARSE_CONDITIONAL:
		case PARSE_DONE:
			// We need at least one value
			if (!isWqlKeyword(token) || openQuotes)
			{
				std::vector<std::string> substrings =
						WqlConditional::dissectConditionalToken(token, openQuotes, quoteType);
				conditionalTokens.insert(conditionalTokens.end(), substrings.begin(), substrings.end());
				state = PARSE_DONE;
			}
			else
			{
				COMMON_LOG_ERROR_F("Expected a value, got keyword '%s'",
						pCh);
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
			}
			break;
		default:
			COMMON_LOG_ERROR_F("invalid WQL parse state: %u", state);
			throw Exception("Internal error parsing WQL string");
		}

		// get the next token that isn't an empty string
		do
		{
			pCh = x_strtok(&pNext, delim);
		}
		while (pCh && std::string(pCh).empty());
	}

	// Two possible end states
	// ending on PARSE_WHERE implies no conditional, which is valid
	if ((state != PARSE_WHERE) && (state != PARSE_DONE))
	{
		COMMON_LOG_ERROR("ran out of tokens before we finished");
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
	}
}

void WqlQuery::processClassName(const std::string& name) throw (Exception)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);

	if (name.empty())
	{
		COMMON_LOG_ERROR("class name in query is an empty string");
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADCLASSNAME, name);
	}

	if (isValidCimClassName(name))
	{
		m_className = name;
	}
	else
	{
		COMMON_LOG_ERROR_F("class name '%s' is invalid", name.c_str());
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADCLASSNAME, name);
	}
}

void WqlQuery::processSelectAttributes(const std::vector<std::string>& attrTokens)
		throw (Exception)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);

	if (attrTokens.empty())
	{
		COMMON_LOG_ERROR("no attributes requested in query");
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADATTR);
	}

	// wildcard means all attributes - so leave the internal list empty
	if (!((attrTokens.size() == 1u) && (attrTokens[0] == WQL_SELECT_ALL)))
	{
		// if we don't have the wildcard, process a list of attribute names
		for (size_t i = 0; i < attrTokens.size(); i++)
		{
			// split on commas if necessary
			size_t length = attrTokens[i].size();
			char tempStr[length + 1];
			s_strcpy(tempStr, attrTokens[i].c_str(), sizeof (tempStr));
			// if the token ends with a comma, zero it out
			// to simplify processing
			if (tempStr[length - 1] == ',')
			{
				tempStr[length - 1] = '\0';
			}

			// tokenize on commas
			char *pNext = tempStr;
			const char *delim = ",";
			char *pToken = x_strtok(&pNext, delim);
			while (pToken)
			{
				std::string attr = pToken;

				// Looks valid
				if (isValidCimName(attr))
				{
					m_attributes.push_back(attr);
				}
				else
				{
					COMMON_LOG_ERROR_F("attribute name '%s' is invalid", attr.c_str());
					throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADATTR,
							attr);
				}

				// get the next token
				pToken = x_strtok(&pNext, delim);
			}
		}
	}
}

void WqlQuery::processConditional(const std::vector<std::string>& conditionalTokens)
		throw (Exception)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);

	// it's okay to have no conditional
	if (!conditionalTokens.empty())
	{
		try
		{
			m_pConditional = new WqlConditional(conditionalTokens);
			if (!m_pConditional)
			{
				throw ExceptionNoMemory(__FILE__, __FUNCTION__,
						"couldn't allocate NvmWqlConditional");
			}
		}
		catch (Exception &)
		{
			if (m_pConditional)
			{
				delete m_pConditional;
				m_pConditional = NULL;
			}

			throw;
		}
	}
}

bool WqlQuery::isWqlKeyword(const std::string& value)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);
	bool result = false;

	// case insensitive comparison
	static size_t selectSize = WQL_SELECT.size() + 1;
	static size_t fromSize = WQL_FROM.size() + 1;
	static size_t whereSize = WQL_WHERE.size() + 1;
	if ((s_strncmpi(value.c_str(), WQL_SELECT.c_str(), selectSize) == 0) ||
		(s_strncmpi(value.c_str(), WQL_FROM.c_str(), fromSize) == 0) ||
		(s_strncmpi(value.c_str(), WQL_WHERE.c_str(), whereSize) == 0))
	{
		result = true;
	}

	return result;
}

bool WqlQuery::isValidCimClassName(const std::string& value)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);
	bool result = true;

	if (value.empty())
	{
		result = false;
	}
	else
	{
		// CIM class name in format:
		//		<identifier>_<name>
		bool identifierFound = false;
		bool nameFound = false;
		for (size_t i = 0; i < value.size(); i++)
		{
			char c = value[i];
			if (identifierFound)
			{
				nameFound = true;

				// name is alphanumeric and underscores
				if (!isalnum(c) && (c != '_'))
				{
					result = false;
					break;
				}
			}
			else // working on the identifier
			{
				// identifier must be alphabetic
				if (c == '_')
				{
					identifierFound = true;
				}
				else if (!isalpha(c))
				{
					result = false;
					break;
				}
			}
		}

		// if one of the pieces is missing, it's not valid
		if (!identifierFound || !nameFound)
		{
			result = false;
		}
	}

	return result;
}

/*
 * CIM naming convention includes alphanumeric characters and underscores
 */
bool WqlQuery::isValidCimName(const std::string& value)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);
	bool result = true;

	if (value.empty())
	{
		result = false;
	}
	else
	{
		for (size_t i = 0; i < value.size(); i++)
		{
			char c = value[i];
			if (!isalnum(c) && (c != '_'))
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

} /* namespace framework */
} /* namespace wbem */
