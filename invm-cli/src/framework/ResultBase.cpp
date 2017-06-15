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

#include "ResultBase.h"
#include <stdarg.h>
#include <stdio.h>

cli::framework::ResultBase::ResultBase()
	: m_errorCode(ERRORCODE_SUCCESS), m_outputType(OUTPUT_TEXT)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}

void cli::framework::ResultBase::setOutputType(enum outputTypes type)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_outputType = type;
}


int cli::framework::ResultBase::getErrorCode() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_errorCode;
}

void cli::framework::ResultBase::setErrorCode(int errorCode)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_errorCode = errorCode;
}

enum cli::framework::ResultBase::outputTypes cli::framework::ResultBase::getOutputType()
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_outputType;
}

bool cli::framework::ResultBase::setOutputOption(StringMap options)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	bool result = true;
	if (options.find(OUTPUT_OPTION) != options.end())
	{
		if (stringsIEqual(options[OUTPUT_OPTION], OPTION_OUTPUT_XML))
		{
			setOutputType(framework::ResultBase::OUTPUT_XML);
		}
		else if (stringsIEqual(options[OUTPUT_OPTION], OPTION_OUTPUT_TEXT))
		{
			// Don't allow the framework to overwrite the output type if the feature
			// decided it should be in table form.
			// No output option specified should look exactly like the text output option.
			if (getOutputType() != framework::ResultBase::OUTPUT_TEXTTABLE)
			{
				setOutputType(framework::ResultBase::OUTPUT_TEXT);
			}
		}
#ifdef CLI_OUTPUT_JSON
		else if (stringsIEqual(options[OUTPUT_OPTION], OPTION_OUTPUT_JSON))
		{
			setOutputType(framework::ResultBase::OUTPUT_JSON);
		}
#endif
#ifdef CLI_OUTPUT_ESX
		else if (stringsIEqual(options[OUTPUT_OPTION], OPTION_OUTPUT_ESX))
		{
			setOutputType(framework::ResultBase::OUTPUT_ESXXML);
		}

		else if (stringsIEqual(options[OUTPUT_OPTION], OPTION_OUTPUT_ESXTABLE))
		{
			setOutputType(framework::ResultBase::OUTPUT_ESXXMLTABLE);
		}
#endif
		else
		{
			result = false;
		}
	}
	return result;
}

std::string cli::framework::ResultBase::output() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::string result;
	switch(m_outputType)
	{
		case OUTPUT_TEXT:
			result = outputText();
			break;
		case OUTPUT_XML:
			result = outputXml();
			break;
		case OUTPUT_ESXXML:
			result = outputEsxXml();
			break;
		case OUTPUT_TEXTTABLE:
			result = outputTextTable();
			break;
		case OUTPUT_JSON:
			result = outputJson();
			break;
		case OUTPUT_ESXXMLTABLE:
			result = outputEsxXmlTable();
			break;
	}
	return result;
}

/*!
 * Default implementation of text table is text
 */
std::string cli::framework::ResultBase::outputTextTable() const
{
	return outputText();
}

/*!
 * Default implementation of esx xml table is esx xml
 */
std::string cli::framework::ResultBase::outputEsxXmlTable() const
{
	return outputEsxXml();
}

/*
 * Convert a variable argument list into a std::string
 */
std::string cli::framework::ResultBase::stringFromArgList(const char *format, ...)
{
	std::string m_result;
	va_list args;
	va_start(args, format);

	// get the required buffer size (minus the ending null character)
	int bufferSize = vsnprintf(NULL, 0, format, args);

	// add space for ending null char, create & fill buffer
	bufferSize++;
	char buffer[bufferSize];
	va_start(args, format);
	vsnprintf(buffer, bufferSize, format, args);

	// set the error message string
	m_result = buffer;
	va_end(args);

	return m_result;
}
