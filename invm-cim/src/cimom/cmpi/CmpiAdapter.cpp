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

#include <cmpi/cmpimacs.h>

#include "logging.h"
#include "ProviderFactory.h"
#include "CmpiAdapter.h"
#include "IntelToCmpi.h"

extern const CMPIBroker *g_pBroker;

bool classIsA(std::string child, std::string parent)
{
	bool result = false;
	wbem::framework::ProviderFactory *pFactory = wbem::framework::ProviderFactory::getSingleton();
	if (g_pBroker && pFactory)
	{
		std::string defaultNamespace = pFactory->getDefaultCimNamespace();
		CMPIObjectPath *objectPath = CMNewObjectPath (g_pBroker,
				defaultNamespace.c_str(),
				child.c_str(), NULL);
		result = CMClassPathIsA(g_pBroker, objectPath, parent.c_str(), NULL);
	}
	return result;
}

wbem::framework::CmpiAdapter::CmpiAdapter(CMPIContext *pContext, const CMPIBroker *pBroker)
{
	m_pContext = pContext;
	m_pBroker = pBroker;
}

void wbem::framework::CmpiAdapter::sendIndication(wbem::framework::Instance &indication)
{
	LogEnterExit logging(__FUNCTION__, __FILE__, __LINE__);
	CMPIStatus status = CBAttachThread(m_pBroker, m_pContext);

	if (status.rc != CMPI_RC_OK)
	{
		COMMON_LOG_ERROR_F("Error attaching thread. Status: %d.", (int)status.rc);
	}
	else
	{
		CMPIInstance *inst = intelToCmpi(m_pBroker, &indication,  &status);
		if (inst == NULL)
		{
			COMMON_LOG_ERROR("Error converting NvmInstance to CMPIInstance. "
					"CMPIInstance is null.");
		}
		else if (status.rc != CMPI_RC_OK)
		{
			COMMON_LOG_ERROR_F("Error converting NvmInstance to CMPIInstance. "
					"Status: %d.", (int)status.rc);
		}
		else
		{
			COMMON_LOG_DEBUG("Attempting to deliver indication");

			std::string cim_namespace = ProviderFactory::getSingleton()->getDefaultCimNamespace();

			status = CBDeliverIndication (m_pBroker, m_pContext, cim_namespace.c_str(), inst);
			if (status.rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CBDeliverIndication returned error code: %d",
						(int)status.rc);
			}
			else
			{
				COMMON_LOG_DEBUG("CBDeliverIndication reported success");
			}
		}
	}
}
