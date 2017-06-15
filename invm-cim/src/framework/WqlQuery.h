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
 * This file contains a class to process WQL strings into values
 * meaningful for the Intel WBEM library.
 */

#ifndef WQLQUERY_H_
#define WQLQUERY_H_

#include <string>
#include <vector>

#include "Exception.h"
#include "WqlConditional.h"

namespace wbem
{
namespace framework
{
/*!
 * Constants for WQL keywords
 */
const std::string WQL_SELECT = "SELECT";
const std::string WQL_SELECT_ALL = "*";
const std::string WQL_FROM = "FROM";
const std::string WQL_WHERE = "WHERE";

class WqlQuery
{
	public:

		/*!
		 * Construct the query object.
		 * @param query - the query string
		 * @throw NvmException if the query string is invalid
		 */
		WqlQuery(const std::string &query) throw (Exception);

		/*!
		 * Copy constructor
		 */
		WqlQuery(const WqlQuery &query);

		/*!
		 * Destroy the query object.
		 */
		virtual ~WqlQuery();

		/*!
		 * Assignment operator
		 */
		WqlQuery &operator=(const WqlQuery &query);

		/*!
		 * Get the string that generated this query.
		 */
		std::string getQueryString() const { return m_query; }

		/*!
		 * Get the class name to be searched by the query.
		 */
		const std::string& getClassName() const { return m_className; }

		/*!
		 * Get the class attributes to be returned by the query.
		 */
		const attribute_names_t& getSelectedAttributes() const { return m_attributes; }

		/*!
		 * Get the conditional, if any.
		 * @note if there is no conditional, returns NULL
		 */
		const WqlConditional* getConditional() const { return m_pConditional; }

		/*!
		 * Returns true if the string is a keyword in a WQL query.
		 */
		static bool isWqlKeyword(const std::string &value);

		/*!
		 * Returns true if the string could be used as a CIM class name.
		 * @remark a CIM class name must start with a letter
		 */
		static bool isValidCimClassName(const std::string &value);

		/*!
		 * Returns true if the string could be used as a CIM class property name.
		 */
		static bool isValidCimName(const std::string &value);

	protected:
		std::string m_query; //!< query as a string

		// dissected pieces of the parsed query
		std::string m_className; //!< name of CIM class to query
		attribute_names_t m_attributes; //!< attributes to select
		WqlConditional *m_pConditional; //!< conditions under which to select the instance

		/*
		 * Initialize the internal class members from the query string.
		 * @param query - query string
		 * @throw NvmException - if the query string is invalid
		 */
		void initFromQuery(const std::string &query) throw (Exception);

		/*
		 * Parse the query string and return relevant values as strings.
		 * @param query - query string
		 * @param className - returns the CIM class name as a string
		 * @param selectTokens - returns the select value(s) tokenized on spaces
		 * @param conditionalTokens - returns the conditional tokenized on spaces
		 * @throw NvmException - if the query string is invalid
		 */
		void parse(const std::string &query,
				std::string &className,
				std::vector<std::string> &selectTokens,
				std::vector<std::string> &conditionalTokens) const
			throw (Exception);

		/*
		 * Processes a string and saves it as the class name member variable.
		 * @remark This method does not validate whether the string refers to a real CIM class,
		 * 		just that it looks like it could.
		 * @param name - class name
		 * @throw NvmException - if the string is invalid
		 */
		void processClassName(const std::string &name)
			throw (Exception);

		/*
		 * Processes a vector of whitespace-separated tokens into a list of attribute names,
		 * saved as a member variable.
		 * @param attrTokens
		 * @throw NvmException - if the strings are invalid
		 */
		void processSelectAttributes(const std::vector<std::string> &attrTokens)
			throw (Exception);

		/*
		 * Processes a list of whitespace-separated tokens into a conditional,
		 * allocated and saved as a member variable.
		 * @param conditionalTokens
		 * @throw NvmException - if the conditional is formatted incorrectly
		 */
		void processConditional(const std::vector<std::string>& conditionalTokens)
			throw (Exception);
};

} /* namespace framework */
} /* namespace wbem */

#endif /* WQLQUERY_H_ */
