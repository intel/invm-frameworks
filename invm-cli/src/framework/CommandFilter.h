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
 * Filters a list of commands based on parsed input from the user.
 */

#ifndef INTEL_CLI_FRAMEWORK_COMMANDFILTER_H
#define INTEL_CLI_FRAMEWORK_COMMANDFILTER_H

#include "CliFrameworkTypes.h"
#include "CommandSpec.h"

namespace cli
{
namespace framework
{
class CommandFilter
{

public:
	CommandFilter(ParsedCommand &parsedCommand, UnknownProperty &unknownProperty);
	CommandFilter(ParsedCommand &parsedCommand);
	CommandSpecList &getLastErasedCommands();

	void filter(CommandSpecList &commandsList);

private:
	// disallow copying
	CommandFilter(const CommandFilter&);
	CommandFilter &operator=(const CommandFilter&);

	ParsedCommand &m_parsedCommand;
	UnknownProperty &m_unknownProperty;
	CommandSpecList m_erasedCommands;

	void filterOnVerb(cli::framework::CommandSpecList &commandsList);

	void filterOnParsedList(cli::framework::CommandSpecList &commandsList,
		const cli::framework::CommandSpecPartType &parsedListType);

	void filterOnUnMetRequirements(CommandSpecList &commandsList);
	void decideUnknown(CommandSpecList &commandsList);

	void shouldRemoveCommand(bool &shouldRemove,
			const cli::framework::CommandSpecPartList &commandSpec,
			const CommandSpecPartType &partType);

	const cli::framework::StringMap &getParsedListOfType(const CommandSpecPartType &type) const;

	std::string getParsedListTypeString(const CommandSpecPartType &parsedListType) const;

	const cli::framework::CommandSpecPartList &getCommandPartListOfType(
		const CommandSpecPartType &parsedListType, const CommandSpec &command) const;


	bool unknownLexemeIsAlreadyProperty() const;
	bool anyCommandHasUnknownProperty(const CommandSpecList &commandsList) const;
	void setUnknownAsValue();
	void setUnknownAsProperty();
	void removeCommand(cli::framework::CommandSpecList &commandsList, size_t commandIndex);
	bool hasUnknownProperty();

	static UnknownProperty UNKNOWNPROPERTY_EMPTY; // used for default property in constructor
};

}
}


#endif //INTEL_CLI_FRAMEWORK_COMMANDFILTER_H
