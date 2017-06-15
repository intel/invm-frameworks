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

#include <logger/logging.h>
#include <CimomAdapter.h>
#include "AssociationMapper.h"
#include "StringUtil.h"

namespace wbem
{
namespace framework
{

AssociationMapper::AssociationMapper(
	InstanceFactoryCreator *pProviderFactory,
	std::map<std::string, struct associationClass> classMap,
	std::vector<struct associationMap> associationTable,
	std::string cimNamespace,
	Instance *pInstance,
	const std::string &associationClassName,
	const std::string &resultClassName,
	const std::string &roleName,
	const std::string &resultRoleName) :
	m_pProviderFactory(pProviderFactory),
	m_classMap(classMap),
	m_associationTable(associationTable),
	m_cimNamespace(cimNamespace),
	m_associationClassName(associationClassName),
	m_resultClassName(resultClassName),
	m_roleName(roleName),
	m_resultRoleName(resultRoleName),
	m_pInstance(NULL)

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


/*!
 * Copy Constructor
 */
AssociationMapper::AssociationMapper(const AssociationMapper &other)
	:
	m_pProviderFactory(other.m_pProviderFactory),
	m_cimNamespace(other.m_cimNamespace),
	m_associationClassName(other.m_associationClassName),
	m_resultClassName(other.m_resultClassName),
	m_roleName(other.m_roleName),
	m_resultRoleName(other.m_resultRoleName),
	m_pInstance(NULL)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (other.m_pInstance != NULL)
	{
		m_pInstance = new Instance(*other.m_pInstance);
	}
}

AssociationMapper::~AssociationMapper()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (m_pInstance)
	{
		delete m_pInstance;
	}
}

AssociationMapper &AssociationMapper::operator=(
	const AssociationMapper &other)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	if (this != &other)
	{
		m_associationClassName = other.m_associationClassName;
		m_resultClassName = other.m_resultClassName;
		m_roleName = other.m_roleName;
		m_resultRoleName = other.m_resultRoleName;
		m_cimNamespace = other.m_cimNamespace;
		m_pProviderFactory = other.m_pProviderFactory;
		if (other.m_pInstance != NULL)
		{
			if (m_pInstance != NULL)
			{
				*m_pInstance = *(other.m_pInstance);
			}
			else
			{
				m_pInstance = new Instance(*other.m_pInstance);
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
 * Loop through the AssociationTable and use appropriate entries to build ObjectPaths
 */
wbem::framework::instance_names_t *AssociationMapper::getAssociationNames()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	COMMON_LOG_DEBUG_F("Association Class: %s, Instance Class: %s",
		m_associationClassName.c_str(),
		(m_pInstance == NULL ? "NULL" : m_pInstance->getClass().c_str()));
	instance_names_t *result = new instance_names_t();

	classIsA("", "");

	for (size_t i = 0; i < m_associationTable.size(); i++)
	{
		try
		{
			addAssociationObjectPaths(*result, m_associationTable[i]);
		}
		catch (Exception &)
		{
			delete result;
			throw;
		}
	}

	COMMON_LOG_DEBUG_F("Returning %d Instance Names", (int) result->size());
	return result;
}

void AssociationMapper::addAssociationObjectPaths(
	instance_names_t &objectPaths, const struct associationMap &association)
throw(Exception)
{
	if (!isAssociationClass(association.associationClassName))
	{
		COMMON_LOG_ERROR_F("Association Class %s not configured in "
			"the Association Class Array",
			association.associationClassName.c_str());
	}
	else
	{
		const associationClass &associationClass = m_classMap[association.associationClassName];

		// initial filtering logic to determine if entry in association table applies to request
		bool instanceIsNull = m_pInstance == NULL;
		bool instanceIsAnt = m_pInstance != NULL && m_pInstance->getClass() ==
													association.antecedentClassName;
		bool instanceIsDep = m_pInstance != NULL && m_pInstance->getClass() ==
													association.dependentClassName;
		bool instanceIsAntOrNull = instanceIsNull || instanceIsAnt;
		bool instanceIsDepOrNull = instanceIsNull || instanceIsDep;

		bool instanceIsEmptyOrAntOrDep = m_pInstance == NULL || instanceIsAnt || instanceIsDep;

		bool assocClassFilter = m_associationClassName.empty() ||
								m_associationClassName == association.associationClassName;

		bool resultClassFilter = resultClassEmptyOrMatches(association, instanceIsAntOrNull,
			instanceIsDepOrNull);

		bool roleFilter = (m_roleName.empty() ||
						   (instanceIsAntOrNull &&
							m_roleName == associationClass.antecedentPropertyName) ||
						   (instanceIsDepOrNull &&
							m_roleName == associationClass.dependentPropertyName));

		bool resultRoleFilter = (m_resultRoleName.empty() ||
								 (instanceIsDepOrNull &&
								  m_resultRoleName == associationClass.antecedentPropertyName) ||
								 (instanceIsAntOrNull &&
								  m_resultRoleName == associationClass.dependentPropertyName));

		if (instanceIsEmptyOrAntOrDep &&
			assocClassFilter && resultClassFilter && roleFilter && resultRoleFilter)
		{
			// add all if no instance path was provided. Otherwise check if the object path matches
			// the antecedent or dependent of the association

			// get the Instance Factory for both Antecedent and Dependent
			InstanceFactory *pDepFactory =
				m_pProviderFactory->getInstanceFactory(association.dependentClassName);
			InstanceFactory *pAntFactory =
				m_pProviderFactory->getInstanceFactory(association.antecedentClassName);
			if (pAntFactory && pDepFactory)
			{
				// Instance class could be antecedent and/or dependent so need to check both.
				if (instanceIsAnt)
				{
					addAssociationObjectPathsWithInstanceAsAntecedent(objectPaths, association,
						*pAntFactory, *pDepFactory);
				}
				if (instanceIsDep)
				{
					addAssociationObjectPathsWithInstanceAsDependent(objectPaths, association,
						*pAntFactory, *pDepFactory);
				}
				// no instance passed in
				if (instanceIsNull)
				{
					addAssociationObjectPathsForAllInstances(objectPaths, association,
						*pAntFactory, *pDepFactory);
				}

				delete pAntFactory;
				delete pDepFactory;
			}
			else
			{
				// Clean up before throwing
				if (pAntFactory)
				{
					delete pAntFactory;
				}
				if (pDepFactory)
				{
					delete pDepFactory;
				}
				COMMON_LOG_ERROR("Unknown error. pAntFactory, or pDepFactory was NULL");
				throw Exception("Antecedent or dependent class factory missing");
			}
		}
	}
}

void AssociationMapper::addAssociationObjectPathsWithInstanceAsAntecedent(
	instance_names_t &objectPaths, const struct associationMap &association,
	InstanceFactory &antFactory, InstanceFactory &depFactory)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);

	// Instance is the Antecedent
	instances_t antecedentInstances = getInstanceListWithMemberInstance();

	// get all possible instances of the Dependent class
	instances_t *pDependentInstances = getInstanceListFromFactory(depFactory);

	// Build up the result list
	addValidObjectPathsForAssociationBetweenAntecedentAndDependentInstances(objectPaths,
		association,
		antecedentInstances, antFactory,
		*pDependentInstances, depFactory);

	delete pDependentInstances;
}

void AssociationMapper::addAssociationObjectPathsWithInstanceAsDependent(
	instance_names_t &objectPaths, const struct associationMap &association,
	InstanceFactory &antFactory, InstanceFactory &depFactory)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	// get all possible instances of the Antecedent class
	instances_t *pAntecedentInstances = getInstanceListFromFactory(antFactory);

	// Instance is the Dependent
	instances_t dependentInstances = getInstanceListWithMemberInstance();

	// Build up the result list
	addValidObjectPathsForAssociationBetweenAntecedentAndDependentInstances(objectPaths,
		association,
		*pAntecedentInstances, antFactory,
		dependentInstances, depFactory);

	delete pAntecedentInstances;
}

void AssociationMapper::addAssociationObjectPathsForAllInstances(
	instance_names_t &objectPaths, const struct associationMap &association,
	InstanceFactory &antFactory, InstanceFactory &depFactory)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	// get all possible instances of the Antecedent class
	instances_t *pAntecedentInstances = getInstanceListFromFactory(antFactory);

	// get all possible instances of the Dependent class
	instances_t *pDependentInstances = NULL;
	try // if something goes haywire, need to clean up the antecedent instances
	{
		pDependentInstances = getInstanceListFromFactory(depFactory);
	}
	catch (Exception &)
	{
		delete pAntecedentInstances;
		throw;
	}

	// Build up the result list
	addValidObjectPathsForAssociationBetweenAntecedentAndDependentInstances(objectPaths,
		association,
		*pAntecedentInstances, antFactory,
		*pDependentInstances, depFactory);

	delete pAntecedentInstances;
	delete pDependentInstances;
}

wbem::framework::instances_t *AssociationMapper::getInstanceListFromFactory(
	InstanceFactory &factory) throw(wbem::framework::Exception)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	attribute_names_t attributes;
	instances_t *pInstances = factory.getInstances(attributes);
	if (!pInstances)
	{
		COMMON_LOG_ERROR("Unknown error. pInstances was NULL");
		throw Exception("Factory returned NULL for getInstances");
	}

	return pInstances;
}

wbem::framework::instances_t AssociationMapper::getInstanceListWithMemberInstance()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	instances_t instances;
	instances.push_back(*m_pInstance);

	return instances;
}

void AssociationMapper::addValidObjectPathsForAssociationBetweenAntecedentAndDependentInstances(
	instance_names_t &objectPaths, const struct associationMap &association,
	instances_t &antInstances, InstanceFactory &antFactory, instances_t &depInstances,
	InstanceFactory &depFactory)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	// Search for valid associations between all antecedent and dependent instances
	for (instances_t::iterator aIter = antInstances.begin();
		 aIter != antInstances.end(); aIter++)
	{
		for (instances_t::iterator dIter = depInstances.begin();
			 dIter != depInstances.end(); dIter++)
		{
			if (instancesHaveAssociation(association,
				*aIter, antFactory, *dIter, depFactory))
			{
				const struct associationClass &associationClass =
					m_classMap[association.associationClassName];

				attributes_t keys;
				framework::Attribute antecdentAttribute
					= Attribute(aIter->getObjectPath().asString(true), true);
				antecdentAttribute.setIsAssociationClassInstance(true);
				keys[associationClass.antecedentPropertyName]
					= antecdentAttribute;
				framework::Attribute dependentAttribute
					= Attribute(dIter->getObjectPath().asString(true), true);
				dependentAttribute.setIsAssociationClassInstance(true);
				keys[associationClass.dependentPropertyName]
					= dependentAttribute;
				ObjectPath path(".", m_cimNamespace, association.associationClassName, keys);
				objectPaths.push_back(path);
			}
		}
	}
}

bool AssociationMapper::instancesHaveAssociation(
	const struct associationMap &association,
	Instance &antInstance, InstanceFactory &antFactory,
	Instance &depInstance, InstanceFactory &depFactory)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	bool instancesAreAssociated = false;

	// determine if instances are associated based on the Association Type
	switch (association.type)
	{
		case ASSOCIATIONTYPE_BASIC:
			instancesAreAssociated = true;
			break;
		case ASSOCIATIONTYPE_COMPLEX:
			instancesAreAssociated =
				antFactory.isAssociated(association.associationClassName, &antInstance,
					&depInstance) &&
				depFactory.isAssociated(association.associationClassName, &antInstance,
					&depInstance);
			break;
		case ASSOCIATIONTYPE_SIMPLEFK:
			instancesAreAssociated = simpleFkMatch(
				&antInstance, association.antecedentFk,
				&depInstance, association.dependentFk);
			break;
	}

	return instancesAreAssociated;
}

/*
 * There's a bit more logic in determining the resultClass filter so it has its own function.
 * Logic:
 * 		If the resultCLassName is empty than no need to filter on it ... return true
 * 		If the instance is (or could be) an antecedent, compare the resultClass with the
 * 			association dependent class name (result is wanted)
 * 			If that doesn't match then check to see if the dependent class name "is a" [result class]
 * 		If the instance is (or could be) a dependent, do the same switching dependent and antecedent
 *
 */
bool AssociationMapper::resultClassEmptyOrMatches(
	const struct associationMap &association, bool instanceIsAnt, bool instanceIsDep)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	bool result = false;
	if (this->m_resultClassName.empty())
	{
		result = true;
	}
	else
	{
		// It's assumed that instanceIsAnt or instanceIsDep is true, or both
		if (instanceIsAnt)
		{
			result = this->m_resultClassName == association.dependentClassName;
			if (!result)
			{
				result = classIsA(association.dependentClassName, m_resultClassName);
			}
		}
		if (!result && instanceIsDep) // if the antecedent matched no need to look at dependent
		{
			result = this->m_resultClassName == association.antecedentClassName;
			if (!result)
			{
				result = classIsA(association.antecedentClassName, m_resultClassName);
			}
		}
	}

	return result;
}

/*
 * Use the Association Class name to find the appropriate attributes to use.
 */
void AssociationMapper::populateAttributeList(
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
bool AssociationMapper::isAssociationClass(const std::string &className)
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
bool AssociationMapper::simpleFkMatch(
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
bool AssociationMapper::filteredFkMatch(
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
bool AssociationMapper::filteredFkMatch(
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

}
}

