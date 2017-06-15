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

#include "ParseErrorResult.h"
#include "CliFrameworkTypes.h"

cli::framework::ParseErrorResult::ParseErrorResult(Token invalidToken)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	setInvalidTokenResult(invalidToken);
}

void cli::framework::ParseErrorResult::setInvalidTokenResult(cli::framework::Token &invalidToken)
{
	m_result = cli::framework::ResultBase::stringFromArgList(TR("Invalid command. Error at or around '%s'."),
			invalidToken.lexeme.c_str());
}

cli::framework::ParseErrorResult::ParseErrorResult()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	m_result = TR("Invalid command.");
}

cli::framework::ParseErrorResult::ParseErrorResult(std::string lexeme,
	cli::framework::TokenType type)
{
	Token invalidToken;
	invalidToken.lexeme = lexeme;
	invalidToken.tokenType = type;
	setInvalidTokenResult(invalidToken);
}
