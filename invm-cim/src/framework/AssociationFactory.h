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


#ifndef _WBEM_FRAMEWORK_NVMASSOCIATIONFACTORY_H
#define _WBEM_FRAMEWORK_NVMASSOCIATIONFACTORY_H
#include <string>
#include "Instance.h"
#include "InstanceFactory.h"
#include "AssociationMapper.h"

namespace wbem
{
namespace framework
{



/*!
 * A generic association provider that will use a mapping table to create instances of requested
 * Association Classes
 */
class AssociationFactory : public wbem::framework::InstanceFactory
{
public:
	/*!
	 * Constructor
	 * @param associationClassName
	 * 		The association class name this instance factory will provide for. If an empty string is passed
	 * 		then all association classes will be assumed.
	 * @param pInstance
	 * 		Pointer to an instance for which associations/references are desired.  Is an optional parameter. If
	 * 		pInstance is not provided then all association classes will be assumed.
	 */
	AssociationFactory(
			Instance *pInstance = NULL,
			const std::string &associationClassName = "",
			const std::string &resultClassName = "",
			const std::string &roleName = "",
			const std::string &resultRoleName = "");

	/*!
	 * Copy Constructor
	 */
	AssociationFactory(const AssociationFactory &factory);

	/*!
	 * Clean up the NvmAssociationFactory
	 */
	virtual ~AssociationFactory();

	/*!
	 * Assignement operator
	 */
	AssociationFactory& operator=(const AssociationFactory& factory);

	/*!
	 * Implementation of the standard CIM method to retrieve a specific instance
	 * @param[in] path
	 * 		The object path of the instance to retrieve.
	 * @param[in] attributes
	 * 		The attributes to retrieve.
	 * @throw NvmException if unable to retrieve the host information.
	 * @todo Should throw an exception if the object path doesn't match
	 * the results of getHostName.
	 * @return The host server instance.
	 */
	virtual framework::Instance* getInstance(framework::ObjectPath &path,
			framework::attribute_names_t &attributes) throw (framework::Exception);

	/*!
	 * Implementation of the standard CIM method to retrieve a list of
	 * host server object paths.
	 * @remarks There is only one host server so return list should only contain one item.
	 * @return The object path of the host server.
	 */
	virtual framework::instance_names_t* getInstanceNames() throw (framework::Exception);

	/*!
	 * Used in the getInstanceFactory. It simply looks at the association table
	 * and checks if the className is an Association Class
	 * @param className
	 * 		The class name to search
	 * 	@return
	 * 		true if className is an Association class.
	 */
	bool isAssociationClass(const std::string &className);

	/*!
	 * Determine if the two instances are associated based on the FKs
	 * @param pAntInstance
	 * 		Antecedent Instance
	 * @param antFk
	 * 		Antecedent Foreign Key
	 * @param pDepInstance
	 * 		Dependent Instance
	 * @param depFk
	 * 		Dependent Foreign Key
	 * @return
	 */
	static bool simpleFkMatch(
			Instance *pAntInstance, const std::string &antFk,
			Instance *pDepInstance, const std::string &depFk);

	/*!
	 * Determine if the two instances are associated based on the FKs.
	  * @param pAntInstance
	 * 		Antecedent Instance
	 * @param antFk
	 * 		Antecedent Foreign Key
	 * @param antFkFilter
	 * 		List of strings that should be removed the the FK value first
	 * @param pDepInstance
	 * 		Dependent Instance
	 * @param depFk
	 * 		Dependent Foreign Key
	 * @param depFkFilter
	 * 		List of strings that should be removed the the FK value first
	 * @return
	 */
	static bool filteredFkMatch(
			Instance *pAntInstance, const std::string &antFk, const std::vector<std::string> &antFkFilter,
			Instance *pDepInstance, const std::string &depFk, const std::vector<std::string> &depFkFilter);

	/*!
	 * Determine if the two instances are associated based on the FKs.
	  * @param pAntInstance
	 * 		Antecedent Instance
	 * @param antFk
	 * 		Antecedent Foreign Key
	 * @param antFkFilter
	 * 		String that should be removed the the FK value first
	 * @param pDepInstance
	 * 		Dependent Instance
	 * @param depFk
	 * 		Dependent Foreign Key
	 * @param depFkFilter
	 * 		String that should be removed the the FK value first
	 * @return
	 */
	static bool filteredFkMatch(
			Instance *pAntInstance, const std::string &antFk, const std::string &antFkFilter,
			Instance *pDepInstance, const std::string &depFk, const std::string &depFkFilter);


protected:
	virtual void populateAttributeList(framework::attribute_names_t &attributes) throw (framework::Exception);

	std::string m_associationClassName;
	std::string m_resultClassName;
	std::string m_roleName;
	std::string m_resultRoleName;
	Instance *m_pInstance;

	// List of all possible associations between different classes in the provider
	std::vector<struct associationMap> m_associationTable;

	// List of all possible association classes with the names of their properties
	std::map<std::string, struct associationClass> m_classMap;

	/*
	 * Override these methods in your subclass to implement specific associations.
	 */
	virtual void initClassMap();
	virtual void initAssociationTable();

	/*
	 * Helper methods to build the class map and association table
	 */
	void addClassToMap(const std::string &className, const std::string &antecedentName, const std::string &dependentName);
	void addAssociationToTable(const std::string &className, const enum associationType type,
			const std::string &antecedentClass, const std::string &dependentClass,
			const std::string &antecedentFk = "", const std::string &dependentFk = "");

	void markInstanceAttributesAsAssociationRefs(framework::Instance &instance);

};
}
}

#endif /* _WBEM_FRAMEWORK_NVMASSOCIATIONFACTORY_H */
