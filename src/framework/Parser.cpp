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

#include "Parser.h"

#include "ParseErrorResult.h"
#include "NoInputErrorResult.h"
#include "DuplicateTokenErrorResult.h"
#include "SyntaxErrorMissingValueResult.h"
#include "SyntaxErrorBadValueResult.h"
#include "CliFrameworkTypes.h"
#include "SyntaxErrorUnexpectedValueResult.h"
#include <string>
#include <sstream>

cli::framework::Parser::Parser()  {}

/*
 * Entry point for the parsing.  It starts off making sure that the first token is a verb.
 */
cli::framework::SyntaxErrorResult *cli::framework::Parser::parse(const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	SyntaxErrorResult *m_result = NULL;

	initialize();

	if (tokens.size() == 0) // must have at least 1 token
	{
		m_result = new NoInputErrorResult();
	}
	else if (tokens[0].tokenType != TOKENTYPE_VERB) // first token must be a verb
	{
		m_result = new ParseErrorResult((Token)tokens[0]);
	}
	else
	{
		// start the state machine. Begins with verb.
		m_result = stateVerb(0, tokens);
	}

	resolveOutputOption(m_result, tokens);
	return m_result;
}

void cli::framework::Parser::initialize()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_parsedCommand.verb = "";
	m_parsedCommand.options.clear();
	m_parsedCommand.targets.clear();
	m_parsedCommand.properties.clear();
}

cli::framework::SyntaxErrorResult *cli::framework::Parser::stateVerb(int currentToken,
	cli::framework::TokenList const &tokens)
{
	SyntaxErrorResult *pResult = NULL;
	m_parsedCommand.verb = tokens[0].lexeme; // save verb into parsed command

	if (tokens.size() > 1) // if there's another token after the verb ...
	{
		switch (tokens[1].tokenType)
		{
			case TOKENTYPE_TARGET: // accept a target after a verb
				pResult = stateTarget(1, tokens);
				break;
			case TOKENTYPE_OPTION: // accept an option after a verb
				pResult = stateOption(1, tokens);
				break;
			case TOKENTYPE_PROPERTY: // accept a property after a verb
				pResult = stateProperty(1, tokens);
				break;
			default: // anything else is a parse error
				pResult = createParseError(tokens[1]);
				break;
		}
	}

	return pResult;
}

/*
 * After transitioning to the target state, valid transitions are to a value, another target, or
 * a property.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateTarget(int currentToken, const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	m_currentTarget = tokens[currentToken].lexeme;

	if (m_parsedCommand.targets.find(m_currentTarget) != m_parsedCommand.targets.end())
	{
		pResult = createDuplicateTokenError(tokens[currentToken]);
	}
	else
	{
		m_parsedCommand.targets[m_currentTarget] = "";
		currentToken ++; // look at next token
		if ((int)tokens.size() > currentToken)
		{
			Token token = tokens[currentToken];
			switch (token.tokenType)
			{
				case TOKENTYPE_VALUE:
					pResult = stateTargetValue(currentToken, tokens);
					break;
				case TOKENTYPE_TARGET:
					pResult = stateTarget(currentToken, tokens);
					break;
				case TOKENTYPE_PROPERTY:
					pResult = stateTargetValueOrProperty(currentToken, tokens);
					break;
				case TOKENTYPE_OPTION:
				case TOKENTYPE_VERB:
				case TOKENTYPE_EQUAL:
				default:
					pResult = createParseError(tokens[currentToken]);
					break;
			}
		}
	}

	return pResult;
}

cli::framework::SyntaxErrorResult *cli::framework::Parser::createDuplicateTokenError(
	cli::framework::Token const &token)
{
	m_ignorePotentialCommands = true;
	return new cli::framework::DuplicateTokenErrorResult(token);
}

/*
 * After transitioning to the target value state, valid transitions are to a another target, or
 * a property.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateTargetValue(int currentToken, const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	// because we can only get there from the target state, we know that m_currentTarget will
	// be set correctly
	m_parsedCommand.targets[m_currentTarget] += tokens[currentToken].lexeme;

	currentToken ++; // look at next token
	if ((int)tokens.size() > currentToken)
	{
		Token token = tokens[currentToken];
		switch (token.tokenType)
		{
		case TOKENTYPE_TARGET:
			pResult = stateTarget(currentToken, tokens);
			break;
		case TOKENTYPE_PROPERTY:
			pResult = stateProperty(currentToken, tokens);
			break;
		case TOKENTYPE_COMMA:
			pResult = stateComma(currentToken, tokens, TOKENTYPE_TARGET);
			break;
		case TOKENTYPE_OPTION:
		case TOKENTYPE_EQUAL:
		case TOKENTYPE_VALUE:
		case TOKENTYPE_VERB:
		default:
			pResult = createParseError(tokens[currentToken]);
			break;
		}
	}
	return pResult;
}

/*
 * The token type is a property, but the property could really be a target value. Try to make
 * a decision on which it is based on the next token, otherwise save for later when we have
 * more context
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateTargetValueOrProperty(
	int currentToken, const TokenList &tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	if ((int)tokens.size() > currentToken + 1)
	{
		int nextToken = currentToken + 1;
		Token token = tokens[nextToken];
		switch (token.tokenType)
		{
			case TOKENTYPE_TARGET:
				// token is target value so revisit it as a target value
				pResult = stateTargetValue(currentToken, tokens);
				break;
			case TOKENTYPE_PROPERTY:
			{
				setUnknownPropertyTo(tokens[currentToken].lexeme, m_currentTarget, PARTTYPE_TARGET);
				pResult = stateProperty(nextToken, tokens);
				break;
			}
			case TOKENTYPE_COMMA:
				// token is target value so revisit it as a target value
				pResult = stateTargetValue(currentToken, tokens);
				break;
			case TOKENTYPE_EQUAL:
				// token is property so revisit it as a property
				pResult = stateProperty(currentToken, tokens);
				break;
			case TOKENTYPE_OPTION:
			case TOKENTYPE_VALUE:
			case TOKENTYPE_VERB:
			default:
				pResult = createParseError(tokens[nextToken]);
				break;
		}
	}
	else
	{
		// end of command, so don't know. Will have to wait for more context
		setUnknownPropertyTo(tokens[currentToken].lexeme, m_currentTarget, PARTTYPE_TARGET);
	}
	return pResult;
}

/*
 * After transitioning to the option state, valid transitions are to a value, option, or target.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateOption(int currentToken, const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	m_currentOption = tokens[currentToken].lexeme;

	if (m_parsedCommand.options.find(m_currentOption) != m_parsedCommand.options.end())
	{
		pResult = createDuplicateTokenError(tokens[currentToken]);
	}
	else
	{
		m_parsedCommand.options[m_currentOption] = "";
		currentToken ++; // look at next token
		if ((int)tokens.size() > currentToken)
		{
			Token token = tokens[currentToken];
			switch (token.tokenType)
			{
			case TOKENTYPE_VALUE:
				token.tokenType = TOKENTYPE_VALUE;
				pResult = stateOptionValue(currentToken, tokens);
				break;
			case TOKENTYPE_PROPERTY:
				pResult = stateOptionValueOrProperty(currentToken, tokens);
				break;
			case TOKENTYPE_OPTION:
				pResult = stateOption(currentToken, tokens);
				break;
			case TOKENTYPE_TARGET:
				pResult = stateTarget(currentToken, tokens);
				break;
			case TOKENTYPE_VERB:
			case TOKENTYPE_EQUAL:
			default:
				pResult = createParseError(tokens[currentToken]);
				break;
			}
		}
	}
	return pResult;
}

cli::framework::SyntaxErrorResult *cli::framework::Parser::stateOptionValueOrProperty(
	int currentToken, cli::framework::TokenList const &tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	if ((int)tokens.size() > currentToken + 1)
	{
		int nextToken = currentToken + 1;
		Token token = tokens[nextToken];
		switch (token.tokenType)
		{
			case TOKENTYPE_TARGET:
				// token is option value so revisit it as an option value
				pResult = stateOptionValue(currentToken, tokens);
				break;
			case TOKENTYPE_PROPERTY:
			{
				setUnknownPropertyTo(tokens[currentToken].lexeme, m_currentOption, PARTTYPE_OPTION);
				pResult = stateProperty(nextToken, tokens);
				break;
			}
			case TOKENTYPE_COMMA:
				// token is option value so revisit it as an option value
				pResult = stateOptionValue(currentToken, tokens);
				break;
			case TOKENTYPE_EQUAL:
				// token is property so revisit it as a property
				pResult = stateProperty(currentToken, tokens);
				break;
			case TOKENTYPE_OPTION:
				// token is option value so revisit it as an option value
				pResult = stateOptionValue(currentToken, tokens);
				break;
			case TOKENTYPE_VALUE:
			case TOKENTYPE_VERB:
			default:
				pResult = createParseError(tokens[nextToken]);
				break;
		}
	}
	else
	{
		// end of command, so don't know. Will have to wait for more context
		setUnknownPropertyTo(tokens[currentToken].lexeme, m_currentOption, PARTTYPE_OPTION);
	}
	return pResult;
}

void cli::framework::Parser::setUnknownPropertyTo(const std::string &lexeme,
		const std::string &token,
		const cli::framework::CommandSpecPartType &type)
{
	m_unknownProperty.tokenKey = token;
	m_unknownProperty.type = type;
	m_unknownProperty.lexeme = lexeme;
}


/*
 * After transitioning to the option value state, valid transitions are to a target or another option.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateOptionValue(int currentToken,
		const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;
	// because we can only get there from the option state, we know that m_currentOption will
	// be set correctly
	m_parsedCommand.options[m_currentOption] += tokens[currentToken].lexeme;

	currentToken ++; // look at next token
	if ((int)tokens.size() > currentToken)
	{
		Token token = tokens[currentToken];
		switch (token.tokenType)
		{
		case TOKENTYPE_OPTION:
			pResult = stateOption(currentToken, tokens);
			break;
		case TOKENTYPE_TARGET:
			pResult = stateTarget(currentToken, tokens);
			break;
		case TOKENTYPE_COMMA:
			pResult = stateComma(currentToken, tokens, TOKENTYPE_OPTION);
			break;
		case TOKENTYPE_PROPERTY:
			pResult = stateProperty(currentToken, tokens);
			break;
		case TOKENTYPE_VALUE:
		case TOKENTYPE_VERB:
		case TOKENTYPE_EQUAL:
		default:
			pResult = createParseError(tokens[currentToken]);
			break;
		}
	}
	return pResult;
}
/*
 * After transitioning to the comma state, valid transitions are only to another value. This
 * cannot be an end state.
 *
 * Note: Right now only option values can have commas.  If this needs to change, this state
 * could receive a parameter indicating which type of value to transition to and which value
 * to add the ',' to.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateComma(int currentToken,
		const TokenList& tokens, TokenType tokenType)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;


	currentToken ++; // look at next token
	if ((int)tokens.size() > currentToken)
	{
		Token token = tokens[currentToken];
		switch (token.tokenType)
		{
		case TOKENTYPE_VALUE:
		case TOKENTYPE_PROPERTY:	// accept property names as token values also ie:FwLogLevel
			if (tokenType == TOKENTYPE_OPTION)
			{
				m_parsedCommand.options[m_currentOption] += ",";
				pResult = stateOptionValue(currentToken, tokens);
			}
			else if (tokenType == TOKENTYPE_TARGET)
			{
				m_parsedCommand.targets[m_currentTarget] += ",";
				pResult = stateTargetValue(currentToken, tokens);
			}
			else
			{
				pResult = createParseError(tokens[currentToken]);
			}
			break;
		default:
			pResult = createParseError(tokens[currentToken]);
			break;
		}
	}
	return pResult;
}

/*
 * After transitioning to the property state, valid transitions are to a another
 * property or the equal (=) state.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateProperty(int currentToken,
		const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	m_currentProperty = tokens[currentToken].lexeme;

	if (m_parsedCommand.properties.find(m_currentProperty) != m_parsedCommand.properties.end())
	{
		pResult = createDuplicateTokenError(tokens[currentToken]);
	}
	else
	{
		m_parsedCommand.properties[m_currentProperty] = "";

		currentToken ++; // look at next token
		if ((int)tokens.size() > currentToken)
		{
			Token token = tokens[currentToken];
			switch (token.tokenType)
			{
				case TOKENTYPE_EQUAL:
					pResult = stateEquals(currentToken, tokens);
					break;
				case TOKENTYPE_PROPERTY:
					pResult = stateProperty(currentToken, tokens);
					break;
				case TOKENTYPE_VALUE:
				case TOKENTYPE_VERB:
				case TOKENTYPE_OPTION:
				case TOKENTYPE_TARGET:
				default:
					pResult = createParseError(tokens[currentToken]);
					break;
			}
		}
	}

	return pResult;
}

/*
 * After transitioning to the property equal state, the valid transitions are to any but another '='.
 * Verb, option, target, and property lexemes will be recognized as the property value, not the
 * tokenized token type. This way property values can be the same as the property,verb, ...
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::stateEquals(int currentToken,
		const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;
	if ((int)tokens.size() > currentToken + 1)
	{
		currentToken ++; // look at next token
		Token token = tokens[currentToken];
		switch (token.tokenType)
		{
			// if it could be a property, it could also be a value, figure out which
			case TOKENTYPE_PROPERTY:
				pResult = statePropertyOrPropertyValue(currentToken, tokens);
				break;
			// Anything can be a property value ... i.e. FriendlyName=dimm, FriendlyName=FriendlyName ...
			case TOKENTYPE_VALUE:
			case TOKENTYPE_VERB:
			case TOKENTYPE_OPTION:
			case TOKENTYPE_TARGET:
			case TOKENTYPE_UNKNOWN: // almost anything can be a value of property
				pResult = statePropertyValue(currentToken, tokens);
				break;
			case TOKENTYPE_EQUAL: // can't have two equals next to each other
			default:
				pResult = createParseError(tokens[currentToken]);
				break;
		}
	}
	return pResult;
}

/*
 * After transitioning to the property_or_property value state, the valid transitions are to either property
 * or property value state.  Need to check future tokens to determine which it is.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::statePropertyOrPropertyValue(int currentToken,
		const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;
	int nextTokenOffset = currentToken + 1;
	if (tokens.size() > (size_t)nextTokenOffset)
	{
		// Check the next token if it exists. If the next token is an '=', this must be a property.
		// Otherwise, this has to be a value.
		Token checkForEqualToken = tokens[nextTokenOffset];
		if (checkForEqualToken.tokenType == TOKENTYPE_EQUAL)
		{
			// must be the next property since it's followed by '='
			pResult = stateProperty(currentToken, tokens);
		}
		else
		{
			// must be the property value
			pResult = statePropertyValue(currentToken, tokens);
		}
	}
	else // if there are no more properties, this must be a value
	{
		// there are not enough tokens left to have another property and equal sign so
		// this must be a value
		pResult = stateProperty(currentToken, tokens);
	}
	return pResult;
}

/*
 * After transitioning to the property value state, a valid transition is to a property.
 */
cli::framework::SyntaxErrorResult* cli::framework::Parser::statePropertyValue(int currentToken,
		const TokenList& tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult* pResult = NULL;

	m_parsedCommand.properties[m_currentProperty] = tokens[currentToken].lexeme;

	currentToken ++; // look at next token
	if ((int)tokens.size() > currentToken)
	{
		Token token = tokens[currentToken];
		switch (token.tokenType)
		{
		case TOKENTYPE_PROPERTY:
			pResult = stateProperty(currentToken, tokens);
			break;
		case TOKENTYPE_OPTION:
		case TOKENTYPE_TARGET:
		case TOKENTYPE_VALUE:
		case TOKENTYPE_VERB:
		case TOKENTYPE_EQUAL:
		default:
			pResult = createParseError(tokens[currentToken]);
			break;
		}
	}
	return pResult;
}

cli::framework::ParsedCommand cli::framework::Parser::getParsedCommand()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	return m_parsedCommand;
}

/*
 * Retrieve the specified property value from the parsed command
 */
std::string cli::framework::Parser::getPropertyValue(const framework::ParsedCommand& parsedCommand,
		const std::string &propertyName,bool *p_exists)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	return getValue(parsedCommand.properties, propertyName, p_exists);
}

/*
 * Retrieve the specified option value from the parsed command
 */
std::string cli::framework::Parser::getOptionValue(const framework::ParsedCommand& parsedCommand,
		const std::string& optionName, bool *p_exists)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	return getValue(parsedCommand.options, optionName, p_exists);
}

/*
 * Retrieve the specified target value from the parsed command
 */
std::string cli::framework::Parser::getTargetValue(const framework::ParsedCommand& parsedCommand,
		const std::string& targetName, bool *p_exists)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	return getValue(parsedCommand.targets, targetName, p_exists);
}

/*
 * Retrieve a comma separated list of target values from the parsed command
 */
std::vector<std::string> cli::framework::Parser::getTargetValues(
		const framework::ParsedCommand& parsedCommand,
		const std::string& targetName, bool *p_exists)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	std::vector<std::string> result;
	std::string values = getValue(parsedCommand.targets, targetName, p_exists);
	if (!values.empty())
	{
		result = tokenizeString(values.c_str(), ',');
	}
	return result;
}

std::string cli::framework::Parser::getValue(const StringMap& map,
		const std::string& keyName, bool* p_keyExists)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	std::string value;
	framework::StringMap::const_iterator iPath = map.find(keyName);
	if (iPath != map.end())
	{
		value = iPath->second;
		if (p_keyExists != NULL)
		{
			*p_keyExists = true;
		}
	}
	else if (p_keyExists != NULL)
	{
		*p_keyExists = false;
	}

	return value;
}

/*
 * If user requested output as XML then make sure output is displayed as xml, even if there's an
 * error with the parsing.
 */
void cli::framework::Parser::resolveOutputOption(SyntaxErrorResult *pSyntaxError,
	const TokenList  &tokens)
{
	if (pSyntaxError && m_parsedCommand.options[OPTION_OUTPUT.name].empty())
	{
		for (size_t i = 0; i < tokens.size(); ++i)
		{
			if (tokens[i].tokenType == TOKENTYPE_OPTION && (tokens[i].lexeme == OPTION_OUTPUT.abr ||
				tokens[i].lexeme == OPTION_OUTPUT.name))
			{
				if ((i + 1) < tokens.size())
				{
					if (stringsIEqual(tokens[i + 1].lexeme, OPTION_OUTPUT_XML))
					{
						m_parsedCommand.options[OPTION_OUTPUT.name] = OPTION_OUTPUT_XML;
					}
				}
			}
		}
	}
}

cli::framework::SyntaxErrorResult *cli::framework::Parser::createParseError(Token const &token)
{
	return new ParseErrorResult(token);
}

bool cli::framework::Parser::includePotentialCommandsIsRequested()
{
	return !m_ignorePotentialCommands;
}
