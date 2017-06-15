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
 * This file contains the implementation of the Attribute class which
 * defines a a generic Intel CIM instance attribute.
 */


#ifndef	_WBEM_FRAMEWORK_ATTRIBUTE_H_
#define	_WBEM_FRAMEWORK_ATTRIBUTE_H_

#include <string>
#include <map>
#include <vector>

#include "Exception.h"
#include "Types.h"

namespace wbem
{
namespace framework
{

/*!
 * Subtypes of the datetime type
 */
enum DatetimeSubtype
{
	DATETIME_SUBTYPE_DATETIME,
	DATETIME_SUBTYPE_INTERVAL
};

/*!
 * The data type of the attribute.
 */
enum DataType
{
	BOOLEAN_T,    //!< Boolean.
	UINT8_T,      //!< Unsigned 8 bit integer.
	UINT16_T,     //!< Unsigned 16 bit integer.
	UINT32_T,     //!< Unsigned 32 bit integer.
	UINT64_T,     //!< Unsigned 64 bit integer.
	SINT8_T,      //!< Signed 8 bit integer.
	SINT16_T,     //!< Signed 16 bit integer.
	SINT32_T,     //!< Signed 32 bit integer.
	SINT64_T,     //!< Signed 64 bit integer.
	REAL32_T,     //!< Floating point number.
	STR_T,        //!< String.
	UINT8_LIST_T,//!< Vector of unsigned 8 bit integers.
	UINT16_LIST_T,//!< Vector of unsigned 16 bit integers.
	UINT32_LIST_T,//!< Vector of unsigned 32 bit integers.
	UINT64_LIST_T,//!< Vector of unsigned 64 bit integers.
	STR_LIST_T,   //!< Vector of strings.
	BOOLEAN_LIST_T,//!< Vector of booleans.
	ENUM_T,       //!< Enumeration meaning a string and an integer.
	ENUM16_T,	  //!< Enumeration with the integer as a uint16
	DATETIME_T,	//!< datetime
	DATETIME_INTERVAL_T //!< datetime interval
};

/*!
 * A generic property of an NvmInstance represented as a key/value pair.
 */
class Attribute
{
	public:

		/*!
		 * Default constructor when not given a specific type.
		 */
		Attribute() :
			m_IsKey(false),
			m_IsEmbedded(false),
			m_IsAssociationClassInstance(false),
			m_Type(BOOLEAN_T)
		{};

		/*!
		 * Destructor.
		 */
		~Attribute();

		/*!
		 * Copy constructor.
		 * @param[in] attribute
		 * 		The attribute to make a copy of.
		 */
		Attribute(const Attribute &attribute);

		/*!
		 * Constructor for a string attribute given a char *.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(const char* value, bool isKey);

		/*!
		 * Constructor for a string attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(const STR &value, bool isKey);

		/*!
		 * Constructor for a boolean attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(BOOLEAN value, bool isKey);

		/*!
		 * Constructor for an unsigned 8 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT8 value, bool isKey);

		/*!
		 * Constructor for an unsigned 16 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT16 value, bool isKey);

		/*!
		 * Constructor for an unsigned 32 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT32 value, bool isKey);

		/*!
		 * Constructor for an unsigned 64 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT64 value, bool isKey);

		/*!
		 * Constructor for a floating point 32 bit number attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(REAL32 value, bool isKey);

		/*!
		 * Constructor for a datetime attribute.
		 * @param[in] value
		 * 		The string value to initialized the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the instance.
		 * 	@param[in]
		 * 		The subtype of type datetime this attribute will represent
		 * @throw NvmException if the string doesn't match the datetime subtype
		 */
		Attribute(const char *value, enum DatetimeSubtype type, bool isKey)
			throw (Exception);

		/*!
		 * Constructor for a datetime attribute.
		 * @param[in] value
		 * 		The string value to initialized the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the instance.
		 * 	@param[in]
		 * 		The subtype of type datetime this attribute will represent
		 * @throw NvmException if the string doesn't match the datetime subtype
		 */
		Attribute(const STR &value, enum DatetimeSubtype type, bool isKey)
			throw (Exception);

		/*!
		 * Constructor for a datetime attribute.
		 * @param[in] value
		 * 		The value to initialized the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the instance.
		 * 	@param[in]
		 * 		The subtype of type datetime this attribute will represent
		 * @throw NvmException if the string doesn't match the datetime subtype
		 */
		Attribute(UINT64 value, enum DatetimeSubtype type, bool isKey)
			throw (Exception);

		/*!
		 * Constructor for a signed 8 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(SINT8 value, bool isKey);

		/*!
		 * Constructor for a signed 16 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(SINT16 value, bool isKey);

		/*!
		 * Constructor for a signed 32 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(SINT32 value, bool isKey);

		/*!
		 * Constructor for a signed 64 bit integer attribute.
		 * @param[in] value
		 * 		The value to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(SINT64 value, bool isKey);

		/*!
		 * Constructor for a vector of unsigned 8 bit integers attribute.
		 * @param[in] values
		 * 		The list of values to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT8_LIST values, bool isKey);

		/*!
		 * Constructor for a vector of unsigned 16 bit integers attribute.
		 * @param[in] values
		 * 		The list of values to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT16_LIST values, bool isKey);

		/*!
		 * Constructor for a vector of unsigned 32 bit integers attribute.
		 * @param[in] values
		 * 		The list of values to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT32_LIST values, bool isKey);

		/*!
		 * Constructor for a vector of unsigned 64 bit integers attribute.
		 * @param[in] values
		 * 		The list of values to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT64_LIST values, bool isKey);

		/*!
		 * Constructor for a vector of strings attribute.
		 * @param[in] values
		 * 		The list of values to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(STR_LIST values, bool isKey);

		/*!
		 * Constructor for a vector of booleans attribute.
		 * @param[in] values
		 * 		The list of values to initialize the attribute to.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(BOOLEAN_LIST values, bool isKey);

		/*!
		 * Constructor for an enumeration attribute.
		 * @param[in] enumValue
		 * 		The value to initialize the attribute to.
		 * @param[in] enumString
		 * 		The string identifying the enumeration.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT32 enumValue, const STR enumString, bool isKey);

		/*!
		 * Constructor for an enumeration attribute.
		 * @param[in] enumValue
		 * 		The value to initialize the attribute to.
		 * @param[in] enumString
		 * 		The string identifying the enumeration.
		 * @param[in] isKey
		 * 		True if this attribute is a key of the Instance.
		 */
		Attribute(UINT16 enumValue, const STR enumString, bool isKey);

		/*!
		 * Sets the attribute to the attribute specified.
		 * @param[in] rhs
		 * 		The attribute to set this attribute to.
		 * @return Returns a pointer to this object.
		 */
		Attribute& operator=(const Attribute& rhs);

		/*!
		 * Converts the attribute value to a string.
		 * @remarks Used by the CLI to display data to the screen and XML.
		 * 	The default uses an empty string for the prefix and suffix,
		 * 	and the string ", " as the list separator.
		 * @param[in] prefix
		 * 		string to put before each value
		 * @param[in] suffix
		 * 		string to put after each value
		 * @param[in] sep
		 * 		string used to separate multiple values
		 * @return The attribute value as a string.
		 */
		std::string asStr() const;
		std::string asStr(std::string prefix, std::string suffix,
			std::string sep) const;


		/*!
		 * Is the attribute a key.
		 * @return true if the attribute is a key, false if not.
		 */
		bool isKey() const;

		/*!
		 * Is the attribute one of the array types?
		 * @return true if it's an array, false if not
		 */
		bool isArray() const;

		/*!
		 * Is the attribute one of the numeric types?
		 * @remark This includes enums, and signed and unsigned ints of various sizes.
		 * 		It doesn't include non-numeric values that can be translated to numbers
		 * 		(booleans or datetimes) or lists of numbers.
		 * @return true if it's a number, false if not
		 */
		bool isNumeric() const;

		/*!
		 * Retrieve the attribute value as a string.
		 * @remarks Only valid for string or enumeration attributes.
		 * @return Returns the attribute value or an empty string for a non-string attribute.
		 */
		std::string stringValue() const;

		/*!
		 * Retrieve the attribute value as an integer.
		 * @return The attribute value or 0 for a non-number type attribute.
		 */
		int intValue() const;

		/*!
		 * Retrieve the attribute value as an unsigned integer.
		 * @return The attribute value or 0 for a non-number type attribute.
		 */
		unsigned int uintValue() const;

		/*!
		 * Retrieve the attribute value as a floating point number.
		 * @return The attribute value if the attribute type is NVM_REAL32_T
		 */
		REAL32 real32Value() const;

		/*!
		 * Retrieve the attribute value as a boolean.
		 * @return The attribute value or false for non-number/boolean type attributes.
		 */
		bool boolValue() const;

		/*!
		 * Retrieve the attribute value as a 64 bit signed integer.
		 * @return The attribute value or 0 for a non-number type attribute.
		 */
		SINT64 sint64Value() const;

		/*!
		 * Retrieve the attribute value as a 64 bit unsigned integer.
		 * @return The attribute value or 0 for a non-number type attribute.
		 */
		UINT64 uint64Value() const;

		/*!
		 * Retrieve the list of 8 bit unsigned integer attribute values.
		 * @return The attribute values or an empty list if not type NVM_UINT8_LIST_T.
		 */
		UINT8_LIST uint8ListValue() const;


		/*!
		 * Retrieve the list of 16 bit unsigned integer attribute values.
		 * @return The attribute values or an empty list if not type NVM_UINT16_LIST_T.
		 */
		UINT16_LIST uint16ListValue() const ;

		/*!
		 * Retrieve the list of 32 bit unsigned integer attribute values.
		 * @return The attribute values or an empty list if not type NVM_UINT32_LIST_T.
		 */
		UINT32_LIST uint32ListValue() const;

		/*!
		 * Retrieve the list of 64 bit unsigned integer attribute values.
		 * @return The attribute values or an empty list if not type NVM_UINT64_LIST_T.
		 */
		UINT64_LIST uint64ListValue() const;

		/*!
		 * Retrieve the list of string attribute values.
		 * @return The attribute values or an empty list if not type NVM_STR_LIST_T.
		 */
		STR_LIST strListValue() const;

		/*!
		 * Retrieve the list of string attribute values.
		 * @return The attribute values or an empty list if not type NVM_BOOLEAN_LIST_T.
		 */
		BOOLEAN_LIST booleanListValue() const;

		/*!
		 * Retrieve the attribute type.
		 * @return The attribute type enumeration value.
		 */
		enum DataType getType() const;

		/*
		 * Equality operator
		 */
		bool operator ==(const Attribute& rhs) const;

		/*
		 * Inequality operator
		 */
		bool operator !=(const Attribute& rhs) const;

	bool isEmbedded();

	void setIsEmbedded(bool value);

	bool isAssociationClassInstance();

	void setIsAssociationClassInstance(bool value);

	void setIsKey(bool isKey);

	static bool typesMatch(enum DataType lhs, enum DataType rhs);

	void normalize();

private:
		union attribute_t
		{
			BOOLEAN boolean;
			SINT8 sint8;
			UINT8 uint8;
			SINT16 sint16;
			UINT16 uint16;
			SINT32 sint32;
			UINT32 uint32;
			SINT64 sint64;
			UINT64 uint64;
			REAL32 real32;
		};

		bool m_IsKey;
		bool m_IsEmbedded;
		bool m_IsAssociationClassInstance;
		attribute_t m_Value;
		DataType m_Type;
		STR m_Str;
		UINT8_LIST m_UInt8List;
		UINT16_LIST m_UInt16List;
		UINT32_LIST m_UInt32List;
		UINT64_LIST m_UInt64List;
		STR_LIST m_StrList;
		BOOLEAN_LIST m_BooleanList;

		/*
		 * Helper function to compare lists of values
		 */
		template <typename TYPE>
		bool listEqual(const TYPE list1, const TYPE list2) const;
		void trimStr(STR &str);
};

/*!
 * Defines a map of an attribute and a name.
 */
typedef std::map<std::string, Attribute> attributes_t;
/*!
 * iterator for mutable attributes_t element
 */
typedef attributes_t::iterator attributes_itr_t;
/*!
 * iterator for non-mutable attributes_t element
 */
typedef attributes_t::const_iterator attributes_const_itr_t;

/*!
 * Defines a list of attribute names.
 */
typedef std::vector<std::string> attribute_names_t;
/*!
 * iterator for mutable attributes_names_t element
 */
typedef attribute_names_t::iterator attribute_names_itr_t; //!< mutable iterator for attribute_names_t
/*!
 * iterator for non-mutable attributes_names_t element
 */
typedef attribute_names_t::const_iterator attribute_names_const_itr_t;

} // framework
} // wbem
#endif  // #ifndef _WBEM_FRAMEWORK_NVMATTRIBUTE_H_


