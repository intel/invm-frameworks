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
 * This file contains common types used in the wbem library.
*/


#ifndef	_WBEM_FRAMEWORK_TYPES_H_
#define	_WBEM_FRAMEWORK_TYPES_H_

#include <vector>
#include <string>

namespace wbem
{
namespace framework
{

/*!
 * Base WBEM library return codes
 */
enum return_codes
{
	NOTFOUND = -1,
	SUCCESS = 0,//!< The method succeeded.
	FAIL = 1,    //!< The method failed.
	REQUIRES_FORCE = 2,    //!< The method failed, but could succeed with a force option.
};

typedef bool BOOLEAN; //!< Boolean.
typedef char SINT8; //!< 8 bit signed integer.
typedef unsigned char UINT8; //!< 8 bit unsigned integer.
typedef short SINT16; //!< 16 bit signed integer.
typedef unsigned short UINT16; //!< 16 bit unsigned integer.
typedef int SINT32;//!< 32 bit signed integer.
typedef unsigned int UINT32;//!< 32 bit unsigned integer.
typedef long long SINT64;//!< 64 bit signed integer.
typedef unsigned long long UINT64;//!< 64 bit unsigned integer.
typedef float REAL32;//!< 32 bit floating point number.
typedef std::string STR; //!< std::string.
typedef std::vector<UINT8> UINT8_LIST; //!< A vector of 8 bit unsigned integers.
typedef std::vector<UINT16> UINT16_LIST; //!< A vector of 16 bit unsigned integers.
typedef std::vector<UINT32> UINT32_LIST; //!< A vector of 32 bit unsigned integers.
typedef std::vector<UINT64> UINT64_LIST; //!< A vector of 64 bit unsigned integers.
typedef std::vector<STR> STR_LIST; //!< A vector of strings.
typedef std::vector<BOOLEAN> BOOLEAN_LIST; //!< A vector of booleans.

}
}
#endif  // _WBEM_FRAMEWORK_TYPES_H_
