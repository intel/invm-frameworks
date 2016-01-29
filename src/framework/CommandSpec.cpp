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

#include "CommandSpec.h"
#include "CliFrameworkTypes.h"

/*
 * Return a string representing the command spec
 */
std::string cli::framework::CommandSpec::asStr() const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::string result = verb + " " +
			listToString(options) +
			listToString(targets) +
			listToString(properties, true);

	return trim(result);
}
cli::framework::CommandSpec::CommandSpec(int id, std::string name, std::string verb, std::string help)
: id(id), name(name), verb(verb), help(help), m_pFeature(NULL)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}

cli::framework::CommandSpec::CommandSpec() : id(0)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addOption(
		const std::string& name,
		const bool isRequired, const std::string& valueText,
		const bool isValueRequired,
		const std::string &helpText,
		const std::string &abr)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);

	CommandSpecPart part = {name, isRequired, valueText, isValueRequired, helpText, abr};

	return addOption(part);
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addOption(const CommandSpecPart& part)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return addPart(options, part);
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addTarget(const std::string& name,
		const bool isRequired, const std::string& valueText,
		const bool isValueRequired,
		const std::string &helpText)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return addTarget((CommandSpecPart) {name, isRequired, valueText, isValueRequired, helpText});
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addTarget(const CommandSpecPart& part)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return addPart(targets, part);
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addProperty(const std::string& name,
		const bool isRequired, const std::string& valueText,
		const bool isValueRequired,
		const std::string &helpText)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return addProperty((CommandSpecPart) {name, isRequired, valueText, isValueRequired, helpText});
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addProperty(const CommandSpecPart& part)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	return addPart(properties, part);
}

cli::framework::CommandSpecPartDecorator cli::framework::CommandSpec::addPart(CommandSpecPartList& partList, const CommandSpecPart& part)
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	// don't add the same one again
	int  foundIndex = -1;
	for (size_t j = 0; j < partList.size() && foundIndex < 0; j++)
	{
		foundIndex = partList[j].name == part.name ? (int)j : foundIndex;
	}
	if (foundIndex < 0)
	{
		partList.push_back(part);
		foundIndex = partList.size() - 1; // new one will always be the last
	}

	CommandSpecPartDecorator partDecorator(partList[foundIndex]);
	return partDecorator;
}

std::string cli::framework::CommandSpec::listToString(const CommandSpecPartList &list, bool isProperty) const
{
	Trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	for(size_t i = 0; i < list.size(); i++)
	{
		std::string name = list[i].name;
		if (!list[i].abr.empty())
		{
			name = name + "|" + list[i].abr;
		}
		std::string value = list[i].value;

		if (!value.empty())
		{
			if (value.size() > 1)
				value = "(" + value + ")";
			if (!list[i].valueRequired)
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
		if (!list[i].required)
		{
			name = "[" + name + value + "]";
		}
		else
		{
			name = name + value;
		}
		result << name + " ";

	}
	return result.str();
}

cli::framework::CommandSpecPartList &cli::framework::CommandSpec::getCommandSpecPartList(
	cli::framework::TokenType type)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	cli::framework::CommandSpecPartList * result = NULL;
	if (type == TOKENTYPE_TARGET)
	{
		result = &targets;
	}
	else if (type == TOKENTYPE_PROPERTY)
	{
		result = &properties;
	}
	else if (type == TOKENTYPE_OPTION)
	{
		result = &options;
	}
	return *result;
}

void * cli::framework::CommandSpec::getFeature() const
{
	return m_pFeature;
}

void cli::framework::CommandSpec::setFeature(void * feature)
{
	m_pFeature = feature;
}
