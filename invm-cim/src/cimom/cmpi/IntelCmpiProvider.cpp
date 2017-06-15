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
 * This file contains the implementation of the CMPI Provider.
 * Each CIM intrinsic method is represented here by a Generic_
 * function and is called directly by the CMPI CIMOM. Associated Cleanup functions are called
 * shortly after the CIMOM has called any intrinsic method. The CIMOM may call multiple intrinsic
 * methods prior to executing the cleanup function.  Currently the cleanup function is used to
 * gather all the logs from the csv file to the database as the CIMOM process may not release the
 * library (which would call lib_unload).
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <cmpi/cmpift.h>
#include "IntelToCmpi.h"
#include "IntelCmpiProvider.h"
#include "CmpiAdapter.h"
#include "IndicationService.h"

// Intel CIM Framework
#include "ObjectPathBuilder.h"
#include "Strings.h"
#include "Attribute.h"
#include "ExceptionBadParameter.h"
#include "Exception.h"
#include "ExceptionNotSupported.h"
#include "ExceptionNoMemory.h"
#include "ExceptionSystemError.h"
#include "ProviderFactory.h"
#include <logger/logging.h>
#include <common_types.h>

#include "AssociationFactory.h"

// Global - this variable is used in the macros above.
const CMPIBroker *g_pBroker;

namespace wbem
{
namespace framework
{

// Pointer to the context the indication provider should use in it's callbacks
CmpiAdapter *g_pCimomContext;
static int g_enabled = 0;

void InstanceProviderInit()
{
}



/*
 * -------------------------------------------------------------------------------------------------
 * The following functions are required for the CMPI Provider.  The "Generic_" prefix is used in the
 * macros above and is expected to not change
 * -------------------------------------------------------------------------------------------------
 */


/*
 * Cleanup prior to unloading the provider
 */
CMPIStatus Generic_Cleanup(CMPIInstanceMI *pThis, const CMPIContext *pContext, CMPIBoolean term)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	CMReturn (CMPI_RC_OK);
}

/*
 * Enumerate the object paths of the instances serviced by this provider
 */
CMPIStatus Generic_EnumInstanceNames(CMPIInstanceMI *cThis, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *ref)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		const char *const className = CMGetCharsPtr(CMGetClassName(ref, NULL), NULL);
		wbem::framework::InstanceFactory *pFactory =
				pProviderFactory->getInstanceFactory(className);

		if (pFactory != NULL)
		{
			if (status.rc == CMPI_RC_OK)
			{
				wbem::framework::instance_names_t *pObjectPaths = NULL;
				try
				{
					pObjectPaths = pFactory->getInstanceNames();
				}
				catch(wbem::framework::ExceptionBadParameter &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
				}
				catch(wbem::framework::ExceptionSystemError &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
				}
				catch(wbem::framework::ExceptionNotSupported &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
				}
				catch(wbem::framework::Exception &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
				}

				if (pObjectPaths != NULL)
				{
					if (status.rc == CMPI_RC_OK)
					{
						wbem::framework::instance_names_t::iterator pathIterator = pObjectPaths->begin();
						for (; pathIterator != pObjectPaths->end(); pathIterator++)
						{
							CMPIObjectPath *cmpiObjectPath;
							CMPIStatus tempStatus;
							cmpiObjectPath = intelToCmpi(g_pBroker, &(*pathIterator), &tempStatus);
							KEEP_ERR(status, tempStatus);

							CMReturnObjectPath(rslt, cmpiObjectPath);
						}
					}
					delete pObjectPaths;
				}
			}
			delete pFactory;
		}
		pProviderFactory->CleanUpProvider();
	}

	CMReturnDone (rslt);
	COMMON_LOG_INFO_F("Returning with status %d", status.rc);

	return status;
}

/*
 * Enumerate the instances serviced by this provider
 */
CMPIStatus Generic_EnumInstances(CMPIInstanceMI *pThis, const CMPIContext *pContext,
		const CMPIResult *pResult, const CMPIObjectPath *pRefCmpiObjectPath, const char **properties)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		const char *const className = CMGetCharsPtr(CMGetClassName(pRefCmpiObjectPath, NULL), NULL);
		wbem::framework::InstanceFactory *pFactory =
				pProviderFactory->getInstanceFactory(className);

		if (pFactory != NULL)
		{
			if (status.rc == CMPI_RC_OK)
			{
				// get all instances with all attributes
				wbem::framework::attribute_names_t attrNames;
				wbem::framework::instances_t *pInstances = NULL;
				try
				{
					pInstances = pFactory->getInstances(attrNames);
				}
				catch(wbem::framework::ExceptionBadParameter &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
				}
				catch(wbem::framework::ExceptionSystemError &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
				}
				catch(wbem::framework::ExceptionNotSupported &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
				}
				catch(wbem::framework::Exception &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
				}

				if (pInstances != NULL)
				{
					if (status.rc == CMPI_RC_OK)
					{
						// iterate through each instance
						wbem::framework::instances_t::iterator iInstance;
						COMMON_LOG_DEBUG_F("Adding %d Instances", (int)pInstances->size());
						for (iInstance = pInstances->begin(); iInstance != pInstances->end(); iInstance++)
						{
							CMPIStatus tempStatus;
							CMPIInstance *pCmpiInstance = intelToCmpi(g_pBroker, &(*iInstance), &tempStatus);
							KEEP_ERR(status, tempStatus);

							if (status.rc == CMPI_RC_OK)
							{
								COMMON_LOG_DEBUG_F("Adding Instance: %s", iInstance->getClass().c_str());
								CMReturnInstance (pResult, pCmpiInstance);
							}
							else
							{
								COMMON_LOG_ERROR("Instance not added.  Issue converting to CMPI");
							}
						}
					}
					delete pInstances;
				}
			}
			delete pFactory;
		}
		pProviderFactory->CleanUpProvider();
	}

	CMReturnDone (pResult);
	COMMON_LOG_INFO_F("Returning status %d", status.rc);

	return status;
}

/*
 * Get instance defined by an object path
 */
CMPIStatus Generic_GetInstance(CMPIInstanceMI *pThis, const CMPIContext *pContext,
		const CMPIResult *pResult, const CMPIObjectPath *pCmpiObjectPath, const char **properties)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		wbem::framework::ObjectPath objectPath;
		cmpiToIntel(pCmpiObjectPath, &objectPath, &status);

		if (status.rc == CMPI_RC_OK)
		{

			wbem::framework::InstanceFactory *pFactory =
					pProviderFactory->getInstanceFactory(objectPath.getClass());
			if (pFactory != NULL)
			{
				if (status.rc == CMPI_RC_OK)
				{
					// With the Intel object path get the Intel Instance
					wbem::framework::attribute_names_t attributes;
					wbem::framework::Instance *pInstance = NULL;
					try
					{
						pInstance = pFactory->getInstance(objectPath, attributes);

					}
					catch(wbem::framework::ExceptionBadParameter &e)
					{
						CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
					}
					catch(wbem::framework::ExceptionSystemError &e)
					{
						CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
					}
					catch(wbem::framework::ExceptionNotSupported &e)
					{
						CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
					}
					catch(wbem::framework::Exception &e)
					{
						CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
					}
					if (pInstance != NULL)
					{
						if (status.rc == CMPI_RC_OK)
						{
							CMPIInstance *pCmpiInstance = intelToCmpi(g_pBroker, pInstance, &status);
							CMReturnInstance (pResult, pCmpiInstance);
						}
						delete pInstance;
					}
				}
				delete pFactory;
			}
		}
		pProviderFactory->CleanUpProvider();
	}
	CMReturnDone (pResult);
	COMMON_LOG_INFO_F("Returning status %d", status.rc);

	return status;
}

/*
 * Create an instance using an object path as reference
 */
CMPIStatus Generic_CreateInstance
		(CMPIInstanceMI *cThis, const CMPIContext *ctx, const CMPIResult *rslt,
				const CMPIObjectPath *cop, const CMPIInstance *inst)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	// TODO: Implement when needed
	CMReturn (CMPI_RC_ERR_NOT_SUPPORTED);
}

/*
 * Get modified attributes
 */
wbem::framework::attributes_t getModifiedAttributes(wbem::framework::Instance *pCurrentInstance, wbem::framework::Instance *pNewInstance)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	wbem::framework::attributes_t attributes;
	wbem::framework::attributes_t::const_iterator propIter = pCurrentInstance->attributesBegin();
	for (; propIter != pCurrentInstance->attributesEnd(); propIter++)
	{
		std::string key = (*propIter).first;
		wbem::framework::Attribute currInstProp = (*propIter).second;
		wbem::framework::Attribute modifiedInstProp;
		pNewInstance->getAttribute(key, modifiedInstProp);

		currInstProp.normalize();
		modifiedInstProp.normalize();

		if (currInstProp != modifiedInstProp)
		{
			attributes[key] = modifiedInstProp;
		}
	}
	return attributes;
}

/*
 * Replace an existing instance using an object path as reference
 */
CMPIStatus Generic_ModifyInstance(
		CMPIInstanceMI *cThis, const CMPIContext *pContext, const CMPIResult *pResult,
		const CMPIObjectPath *pCmpiObjectPath, const CMPIInstance *pCmpiInstance,
		const char **ppProperties)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		wbem::framework::ObjectPath objectPath;
		cmpiToIntel(pCmpiObjectPath, &objectPath, &status);

		if (status.rc == CMPI_RC_OK)
		{
			wbem::framework::InstanceFactory *pProvider =
					pProviderFactory->getInstanceFactory(objectPath.getClass());
			if (pProvider != NULL)
			{
				// With the Intel object path get the Intel Instance
				wbem::framework::attribute_names_t attributenames;
				wbem::framework::Instance *pCurrentInstance = NULL;
				wbem::framework::Instance *pNewInstance = NULL;
				wbem::framework::Instance *pModifiedInstance = NULL;
				try
				{

					pCurrentInstance = pProvider->getInstance(objectPath, attributenames);
					pNewInstance = cmpiToIntel(pCmpiObjectPath, pCmpiInstance, &status);

					if (status.rc == CMPI_RC_OK) // make sure no errors so far
					{
						wbem::framework::attributes_t attributes;

						attributes = getModifiedAttributes(pCurrentInstance, pNewInstance);
						pModifiedInstance = pProvider->modifyInstance(objectPath, attributes);
						if (pModifiedInstance != NULL)
						{
							CMPIInstance *pNewCmpiInstance = intelToCmpi(g_pBroker, pModifiedInstance, &status);
							if (status.rc == CMPI_RC_OK)
							{
								CMReturnInstance(pResult, pNewCmpiInstance);
							}
						}

					}
				}
				catch(wbem::framework::ExceptionBadParameter &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
				}
				catch(wbem::framework::ExceptionSystemError &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
				}
				catch(wbem::framework::ExceptionNoMemory &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM,
							"System ran out of memory");
				}
				catch(wbem::framework::ExceptionNotSupported &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
				}
				catch(wbem::framework::Exception &e)
				{
					CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
				}
				delete (pNewInstance);
				delete (pCurrentInstance);
				delete (pModifiedInstance);
				delete (pProvider);
			}
		}
		pProviderFactory->CleanUpProvider();
	}

	CMReturnDone (pResult);
	COMMON_LOG_INFO_F("Returning status %d", status.rc);

	return status;
}

/*
 * Delete an instance using an object path as reference
 */
CMPIStatus Generic_DeleteInstance
		(CMPIInstanceMI *cThis, const CMPIContext *ctx, const CMPIResult *rslt,
				const CMPIObjectPath *cop)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	// TODO: Implement when needed
	CMReturn (CMPI_RC_ERR_NOT_SUPPORTED);
}

/*
 * query the enumeration of instances of the class and subclass defined by an object path
 */
CMPIStatus Generic_ExecQuery
		(CMPIInstanceMI *cThis, const CMPIContext *ctx, const CMPIResult *rslt,
				const CMPIObjectPath *cop, const char *lang, const char *query)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	// TODO: Implement when needed
	CMReturn (CMPI_RC_ERR_NOT_SUPPORTED);
}


/*
 * Cleanup prior to unloading the provider
 */
CMPIStatus Generic_AssociationCleanup(CMPIAssociationMI *mi, const CMPIContext *ctx, CMPIBoolean terminating)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMReturn (CMPI_RC_OK);
}

/*
 * Get Associated Classes of an instance
 */
CMPIStatus Generic_Associators(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op, const char *assocClass,
		const char *resultClass, const char *role, const char *resultRole, const char **properties)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("assocClass: %s", assocClass);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		wbem::framework::ObjectPath objectPath;
		cmpiToIntel(op, &objectPath, &status);

		COMMON_LOG_DEBUG_F("class: %s", objectPath.getClass().c_str());
		wbem::framework::InstanceFactory *pFactory =
				pProviderFactory->getInstanceFactory(objectPath.getClass());

		if (pFactory != NULL)
		{
			wbem::framework::instances_t *pInstances = NULL;
			try
			{
				std::string assocClassStr = assocClass != NULL
						? assocClass
						: objectPath.getClass();
				std::string resultClassStr = resultClass != NULL ? resultClass : "";
				std::string roleStr = role != NULL ? role : "";
				std::string resultRoleStr = resultRole != NULL ? resultRole : "";

				pInstances = pFactory->associatorInstances(objectPath,
						assocClassStr, resultClassStr, roleStr, resultRoleStr);
			}
			catch(wbem::framework::ExceptionBadParameter &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
			}
			catch(wbem::framework::ExceptionSystemError &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
			}
			catch(wbem::framework::ExceptionNotSupported &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
			}
			catch(wbem::framework::Exception &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
			}

			if (pInstances != NULL)
			{
				if (status.rc == CMPI_RC_OK)
				{
					wbem::framework::instances_t::iterator iInstance = pInstances->begin();
					for (; iInstance != pInstances->end(); iInstance++)
					{
						CMPIInstance *pCmpiInstance =
								intelToCmpi(g_pBroker, &(*iInstance), &status);
						CMReturnInstance(rslt, pCmpiInstance);
					}
				}
				delete pInstances;
			}
			delete pFactory;
		}
		else
		{
			CMSetStatus(&status, CMPI_RC_ERR_INVALID_CLASS);
		}
		pProviderFactory->CleanUpProvider();
	}

	COMMON_LOG_INFO_F("Returning status %d", status.rc);
	return status;
}

/*
 * Get Object Paths for Associated Classes
 */
CMPIStatus Generic_AssociatorNames(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op, const char *assocClass,
		const char *resultClass, const char *role, const char *resultRole)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		try
		{
			COMMON_LOG_DEBUG_F("assocClass: %s", assocClass);
			wbem::framework::ObjectPath objectPath;
			cmpiToIntel(op, &objectPath, &status);
			COMMON_LOG_DEBUG_F("Class Name: %s", objectPath.getClass().c_str());

			wbem::framework::InstanceFactory *pFactory =
					pProviderFactory->getInstanceFactory(objectPath.getClass());
			if (pFactory != NULL)
			{
				std::string assocClassStr = assocClass != NULL
						? assocClass
						: objectPath.getClass();
				std::string resultClassStr = resultClass != NULL ? resultClass : "";
				std::string roleStr = role != NULL ? role : "";
				std::string resultRoleStr = resultRole != NULL ? resultRole : "";

				wbem::framework::instance_names_t *pObjectPaths =
						pFactory->associatorNames(objectPath, assocClassStr, resultClassStr,
								roleStr, resultRoleStr);
				wbem::framework::instance_names_t::iterator iter = pObjectPaths->begin();
				for (; iter != pObjectPaths->end(); iter++)
				{
					COMMON_LOG_DEBUG_F("Adding ObjectPath ClassName to result:  [%s]", iter->getClass().c_str());
					CMPIObjectPath *cmpiObjectPath = intelToCmpi(g_pBroker, &(*iter), &status);

					CMReturnObjectPath(rslt, cmpiObjectPath);
				}

				delete pObjectPaths;
				delete pFactory;
			}
		}
		catch(wbem::framework::Exception &e)
		{
			CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_FAILED, e.what());
			COMMON_LOG_ERROR_F("An unknown error occurred getting AssociatorNames:", e.what());
		}
		pProviderFactory->CleanUpProvider();
	}

	CMReturnDone(rslt);
	COMMON_LOG_INFO_F("Returning status %d", status.rc);
	return status;
}

/*
 * Get instances of an Association Class for an instance
 */
CMPIStatus Generic_References(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op,
		const char *resultClass, const char *role, const char **properties)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		wbem::framework::ObjectPath objectPath;
		cmpiToIntel(op, &objectPath, &status);

		wbem::framework::InstanceFactory *pFactory =
				pProviderFactory->getInstanceFactory(objectPath.getClass());
		if (pFactory != NULL)
		{
			wbem::framework::instances_t *pInstances = NULL;
			try
			{
				std::string resultClassStr = resultClass != NULL ? resultClass
																 : objectPath.getClass();
				std::string roleStr = role != NULL ? role : "";

				// references are the association class so assocClass = resultClass
				pInstances = pFactory->referenceInstances(objectPath, resultClassStr, "", roleStr);
				if (pInstances != NULL)
				{
					COMMON_LOG_DEBUG_F("got %llu instances", pInstances->size());
					if (status.rc == CMPI_RC_OK)
					{
						wbem::framework::instances_t::iterator iInstance = pInstances->begin();
						for (; iInstance != pInstances->end(); iInstance++)
						{
							COMMON_LOG_DEBUG_F("Converting instance %s to CMPI",
									iInstance->getObjectPath().asString().c_str());

							CMPIObjectPath *cmpiObjectPath = CMNewObjectPath (g_pBroker,
									CMGetCharsPtr(CMGetNameSpace(op, &status), NULL),
									resultClassStr.c_str(), &status);

							wbem::framework::attributes_t keys = iInstance->getObjectPath().getKeys();
							wbem::framework::attributes_t::iterator key = keys.begin();
							std::map<std::string, CMPIData> cmpiKeys;
							for (; key != keys.end(); key++)
							{
								COMMON_LOG_DEBUG_F("converting key %s", key->second.asStr().c_str());
								CMPIData cmpiAttribute;
								wbem::framework::ObjectPath objectPath;
								wbem::framework::ObjectPathBuilder(key->second.asStr()).Build(&objectPath);

								CMPIObjectPath *refPath = intelToCmpi(g_pBroker, &objectPath, &status);

								COMMON_LOG_DEBUG_F("creating CMPI attribute with key %s", key->first.c_str());
								cmpiAttribute.state = CMPI_keyValue;
								cmpiAttribute.type = CMPI_ref;
								cmpiAttribute.value.ref = refPath;
								cmpiKeys[key->first] = cmpiAttribute;
								CMAddKey(cmpiObjectPath, key->first.c_str(), &(cmpiAttribute.value), cmpiAttribute.type);
							}

							CMPIInstance *pCmpiInstance = CMNewInstance(g_pBroker, cmpiObjectPath, &status);

							std::map<std::string, CMPIData>::iterator iCmpiKey = cmpiKeys.begin();
							for (; iCmpiKey != cmpiKeys.end(); iCmpiKey++)
							{
								COMMON_LOG_DEBUG_F("Setting key property %s", iCmpiKey->first.c_str());
								CMSetProperty(pCmpiInstance, iCmpiKey->first.c_str(),
										&(iCmpiKey->second.value), iCmpiKey->second.type);
							}

							CMReturnInstance(rslt, pCmpiInstance);
						}
					}
					delete pInstances;
				}
			}
			catch(wbem::framework::ExceptionBadParameter &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
			}
			catch(wbem::framework::ExceptionSystemError &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
			}
			catch(wbem::framework::ExceptionNotSupported &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
			}
			catch(wbem::framework::Exception &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
			}
			delete pFactory;
		}
		pProviderFactory->CleanUpProvider();
	}

	CMReturnDone(rslt);

	COMMON_LOG_INFO_F("Returning status %d", status.rc);
	return status;
}

/*
 * Get Object Paths of an Association Class for an instance
 */
CMPIStatus Generic_ReferenceNames(CMPIAssociationMI *mi, const CMPIContext *ctx,
		const CMPIResult *rslt, const CMPIObjectPath *op,
		const char *resultClass, const char *role)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIStatus status = {CMPI_RC_OK, 0};

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		wbem::framework::ObjectPath objectPath;
		cmpiToIntel(op, &objectPath, &status);

		wbem::framework::InstanceFactory *pFactory =
				pProviderFactory->getInstanceFactory(objectPath.getClass());
		if (pFactory != NULL)
		{
			try
			{
				std::string resultClassStr = resultClass != NULL ? resultClass
						: objectPath.getClass();
				std::string roleStr = role != NULL ? role : "";

				// references are the association class so assocClass = resultClass
				wbem::framework::instance_names_t *pNames = pFactory->referenceNames(objectPath,
						resultClassStr, "", roleStr);

				wbem::framework::instance_names_t::iterator name = pNames->begin();
				for (; name != pNames->end(); name++)
				{
					CMPIObjectPath *cmpiObjectPath =
							CMNewObjectPath (g_pBroker,
									CMGetCharsPtr(CMGetNameSpace(op, &status), NULL), resultClassStr.c_str(), &status);

					wbem::framework::attributes_t keys = name->getKeys();
					wbem::framework::attributes_t::iterator key = keys.begin();
					for (; key != keys.end(); key++)
					{
						CMPIData cmpiAttribute;
						wbem::framework::ObjectPath objectPath;
						wbem::framework::ObjectPathBuilder(key->second.asStr()).Build(&objectPath);

						CMPIObjectPath *refPath = intelToCmpi(g_pBroker, &objectPath, &status);

						cmpiAttribute.state = CMPI_keyValue;
						cmpiAttribute.type = CMPI_ref;
						cmpiAttribute.value.ref = refPath;
						CMAddKey(cmpiObjectPath, key->first.c_str(), &(cmpiAttribute.value), cmpiAttribute.type);
					}

					CMReturnObjectPath(rslt, cmpiObjectPath);
				}
				delete pNames;
			}
			catch(wbem::framework::ExceptionBadParameter &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_INVALID_PARAMETER, e.what());
			}
			catch(wbem::framework::ExceptionSystemError &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR_SYSTEM, e.what());
			}
			catch(wbem::framework::ExceptionNotSupported &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_NOT_SUPPORTED, e.what());
			}
			catch(wbem::framework::Exception &e)
			{
				CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERROR, e.what());
			}
			delete pFactory;
		}
		else
		{
			CMSetStatus(&status, CMPI_RC_ERR_INVALID_CLASS);
		}
		pProviderFactory->CleanUpProvider();
	}

	CMReturnDone(rslt);

	COMMON_LOG_INFO_F("Returning status %d", status.rc);
	return status;
}

CMPIStatus Generic_MethodCleanup(CMPIMethodMI *mi, const CMPIContext *ctx,
		CMPIBoolean terminating)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return status;
}

/*
 * Execute a method
 */
CMPIStatus Generic_InvokeMethod(CMPIMethodMI *mi, const CMPIContext *ctx, const CMPIResult *rslt,
		const CMPIObjectPath *op, const char *method, const CMPIArgs *in, CMPIArgs *out)
{
	CMPIStatus status = {CMPI_RC_ERR_CLASS_HAS_INSTANCES, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("MI Name: %s", mi->ft->miName);
	COMMON_LOG_DEBUG_F("Method: %s", method);
	wbem::framework::UINT32 wbemRc = CMPI_RC_OK;

	wbem::framework::ProviderFactory *pProviderFactory =
			wbem::framework::ProviderFactory::getSingleton();
	if (pProviderFactory == NULL)
	{
		status.rc = CMPI_RC_ERROR;
	}
	else
	{
		pProviderFactory->InitializeProvider();

		wbem::framework::ObjectPath path;
		cmpiToIntel(op, &path, &status);
		if (status.rc == CMPI_RC_OK)
		{
			COMMON_LOG_DEBUG_F("Object Class: %s", path.getClass().c_str());
			CMPICount inArgCount = CMGetArgCount(in, &status);
			if (status.rc == CMPI_RC_OK)
			{
				COMMON_LOG_DEBUG_F("In Arg Count: %d", inArgCount);
				wbem::framework::InstanceFactory *pFactory =
						pProviderFactory->getInstanceFactory(path.getClass());
				if (pFactory != NULL)
				{
					// build up the arguments
					wbem::framework::attributes_t inAttributes;
					for (CMPICount i = 0; i < inArgCount; i++)
					{
						CMPIStatus tempStatus = {CMPI_RC_OK, 0};
						CMPIString *pName;
						CMPIData arg = CMGetArgAt(in, i, &pName, &tempStatus);
						KEEP_ERR(status, tempStatus);
						if (tempStatus.rc == CMPI_RC_OK)
						{
							wbem::framework::Attribute *pAttribute = cmpiToIntel(&arg, false, &tempStatus);
							KEEP_ERR(status, tempStatus);
							if (tempStatus.rc == CMPI_RC_OK)
							{
								const char *name = CMGetCharsPtr(pName, &tempStatus);
								KEEP_ERR(status, tempStatus);
								inAttributes[name] = *pAttribute;
							}
							delete pAttribute;
						}
					}

					wbem::framework::attributes_t outAttributes;

					// Execute the method - the results of executeMethod are standard CIM error codes so
					// can cast to a CMPIrc
					wbem::framework::UINT32 httpRc = pFactory->executeMethod(wbemRc,
							std::string(method), path, inAttributes, outAttributes);
					if (httpRc != wbem::framework::MOF_ERR_SUCCESS) // http errors trump wbem errors
					{
						status.rc = (CMPIrc) httpRc;
					}

					// convert outAttributes back to out CMPIArgs
					// NOTE: this isn't tested yet because the
					// extrinsic methods implemented don't have out arguments
					wbem::framework::attributes_t::iterator iAttribute = outAttributes.begin();
					for (; iAttribute != outAttributes.end(); iAttribute++)
					{
						CMPIData cmpiAttribute;
						intelToCmpi(g_pBroker, &(iAttribute->second), &cmpiAttribute, &status);
						CMAddArg(out, iAttribute->first.c_str(), &(cmpiAttribute.value), cmpiAttribute.type);
					}
					delete pFactory;
				}
			}
		}
		else
		{
			status.rc = CMPI_RC_ERR_INVALID_CLASS;
			COMMON_LOG_ERROR_F("Could not get instance factory for %s", path.getClass().c_str());
		}
		pProviderFactory->CleanUpProvider();
	}

	// this should be the wbemRc code returned here
	CMReturnData (rslt, (CMPIValue *) &wbemRc, CMPI_uint32);
	CMReturnDone(rslt);

	// TODO: this should be the httpRc code that is returned here?? then why do we set it to wbemrc earlier??
	COMMON_LOG_INFO_F("Returning status %d", status.rc);
	COMMON_LOG_INFO_F("Status.rc return value: %d, wbemRc return value: %d", status.rc, wbemRc);

	return status;
}

/*
 * -------------------------------------------------------------------------------------------------
 * Indication Provider CMPI Entry Point Functions
 * -------------------------------------------------------------------------------------------------
 */

/*
 * Activate the subscription.
 */
CMPIStatus Generic_ActivateFilter(CMPIIndicationMI *mi,
		const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *classPath,
		CMPIBoolean firstActivation)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	if (g_enabled == 0)
	{
		CMPIContext *context = CBPrepareAttachThread (g_pBroker, ctx);

		try
		{
			g_pCimomContext = new CmpiAdapter (context, g_pBroker);

			IndicationService *pService =
					ProviderFactory::getSingleton()->getIndicationService();
			pService->startIndicating(g_pCimomContext);
			g_enabled++;
		}
		catch (Exception &e)
		{
			COMMON_LOG_ERROR_F("Failed to create indication subscription: %s", e.what());
			CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_FAILED, e.what());
		}
	}
	else
	{
		g_enabled++;
	}

	return status;
}

/*
 * The subscription was removed.
 */
CMPIStatus Generic_DeActivateFilter(CMPIIndicationMI *mi,
		const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *classPath,
		CMPIBoolean lastActivation)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	g_enabled--;
	if(g_enabled == 0)
	{
		IndicationService *pService =
				ProviderFactory::getSingleton()->getIndicationService();
		try
		{
			pService->stopIndicating();
		}
		catch (Exception &e)
		{
			COMMON_LOG_ERROR_F("Failed to delete indication subscription: %s", e.what());
			CMSetStatusWithChars(g_pBroker, &status, CMPI_RC_ERR_FAILED, e.what());
		}
	}

	return status;
}
/*
 * Required Indication Provider Functions. They should all just return CMPI_RC_OK
 */
CMPIStatus Generic_IndicationCleanup(CMPIIndicationMI *mi, const CMPIContext *ctx,
		CMPIBoolean terminating)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return status;
}
CMPIStatus Generic_AuthorizeFilter(CMPIIndicationMI *mi,
		const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *op,
		const char *owner)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return status;
}
CMPIStatus Generic_MustPoll(CMPIIndicationMI *mi, const CMPIContext *ctx,
		const CMPISelectExp *filter,
		const char *className,
		const CMPIObjectPath *classPath)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return status;
}
CMPIStatus Generic_EnableIndications(CMPIIndicationMI *mi, const CMPIContext *)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return status;
}

/*
 * Required Indication Provider Function
 */
CMPIStatus Generic_DisableIndications(CMPIIndicationMI *mi, const CMPIContext *)
{
	CMPIStatus status = {CMPI_RC_OK, 0};
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return status;
}

}
}
