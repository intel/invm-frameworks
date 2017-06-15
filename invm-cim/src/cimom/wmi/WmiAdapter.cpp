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

#include <common/logger/logging.h>
#include "WmiAdapter.h"
#include "IntelToWmi.h"

/*
 * WMI doesn't have an equivelant classIsA function, so just return false
 */
bool classIsA(std::string child, std::string parent)
{
	return false;
}

void wbem::wmi::WmiAdapter::sendIndication(wbem::framework::Instance &indication)
{
    LogEnterExit logging(__FUNCTION__, __FILE__, __LINE__);
    IWbemClassObject *pInstance = NULL;

    IntelToWmi::ToWmiInstance(&indication, m_pContext, m_pNamespaceService, &pInstance);

    if (pInstance == NULL)
    {
        COMMON_LOG_ERROR("Error getting WmiInstance for NvmInstance. pInstance is NULL");
    }
    else
    {
        COMMON_LOG_DEBUG("Sending indication...");
        // Tell WMI to indicate the indication instance
        HRESULT rc = m_pSink->Indicate(1, &pInstance);
        if (rc != WBEM_NO_ERROR)
        {
            COMMON_LOG_ERROR_F("Error creating indication. Error: %d", rc);
        }
        else
        {
            COMMON_LOG_DEBUG("... Indication sent successfully.");

        }
    }
}


wbem::wmi::WmiAdapter::WmiAdapter(IWbemObjectSink *pSink, IWbemContext *pContext, IWbemServices *pNamespaceService)
{
	LogEnterExit logging(__FUNCTION__, __FILE__, __LINE__);

    m_pSink = pSink;
    m_pSink->AddRef();
    m_pContext = pContext;
    m_pContext->AddRef();
    m_pNamespaceService = pNamespaceService;
    m_pNamespaceService->AddRef();
}

wbem::wmi::WmiAdapter::~WmiAdapter()
{
	LogEnterExit logging(__FUNCTION__, __FILE__, __LINE__);

	m_pSink->Release();
	m_pContext->Release();
	m_pNamespaceService->Release();
}
