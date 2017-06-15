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

#include <string>
#include <sstream>

#include <time/time_utilities.h>
#include <logger/logging.h>

// output formatting
#include <iomanip>
#include "Attribute.h"
#include "ExceptionBadParameter.h"

wbem::framework::Attribute::~Attribute()
{
}

wbem::framework::Attribute::Attribute(const wbem::framework::Attribute &attribute)
{
	m_IsKey = attribute.m_IsKey;
	m_Type = attribute.m_Type;
	m_IsEmbedded = attribute.m_IsEmbedded;
	m_IsAssociationClassInstance = attribute.m_IsAssociationClassInstance;

	switch (m_Type)
	{
		case BOOLEAN_T:
			m_Value.boolean = attribute.m_Value.boolean;
			break;
		case UINT8_T:
			m_Value.uint8 = attribute.m_Value.uint8;
			break;
		case UINT16_T:
			m_Value.uint16 = attribute.m_Value.uint16;
			break;
		case UINT32_T:
			m_Value.uint32 = attribute.m_Value.uint32;
			break;
		case DATETIME_T:
		case DATETIME_INTERVAL_T:
		case UINT64_T:
			m_Value.uint64 = attribute.m_Value.uint64;
			break;
		case SINT8_T:
			m_Value.sint8 = attribute.m_Value.sint8;
			break;
		case SINT16_T:
			m_Value.sint16 = attribute.m_Value.sint16;
			break;
		case SINT32_T:
			m_Value.sint32 = attribute.m_Value.sint32;
			break;
		case SINT64_T:
			m_Value.sint64 = attribute.m_Value.sint64;
			break;
		case STR_T:
			m_Str = attribute.m_Str;
			break;
		case UINT8_LIST_T:
			m_UInt8List = attribute.m_UInt8List;
			break;
		case UINT16_LIST_T:
			m_UInt16List = attribute.m_UInt16List;
			break;
		case UINT32_LIST_T:
			m_UInt32List = attribute.m_UInt32List;
			break;
		case UINT64_LIST_T:
			m_UInt64List = attribute.m_UInt64List;
			break;
		case STR_LIST_T:
			m_StrList = attribute.m_StrList;
			break;
		case BOOLEAN_LIST_T:
			m_BooleanList = attribute.m_BooleanList;
			break;
		case ENUM16_T:
			m_Str = attribute.m_Str;
			m_Value.uint16 = attribute.m_Value.uint16;
			break;
		case ENUM_T:
		default:
			m_Str = attribute.m_Str;
			m_Value.uint32 = attribute.m_Value.uint32;
			break;
	}
}

wbem::framework::Attribute::Attribute(const STR &value, bool isKey)
{
	m_Type = STR_T;
	m_Str = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(const char *value, bool isKey)
{
	m_Type = STR_T;
	m_Str = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(BOOLEAN value, bool isKey)
{
	m_Type = BOOLEAN_T;
	m_Value.boolean = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(SINT8 value, bool isKey)
{
	m_Type = SINT8_T;
	m_Value.sint8 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT8 value, bool isKey)
{
	m_Type = UINT8_T;
	m_Value.uint8 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(SINT16 value, bool isKey)
{
	m_Type = SINT16_T;
	m_Value.sint16 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT16 value, bool isKey)
{
	m_Type = UINT16_T;
	m_Value.uint16 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT32 value, bool isKey)
{
	m_Type = UINT32_T;
	m_Value.uint32 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(SINT32 value, bool isKey)
{
	m_Type = SINT32_T;
	m_Value.sint32 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(SINT64 value, bool isKey)
{
	m_Type = SINT64_T;
	m_Value.sint64 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT64 value, bool isKey)
{
	m_Type = UINT64_T;
	m_Value.uint64 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(REAL32 value, bool isKey)
{
	m_Type = REAL32_T;
	m_Value.real32 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(const char *value, enum DatetimeSubtype type, bool isKey)
	throw (Exception)
{
	enum datetime_type strType = convert_datetime_string_to_seconds(value, &(m_Value.uint64));
	m_IsKey = isKey;
	if ((strType == DATETIME_TYPE_DATETIME) && (type == DATETIME_SUBTYPE_DATETIME))
	{
		m_Type = DATETIME_T;
	}
	else if ((strType == DATETIME_TYPE_INTERVAL) && (type == DATETIME_SUBTYPE_INTERVAL))
	{
		m_Type = DATETIME_INTERVAL_T;
	}
	else // string format not valid
	{
		throw ExceptionBadParameter("value");
	}
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(const STR &value, enum DatetimeSubtype type, bool isKey)
	throw (Exception)
{
	enum datetime_type strType = convert_datetime_string_to_seconds(value.c_str(), &(m_Value.uint64));
	m_IsKey = isKey;

	if ((strType == DATETIME_TYPE_DATETIME) && (type == DATETIME_SUBTYPE_DATETIME))
	{
		m_Type = DATETIME_T;
	}
	else if ((strType == DATETIME_TYPE_INTERVAL) && (type == DATETIME_SUBTYPE_INTERVAL))
	{
		m_Type = DATETIME_INTERVAL_T;
	}
	else // string format not valid
	{
		COMMON_LOG_ERROR_F("bad value for datetime: %s (type: %d), expected type: %d",
				value.c_str(), strType, type);
		throw ExceptionBadParameter("value");
	}
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(const UINT64 value, enum DatetimeSubtype type, bool isKey)
	throw (Exception)
{
	m_Value.uint64 = value;
	m_IsKey = isKey;
	if (type == DATETIME_SUBTYPE_DATETIME)
	{
		m_Type = DATETIME_T;
	}
	else
	{
		m_Type = DATETIME_INTERVAL_T;
	}
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT8_LIST values, bool isKey)
{
	m_Type = UINT8_LIST_T;
	m_UInt8List = values;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT16_LIST values, bool isKey)
{
	m_Type = UINT16_LIST_T;
	m_UInt16List = values;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT32_LIST values, bool isKey)
{
	m_Type = UINT32_LIST_T;
	m_UInt32List = values;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT64_LIST values, bool isKey)
{
	m_Type = UINT64_LIST_T;
	m_UInt64List = values;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(STR_LIST values, bool isKey)
{
	m_Type = STR_LIST_T;
	m_StrList = values;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(BOOLEAN_LIST values, bool isKey)
{
	m_Type = BOOLEAN_LIST_T;
	m_BooleanList = values;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT32 value, const STR enumString, bool isKey)
{
	m_Type = ENUM_T;
	m_Str = enumString;
	m_Value.uint32 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

wbem::framework::Attribute::Attribute(UINT16 value, const STR enumString, bool isKey)
{
	m_Type = ENUM16_T;
	m_Str = enumString;
	m_Value.uint16 = value;
	m_IsKey = isKey;
	m_IsEmbedded = false;
	m_IsAssociationClassInstance = false;
}

bool wbem::framework::Attribute::isKey() const
{
	return m_IsKey;
}

std::string wbem::framework::Attribute::stringValue() const
{
	if (m_Type == STR_T || m_Type == ENUM_T || m_Type == ENUM16_T)
	{
		return m_Str;
	}
	if (m_Type == DATETIME_T)
	{
		COMMON_DATETIME_STR datetime;
		convert_seconds_to_datetime(m_Value.uint64, datetime);
		return datetime;
	}
	if (m_Type == DATETIME_INTERVAL_T)
	{
		COMMON_DATETIME_STR interval;
		convert_seconds_to_datetime_interval(m_Value.uint64, interval);
		return interval;
	}
	COMMON_LOG_WARN_F("Attempted to get stringValue of an attribute of type: %d", m_Type);
	return std::string();
}


/*
 * Convert the value to an integer and return
 */
int wbem::framework::Attribute::intValue() const
{
	int result;
	switch (m_Type)
	{
		case UINT8_T:
			result = (int)m_Value.uint8;
			break;
		case ENUM16_T:
		case UINT16_T:
			result = (int)m_Value.uint16;
			break;
		case UINT32_T:
			result = (int)m_Value.uint32;
			break;
		case DATETIME_INTERVAL_T:
		case DATETIME_T:
			case UINT64_T:
			result = (int)m_Value.uint64;
			break;
		case SINT8_T:
			result = (int)m_Value.sint8;
			break;
		case SINT16_T:
			result = (int)m_Value.sint16;
			break;
		case SINT32_T:
			result = (int)m_Value.sint32;
			break;
		case SINT64_T:
			result = (int)m_Value.sint64;
			break;
		case REAL32_T:
			result = (int)m_Value.real32;
			break;
		case BOOLEAN_T:
			result = (m_Value.boolean ? 1 : 0);
			break;
		case ENUM_T:
			result = (int)m_Value.uint32;
			break;
		default:
			COMMON_LOG_ERROR("Invalid type.");
			result = 0;
			break;
	}

	return result;
}


/*
 * Convert the value to an unsigned integer and return
 */
unsigned int wbem::framework::Attribute::uintValue() const
{
	unsigned int result;


	switch (m_Type)
	{
		case UINT8_T:
			result = (unsigned int)m_Value.uint8;
			break;
		case ENUM16_T:
		case UINT16_T:
			result = (unsigned int)m_Value.uint16;
			break;
		case ENUM_T:
		case UINT32_T:
			result = (unsigned int)m_Value.uint32;
			break;
		case DATETIME_T:
		case DATETIME_INTERVAL_T:
		case UINT64_T:
			result = (unsigned int)m_Value.uint64;
			break;
		case SINT8_T:
			result = (unsigned int)m_Value.sint8;
			break;
		case SINT16_T:
			result = (unsigned int)m_Value.sint16;
			break;
		case SINT32_T:
			result = (unsigned int)m_Value.sint32;
			break;
		case SINT64_T:
			result = (unsigned int)m_Value.sint64;
			break;
		case BOOLEAN_T:
			result = (m_Value.boolean ? 1 : 0);
			break;
		case REAL32_T:
			result = (unsigned int)m_Value.real32;
			break;
		default:
			COMMON_LOG_ERROR("Invalid type.");
			result = 0;
			break;
	}

	return result;
}

/*
 * Convert the value to an unsigned int 64 and return
 */
wbem::framework::UINT64 wbem::framework::Attribute::uint64Value() const
{
	UINT64 result;
	switch (m_Type)
	{
		case UINT8_T:
			result = (UINT64)m_Value.uint8;
			break;
		case ENUM16_T:
		case UINT16_T:
			result = (UINT64)m_Value.uint16;
			break;
		case ENUM_T:
		case UINT32_T:
			result = (UINT64)m_Value.uint32;
			break;
		case DATETIME_T:
		case DATETIME_INTERVAL_T:
		case UINT64_T:
			result = (UINT64)m_Value.uint64;
			break;
		case SINT8_T:
			result = (UINT64)m_Value.sint8;
			break;
		case SINT16_T:
			result = (UINT64)m_Value.sint16;
			break;
		case SINT32_T:
			result = (UINT64)m_Value.sint32;
			break;
		case SINT64_T:
			result = (UINT64)m_Value.sint64;
			break;
		case REAL32_T:
			result = (UINT64)m_Value.real32;
			break;
		case BOOLEAN_T:
			result = (m_Value.boolean ? 1 : 0);
			break;
		default:
			COMMON_LOG_ERROR("Invalid type.");
			result = 0;
			break;
	}

	return result;
}

/*
 * Convert the value to an int 64 and return
 */
wbem::framework::SINT64 wbem::framework::Attribute::sint64Value() const
{
	SINT64 result;
	switch (m_Type)
	{
		case UINT8_T:
			result = (SINT64)m_Value.uint8;
			break;
		case ENUM16_T:
		case UINT16_T:
			result = (SINT64)m_Value.uint16;
			break;
		case ENUM_T:
		case UINT32_T:
			result = (SINT64)m_Value.uint32;
			break;
		case DATETIME_T:
		case DATETIME_INTERVAL_T:
		case UINT64_T:
			result = (SINT64)m_Value.uint64;
			break;
		case SINT8_T:
			result = (SINT64)m_Value.sint8;
			break;
		case SINT16_T:
			result = (SINT64)m_Value.sint16;
			break;
		case SINT32_T:
			result = (SINT64)m_Value.sint32;
			break;
		case SINT64_T:
			result = m_Value.sint64;
			break;
		case BOOLEAN_T:
			result = (m_Value.boolean ? 1 : 0);
			break;
		case REAL32_T:
			result = (SINT64)m_Value.real32;
			break;
		default:
			COMMON_LOG_ERROR("Invalid type.");
			result = 0;
			break;
	}

	return result;
}

/*
 * Convert the value to a boolean and return it
 */
bool wbem::framework::Attribute::boolValue() const
{
	bool result;
	switch (m_Type)
	{
		case BOOLEAN_T:
			result = m_Value.boolean;
			break;
		case UINT8_T:
			result = (m_Value.uint8 > 0 ? true : false);
			break;
		case ENUM16_T:
		case UINT16_T:
			result = (m_Value.uint16 > 0 ? true : false);
			break;
		case ENUM_T:
		case UINT32_T:
			result = (m_Value.uint32 > 0 ? true : false);
			break;
		case DATETIME_T:
		case DATETIME_INTERVAL_T:
		case UINT64_T:
			result = (m_Value.uint64 > 0 ? true : false);
			break;
		case SINT8_T:
			result = (m_Value.sint8 > 0 ? true : false);
			break;
		case SINT16_T:
			result = (m_Value.sint16 > 0 ? true : false);
			break;
		case SINT32_T:
			result = (m_Value.sint32 > 0 ? true : false);
			break;
		case SINT64_T:
			result = (m_Value.sint64 > 0 ? true : false);
			break;
		case REAL32_T:
			result = (m_Value.real32 != 0 ? true : false);
			break;

		default:
			result = false;
			COMMON_LOG_ERROR("Invalid type.");
			break;
	}

	return result;
}

wbem::framework::REAL32 wbem::framework::Attribute::real32Value() const
{
	return m_Value.real32;
}

wbem::framework::UINT8_LIST wbem::framework::Attribute::uint8ListValue() const
{
	UINT8_LIST result;
	if (m_Type == UINT8_LIST_T)
	{
		result = m_UInt8List;
	}
	else
	{
		COMMON_LOG_ERROR("Invalid type.");
	}
	return result;
}

wbem::framework::UINT16_LIST wbem::framework::Attribute::uint16ListValue() const
{
	UINT16_LIST result;
	if (m_Type == UINT16_LIST_T)
	{
		result = m_UInt16List;
	}
	else
	{
		COMMON_LOG_ERROR("Invalid type.");
	}
	return result;
}

wbem::framework::UINT32_LIST wbem::framework::Attribute::uint32ListValue() const
{
	UINT32_LIST result;
	if (m_Type == UINT32_LIST_T)
	{
		result = m_UInt32List;
	}
	else
	{
		COMMON_LOG_ERROR("Invalid type.");
	}
	return result;
}

wbem::framework::UINT64_LIST wbem::framework::Attribute::uint64ListValue() const
{
	UINT64_LIST result;
	if (m_Type == UINT64_LIST_T)
	{
		result = m_UInt64List;
	}
	else
	{
		COMMON_LOG_ERROR("Invalid type.");
	}
	return result;
}

wbem::framework::STR_LIST wbem::framework::Attribute::strListValue() const
{
	STR_LIST result;
	if (m_Type == STR_LIST_T)
	{
		result = m_StrList;
	}
	else
	{
		COMMON_LOG_ERROR("Invalid type.");
	}
	return result;
}

wbem::framework::BOOLEAN_LIST wbem::framework::Attribute::booleanListValue() const
{
	BOOLEAN_LIST result;
	if (m_Type == BOOLEAN_LIST_T)
	{
		result = m_BooleanList;
	}
	else
	{
		COMMON_LOG_ERROR("Invalid type.");
	}
	return result;
}

wbem::framework::Attribute& wbem::framework::Attribute::operator=(const Attribute& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	m_IsKey = rhs.m_IsKey;
	m_Type = rhs.m_Type;
	m_IsEmbedded = rhs.m_IsEmbedded;
	m_IsAssociationClassInstance = rhs.m_IsAssociationClassInstance;

	switch (m_Type)
	{
		case BOOLEAN_T:
			m_Value.boolean = rhs.m_Value.boolean;
			break;
		case UINT8_T:
			m_Value.uint8 = rhs.m_Value.uint8;
			break;
		case UINT16_T:
			m_Value.uint16 = rhs.m_Value.uint16;
			break;
		case UINT32_T:
			m_Value.uint32 = rhs.m_Value.uint32;
			break;
		case DATETIME_INTERVAL_T:
		case DATETIME_T:
		case UINT64_T:
			m_Value.uint64 = rhs.m_Value.uint64;
			break;
		case SINT8_T:
			m_Value.sint8 = rhs.m_Value.sint8;
			break;
		case SINT16_T:
			m_Value.sint16 = rhs.m_Value.sint16;
			break;
		case SINT32_T:
			m_Value.sint32 = rhs.m_Value.sint32;
			break;
		case SINT64_T:
			m_Value.sint64 = rhs.m_Value.sint64;
			break;
		case REAL32_T:
			m_Value.real32 = rhs.m_Value.real32;
			break;
		case STR_T:
			m_Str = rhs.m_Str;
			break;
		case UINT8_LIST_T:
			m_UInt8List = rhs.m_UInt8List;
			break;
		case UINT16_LIST_T:
			m_UInt16List = rhs.m_UInt16List;
			break;
		case UINT32_LIST_T:
			m_UInt32List = rhs.m_UInt32List;
			break;
		case UINT64_LIST_T:
			m_UInt64List = rhs.m_UInt64List;
			break;
		case STR_LIST_T:
			m_StrList = rhs.m_StrList;
			break;
		case BOOLEAN_LIST_T:
			m_BooleanList = rhs.m_BooleanList;
			break;
		case ENUM16_T:
			m_Str = rhs.m_Str;
			m_Value.uint16 = rhs.m_Value.uint16;
			break;
		case ENUM_T:
			m_Str = rhs.m_Str;
			m_Value.uint32 = rhs.m_Value.uint32;
			break;
	}
	return *this;
}

/*
 * Used by the CLI to display data to the screen and XML
 */
std::string wbem::framework::Attribute::asStr() const
{
	return asStr("", "", ", ");
}
std::string wbem::framework::Attribute::asStr(std::string prefix, std::string suffix,
					std::string sep) const
{
	std::stringstream result;
	COMMON_DATETIME_STR datetime;
	result << prefix;
	switch (m_Type)
	{
		case ENUM16_T:
		case ENUM_T:
			result << m_Str;
			break;
		case STR_T:
			result << m_Str;
			break;
		case UINT8_T:
			result << (unsigned int)m_Value.uint8;
			break;
		case UINT16_T:
			result << (unsigned int)m_Value.uint16;
			break;
		case UINT32_T:
			result << (unsigned int)m_Value.uint32;
			break;
		case UINT64_T:
			result << (unsigned long long)m_Value.uint64;
			break;
		case SINT8_T:
			result << (SINT16)(m_Value.sint8 & 0xFF);
			break;
		case SINT16_T:
			result << (short)m_Value.sint16;
			break;
		case SINT32_T:
			result << (int)m_Value.sint32;
			break;
		case SINT64_T:
			result << (long long)m_Value.sint64;
			break;
		case REAL32_T:
			result << m_Value.real32;
			break;
		case BOOLEAN_T:
			result << (m_Value.boolean ? "true" : "false");
			break;
		case UINT8_LIST_T:
			for (unsigned int i = 0; i < m_UInt8List.size(); i++)
			{
				if (i > 0)
				{
					result << suffix << sep << prefix;
				}
				result << (unsigned int)m_UInt8List[i];
			}
			break;
		case UINT16_LIST_T:
			for (unsigned int i = 0; i < m_UInt16List.size(); i++)
			{
				if (i > 0)
				{
					result << suffix << sep << prefix;
				}
				result << (unsigned int)m_UInt16List[i];
			}
			break;
		case UINT32_LIST_T:
			for (unsigned int i = 0; i < m_UInt32List.size(); i++)
			{
				if (i > 0)
				{
					result << suffix << sep << prefix;
				}
				result << (unsigned int)m_UInt32List[i];
			}
			break;
		case UINT64_LIST_T:
			for (unsigned int i = 0; i < m_UInt64List.size(); i++)
			{
				if (i > 0)
				{
					result << suffix << sep << prefix;
				}
				result << (unsigned long long)m_UInt64List[i];
			}
			break;
		case STR_LIST_T:
			for (unsigned int i = 0; i < m_StrList.size(); i++)
			{
				if (i > 0)
				{
					result << suffix << sep << prefix;
				}
				result << m_StrList[i];
			}
			break;
		case BOOLEAN_LIST_T:
			for (unsigned int i = 0; i < m_BooleanList.size(); i++)
			{
				if (i > 0)
				{
					result << suffix << sep << prefix;
				}
				result << (m_BooleanList[i] ? "true" : "false");
			}
			break;
		case DATETIME_T:
			convert_seconds_to_datetime(m_Value.uint64, datetime);
			result << datetime;
			break;
		case DATETIME_INTERVAL_T:
			convert_seconds_to_datetime_interval(m_Value.uint64, datetime);
			result << datetime;
			break;
		default:
			COMMON_LOG_ERROR_F("Invalid attribute type %d", m_Type);
			break;
	}
	result << suffix;
	return result.str();
}

enum wbem::framework::DataType wbem::framework::Attribute::getType() const
{
	return m_Type;
}

/*
 * Helper function to compare lists of values
 */
template <typename TYPE>
bool wbem::framework::Attribute::listEqual(const TYPE list1, const TYPE list2) const
{
	bool result = true;
	if (list1.size() != list2.size())
	{
		result = false;
	}
	else
	{
		typename TYPE::const_iterator iter1  = list1.begin();
		typename TYPE::const_iterator iter2  = list2.begin();
		for (; iter1 != list1.end() && iter2 != list2.end(); iter1++, iter2++)
		{
			if (*iter1 != *iter2)
			{
				result = false;
				break;
			}
		}
	}
	return result;
}


/*
 * Find out if the attribute is an array type
 */
bool wbem::framework::Attribute::isArray() const
{
	bool result = false;
	switch (m_Type)
	{
	case UINT8_LIST_T:
	case UINT16_LIST_T:
	case UINT32_LIST_T:
	case UINT64_LIST_T:
	case STR_LIST_T:
	case BOOLEAN_LIST_T:
		result = true;
		break;
	default:
		result = false;
		break;
	}

	return result;
}

/*
 * Find out if the attribute is a native numeric type
 */
bool wbem::framework::Attribute::isNumeric() const
{
	bool result = false;
	switch (m_Type)
	{
	case UINT8_T:
	case SINT8_T:
	case UINT16_T:
	case SINT16_T:
	case UINT32_T:
	case SINT32_T:
	case UINT64_T:
	case SINT64_T:
	case REAL32_T:
	case ENUM_T:
	case ENUM16_T:
		result = true;
		break;
	default:
		result = false;
		break;
	}

	return result;
}

/*
 * Inequality operator.
 */
bool wbem::framework::Attribute::operator!=(const Attribute& rhs) const
{
	return (*this == rhs ? false : true);
}

/*
 * Equality operator.
 */
bool wbem::framework::Attribute::operator==(const Attribute& rhs) const
{
	bool result = true;

	enum DataType rhsType = rhs.getType();
	enum DataType lhsType = this->m_Type;
	bool rhsIsKey = rhs.isKey();
	bool lhsIsKey = this->m_IsKey;

	if (!(typesMatch(lhsType, rhsType)) || (rhsIsKey != lhsIsKey))
	{
		result = false;
	}
	else
	{
		switch(lhsType)
		{
			case BOOLEAN_T:
				result = (this->m_Value.boolean == rhs.boolValue());
				break;
			case ENUM_T:
			case UINT8_T:
				result = (this->m_Value.uint8 == (UINT8)rhs.uintValue());
				break;
			case ENUM16_T:
			case UINT16_T:
				result = (this->m_Value.uint16 == (UINT16)rhs.uintValue());
				break;
			case UINT32_T:
				result = (this->m_Value.uint32 == (UINT32)rhs.uintValue());
				break;
			case DATETIME_T:
			case DATETIME_INTERVAL_T:
			case UINT64_T:
				result = (this->m_Value.uint64 == rhs.uint64Value());
				break;
			case SINT8_T:
				result = (this->m_Value.sint8 == (SINT8)rhs.uintValue());
				break;
			case SINT16_T:
				result = (this->m_Value.sint16 == (SINT16)rhs.uintValue());
				break;
			case SINT32_T:
				result = (this->m_Value.sint32 == (SINT32)rhs.uintValue());
				break;
			case SINT64_T:
				result = (this->m_Value.sint64 == rhs.sint64Value());
				break;
			case STR_T:
				result = (this->m_Str == rhs.stringValue());
				break;
			case UINT8_LIST_T:
				result = listEqual(this->m_UInt8List, rhs.uint8ListValue());
				break;
			case UINT16_LIST_T:
				result = listEqual(this->m_UInt16List, rhs.uint16ListValue());
				break;
			case UINT32_LIST_T:
				result = listEqual(this->m_UInt32List, rhs.uint32ListValue());
				break;
			case UINT64_LIST_T:
				result = listEqual(this->m_UInt64List, rhs.uint64ListValue());
				break;
			case STR_LIST_T:
				result = listEqual(this->m_StrList, rhs.strListValue());
				break;
			case BOOLEAN_LIST_T:
				result = listEqual(this->m_BooleanList, rhs.booleanListValue());
				break;
			default:
				result = false;
				break;
		}
	}
	return result;
}

bool wbem::framework::Attribute::typesMatch(enum DataType lhs, enum DataType rhs)
{
	bool match = false;

	if (lhs == rhs)
	{
		match = true;
	}
	else
	{
		if ((lhs == ENUM_T) || (lhs == UINT8_T))
		{
			match = ((rhs == ENUM_T) || (rhs == UINT8_T));
		}
		else if ((lhs == ENUM16_T) || (lhs == UINT16_T))
		{
			match = ((rhs == ENUM16_T) || (rhs == UINT16_T));
		}
	}
	return match;
}

void wbem::framework::Attribute::trimStr(STR &str)
{
    std::string::iterator strStart = str.begin();
    std::string::iterator strEnd = str.end();

    while (std::isspace(*strStart))
    {
        ++strStart;
    }

    if (strStart != strEnd)
    {
        while (std::isspace(*(strEnd - 1)))
        {
            --strEnd;
        }
    }

    str.assign(strStart, strEnd);
}

/*
 * This is as a result of pegasus and sfcb CIMOM behavior where the normalization of the
 * parsed XML string data (XML specification requirement) leads to a false mismatch between
 * the Intel attribute and the modified cmpi attribute passed in the Generic_Modify
 * function by the CIMOM
 */
void wbem::framework::Attribute::normalize()
{
	if (this->m_Type == STR_T)
	{
		trimStr(this->m_Str);
	}
	if (this->m_Type == STR_LIST_T)
	{
		for (STR_LIST::iterator i = this->m_StrList.begin();
				i != this->m_StrList.end(); i++)
		{
			trimStr(*i);
		}
	}
}


bool wbem::framework::Attribute::isEmbedded()
{
	return m_IsEmbedded && m_Type == STR_T;
}

void wbem::framework::Attribute::setIsEmbedded(bool value)
{

	m_IsEmbedded = value;
}

bool wbem::framework::Attribute::isAssociationClassInstance()
{
	return m_IsAssociationClassInstance && m_Type == STR_T;
}

void wbem::framework::Attribute::setIsAssociationClassInstance(bool value)
{
	m_IsAssociationClassInstance = value;
}

void wbem::framework::Attribute::setIsKey(bool isKey)
{
	m_IsKey = isKey;
}
