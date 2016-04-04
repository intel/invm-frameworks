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

#include "FeatureBase.h"
#include "SyntaxErrorMissingValueResult.h"
#include "SyntaxErrorUnexpectedValueResult.h"
#include "Trace.h"
#include "osAdapter.h"

cli::framework::FeatureBase::FeatureBase()
 : m_checkUserResponse(&checkUserResponse),
   m_readUserHiddenString(&readUserHiddenString)
{
}

/*
 * Prompt user and give them a choice to answer yes (return true) or no (return false) to the prompt.
 * Default is false.
 */
bool cli::framework::FeatureBase::promptUserYesOrNo(std::string prompt)
{
	// prompt user to make sure they want to do that

	std::string promptQuestion = framework::ResultBase::stringFromArgList(
			(prompt + " " + YES_OR_NO + " ").c_str());
	std::cout << promptQuestion;
	return m_checkUserResponse();
}


/*
 * Prompt user for string response.  This is just here so we can override it for testing.
 */
void cli::framework::FeatureBase::readUserHiddenString(std::string *p_strResponse)
{
	os_readUserHiddenString(p_strResponse);
}

/*
 * Prompt user for a string response to the prompt.
 */
std::string cli::framework::FeatureBase::promptUserHiddenString(std::string prompt)
{
	std::cout << prompt;
	std::string strResponse;
	m_readUserHiddenString(&strResponse);
	return strResponse;
}

/*
 * Check user input.  Return true (user answered yes) or false (user answered no).
 * Default is false.
 */
bool cli::framework::FeatureBase::checkUserResponse()
{
	bool okayToContinue = false;

	// read response and return if user said yes to continue
	std::string answer;
	std::getline(std::cin, answer);

	answer[0] = tolower(answer[0]);
	if ((answer.length() == PROMPT_YES.length()) &&
			(0 == answer.compare(PROMPT_YES)))
	{
			okayToContinue = true;
	}
	return okayToContinue;
}
