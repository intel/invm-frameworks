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
 * Class to validate the output options
 */

#ifndef _CLI_FRAMEWORK_OUTPUTOPTIONSVALIDATOR_
#define _CLI_FRAMEWORK_OUTPUTOPTIONSVALIDATOR_

#include "SyntaxErrorResult.h"
#include "DuplicateTokenErrorResult.h"
#include "CliFrameworkTypes.h"

#define	MAX_OUTPUT_OPTIONS	2

// the error message is put together this way instead of a more dynamic approach because
// it makes translation easier.
#ifdef CLI_OUTPUT_JSON
const std::string OUTPUT_ERROR_MSG = TR("Option 'output' accepts 'text', 'json', 'nvmxml' and 'verbose'. 'text', 'json', and 'nvmxml' cannot be used together");
#else
const std::string OUTPUT_ERROR_MSG = TR("Option '-output' accepts 'text', 'nvmxml' and 'verbose'. 'text' and 'nvmxml' cannot be used together.");
#endif

namespace cli
{
namespace framework
{
class OutputOptionsValidator
{
	public:
		OutputOptionsValidator(const ParsedCommand &parsedCommand);

		SyntaxErrorResult *validate();

	private:
		std::vector<std::string> m_optionValues;
		std::vector<std::string> m_validOutputFormats;
		std::vector<std::string> m_validOutputOptions;

		bool outputOptionsAreValid();
		bool conflictingOutputOptionsExist();
		cli::framework::SyntaxErrorResult *checkForDuplicateOutputOptions();
		bool isFormatOption(const std::string &option);
		bool isValidOption(const std::string &option);
		cli::framework::SyntaxErrorResult *checkOutputOptionConsistency();
};

}
}

#endif
