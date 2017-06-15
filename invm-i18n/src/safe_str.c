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

#include <stddef.h>
#include "safe_str.h"
#include <stdlib.h>

/*
 * safe copy of a string.
 */
char *safe_strncpy(char *dst, size_t dst_size, const char *src, size_t src_size)
{
	if (dst && src && (dst_size != 0))
	{
		int charsafe_to_copy = dst_size - 1; // leave 1 for null terminator
		charsafe_to_copy = (src_size < charsafe_to_copy) ? src_size : charsafe_to_copy;

		int i = 0;
		while ((i < charsafe_to_copy) && ((dst[i] = src[i]) != '\0'))
		{
			i++;
		}

		dst[i] = '\0';
	}

	return dst;
}

/*
 * safe concatenation of a string.
 */
char *safe_strncat(char *dst, size_t dst_size, const char *src, size_t src_size)
{
	if (dst && src && (dst_size != 0) && (src_size != 0))

	{
		size_t dst_i = safe_strnlen(dst, dst_size); // current length
		int free_dst_chars = ((int)dst_size - 1) - (int)dst_i; // leave room for null terminator
		if (free_dst_chars > 0)
		{
			// handle possible size difference between src and dst
			int charsafe_to_copy = ((int)src_size < free_dst_chars) ? (int)src_size : free_dst_chars;
			int src_i = 0;
			while ((src_i < charsafe_to_copy) && ((dst[dst_i] = src[src_i]) != '\0'))
			{
				dst_i++;
				src_i++;
			}
			dst[dst_i] = '\0';
		}
	}

	return dst;
}

/*
 * searches a string for the null terminator up to the max_len passed
 */
size_t safe_strnlen(const char *str, size_t max_len)
{
	size_t i = 0;
	if (str)
	{
		while ((i < max_len) && (str[i] != '\0'))
		{
			i++;
		}
	}

	return i;
}

/*
 * searches a string str1 for the first occurrence of str2
 */
const char *safe_strnstrn(const char *str1, size_t str1len, char *str2, size_t str2len)
{
	int done = 0;
	const char *ret_ptr = NULL;

	if (str1 && str2 && *str1 && *str2 && str1len >= str2len)
	{
		char c = *str2;
		while (safe_strncmp(str1, str2, str2len) != 0)
		{
			str1++;
			str1len--;
			while (c != *str1)
			{
				str1++;
				str1len--;
				if (str1len < str2len)
				{
					done = 1;
					break;
				}
			}

			if (done)
			{
				break;
			}
		}

		if (!done)
		{
			ret_ptr = str1;
		}
	}
	return ret_ptr;
}

/*
 * Safe locate the first occurrence of a character with a string
 */
char *safe_strchr(char *str, char ch, int max_len)
{
	char *last_char = NULL;

	if (str)
	{
		int i = 0;
		while ((i < max_len) && (str[i] != '\0'))
		{
			if (str[i] == ch)
			{
				last_char = &str[i];
				break;
			}
			i++;
		}
	}

	return last_char;
}

/*
 * safe case-sensitive string comparison
 */
int safe_strncmp(const char *const str1, const char *const str2, size_t maxlen)
{
	int ret = -1;

	if (str1 && str2 && (maxlen != 0))
	{
		ret = 1;
		size_t idx = 0;

		size_t str1size = safe_strnlen(str1, maxlen);
		size_t str2size = safe_strnlen(str2, maxlen);

		size_t size = str1size > str2size ? str1size : str2size;

		while ((idx < size) && (str1[idx] == str2[idx]))
		{
			idx++;
		}

		if (idx == size)
		{
			// return zero if the entire string matches up to the input size
			ret = 0;
		}
		else
		{
			if (str1[idx] < str2[idx])
			{
				ret = -1;
			}
			else
			{
				ret = 1;
			}
		}
	}

	return ret;
}

/*
 * Safe string duplication
 */
char *safe_strndup(const char *str, size_t maxlen)
{
	char *new_str = NULL;
	if (str)
	{
		size_t len = safe_strnlen(str, maxlen);

		// Add a spot for the null terminator
		len++;

		new_str = (char *) malloc(len);

		if (new_str != NULL)
		{
			safe_strncpy(new_str, len, str, len);
		}
	}
	return new_str;
}

