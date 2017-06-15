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

#include "CliFrameworkTypes.h"

/*
 * common logger for the cli framework
 */
cli::framework::Logger cli::framework::logger;

extern std::string cli::framework::toLower(const std::string& value)
{
	std::string result = value;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

std::string cli::framework::trim(const std::string& value)
{
	std::string whitespace = " \t";
	size_t firstReal = value.find_first_not_of(whitespace);
	size_t lastReal = value.find_last_not_of(whitespace);

	std::string result(value, firstReal, lastReal - firstReal + 1);

	return result;
}

/*!
 * Utility method to tokenize a std::string
 */
std::vector<std::string> cli::framework::tokenizeString(const char *str, char delimeter)
{
	std::vector<std::string> result;
	do
	{
		const char *begin = str;
		while(*str != delimeter && *str)
			str++;
		std::string value(begin,str);
		value = trim(value);
		result.push_back(value);
	} while (0 != *str++);

	return result;
}


bool cli::framework::stringsIEqual(const std::string &str1, const std::string &str2)
{
 	Trace trace(__FILE__, __FUNCTION__, __LINE__);
        bool equal = false;
        if ((str1.size() == str2.size()) &&
                (toLower(str1) == toLower(str2)))
        {
                equal = true;
        }
        return equal;
}

bool cli::framework::stringIsNumeric(const std::string &str)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	bool isNumeric = true;

	//Used to index where to start in the given string.
	int startIndex = 0;

	//If the string is empty return false
	if (str.empty())
	{
		isNumeric = false;
	}
	else
	{
		bool decimalFound = false;
		bool hexCheck = false;

		//Check if the string is potentially hex
		if (str.size() >= 3 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		{
			startIndex = 2;
			hexCheck = true;
		}
		//Return false if we are not a hex string (checked above) and start with 0 and have 2 or more characters
		//If the second character is a period we could have a valid decimal number.
		else if (str.size() >= 2 && str[0] == '0' && str[1] != '.')
		{
			startIndex = str.length();
			isNumeric = false;
		}
		//Ignore the minus sign if it is the first character of the string.
		else if (str[0] == '-')
		{
			startIndex = 1;
		}

		for (unsigned int i = startIndex; i<str.length(); i++)
		{
			//First check if we are testing for hex number
			if (hexCheck)
			{
				if (!isxdigit(str[i]))
				{
					isNumeric = false;
					break;
				}
			}
			else
			{
				//ignore the first period. (i.e. decimal points)
				//Fail if multiple decimal points are found.
				if (str[i] == '.' && !decimalFound)
				{
					decimalFound = true;
					continue;
				}
				if (!isdigit(str[i]))
				{
					isNumeric = false;
					break;
				}
			}
		}
	}
	return isNumeric;
}


std::string cli::framework::tokenTypeToString(TokenType type)
{
	std::string result;
	switch (type)
	{
	case TOKENTYPE_VALUE:
		result = "Value";
		break;
	case TOKENTYPE_VERB:
		result = "Verb";
		break;
	case TOKENTYPE_OPTION:
		result = "Option";
		break;
	case TOKENTYPE_TARGET:
		result = "Target";
		break;
	case TOKENTYPE_PROPERTY:
		result = "Property";
		break;
	case TOKENTYPE_EQUAL:
		result = "Equal";
		break;
	case TOKENTYPE_COMMA:
		result = "Comma";
		break;
	case TOKENTYPE_UNKNOWN:
		result = "UNKNOWN";
		break;
	}

	return result;
}


/*!
 * Check if the specified parsed command contains the specified command part
 */
bool cli::framework::parsedCommandContains(const ParsedCommand& parsedCommand, const CommandSpecPart& cmdPart)
{
	bool foundIt = false;
	if (parsedCommand.options.find(cmdPart.name)
			!= parsedCommand.options.end())
	{
		foundIt = true;
	}
	return foundIt;
}


int cli::framework::getTokenIndex(const std::string &lexeme,
	const cli::framework::CommandSpecPartList &tokenList)
{
	int result = -1;
	for (size_t t = 0; t < tokenList.size() && result < 0; t++) // for each target or property
	{
		if (cli::framework::stringsIEqual(tokenList[t].name, lexeme))
		{
			result = (int)t;
		}
	}
	return result;
}
