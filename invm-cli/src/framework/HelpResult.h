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
 * Result for when help is requested or needed
 */

#ifndef _CLI_FRAMEWORK_HELPRESULT_H
#define _CLI_FRAMEWORK_HELPRESULT_H

#include "ResultBase.h"
#include "CommandSpec.h"
#include "cr_i18n.h"

namespace cli
{
namespace framework
{

const std::string helpResultIntroStr =
		N_TR("The NVMCLI exposes all available management features of the underlying NVM-DIMM. \n"
		"The command line syntax follows the DMTF SM CLP standard (DSP0214) "
		"with the exception of the target portion of the command. "
		"The modified  syntax used utilizes key properties of the target without "
		"requiring a syntactically correct CIM object path. \n\n"); //!< Introduction string
const std::string helpResultUtilityStr = N_TR("Utility"); //!< Utility title
const std::string helpResultUsageStr = N_TR("Usage"); //!< Usage title
const std::string helpResultSyntaxStr = "<verb>[<options>][<targets>][<properties>]"; //!< Syntax
const std::string helpResultXmlTag = "Help"; //!< Help tag
const std::string helpResultCommandXmlTag = "Command"; //!< Command tag
const std::string helpResultCommandsStr = N_TR("Commands"); //!< Commands title
const std::string helpResultNameStr = N_TR("Name"); //!< Name tag
const std::string helpResultDescriptionStr = N_TR("Description"); //!< Description tag
const std::string helpResultSynopsisStr = N_TR("Synopsis"); //!< Synopsis Title
const std::string helpResultVerbStr = N_TR("Verb"); //!< Verb Tag
const std::string helpResultOptionsStr = N_TR("Options"); //!< Options tag
const std::string helpResultOptionXmlTag = "Option"; //!< Option tag
const std::string helpResultTargetsStr = N_TR("Targets"); //!< Targets tag
const std::string helpResultTargetXmlTag = "Target"; //!< Target tag
const std::string helpResultPropertiesStr = N_TR("Properties"); //!< Properties tag
const std::string helpResultPropertyXmlTag = "Property"; //!< Property tag
const std::string helpResultRequiredXmlTag = "Required"; //!< Required tag
const std::string helpResultValueXmlTag = "Value"; //!< Value tag
const std::string helpResultValueRequiredXmlTag = "ValueRequired"; //!< Value reqd tag
const std::string helpResultTrueXmlTag = "True"; //!< True string
const std::string helpResultFalseXmlTag = "False"; //!< False string
const std::string helpResultNoCommandsStr = N_TR("No commands to display"); //!< No Commands message

/*!
 * Result for when help is requested or needed
 */
class HelpResult : public ResultBase
{
public:
	/*!
	 * Constructor
	 */
	HelpResult();
	/*!
	 * Output help as text
	 * @return
	 * 		The output as a string
	 */
	std::string outputText() const;
	/*!
	 * Output help as XML
	 * @return
	 * 		The output as an XML string
	 */
	std::string outputXml() const;

	/*!
	 * Output help as ESX XML
	 * @return
	 * 		The output as an ESX XML string
	 */
	std::string outputEsxXml() const;

	/*!
	 * Output help as JSON
	 * @return
	 * 		The output as an JSON string
	 */
	std::string outputJson() const;

	/*!
	 * Add a CommandSpec to get help on
	 * @param commandSpec
	 * 		The CommandSpec to get help on.
	 */
	void push_back(const CommandSpec &commandSpec);

	/*!
	 * get how many commands are in Help
	 * @return
	 * 		Count
	 */
	size_t getCount() const;

	/*!
	 * Setter for showing full help
	 * @param value
	 */
	void setShowFull(bool value);

	/*!
	 * Getter for showing full help
	 * @return
	 */
	bool getShowFull() const;

	void setShouldWrap(bool value);

private:
	CommandSpecList m_commands;
	std::string listToString(CommandSpecPartList list, bool isProperty, size_t start, size_t width) const;
	std::string listToXml(std::string listName, std::string itemName, CommandSpecPartList list) const;
	std::string listToJson(std::string listName, CommandSpecPartList list) const;
	std::string wrap(std::string text, size_t maxWidth) const;
	std::string consolidateToColumn(size_t colStart, size_t colWidth, std::string text) const;
	std::string CommandSpecPartToString(cli::framework::CommandSpecPart part, bool isProperty) const;
	bool m_showFull;
	bool m_shouldWrap;
};
}
}

#endif
