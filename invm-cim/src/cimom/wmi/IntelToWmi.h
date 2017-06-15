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
 * This file contains static utility functions that convert between Intel framework and WMI
*/


#ifndef _WBEM_WMI_NVMTOWMI_H_
#define _WBEM_WMI_NVMTOWMI_H_

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <windows.h>

#include <framework/Instance.h>
#include <framework/InstanceFactory.h>
#include <framework/ObjectPath.h>

namespace wbem
{
namespace wmi
{

class IntelToWmi
{
public:
	static void BstrToObjectPath(BSTR wmiObjectPath, framework::ObjectPath *pObjectPath);
	static SCODE ToWmiInstance(wbem::framework::Instance *pInstance,
			IWbemContext *pContext,
			IWbemServices * pNamespace,
			IWbemClassObject FAR** pNewInst );
	static HRESULT STDMETHODCALLTYPE ToWmiAttribute(wbem::framework::Attribute &attribute, VARIANT &v);
	template <class T> static SAFEARRAY * List(const std::vector<T> &list, enum VARENUM type);
	static SAFEARRAY * StrList(const std::vector<std::string> &list);
	static wbem::framework::UINT16_LIST toUint16List(VARIANT& v);
	static wbem::framework::STR_LIST toReferenceList(VARIANT& v);
	static wbem::framework::STR_LIST toStrList(VARIANT& v);
	static enum wbem::framework::DatetimeSubtype getDatetimeSubtype(std::string datetime);
	static wbem::framework::Attribute *ToIntelAttribute(VARIANT& v, CIMTYPE &cimType);
	static HRESULT STDMETHODCALLTYPE ToIntelInstance(framework::ObjectPath &path,
			wbem::framework::Instance &newInstance,IWbemClassObject __RPC_FAR* pInst );
};

}
}
#endif
