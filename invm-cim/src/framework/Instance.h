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
 * This file contains the base class for Intel CIM instances.
 */

#ifndef	_WBEM_FRAMEWORK_INSTANCE_H_
#define	_WBEM_FRAMEWORK_INSTANCE_H_

#include <map>
#include <string>

#include "Attribute.h"
#include "Exception.h"
#include "ObjectPath.h"


namespace wbem
{
namespace framework
{

/*!
 * The base class for all CIM instances.
 */
class Instance
{
	public:
		/*!
		 * Initialize a new instance and create an empty attribute map.
		 */
		Instance();

		/*!
		 * Initialize a new instance from an object path.
		 * @param[in] path
		 * 		The object path.
		 */
		Instance(ObjectPath &path);

		/*!
		 * Initialize a new instance from a CIM XML string.
		 * @param[in] cimXml
		 * 		The XML string.
		 * @throw
		 * 		NvmException if the XML string is not valid.
		 */
		Instance(std::string cimXml);

		/*!
		 * Retrieve the specified attribute.
		 * @param[in] key
		 * 		The name of the attribute.
		 * @param[in,out] value
		 * 		The attribute.
		 * @return
		 * 		wbem::framework::SUCCESS if the attribute was found.
		 * 		wbem::framework::FAIL if the attribute was not found.
		 */
		int getAttribute(const std::string& key, wbem::framework::Attribute& value) const;

		/*!
		 * Retrieve the specified attribute and update the key name to the proper case if required
		 * @param[in] key
		 * 		The name of the attribute.
		 * @param[in,out] value
		 * 		The attribute.
		 * @return
		 * 		wbem::framework::SUCCESS if the attribute was found.
		 * 		wbem::framework::FAIL if the attribute was not found.
		 */
		int getAttributeI(std::string& key, wbem::framework::Attribute& value) const;

		/*!
		 * Add the specified attribute.
		 * @param[in] key
		 * 		The name of the attribute.
		 * @param[in] value
		 * 		The attribute to get.
		 * @return
		 * 		wbem::framework::SUCCESS.
		 */
		int setAttribute(const std::string& key, const wbem::framework::Attribute& value);

		/*!
		 * Add the specified attribute if the specified list of attribute names is
		 * empty (implying add all) or the list contains the name of the attribute.
		 * @param key
		 * 		The name of the attribute to add.
		 * @param value
		 * 		The attribute to add.
		 * @param attributes
		 * 		The list of attribute names.  An empty list means add all attributes.
		 * @remarks This method is used for attribute filtering.
		 * @return
		 * 		wbem::framework::SUCCESS.
		 */
		int setAttribute(const std::string& key, const framework::Attribute &value,
			const framework::attribute_names_t &attributes);

		/*!
		 * Convert the instance into an NvmObjectPath.
		 * @return
		 * 		The object path.
		 */
		framework::ObjectPath getObjectPath() const;

		/*!
		 * Retrieve the beginning of the attributes list.
		 * @return
		 * 		An iterator pointing to the beginning of the attributes list.
		 */
		attributes_t::const_iterator attributesBegin() const;

		/*!
		 * Retrieve the end of the attributes list.
		 * @return
		 * 		An iterator pointing to the end of the attributes list.
		 */
		attributes_t::const_iterator attributesEnd() const;

		/*!
		 * Returns the number of total attributes in the instance (includes key attributes)
		 */
		size_t attributesCount() const;

		/*!
		 * Convert the instance into a CIM XML string.
		 * @return
		 * 		The CIM XML string.
		 */
		std::string getCimXml() const;

		/*!
		 * Get the Instance class.
		 * @return
		 * 		The Instance class.
		 */
		std::string getClass() const;

		/*!
		 * Get the Instance host.
		 * @return
		 * 		The Instance host.
		 */
		std::string getHost() const;

		/*!
		 * Get the Instance namespace.
		 * @return
		 * 		The Instance namespace.
		 */
		std::string getNamespace() const;

		/*!
		 *
		 * @param pInstance
		 * @param attributes
		 * @param attributeKey
		 * @param attributeValue
		 */
		template<typename T>
		static void setAttributeToInstance(Instance *pInstance, attribute_names_t &attributes,
				std::string &attributeKey, const T &attributeValue);

		/*
		 * Equality operator
		 */
		bool operator == (const Instance& rhs) const;

		/*
		 * Inequality operator
		 */
		bool operator != (const Instance& rhs) const;
	private:
		std::string m_Class;
		std::string m_Host;
		std::string m_Namespace;
		attributes_t m_InstanceAttributes;
};

/*!
 * A list of Instance objects.
 */
typedef std::vector<Instance> instances_t;

}  // framework
}  // wbem


/*!
 * Inline helper method to set the value of an attribute on the specified instance
 * if it is included in the specified list of attribute names.
 * @param pInstance
 * 		The instance to set the value of the attribute on.
 * @param attributes
 * 		The list of attribute names.
 * @param attributeKey
 * 		The attribute name.
 * @param attributeValue
 * 		The attribute value.
 */
template<typename T>
inline void wbem::framework::Instance::setAttributeToInstance(Instance* pInstance,
		attribute_names_t& attributes, std::string& attributeKey, const T& attributeValue)
{
	// loop through each desired attribute name to see if attributeKey is wanted
	for (attribute_names_t::const_iterator iter=attributes.begin(); iter!=attributes.end(); ++iter)
	{
	if (iter->compare(attributeKey) == 0) // found attributeKey ... add
		{
			framework::Attribute attribute(attributeValue, false);
			pInstance->setAttribute(attributeKey, attribute);
		}
	}
}

#endif  // #ifndef _WBEM_FRAMEWORK_INSTANCE_H_
