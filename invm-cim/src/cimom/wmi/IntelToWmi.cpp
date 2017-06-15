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
 * This file contains static utility functions that convert between Intel and WMI
*/

#include <objbase.h>
#include <initguid.h>
#include <strsafe.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <comutil.h>
#include <windows.h>
#include <WbemCli.h>
#include "wmiUtilities.h"
#include <comutil.h>
#include <logger/logging.h>
#include <mapidbg.h>

#include "IntelToWmi.h"
#include <framework/Attribute.h>
#include <framework/ObjectPathBuilder.h>
#include <framework/Types.h>
#include <framework/StringUtil.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define CIM_UINT16LIST	 8210
#define CIM_STRLIST		 8200
#define CIM_REFERENCELIST     8294

void wbem::wmi::IntelToWmi::BstrToObjectPath(BSTR wmiObjectPath, framework::ObjectPath* pObjectPath)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	// Parse ObjectPath into ObjectPath
	char *fullObjectPath;
	fullObjectPath = _com_util::ConvertBSTRToString(wmiObjectPath);

	if (fullObjectPath != NULL)
	{
		wbem::framework::ObjectPathBuilder builder(fullObjectPath);
		delete[] fullObjectPath;
		builder.Build(pObjectPath);
	}
}

/*
 * Utility methods for converting c++ classes to WMI/WBEM c++ classes
 * pInstance - the instance being converted
 * pContext - WMI context on which the WMI instance will be created.
 * pNamespace - namespace service used to create the WMI instance
 * pNewInst - this will point to the new WMI instance created
 */
SCODE wbem::wmi::IntelToWmi::ToWmiInstance(wbem::framework::Instance *pInstance,
		IWbemContext *pContext,
		IWbemServices *pNamespace,
		IWbemClassObject FAR** pNewInst )
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	SCODE sc;
	COMMON_LOG_DEBUG_F("Converting instance of class: %s", pInstance->getClass().c_str());

	BSTR refStr = _com_util::ConvertStringToBSTR(pInstance->getClass().c_str());
	// this is a C++ object that represents a CIM class. It is used to create CIM instances of
	// the CIM class.
	IWbemClassObject *pClass = NULL;
	sc = pNamespace->GetObject(refStr, 0, pContext, &pClass, NULL);

	if (FAILED(sc))
	{
		COMMON_LOG_ERROR_F("WMI unable to get IWbemClassObject from namespace service. Error: 0x%x", sc);
	}
	else
	{
		sc = pClass->SpawnInstance(0, pNewInst); // Ask WMI to create an instance of the CIM class
		pClass->Release();

		if(FAILED(sc))
		{
			COMMON_LOG_ERROR_F("WMI unable to spawn a new Instance. Error: 0x%x", sc);
		}
		else
		{
			VARIANT v; // VARIANT = WMI version of Attribute
			for (wbem::framework::attributes_t::const_iterator pAttribute = pInstance->attributesBegin();
				pAttribute != pInstance->attributesEnd(); pAttribute++)
			{
				std::string attributeKey = pAttribute->first;
				COMMON_LOG_DEBUG_F("Converting attribute: %s", attributeKey.c_str());

				wbem::framework::Attribute attribute = (pAttribute->second);

				// If the attribute is marked as embedded, another instance needs to be created from the
				// string value of the attribute (should be CIM-XML). Because this needs the namespace service and
				// context going to create the VARIANT here, instead of calling ToWmiAttribute to create the VARIANT.
				if (attribute.isEmbedded())
				{
					framework::Instance Reference(attribute.stringValue());

					IWbemClassObject *pWmiReference = NULL;
					ToWmiInstance(&Reference, pContext, pNamespace, &pWmiReference);

					// WMI requires the embedded instance to be added to the root instace as an
					// IUnknown. This is the WMI way to get the appropriate pointer. Not exactly
					// sure why, but this is what works.
					IUnknown *pUnknown;
					pWmiReference->QueryInterface(IID_IUnknown, (void**)&pUnknown);

					VARIANT vt;
					(V_UNKNOWN(&vt) = pUnknown)->AddRef(); // must addRef or it crashes.
					vt.vt = VT_UNKNOWN;

					sc = (*pNewInst)->Put(convert::str_to_wstr(attributeKey).c_str(), 0, &vt, 0);
					if (FAILED(sc))
					{
						COMMON_LOG_ERROR_F("Error adding embedded instance VARIANT to instance: 0x%x", sc);
					}

					pWmiReference->Release();
					pUnknown->Release();
				}
				else
				{
					VariantInit(&v);
					sc = ToWmiAttribute(attribute, v);
					if (sc == WBEM_NO_ERROR)
					{
						sc = (*pNewInst)->Put(convert::str_to_wstr(attributeKey).c_str(), 0, &v, 0);
						if (FAILED(sc))
						{
							COMMON_LOG_ERROR_F("Put Error. Attribute: %s. Error: 0x%x", attributeKey.c_str(), sc);
						}
					}
					else
					{
						COMMON_LOG_ERROR_F("Error converting attribute: %s", attributeKey.c_str());
					}
					VariantClear(&v);
				}
			}

			if (FAILED(sc))
			{
				COMMON_LOG_ERROR_F("Error converting the Instance to a IWbemClassObject. Error Code: 0x%x", sc);
			}
		}
	}

	return sc;
}

/*
 * Helper method to build c++ object path from path string
 */
HRESULT STDMETHODCALLTYPE buildObjectPath(std::string objPathString, wbem::framework::ObjectPath &path)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	HRESULT result = WBEM_NO_ERROR;

	if (!objPathString.empty())
	{
		wbem::framework::ObjectPathBuilder builder(objPathString);
		builder.Build(&path);
	}
	else
	{
		COMMON_LOG_ERROR("Did not find __Path attribute to build ObjectPath with");
		result = WBEM_E_FAILED;
	}
	return result;
}
/*
 * Utility methods for converting c++ WMI/WBEM classes to c++ classes
 * path - the object path of the WMI instance converted to c++ instance
 * newInstance - the new instance being created
 * pInst - this points to the WMI instance being converted
 */
HRESULT STDMETHODCALLTYPE wbem::wmi::IntelToWmi::ToIntelInstance(wbem::framework::ObjectPath &path,
		wbem::framework::Instance &newInstance, IWbemClassObject __RPC_FAR* pInst )
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT result = WBEM_NO_ERROR;

	// check used parameters
	if (pInst == NULL)
	{
		result = WBEM_E_INVALID_PARAMETER;
	}
	else
	{
		BSTR bName; // used to get name of attribute
		VARIANT val; // WMI attribute
		CIMTYPE type; // WMI attribute type

		// convert WMI Instance to list of Attributes by iterating over pInst attributes
		wbem::framework::attributes_t attributes;
		pInst->BeginEnumeration(0);
		std::string objectPathString; // used to get string object path (in __Path)

		while (pInst->Next(0, &bName, &val, &type, NULL) != WBEM_S_NO_MORE_DATA)
		{
			char *name = _com_util::ConvertBSTRToString(bName);
			if (name != NULL)
			{
				wbem::framework::Attribute *pAttribute = IntelToWmi::ToIntelAttribute(val, type);

				if (pAttribute != NULL)
				{
					if (std::string(name) == "__PATH")
					{
						objectPathString = pAttribute->stringValue();
						result = buildObjectPath(objectPathString, path);
					}
					attributes[name] = *pAttribute;
					delete pAttribute;
				}
				else
				{
					COMMON_LOG_ERROR_F("WMI Variant %s was not converted to an Attribute.",
							name);
					result = WBEM_E_FAILED;
				}
			}
			else
			{
				COMMON_LOG_ERROR("Could not get Attribute name from BSTR");
			}
		}

		if (result == WBEM_NO_ERROR)
		{
			newInstance = wbem::framework::Instance(path);
			wbem::framework::attributes_t keys = path.getKeys();
			wbem::framework::attributes_itr_t attr_iter = attributes.begin();

			for (;attr_iter != attributes.end(); attr_iter++)
			{
				if (keys.find(attr_iter->first) != keys.end())
				{
					attr_iter->second.setIsKey(true);
				}

				newInstance.setAttribute(attr_iter->first, attr_iter->second);
			}
		}
	}
	return result;
}

/*
 * Helper method to convert an Attribute to a WMI Variant
 */
HRESULT STDMETHODCALLTYPE wbem::wmi::IntelToWmi::ToWmiAttribute(wbem::framework::Attribute &attribute, VARIANT &v )
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT result = WBEM_NO_ERROR;

	_variant_t(attribute.asStr().c_str());

	switch (attribute.getType())
	{
		case wbem::framework::BOOLEAN_T:
			v.vt = VT_BOOL;
			v.boolVal = attribute.boolValue() ? VARIANT_TRUE : VARIANT_FALSE;
			break;
		case wbem::framework::ENUM_T:
		case wbem::framework::ENUM16_T:
		case wbem::framework::UINT8_T:
			v.vt = VT_UI1;
			v.uiVal = attribute.uintValue();
			break;
		case wbem::framework::UINT16_T:
		case wbem::framework::UINT32_T:
			v.vt = VT_I4;
			v.lVal = attribute.uintValue();
			break;
		case wbem::framework::SINT8_T:
		case wbem::framework::SINT16_T:
			v.vt = VT_I2;
			v.iVal = attribute.intValue();
			break;
		case wbem::framework::SINT32_T:
			v.vt = VT_I4;
			v.intVal = attribute.intValue();
			break;
		case wbem::framework::UINT64_T:
		case wbem::framework::SINT64_T:
			// from http://msdn.microsoft.com/en-us/library/windows/desktop/aa392716(v=vs.85).aspx
			// Unsigned 64-bit integer in string form. This type follows hexadecimal or decimal format according to ANSI C rules.
			v.vt = VT_BSTR;
			v.bstrVal = SysAllocString(convert::str_to_wstr(attribute.asStr()).c_str());
			if(!v.bstrVal)
			{
				COMMON_LOG_ERROR("WMI unable to create string");
				result = WBEM_E_OUT_OF_MEMORY;
			}
			break;
		case wbem::framework::DATETIME_T:
		case wbem::framework::DATETIME_INTERVAL_T:
		case wbem::framework::STR_T:
			v.vt = VT_BSTR;
			v.bstrVal = SysAllocString(convert::str_to_wstr(attribute.stringValue()).c_str());
			if(!v.bstrVal)
			{
				COMMON_LOG_ERROR("WMI unable to create string");
				result = WBEM_E_OUT_OF_MEMORY;
			}
			break;
		case wbem::framework::UINT16_LIST_T:
			v.vt = VT_ARRAY | VT_I2;
			v.parray = IntelToWmi::List(attribute.uint16ListValue(), VT_I2);
			if (!v.parray)
			{
				result = WBEM_E_OUT_OF_MEMORY; // error is logged in List fn
			}
			break;
		case wbem::framework::UINT32_LIST_T:
			v.vt = VT_ARRAY | VT_I4;
			v.parray = IntelToWmi::List(attribute.uint32ListValue(), VT_I4);
			if (!v.parray)
			{
				result = WBEM_E_OUT_OF_MEMORY; // error is logged in List fn
			}
			break;
		case wbem::framework::UINT64_LIST_T:
			v.vt = VT_ARRAY | VT_I8;
			v.parray = IntelToWmi::List(attribute.uint64ListValue(), VT_I8);
			if (!v.parray)
			{
				result = WBEM_E_OUT_OF_MEMORY; // error is logged in List fn
			}
			break;
		case wbem::framework::STR_LIST_T:
			v.vt = VT_ARRAY | VT_BSTR;
			v.parray = IntelToWmi::StrList(attribute.strListValue());
			if (!v.parray)
			{
				result = WBEM_E_OUT_OF_MEMORY; // error is logged in List fn
			}
			break;
		default:
			COMMON_LOG_ERROR_F("unrecognized type %d", (int) attribute.getType());
			result = WBEM_E_FAILED;
	}

	return result;
}


template <class T>
SAFEARRAY * wbem::wmi::IntelToWmi::List(const std::vector<T> &list,  enum VARENUM type)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	SAFEARRAYBOUND arrayBound[1] = {0};
	arrayBound[0].lLbound = 0;
	arrayBound[0].cElements = list.size();

	// the psafeArray is assigned to a VARIANT which should handle destroying it
	SAFEARRAY *psafeArray = SafeArrayCreate(type, 1, arrayBound);
	if (!psafeArray) // no memory
	{
		COMMON_LOG_ERROR("WMI unable to create array");
	}
	else
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			SafeArrayPutElement(psafeArray, (long*)&i, (void *) &(list[i]));
		}
	}

	return psafeArray;
}

SAFEARRAY * wbem::wmi::IntelToWmi::StrList(const std::vector<std::string> &list)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	SAFEARRAYBOUND arrayBound[1] = {0};
	arrayBound[0].lLbound = 0;
	arrayBound[0].cElements = list.size();

	// the psafeArray is assigned to a VARIANT which should handle destroying it
	SAFEARRAY *psafeArray = SafeArrayCreate(VT_BSTR, 1, arrayBound);
	if (!psafeArray) // no memory
	{
		COMMON_LOG_ERROR("WMI unable to create array");
	}
	else
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			// allocate the BSTR from the string
			BSTR bStr = SysAllocString(convert::str_to_wstr(list[i]).c_str());
			if (bStr)
			{
				SafeArrayPutElement(psafeArray, (long*)&i, (void *)bStr);
			}
			else // no memory
			{
				COMMON_LOG_ERROR("WMI unable to create string");
				SafeArrayDestroy(psafeArray);
				break;
			}
		}
	}

	return psafeArray;
}

wbem::framework::UINT16_LIST wbem::wmi::IntelToWmi::toUint16List(VARIANT& v)
{
	framework::UINT16_LIST uint16List;
	long lBound, uBound;

	if (!FAILED(SafeArrayGetLBound(v.parray, 1, &lBound)) && !FAILED(SafeArrayGetUBound(v.parray, 1, &uBound)))
	{
		for (long i = lBound; i <= uBound; i++)
		{
			UINT16 uint16Val;
			SafeArrayGetElement(v.parray, &i, &uint16Val);
			uint16List.push_back(uint16Val);
		}
	}

	return uint16List;
}

wbem::framework::STR_LIST wbem::wmi::IntelToWmi::toReferenceList(VARIANT& v)
{
	framework::STR_LIST refList;
	long lBound, uBound;

	if (!FAILED(SafeArrayGetLBound(v.parray, 1, &lBound)) && !FAILED(SafeArrayGetUBound(v.parray, 1, &uBound)))
	{
		for (long i = lBound; i <= uBound; i++)
		{
			BSTR bString;
			SafeArrayGetElement(v.parray, &i, &bString);
			std::string str = std::string(_com_util::ConvertBSTRToString(bString));
			refList.push_back(str);
		}
	}

	return refList;
}

wbem::framework::STR_LIST wbem::wmi::IntelToWmi::toStrList(VARIANT& v)
{
	framework::STR_LIST strList;
	long lBound, uBound;

	if (!FAILED(SafeArrayGetLBound(v.parray, 1, &lBound)) && !FAILED(SafeArrayGetUBound(v.parray, 1, &uBound)))
	{
		for (long i = lBound; i <= uBound; i++)
		{
			BSTR bString;
			SafeArrayGetElement(v.parray, &i, &bString);
			std::string str = std::string(_com_util::ConvertBSTRToString(bString));
			strList.push_back(str);
		}
	}

	return strList;
}

enum wbem::framework::DatetimeSubtype wbem::wmi::IntelToWmi::getDatetimeSubtype(std::string datetime)
{
	return datetime[21] == ':' ?
			wbem::framework::DATETIME_SUBTYPE_INTERVAL : wbem::framework::DATETIME_SUBTYPE_DATETIME;
}

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa380072(v=vs.85).aspx
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms221170(v=vs.85).aspx
 * contains what the VARTYPEs are and which field to use for each
 */
wbem::framework::Attribute *wbem::wmi::IntelToWmi::ToIntelAttribute(VARIANT& v, CIMTYPE &cimType)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("CIM Type: %d", (int)cimType);

	if (v.vt == VT_EMPTY || v.vt == VT_NULL)
	{
		return new framework::Attribute();
	}
	else
	{
		switch(cimType)
		{
			case CIM_SINT8:
				return new framework::Attribute((wbem::framework::SINT8)v.iVal, false);
			case CIM_UINT8:
				return new framework::Attribute((wbem::framework::UINT8)v.uiVal, false);
			case CIM_SINT16:
				return new framework::Attribute((wbem::framework::SINT16)v.iVal, false);
			case CIM_UINT16:
				return new framework::Attribute((wbem::framework::UINT16)v.lVal, false);
			case CIM_SINT32:
				return new framework::Attribute((wbem::framework::SINT32)v.intVal, false);
			case CIM_UINT32:
				return new framework::Attribute((wbem::framework::UINT32)v.uintVal, false);
			case CIM_SINT64:
			{
				std::string str = std::string(_com_util::ConvertBSTRToString(v.bstrVal));
				wbem::framework::SINT64 value = wbem::framework::StringUtil::stringToInt64(str);
				return new framework::Attribute(value, false);
			}
			case CIM_UINT64:
			{
				std::string str = std::string(_com_util::ConvertBSTRToString(v.bstrVal));
				wbem::framework::UINT64 value = wbem::framework::StringUtil::stringToUint64(str);
				return new framework::Attribute(value, false);
			}
			case CIM_REAL32:
				return new framework::Attribute((wbem::framework::REAL32)v.fltVal, false);
			case CIM_BOOLEAN:
				return new framework::Attribute((wbem::framework::BOOLEAN)v.boolVal, false);
			case CIM_STRING:
			{
				std::string str = std::string(_com_util::ConvertBSTRToString(v.bstrVal));
				return new framework::Attribute(str, false);
			}
			case CIM_DATETIME:
			{
				std::string str = std::string(_com_util::ConvertBSTRToString(v.bstrVal));
				enum wbem::framework::DatetimeSubtype datetimeSubtype = getDatetimeSubtype(str);
				return new framework::Attribute(str, datetimeSubtype, false);
			}
			case CIM_STRLIST:
			{
				framework::STR_LIST strList = toStrList(v);
				return new framework::Attribute(strList, false);
			}
			case CIM_UINT16LIST:
			{
				framework::UINT16_LIST uint16List = toUint16List(v);
				return new framework::Attribute(uint16List, false);
			}
			case CIM_REFERENCE:
			{
				std::string str = std::string(_com_util::ConvertBSTRToString(v.bstrVal));
				return new framework::Attribute(str, false);
			}
			case CIM_REFERENCELIST:
			{
				framework::STR_LIST refList = toReferenceList(v);
				return new framework::Attribute(refList, false);
			}
			case CIM_CHAR16:
			case CIM_OBJECT:
			case CIM_FLAG_ARRAY:
				COMMON_LOG_ERROR_F("Type[%i] not implemented", (int)cimType);
			default:
				return new framework::Attribute();
		}
	}
}
