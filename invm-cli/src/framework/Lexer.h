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
 * The lexer is responsible for converting string tokens into known token types
 */

#ifndef _CLI_FRAMEWORK_LEXER_H_
#define _CLI_FRAMEWORK_LEXER_H_

#include <string>
#include <vector>
#include <map>
#include "CliFrameworkTypes.h"

namespace cli
{
namespace framework
{

/*!
 * Lexer is responsible for converting strings into known token types
 */
class Lexer
{
public:
	/*!
	 * Construct the Lexer
	 * @param verbs
	 * 		List of acceptable verbs
	 * @param targets
	 * 		List of acceptable targets
	 * @param options
	 * 		List of acceptable options
	 * @param properties
	 * 		List of acceptable properties
	 */
	Lexer(
			const StringList &verbs = std::vector<std::string>(),
			const StringList &targets = std::vector<std::string>(),
			const StringList &options = std::vector<std::string>(),
			const StringList &properties = std::vector<std::string>());

	/*!
	 * Convert the input into a list of Tokens
	 * @param argCount
	 * 		number of input tokens
	 * @param args
	 * 		input tokens
	 * @return
	 * 		the list of identified tokens
	 */
	std::vector<Token> tokenize(const int argCount, const char *args[]);
	/*!
	 * Convert the input into a list of tokens
	 * @param args
	 * 		list of strings user entered
	 * @return
	 * 		the list of tokens
	 */
	TokenList tokenize(const StringList &args);

private:
	StringList m_verbs;
	StringList m_targets;
	StringList m_options;
	StringList m_properties;
};


}
}
#endif
