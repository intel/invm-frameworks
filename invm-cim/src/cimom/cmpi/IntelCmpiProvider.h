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
 * This file contains default CMPI entry points that can be used by an external library.
 */

#include <cmpi/cmpift.h>

// Redefine the CMPI instance stub macro so we can pass in a name
// - name 	- must be unique for each use of this macro
#define	CMInstanceMIStubName(name) \
		static CMPIInstanceMIFT name##_instance={ \
				CMPICurrentVersion, \
				CMPICurrentVersion, \
				"instance" #name "_Provider", \
				wbem::framework::Generic_Cleanup, \
				wbem::framework::Generic_EnumInstanceNames, \
				wbem::framework::Generic_EnumInstances, \
				wbem::framework::Generic_GetInstance, \
				wbem::framework::Generic_CreateInstance, \
				CMInstanceMIStubChange(wbem::framework::Generic_), \
				wbem::framework::Generic_DeleteInstance, \
				wbem::framework::Generic_ExecQuery, \
}; \
CMPI_EXTERN_C \
CMPIInstanceMI* name##_Provider_Create_InstanceMI(const CMPIBroker* brkr,const CMPIContext *ctx, CMPIStatus *rc) { \
	static CMPIInstanceMI mi={ \
			NULL, \
			&name##_instance, \
}; \
g_pBroker=brkr; \
wbem::framework::InstanceProviderInit(); \
return &mi;  \
}

// Redefine the CMPI association stub macro so we can pass in a name
// - name 	- must be unique for each use of this macro
#define CMAssociationMIStubName(name) \
		static CMPIAssociationMIFT name##_association ={ \
				CMPICurrentVersion, \
				CMPICurrentVersion, \
				"association" #name "_Provider", \
				wbem::framework::Generic_AssociationCleanup, \
				wbem::framework::Generic_Associators, \
				wbem::framework::Generic_AssociatorNames, \
				wbem::framework::Generic_References, \
				wbem::framework::Generic_ReferenceNames, \
}; \
CMPI_EXTERN_C \
CMPIAssociationMI* name##_Provider_Create_AssociationMI(const CMPIBroker* brkr,const CMPIContext *ctx,  CMPIStatus *rc) { \
	static CMPIAssociationMI mi={ \
			NULL, \
			&name##_association, \
}; \
g_pBroker=brkr; \
CMNoHook; \
return &mi;  \
}

// Redefine the CMPI method provider stub macro so we can pass in a name
// - name 	- must be unique for each use of this macro
#define CMMethodMIStubName(name) \
		static CMPIMethodMIFT name##methMIFT__={ \
				CMPICurrentVersion, \
				CMPICurrentVersion, \
				"method" #name "_Provider", \
				wbem::framework::Generic_MethodCleanup, \
				wbem::framework::Generic_InvokeMethod, \
}; \
CMPI_EXTERN_C \
CMPIMethodMI* name##_Provider_Create_MethodMI(const CMPIBroker* brkr, const CMPIContext *ctx,  CMPIStatus *rc) { \
	static CMPIMethodMI mi={ \
			NULL, \
			&name##methMIFT__, \
}; \
g_pBroker=brkr; \
CMNoHook; \
return &mi; \
}

// Redefine the CMPI indication provider stub macro so we can pass in a name
// - name 	- must be unique for each use of this macro
#define	CMIndicationMIStubName(name) \
		static CMPIIndicationMIFT name##indMIFT__={ \
				CMPICurrentVersion, \
				CMPICurrentVersion, \
				"Indication" #name "_Provider", \
				wbem::framework::Generic_IndicationCleanup, \
				wbem::framework::Generic_AuthorizeFilter, \
				wbem::framework::Generic_MustPoll, \
				wbem::framework::Generic_ActivateFilter, \
				wbem::framework::Generic_DeActivateFilter, \
				wbem::framework::Generic_EnableIndications, \
				wbem::framework::Generic_DisableIndications, \
}; \
CMPI_EXTERN_C \
CMPIIndicationMI*  name##_Provider_Create_IndicationMI(const CMPIBroker* brkr,const CMPIContext *ctx,CMPIStatus *rc) { \
	static CMPIIndicationMI mi={ \
			NULL, \
			&name##indMIFT__, \
}; \
g_pBroker=brkr; \
CMNoHook; \
return &mi; \
}

extern const CMPIBroker *g_pBroker;

namespace wbem
{
namespace framework
{

/*
 * Default implementation of CMPI methods - call these from your provider if you have no need for special behavior
 */

void InstanceProviderInit();

CMPIStatus Generic_Cleanup(CMPIInstanceMI *pThis, const CMPIContext *pContext, CMPIBoolean term);

CMPIStatus Generic_EnumInstanceNames(CMPIInstanceMI *cThis, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *ref);

CMPIStatus Generic_EnumInstances(CMPIInstanceMI *pThis, const CMPIContext *pContext,
		const CMPIResult *pResult, const CMPIObjectPath *pRefCmpiObjectPath, const char **properties);

CMPIStatus Generic_CreateInstance(CMPIInstanceMI *cThis, const CMPIContext *ctx, const CMPIResult *rslt,
				const CMPIObjectPath *cop, const CMPIInstance *inst);

CMPIStatus Generic_ModifyInstance(
		CMPIInstanceMI *cThis, const CMPIContext *pContext, const CMPIResult *pResult,
		const CMPIObjectPath *pCmpiObjectPath, const CMPIInstance *pCmpiInstance,
		const char **ppProperties);

CMPIStatus Generic_DeleteInstance(CMPIInstanceMI *cThis, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *cop);

CMPIStatus Generic_GetInstance(CMPIInstanceMI *pThis, const CMPIContext *pContext,
		const CMPIResult *pResult, const CMPIObjectPath *pCmpiObjectPath, const char **properties);

CMPIStatus Generic_ExecQuery(CMPIInstanceMI *cThis, const CMPIContext *ctx, const CMPIResult *rslt,
				const CMPIObjectPath *cop, const char *lang, const char *query);

CMPIStatus Generic_AssociationCleanup(CMPIAssociationMI *mi, const CMPIContext *ctx, CMPIBoolean terminating);

CMPIStatus Generic_Associators(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op, const char *assocClass,
		const char *resultClass, const char *role, const char *resultRole, const char **properties);

CMPIStatus Generic_AssociatorNames(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op, const char *assocClass,
		const char *resultClass, const char *role, const char *resultRole);

CMPIStatus Generic_References(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op,
		const char *resultClass, const char *role, const char **properties);

CMPIStatus Generic_ReferenceNames(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op,
		const char *resultClass, const char *role);

CMPIStatus Generic_MethodCleanup(CMPIMethodMI *mi, const CMPIContext *ctx,
		CMPIBoolean terminating);

CMPIStatus Generic_InvokeMethod(CMPIMethodMI *mi, const CMPIContext *ctx, const CMPIResult *rslt,
		const CMPIObjectPath *op, const char *method, const CMPIArgs *in, CMPIArgs *out);

CMPIStatus Generic_IndicationCleanup(CMPIIndicationMI *mi, const CMPIContext *ctx,
		CMPIBoolean terminating);

CMPIStatus Generic_ActivateFilter(CMPIIndicationMI *mi,
		const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *classPath,
		CMPIBoolean firstActivation);

CMPIStatus Generic_DeActivateFilter(CMPIIndicationMI *mi,
		const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *classPath,
		CMPIBoolean lastActivation);

CMPIStatus Generic_AuthorizeFilter(CMPIIndicationMI *mi,
		const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *op,
		const char *owner);

CMPIStatus Generic_MustPoll(CMPIIndicationMI *mi, const CMPIContext *ctx,
		const CMPISelectExp *filter, const char *className, const CMPIObjectPath *classPath);

CMPIStatus Generic_EnableIndications(CMPIIndicationMI *mi, const CMPIContext *);
CMPIStatus Generic_DisableIndications(CMPIIndicationMI *mi, const CMPIContext *);

}
}
