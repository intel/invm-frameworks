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
 * This file contains utility methods used by the WMI Provider
 */

#include <objbase.h>
#include <strsafe.h>
#include <string>
#include <comutil.h>
#include <windows.h>
#include <WbemCli.h>
#include "wmiUtilities.h"

#include <logger/logging.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;



/*
 * ************************************************************************************************
 * comutil.h functions
 *
 * ConvertStringToBSTR and ConvertBSTRToString are declared in comutil.h and defined in
 * comsuppw.lib which isn't available for MinGw. These implementations are adapted from
 * http://www.codeproject.com/Articles/1969/BUG-in-_com_util-ConvertStringToBSTR-and-_com_util.
 * From this post, it sounds like it's better not to use the implementation from comsuppw.lib
 * anyway.
 * ************************************************************************************************
 */
/*
 * Convert char * to BSTR
 */
BSTR  _com_util::ConvertStringToBSTR(const char* src)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	BSTR result(NULL);

	if (src == NULL)
	{
		COMMON_LOG_ERROR("Parameter src cannot be NULL");
	}
	else
	{

		DWORD result_len;

		if ((result_len = ::MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0)) > 0) // get length
		{
			if ((result = ::SysAllocStringLen(NULL, result_len)) != NULL)
			{
				if(!::MultiByteToWideChar(CP_ACP, 0, src, -1, result, result_len))
				{
					DWORD error = ::GetLastError();
					if (ERROR_INSUFFICIENT_BUFFER != error)
					{
						::SysFreeString(result); //must clean up
						result = NULL;
					}
					COMMON_LOG_ERROR_F("Error %d converting string %s to BSTR", error, src);
				}
			}
			else
			{
				COMMON_LOG_ERROR("Error allocating the string length");
			}
		}
		else
		{
			COMMON_LOG_ERROR("Error getting the string length");
		}
	}

	return result;
};


/*
 * Convert BSTR to char *
 */
char* _com_util::ConvertBSTRToString(BSTR src)
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	char *result = NULL;

	if (src == NULL)
	{
		COMMON_LOG_ERROR("Parameter src cannot be NULL");
	}
	else
	{
		DWORD result_len;
		DWORD src_len = ::SysStringLen(src);


		if((result_len = ::WideCharToMultiByte(CP_ACP, 0, src, src_len + 1, NULL, 0, 0, 0)))
		{
			result = new char[result_len];
			if(result)
			{
				result[result_len - 1]  = '\0';

				if(!::WideCharToMultiByte(CP_ACP, 0, src, src_len + 1, result, result_len, 0, 0))
				{
					delete []result; //clean up if failed;
					result = NULL;

					COMMON_LOG_ERROR("Error converting BSTR to String");
				}
			}
			else
			{
				COMMON_LOG_ERROR("Error allocating the string");
			}
		}
		else
		{
			COMMON_LOG_ERROR("Error getting the string length");
		}
	}

	return result;
};


/*
* Impersonate
* 
* Purpose: Impersonate the client
*/
HRESULT STDMETHODCALLTYPE Impersonate()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	HRESULT hr = CoImpersonateClient();
	if (SUCCEEDED(hr))
	{
		// Check to see if call is at lower than
		// RPC_C_IMP_LEVEL_IMPERSONATE level. If that is the case,
		// the provider will not be able to impersonate
		// the client to access the protected resources.

		DWORD t_CurrentImpersonationLevel = GetCurrentImpersonationLevel();
		if (t_CurrentImpersonationLevel < RPC_C_IMP_LEVEL_IMPERSONATE)
		{
			//	Revert before you perform any operations
			CoRevertToSelf();
			hr = WBEM_E_ACCESS_DENIED;
		}
	}
	return hr;
}

/*
*
* GetCurrentImpersonationLevel
* Description: Get COM impersonation level of caller.
*
*/
DWORD GetCurrentImpersonationLevel()
{
	LogEnterExit logging(__FILE__, __FUNCTION__, __LINE__);
	DWORD t_ImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS ;
	HANDLE t_ThreadToken = NULL ;
	BOOL t_Status = OpenThreadToken (
		GetCurrentThread() ,
		TOKEN_QUERY,
		TRUE,
		&t_ThreadToken);

	if ( t_Status )
	{
		SECURITY_IMPERSONATION_LEVEL t_Level = SecurityAnonymous ;
		DWORD t_Returned = 0 ;

		t_Status = GetTokenInformation (
			t_ThreadToken ,
			TokenImpersonationLevel ,
			& t_Level ,
			sizeof ( SECURITY_IMPERSONATION_LEVEL ) ,
			& t_Returned);

		CloseHandle ( t_ThreadToken ) ;

		if ( t_Status == FALSE )
		{
			t_ImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS ;
		}
		else
		{
			switch ( t_Level )
			{
			case SecurityAnonymous:
				{
					t_ImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS;
				}
				break;

			case SecurityIdentification:
				{
					t_ImpersonationLevel = RPC_C_IMP_LEVEL_IDENTIFY;
				}
				break ;

			case SecurityImpersonation:
				{
					t_ImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
				}
				break ;

			case SecurityDelegation:
				{
					t_ImpersonationLevel = RPC_C_IMP_LEVEL_DELEGATE ;
				}
				break ;

			default:
				{
					t_ImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS ;
				}
				break ;
			}
		}
	}
	else
	{
		ULONG t_LastError = GetLastError();

		if (t_LastError == ERROR_NO_IMPERSONATION_TOKEN || t_LastError == ERROR_NO_TOKEN)
		{
			t_ImpersonationLevel = RPC_C_IMP_LEVEL_DELEGATE ;
		}
		else
		{
			if (t_LastError == ERROR_CANT_OPEN_ANONYMOUS)
			{
				t_ImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS;
			}
			else
			{
				t_ImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS;
			}
		}
	}

	return t_ImpersonationLevel;
}

/*
* str_to_wstr, wstr_to_str
* 
* Purpose: utility methods to convert between std::wstring and std::strings.  Useful when working
* with Windows OS
*/
std::wstring convert::str_to_wstr( const std::string& str )
{
	size_t size = str.length();
	std::wstring wstr(size + 1, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, &wstr[0], size);
	return wstr;
}

std::string convert::wstr_to_str( const std::wstring& wstr )
{
	size_t size = wstr.length();
	std::string str(size + 1, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), size, &str[0], size, NULL, NULL );
	return str;
}
