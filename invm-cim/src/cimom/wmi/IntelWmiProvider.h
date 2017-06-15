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
 * This file contains the IntelWmiProvider class which maps Intel WBEM to WMI.
 */

#ifndef _INTELWMIPROVIDER_H_
#define _INTELWMIPROVIDER_H_

#include <wbemprov.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <logger/logging.h>
#include <framework/Attribute.h>
#include <framework/Instance.h>
#include <framework/InstanceFactory.h>
#include "wmiUtilities.h"

extern long g_cObj; //!< keep track of when the module can be unloaded
extern long g_cLock; //!< keep track of when the module can be unloaded

namespace wbem
{
namespace wmi
{
/*!
 * Provider interfaces are provided by objects of this class.
 *
 * @details
 * 		This class overrides methods from the IWbemServices, IWbemProviderInit and IUnknown
 * 		interfaces.  See the related links for more details about each interface.
 *		IUnknown - http://msdn.microsoft.com/en-us/library/windows/desktop/ms680509(v=vs.85).aspx
 *		IWbemProviderInit - http://msdn.microsoft.com/en-us/library/windows/desktop/aa391858(v=vs.85).aspx
 *		IWbemServices - http://msdn.microsoft.com/en-us/library/windows/desktop/aa392093(v=vs.85).aspx
 */
class IntelWmiProvider : public IWbemServices, public IWbemProviderInit, public IWbemEventProvider, public IWbemEventProviderQuerySink
{
protected:
	ULONG m_cRef; //!< Object reference count
	IWbemServices *m_pNamespace; //!< namespace

public:
	/*!
	 * Constructs an IntelWmiProvider
	 * @param ObjectPath
	 * 		[Optional]Path of requested object
	 * @param User
	 * 		[Optional] Username for connection
	 * @param Password
	 * 		[Optional] Password for connection
	 * @param pCtx
	 * 		[Optional] An instance of the IWbemContext.
	 */
	IntelWmiProvider(BSTR ObjectPath = NULL, BSTR User = NULL, BSTR Password = NULL,
			IWbemContext * pCtx=NULL);
	virtual ~IntelWmiProvider(void);

	/*
	 * **********************
	 * IUnkown
	 * **********************
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
	 * **********************
	 * IWbemProviderInit
	 * **********************
	 */
	/*!
	 * Initializes the WMI Provider
	 * @details
	 * 		http://msdn.microsoft.com/en-us/library/windows/desktop/aa391858(v=vs.85).aspx
	 * @param[in] pszUser
	 * 		A pointer to the user name, if per-user initialization was requested
	 * 		in the __Win32Provider registration instance for this provider. Otherwise, this is NULL.
	 * @param[in] lFlags
	 * 		Reserved. This parameter must be 0 (zero).
	 * @param[in] pszNamespace
	 * 		A namespace name for which the provider is initialized.
	 * @param[in] pszLocale
	 * 		Locale name for which the provider is being initialized.
	 * @param[in] pNamespace
	 * 		An IWbemServices pointer back into Windows Management.
	 * @param[in] pCtx
	 * 		An IWbemContext pointer associated with initialization.
	 * @param[in] pInitSink
	 *		An IWbemProviderInitSink pointer that is used by the provider to report initialization status.
	 * @return
	 * 		Success or failure
	 */
	HRESULT STDMETHODCALLTYPE Initialize(
			LPWSTR pszUser,
			LONG lFlags,
			LPWSTR pszNamespace,
			LPWSTR pszLocale,
			IWbemServices *pNamespace,
			IWbemContext *pCtx,
			IWbemProviderInitSink *pInitSink
	);

	/*
	 * **********************
	 * IWbemServices
	 * **********************
	 */

	/*!
	 * Get an instance by object path
	 * @param[in] Path
	 * 		the object path
	 * @param[out] pObj
	 * 		Pointer that will point to the memory where the object will be created
	 * @param[in] pCtx
	 * @return
	 * 		Success or error code
	 */
	SCODE GetByPath(BSTR Path, IWbemClassObject FAR* FAR* pObj, IWbemContext  *pCtx);

	/*!
	 * Asynchronously retrieves an object�an instance or class definition.
	 * @details
	 * 		http://msdn.microsoft.com/en-us/library/windows/desktop/aa392110(v=vs.85).aspx
	 * @param[in] ObjectPath
	 *		Path of the object to retrieve
	 * @param[in] lFlags
	 * 		See website reference for possible flags
	 * @param[in] pCtx
	 * 		Typically NULL
	 * @param[in] pResponseHandler
	 * 		Pointer to the caller's implementation of IWbemObjectSink
	 * @return
	 * 		This method returns an HRESULT that indicates the status of the method call
	 *
	 */
	HRESULT STDMETHODCALLTYPE GetObjectAsync(
			const BSTR ObjectPath,
			long lFlags,
			IWbemContext __RPC_FAR *pCtx,
			IWbemObjectSink __RPC_FAR *pResponseHandler);

	/*!
	 * Creates an instance enumerator that executes asynchronously
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa392098(v=vs.85).aspx
	 * @param[in] Class
	 * 		Name of class enumerating
	 * @param[in] lFlags
	 *		see web link for options
	 * @param[in] pCtx
	 * 		Typically NULL
	 * @param[in] pResponseHandler
	 * 		Pointer to the caller's implementation of IWbemObjectSink
	 * @return
	 * 		This method returns an HRESULT indicating the status of the method call.
	 */
	HRESULT STDMETHODCALLTYPE CreateInstanceEnumAsync(
			/* [in] */ const BSTR Class,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler);

	/*!
	 * Asynchronously creates or updates an instance of an existing class.
	 * http://msdn.microsoft.com/en-us/library/aa392116(v=vs.85).aspx
	 * @param[in] pInst
	 * 		Pointer to the instance to be written to the WMI repository
	 * @param[in] lFlags
	 * 		Ignored
	 * @param[in] pCtx
	 * 		Ignored
	 * @param[in] pResponseHandler
	 * 		Pointer to the caller's implementation of IWbemObjectSink
	 * @return
	 */
	HRESULT STDMETHODCALLTYPE PutInstanceAsync(
			IWbemClassObject __RPC_FAR *pInst,
			long lFlags,
			IWbemContext __RPC_FAR *pCtx,
			IWbemObjectSink __RPC_FAR *pResponseHandler);

	/*!
	 * Utility method for getting an NvmInstanceFactory from a class name
	 * @param className
	 * 		The class name.
	 * @return
	 * 		Pointer to the instance factory.  If the class name is not found, NULL is returned.
	 */
	wbem::framework::InstanceFactory *GetFactory(const std::string className);


	/*
	 * *************************************************************************************
	 * The following functions are required to be overriden, but not required currently to be
	 * implemented, so they all return Not Supported.
	 * *************************************************************************************
	 */
	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE OpenNamespace(
			/* [in] */ const BSTR Namespace,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [unique][in][out] */ IWbemServices __RPC_FAR *__RPC_FAR *ppWorkingNamespace,
			/* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppResult)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE CancelAsyncCall(
			/* [in] */ IWbemObjectSink __RPC_FAR *pSink)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE QueryObjectSink(
			/* [in] */ long lFlags,
			/* [out] */
			IWbemObjectSink __RPC_FAR *__RPC_FAR *ppResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE GetObject(
			/* [in] */ const BSTR ObjectPath,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [unique][in][out] */
			IWbemClassObject __RPC_FAR *__RPC_FAR *ppObject,
			/* [unique][in][out] */
			IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE PutClass(
			/* [in] */ IWbemClassObject __RPC_FAR *pObject,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [unique][in][out] */
			IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE PutClassAsync(
			/* [in] */ IWbemClassObject __RPC_FAR *pObject,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE DeleteClass(
			/* [in] */ const BSTR Class,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [unique][in][out] */
			IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE DeleteClassAsync(
			/* [in] */ const BSTR Class,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE CreateClassEnum(
			/* [in] */ const BSTR Superclass,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [out] */
			IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE CreateClassEnumAsync(
			/* [in] */ const BSTR Superclass,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE PutInstance(
			/* [in] */ IWbemClassObject __RPC_FAR *pInst,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [unique][in][out] */
			IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE DeleteInstance(
			/* [in] */ const BSTR ObjectPath,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [unique][in][out] */
			IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE DeleteInstanceAsync(
			/* [in] */ const BSTR ObjectPath,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE CreateInstanceEnum(
			/* [in] */ const BSTR Class,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [out] */
			IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE ExecQuery(
			/* [in] */ const BSTR QueryLanguage,
			/* [in] */ const BSTR Query,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [out] */
			IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE ExecQueryAsync(
			/* [in] */ const BSTR QueryLanguage,
			/* [in] */ const BSTR Query,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE ExecNotificationQuery(
			/* [in] */ const BSTR QueryLanguage,
			/* [in] */ const BSTR Query,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [out] */
			IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE ExecNotificationQueryAsync(
			/* [in] */ const BSTR QueryLanguage,
			/* [in] */ const BSTR Query,
			/* [in] */ long lFlags,
			/* [in] */ IWbemContext __RPC_FAR *pCtx,
			/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
	{return WBEM_E_NOT_SUPPORTED;};

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE ExecMethod( const BSTR,
			const BSTR,
			long,
			IWbemContext*,
			IWbemClassObject*,
			IWbemClassObject**,
			IWbemCallResult**)
	{
		LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
		return WBEM_E_NOT_SUPPORTED;
	}

	/*!
	 * Not Supported
	 */
	HRESULT STDMETHODCALLTYPE ExecMethodAsync( const BSTR,
			const BSTR,
			long,
			IWbemContext*,
			IWbemClassObject*,
			IWbemObjectSink*);


	/*
	 * **********************
	 * IWbemEventProvider
	 * **********************
	 */

	/*!
	 * Signal event provider to begin delivery of events
	 * http://msdn.microsoft.com/en-us/library/aa391744(v=vs.85).aspx
	 * @param[in] pSink
	 * 		Pointer to the object sink to which the provider will deliver its events.
	 * @param[in] lFlags
	 * 		Ignored
	 * @return
	 * 	 WBEM_E_FAILED or WBEM_S_NO_ERROR
	 */
	HRESULT ProvideEvents(IWbemObjectSink *pSink,long lFlags);

	HRESULT NewQuery(unsigned long dwId, WBEM_WSTR wszQueryLanguage, WBEM_WSTR wszQuery);

	HRESULT CancelQuery(unsigned long dwId);

private:
	SCODE InstancesToWmi(
			const wbem::framework::Instance *pInstance,
			IWbemClassObject * pClass,
			IWbemClassObject FAR** pNewInst );
	HRESULT STDMETHODCALLTYPE AttributeToVariant(wbem::framework::Attribute &atttribute,
			VARIANT &v);

	void addMethodReturnCodeToReturnObject(
			const char *className, const BSTR strMethodName, IWbemContext *pContext,
			IWbemObjectSink *pResponseHandler, wbem::framework::UINT32 &wbemRc);

	HRESULT convertHttpRcToWmiRc(wbem::framework::UINT32 httpRc);

	template <class T>
	SAFEARRAY *ListToSafeArray(const std::vector<T> &list);

	int m_queryCount;
	bool m_eventsEnabled;
	IWbemContext *m_pWbemContext;
};
}
}


#endif
