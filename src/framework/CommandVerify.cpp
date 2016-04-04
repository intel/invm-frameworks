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

#include "CommandVerify.h"
#include "Trace.h"
#include "SyntaxErrorUnexpectedValueResult.h"
#include "SyntaxErrorMissingValueResult.h"
#include "SyntaxErrorBadValueResult.h"


cli::framework::SyntaxErrorResult *cli::framework::CommandVerify::verify(
	cli::framework::ParsedCommand parsedCommand, cli::framework::CommandSpec commandSpec)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult *pResult = NULL;

	pResult = verifyCommonOptions(parsedCommand);
	if (!pResult)
	{
		pResult = verifyRequiredValues(parsedCommand, commandSpec);
	}

	return pResult;

}


cli::framework::SyntaxErrorResult *cli::framework::CommandVerify::verifyCommonOptions(
	const ParsedCommand &parsedCommand)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	SyntaxErrorResult *pResult = NULL;

	// can't include options -all and -display
	if (parsedCommand.options.find(OPTION_ALL.name) != parsedCommand.options.end()
		&& parsedCommand.options.find(OPTION_DISPLAY.name) != parsedCommand.options.end())
	{
		pResult = new SyntaxErrorResult(TR("Options 'all' and 'display' cannot be used together."));
	}
		// option -all shouldn't have a value
	else if (parsedCommand.options.find(OPTION_ALL.name) != parsedCommand.options.end()
		&& parsedCommand.options.at(OPTION_ALL.name) != "")
	{
		pResult = new SyntaxErrorUnexpectedValueResult(framework::TOKENTYPE_OPTION, "all");
	}
		// -display needs to have a value
	else if (parsedCommand.options.find(OPTION_DISPLAY.name) != parsedCommand.options.end()
		&& parsedCommand.options.at(OPTION_DISPLAY.name) == "")
	{
		pResult = new SyntaxErrorMissingValueResult(framework::TOKENTYPE_OPTION, "display");
	}
		// -help shouldn't have a value
	else if (parsedCommand.options.find(OPTION_HELP.name) != parsedCommand.options.end()
		&& parsedCommand.options.at(OPTION_HELP.name) != "")
	{
		pResult = new SyntaxErrorUnexpectedValueResult(framework::TOKENTYPE_OPTION, "help");
	}
		// -output value must match valid values
	else if (pResult == NULL && parsedCommand.options.find(OPTION_OUTPUT.name) != parsedCommand.options.end())
	{
		std::vector<std::string> validOutputTypes;
		validOutputTypes.push_back(OPTION_OUTPUT_TEXT);
		validOutputTypes.push_back(OPTION_OUTPUT_XML);

		// the error message is put together this way instead of a more dynamic approach because
		// it makes translation easier.
		std::string outputErrorMsg = TR("Option 'output' expects 'text' or 'nvmxml'.");
#ifdef CLI_OUTPUT_JSON
		validOutputTypes.push_back(OPTION_OUTPUT_JSON);
		outputErrorMsg = TR("Option 'output' expects 'text', 'json' or 'nvmxml'.");
#endif
#ifdef CLI_OUTPUT_ESX
		outputErrorMsg = TR("Option 'output' expects 'text', 'esx', 'esxtable', or 'nvmxml'.");
		validOutputTypes.push_back(OPTION_OUTPUT_ESX);
		validOutputTypes.push_back(OPTION_OUTPUT_ESXTABLE);
#endif
#if defined(CLI_OUTPUT_ESX) && defined(CLI_OUTPUT_JSON)
		outputErrorMsg = TR("Option 'output' expects 'text', 'json', 'nvmxml', 'esx', or 'esxtable'.");
#endif

		std::string outputValue = parsedCommand.options.at(OUTPUT_OPTION);
		bool validType = false;
		for (std::vector<std::string>::const_iterator iter = validOutputTypes.begin();
			iter != validOutputTypes.end(); iter++)
		{
			if (stringsIEqual(outputValue, *iter))
			{
				validType = true;
				break;
			}
		}

		if (!validType)
		{
			pResult = new SyntaxErrorResult(outputErrorMsg);
		}
	}
	// -units needs to have a value
	else if (parsedCommand.options.find(OPTION_UNITS.name) != parsedCommand.options.end()
			&& parsedCommand.options.at(OPTION_UNITS.name) == "")
	{
		pResult = new SyntaxErrorMissingValueResult(framework::TOKENTYPE_OPTION, "units");
	}

	return pResult;
}


cli::framework::SyntaxErrorResult *cli::framework::CommandVerify::verifyRequiredValues(
	const ParsedCommand &m_parsedCommand,
	const CommandSpec &spec)
{
	cli::framework::SyntaxErrorResult *pResult = NULL;

	pResult = verifyRequiredValues(spec.options, m_parsedCommand.options, TOKENTYPE_OPTION);
	if (pResult == NULL)
	{
		pResult = verifyRequiredValues(spec.targets, m_parsedCommand.targets, TOKENTYPE_TARGET);
	}
	if (pResult == NULL)
	{
		pResult = verifyRequiredValues(spec.properties, m_parsedCommand.properties, TOKENTYPE_PROPERTY);
	}
	return pResult;
}

cli::framework::SyntaxErrorResult *cli::framework::CommandVerify::verifyRequiredValues(
	const CommandSpecPartList& specList, const StringMap& parsedList,
	enum TokenType type)
{
	cli::framework::SyntaxErrorResult *pResult = NULL;
	for (size_t i = 0; i < specList.size() && pResult == NULL; i ++)
	{
		const CommandSpecPart &part = specList[i];
		bool parsed = parsedList.find(part.name) != parsedList.end();
		if (parsed)
		{
			std::string value = parsedList.at(part.name);
			if (part.valueRequired && value.empty())
			{
				pResult = new SyntaxErrorMissingValueResult(type, part.name);
			}
			else if (part.noValueAccepted && !value.empty())
			{
				pResult = new SyntaxErrorUnexpectedValueResult(type, part.name);
			}
		}
	}
	return pResult;
}

