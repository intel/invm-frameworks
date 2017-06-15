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


#include "IntelWmiProviderFactory.h"
#include "IntelWmiProvider.h"
#include <logger/Trace.h>

/*
 *
 * IntelProviderFactory::IntelProviderFactory
 * IntelProviderFactory::~IntelProviderFactory
 *
 * Constructor Parameters:
 *  None
 */
wbem::wmi::IntelWmiProviderFactory::IntelWmiProviderFactory()
{
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
	m_cRef=0L;
}

wbem::wmi::IntelWmiProviderFactory::~IntelWmiProviderFactory(void)
{
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
}

/*
 * QueryInterface, AddRef, & Release are standard Ole routines needed for all interfaces
 */
STDMETHODIMP wbem::wmi::IntelWmiProviderFactory::QueryInterface(REFIID riid,  LPVOID * ppv)
{	
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
	*ppv=NULL;
	HRESULT rc = E_NOINTERFACE;
	if (IID_IUnknown==riid || IID_IClassFactory==riid)
	{
		*ppv=this;
	}

	if (NULL != *ppv)
	{
		((LPUNKNOWN)*ppv)->AddRef();
		rc = NOERROR;
	}

	return rc;
}

STDMETHODIMP_(ULONG) wbem::wmi::IntelWmiProviderFactory::AddRef(void)
{
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
	m_cRef++;
	return m_cRef;
}

STDMETHODIMP_(ULONG) wbem::wmi::IntelWmiProviderFactory::Release(void)
{
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
	ULONG nNewCount = InterlockedDecrement((long *)&m_cRef);
	if (0L == nNewCount)
	{
		delete this;
	}
	return nNewCount;
}

/*
 * Instantiates a Locator object
 * and returns an interface pointer.
*/
STDMETHODIMP wbem::wmi::IntelWmiProviderFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,  LPVOID *ppvObj)
{
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
	IntelWmiProvider *pObj;
	HRESULT hr = 0;
	*ppvObj = NULL;

	// This object does not support aggregation.
	if (NULL!=pUnkOuter)
	{
		hr = CLASS_E_NOAGGREGATION;
	}
	else
	{
		// Create the locator object.
		pObj = new IntelWmiProvider();
		if (NULL == pObj)
		{
			hr = E_OUTOFMEMORY;
		}
		else
		{
			hr = pObj->QueryInterface(riid, ppvObj);

			//Kill the object if initial creation or Init failed.
			if (FAILED(hr))
			{
				delete pObj;
			}
		}
	}

	return hr;
}

/*
 *  Increments or decrements the lock count of the DLL.  If the
 *  lock count goes to zero and there are no objects, the DLL
 *  is allowed to unload.  See DllCanUnloadNow.
 */
STDMETHODIMP wbem::wmi::IntelWmiProviderFactory::LockServer(BOOL fLock)
{
	wbem::framework::Trace logging(__FILE__, __FUNCTION__, __LINE__);
	if (fLock)
	{
		InterlockedIncrement(&g_cLock);
	}
	else
	{
		InterlockedDecrement(&g_cLock);
	}
	return NOERROR;
}

// Called from the product side when Ole tries to determine if the DLL can be freed
SCODE wbem::wmi::IntelWmiProviderFactory::CimFrameworkDLLCanUnloadNow(void)
{
	return DllCanUnloadNow();
}
