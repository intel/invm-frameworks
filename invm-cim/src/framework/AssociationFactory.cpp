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
 * This file contains the implementation of the NvmAssociationFactory which
 * contains functionality to provide generically for Association Classes.
 */

#include "CimomAdapter.h"
#include <logger/logging.h>
#include "AssociationFactory.h"
#include "ObjectPath.h"
#include "ProviderFactory.h"
#include "StringUtil.h"

wbem::framework::AssociationFactory::AssociationFactory(
	Instance *pInstance,
	const std::string &associationClassName,
	const std::string &resultClassName,
	const std::string &roleName,
	const std::string &resultRoleName)
	: InstanceFactory(),
	m_associationClassName(associationClassName),
	m_resultClassName(resultClassName),
	m_roleName(roleName),
	m_resultRoleName(resultRoleName),
	m_pInstance(NULL),
	m_associationTable(),
	m_classMap()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (pInstance != NULL)
	{
		m_pInstance = new Instance(*pInstance);
	}
	COMMON_LOG_DEBUG_F("associationClassName: %s", associationClassName.c_str());
	COMMON_LOG_DEBUG_F("pInstance: %s",
		pInstance == NULL ? "Is NULL" : pInstance->getClass().c_str());
}

void wbem::framework::AssociationFactory::initClassMap()
{
	// Override in subclass - by default, no association classes
}

void wbem::framework::AssociationFactory::initAssociationTable()
{
	// Override in subclass - by default, no associations
}

/*!
 * Copy Constructor
 */
wbem::framework::AssociationFactory::AssociationFactory(const AssociationFactory &factory)
	: InstanceFactory(factory),
	m_associationClassName(factory.m_associationClassName),
	m_resultClassName(factory.m_resultClassName),
	m_roleName(factory.m_roleName),
	m_resultRoleName(factory.m_resultRoleName),
	m_pInstance(NULL)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (factory.m_pInstance != NULL)
	{
		m_pInstance = new Instance(*factory.m_pInstance);
	}
}

wbem::framework::AssociationFactory::~AssociationFactory()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (m_pInstance)
	{
		delete m_pInstance;
	}
}

wbem::framework::AssociationFactory &wbem::framework::AssociationFactory::operator=(
	const wbem::framework::AssociationFactory &factory)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (this != &factory)
	{
		m_associationClassName = factory.m_associationClassName;
		m_resultClassName = factory.m_resultClassName;
		m_roleName = factory.m_roleName;
		m_resultRoleName = factory.m_resultRoleName;
		if (factory.m_pInstance != NULL)
		{
			if (m_pInstance != NULL)
			{
				*m_pInstance = *(factory.m_pInstance);
			}
			else
			{
				m_pInstance = new Instance(*factory.m_pInstance);
			}
		}
		else
		{
			if (m_pInstance)
			{
				delete m_pInstance;
			}
			m_pInstance = NULL;
		}
	}
	return *this;
}

/*
 * Return an instance given an object path
 */
wbem::framework::Instance *wbem::framework::AssociationFactory::getInstance(
	framework::ObjectPath &path,
	framework::attribute_names_t &attributes)
throw(framework::Exception)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	checkAttributes(attributes);
	wbem::framework::Instance *pInstance = new Instance(path);
	if (pInstance)
	{
		markInstanceAttributesAsAssociationRefs(*pInstance);
	}
	return pInstance;
}

/*
 * Loop through the AssociationTable and use appropriate entries to build ObjectPaths
 */
wbem::framework::instance_names_t *wbem::framework::AssociationFactory::getInstanceNames()
throw(framework::Exception)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	AssociationMapper mapper(ProviderFactory::getSingleton(),
		m_classMap,
		m_associationTable,
		m_cimNamespace, m_pInstance,
		m_associationClassName, m_resultClassName, m_roleName, m_resultRoleName);

	return mapper.getAssociationNames();
}


/*
 * Use the Association Class name to find the appropriate attributes to use.
 */
void wbem::framework::AssociationFactory::populateAttributeList(
	framework::attribute_names_t &attributes)
throw(framework::Exception)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (!m_associationClassName.empty())
	{
		if (isAssociationClass(m_associationClassName))
		{
			const struct associationClass &assocClass = m_classMap[m_associationClassName];

			attributes.push_back(assocClass.antecedentPropertyName);
			attributes.push_back(assocClass.dependentPropertyName);
		}
		else
		{
			COMMON_LOG_ERROR_F("Association type %s not supported by this provider",
				m_associationClassName.c_str());
		}
	}
}

/*
 * Helper function that indicates if a class name is an Association Class
 */
bool wbem::framework::AssociationFactory::isAssociationClass(const std::string &className)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	bool found = false;

	if (m_classMap.find(className) != m_classMap.end())
	{
		found = true;
	}

	return found;
}

/*
 * Determine if the instances match
 */
bool wbem::framework::AssociationFactory::simpleFkMatch(
	Instance *pAntInstance, const std::string &antFk,
	Instance *pDepInstance, const std::string &depFk)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	return filteredFkMatch(pAntInstance, antFk, std::vector<std::string>(),
		pDepInstance, depFk, std::vector<std::string>());
}

/*
 * Determine if the instances math
 */
bool wbem::framework::AssociationFactory::filteredFkMatch(
	Instance *pAntInstance, const std::string &antFk, const std::string &antFkFilter,
	Instance *pDepInstance, const std::string &depFk, const std::string &depFkFilter)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	std::vector<std::string> antFkFilterList;
	std::vector<std::string> depFkFilterList;
	antFkFilterList.push_back(antFkFilter);
	depFkFilterList.push_back(depFkFilter);
	return filteredFkMatch(pAntInstance, antFk, antFkFilterList,
		pDepInstance, depFk, depFkFilterList);
}


/*
 * Determine if the instances math
 */
bool wbem::framework::AssociationFactory::filteredFkMatch(
	Instance *pAntInstance, const std::string &antFk, const std::vector<std::string> &antFkFilter,
	Instance *pDepInstance, const std::string &depFk, const std::vector<std::string> &depFkFilter)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	bool result = false;
	framework::Attribute antAttribute;
	framework::Attribute depAttribute;
	if (pAntInstance->getAttribute(antFk, antAttribute) == framework::SUCCESS &&
		pDepInstance->getAttribute(depFk, depAttribute) == framework::SUCCESS)
	{
		std::string antString = StringUtil::removeStrings(antAttribute.asStr(), antFkFilter);
		std::string depString = StringUtil::removeStrings(depAttribute.asStr(), depFkFilter);
		result = (antString == depString);
	}
	return result;
}

void wbem::framework::AssociationFactory::addClassToMap(const std::string &className,
	const std::string &antecedentName, const std::string &dependentName)
{
	struct associationClass assocClass = {className, antecedentName, dependentName};
	m_classMap[className] = assocClass;
}

void wbem::framework::AssociationFactory::addAssociationToTable(const std::string &className,
	const enum associationType type, const std::string &antecedentClass,
	const std::string &dependentClass, const std::string &antecedentFk,
	const std::string &dependentFk)
{
	struct associationMap assoc = {className, type,
								   antecedentClass, dependentClass,
								   antecedentFk, dependentFk};
	m_associationTable.push_back(assoc);
}

void wbem::framework::AssociationFactory::markInstanceAttributesAsAssociationRefs(
		framework::Instance& instance)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	attributes_t updatedAttributes;

	// need a mutable copy
	for (attributes_t::const_iterator oldAttrIter = instance.attributesBegin();
			oldAttrIter != instance.attributesEnd(); oldAttrIter++)
	{
		updatedAttributes[oldAttrIter->first] = oldAttrIter->second;
	}

	for (attributes_t::iterator attrIter = updatedAttributes.begin();
			attrIter != updatedAttributes.end(); attrIter++)
	{
		attrIter->second.setIsAssociationClassInstance(true);
		instance.setAttribute(attrIter->first, attrIter->second);
	}
}
