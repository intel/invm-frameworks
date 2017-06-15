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
 * This file contains the base class for Intel CIM provider classes.
 */


#include <logger/logging.h>

#include <algorithm>

#include "ExceptionBadAttribute.h"
#include "ExceptionNotSupported.h"
#include "InstanceFactory.h"

#include "ProviderFactory.h"
#include "ObjectPathBuilder.h"

wbem::framework::InstanceFactory::InstanceFactory()
{
	// Default - subclasses should override
	setCimNamespace(INTEL_ROOT_NAMESPACE);
}

wbem::framework::InstanceFactory::InstanceFactory(const std::string &cimNamespace)
{
	setCimNamespace(cimNamespace);
}

wbem::framework::InstanceFactory::InstanceFactory(const InstanceFactory& factory) :
		m_cimNamespace(factory.m_cimNamespace)
{
}

/*
 * Destructor body is required.
 */
wbem::framework::InstanceFactory::~InstanceFactory()
{
}

wbem::framework::ObjectPath* wbem::framework::InstanceFactory::createInstance(
	Instance* instance)
{
	throw wbem::framework::ExceptionNotSupported(__FILE__, (char*) __func__);
}

wbem::framework::Instance* wbem::framework::InstanceFactory::modifyInstance(
	ObjectPath &path, attributes_t &attributes)
{
	throw wbem::framework::ExceptionNotSupported(__FILE__, (char*) __func__);
}

wbem::framework::Instance* wbem::framework::InstanceFactory::deleteInstance(
	ObjectPath &path)
{
	throw wbem::framework::ExceptionNotSupported(__FILE__, (char*) __func__);
}

wbem::framework::Instance* wbem::framework::InstanceFactory::getInstance(
	ObjectPath &path, attribute_names_t &attributes)
{
	throw wbem::framework::ExceptionNotSupported(__FILE__, (char*) __func__);
}

wbem::framework::instance_names_t* wbem::framework::InstanceFactory::getInstanceNames()
{
	throw wbem::framework::ExceptionNotSupported(__FILE__, (char*) __func__);
}

wbem::framework::instances_t* wbem::framework::InstanceFactory::getInstances(
		attribute_names_t &attributes)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	// declarations visible to both try and catch
	instance_names_t *pPaths = NULL;
	instances_t *pInstList = NULL;

	try
	{
		checkAttributes(attributes);

		// get the instances names using intrinsic
		pPaths = getInstanceNames();

		if(pPaths != NULL)
		{
			// create the return list
			pInstList = new framework::instances_t();

			// loop through the names
			for (framework::instance_names_t::iterator iter = pPaths->begin();
					iter != pPaths->end(); iter++)
			{
				framework::Instance* pInst = NULL;
				try
				{
					// create an instance for the name, add it to the list
					pInst = getInstance(*iter, attributes);
					if (pInst != NULL)
					{
						pInstList->push_back(*pInst);
						delete pInst;
					}
				}
				// if a single instance fails, eat the exception and keep going
				catch (framework::Exception &e)
				{
					if (pInst != NULL)
					{
						delete pInst;
					}
					// if only one instance throw the exception
					if (pPaths->size() == 1)
					{
						throw;
					}
					// else eat the exception and keep going
					COMMON_LOG_WARN_F("Error adding instance: %s", e.what());
				}
			}
		}
		else
		{
			COMMON_LOG_ERROR("getInstanceNames() returned NULL");
		}
	}
	// on error, cleanup but don't handle
	catch (Exception &)
	{
		if (pPaths)
		{
			delete pPaths;
		}
		if (pInstList)
		{
			delete pInstList;
		}

		// Re-throw the original (dynamic-type) exception.  We do this so that
		// we prevent the "slicing" of data from the original exception, which
		// would occur when throwing the caught base (static-type) exception.
		// This is also more efficient because we carry the existing exception forward
		// instead of telling throw to create a new copy from the caught (static-type)
		// base exception.
		throw;
	}

	// clean up
	if (pPaths)
	{
		pPaths->clear();
		delete pPaths;
	}

	return pInstList;
}

wbem::framework::UINT32 wbem::framework::InstanceFactory::executeMethod(
	wbem::framework::UINT32 &wbem_return,
	const std::string method,
	wbem::framework::ObjectPath &object,
	wbem::framework::attributes_t &inParms,
	wbem::framework::attributes_t &outParms)
{
	/*
	 * The error returned below is overridden in all the concrete
	 * method implementations we support. The line below assures we let
	 * clients know other methods are not supported.
	 */
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return CIM_ERR_METHOD_NOT_AVAILABLE;
}

/*
 * Get all associated instances and return their instance paths. Not as efficient as it might be,
 * but in order to map associated classes together entire instances are needed anyway.
 */
wbem::framework::instance_names_t* wbem::framework::InstanceFactory::associatorNames(
	ObjectPath &objectPath,
	const std::string &associationClassName,
	const std::string &resultClassName,
	const std::string &roleName,
	const std::string &resultRoleName)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("Object Path ClassName: %s", objectPath.getClass().c_str());
	COMMON_LOG_DEBUG_F("associationClassName: %s", associationClassName.c_str());
	COMMON_LOG_DEBUG_F("resultClassName: %s", resultClassName.c_str());
	COMMON_LOG_DEBUG_F("roleName: %s", roleName.c_str());
	COMMON_LOG_DEBUG_F("resultRoleName: %s", resultRoleName.c_str());

	instance_names_t *pNames = new instance_names_t();

	instances_t *pInstances = associatorInstances(objectPath,
			associationClassName, resultClassName, roleName, resultRoleName);
	instances_t::iterator iInstance = pInstances->begin();

	for (; iInstance != pInstances->end(); iInstance++)
	{
		pNames->push_back(iInstance->getObjectPath());
	}

	return pNames;
}

/*
 * Loop through the associator table and anywhere the objectPaht's classname is an antecedent or
 * dependent create the instances on the other side of the association.  If the
 * association class name is passed then only add instances if the association matches.
 */
wbem::framework::instances_t* wbem::framework::InstanceFactory::associatorInstances(
		ObjectPath &objectPath,
		const std::string &associationClassName,
		const std::string &resultClassName,
		const std::string &roleName,
		const std::string &resultRoleName)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("Object Path ClassName: %s", objectPath.getClass().c_str());
	COMMON_LOG_DEBUG_F("associationClassName: %s", associationClassName.c_str());
	COMMON_LOG_DEBUG_F("resultClassName: %s", resultClassName.c_str());
	COMMON_LOG_DEBUG_F("roleName: %s", roleName.c_str());
	COMMON_LOG_DEBUG_F("resultRoleName: %s", resultRoleName.c_str());

	instances_t* pInstances = new instances_t();

	attribute_names_t attributes;
	Instance *pInstance = getInstance(objectPath, attributes);
	std::vector<InstanceFactory *> associationFactories = ProviderFactory::getAssociationFactoriesStatic(
			pInstance,
			associationClassName, resultClassName, roleName, resultRoleName);
	COMMON_LOG_DEBUG_F("Got %u association factories", associationFactories.size());
	while (!associationFactories.empty())
	{
		COMMON_LOG_DEBUG("Inspecting association factory...");
		InstanceFactory *pAssociationFactory = associationFactories.back();
		if (pAssociationFactory)
		{
			instance_names_t *pAssociationNames = pAssociationFactory->getInstanceNames();
			COMMON_LOG_DEBUG_F("Got %u association names", pAssociationNames->size());

			instance_names_t::iterator iter = pAssociationNames->begin();
			for(; iter != pAssociationNames->end(); iter++)
			{
				attributes_t keys = iter->getKeys();
				attributes_t::iterator iKey = keys.begin();
				for(; iKey != keys.end(); iKey++)
				{
					ObjectPathBuilder builder(iKey->second.asStr());
					ObjectPath associatedObjectPath;
					builder.Build(&associatedObjectPath);
					// don't list the instance as one of its own associators
					if (associatedObjectPath.asString(true) != objectPath.asString(true))
					{
						COMMON_LOG_DEBUG_F("Adding associator: %s",
								associatedObjectPath.asString(true).c_str());
						InstanceFactory *pAssociatedFactory =
								ProviderFactory::getInstanceFactoryStatic(associatedObjectPath.getClass());
						if (pAssociatedFactory != NULL)
						{
							attribute_names_t attributes;
							wbem::framework::Instance *pAssociatedInstance = pAssociatedFactory->getInstance(
									associatedObjectPath, attributes);
							pInstances->push_back(*pAssociatedInstance);

							delete pAssociatedInstance;
							delete pAssociatedFactory;
						}
					}
				}
			}

			delete pAssociationNames;
			delete pAssociationFactory;
		}

		associationFactories.pop_back();
	}

	if (pInstance)
	{
		delete (pInstance);
	}

	return pInstances;
}

/*
 * Reference names are the object paths of association classes with an antecedent
 * or dependent equal to the object path being requested of.
 */
wbem::framework::instance_names_t *wbem::framework::InstanceFactory::referenceNames(
		ObjectPath &objectPath,
		const std::string &associationClassName,
		const std::string &resultClassName,
		const std::string &roleName,
		const std::string &resultRoleName)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("Object Path ClassName: %s", objectPath.getClass().c_str());
	COMMON_LOG_DEBUG_F("associationClassName: %s", associationClassName.c_str());
	COMMON_LOG_DEBUG_F("resultClassName: %s", resultClassName.c_str());
	COMMON_LOG_DEBUG_F("roleName: %s", roleName.c_str());
	COMMON_LOG_DEBUG_F("resultRoleName: %s", resultRoleName.c_str());

	wbem::framework::instance_names_t *pInstanceNames = NULL;

	attribute_names_t attributes;
	std::vector<InstanceFactory *> associationFactories = ProviderFactory::getAssociationFactoriesStatic(
			getInstance(objectPath, attributes),
			associationClassName, resultClassName, roleName, resultRoleName);
	while (!associationFactories.empty())
	{
		InstanceFactory * pAssociationFactory = associationFactories.back();
		if (pAssociationFactory)
		{
			pInstanceNames = pAssociationFactory->getInstanceNames();

			delete pAssociationFactory;
		}

		associationFactories.pop_back();
	}

	return pInstanceNames;
}

/*
 * Reference instances are the instances of association classes with an antecedent
 * or dependent equal to the object path being requested of.
 */
wbem::framework::instances_t *wbem::framework::InstanceFactory::referenceInstances(
		ObjectPath &objectPath,
		const std::string &associationClassName,
		const std::string &resultClassName,
		const std::string &roleName,
		const std::string &resultRoleName)
{
	COMMON_LOG_DEBUG_F("Object Path ClassName: %s", objectPath.getClass().c_str());
	COMMON_LOG_DEBUG_F("associationClassName: %s", associationClassName.c_str());
	COMMON_LOG_DEBUG_F("resultClassName: %s", resultClassName.c_str());
	COMMON_LOG_DEBUG_F("roleName: %s", roleName.c_str());
	COMMON_LOG_DEBUG_F("resultRoleName: %s", resultRoleName.c_str());

	wbem::framework::instances_t *pInstances = NULL;

	attribute_names_t attributesOne;
	std::vector<InstanceFactory *> associationFactories = ProviderFactory::getAssociationFactoriesStatic(
			getInstance(objectPath, attributesOne),
			associationClassName, resultClassName, roleName, resultRoleName);
	while (!associationFactories.empty())
	{
		InstanceFactory * pAssociationFactory = associationFactories.back();
		if (pAssociationFactory)
		{
			attribute_names_t attributesTwo;
			pInstances = pAssociationFactory->getInstances(attributesTwo);

			delete pAssociationFactory;
		}

		associationFactories.pop_back();
	}
	return pInstances;
}

bool wbem::framework::InstanceFactory::containsAttribute(
	const std::string &key, const attribute_names_t &attributes)
{
	// Compare case-insensitive values
	std::string keyLower = key;
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), tolower);

	// look at each attribute
	for (attribute_names_t::const_iterator iter=attributes.begin(); iter!=attributes.end(); ++iter)
	{
		// Convert attribute to lowercase
		std::string attrLower = *iter;
		std::transform(attrLower.begin(), attrLower.end(), attrLower.begin(), tolower);
		if (keyLower == attrLower) // found the key
		{
			return true;
		}
	}
	return false;
}

/*
 * Verify the attributes list
 * @param attributes
 * 		The list of attributes passed by the called
 * @throw NvmExceptionBadParameter if an attribute is invalid
 */
void wbem::framework::InstanceFactory::checkAttributes(attribute_names_t &attributes)
{
	attribute_names_t supportedAttributes;
	populateAttributeList(supportedAttributes);
	if (attributes.empty())
	{
		attributes = supportedAttributes;
	}
	else
	{
		for (attribute_names_t::const_iterator iter = attributes.begin(); iter != attributes.end(); iter++)
		{
			if (!containsAttribute((*iter), supportedAttributes))
			{
				throw ExceptionBadAttribute((*iter).c_str());
			}
		}
	}
}

bool wbem::framework::InstanceFactory::isAssociated(const std::string &associationClass,
		Instance* pAntInstance, Instance* pDepInstance)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return true;
}

void wbem::framework::InstanceFactory::checkPath(const wbem::framework::ObjectPath &path)
{
	instance_names_t *pPaths = getInstanceNames();

	const attributes_t &keys = path.getKeys();
	for(attributes_const_itr_t pair = keys.begin();
		pair != keys.end(); pair++)
	{
		const std::string &keyName = pair->first;
		const Attribute &value = pair->second;

		bool found = false;
		for(size_t i = 0; i < pPaths->size() && !found; i++)
		{
			ObjectPath &realPath = (*pPaths)[i];

			if (realPath.getKeyValue(keyName) == value)
			{
				found = true;
			}
		}
		if (!found)
		{
			throw ExceptionBadAttribute(keyName.c_str());
		}
	}
}
