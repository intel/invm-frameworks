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

/*
 * Class to parse the output options and verbose
 */

#include "OutputOptions.h"

cli::framework::OutputOptions::OutputOptions(const ParsedCommand &parsedCommand)
{
	m_verbose = false;
	m_outputFormat = "text";
	StringMap::const_iterator options_iter = parsedCommand.options.find(OUTPUT_OPTION);

	if (options_iter != parsedCommand.options.end() && !options_iter->second.empty())
	{
		m_outputOptionValues = tokenizeString(toLower(options_iter->second).c_str(), ',');

		m_validOutputFormats = getValidOutputFormats();
		setVerbose();
		setOutputFormat();
	}
}

void cli::framework::OutputOptions::setVerbose()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	if (m_outputOptionValues.end() != find(m_outputOptionValues.begin(),
			m_outputOptionValues.end(), OPTION_OUTPUT_VERBOSE))
	{
		m_verbose = true;
	}
	else
	{
		m_verbose = false;
	}
}

void cli::framework::OutputOptions::setOutputFormat()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::vector<std::string>::const_iterator iter;

	for (iter = m_outputOptionValues.begin(); iter != m_outputOptionValues.end(); iter++)
	{
		if (m_validOutputFormats.end() != find(m_validOutputFormats.begin(),
				m_validOutputFormats.end(), *iter))
		{
			m_outputFormat = *iter;
			break;
		}
	}
}

bool cli::framework::OutputOptions::getVerbose() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_verbose;
}

std::string cli::framework::OutputOptions::getOutputType() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_outputFormat;
}
