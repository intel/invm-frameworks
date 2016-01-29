/*
 * Copyright (c) 2015, Intel Corporation
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

#include <stdio.h>

#include "HelpResult.h"
#include "Framework.h"
#include <iomanip>
#include "osAdapter.h"

cli::framework::HelpResult::HelpResult() :m_showFull(false), m_shouldWrap(true)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}

/*
 * Return text help for the command specs inserted.
 */
std::string cli::framework::HelpResult::outputText() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;

	std::string executable = cli::framework::Framework::getFramework()->executableName;

	// get the current window size so can wrap output intelligibly
	struct WindowSize winSize;
	getWindowSize(winSize);

	size_t tabSize = 4; // number of spaces used as a tab
	size_t col2 = winSize.width - (tabSize); // space left for text
	if (m_commands.size() > 1) // show quick help for many commands
	{
		// show usage
		result << TRS(helpResultUsageStr) << ":  " << executable << " " <<
				helpResultSyntaxStr << std::endl << std::endl;

		// start commands ...
		result << TRS(helpResultCommandsStr) << ": " << std::endl;

		for(size_t i = 0; i < m_commands.size(); i++)
		{
			const CommandSpec &command = m_commands[i];
			std::string commandName = command.name + ":";

			result << std::left << std::setw(tabSize) << commandName << std::endl;

			if (col2 <= 0)
			{
				result << command.asStr();
			}
			else
			{
				result << consolidateToColumn(tabSize, col2, command.asStr());
			}
		}
	}
	else if (m_commands.size() == 1) // show full help for just one
	{
		const CommandSpec &command = m_commands[0];
		result <<  TRS(helpResultNameStr) << ": "  << command.name << std::endl;
		result << TRS(helpResultDescriptionStr) << ":"  << std::endl;
		result << consolidateToColumn(tabSize, col2, command.help);
		result << 	TRS(helpResultSynopsisStr) << ":" << std::endl <<
				consolidateToColumn(tabSize, col2, command.asStr());
		result << TRS(helpResultVerbStr) << ":" << std::endl;
		result << std::left << std::setw(tabSize) << "" << command.verb << std::endl;
		result << TRS(helpResultOptionsStr) << ":" << std::endl
				<< listToString(command.options, false, tabSize, col2);
		result <<  TRS(helpResultTargetsStr) << ":" << std::endl << listToString(command.targets, false, tabSize, col2);
		result <<  TRS(helpResultPropertiesStr) << ":"  << std::endl << listToString(command.properties, true, tabSize, col2);
		result << std::endl;
	}
	else // no commands to show help for
	{
		result << TRS(helpResultNoCommandsStr)  << std::endl;
	}

	return result.str();
}

/*
 * create an XML document from the command specs inserted, then return the pretty print version
 * of the XML
 */
std::string cli::framework::HelpResult::outputXml() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;
	// start tag
	result << "<" << helpResultXmlTag << ">";
	if (m_commands.size() > 0)
	{
		result << "\n";
	}

	// commands
	CommandSpecList::const_iterator iter = m_commands.begin();
	for(; iter != m_commands.end(); iter++)
	{
		// command tag
		result << "\t<" << helpResultCommandXmlTag << " ";
		// name attribute
		result << helpResultNameStr << "=" << "\"" << iter->name << "\"";
		result << ">\n";

		// description
		result << "\t\t<" << helpResultDescriptionStr << ">";
		result << iter->help;
		result << "</" << helpResultDescriptionStr << ">\n";

		// verb
		result << "\t\t<" << helpResultVerbStr << ">";
		result << iter->verb;
		result << "</" << helpResultVerbStr << ">\n";

		// options
		result << listToXml(helpResultOptionsStr, helpResultOptionXmlTag, iter->options);

		// targets
		result << listToXml(helpResultTargetsStr, helpResultTargetXmlTag, iter->targets);

		// properties
		result << listToXml(helpResultPropertiesStr, helpResultPropertyXmlTag, iter->properties);

		// command end tag
		result << "\t</" << helpResultCommandXmlTag << ">\n";
	}

	// Help end tag
	result << "</" << helpResultXmlTag << ">\n";
	return result.str();
}

/*
 * create an JSON document from the command specs inserted, then return the pretty print version
 * of the JSON
 */
std::string cli::framework::HelpResult::outputJson() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;
	// start brace
	result << "{\n";

	// commands
	CommandSpecList::const_iterator iter = m_commands.begin();
	for (; iter != m_commands.end(); iter++)
	{
		
		// name of the command
		result << "\t\"" << iter->name << "\": {\n";

		// description
		result << "\t\t\"" << helpResultDescriptionStr << "\":";
		result << "\"" << iter->help << "\",\n";

		// verb
		result << "\t\t\"" << helpResultVerbStr << "\":";
		result << "\"" << iter->verb << "\",\n";

		// options
		result << listToJson(helpResultOptionsStr, iter->options);
		result << "\t\t},\n";

		// targets
		result << listToJson(helpResultTargetsStr, iter->targets);
		result << "\t\t},\n";

		// properties
		result << listToJson(helpResultPropertiesStr, iter->properties);
		result << "\t\t}\n";

		// command end brace
		result << "\t}";

		//Check the next iterator to see if we have reached the end
		//If the end has not been reached, add a comma
		CommandSpecList::const_iterator temp_iter = iter + 1;
		if (temp_iter != m_commands.end())
		{
			result << ",";
		}
		result << "\n";		
	}

	// Help end brace
	result << "}\n";
	return result.str();
}

void cli::framework::HelpResult::push_back(const CommandSpec& commandSpec)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_commands.push_back(commandSpec);
}

size_t cli::framework::HelpResult::getCount() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_commands.size();
}

void cli::framework::HelpResult::setShowFull(bool value)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_showFull = value;
}

void cli::framework::HelpResult::setShouldWrap(bool value)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	m_shouldWrap = value;
}

bool cli::framework::HelpResult::getShowFull() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return m_showFull;
}

/*
 * Convert the command part (options, targets, or properties) into a string representation that can be 
 * displayed to the user.
 */
std::string cli::framework::HelpResult::listToString(CommandSpecPartList list,
		bool isProperty, size_t start, size_t width) const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	for(size_t i = 0; i < list.size(); i++)
	{
		std::string part = CommandSpecPartToString(list[i], isProperty);
		result << consolidateToColumn(start, width,  part);

	}
	return result.str();
}

/*
 * create an XML tree from the command part (options, targets, or properties)
 */
std::string cli::framework::HelpResult::listToXml(
		std::string listName,
		std::string itemName,
		CommandSpecPartList list) const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;

	// list tag
	result << "\t\t<" << listName << ">";
	if (list.size() > 0)
	{
		result << "\n";
	}

	// list items
	CommandSpecPartList::iterator iter = list.begin();
	for (; iter != list.end(); iter++)
	{
		// item tag
		result << "\t\t\t<" << itemName;

		// item name attribute
		result << " " << helpResultNameStr << "=" << "\"" << iter->name << "\"";

		// item description attribute
		result << " " << helpResultDescriptionStr << "=" << "\"" << iter->help << "\"";

		// item required attribute
		result << " " << helpResultRequiredXmlTag << "=" << "\"";
		result << (iter->required ? helpResultTrueXmlTag : helpResultFalseXmlTag) << "\"";

		// item value attribute
		result << " " << helpResultValueXmlTag << "=" << "\"" << iter->value << "\"";

		// item value required attribute
		result << " " << helpResultValueRequiredXmlTag << "=" << "\"";
		result << (iter->valueRequired ? helpResultTrueXmlTag : helpResultFalseXmlTag) << "\"";

		// item end tag
		result << "/>\n";
	}


	// end list
	if (list.size() > 0)
	{
		result << "\t\t";
	}
	result << "</" << listName << ">\n";

	return result.str();
}

/*
* create an JSON output from the command part (options, targets, or properties)
*/
std::string cli::framework::HelpResult::listToJson(
	std::string listName,
	CommandSpecPartList list) const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	std::stringstream result;

	// list tag
	result << "\t\t\"" << listName << "\": {\n";

	// list items
	CommandSpecPartList::iterator iter = list.begin();
	for (; iter != list.end(); iter++)
	{

		// item name attribute
		// For JSON output we need to replace double quotes with single quotes
		std::string nameStr = iter->name;
		std::replace(nameStr.begin(), nameStr.end(), '\"', '\'');
		result << "\t\t\t\"" << nameStr << "\": {\n";

		// item description attribute
		// For JSON output we need to replace double quotes with single quotes
		std::string helpDescription = iter->help;
		std::replace(helpDescription.begin(), helpDescription.end(), '\"', '\'');

		result << "\t\t\t\"" << helpResultDescriptionStr << "\":" << "\"" << helpDescription << "\",\n";

		// item required attribute
		result << "\t\t\t\"" << helpResultRequiredXmlTag << "\":" << "";
		result << (iter->required ? toLower(helpResultTrueXmlTag) : toLower(helpResultFalseXmlTag)) << ",\n";

		// item value attribute
		// For JSON output we need to replace double quotes with single quotes
		std::string valueStr = iter->value;
		std::replace(valueStr.begin(), valueStr.end(), '\"', '\'');
		result << "\t\t\t\"" << helpResultValueXmlTag << "\":" << "\"" << valueStr << "\",\n";

		// item value required attribute
		result << "\t\t\t\"" << helpResultValueRequiredXmlTag << "\":" << "";
		result << (iter->valueRequired ? toLower(helpResultTrueXmlTag) : toLower(helpResultFalseXmlTag)) << "\n";

		result << "\t\t\t}";
		//Check the next iterator to see if we have reached the end
		//If the end has not been reached, add a comma
		CommandSpecPartList::iterator temp_iter = iter + 1;
		if (temp_iter != list.end())
		{
			result << ",";
		}
		result << "\n";
	}

	return result.str();
}

std::string cli::framework::HelpResult::outputEsxXml() const
{
	// help result should never be called for ESX. ESX handles help.
	return "";
}

/*
 * wrap a string to a max width breaking on an empty space
 */
std::string cli::framework::HelpResult::wrap(std::string text, size_t maxWidth) const
{
	if (text.length() > maxWidth)
	{
		text = text.substr(0, maxWidth);
		size_t lastSpace = text.find_last_of(" ");
		size_t lastOr = text.find_last_of("|");
		if (lastSpace != std::string::npos && lastSpace > 0)
		{
			text = text.substr(0, lastSpace + 1);
		}
		else if (lastOr != std::string::npos && lastOr > 0)
		{
			text = text.substr(0, lastOr + 1);
		}

	}
	return text;
}

/*
 * make text fit within a certain column of the console
 */
std::string cli::framework::HelpResult::consolidateToColumn(size_t colStart,
		size_t colWidth, std::string text) const
{
	std::stringstream result;
	if (m_shouldWrap)
	{
		if (colWidth > 0)
		{
			bool first = false;
			size_t idx = 0;
			do
			{
				if (!first)
				{
					result << std::left  << std::setw(colStart) << " ";
				}
				first = false;
				std::string wrappedText = wrap(text.substr(idx, text.length()), colWidth);
				result << wrappedText << std::endl;
				idx += wrappedText.length();
			} while (idx < text.length());
		}
		else
		{
			result << text;
		}
	}
	else
	{
		result << text;
	}
	return result.str();
}

/*
 * Convert a command spec to a string
 */
std::string cli::framework::HelpResult::CommandSpecPartToString(
		cli::framework::CommandSpecPart part, bool isProperty) const
{
	std::string result =  part.name;
	if (!part.abr.empty())
	{
		result += "|" + part.abr;
	}
	std::string value = part.value;

	if (!value.empty())
	{
		value = "(" + value + ")";
		if (!part.valueRequired)
		{
			value = "[" + value + "]";
		}
		if (isProperty)
		{
			value = " = " + value;
		}
		else
		{
			value = " " + value;
		}
	}
	if (!part.required)
	{
		result = "[" + result + value + "]";
	}
	else
	{
		result = result + value;
	}
	if (!part.help.empty())
	{
		// Note about translation - because some of the help text are defined in const
		// CommandSpecParts, they need to be translated here. If there is no translation (or it is
		// already translated, gettext will just return the original text it receives.
		result += std::string(" -- ") + TRS(part.help);
	}
	return result;
}
