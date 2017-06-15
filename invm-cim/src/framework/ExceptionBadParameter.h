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
 * This file contains the definition of the exception class for an invalid parameter.
 */

#ifndef	_WBEM_FRAMEWORK_EXCEPTION_BADPARAMETER_H_
#define	_WBEM_FRAMEWORK_EXCEPTION_BADPARAMETER_H_

#include "Exception.h"

namespace wbem
{
namespace framework
{

/*!
 * An exception for an invalid input parameter
 */
class ExceptionBadParameter: public Exception
{
	public:
	/*!
	 * Initialize a bad parameter exception
	 * @param[in] pParameter
	 * 		The name of the input parameter that was bad.
	 */
	ExceptionBadParameter(const char *pParameter);
	~ExceptionBadParameter() throw () {}

	/*
	 * return the bad parameter that this exception was thrown with
	 */
	std::string getBadParameter();

	private:
		std::string m_badParameter;
};

} // framework
} // wbem

#endif // _WBEM_FRAMEWORK_EXCEPTION_BADPARAMETER_H_
