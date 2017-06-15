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
 * This file contains the definition of the base Exception class for the
 * Intel WBEM library.
 */

#ifndef	_WBEM_FRAMEWORK_EXCEPTION_H_
#define	_WBEM_FRAMEWORK_EXCEPTION_H_

#include <exception>
#include <string>
#include <stdarg.h>
#include <stdio.h>

#include "Strings.h"

#define	ERROR_MESSAGE_LEN 1024 //!< The length of the exception error message

namespace wbem
{
namespace framework
{

/*!
 * Generic exception for the Wbem library.
 */
class Exception : public std::exception
{
	public:

		/*!
		 * Initialize a new empty exception.
		 */
		Exception();

		/*!
		 * Initialize a new exception with a message string.
		 * @param[in] message
		 * 		The message.
		 */
		Exception(const std::string &message);

		/*!
		 * Destructor cleans up
		 */
		virtual ~Exception() throw ();

		/*!
		 * Retrieve a text string describing the exception that occurred.
		 * @return
		 * 		The exception message.
		 */
		virtual const char* what() const throw();

	protected:
		/*!
		 * Log the exception to the debug log.
		 */
		void logDebugMessage();

		std::string m_Message; //!< The exception string message
};

} // framework
} // wbem

#endif // _WBEM_FRAMEWORK_NVMEXCEPTION_H_
