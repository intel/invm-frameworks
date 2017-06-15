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
 * This file contains a class to represent a WQL conditional statement.
 */

#include <string/s_str.h>
#include <string/x_str.h>
#include <stdlib.h>
#include <cerrno>
#include <sstream>
#include <logger/logging.h>
#include "ExceptionInvalidWqlQuery.h"
#include "WqlConditional.h"
#include "WqlQuery.h"

namespace wbem
{
namespace framework
{

/*
 * Constructor
 */
WqlConditional::WqlConditional(const std::vector<std::string>& tokens) throw (Exception)
{
	// rebuild a string from the tokens
	std::stringstream stream;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::string token = tokens[i];
		if (i > 0)
		{
			stream << " ";
		}
		stream << token;
	}
	m_str = stream.str();
	initFromTokens(tokens);
}

WqlConditional::WqlConditional(const std::string& str) throw (Exception) :
		m_str(str)
{
	char tempStr[str.size() + 1];
	s_strcpy(tempStr, str.c_str(), sizeof (tempStr));

	// set of conditional tokens
	std::vector<std::string> tokens;

	// Tokenize the input string
	bool openQuotes = false; // track open quotes
	char quoteType = '\0'; // track open quotes
	const char *delim = " \t\n"; // delimiters - whitespace
	char *pNext = tempStr; // where to start search for next token
	char *pCh = x_strtok(&pNext, delim);
	while (pCh != NULL)
	{
		std::string token = pCh;
		std::vector<std::string> substrings = dissectConditionalToken(token, openQuotes, quoteType);
		tokens.insert(tokens.end(), substrings.begin(), substrings.end());

		// Keep going until we find a non-empty substring
		do
		{
			pCh = x_strtok(&pNext, delim);
		}
		while (pCh && std::string(pCh).empty());
	}

	initFromTokens(tokens);
}

/*
 * Destructor
 */
WqlConditional::~WqlConditional()
{
}

/*
 * Copy constructor
 */
WqlConditional::WqlConditional(const WqlConditional& cond)
{
	m_conditions = cond.m_conditions;
}

/*
 * Assignment operator
 */
WqlConditional& WqlConditional::operator=(const WqlConditional& cond)
{
	m_conditions = cond.m_conditions;

	return *this;
}

/*
 * Parse the conditional from tokens
 */
void WqlConditional::initFromTokens(const std::vector<std::string> &tokens) throw (Exception)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);

	// Parsing states
	enum ConditionalParseState
	{
		STATE_NAME,
		STATE_OP,
		STATE_VALUE,
		STATE_DONE
	};
	enum ConditionalParseState state = STATE_NAME;
	int depth = 0; // keep track of nested parentheses - for format validation
	bool openQuotes = false; // keep track of open quote marks
	std::string quoteType = ""; // what type of quote mark is open?
	struct WqlComparisonClause temp;
	std::string tempValue = "";

	// Build the conditional list
	for (std::vector<std::string>::const_iterator iter = tokens.begin(); iter != tokens.end(); iter++)
	{
		const std::string &token = *iter;
		switch (state)
		{
		case STATE_NAME:
			// We only expect opening parens at the beginnings of clauses
			if (token == "(")
			{
				depth++;
			}
			else
			{
				if (WqlQuery::isValidCimName(token))
				{
					temp.attributeName = token;
					state = STATE_OP;
				}
				else
				{
					COMMON_LOG_ERROR_F("expected valid attribute name, got: %s", token.c_str());
					throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADATTR, token);
				}
			}
			break;
		case STATE_OP:
			if (token == WQL_EQ)
			{
				temp.op = OP_EQ;
			}
			else if (token == WQL_GT)
			{
				temp.op = OP_GT;
			}
			else if (token == WQL_LT)
			{
				temp.op = OP_LT;
			}
			else if (token == WQL_GE)
			{
				temp.op = OP_GE;
			}
			else if (token == WQL_LE)
			{
				temp.op = OP_LE;
			}
			else if ((token == WQL_NE1) || (token == WQL_NE2))
			{
				temp.op = OP_NE;
			}
			else
			{
				COMMON_LOG_ERROR_F("expected operator token, got: %s", token.c_str());
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADOPERATOR, token);
			}
			state = STATE_VALUE;
			break;
		case STATE_VALUE:
			if (openQuotes)
			{
				if (token == quoteType) // closing a string
				{
					// see if it's a datetime type
					try
					{
						temp.value = Attribute(tempValue, DATETIME_SUBTYPE_DATETIME, false);
					}
					catch (Exception &) // wasn't a datetime string
					{
						temp.value = Attribute(tempValue, false);
					}

					// clear string tracking values
					tempValue = "";
					openQuotes = false;
					quoteType = "";

					state = STATE_DONE;
				}
				else
				{
					// string in progress
					if (!tempValue.empty())
					{
						tempValue += " ";
					}
					tempValue += token;
				}
			}
			else if ((token == "\"") || (token == "'")) // opening quote marks
			{
				openQuotes = true;
				quoteType = token;
			}
			else
			{
				// figure out the type
				// try a numeric value
				UINT64 intValue = 0;
				char *pEnd = NULL;
				errno = 0;
				intValue = strtoull(token.c_str(), &pEnd, 0);
				if ((errno == 0) && pEnd && (*pEnd == '\0')) // it was a valid 64-bit number
				{
					temp.value = Attribute(intValue, false);
				}
				// try boolean
				else if (s_strncmpi(token.c_str(), WQL_TRUE.c_str(), WQL_TRUE.size() + 1) == 0)
				{
					temp.value = Attribute(true, false);
				}
				else if (s_strncmpi(token.c_str(), WQL_FALSE.c_str(), WQL_FALSE.size() + 1) == 0)
				{
					temp.value = Attribute(false, false);
				}
				// can't recognize it
				else
				{
					COMMON_LOG_ERROR_F("couldn't decipher the type of token: %s", token.c_str());
					throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADVALUE, token);
				}

				state = STATE_DONE;
			}

			// we finished harvesting the value
			if (state == STATE_DONE)
			{
				m_conditions.push_back(temp);
			}
			break;
		case STATE_DONE: // reached the end of a valid statement
			// might have a closing parenthesis in this state
			if (token == ")")
			{
				depth--;
			}
			else if (s_strncmpi(token.c_str(), WQL_AND.c_str(), WQL_AND.size() + 1) == 0)
				// could connect to another clause with AND
			{
				state = STATE_NAME;
			}
			else
			{
				COMMON_LOG_ERROR_F("expected AND, got: %s", token.c_str());
				throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADOPERATOR, token);
			}
			break;
		default:
			COMMON_LOG_ERROR_F("Invalid parsing state: %d", state);
			throw Exception("Internal error while parsing conditional");
		}
	}

	// Validate that the input string was valid/complete
	if (openQuotes)
	{
		COMMON_LOG_ERROR_F("unmatched quotes: %s", quoteType.c_str());
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_UNMATCHEDQUOTES);
	}

	if (depth != 0)
	{
		COMMON_LOG_ERROR_F("unmatched parentheses: %d", depth);
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_UNMATCHEDPARENS);
	}

	if (state != STATE_DONE)
	{
		COMMON_LOG_ERROR_F("ended in the wrong state: %d", state);
		throw ExceptionInvalidWqlQuery(ExceptionInvalidWqlQuery::REASON_BADLYFORMED);
	}
}

/*
 * Break apart whitespace-delimited token into all meaningful conditional clause tokens
 */
std::vector<std::string> WqlConditional::dissectConditionalToken(const std::string& token, bool &openQuotes, char &quoteType)
{
	Trace logging(__FILE__, __FUNCTION__, __LINE__);
	std::vector<std::string> newTokens;

	if (!token.empty())
	{
		// walk through string and build up substrings
		std::string substring;
		size_t step = 1;
		bool prevSpecial = false;
		for (size_t i = 0; i < token.size(); i += step)
		{
			bool foundSubstrStart = false;
			char current = token[i];
			char next = ((i + 1) == token.size()) ?
					'\0' :
					token[i + 1];

			// ignore most characters inside quotes
			if (openQuotes)
			{
				if (prevSpecial) // beginning of the string inside quotes
				{
					foundSubstrStart = true;
					prevSpecial = false;
				}

				step = 1;

				if (current == '\\') // escape next char
				{
					i++; // exclude the escape and save the next char without inspection
				}
				else if (current == quoteType) // closing quote mark
				{
					openQuotes = false;
					foundSubstrStart = true;
					prevSpecial = true;
				}
			}
			else // not inside quotes - process special characters
			{
				switch (current)
				{
				case '"':
				case '\'':
					openQuotes = true;
					quoteType = current;
					// quotes are a single character token
				case ')':
				case '(':
				case '=':
					// single-character tokens
					foundSubstrStart = true;
					prevSpecial = true;
					step = 1;
					break;
				case '<': // could be < or <= or <>
					if (next == '>') // Not equal (<>)
					{
						foundSubstrStart = true;
						prevSpecial = true;
						step = 2;
						break;
					}
					// otherwise drop through to same checks as GT/GE
				case '>': // could be > or >=
					foundSubstrStart = true;
					prevSpecial = true;
					if (next == '=') // >= or <=
					{
						step = 2;
					}
					else // > or <
					{
						step = 1;
					}
					break;
				case '!': // might be !=
					if (next == '=')
					{
						foundSubstrStart = true;
						prevSpecial = true;
						step = 2;
					}
					break;
				default: // not a special character
					step = 1;
					if (prevSpecial) // but previous char was special
					{
						foundSubstrStart = true;
						prevSpecial = false;
					}
					break;
				}
			}

			// started a new substring, store away the old one
			if ((i > 0) && foundSubstrStart)
			{
				newTokens.push_back(substring);
				substring.clear();
			}

			substring += token.substr(i, step);
		}

		// final substring
		newTokens.push_back(substring);
	}

	return newTokens;
}

} /* namespace framework */
} /* namespace wbem */


