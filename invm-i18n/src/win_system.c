/*
 * Copyright (c) 2015, 2016 Intel Corporation
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

#include <windows.h>
#include "system.h"
#include <stdio.h>
#include "safe_str.h"
#include <io.h>
#include "libIntel_i18n.h"
#include "libIntel_i18n_local.h"
#include <errno.h>

void win_close_lib();

DWORD getpagesize();

DWORD getpagesize()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return system_info.dwPageSize;
}

/*
 * Generic function to memory map a file.
 */
void* my_mmap(void* start, size_t length, int fd, size_t offset)
{
	HANDLE handle;

	if (offset % getpagesize() != 0)
	{
		exit(1);
	}

	handle = CreateFileMapping((HANDLE)_get_osfhandle(fd), NULL,
			PAGE_READONLY, 0, 0, NULL);

	if (handle != NULL)
	{
		start = MapViewOfFile(handle, FILE_MAP_COPY, 0, offset, length);
		CloseHandle(handle);
	}

	return start;
}

/*
 * Generic function to unmap a file from memory.
 */
int my_munmap(void *start, size_t length)
{
	UnmapViewOfFile(start);
	return 0;
}

/**
 * Generic function to retrieve the locale.  I'm simplifying this to
 * ignore the "modifier" and return "language_territory.codeset"
 */
int get_locale(I18N_STRING locale)
{
	int  error = I18N_SUCCESS;
	WCHAR localeBuffer[I18N_STRING_LEN];

	if (GetUserDefaultLocaleName(localeBuffer, I18N_STRING_LEN)  == 0)
	{
		locale[0] = '\0';
		error = I18N_FAIL;
	}
	else if (wcstombs(locale, localeBuffer, I18N_STRING_LEN) == (size_t)-1)
	{
		locale[0] = '\0';
		error = I18N_FAIL;
	}
	return error;
}

char *get_separator()
{
	return "\\";
}

int get_catalog_file(struct domainbinding *binding, I18N_STRING catalog_file)
{
	int rc = I18N_SUCCESS;
	I18N_STRING locale;

	get_locale(locale);
	char *separator = get_separator();

	safe_strncpy(catalog_file, I18N_STRING_LEN, binding->path, I18N_STRING_LEN);

	safe_strncat(catalog_file, I18N_STRING_LEN, separator, I18N_STRING_LEN);

	safe_strncat(catalog_file, I18N_STRING_LEN, "lang", I18N_STRING_LEN);

	safe_strncat(catalog_file, I18N_STRING_LEN, separator, I18N_STRING_LEN);

	safe_strncat(catalog_file, I18N_STRING_LEN, locale, safe_strnlen(locale, I18N_STRING_LEN));

	safe_strncat(catalog_file, I18N_STRING_LEN, ".mo", I18N_STRING_LEN);

	return rc;
}

/*
 * Windows API call that deals with DLL administrative tasks.
 */
BOOL __stdcall DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	int rc = I18N_SUCCESS;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			win_close_lib();
			break;
		case DLL_THREAD_DETACH:
			break;
	}

	if (rc >= 0)
		return TRUE;
	else
		return FALSE;
}

void win_close_lib()
{
	while (__bindings)
	{
		struct domainbinding *tmp = __bindings;
		__bindings = __bindings->next;
		if (tmp->mohandle.mo.mo_otable)
		{
			free(tmp->mohandle.mo.mo_otable);
		}
		if (tmp->mohandle.mo.mo_ttable)
		{
			free(tmp->mohandle.mo.mo_ttable);
		}
		if (tmp->mohandle.mo.mo_charset)
		{
			free(tmp->mohandle.mo.mo_charset);
		}
		free(tmp);
	}
}
