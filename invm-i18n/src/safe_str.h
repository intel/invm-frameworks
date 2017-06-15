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

/*
 * Safe string manipulation utility functions
 */

#ifndef	_SAFE_STR_H_
#define	_SAFE_STR_H_

#ifdef __cplusplus
extern "C"
{
#endif
/*
 * Safe copy of a char array
 */
char *safe_strncpy(char *dst, size_t dst_size, const char *src, size_t src_size);

/*
 * Safe concatenation of strings of different sizes.
 */
char *safe_strncat(char *dst, size_t dst_size, const char *src, size_t src_size);

/*
 * Safely determines the length of a string up to max_len
 */
size_t safe_strnlen(const char *str, size_t max_len);

/*
 *  Safely find the first occurence of str2 within str1.
 */
const char *safe_strnstrn(const char *str1, size_t max1len, char *str2, size_t max2len);

/*
 * Safe find of the last instance of a given char within a string.
 */
char *safe_strchr(char *str, char ch, int max_len);

/*
 * Safe case-sensitive string comparison
 */
int safe_strncmp(const char *const str1, const char *const str2, size_t size);

/*
 * Safe case-sensitive string duplication
 */
char *safe_strndup(const char *str, size_t maxlen);

#ifdef __cplusplus
}
#endif
#endif /* _SAFE_STR_H_ */
