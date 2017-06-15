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
 * This file defines an Intel CIM object path. Object paths are essentially a
 * multi-part key that uniquely identifies a given CIM instance.
 */

#ifndef	_WBEM_FRAMEWORK_OBJECT_PATH_H_
#define	_WBEM_FRAMEWORK_OBJECT_PATH_H_

#include <iostream>
#include <string>
#include <vector>

#include "Attribute.h"
#include "Exception.h"


namespace wbem
{
namespace framework
{

/*!
 * A CIM object path is a multi-part key that uniquely identifies a given CIM instance.
 */
class ObjectPath
{
	public:

		/*!
		 * Initialize an empty object path.
		 */
		ObjectPath();

		/*!
		 * Initialize a new object path.
		 * @param[in] host
		 * 		The host server name.
		 * @param[in] wbem_namespace
		 * 		The CIM namespace.
		 * @param[in] wbem_class
		 * 		The NvmInstance class name.
		 * @param[in] keys
		 *		A list of the key attributes that uniquely identify the instance.
		 */
		ObjectPath(std::string host, std::string wbem_namespace, std::string
				wbem_class, attributes_t keys);

		/*!
		 * Clean up the object path
		 */
		~ObjectPath();

		/*!
		 * Convert the object path to a CIM standard string.
		 * @param ignoreHostName
		 * 		If the ignoreHostName flag is true then a '.' will be used instead of the host name.
		 * 		This is useful for simulated builds because in the simulator the
		 * 		host name of a path is still real. The CIMOM creates the full object path and will
		 * 		include the real host name. This makes associations difficult because 'ref' object paths are
		 * 		constructed by the association provider and therefore have the simulated host name
		 * 		which won't match up with the real host name. A '.' for host name just means local.
		 * @return The object path as a string.
		 */
		std::string asString(bool ignoreHostName = false) const;

		/*!
		 * Retrieve the specified key.
		 * @param[in] key
		 * 		The name of the key to retrieve.
		 * @throw NvmException on invalid key name.
		 * @return The key attribute.
		 */
		const Attribute& getKeyValue(const std::string& key) const throw (Exception);

		/*
		 * Check that the key attribute exists and has the same string value as expected.
		 * If it doesn't it will throw an exception.
		 */
		void checkKey(const std::string &key, const std::string &expected) const throw (Exception);

		/*!
		 * Retrieve the list of key attributes that uniquely identify the instance.
		 * @return
		 */
		const attributes_t& getKeys() const;

		/*!
		 * Retrieve the NvmInstance class name.
		 * @return The class name.
		 */
		const std::string& getClass() const;

		/*!
		 * Retrieve the host server name.
		 * @return The host server name.
		 */
		const std::string& getHost() const;

		/*!
		 * Retrieve the NvmInstance namespace name.
		 * @return The namespace name.
		 */
		const std::string& getNamespace() const;

		/*!
		 * Determine if the specified object path is equal.
		 * @param[in] rhs
		 * 		The object path to compare.
		 * @return
		 * 		True if equal.
		 * 		False if not equal.
		 */
		bool operator==(const ObjectPath &rhs);

		/*!
		 * Determine if the specified object path is not equal.
		 * @param[in] rhs
		 * 		The object path to compare.
		 * @return
		 * 		True if not equal.
		 * 		False if equal.
		 */
		bool operator!=(const ObjectPath &rhs);

		/*!
		 * Determine if the object path is empty.
		 * @return
		 * 		True if empty.
		 * 		False if not empty.
		 */
		bool empty();

		/*!
		 * Set the properties of the object path to those specified.
		 * @param[in] host
		 *		The host server name.
		 * @param[in] wbem_namespace
		 * 		The NvmInstance namespace name.
		 * @param[in] wbem_class
		 * 		The NvmInstance class name.
		 * @param[in] keys
		 * 		The key attributes that uniquely identify the instance.
		 */
		void setObjectPath(std::string host, std::string wbem_namespace,
			std::string wbem_class, attributes_t keys);

	private:
		std::string m_Host;
		std::string m_Namespace;
		std::string m_Class;

		attributes_t m_Keys;
};

/*!
 * A list of instance object paths.
 */
typedef std::vector<ObjectPath> instance_names_t;

}
}
#endif  // #ifndef _WBEM_FRAMEWORK_OBJECT_PATH_H_

