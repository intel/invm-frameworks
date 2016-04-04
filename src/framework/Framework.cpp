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

#include "Framework.h"

#include "Lexer.h"
#include "ErrorResult.h"
#include "ParseErrorResult.h"
#include "SyntaxErrorResult.h"
#include "SyntaxErrorUnexpectedValueResult.h"
#include "SyntaxErrorMissingValueResult.h"
#include "SyntaxErrorBadValueResult.h"
#include "HelpFeature.h"
#include "Parser.h"
#include "CliFrameworkTypes.h"
#include "CommandFilter.h"
#include "CommandVerify.h"
#include <sstream>

bool cli::framework::Framework::instanceFlag = false;
cli::framework::Framework* cli::framework::Framework::pFrameworkInst = NULL;


/*
 * Return single instance of Framework
 */
cli::framework::Framework* cli::framework::Framework::getFramework()
{
	if (!instanceFlag)
	{
		pFrameworkInst = new cli::framework::Framework;
		pFrameworkInst->executableName = "";
		instanceFlag = true;

		// add help feature
		cli::framework::FeatureBase *pHelpFeature = new cli::framework::HelpFeature();
		pFrameworkInst->registerFeature("HelpFeature", pHelpFeature);
	}
	return pFrameworkInst;
}

/*
 * Execute is the main entry point for the framework.  It parses the user's input, matches it
 * to the appropriate CommandSpec/Feature and then runs the feature, returning the Result from
 * the feature.
 * Note: The CommandSpecList is iterated backward because the iteration loop typically modifies
 * 		 the list which changes indexes.
 */
cli::framework::ResultBase* cli::framework::Framework::execute(const StringList &tokens)
{
	// This is what nvmcli calls to execute the command line - tokens would be everything after nvmcli
	Trace(__FILE__, __FUNCTION__, __LINE__);
	const char *args[tokens.size()];

	for (size_t i = 0; i < tokens.size(); i++)
	{
		args[i] = tokens[i].c_str();
	}

	return execute((int)tokens.size(), (const char **)args);
}

/*
 * Execute is the main entry point for the framework.  It parses the user's input, matches it
 * to the appropriate CommandSpec/Feature and then runs the feature, returning the Result from
 * the feature.
 */
cli::framework::ResultBase* cli::framework::Framework::execute(int argCount, const char* args[])
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	ResultBase *pResult = NULL;
	if (argCount > 0)
	{
		logger << "Executing: " << tokenArrayToString(argCount, args) << std::endl;

		// add help and output options
		CommandSpecList commands = getRegisteredCommands();
		logger << "Commands to consider: " << commands.size() << std::endl;

		// convert user input into recognized tokens.  Command List is needed because
		// it is used to know what verbs, targets, and properties are supported
		TokenList tokens = tokenize(argCount, args, commands);
		logger << "Tokenized input is: " << tokenArrayToString(tokens) << std::endl;

		SyntaxErrorResult *pError = NULL;
		// The parser will convert the list of tokens into a parsed command.  It validates that the tokens
		// are in the proper order and will assign values to options, targets, and properties.
		Parser parser;
		pError = parser.parse(tokens);
		ParsedCommand parsedCommand = parser.getParsedCommand();

		// The filter will take the parsedCommand and filter the possible commands, hopefully
		// leaving only one left to execute.
		UnknownProperty unknownProperty = parser.getUnknownProperty();
		CommandFilter commandFilter(parsedCommand, unknownProperty);
		commandFilter.filter(commands);

		if (pError != NULL)
		{
			pResult = pError;
			if (parser.includePotentialCommandsIsRequested())
			{
				pError->setPotentialCommands(commandFilter.getLastErasedCommands());
			}
		}
		else
		{
			if (commands.size() == 1) // success ... 1 command to run
			{
				CommandVerify verify;
				pResult = verify.verify(parsedCommand, commands[0]);

				if (pResult == NULL)
				{
					// All good ... go, fight, win!!
					FeatureBase *pFeature = NULL;
					if (parsedCommand.options.find(OPTION_HELP.name) != parsedCommand.options.end())
					{
						// Intercept feature if asking for help
						pFeature = new HelpFeature(commands[0]);
					}
					else
					{
						pFeature = getCommandFeature(commands[0]);
					}

					logger << "Executing: " << commands[0].asStr() << std::endl;
					pResult = pFeature->run(commands[0].id, parsedCommand);
				}
			}
			else if (commands.empty())
			{
				logger << "No commands left. Setting potential commands with " <<
					commandFilter.getLastErasedCommands().size() << " commands" << std::endl;

				pError = new ParseErrorResult();
				pError->setPotentialCommands(commandFilter.getLastErasedCommands());
				pResult = pError;
			}
			else if (commands.size() > 1)
			{
				// this only happens if there are two CommandSpecs with the same syntax
				pResult = new ErrorResult(ErrorResult::ERRORCODE_UNKNOWN,
					"The command list had more than one after filtering. This means that there might be more than one CommandSpec with the same required syntax.");
				logger << "The conflicting commands are: ";
				CommandSpecList::const_iterator iter = commands.begin();
				for (; iter != commands.end(); iter++)
				{
					logger << "\"" + iter->asStr() << "\" ";
				}
				logger << std::endl;
			}
		}

		if (pResult != NULL)
		{
			pResult->setOutputOption(parser.getParsedCommand().options);
		}
	}
	else
	{
		pResult = HelpFeature().run(HelpFeature::HELP, (ParsedCommand){VERB_HELP});
	}
	return pResult;
}

/*
 * The optionsMap serves two purposes.  One is to act as an abbreviation lookup for options.  Second
 * is to include default options that aren't explicitly required to be included in CommandSpecs
 * but are always available (-help, and -output)
 */
void getOptionsMap(cli::framework::StringMap &optionsMap)
{
	optionsMap["-a"] = "-all";
	optionsMap["-d"] = "-display";
	optionsMap["-w"] = "-wait";
	optionsMap["-f"] = "-force";
	optionsMap["-help"] = "-help";
	optionsMap["-h"] = "-help";
	optionsMap["-output"] = "-output";
	optionsMap["-o"] = "-output";
	optionsMap["-x"] = "-examine";
	optionsMap["-units"] = "-units";
}

cli::framework::TokenList  cli::framework::Framework::tokenize(const int tokenCount,
		const char *tokens[],
		CommandSpecList allCommandSpecs)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	// build up all verbs, options, targets, and properties:
	StringList verbs;
	StringList targets;
	StringList properties;
	StringList options;



	for (size_t c = 0; c < allCommandSpecs.size(); c++)
	{
		verbs.push_back(allCommandSpecs[c].verb);
		for (size_t t = 0; t < allCommandSpecs[c].targets.size(); t++)
		{
			targets.push_back(allCommandSpecs[c].targets[t].name);
		}

		for (size_t p = 0; p < allCommandSpecs[c].properties.size(); p++)
		{
			properties.push_back(allCommandSpecs[c].properties[p].name);
		}
		for (size_t o = 0; o < allCommandSpecs[c].options.size(); o++)
		{
			options.push_back(allCommandSpecs[c].options[o].name);
			// only options have abbreviations
			if (!allCommandSpecs[c].options[o].abr.empty())
			{
				options.push_back(allCommandSpecs[c].options[o].abr);
			}
		}
	}

	// Options come from built in framework option map and command specs
	cli::framework::StringMap optionsMap;
	getOptionsMap(optionsMap);
	Lexer lexer(verbs, targets, options, properties);

	TokenList result = lexer.tokenize(tokenCount, tokens);

	// convert potential option abbreviations to name
	for (size_t i = 0; i < result.size(); i++)
	{
		if (result[i].tokenType == TOKENTYPE_OPTION)
		{
			if (optionsMap.find(result[i].lexeme) != optionsMap.end())
			{
				result[i].lexeme = optionsMap[result[i].lexeme];
			}
		}
	}

	return result;
}

std::string cli::framework::Framework::tokenArrayToString(const int tokenCount, const char* tokens[])
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	std::string result = "";
	for (int i = 0; i < tokenCount; i++)
	{
		result += std::string(tokens[i]) + " ";
	}
	return result;
}

std::string cli::framework::Framework::tokenArrayToString(TokenList tokens)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	std::stringstream result;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		result << tokens[i].lexeme << "[" << tokenTypeToString(tokens[i].tokenType) << "]" << " ";
	}
	return result.str();
}



/*
 * Register the specified feature instance by adding it to the common feature list
 */
void cli::framework::Framework::registerFeature(std::string featurename, cli::framework::FeatureBase *featureInst)
{
	// add to master feature list
	m_featureList.insert(std::pair<std::string, cli::framework::FeatureBase*>(featurename, featureInst));
}

/*
 * Remove a particular feature from the list & free it's resources
 */
void cli::framework::Framework::removeFeature(std::string featureName)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	featureList::iterator featureIttr = m_featureList.begin();
	for (; featureIttr != m_featureList.end(); featureIttr++)
	{
		if (0 == featureName.compare(featureIttr->first))
		{
			delete featureIttr->second;
			m_featureList.erase(featureIttr);
		}
	}
}

/*
  clear the Feature Registry list and free resources
 */
void cli::framework::Framework::clearFeatureList()
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	featureList::iterator featureIttr = m_featureList.begin();
	for (; featureIttr != m_featureList.end(); featureIttr++)
	{
		delete featureIttr->second;
	}

	m_featureList.clear();
}

cli::framework::CommandSpecList cli::framework::Framework::getRegisteredCommands()
{
	CommandSpecList result;
	Trace trace(__FILE__, __FUNCTION__, __LINE__);

	featureList::iterator featureIttr = m_featureList.begin();
	for (; featureIttr != m_featureList.end(); featureIttr++)
	{
		FeatureBase *featureInst = featureIttr->second;

		cli::framework::CommandSpecList commands;
		featureInst->getPaths(commands);

		for (size_t i = 0; i < commands.size(); i++)
		{
			// add default options to the command.
			CommandSpec &command = commands[i];

			addDefaultOptions(command);
			command.setFeature(featureIttr->second);

			result.push_back(command);
		}
	}

	return result;
}

void cli::framework::Framework::addDefaultOptions(cli::framework::CommandSpec &command)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	command.addOption(cli::framework::OPTION_HELP);
	command.addOption(cli::framework::OPTION_OUTPUT);
}


cli::framework::FeatureBase *cli::framework::Framework::getCommandFeature(const CommandSpec &command)
{
	Trace trace(__FILE__, __FUNCTION__, __LINE__);
	return (cli::framework::FeatureBase *)command.getFeature();
}
