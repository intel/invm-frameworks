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
#ifndef INTEL_CIM_FRAMEWORK_ASSOCIATIONMAPPER_H
#define INTEL_CIM_FRAMEWORK_ASSOCIATIONMAPPER_H

#include "ObjectPath.h"
#include "InstanceFactory.h"
#include "Instance.h"
#include "InstanceFactoryCreator.h"

namespace wbem
{
namespace framework
{


/*!
 * Type of association defined in the Association table
 */
enum associationType
{
	ASSOCIATIONTYPE_BASIC,   //!< ASSOCIATIONTYPE_BASIC
	ASSOCIATIONTYPE_SIMPLEFK, //!< ASSOCIATIONTYPE_SIMPLEFK
	ASSOCIATIONTYPE_COMPLEX  //!< ASSOCIATIONTYPE_COMPLEX
};

/*!
 * Represents an Association Class
 */
struct associationClass
{
	std::string className; //!< Name of class
	std::string antecedentPropertyName; //!< Name of the antecedent property
	std::string dependentPropertyName; //!< Name of the dependent property
};

/*!
 * Data structure to contain the mapping data to dynamically build associations and association
 * class instances
 */
struct associationMap
{
	std::string associationClassName; //!< name of the association class
	enum associationType type; //!< information on how to determine if instances are associated
	std::string antecedentClassName; //!< defines the reference to the antecedent
	std::string dependentClassName;  //!< defines the reference to the dependent
	std::string antecedentFk; //!< Type SimpleFk requires antecedentFK
	std::string dependentFk; //!< Type SimpleFk requires dependentFK
};

class AssociationMapper
{
public:

	AssociationMapper(
		InstanceFactoryCreator *pProviderFactory,
		std::map<std::string, struct associationClass> classMap,
		std::vector<struct associationMap> associationTable,
		std::string cimNamespace,
		Instance *pInstance = NULL,
		const std::string &associationClassName = "",
		const std::string &resultClassName = "",
		const std::string &roleName = "",
		const std::string &resultRoleName = "");

	/*!
	 * Copy Constructor
	 */
	AssociationMapper(const AssociationMapper &factory);

	/*!
	 * Clean up the NvmAssociationFactory
	 */
	virtual ~AssociationMapper();

	/*!
	 * Assignement operator
	 */
	AssociationMapper &operator=(const AssociationMapper &other);

	virtual framework::instance_names_t *getAssociationNames();


	bool isAssociationClass(const std::string &className);

	static bool simpleFkMatch(
		Instance *pAntInstance, const std::string &antFk,
		Instance *pDepInstance, const std::string &depFk);

	static bool filteredFkMatch(
		Instance *pAntInstance, const std::string &antFk,
		const std::vector<std::string> &antFkFilter,
		Instance *pDepInstance, const std::string &depFk,
		const std::vector<std::string> &depFkFilter);

	static bool filteredFkMatch(
		Instance *pAntInstance, const std::string &antFk, const std::string &antFkFilter,
		Instance *pDepInstance, const std::string &depFk, const std::string &depFkFilter);


private:
	virtual void populateAttributeList(
		framework::attribute_names_t &attributes) throw(framework::Exception);

	InstanceFactoryCreator *m_pProviderFactory;
	std::map<std::string, struct associationClass> m_classMap;
	std::vector<struct associationMap> m_associationTable;
	std::string m_cimNamespace;
	std::string m_associationClassName;
	std::string m_resultClassName;
	std::string m_roleName;
	std::string m_resultRoleName;
	Instance *m_pInstance;

	bool resultClassEmptyOrMatches(const struct associationMap &association,
		bool instanceIsAnt,
		bool instanceIsDep);

	void addAssociationObjectPaths(instance_names_t &objectPaths,
		const struct associationMap &association)
		throw(Exception);

	/*
	 * Add any valid association object paths using m_pInstance as the dependent.
	 */
	void addAssociationObjectPathsWithInstanceAsAntecedent(instance_names_t &objectPaths,
		const struct associationMap &association,
		InstanceFactory &antFactory, InstanceFactory &depFactory);

	/*
	 * Add any valid association object paths using m_pInstance as the antecedent.
	 */
	void addAssociationObjectPathsWithInstanceAsDependent(instance_names_t &objectPaths,
		const struct associationMap &association,
		InstanceFactory &antFactory, InstanceFactory &depFactory);

	/*
	 * Add any valid association object paths between all antecedent and dependent
	 * class instances.
	 */
	void addAssociationObjectPathsForAllInstances(instance_names_t &objectPaths,
		const struct associationMap &association,
		InstanceFactory &antFactory, InstanceFactory &depFactory);

	/*
	 * Grabs all instances from an instance factory.
	 */
	instances_t *getInstanceListFromFactory(InstanceFactory &factory) throw(Exception);

	/*
	 * Grabs a non-pointer copy of an instance list with just the member instance.
	 */
	instances_t getInstanceListWithMemberInstance();

	/*
	 * Adds any valid association object paths found matching the given association between antecedent
	 * and dependent instances.
	 */
	void addValidObjectPathsForAssociationBetweenAntecedentAndDependentInstances(
		instance_names_t &objectPaths,
		const struct associationMap &association,
		instances_t &antInstances, InstanceFactory &antFactory,
		instances_t &depInstances, InstanceFactory &depFactory);

	/*
	 * Returns true if the antecedent and dependent instances have a given association.
	 */
	bool instancesHaveAssociation(const struct associationMap &association,
		Instance &antInstance, InstanceFactory &antFactory,
		Instance &depInstance, InstanceFactory &depFactory);
};

}
}


#endif //INTEL_CIM_FRAMEWORK_ASSOCIATIONMAPPER_H
