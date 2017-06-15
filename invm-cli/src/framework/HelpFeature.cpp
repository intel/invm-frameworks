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

#include "HelpFeature.h"

#include "FeatureBase.h"
#include "SimpleResult.h"
#include "PropertyListResult.h"
#include "HelpResult.h"
#include "ObjectListResult.h"
#include "NotImplementedErrorResult.h"

#include "CommandSpec.h"
#include "Trace.h"
#include <vector>
#include <string>
#include <sstream>
#include "cr_i18n.h"
#include "Framework.h"

/*
 * Command Specs the Help Feature supports
 */
void cli::framework::HelpFeature::getPaths(CommandSpecList &list)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	CommandSpecList result;
	CommandSpec help(HELP, TR("Help"), framework::VERB_HELP, TR("Show help for the supported commands."));
	help.addProperty(PROPERTY_COMMANDNAME);
	help.addProperty(PROPERTY_VERB);
	list.push_back(help);
}

// Constructor, just calls super class
cli::framework::HelpFeature::HelpFeature() : cli::framework::FeatureBase()
{
	cli::framework::HelpFeature::Name = "Help";
}

/*
 * Get all commandspecs from all features and show help.  Apply filter as appropriate.
 */
cli::framework::ResultBase * cli::framework::HelpFeature::run(
		const int &commandSpecId, const framework::ParsedCommand& parsedCommand)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	framework::HelpResult *pResult = NULL;

	pResult = new HelpResult();
	if (parsedCommand.verb == VERB_HELP)
	{
		std::string verbFilter;
		std::string commandNameFilter("");
		if (parsedCommand.properties.find(PROPERTY_VERB.name) != parsedCommand.properties.end())
		{
			verbFilter = parsedCommand.properties.at(PROPERTY_VERB.name);
		}

		if (parsedCommand.properties.find(PROPERTY_COMMANDNAME.name) != parsedCommand.properties.end())
		{
			commandNameFilter = parsedCommand.properties.at(PROPERTY_COMMANDNAME.name);
		}


		cli::framework::Framework *pFrameworkInst = cli::framework::Framework::getFramework();

		CommandSpecList commands = pFrameworkInst->getRegisteredCommands();
		for (size_t i = 0; i < commands.size(); i++)
		{
			CommandSpec &command = commands[i];
			if ((verbFilter.empty() || toLower(command.verb) == toLower(verbFilter)) &&
					(commandNameFilter.empty() || matchCommandName(commandNameFilter, command.name)))
			{
				pResult->push_back(commands[i]);
			}
		}
	}
	else
	{
		// Help is an option then, not the verb
		pResult->push_back(m_command);
		pResult->setShowFull(true);
	}


	if (pResult->getCount() == 1)
	{
		pResult->setShowFull(true);
	}


	return pResult;
}

/*
 * helper function to help with filtering.  It searches through the command name to and returns true if
 * the command name was found.  Otherwise, false.
 */
bool cli::framework::HelpFeature::matchCommandName(std::string filter, std::string commandName)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return toLower(commandName).find(toLower(filter)) != std::string::npos;
}

cli::framework::HelpFeature::HelpFeature(CommandSpec command) : m_command(command)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}
