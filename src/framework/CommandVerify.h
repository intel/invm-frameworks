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

/*
 * Verifies a command after it has been selected as 'the' command that matches the user input.
 */

#ifndef INTEL_CLI_FRAMEWORK_COMMANDVERIFY_H
#define INTEL_CLI_FRAMEWORK_COMMANDVERIFY_H


#include "SyntaxErrorResult.h"

namespace cli
{
namespace framework
{
class CommandVerify
{
public:
	SyntaxErrorResult *verify(ParsedCommand parsedCommand, CommandSpec commandSpec);

private:
	SyntaxErrorResult *verifyCommonOptions(const ParsedCommand &parsedCommand);

	SyntaxErrorResult *verifyRequiredValues(const ParsedCommand &m_parsedCommand,
		const CommandSpec &spec);

	SyntaxErrorResult *verifyRequiredValues(const CommandSpecPartList& specList,
		const StringMap& parsedList, enum TokenType type);
};

}
}


#endif //INTEL_CLI_FRAMEWORK_COMMANDVERIFY_H
