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
 * This file contains the definition of conversion functions between the
 * WBEM implementation to CMPI.
 */

#include "Attribute.h"
#include "Instance.h"
#include "InstanceFactory.h"
#include "ObjectPath.h"

namespace wbem
{
namespace framework
{

/*!
 * Keep the first error. If status is OK and the new status is not OK, then change status.
 * @param status
 * 		current status
 * @param newStatus
 * 		new status
 */
#define	KEEP_ERR(status, newStatus) \
		if ((status).rc == CMPI_RC_OK && (newStatus).rc != CMPI_RC_OK) \
			{ (status).rc = (newStatus).rc; }

/*!
 * Convert an ObjectPath to a new CMPIObjectPath
 * @param[in] pBroker
 * @param[in] pObjectPath
 * @param[out] pRc
 * @return
 */
CMPIObjectPath *intelToCmpi(const CMPIBroker * pBroker, wbem::framework::ObjectPath *pObjectPath, CMPIStatus *pRc);

/*!
 * Convert an Instance to a CMPI Instance
 * @param[in] pBroker
 * 		The CMPI broker is needed to create new char*
 * @param[in] pInstance
 * @param[out] pRc
 * @return
 * 		A pointer to the Instance created
 */
CMPIInstance *intelToCmpi(const CMPIBroker * pBroker, wbem::framework::Instance *pInstance, CMPIStatus *pRc);

/*!
 * Convert an Attribute to a CMPI Attribute
 * @param[in] pBroker
 * 		The CMPI broker is needed to create new char*
 * @param[in] pAttribute
 * @param[in] pCmpiAttribute
 * @param[out] pRc
 */
void intelToCmpi(const CMPIBroker * pBroker, wbem::framework::Attribute *pAttribute, CMPIData *pCmpiAttribute, CMPIStatus *pRc);

/*!
 * Convert a CMPI Attribute to an Attribute.
 * @param[in] pCmpiAttribute
 * @param[in] isKey
 * @param[out] pRc
 * @return
 * 		Returns the new Attribute
 */
wbem::framework::Attribute* cmpiToIntel(CMPIData *pCmpiAttribute, bool isKey, CMPIStatus *pRc);

/*!
 * Convert a CMPI Object path to an ObjectPath
 * @param[in] pCmpiObjectPath
 * @param[out] pObjectPath
 * @param[out] pRc
 */
void cmpiToIntel(const CMPIObjectPath *pCmpiObjectPath, wbem::framework::ObjectPath *pObjectPath, CMPIStatus *pRc);

/*!
 * Convert a CMPI Instance to an Instance
 * @param[in] pCmpiObjectPath
 * @param[in] pCmpiInstance
 * @param[out] pRc
 * @return
 *		Returns a new instance
 */
wbem::framework::Instance *cmpiToIntel(const CMPIObjectPath *pCmpiObjectPath, const CMPIInstance *pCmpiInstance, CMPIStatus *pRc);

/*!
 * Check if attribute exists and is a key attribute
 * @param[in] pNewInstance
 * @param[in] key
 * @return
 *		Returns true if attribute exists and is key
 */
bool isAttributeKey(wbem::framework::Instance *pNewInstance, std::string attributeName);
}
}
