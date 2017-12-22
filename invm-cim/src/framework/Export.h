/*
* Copyright (c) 2015 2016 2017, Intel Corporation
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


#ifndef	_WBEM_EXPORTS_H_
#define	_WBEM_EXPORTS_H_

#ifdef __WINDOWS__ // Windows
#define	INVM_CIM_DLL_IMPORT __declspec(dllimport)
#define	INVM_CIM_DLL_EXPORT __declspec(dllexport)
#else // Linux/ESX
#define	INVM_CIM_DLL_IMPORT __attribute__((visibility("default")))
#define	INVM_CIM_DLL_EXPORT __attribute__((visibility("default")))
#endif // end Linux/ESX

// INVM_CIM_API is used for the public API symbols.
#ifdef	__INVM_CIM_DLL__ // defined if compiled as a DLL
#ifdef	__INVM_CIM_DLL_EXPORTS__ // defined if we are building the DLL (instead of using it)
#define	INVM_CIM_API INVM_CIM_DLL_EXPORT
#else
#define	INVM_CIM_API INVM_CIM_DLL_IMPORT
#endif // __INVM_CIM_DLL_EXPORTS__
#else // INVM_CIM_API is not defined, everything is exported
#define	INVM_CIM_API
#endif // INVM_CIM_API

#endif // _WBEM_EXPORTS_H_