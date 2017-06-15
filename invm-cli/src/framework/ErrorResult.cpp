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

#include "ErrorResult.h"
#include "cr_i18n.h"

cli::framework::ErrorResult::ErrorResult(std::string prefix)
		: SimpleResult (""), m_prefix(prefix)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	setErrorCode(ERRORCODE_UNKNOWN);
}

cli::framework::ErrorResult::ErrorResult(int errorCode,
		std::string errorMessage, std::string prefix)
		: SimpleResult (errorMessage),  m_prefix(prefix)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	setErrorCode(errorCode);
}

std::string cli::framework::ErrorResult::getErrorString() const
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	std::string result = "";
	switch(getErrorCode())
	{
		case ERRORCODE_SYNTAX:
			result = TR("Syntax Error");
			break;
		case ERRORCODE_NOTSUPPORTED:
		case ERRORCODE_OUTOFMEMORY:
		case ERRORCODE_UNKNOWN:
		default:
			result = TR("Error");
			break;
	}

	return result;
}

std::string cli::framework::ErrorResult::outputText() const
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	log_error(logger, getErrorString() + m_result);
	std::stringstream result;
	if (!m_prefix.empty())
	{
		result << m_prefix << ": ";
	}
	result << getErrorString() << ": " << m_result;
	return result.str();
}

std::string cli::framework::ErrorResult::outputXml() const
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;
	// start tag
	result << "<Error ";
	// type attribute
	result << errorResultXmlTypeAttribute << "=" << "\"" << getErrorCode() << "\"";
	// value
	result << ">";
	result << outputText();
	// end tag
	result << "</Error>\n";
	return result.str();
}

std::string cli::framework::ErrorResult::outputEsxXml() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return std::string("ERROR: ") + outputText();
}

std::string cli::framework::ErrorResult::outputJson() const
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;
	// start brace
	result << "{\n";
	// type attribute
	result << "\t\"" << errorResultXmlTag << "_" << errorResultXmlTypeAttribute << "\":" << getErrorCode() << ",\n";
	// value
	result << "\t\"" << errorResultJsonDescription << "\":" << "\"" << outputText() << "\"\n";
	
	// end brace
	result << "}\n";
	return result.str();
}


