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

#ifndef	_WBEM_FRAMEWORK_INSTANCE_FACTORY_H_
#define	_WBEM_FRAMEWORK_INSTANCE_FACTORY_H_

#include "Exception.h"
#include "Instance.h"
#include "ObjectPath.h"

namespace wbem
{
namespace framework
{
/*
 * WBEM has two return values for operations/methods.  Per-method return
 * values are defined in MOFs and HTTP return values are defined in the
 * CIM Operations over HTTP spec. The HTTP return values are typically
 * used by CIMOMs, but may be used by provider when no suitable return
 * value is defined in the MOFs.
 */

/*!
 * Common CIM return code indicating the method succeeded.
 */
static const wbem::framework::UINT32 MOF_ERR_SUCCESS = 0;

/*!
 * HTTP error code indicating that the CIMOM was unable to invoke the method.
 * @remarks HTTP return value used by our providers. The names and
 * values match those used in CIM Operations over HTTP.
 */
static const wbem::framework::UINT32 CIM_ERR_FAILED = 1;

/*!
 * HTTP error code indicating one or more input parameters are invalid.
 * @remarks HTTP return value used by our providers. The names and
 * values match those used in CIM Operations over HTTP.
 */
static const wbem::framework::UINT32 CIM_ERR_INVALID_PARAMETER = 4;

/*!
 * HTTP error code indicating that method is not supported.
 * @remarks HTTP return value used by our providers. The names and
 * values match those used in CIM Operations over HTTP.
 */
static const wbem::framework::UINT32 CIM_ERR_NOTSUPPORTED = 7;

/*!
 * HTTP error code indicating that method is not available.
 * @remarks HTTP return value used by our providers. The names and
 * values match those used in CIM Operations over HTTP.
 */

static const wbem::framework::UINT32 CIM_ERR_METHOD_NOT_AVAILABLE = 16;

/*!
 * The base class for CIM instance factories.
 */
class InstanceFactory
{
	public:

		/*!
		 * Initialize a new factory.
		 */
		InstanceFactory();

		/*
		 * Initialize a new factory in a specific CIM namespace
		 */
		InstanceFactory(const std::string &cimNamespace);

		InstanceFactory(const InstanceFactory &factory);

		/*!
		 * Clean up the factory.
		 */
		virtual ~InstanceFactory() = 0;

		/*
		 * Set the CIM namespace that instances returned from the
		 * factory will belong to.
		 */
		void setCimNamespace(const std::string &name) { m_cimNamespace = name; }

		/*
		 * Fetch the CIM namespace that instances returned from the
		 * factory will belong to.
		 */
		std::string getCimNamespace() { return m_cimNamespace; }

		/*!
		 * Standard CIM method to create a new instance.
		 * @param[in,out] instance
		 * 		The instance to be created.
		 * @throw NvmException if not implemented.
		 * @return The object path to the new instance.
		 */
		virtual ObjectPath* createInstance(Instance* instance);

		/*!
		 * Standard CIM method to modify an existing instance.
		 * @param[in] path
		 * 		The object path of the instance to modify.
		 * @param[in] attributes
		 * 		The attributes to modify.
		 * @throw NvmException if not implemented.
		 * @return The updated instance.
		 */
		virtual Instance* modifyInstance(ObjectPath &path, attributes_t &attributes);

		/*!
		 * Standard CIM method to delete an existing instance.
		 * @param[in] path
		 * 		The object path of the instance to delete.
		 * @throw NvmException if not implemented.
		 * @return The instance that was deleted.
		 */
		virtual Instance* deleteInstance(ObjectPath &path);

		/*!
		 * Standard CIM method to retrieve a single instance.
		 * @param[in] path
		 * 		The object path of the instance to retrieve.
		 * @param[in] attributes
		 * 		The attributes to retrieve.
		 * @throw NvmException if not implemented.
		 * @return The requested instance.
		 */
		virtual Instance* getInstance(ObjectPath &path, attribute_names_t &attributes);

		/*!
		 * Standard CIM method to retrieve a list of object paths for the instances in this factory.
		 * @throw NvmException if not implemented.
		 * @return The list of object paths.
		 */
		virtual instance_names_t* getInstanceNames();

		/*!
		 * Generic method interface to execute/invoke an extrinsic WBEM method.
		 * @param wbem_return
		 * @param[in] method
		 * 		Extrinsic method return (as defined in MOF).
		 * @param[in] object
		 * 		The object path to the instance supporting the method.
		 * @param[in] inParms
		 * 		STL map of in parameters.
		 * @param[in] outParms
		 * 		STL map of out parameters.
		 * @remarks Caller must delete all inParms and outParms members and maps.
		 * @remarks See concrete subclass headers for specifics.
		 * @return HTTP return code (see CIM Operations over HTTP)
		 */
		virtual wbem::framework::UINT32 executeMethod(
			wbem::framework::UINT32 &wbem_return,
			const std::string method,
			wbem::framework::ObjectPath &object,
			wbem::framework::attributes_t &inParms,
			wbem::framework::attributes_t &outParms);

		/*!
		 *	Standard CIM method to retrieve a list of the names of instances
		 *	that are associated with the specified instance.
		 */
		virtual instance_names_t* associatorNames(ObjectPath &objectPath,
				const std::string &associationClassName = "",
				const std::string &resultClassName = "",
				const std::string &roleName = "",
				const std::string &resultRoleName = "");

		/*!
		 * Standard CIM method to retrieve a list of instances that are associated
		 * with the specified instance.
		 */
		virtual instances_t* associatorInstances(ObjectPath &objectPath,
				const std::string &associationClassName = "",
				const std::string &resultClassName = "",
				const std::string &roleName = "",
				const std::string &resultRoleName = "");

		/*!
		 * Standard CIM method to retrieve a list of the names of association objects
		 * that refer to the specified instance.
		 */
		virtual wbem::framework::instance_names_t *referenceNames(
				ObjectPath &objectPath,
				const std::string &associationClassName = "",
				const std::string &resultClassName = "",
				const std::string &roleName = "",
				const std::string &resultRoleName = "");

		/*!
		 * Standard CIM method to retrieve a list of the associated objects that
		 * refer to the specified instance.
		 */
		virtual wbem::framework::instances_t *referenceInstances(
				ObjectPath &objectPath,
				const std::string &associationClassName = "",
				const std::string &resultClassName = "",
				const std::string &roleName = "",
				const std::string &resultRoleName = "");

		// default implementation exists but requires that getInstance and getInstanceNames
		// and populateAttributeList are implemented
		/*!
		 * Standard CIM method to retrieve a list of instances in this factory.
		 * @param[in] attributes
		 * 		The list of attribute names to retrieve for each instance.
		 * @remarks Default implementation that uses getInstanceNames, getInstance and
		 * populateAttributeList
		 * @return
		 * 		The list of instances.
		 */
		virtual instances_t* getInstances(attribute_names_t &attributes);


		// convenience method
		/*!
		 * Helper method to determine if the the attribute name is in the specified list of attribute names.
		 * @param[in] key
		 * 		The attribute name to look for.
		 * @param[in] attributes
		 * 		The list of attribute names to search
		 * @return
		 * 		true if the attribute name was found in the list.
		 * 		false if the attribute name was not found in the list.
		 */
		static bool containsAttribute(const std::string &key, const attribute_names_t &attributes);

		/*!
		 * Determines if the two instances should be associated by the Association Class. Usually only
		 * used if the association is more complex than simple FK relationships
		 * @param associationClass
		 * @param pAntInstance
		 * @param pDepInstance
		 * @return
		 * 		Returns true, but can be overridden by concrete classes to apply filtering
		 */
		virtual bool isAssociated(const std::string &associationClass,
				Instance *pAntInstance, Instance *pDepInstance);

	protected:
		std::string m_cimNamespace; // The CIM namespace to use for instances in this factory

		/*!
		 * Create a default list of attributes names to retrieve.
		 * @param[in,out] attributes
		 * 		The list of attribute names to populate.
		 * @remarks Implementation of this method is required to use the base class
		 * implementation of getInstances.
		 */
		virtual void populateAttributeList(attribute_names_t &attributes) = 0;

		/*!
		 * Verify the attributes list
		 * @param attributes
		 * 		The list of attributes passed by the called
		 * @throw NvmExceptionBadParameter if an attribute is invalid
		 */
		void checkAttributes(attribute_names_t &attributes);

		/*
		 * Check that each paths' keys exist in an object path received from getInstanceNames.
		 *
		 * Note: This function performs a getInstanceNames call, so it is not ideal in all cases. It
		 * is best used when there will only be a single object path for a class and most properties
		 * have static values.
		 */
		void checkPath(const ObjectPath &path);
};
}
}
#endif  // #ifndef _WBEM_FRAMEWORK_INSTANCE_FACTORY_H_
