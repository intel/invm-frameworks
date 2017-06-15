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

#include "IntelWmiProvider.h"
#include "wmiUtilities.h"
#include "IntelToWmi.h"
#include "WmiAdapter.h"
#include <cimom/CimomAdapter.h>
#include <logger/logging.h>

#include <objbase.h>
#include <initguid.h>
#include <iostream>
#include <string>
#include <comutil.h>
#include <windows.h>
#include <vector>
#include <wbemcli.h>
#include <ntdef.h>
#include <framework/Attribute.h>
#include <framework/Exception.h>
#include <framework/ExceptionBadParameter.h>
#include <framework/ExceptionNoMemory.h>
#include <framework/ExceptionNotSupported.h>
#include <framework/ExceptionSystemError.h>
#include <framework/ObjectPathBuilder.h>
#include <framework/ProviderFactory.h>

/*
 * These would normally be defined in a Microsoft library (wbemuuid.cpp) that isn't available for MinGW, but the
 * correct value is documentated at http://msdn.microsoft.com/en-us/library/cc250705.aspx and
 * http://api.kde.org/kdesupport-api/kdewin-apidocs/wbemuuid_8cpp_source.html
 */
extern "C" const GUID IID_IWbemServices
	= { 0x9556dc99,0x828c,0x11cf,{ 0xa3,0x7e,0x00,0xaa,0x00,0x32,0x40,0xc7 } };
extern "C" const GUID IID_IWbemProviderInit
	= { 0x1be41572,0x91dd,0x11d1,{ 0xae,0xb2,0x00,0xc0,0x4f,0xb6,0x88,0x20 } };
extern "C" const GUID IID_IWbemEventProvider
	= { 0xe245105b,0xb06e,0x11d0,{ 0xad,0x61,0x00,0xc0,0x4f,0xd8,0xfd,0xff } };

extern "C" const GUID IID_IWbemEventProviderQuerySink
	= { 0x580acaf8,0xfa1c,0x11d0,{ 0xad,0x72,0x00,0xc0,0x4f,0xd8,0xfd,0xff } };
extern "C" const GUID IID_IWbemEventProviderSecurity
	= { 0x631f7d96,0xd993,0x11d2,{ 0xb3,0x39,0x00,0x10,0x5a,0x1f,0x4a,0xaf } };


/*
 * IntelWmiProvider::IntelWmiProvider
 * IntelWmiProvider::~IntelWmiProvider
 */
wbem::wmi::IntelWmiProvider::IntelWmiProvider(BSTR ObjectPath, BSTR User, BSTR Password, IWbemContext * pCtx)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	m_pNamespace = NULL;
	m_cRef=0;
	m_eventsEnabled = false;
	m_queryCount = 0;
	InterlockedIncrement(&g_cObj);

	return;
}

wbem::wmi::IntelWmiProvider::~IntelWmiProvider(void)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	InterlockedDecrement(&g_cObj);
	return;
}

/*
 * QueryInterface, AddRef, & Release are standard Ole routines needed for all interfaces
 */
STDMETHODIMP wbem::wmi::IntelWmiProvider::QueryInterface(REFIID riid, LPVOID * ppv)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT rc = NOERROR;
	*ppv=NULL;

	// Because you have multi inheritance,
	// it is necessary to cast the return type
	if (riid== IID_IWbemServices)
	{
		*ppv=(IWbemServices*)this;
	}
	else if (IID_IWbemEventProviderQuerySink == riid)
	{
		*ppv = (IWbemEventProviderQuerySink *)this;
	}
	else if (IID_IWbemEventProvider == riid)
	{
		*ppv = (IWbemEventProvider *)this;
	}
	else if (IID_IUnknown == riid || IID_IWbemProviderInit == riid)
	{
		*ppv=(IWbemProviderInit*)this;
	}

	if (NULL != *ppv)
	{
		AddRef();
		rc = NOERROR;
	}
	else
	{
		rc = E_NOINTERFACE;
	}
	return rc;
}

STDMETHODIMP_(ULONG) wbem::wmi::IntelWmiProvider::AddRef(void)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	m_cRef++;
	return m_cRef;
}

STDMETHODIMP_(ULONG) wbem::wmi::IntelWmiProvider::Release(void)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	ULONG nNewCount = InterlockedDecrement((long *)&m_cRef);
	if (0L == nNewCount)
	{
		// release references to the global pointers.
		m_pNamespace->Release();
		m_pNamespace = NULL;

		m_pWbemContext->Release();
		m_pWbemContext = NULL;

		delete this;
	}
	return nNewCount;
}

/*
 *
 * This is the implementation of IWbemProviderInit.
 * The method is required to initialize with CIMOM.
 *
 */
STDMETHODIMP wbem::wmi::IntelWmiProvider::Initialize(
		LPWSTR pszUser, LONG lFlags, LPWSTR pszNamespace,
		LPWSTR pszLocale, IWbemServices *pNamespace,
		IWbemContext *pCtx, IWbemProviderInitSink *pInitSink)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	WBEMSTATUS status = WBEM_S_NO_ERROR;

	if (!pNamespace)
	{
		status = WBEM_E_FAILED;
	}
	else
	{
		m_pNamespace = pNamespace;
		m_pNamespace->AddRef();

		m_pWbemContext = pCtx;
		m_pWbemContext->AddRef();
	}

	if (status == WBEM_S_NO_ERROR)
	{
		pInitSink->SetStatus(WBEM_S_INITIALIZED, 0);
	}
	else
	{
		COMMON_LOG_ERROR("Error Initializing");
		pInitSink->SetStatus(status , 0);
	}
	return status;
}

/*
 *	IntelWmiProvider::CreateInstanceEnumAsync
 *
 *	Purpose: Asynchronously enumerates the instances.
 */
SCODE wbem::wmi::IntelWmiProvider::CreateInstanceEnumAsync(
		const BSTR RefStr, long lFlags, IWbemContext *pCtx,
		IWbemObjectSink FAR* pHandler)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	SCODE sc = S_OK;

	if (pHandler == NULL)
	{
		sc = WBEM_E_INVALID_PARAMETER;
	}
	else
	{
		char *mofClass;
		mofClass = _com_util::ConvertBSTRToString(RefStr);
		if (NULL == mofClass || m_pNamespace == NULL)
		{
			// mofClass will be NULL if RefStr is not a valid BSTR
			sc = WBEM_E_INVALID_PARAMETER;
		}
		else
		{
			COMMON_LOG_DEBUG_F("CreateInstanceEnum for %s class\n", mofClass);

			HRESULT hr = Impersonate();
			if (FAILED(hr))
			{
				sc = hr;
			}
			else
			{
				wbem::framework::ProviderFactory *pProviderFactory =
						wbem::framework::ProviderFactory::getSingleton();
				if (pProviderFactory == NULL)
				{
					sc = WBEM_E_FAILED;
				}
				else
				{
					pProviderFactory->InitializeProvider();

					// get the instance factory ...
					wbem::framework::InstanceFactory *pFactory =
							pProviderFactory->getInstanceFactory(mofClass);
					if (pFactory == NULL)
					{
						sc = WBEM_E_INVALID_CLASS;
					}
					else
					{

						wbem::framework::instances_t *pInstances = NULL;
						try
						{
							wbem::framework::attribute_names_t attributes;
							pInstances = pFactory->getInstances(attributes);
							if (pInstances == NULL)
							{
								sc = WBEM_E_FAILED;
								COMMON_LOG_ERROR("Unable to get instances");
							}

							for (wbem::framework::instances_t::iterator instIter = pInstances->begin();
									instIter != pInstances->end() && SUCCEEDED(sc); instIter++)
							{
								IWbemClassObject FAR* pNewInst = NULL;
								sc = IntelToWmi::ToWmiInstance(&(*instIter), pCtx, m_pNamespace, &pNewInst);
								if (SUCCEEDED(sc))
								{
									pHandler->Indicate(1, &pNewInst);
									pNewInst->Release();
								}
							}
						}
						catch (wbem::framework::ExceptionNotSupported &)
						{
							sc = WBEM_E_INVALID_METHOD;
						}
						catch (wbem::framework::ExceptionBadParameter &)
						{
							sc = WBEM_E_INVALID_PARAMETER;
						}
						catch (wbem::framework::ExceptionNoMemory &)
						{
							sc = WBEM_E_OUT_OF_MEMORY;
						}
						catch (wbem::framework::Exception &)
						{
							sc = WBEM_E_PROVIDER_FAILURE;
						}
						if (pInstances)
						{
							delete pInstances;
							pInstances = NULL;
						}
						delete pFactory;
					}
					pProviderFactory->CleanUpProvider();
				}
				CoRevertToSelf();
			}
		}
		pHandler->SetStatus(0, sc, NULL, NULL);
	}
	return sc;
}

/*
 * Creates an instance given a particular path value.
 */
SCODE wbem::wmi::IntelWmiProvider::GetObjectAsync(const BSTR ObjectPath, long lFlags,
		IWbemContext  *pCtx, IWbemObjectSink FAR* pHandler)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	SCODE rc = S_OK;

	if (pHandler == NULL)
	{
		rc = WBEM_E_INVALID_PARAMETER;
	}
	// Do a check of arguments and ensure you have a pointer to Namespace
	else
	{
		if (ObjectPath == NULL || m_pNamespace == NULL)
		{
			rc = WBEM_E_INVALID_PARAMETER;
		}
		else
		{
			//Impersonate the client
			HRESULT hr = Impersonate();
			if (FAILED(hr))
			{
				rc = hr;
			}
			else
			{
				IWbemClassObject FAR* pObj;
				BOOL bOK = FALSE;

				wbem::framework::ProviderFactory *pProviderFactory =
						wbem::framework::ProviderFactory::getSingleton();
				if (pProviderFactory == NULL)
				{
					rc = WBEM_E_FAILED;
				}
				else
				{
					pProviderFactory->InitializeProvider();
					// do the get, pass the object on to the notify
					rc = GetByPath(ObjectPath, &pObj, pCtx);
					if (rc == S_OK)
					{
						pHandler->Indicate(1, &pObj);
						pObj->Release();
						bOK = TRUE;
					}

					rc = (bOK) ? S_OK : WBEM_E_NOT_FOUND;
					pProviderFactory->CleanUpProvider();
				}
				CoRevertToSelf();
			}
		}
		// Set Status
		pHandler->SetStatus(0,rc, NULL, NULL);
	}
	return rc;
}

/*
 * Creates an instance given a particular Path value.
 */
SCODE wbem::wmi::IntelWmiProvider::GetByPath
	(BSTR ObjectPath, IWbemClassObject FAR* FAR* ppObj, IWbemContext  *pCtx)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	SCODE rc = S_OK;

	// Parse ObjectPath into ObjectPath
	wbem::framework::ObjectPath objectpath;
	IntelToWmi::BstrToObjectPath(ObjectPath, &objectpath);

	std::string className = objectpath.getClass();

	wbem::framework::InstanceFactory *pFactory =
			wbem::framework::ProviderFactory::getInstanceFactoryStatic(className);
	if (pFactory == NULL)
	{
		rc = WBEM_E_INVALID_CLASS;
	}
	else
	{
		try
		{
			wbem::framework::attribute_names_t attributeNames;
			wbem::framework::Instance *pInstance = pFactory->getInstance(objectpath, attributeNames);
			if (pInstance == NULL)
			{
				rc = WBEM_E_NOT_FOUND;
			}
			else
			{
				BSTR classNameB = SysAllocString(convert::str_to_wstr(className).c_str());
				IWbemClassObject* pClass = NULL;
				rc = m_pNamespace->GetObject(classNameB, 0, pCtx, &pClass, NULL);
				if (rc != S_OK)
				{
					rc = WBEM_E_FAILED;
				}
				else
				{
					rc = IntelToWmi::ToWmiInstance(pInstance, pCtx, m_pNamespace, ppObj);
					pClass->Release();
				}
				delete pInstance;
			}
		}
		catch (wbem::framework::ExceptionBadParameter &)
		{
			rc = WBEM_E_INVALID_PARAMETER;
		}
		catch (wbem::framework::ExceptionSystemError &e)
		{
			rc = WBEM_E_PROVIDER_FAILURE;
		}
		catch (wbem::framework::ExceptionNoMemory &)
		{
			rc = WBEM_E_OUT_OF_MEMORY;
		}
		catch (wbem::framework::ExceptionNotSupported &)
		{
			rc = WBEM_E_NOT_SUPPORTED;
		}
		catch (wbem::framework::Exception &)
		{
			rc = WBEM_E_FAILED;
		}


		if (pFactory)
		{
			delete pFactory;
		}
	}
	return rc;
}

void wbem::wmi::IntelWmiProvider::addMethodReturnCodeToReturnObject(
		const char *className, const BSTR strMethodName, IWbemContext *pContext,
		IWbemObjectSink *pResponseHandler, wbem::framework::UINT32 &wbemRc)
{
	// Need output argument class to put the method result into
	// First, get the class instance
	IWbemClassObject * pClass = NULL;
	m_pNamespace->GetObject(_com_util::ConvertStringToBSTR(className), 0, pContext, &pClass, NULL);

	// Get an instance for the output parameters class
	IWbemClassObject * pOutClass = NULL;
	pClass->GetMethod(strMethodName, 0, NULL, &pOutClass);

	// Get an instance for the actual output parameters
	IWbemClassObject* pOutParams;
	pOutClass->SpawnInstance(0, &pOutParams);

	// build the VARIANT that will represent the "ReturnValue" of the method. Will always be a CIM_UINT32
	VARIANT var;
	VariantInit(&var);
	VariantClear(&var);
	var.vt = VT_I4;
	var.lVal = (long)wbemRc;
	BSTR retValName = SysAllocString(L"ReturnValue");

	// add to output parameters
	pOutParams->Put(retValName , 0, &var, CIM_UINT32);

	// indicate output parameters
	pResponseHandler->Indicate(1, &pOutParams);

	// Clean up
	pOutParams->Release();
	pOutClass->Release();
	pClass->Release();
	SysFreeString(retValName);
}

HRESULT wbem::wmi::IntelWmiProvider::convertHttpRcToWmiRc(wbem::framework::UINT32 httpRc)
{
	HRESULT rc = WBEM_NO_ERROR;
	// Of course WMI is different than the CIM standard for error codes so do the mapping
	switch (httpRc)
	{
		case wbem::framework::MOF_ERR_SUCCESS:
			rc = WBEM_NO_ERROR;
			break;
		case wbem::framework::CIM_ERR_FAILED:
			rc = WBEM_E_FAILED;
			break;
		case wbem::framework::CIM_ERR_NOTSUPPORTED:
			rc = WBEM_E_NOT_SUPPORTED;
			break;
		case wbem::framework::CIM_ERR_INVALID_PARAMETER:
			rc = WBEM_E_INVALID_PARAMETER;
			break;
		case wbem::framework::CIM_ERR_METHOD_NOT_AVAILABLE:
			rc = WBEM_E_INVALID_METHOD;
			break;
	}
	return rc;
}

/*
 * Entry point for Invoke method
 * http://msdn.microsoft.com/en-us/library/aa392104(v=vs.85).aspx
 *
 */
HRESULT wbem::wmi::IntelWmiProvider::ExecMethodAsync(const BSTR strObjectPath,
		const BSTR strMethodName,
		long lFlags,
		IWbemContext *pContext,
		IWbemClassObject *pInParams,
		IWbemObjectSink *pResponseHandler)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT rc = WBEM_NO_ERROR;

	if (pResponseHandler == NULL)
	{
		rc = WBEM_E_INVALID_PARAMETER;
	}
	else
	{
		framework::ObjectPath objectPath; // use the object path to get the class name
		IntelToWmi::BstrToObjectPath(strObjectPath, &objectPath);
		const char *className = objectPath.getClass().c_str();
		COMMON_LOG_DEBUG_F("ObjectPath Class: %s", objectPath.getClass().c_str());
		char *methodName = _com_util::ConvertBSTRToString(strMethodName);
		if (methodName == NULL || m_pNamespace == NULL)
		{
			rc = WBEM_E_INVALID_PARAMETER;
		}
		else
		{
			COMMON_LOG_DEBUG_F("Method Name: %s", methodName);
			//Impersonate the client
			HRESULT hr = Impersonate();
			if (FAILED (hr))
			{
				rc = hr;
			}
			else
			{
				wbem::framework::ProviderFactory *pProviderFactory =
						wbem::framework::ProviderFactory::getSingleton();
				if (pProviderFactory == NULL)
				{
					rc = WBEM_E_FAILED;
				}
				else
				{
					pProviderFactory->InitializeProvider();
					wbem::framework::InstanceFactory *pFactory =
							pProviderFactory->getInstanceFactory(objectPath.getClass());
					if (pFactory == NULL)
					{
							rc = WBEM_E_INVALID_CLASS;
							COMMON_LOG_ERROR_F("Couldn't get factory for class %s",
									objectPath.getClass().c_str());
					}
					else
					{
						// convert pInParams to Attributes ...
						BSTR bName;
						VARIANT val;
						CIMTYPE type;
						framework::attributes_t inAttributes;
						if (pInParams != NULL)
						{
							pInParams->BeginEnumeration(0);
							while (pInParams->Next(0, &bName, &val, &type, NULL) != WBEM_S_NO_MORE_DATA)
							{
								char *name = _com_util::ConvertBSTRToString(bName);
								if (NULL != name)
								{
									// ignore system attributes which start with __
									if (!(name[0] == '_' && name[1] == '_'))
									{
										framework::Attribute attribute;
										framework::Attribute *pAttribute = IntelToWmi::ToIntelAttribute(val, type);
										if (pAttribute != NULL)
										{
											inAttributes[name] = *pAttribute;
											delete pAttribute;
										}
										else
										{
											COMMON_LOG_ERROR("pAttribute wasn't created");
											rc = WBEM_E_FAILED;
										}
									}
									delete[] name;
								}
								else
								{
									COMMON_LOG_ERROR("Could not convert bName to name");
									rc = WBEM_E_FAILED;
								}
							}
						}

						wbem::framework::attributes_t outAttributes;
						wbem::framework::UINT32 wbemRc = 0;
						wbem::framework::UINT32 httpRc =
							pFactory->executeMethod(wbemRc, methodName, objectPath,
									inAttributes, outAttributes);

						// Of course WMI is different than the CIM standard for error codes so do the mapping
						rc = convertHttpRcToWmiRc(httpRc);

						addMethodReturnCodeToReturnObject(className, strMethodName, pContext, pResponseHandler, wbemRc);
						delete pFactory;
					}
					pProviderFactory->CleanUpProvider();
				}
				CoRevertToSelf();
			}
			delete[] methodName;
		}
		pResponseHandler->SetStatus(0, rc, NULL, NULL);
	}
	return rc;
}

/*
 * helper to get modified attributes
 */
wbem::framework::attributes_t getModifiedAttributes(wbem::framework::Instance *pCurrentInstance, wbem::framework::Instance *pNewInstance)
{
    LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

    wbem::framework::attributes_t attributes;
    wbem::framework::attributes_t::const_iterator propIter = pCurrentInstance->attributesBegin();
    for (; propIter != pCurrentInstance->attributesEnd(); propIter++)
    {
        std::string key = (*propIter).first;
        wbem::framework::Attribute modifiedInstProp;
        pNewInstance->getAttribute(key, modifiedInstProp);
        wbem::framework::Attribute currInstProp = (*propIter).second;

        if (currInstProp != modifiedInstProp)
        {
            attributes[key] = modifiedInstProp;
        }
    }
    return attributes;
}

/*
 * Modify Instance
 */
HRESULT STDMETHODCALLTYPE wbem::wmi::IntelWmiProvider::PutInstanceAsync(
		/* [in] */ IWbemClassObject __RPC_FAR *pInst,
		/* [in] */ long lFlags,
		/* [in] */ IWbemContext __RPC_FAR *pCtx,
		/* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT result = WBEM_NO_ERROR;

	if (pResponseHandler == NULL)
	{
		result = WBEM_E_INVALID_PARAMETER;
	}
	else
	{
		// check used parameters
		if (pInst == NULL)
		{
			result = WBEM_E_INVALID_PARAMETER;
		}
		else
		{
			wbem::framework::Instance newInstance;
			wbem::framework::attribute_names_t attributenames;
			wbem::framework::ObjectPath path;
			wbem::framework::attributes_t modifiedAttributes;

			HRESULT hr = Impersonate();
			if (FAILED(hr))
			{
				result = hr;
			}
			else
			{
				wbem::framework::ProviderFactory *pProviderFactory =
						wbem::framework::ProviderFactory::getSingleton();
				if (pProviderFactory == NULL)
				{
					result = WBEM_E_FAILED;
				}
				else
				{
					pProviderFactory->InitializeProvider();

					result = IntelToWmi::ToIntelInstance(path, newInstance, pInst);
					wbem::framework::InstanceFactory *pFactory =
							pProviderFactory->getInstanceFactory(path.getClass());

					wbem::framework::Instance *pCurrentInstance = pFactory->getInstance(path, attributenames);

					modifiedAttributes = getModifiedAttributes(pCurrentInstance, &newInstance);
					if (pFactory != NULL)
					{
						try
						{
							pFactory->modifyInstance(path, modifiedAttributes);
						}
						catch (wbem::framework::ExceptionBadParameter &)
						{
							result = WBEM_E_INVALID_PARAMETER;
						}
						catch (wbem::framework::ExceptionSystemError &e)
						{
							result = WBEM_E_PROVIDER_FAILURE;
						}
						catch (wbem::framework::ExceptionNoMemory &)
						{
							result = WBEM_E_OUT_OF_MEMORY;
						}
						catch (wbem::framework::ExceptionNotSupported &)
						{
							result = WBEM_E_NOT_SUPPORTED;
						}
						catch (wbem::framework::Exception &)
						{
							result = WBEM_E_FAILED;
						}
						delete pFactory;
					}
					pProviderFactory->CleanUpProvider();
				}
				CoRevertToSelf();
			}
		}
		pResponseHandler->SetStatus(0, result, NULL, NULL);
	}
	return result;
};

/*
 * Copy off WbemObjectSink object and start listening
 */
HRESULT wbem::wmi::IntelWmiProvider::ProvideEvents(IWbemObjectSink *pSink, long lFlags)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT rc = WBEM_S_NO_ERROR;

	if (!m_eventsEnabled)
	{
		try
		{
			framework::IndicationService *pService = framework::ProviderFactory::getSingleton()->getIndicationService();
			WmiAdapter *pContext = new WmiAdapter(pSink, m_pWbemContext, m_pNamespace);
			pService->startIndicating(pContext);
			m_eventsEnabled = true;
		}
		catch(framework::Exception &ex)
		{
			COMMON_LOG_ERROR_F("Failed to start indicating: %s", ex.what());
			rc = WBEM_E_FAILED;
		}
	}

	COMMON_LOG_DEBUG_F("ProvideEvents: %d", rc);
	return rc;
}

HRESULT wbem::wmi::IntelWmiProvider::NewQuery(unsigned long dwId,
		WBEM_WSTR wszQueryLanguage, WBEM_WSTR wszQuery)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT rc = WBEM_S_NO_ERROR;

	std::wstring query(wszQuery);
	std::string queryStr = convert::wstr_to_str(query);

	COMMON_LOG_INFO_F("NewQuery with ID %u: %s", dwId, queryStr.c_str());
	m_queryCount++;

	COMMON_LOG_DEBUG_F("NewQuery: %d", rc);
	return rc;
}

HRESULT wbem::wmi::IntelWmiProvider::CancelQuery(unsigned long dwId)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT rc = WBEM_S_NO_ERROR;

	COMMON_LOG_INFO_F("Cancel query with ID %u", dwId);

	m_queryCount--;
	if (m_queryCount == 0 && m_eventsEnabled)
	{
		framework::IndicationService *pService = framework::ProviderFactory::getSingleton()->getIndicationService();
		pService->stopIndicating();
		m_eventsEnabled = false;

		wbem::framework::CimomAdapter *pContext = pService->getContext();
		delete pContext;
	}

	COMMON_LOG_DEBUG_F("CancelQuery: %d", rc);
	return rc;
}
