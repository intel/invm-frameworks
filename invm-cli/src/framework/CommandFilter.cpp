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

#include "CommandFilter.h"
#include "CliFrameworkTypes.h"
cli::framework::UnknownProperty cli::framework::CommandFilter::UNKNOWNPROPERTY_EMPTY;

cli::framework::CommandFilter::CommandFilter(cli::framework::ParsedCommand &parsedCommand,
		UnknownProperty &unknownProperty) :
		m_parsedCommand(parsedCommand),
		m_unknownProperty(unknownProperty)
{
}

cli::framework::CommandFilter::CommandFilter(cli::framework::ParsedCommand &parsedCommand) :
		m_parsedCommand(parsedCommand),
		m_unknownProperty(UNKNOWNPROPERTY_EMPTY)
{
}



void cli::framework::CommandFilter::filter(CommandSpecList &commandsList)
{
	logger << "Starting filter for " << commandsList.size() << " commands." << std::endl;

	filterOnVerb(commandsList);
	filterOnParsedList(commandsList, PARTTYPE_TARGET);
	filterOnParsedList(commandsList, PARTTYPE_PROPERTY);
	filterOnParsedList(commandsList, PARTTYPE_OPTION);
	filterOnUnMetRequirements(commandsList);

	if (hasUnknownProperty())
	{
		decideUnknown(commandsList);
	}
}

void cli::framework::CommandFilter::filterOnVerb(cli::framework::CommandSpecList &commandsList)
{
	logger << "Filtering on verb: " << m_parsedCommand.verb << std::endl;
	for (size_t c = commandsList.size(); c > 0; c--)
	{
		const cli::framework::CommandSpec &command = commandsList[c - 1];
		if (command.verb != m_parsedCommand.verb)
		{
			removeCommand(commandsList, c - 1);
		}
	}

	logger << " --- " << commandsList.size() << " commands left." << std::endl;
}

void cli::framework::CommandFilter::removeCommand(cli::framework::CommandSpecList &commandsList, size_t commandIndex)
{
	logger << " --- Removing: \"" << commandsList[commandIndex].asStr() << "\"" << std::endl;
	this->m_erasedCommands.push_back(commandsList[commandIndex]);
	commandsList.erase(commandsList.begin() + commandIndex);
}

void cli::framework::CommandFilter::filterOnParsedList(CommandSpecList &commandsList,
	const CommandSpecPartType &parsedListType)
{
	const std::string parsedTokenType = getParsedListTypeString(parsedListType);
	const cli::framework::StringMap &parsedList = getParsedListOfType(parsedListType);

	for (StringMap::const_iterator i = parsedList.begin();
		i != parsedList.end() && commandsList.size() > 0; i++)
	{
		m_erasedCommands.clear();

		std::string parsedItemName = i->first;

		logger << "Filtering on " << parsedTokenType << ": "<< parsedItemName << std::endl;

		for (size_t c = commandsList.size(); c > 0; c--)
		{
			int commandIndex = c - 1;
			const cli::framework::CommandSpec &command = commandsList[commandIndex];
			cli::framework::CommandSpecPartList commandPartList
				= getCommandPartListOfType(parsedListType, command);

			if (getTokenIndex(parsedItemName, commandPartList) < 0)
			{
				removeCommand(commandsList, commandIndex);
			}
		}

		logger << " --- " << commandsList.size() << " commands left." << std::endl;
	}
}

void cli::framework::CommandFilter::filterOnUnMetRequirements(
	cli::framework::CommandSpecList &commandsList)
{
	logger << "Filtering " << commandsList.size() << " commands on unmet requirements" << std::endl;
	for (size_t c = commandsList.size(); c > 0; c--)
	{
		const CommandSpec &command = commandsList[c - 1];

		bool shouldRemove = false;
		shouldRemoveCommand(shouldRemove, command.options, PARTTYPE_OPTION);
		shouldRemoveCommand(shouldRemove, command.targets, PARTTYPE_TARGET);
		shouldRemoveCommand(shouldRemove, command.properties, PARTTYPE_PROPERTY);

		if (shouldRemove)
		{
			removeCommand(commandsList, c - 1);
		}
	}

	logger << "Commands left after checking required parts: " << commandsList.size() << std::endl;
}

void cli::framework::CommandFilter::shouldRemoveCommand(bool &shouldRemove,
		const cli::framework::CommandSpecPartList &commandSpec, const CommandSpecPartType &partType)
{
	const StringMap &parsedMap = getParsedListOfType(partType);
	std::string itemType = getParsedListTypeString(partType);

	for (size_t t = 0; t < commandSpec.size() && !shouldRemove; t++)
	{
		const cli::framework::CommandSpecPart &item = commandSpec[t];

		bool found = false;

		for (StringMap::const_iterator i = parsedMap.begin();
			 i != parsedMap.end() && !found; i++)
		{
			if (stringsIEqual(i->first, item.name))
			{
				found = true;
			}
		}

		// If currently looking at properties, then the property may have been found but might
		// not be known it is a property yet.
		if (partType == PARTTYPE_PROPERTY && stringsIEqual(item.name, m_unknownProperty.lexeme))
		{
			found = true;
		}

		if (item.required && !found)
		{
			logger << itemType + " '" << item.name << "' required, but not found." << std::endl;
			shouldRemove = true;
		}
	}
}

void cli::framework::CommandFilter::decideUnknown(cli::framework::CommandSpecList &commandsList)
{
	logger << "Have an unknown property. Deciding what to do with it." << std::endl;
	if (unknownLexemeIsAlreadyProperty())
	{
		setUnknownAsValue();
	}
	else
	{
		if (anyCommandHasUnknownProperty(commandsList))
		{
			setUnknownAsProperty();
			filterOnParsedList(commandsList, PARTTYPE_PROPERTY);
		}
		else
		{
			setUnknownAsValue();
		}
	}
}

const cli::framework::CommandSpecPartList &cli::framework::CommandFilter::getCommandPartListOfType(
	CommandSpecPartType const &parsedListType, CommandSpec const &command) const
{
	const CommandSpecPartList *pResult = NULL;
	switch (parsedListType)
	{

		case cli::framework::PARTTYPE_OPTION:
			pResult = &command.options;
			break;
		case cli::framework::PARTTYPE_TARGET:
			pResult = &command.targets;
			break;
		case cli::framework::PARTTYPE_PROPERTY:
			pResult = &command.properties;
			break;
	}

	return *pResult;
}

std::string cli::framework::CommandFilter::getParsedListTypeString(
	cli::framework::CommandSpecPartType const &parsedListType) const
{
	std::string parsedTokenType = "";
	switch (parsedListType)
	{
		case cli::framework::PARTTYPE_OPTION:
			parsedTokenType = "option";
			break;
		case cli::framework::PARTTYPE_TARGET:
			parsedTokenType = "target";
			break;
		case cli::framework::PARTTYPE_PROPERTY:
			parsedTokenType = "property";
			break;
	}
	return parsedTokenType;
}

const cli::framework::StringMap &cli::framework::CommandFilter::getParsedListOfType(
	const CommandSpecPartType &type) const
{
	const StringMap *pParsedList = NULL;

	switch (type)
	{
		case cli::framework::PARTTYPE_OPTION:
			pParsedList = &m_parsedCommand.options;
			break;
		case cli::framework::PARTTYPE_TARGET:
			pParsedList = &m_parsedCommand.targets;
			break;
		case cli::framework::PARTTYPE_PROPERTY:
			pParsedList = &m_parsedCommand.properties;
			break;
	}
	return *pParsedList;
}



cli::framework::CommandSpecList &cli::framework::CommandFilter::getLastErasedCommands()
{
	return m_erasedCommands;
}

bool cli::framework::CommandFilter::unknownLexemeIsAlreadyProperty() const
{
	const bool result = m_parsedCommand.properties.find(m_unknownProperty.lexeme)
			!= m_parsedCommand.properties.end();
	logger << "Is unknown lexeme '" << m_unknownProperty.lexeme
		<< "' already a parsed property: " << (result ? "Yes" : "No") << std::endl;
	return result;
}

bool cli::framework::CommandFilter::anyCommandHasUnknownProperty(
	const CommandSpecList &commandsList) const
{
	bool found = false;
	for (size_t c = 0; c < commandsList.size() && !found; c++)
	{
		for (size_t p = 0; p < commandsList[c].properties.size() && !found; p++)
		{
			if (stringsIEqual(commandsList[c].properties[p].name,
					m_unknownProperty.lexeme))
			{
				found = true;
			}
		}
	}

	logger << "Does any command have '" <<m_unknownProperty.lexeme
		<< "' as a property: " << (found ? "Yes" : "No") << std::endl;

	return found;
}

void cli::framework::CommandFilter::setUnknownAsValue()
{
	UnknownProperty &unknownProperty = m_unknownProperty;
	std::string value = unknownProperty.lexeme;
	std::string key = unknownProperty.tokenKey;
	logger << "Setting the unknown token as value of " << key << std::endl;

	switch (unknownProperty.type)
	{
		case PARTTYPE_OPTION:
			m_parsedCommand.options[key] = value;
			break;
		case PARTTYPE_TARGET:
			m_parsedCommand.targets[key] = value;
			break;
		case PARTTYPE_PROPERTY:
			m_parsedCommand.properties[key] = value;
			break;
	}

	unknownProperty.tokenKey = "";
	unknownProperty.lexeme = "";
}

void cli::framework::CommandFilter::setUnknownAsProperty()
{
	logger << "Setting the unknown token as property." << std::endl;
	m_parsedCommand.properties[m_unknownProperty.lexeme] = "";

	m_unknownProperty.tokenKey = "";
	m_unknownProperty.lexeme = "";
}

bool cli::framework::CommandFilter::hasUnknownProperty()
{
	return !m_unknownProperty.lexeme.empty();
}
