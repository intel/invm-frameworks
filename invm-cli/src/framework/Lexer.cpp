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

#include "Lexer.h"
#include "CliFrameworkTypes.h"
#include <iostream>

cli::framework::Lexer::Lexer(
		const std::vector<std::string>& verbs,
		const std::vector<std::string>& targets,
		const std::vector<std::string>& options,
		const std::vector<std::string>& properties)
: m_verbs(verbs), m_targets(targets), m_options(options), m_properties(properties)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	// make sure targets and options start with '-'
	StringList::iterator iter;

	for (iter = m_targets.begin(); iter != m_targets.end(); iter++)
	{
		if ((*iter)[0] != '-')
		{
			(*iter) = "-" + (*iter);
		}
	}
	for (iter = m_options.begin(); iter != m_options.end(); iter++)
	{
		if ((*iter)[0] != '-')
		{
			(*iter) = "-" + (*iter);
		}
	}
}

/*
 * Parse identifies each string in a list of strings using the possible verbs, targets, options,
 * and properties. If a string isn't matched then it is assumed that is is the value of a target, option,
 * or property
 */
std::vector<cli::framework::Token> cli::framework::Lexer::tokenize(const int argCount, const char *args[])
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	StringList stringList;

	for (int i = 0; i < argCount; i++)
	{
		stringList.push_back(args[i]);
	}

	return tokenize(stringList);

}

std::vector<cli::framework::Token> cli::framework::Lexer::tokenize(const StringList& args)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	// Preprocessing ...
	// split any "property=value" string into "property = value"
	// keep comman (,) by iteself (example: "-display 'attribute1,attribute2'" => -display 'attribute1' ',' 'attribute2')
	std::vector<std::string> cleanTokens;
	for(size_t i = 0; i < args.size(); i ++)
	{
		std::string tokenStr = std::string(args[i]);
		int equal = tokenStr.find('=');
		if (equal > 0)
		{
			std::string property = tokenStr.substr(0, equal);
			std::string propertyValue = tokenStr.substr(equal + 1, tokenStr.length());
			cleanTokens.push_back(property);
			cleanTokens.push_back("=");
			cleanTokens.push_back(propertyValue);
		}
		else
		{
			cleanTokens.push_back(tokenStr);
		}
	}

	std::vector<std::string> cleanTokens2;
	for(size_t i = 0; i < cleanTokens.size(); i ++)
	{
		int equal;
		std::string tokenStr = std::string(cleanTokens[i]);
		while ((equal = tokenStr.find(',')) >= 0)
		{
			std::string first = tokenStr.substr(0, equal);
			std::string second = tokenStr.substr(equal + 1, tokenStr.length());
			if (!first.empty())
			{
				cleanTokens2.push_back(first);
			}
			cleanTokens2.push_back(",");
			tokenStr = second;
		}
		if (!tokenStr.empty())
		{
			cleanTokens2.push_back(tokenStr);
		}
	}

	cleanTokens = cleanTokens2;

	std::vector<cli::framework::Token> result;
	for(unsigned int i = 0; i < cleanTokens.size(); i ++)
	{
		bool done = false;
		TokenType type = TOKENTYPE_VALUE; // default is a VALUE
		std::string tokenStr = std::string(cleanTokens[i]);

		std::string tokenStrLower = toLower(tokenStr);

		// if token starts with a '-' then it cannot be a value for a target or option (but maybe
		// a property)
		if (tokenStrLower.size() > 0 && tokenStrLower[0] == '-')
		{
			type = TOKENTYPE_UNKNOWN;
		}

		if (tokenStrLower == "=")
		{
			type = TOKENTYPE_EQUAL;
			done = true;
		}
		if (tokenStrLower == ",")
		{
			type = TOKENTYPE_COMMA;
			done = true;
		}

		// Is it a verb
		for (int j = 0; j < (int)m_verbs.size() && !done; j++)
		{
			if (tokenStrLower == toLower(m_verbs[j]))
			{
				logger << "Found verb: " << tokenStrLower << std::endl;
				tokenStr = m_verbs[j];
				type = TOKENTYPE_VERB;
				done = true;
			}
		}

		// Is it a target
		for (int j = 0; j < (int)m_targets.size() && !done; j++)
		{
			if (tokenStrLower == toLower(m_targets[j]))
			{
				logger << "Found target: " << tokenStrLower << std::endl;
				tokenStr = m_targets[j];
				type = TOKENTYPE_TARGET;
				done = true;
			}
		}

		// Is it an option
		for ( int j = 0; j < (int)m_options.size() && !done; j++)
		{
			if (tokenStrLower == toLower(m_options[j]))
			{
				logger << "Found option: " << tokenStrLower << std::endl;
				tokenStr = m_options[j];
				type = TOKENTYPE_OPTION;
				done = true;
			}
		}

		// Is it a property
		for ( unsigned int j = 0; j < m_properties.size() && !done; j ++ )
		{
			if (tokenStrLower == toLower(m_properties[j]))
			{
				logger << "Found property: " << tokenStrLower << std::endl;
				tokenStr = m_properties[j];
				type = TOKENTYPE_PROPERTY;
				done = true;
			}
		}

		result.push_back((Token){tokenStr, type});
	}
	return result;
}
