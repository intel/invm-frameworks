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
 * This file contains a class to represent a WQL conditional statement.
 */

#ifndef WQLCONDITIONAL_H_
#define WQLCONDITIONAL_H_

#include <string>
#include <vector>

#include "Attribute.h"
#include "Exception.h"

namespace wbem
{
namespace framework
{
/*!
 * Constants for WQL conditional operators
 */
const std::string WQL_AND = "AND";
const std::string WQL_OR = "OR";
const std::string WQL_EQ = "=";
const std::string WQL_GT = ">";
const std::string WQL_LT = "<";
const std::string WQL_GE = ">=";
const std::string WQL_LE = "<=";
const std::string WQL_NE1 = "!=";
const std::string WQL_NE2 = "<>";

/*!
 * Constants for WQL boolean values
 */
const std::string WQL_TRUE = "TRUE";
const std::string WQL_FALSE = "FALSE";

/*!
 * Operators in a comparison clause
 */
enum ComparisonOperator
{
	OP_EQ, //!< equals
	OP_GT, //!< greater than
	OP_LT, //!< less than
	OP_GE, //!< greater than or equal
	OP_LE, //!< less than or equal
	OP_NE //!< not equal
};

/*!
 * Represents a comparison between an instance attribute and a value
 */
struct WqlComparisonClause
{
	std::string attributeName; //!< instance attribute name
	Attribute value; //!< comparison value
	ComparisonOperator op; //!< what kind of comparison to perform
};

/*!
 * Represents a conditional expression in a WQL query.
 * @remark for now, we do not deal with the OR operators.
 */
class WqlConditional
{
	public:
		/*!
		 * Constructor.
		 * @param tokens - list of tokens
		 * @throw NvmException - if the expression is invalid
		 */
		WqlConditional(const std::vector<std::string> &tokens)
			throw (Exception);

		/*!
		 * Constructor.
		 * @param str - conditional as a string
		 * @throw NvmException - if the expression is invalid
		 */
		WqlConditional(const std::string &str) throw (Exception);

		/*!
		 * Copy constructor
		 */
		WqlConditional(const WqlConditional &cond);

		/*!
		 * Destructor
		 */
		~WqlConditional();

		/*!
		 * Assignment operator.
		 */
		WqlConditional &operator=(const WqlConditional &cond);

		/*!
		 * Returns the list of comparison clauses.
		 */
		const std::vector<struct WqlComparisonClause> &getConditions() const
		{ return m_conditions; }

		/*!
		 * Break apart whitespace-delimited token into tokens meaningful for conditional.
		 * A whitespace-delimited token may contain multiple conditional tokens not
		 * separated by whitespace.
		 * @param token - whitespace-delimited token
		 * @param openQuotes - boolean reference to track open quotes
		 * @param quoteType - type of quote mark to match if quotes are open
		 * @return vector of separated conditional tokens
		 */
		static std::vector<std::string> dissectConditionalToken(
				const std::string& token, bool &openQuotes, char &quoteType);

		/*!
		 * Fetch a string representation of the conditional.
		 */
		std::string getString() const { return m_str; }

	protected:
		//!< These are assumed to be connected by AND operators
		std::vector<struct WqlComparisonClause> m_conditions;
		std::string m_str; //!< full conditional string

		/*
		 * Initialize the internal comparison clause
		 * @param tokens - list of tokens
		 * @throw NvmException - if the tokens are invalid
		 */
		void initFromTokens(const std::vector<std::string> &tokens)
			throw (Exception);
};

} /* namespace framework */
} /* namespace wbem */

#endif /* WQLCONDITIONAL_H_ */
