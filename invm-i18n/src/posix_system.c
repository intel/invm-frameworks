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

#include <stdlib.h>
#include <string.h>
#include "libIntel_i18n.h"
#include "libIntel_i18n_local.h"
#include "safe_str.h"
#include <sys/mman.h>
#include <errno.h>

#define messages "LC_MESSAGES"

/**
 * Generic function to memory map a file.
 */
void* my_mmap(void* start, size_t length, int fd, size_t offset)
{
	return mmap(start, length, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, offset);
}

/**
 * Generic function to unmap a file from memory.
 */
int my_munmap(void *start, size_t length) 
{ 
	return munmap(start, length);
} 

/**
 * On posix the locale can be retrieved from environment variables.
 */
int get_locale(I18N_STRING locale)
{
	int rc = I18N_SUCCESS;
	char *lang;

	lang = getenv("LANG");

	// lang not set problem
	if (lang)
	{
		safe_strncpy(locale, I18N_STRING_LEN, strtok(lang, "."), I18N_STRING_LEN);	
	}
	else
	{
		rc = I18N_FAIL;
	}
	return rc;
}

/*
 * return the file path separator for this type of OS
 */
void get_separator(I18N_STRING separator)
{
	safe_strncpy(separator, I18N_STRING_LEN, "/", I18N_STRING_LEN);
}

int get_catalog_file(struct domainbinding *binding, I18N_STRING catalog_file)
{
	int rc = I18N_SUCCESS;
	I18N_STRING locale;
	if ((rc = get_locale(locale)) == I18N_SUCCESS)
	{
		I18N_STRING separator;
		get_separator(separator);
		char *domain = textdomain(NULL);

		safe_strncpy(catalog_file, I18N_STRING_LEN, binding->path, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, separator, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, locale, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, separator, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, messages, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, separator, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, domain, I18N_STRING_LEN);

		safe_strncat(catalog_file, I18N_STRING_LEN, ".mo", I18N_STRING_LEN);

	}
	return rc;
}

void __attribute__((destructor)) lib_unload();

void lib_unload()
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
