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
 * This file contains the implementation of conversion functions between the
 * Intel WBEM implementation to CMPI.
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <cmpi/cmpift.h>

// Intel CIM Framework
#include "ObjectPathBuilder.h"
#include "Strings.h"
#include "Attribute.h"
#include "ProviderFactory.h"
#include "logging.h"
#include "common_types.h"

#include "IntelToCmpi.h"

namespace wbem
{
namespace framework
{

const std::string PEGASUS = "Pegasus";

// Convert an Instance to a CMPIInstance
CMPIInstance * intelToCmpi(const CMPIBroker * pBroker, wbem::framework::Instance *pInstance, CMPIStatus *pStatus)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPIInstance *pCmpiInstance = NULL;

	if (pStatus == NULL)
	{
		COMMON_LOG_ERROR("Parameter pStatus cannot be NULL");
	}
	else if (pBroker == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pBroker cannot be NULL");
	}
	else if (pInstance == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pInstance cannot be NULL");
	}
	else
	{
		wbem::framework::ObjectPath objPath = pInstance->getObjectPath();
		CMPIObjectPath *pObjPath = intelToCmpi(pBroker, &(objPath), pStatus);

		if (pObjPath != NULL && pStatus->rc == CMPI_RC_OK)
		{
			pCmpiInstance = CMNewInstance (pBroker, pObjPath, pStatus);
			if (pCmpiInstance != NULL && pStatus->rc == CMPI_RC_OK)
			{
				wbem::framework::attributes_t::const_iterator iAttribute = pInstance->attributesBegin();
				for (; iAttribute != pInstance->attributesEnd(); iAttribute++)
				{
					std::string attrName = (*iAttribute).first;
					wbem::framework::Attribute attr = (*iAttribute).second;
					COMMON_LOG_DEBUG_F("Converting Attribute to CmpiAttribute: %s - %d",
							attrName.c_str(), attr.getType());
					CMPIData cmpiAttribute;

					intelToCmpi(pBroker, &attr, &cmpiAttribute, pStatus);

					if (pStatus->rc == CMPI_RC_OK)
					{
						CMSetProperty (pCmpiInstance, attrName.c_str(), &cmpiAttribute.value, cmpiAttribute.type);
					}
					else
					{
						COMMON_LOG_ERROR_F("Error (%d) converting instance property '%s' to CMPI", pStatus->rc, attrName.c_str());
					}
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPIInstance conversion failed for Object Path: %s", objPath.asString().c_str());
			}
		}
	}
	return pCmpiInstance;

}

// Convert Attribute to CMPI Attribute
void intelToCmpi(const CMPIBroker *pBroker, wbem::framework::Attribute *pAttribute, CMPIData *pCmpiAttribute, CMPIStatus *pStatus)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	if (pStatus == NULL)
	{
		COMMON_LOG_ERROR("Parameter pStatus cannot be NULL");
	}
	else if (pBroker == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pBroker cannot be NULL");
	}
	else if (pAttribute == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pAttribute cannot be NULL");

	}
	else if (pCmpiAttribute == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pCmpiAttribute cannot be NULL");
	}
	else
	{
		// Make sure status is OK to start
		pStatus->rc = CMPI_RC_OK;

		switch (pAttribute->getType())
		{
		case wbem::framework::BOOLEAN_T:
			pCmpiAttribute->value.boolean = pAttribute->boolValue();
			pCmpiAttribute->type = CMPI_boolean;
			break;
		case wbem::framework::UINT8_T:
			pCmpiAttribute->value.uint8 = pAttribute->uintValue();
			pCmpiAttribute->type = CMPI_uint8;
			break;
		case wbem::framework::ENUM16_T:
		case wbem::framework::UINT16_T:
			pCmpiAttribute->value.uint16 = pAttribute->uintValue();
			pCmpiAttribute->type = CMPI_uint16;
			break;
		case wbem::framework::ENUM_T:
		case wbem::framework::UINT32_T:
			pCmpiAttribute->value.uint32 = pAttribute->uintValue();
			pCmpiAttribute->type = CMPI_uint32;
			break;
		case wbem::framework::UINT64_T:
			pCmpiAttribute->value.uint64 = pAttribute->uint64Value();
			pCmpiAttribute->type = CMPI_uint64;
			break;
		case wbem::framework::SINT8_T:
			pCmpiAttribute->value.sint8 = pAttribute->intValue();
			pCmpiAttribute->type = CMPI_sint8;
			break;
		case wbem::framework::SINT16_T:
			pCmpiAttribute->value.sint16 = pAttribute->intValue();
			pCmpiAttribute->type = CMPI_sint16;
			break;
		case wbem::framework::SINT32_T:
			pCmpiAttribute->value.sint32 = pAttribute->intValue();
			pCmpiAttribute->type = CMPI_sint32;
			break;
		case wbem::framework::SINT64_T:
			pCmpiAttribute->value.sint64 = pAttribute->sint64Value();
			pCmpiAttribute->type = CMPI_sint64;
			break;
		case wbem::framework::STR_T:
		{
			// Pegasus expects an actual embedded instance while SFCB expect a string
			// of CIM-XML. Looking at broker name to determine what to do.
			if (pAttribute->isEmbedded())
			{
				pCmpiAttribute->type = CMPI_ENC;
				wbem::framework::Instance embedded(pAttribute->stringValue());
				CMPIInstance * cmpiEmbedded = intelToCmpi(pBroker, &embedded, pStatus);
				pCmpiAttribute->value.inst = cmpiEmbedded;
			}
			// handles enumerating association classes
			else if (pAttribute->isAssociationClassInstance())
			{
				pCmpiAttribute->type = CMPI_ref;
				wbem::framework::ObjectPathBuilder builder(pAttribute->stringValue());
				wbem::framework::ObjectPath path;
				builder.Build(&path);
				pCmpiAttribute->value.ref =  intelToCmpi(pBroker, &(path), pStatus);
			}
			else
			{
				pCmpiAttribute->value.string =
					CMNewString(pBroker, pAttribute->stringValue().c_str(), pStatus);
				pCmpiAttribute->type = CMPI_string;
			}

			break;
		}
		case wbem::framework::DATETIME_INTERVAL_T:
		case wbem::framework::DATETIME_T:
		{
			pCmpiAttribute->value.dateTime =
					CMNewDateTimeFromChars(pBroker, pAttribute->stringValue().c_str(), pStatus);
			pCmpiAttribute->type = CMPI_dateTime;
			break;
		}

		case wbem::framework::UINT8_LIST_T:
		{
			// TODO: This implementation is untested. When a class with a uint8[] is implemented, pay special attention to this code
			pCmpiAttribute->value.array = CMNewArray(pBroker, pAttribute->uint8ListValue().size(), CMPI_uint8, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pCmpiAttribute->type = CMPI_uint8A;
				for (int i = 0; i < (int)pAttribute->uint8ListValue().size(); i++)
				{
					CMPIValue value;
					value.uint8 = pAttribute->uint8ListValue()[i];
					pCmpiAttribute->value.array->ft->setElementAt(pCmpiAttribute->value.array, i, &value, CMPI_uint8);
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPI error converting a UINT8_LIST to a CMPI uint8 array. Error %d", pStatus->rc);
			}
			break;
		}
		case wbem::framework::UINT16_LIST_T:
		{
			// TODO: This implementation is untested. When a class with a uint16[] is implemented, pay special attention to this code
			pCmpiAttribute->value.array = CMNewArray(pBroker, pAttribute->uint16ListValue().size(), CMPI_uint16, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pCmpiAttribute->type = CMPI_uint16A;
				for (int i = 0; i < (int)pAttribute->uint16ListValue().size(); i++)
				{
					CMPIValue value;
					value.uint16 = pAttribute->uint16ListValue()[i];
					pCmpiAttribute->value.array->ft->setElementAt(pCmpiAttribute->value.array, i, &value, CMPI_uint16);
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPI error converting a UINT16_LIST to a CMPI uint16 array. Error %d", pStatus->rc);
			}
			break;
		}
		case wbem::framework::UINT32_LIST_T:
		{
			// TODO: This implementation is untested. When a class with a uint32[] is implemented, pay special attention to this code
			pCmpiAttribute->value.array = CMNewArray(pBroker, pAttribute->uint32ListValue().size(), CMPI_uint32, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pCmpiAttribute->type = CMPI_uint32A;
				for (int i = 0; i < (int)pAttribute->uint32ListValue().size(); i++)
				{
					CMPIValue value;
					value.uint32 = pAttribute->uint32ListValue()[i];
					pCmpiAttribute->value.array->ft->setElementAt(pCmpiAttribute->value.array, i, &value, CMPI_uint32);
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPI error converting a UINT32_LIST to a CMPI uint32 array. Error %d", pStatus->rc);
			}
			break;
		}
		case wbem::framework::UINT64_LIST_T:
		{
			// TODO: This implementation is untested. When a class with a uint64[] is implemented, pay special attention to this code
			pCmpiAttribute->value.array = CMNewArray(pBroker, pAttribute->uint64ListValue().size(), CMPI_uint64, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pCmpiAttribute->type = CMPI_uint64A;
				for (int i = 0; i < (int)pAttribute->uint64ListValue().size(); i++)
				{
					CMPIValue value;
					value.uint64 = pAttribute->uint64ListValue()[i];
					pCmpiAttribute->value.array->ft->setElementAt(pCmpiAttribute->value.array, i, &value, CMPI_uint64);
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPI error converting a UINT64_LIST to a CMPI uint64 array. Error %d", pStatus->rc);
			}
			break;
		}
		case wbem::framework::STR_LIST_T:
		{
			// TODO: This implementation is untested. When a class with a string[] is implemented, pay special attention to this code
			pCmpiAttribute->value.array = CMNewArray(pBroker, pAttribute->strListValue().size(), CMPI_string, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pCmpiAttribute->type = CMPI_stringA;
				for (int i = 0; i < (int)pAttribute->strListValue().size(); i++)
				{
					CMPIValue value;
					value.string = CMNewString(pBroker, pAttribute->strListValue()[i].c_str(), pStatus);
					if (pStatus->rc == CMPI_RC_OK)
					{
						pCmpiAttribute->value.array->ft->setElementAt(pCmpiAttribute->value.array, i, &value, CMPI_string);
					}
					else
					{
						COMMON_LOG_ERROR_F("CMPI error creating a new CMPI string. Error %d", pStatus->rc);
					}
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPI error converting a STRING_LIST to a CMPI string array. Error %d", pStatus->rc);
			}
			break;
		}
		case wbem::framework::BOOLEAN_LIST_T:
		{
			pCmpiAttribute->value.array = CMNewArray(pBroker, pAttribute->booleanListValue().size(), CMPI_boolean, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pCmpiAttribute->type = CMPI_booleanA;
				for (int i = 0; i < (int)pAttribute->booleanListValue().size(); i++)
				{
					CMPIValue value;
					value.boolean = pAttribute->booleanListValue().at(i);
					pCmpiAttribute->value.array->ft->setElementAt(pCmpiAttribute->value.array, i, &value, CMPI_boolean);
				}
			}
			else
			{
				COMMON_LOG_ERROR_F("CMPI error converting a BOOLEAN_LIST to a CMPI boolean array. Error %d", pStatus->rc);
			}
			break;
		}
		default:
			COMMON_LOG_ERROR("Tried to convert an attribute with an invalid data type");
			CMSetStatus(pStatus, CMPI_RC_ERR_INVALID_DATA_TYPE)
			;
			break;
		}
	}

}

// check if attribute exists and is a key attribute
bool isAttributeKey(wbem::framework::Instance *pNewInstance, std::string attributeName)
{
	bool isKey = false;
	wbem::framework::Attribute attribute;
	if ((pNewInstance->getAttribute(attributeName, attribute) == wbem::framework::SUCCESS) && attribute.isKey())
	{
		isKey = true;
	}
	return isKey;
}

// Take a CMPI Instance and create a new Instance
wbem::framework::Instance *cmpiToIntel(const CMPIObjectPath *pCmpiObjectPath, const CMPIInstance *pCmpiInstance,
		CMPIStatus *pStatus)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	CMPICount count = CMGetPropertyCount(pCmpiInstance,pStatus);
	wbem::framework::Instance *pNewInstance = NULL;

	if (pStatus->rc == CMPI_RC_OK)
	{
		wbem::framework::ObjectPath newInstanceObjPath;
		cmpiToIntel(pCmpiObjectPath, &newInstanceObjPath, pStatus);
		pNewInstance = new wbem::framework::Instance(newInstanceObjPath);

		for (CMPICount i = 0; i < count; i++)
		{
			CMPIString *pName;
			CMPIStatus tempStatus;
			bool isKey = false;

			CMPIData cmpiInstanceProp = CMGetPropertyAt(pCmpiInstance, i, &pName, &tempStatus);
			KEEP_ERR(*pStatus, tempStatus);

			if (tempStatus.rc == CMPI_RC_OK)
			{
				char *name = CMGetCharPtr(pName);
				std::string attributeName(name);
				if (cmpiInstanceProp.state == CMPI_goodValue)
				{
					// Get attribute and check if the attribute is a key
					isKey = isAttributeKey(pNewInstance, attributeName);
					wbem::framework::Attribute *pAttribute = cmpiToIntel(&cmpiInstanceProp, isKey, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);

					if (tempStatus.rc == CMPI_RC_OK && pAttribute != NULL)
					{
						pNewInstance->setAttribute(attributeName, *pAttribute);
					}
					delete (pAttribute);
				}
			}
		}
	}
	else
	{
		COMMON_LOG_ERROR("Unable to get property count");
	}

	return pNewInstance;
}

// Take a CMPI Attribute and create a new Attribute
wbem::framework::Attribute* cmpiToIntel(CMPIData *pCmpiAttribute, bool isKey, CMPIStatus *pStatus)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	wbem::framework::Attribute *pResult = NULL;
	if (pStatus == NULL)
	{
		COMMON_LOG_ERROR("Parameter pStatus cannot be NULL");
	}
	else if (pCmpiAttribute == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pCmpiAttribute cannot be NULL");

	}
	else
	{
		COMMON_LOG_DEBUG_F("CMPI attribute type [%u]", pCmpiAttribute->type);

		switch(pCmpiAttribute->type)
		{
		case CMPI_boolean      :
			pResult =  new wbem::framework::Attribute((bool)pCmpiAttribute->value.boolean, isKey);
			break;
		case CMPI_char16       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.char16, isKey);
			break;
		case CMPI_uint8        :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.uint8, isKey);
			break;
		case CMPI_uint16       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.uint16, isKey);
			break;
		case CMPI_uint32       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.uint32, isKey);
			break;
		case CMPI_uint64       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.uint64, isKey);
			break;
		case CMPI_sint8        :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.sint8, isKey);
			break;
		case CMPI_sint16       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.sint16, isKey);
			break;
		case CMPI_sint32       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.sint32, isKey);
			break;
		case CMPI_sint64       :
			pResult =  new wbem::framework::Attribute(pCmpiAttribute->value.sint64, isKey);
			break;
		case CMPI_string       :
		{
			pResult =  new wbem::framework::Attribute(std::string(CMGetCharPtr(pCmpiAttribute->value.string)), isKey);
			break;
		}
		case CMPI_uint8A      :
		{
			// TODO: This implementation is untested. When a class with a uint8[] is implemented, pay special attention to this code
			std::vector<COMMON_UINT8> list;
			int size = CMGetArrayCount(pCmpiAttribute->value.array, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				for (int i = 0; i < size; i++)
				{
					CMPIStatus tempStatus;
					CMPIData data = CMGetArrayElementAt(pCmpiAttribute->value.array, i, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (tempStatus.rc == CMPI_RC_OK)
					{
						list.push_back(data.value.uint8);
					}
				}
			}
			if (pStatus->rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CMPI error converting a CMPI uint8 array to a UINT8_List. Error %d", pStatus->rc);
			}
			// create even if there's an error (it will just be empty)
			pResult = new wbem::framework::Attribute(list, isKey);
			break;
		}
		case CMPI_uint16A      :
		{
			// TODO: This implementation is untested. When a class with a uint16[] is implemented, pay special attention to this code
			std::vector<COMMON_UINT16> list;
			int size = CMGetArrayCount(pCmpiAttribute->value.array, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				for (int i = 0; i < size; i++)
				{
					CMPIStatus tempStatus;
					CMPIData data = CMGetArrayElementAt(pCmpiAttribute->value.array, i, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (tempStatus.rc == CMPI_RC_OK)
					{
						list.push_back(data.value.uint16);
					}
				}
			}
			if (pStatus->rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CMPI error converting a CMPI uint16 array to a UINT16_List. Error %d", pStatus->rc);
			}
			// create even if there's an error (it will just be empty)
			pResult = new wbem::framework::Attribute(list, isKey);
			break;
		}
		case CMPI_uint32A      :
		{
			// TODO: This implementation is untested. When a class with a uint32[] is implemented, pay special attention to this code
			std::vector<COMMON_UINT32> list;
			int size = CMGetArrayCount(pCmpiAttribute->value.array, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				for (int i = 0; i < size; i++)
				{
					CMPIStatus tempStatus;
					CMPIData data = CMGetArrayElementAt(pCmpiAttribute->value.array, i, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (tempStatus.rc == CMPI_RC_OK)
					{
						list.push_back(data.value.uint32);
					}
				}
			}
			if (pStatus->rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CMPI error converting a CMPI uint32 array to a UINT32_List. Error %d", pStatus->rc);
			}
			// create even if there's an error (it will just be empty)
			pResult = new wbem::framework::Attribute(list, isKey);
			break;
		}
		case CMPI_stringA      :
		{
			// TODO: This implementation is untested. When a class with a string[] is implemented, pay special attention to this code
			std::vector<std::string> list;
			int size = CMGetArrayCount(pCmpiAttribute->value.array, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				for (int i = 0; i < size; i++)
				{
					CMPIStatus tempStatus;
					CMPIData data = CMGetArrayElementAt(pCmpiAttribute->value.array, i, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (tempStatus.rc == CMPI_RC_OK)
					{
						if (data.value.string != NULL)
						{
							const char *value = CMGetCharsPtr(data.value.string, &tempStatus);
							KEEP_ERR(*pStatus, tempStatus);
							if (tempStatus.rc == CMPI_RC_OK)
							{
								std::string valueStr(value);
								list.push_back(valueStr);
							}
						}
						else
						{
							std::string valueStr = "";
							list.push_back(valueStr);
						}
					}
				}
			}
			if (pStatus->rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CMPI error converting a CMPI string array to a STR_List. Error %d", pStatus->rc);
			}
			// create even if there's an error (it will just be empty)
			pResult = new wbem::framework::Attribute(list, isKey);
			break;
		}
		case CMPI_ref          :
		{
			wbem::framework::ObjectPath objectPath;
			cmpiToIntel(pCmpiAttribute->value.ref, &objectPath, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				pResult = new wbem::framework::Attribute(objectPath.asString(), isKey);
			}
			else
			{
				COMMON_LOG_ERROR("Couldn't convert the ref attribute to an ObjectPath");
			}
			break;
		}
		case CMPI_booleanA      :
		{
			// TODO: This implementation is untested. When a class with a boolean[] is implemented, pay special attention to this code
			std::vector<BOOLEAN> list;
			int size = CMGetArrayCount(pCmpiAttribute->value.array, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				for (int i = 0; i < size; i++)
				{
					CMPIStatus tempStatus;
					CMPIData data = CMGetArrayElementAt(pCmpiAttribute->value.array, i, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (tempStatus.rc == CMPI_RC_OK)
					{
						list.push_back(data.value.boolean);
					}
				}
			}
			if (pStatus->rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CMPI error converting a CMPI boolean array to a BOOLEAN_LIST. Error %d", pStatus->rc);
			}
			// create even if there's an error (it will just be empty)
			pResult = new wbem::framework::Attribute(list, isKey);
			break;
		}
		case CMPI_refA         :
		{
			std::vector<std::string> list;
			int size = CMGetArrayCount(pCmpiAttribute->value.array, pStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				for (int i = 0; i < size; i++)
				{
					CMPIStatus tempStatus;
					CMPIData data = CMGetArrayElementAt(pCmpiAttribute->value.array, i, &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (tempStatus.rc == CMPI_RC_OK)
					{
						wbem::framework::ObjectPath objectPath;
						cmpiToIntel(data.value.ref, &objectPath, &tempStatus);
						KEEP_ERR(*pStatus, tempStatus);
						if (tempStatus.rc == CMPI_RC_OK)
						{
							list.push_back(objectPath.asString());
						}
					}
				}
			}
			if (pStatus->rc != CMPI_RC_OK)
			{
				COMMON_LOG_ERROR_F("CMPI error converting a CMPI ref array to a STR_LIST. Error %d", pStatus->rc);
			}
			// create even if there's an error (it will just be empty)
			pResult = new wbem::framework::Attribute(list, isKey);
			break;
		}
		case CMPI_dateTime     :
		{
				CMPIString *cmpiDateTime = CMGetStringFormat(pCmpiAttribute->value.dateTime, pStatus);
				const char *dateTime = CMGetCharsPtr(cmpiDateTime, pStatus);
				pResult = new wbem::framework::Attribute(dateTime,
						wbem::framework::DATETIME_SUBTYPE_DATETIME, isKey);
				break;
		}
		// current unsupported types
		case CMPI_real32       :
		case CMPI_real64       :
		case CMPI_args         :
		case CMPI_class        :
		case CMPI_filter       :
		case CMPI_enumeration  :
		case CMPI_chars        :
		case CMPI_ptr          :
		case CMPI_charsptr     :
		case CMPI_char16A      :
		case CMPI_real32A      :
		case CMPI_real64A      :
		case CMPI_uint64A      :
		case CMPI_sint8A       :
		case CMPI_sint16A      :
		case CMPI_sint32A      :
		case CMPI_sint64A      :
		case CMPI_charsA       :
		case CMPI_dateTimeA    :
		case CMPI_instanceA    :
		case CMPI_charsptrA    :
		default:
			CMSetStatus(pStatus, CMPI_RC_ERR_INVALID_DATA_TYPE);
			break;
		}
	}

	return pResult;
}



// convert object path
CMPIObjectPath *intelToCmpi(const CMPIBroker *pBroker, wbem::framework::ObjectPath *pObjectPath, CMPIStatus *pStatus)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	CMPIObjectPath *cmpiObjectPath = NULL;
	if (pStatus == NULL)
	{
		COMMON_LOG_ERROR("Parameter pStatus cannot be NULL");
	}
	else if (pBroker == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pBroker cannot be NULL");

	}
	else if (pObjectPath == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pObjectPath cannot be NULL");

	}
	else
	{
		cmpiObjectPath = CMNewObjectPath (pBroker,
				pObjectPath->getNamespace().c_str(), pObjectPath->getClass().c_str(), pStatus);

		if (pStatus->rc == CMPI_RC_OK)
		{
			wbem::framework::attributes_t keys = pObjectPath->getKeys();
			wbem::framework::attributes_t::iterator iKey = keys.begin();
			for(; iKey != keys.end() && pStatus->rc == CMPI_RC_OK; iKey++)
			{
				wbem::framework::Attribute keyAttribute = iKey->second;
				CMPIData cmpiAttribute;
				CMPIStatus tempStatus;
				intelToCmpi(pBroker, &keyAttribute, &cmpiAttribute, &tempStatus);
				KEEP_ERR(*pStatus, tempStatus);
				if (tempStatus.rc == CMPI_RC_OK)
				{
					CMAddKey(cmpiObjectPath, iKey->first.c_str(), &(cmpiAttribute.value), cmpiAttribute.type);
				}
			}
		}
		else
		{
			COMMON_LOG_ERROR("CMPI failed to create an Object Path");
		}
	}

	return cmpiObjectPath;
}

// convert object path
void cmpiToIntel(const CMPIObjectPath *pCmpiObjectPath, wbem::framework::ObjectPath *pObjectPath, CMPIStatus *pStatus)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	if (pStatus == NULL)
	{
		COMMON_LOG_ERROR("Parameter pStatus cannot be NULL");
	}
	else if (pCmpiObjectPath == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pCmpiObjectPath cannot be NULL");

	}
	else if (pObjectPath == NULL)
	{
		pStatus->rc = CMPI_RC_ERR_FAILED;
		COMMON_LOG_ERROR("Parameter pObjectPath cannot be NULL");

	}
	else
	{
		wbem::framework::attributes_t keys;


		CMPICount keyCount = CMGetKeyCount(pCmpiObjectPath, pStatus);
		CMPIStatus tempStatus;
		for(CMPICount i = 0; i < keyCount&& pStatus->rc == CMPI_RC_OK; i++)
		{
			CMPIString *cmpiKeyName;
			CMGetKeyAt(pCmpiObjectPath, i, &cmpiKeyName, &tempStatus);
			KEEP_ERR(*pStatus, tempStatus);
			if (pStatus->rc == CMPI_RC_OK)
			{
				std::string keyName(cmpiKeyName->ft->getCharPtr(cmpiKeyName, &tempStatus));
				KEEP_ERR(*pStatus, tempStatus);
				if (pStatus->rc == CMPI_RC_OK)
				{
					wbem::framework::Attribute *pAttribute;
					CMPIData cmpiAttribute = pCmpiObjectPath->ft->getKey(pCmpiObjectPath, keyName.c_str(), &tempStatus);
					KEEP_ERR(*pStatus, tempStatus);
					if (pStatus->rc == CMPI_RC_OK)
					{
						pAttribute = cmpiToIntel(&cmpiAttribute, true, &tempStatus);
						KEEP_ERR(*pStatus, tempStatus);
						if (pAttribute != NULL)
						{
							keys.insert(std::pair<std::string, wbem::framework::Attribute>(keyName, *pAttribute));
							delete pAttribute;
						}
					}
				}
			}
		}

		// Do this even if there was an error with a key
		CMPIString *pCmpiNs = pCmpiObjectPath->ft->getNameSpace(pCmpiObjectPath, &tempStatus);
		KEEP_ERR(*pStatus, tempStatus);
		CMPIString *pCmpiHost = pCmpiObjectPath->ft->getHostname(pCmpiObjectPath, &tempStatus);
		KEEP_ERR(*pStatus, tempStatus);
		CMPIString *pCmpiClass = pCmpiObjectPath->ft->getClassName(pCmpiObjectPath, &tempStatus);
		KEEP_ERR(*pStatus, tempStatus);

		if (pCmpiNs != NULL && pCmpiHost != NULL && pCmpiClass != NULL)
		{
			const char *pNamespace = CMGetCharsPtr(pCmpiNs, &tempStatus);
			KEEP_ERR(*pStatus, tempStatus);

			const char *pHost = CMGetCharsPtr(pCmpiHost, &tempStatus);
			KEEP_ERR(*pStatus, tempStatus);

			// host isn't required, so if it's not provided (is null) make it an empty string
			if (pHost == NULL)
			{
				pHost = "";
			}

			const char *pClass = CMGetCharsPtr(pCmpiClass, &tempStatus);
			KEEP_ERR(*pStatus, tempStatus);

			if (pNamespace != NULL && pClass != NULL)
			{
				pObjectPath->setObjectPath(
					pHost,
					pNamespace,
					pClass,
					keys);
			}
			else
			{
				COMMON_LOG_ERROR("Namespace, Host, or Class was null.  Unable to set ObjectPath");
			}
		}
		else
		{
			COMMON_LOG_ERROR("Could not get valid values from CMPI Object Path");
		}
		if (pStatus->rc != CMPI_RC_OK)
		{
			COMMON_LOG_ERROR_F("CMPI error: %d", pStatus->rc);
		}
	}
}
}
}
