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
 * This file contains the definition of the exception class
 * for an invalid WQL query string.
 */


#ifndef EXCEPTIONINVALIDWQLQUERY_H_
#define EXCEPTIONINVALIDWQLQUERY_H_

#include <string>
#include "Exception.h"
#include "Types.h"

namespace wbem
{
namespace framework
{

/*!
 * Message strings for reason values
 */
const std::string INVALIDWQL_REASONUNKNOWN = "The query string is invalid for an unknown reason.";
const std::string INVALIDWQL_REASONBADLYFORMED = "The query structure is badly formed.";
const std::string INVALIDWQL_REASONBADCLASSNAME = "The query parser expected a CIM class name but found: ";
const std::string INVALIDWQL_REASONBADATTR = "The query parser expected an attribute but found: ";
const std::string INVALIDWQL_REASONBADOPERATOR = "The query parser expected an operator but found: ";
const std::string INVALIDWQL_REASONBADVALUE = "The query parser expected a value of a valid type but found: ";
const std::string INVALIDWQL_REASONUNMATCHEDQUOTES = "The query string contains unmatched or mismatched quote marks.";
const std::string INVALIDWQL_REASONUNMATCHEDPARENS = "The query string contains unmatched parentheses.";

/*!
 * An exception for an invalid WQL query string
 */
class ExceptionInvalidWqlQuery : public Exception
{
	public:
		/*!
		 * Reasons a WQL string could be invalid
		 */
		enum InvalidWqlReason
		{
			REASON_UNKNOWN, //!< couldn't determine a reason
			REASON_BADLYFORMED, //!< query keyword structure is badly formed
			REASON_BADCLASSNAME, //!< invalid class name
			REASON_BADATTR, //!< attribute was an invalid value
			REASON_BADOPERATOR, //!< operator is invalid or not supported
			REASON_BADVALUE, //!< value was an invalid type or specified incorrectly
			REASON_UNMATCHEDQUOTES, //!< unmatched or mismatched quote marks
			REASON_UNMATCHEDPARENS //!< unmatched parentheses
		};

		/*!
		 * Constructor for the exception.
		 * @param reason - reason the query was considered invalid
		 * @param token - optional - the token that caused the issue
		 */
		ExceptionInvalidWqlQuery(const enum InvalidWqlReason reason, const std::string &token = "");

		/*!
		 * Destructor
		 */
		virtual ~ExceptionInvalidWqlQuery() throw ();

		/*!
		 * Fetch the reason value.
		 * @return InvalidWqlReason
		 */
		enum InvalidWqlReason getReason() { return m_reason; }

		/*!
		 * Fetch the bad token.
		 * @return string if one was set, empty string if not set
		 */
		std::string getToken() { return m_token; }

	protected:
		enum InvalidWqlReason m_reason; //!< reason the query was invalid
		std::string m_token; //!< bad token - optional
};

} /* namespace framework */
} /* namespace wbem */

#endif /* EXCEPTIONINVALIDWQLQUERY_H_ */
