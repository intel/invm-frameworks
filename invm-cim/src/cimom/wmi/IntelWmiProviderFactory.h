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
 * This file contains the IntelProviderFactory class to provide an instance of the IntelWmiProvider.
 * It's the entry point and pass-through for Microsoft's Component Object Model (COM) system.
 * Documentation at http://msdn.microsoft.com/en-us/library/windows/desktop/aa390359(v=vs.85).aspx
 */

#ifndef _CPROVFACTORY_H
#define _CPROVFACTORY_H

#include <wbemprov.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

namespace wbem
{
namespace wmi
{

/*!
 * Implements the IClassFactory and IUnknown interfaces.
 * @details
 * This class makes it possible to register (regsvr32.exe) the IntelWmiProvider within the
 * Windows OS.
 *
 */
class IntelWmiProviderFactory : public IClassFactory
{
protected:
	ULONG m_cRef; //!< maintain count of references

public:
	/*!
	 * Constructor
	 */
	IntelWmiProviderFactory(void);
	/*!
	 * Destructor
	 */
	virtual ~IntelWmiProviderFactory(void);

	/*
	 * *******************************************************************************
	 * IUnknown membersmembers
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms680509(v=vs.85).aspx
	 * *******************************************************************************
	 */
	/*!
	 * Retrieves pointers to the supported interfaces on an object.
	 * @details
	 * 		http://msdn.microsoft.com/en-us/library/windows/desktop/ms682521(v=vs.85).aspx
	 * @param[in] riid
	 * 		The identifier of the interface being requested.
	 * @param[out] ppvObject
	 * 		The address of a pointer variable that receives the interface pointer requested in the riid parameter.
	 * @return
	 * 		This method returns S_OK if the interface is supported, and E_NOINTERFACE otherwise.
	 * 		If ppvObject is NULL, this method returns E_POINTER.
	 */

	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject );
	/*!
	 * Increments the reference count for an interface on an object.
	 * @details
	 * 		http://msdn.microsoft.com/en-us/library/windows/desktop/ms691379(v=vs.85).aspx
	 * @return
	 * 		The method returns the new reference count.
	 * 		This value is intended to be used only for test purposes.
	 */
	STDMETHODIMP_(ULONG) AddRef(void);

	/*!
	 * Decrements the reference count for an interface on an object.
	 * @details
	 * 		http://msdn.microsoft.com/en-us/library/windows/desktop/ms682317(v=vs.85).aspx
	 * @return
	 * 		The method returns the new reference count.
	 * 		This value is intended to be used only for test purposes.
	 */
	STDMETHODIMP_(ULONG) Release(void);

	/*
	 * *******************************************************************************
	 * IClassFactory members
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms694364(v=vs.85).aspx
	 * *******************************************************************************
	 */
	/*!
	 * Creates an uninitialized object
	 */
	STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID *);

	/*!
	 * Locks an object application open in memory
	 * @details
	 *		http://msdn.microsoft.com/en-us/library/ms682332(v=vs.85).aspx
	 * @param[in] fLock
	 * 		If TRUE, increments the lock count; if FALSE, decrements the lock count.
	 * @return
	 * 		This method can return the standard return values
	 * 		E_OUTOFMEMORY, E_UNEXPECTED, E_FAIL, and S_OK.
	 */
	STDMETHODIMP LockServer(BOOL fLock);

	/*!
	 * Determine if the library can be unloaded.
	 * @details
	 * 		The framework maintains a lock and a count of active objects that are needed
	 *		 to determine if the DLL can be unloaded by OLE. This method is called from
	 * 		the product code to see if the library can be unloaded.
	 * @return
	 * 		This method can return S_OK or S_FALSE.
	 */
	static SCODE CimFrameworkDLLCanUnloadNow(void);
};
}
}

#endif
