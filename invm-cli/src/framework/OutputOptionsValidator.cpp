/*
 * Copyright (c) 2017, Intel Corporation
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

#include <algorithm>
#include "OutputOptionsValidator.h"

cli::framework::OutputOptionsValidator::OutputOptionsValidator(const ParsedCommand &parsedCommand)
{
	StringMap::const_iterator options_iter = parsedCommand.options.find(OUTPUT_OPTION);

	m_validOutputFormats = getValidOutputFormats();

	m_validOutputOptions = getValidOutputOptions();

	if (options_iter != parsedCommand.options.end() && !options_iter->second.empty())
	{
		m_optionValues = tokenizeString(toLower(options_iter->second).c_str(), ',');
	}
}

bool cli::framework::OutputOptionsValidator::isFormatOption(const std::string &option)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	bool isFormat = false;

	if (std::find(m_validOutputFormats.begin(), m_validOutputFormats.end(), option) !=
				m_validOutputFormats.end())
	{
		isFormat = true;
	}

	return isFormat;
}

bool cli::framework::OutputOptionsValidator::isValidOption(const std::string &option)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	bool isOption = false;

	if (std::find(m_validOutputOptions.begin(), m_validOutputOptions.end(), option) !=
				m_validOutputOptions.end())
	{
		isOption = true;
	}

	return isOption;
}

bool cli::framework::OutputOptionsValidator::outputOptionsAreValid()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	bool validOption = true;

	for (std::vector<std::string>::const_iterator iter = m_optionValues.begin();
		iter != m_optionValues.end(); iter++)
	{
		if (!isValidOption(*iter))
		{
			validOption = false;
			break;
		}
	}

	return validOption;
}

bool cli::framework::OutputOptionsValidator::conflictingOutputOptionsExist()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	int formatOptionCount = 0;
	bool conflictExists = false;

	for (std::vector<std::string>::const_iterator iter = m_optionValues.begin();
		iter != m_optionValues.end(); iter++)
	{
		if (isFormatOption(*iter))
		{
			formatOptionCount++;

			if (formatOptionCount > 1)
			{
				conflictExists = true;
				break;
			}
		}
	}

	return conflictExists;
}

cli::framework::SyntaxErrorResult *cli::framework::OutputOptionsValidator::checkForDuplicateOutputOptions()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	cli::framework::SyntaxErrorResult *pSyntaxError = NULL;
	const int maxOutputTypeCount = 1;
	std::vector<std::string>::const_iterator iter;

	for (iter = m_validOutputOptions.begin(); iter != m_validOutputOptions.end(); iter++)
	{
		int outputTypeCount = std::count(m_optionValues.begin(), m_optionValues.end(), *iter);

		if (outputTypeCount > maxOutputTypeCount)
		{
			pSyntaxError = new cli::framework::DuplicateTokenErrorResult(*iter, TOKENTYPE_VALUE);
			break;
		}
	}

	return pSyntaxError;
}

cli::framework::SyntaxErrorResult *cli::framework::OutputOptionsValidator::checkOutputOptionConsistency()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	cli::framework::SyntaxErrorResult *pResult = NULL;

	if (!outputOptionsAreValid() || conflictingOutputOptionsExist())
	{
		pResult = new SyntaxErrorResult(OUTPUT_ERROR_MSG);
	}
	return pResult;
}

cli::framework::SyntaxErrorResult *cli::framework::OutputOptionsValidator::validate()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	cli::framework::SyntaxErrorResult *pResult = NULL;

	if (m_optionValues.size() <= MAX_OUTPUT_OPTIONS)
	{
		pResult = checkForDuplicateOutputOptions();

		if (!pResult)
		{
			pResult = checkOutputOptionConsistency();
		}
	}
	else
	{
		pResult = new SyntaxErrorResult(OUTPUT_ERROR_MSG);
	}

	return pResult;
}
