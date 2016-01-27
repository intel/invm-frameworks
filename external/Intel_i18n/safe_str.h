/*
 * Copyright (c) 2015, Intel Corporation
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
/*!
 * Safe copy of a @b char array
 * @param[out] dst
 * 		Pointer to the destination buffer to copy characters into
 * @param[in] dst_size
 * 		Size of the destination buffer
 * @param[in] src
 * 		Pointer to the source array to copy characters from
 * @param[in] src_size
 * 		Size of the source array
 * @return
 * 		The number of characters that were copied including null terminator.
 */
char *safe_strncpy(char *dst, size_t dst_size, const char *src, size_t src_size);

/*!
 * Safe concatenation of strings of different sizes.
 * This function will attempt to concatenate as much of @c src to @c dst as possible,
 * while still null-terminating the resulting @c dst string.
 * @remarks
 * 		Concatenation begins at, and replaces, the first null-terminator in @c dst. This
 * 		means that @c dst must be null-terminated, otherwise no concatenation can occur.
 * @param[out] dst
 * 		Pointer to the destination buffer to copy characters into
 * @param[in] dst_size
 * 		Size of the destination buffer
 * @param[in] src
 * 		Pointer to the source array to copy characters from
 * @param[in] src_size
 * 		Size of the source array
 * @return
 * 		Pointer to the @c dst.
 */
char *safe_strncat(char *dst, size_t dst_size, const char *src, size_t src_size);

/*!
 * Safely determines the length of a string up to @c max_len
 * @remarks
 * 		Stops counting characters when a null terminator or @c max_len is reached
 * @param[in] str
 * 		The string whose length to determine
 * @param[in] max_len
 * 		The maximum number of characters to consider
 * @return
 * 		A value between 0 and @c max_len, denoting the size of the string,
 * 		not including the null terminator if found.
 */
size_t safe_strnlen(const char *str, size_t max_len);

/*!
 *  Safely find the first occurence of str2 within str1.
 *  @remarks
 *  	The matching process does not include terminating null-characters, but it stops there.
 *  @param[in] str1
 *  	The string to be searched.
 *  @param[in] str1len
 *  	The maximum length of str1
 *  @param[in] str2
 *  	The string searched for
 *  @param[in] str2len
 *  	The maximum length of str2
 *  @return
 *  	A pointer to the first occurrence of the entire sequence of characters in str2, or a
 *  	null-pointer if the sequence is not present in str1
 */
const char *safe_strnstrn(const char *str1, size_t max1len, char *str2, size_t max2len);

/*!
 * Safe find of the last instance of a given @b char within a string.
 * @param[in] str
 * 		The string to be searched for @b char @c ch
 * @param[in] ch
 * 		The @b char which we are searching for the last instance of.
 * @param[in] max_len
 * 		The maximum number of characters to search for @c ch within.
 * @return
 * 		A pointer to the last instance of @c ch within @c str @n
 * 		else, @b NULL if an instance of @c ch is not found.
 */
char *safe_strchr(char *str, char ch, int max_len);

/*!
 * Safe case-sensitive string comparison
 * @remarks
 * 		This function compares the number of characters defined by @c size
 * 		and is not ended when a null terminator is encountered.
 * @param str1
 * 		Pointer to the first string to be compared
 * @param str2
 * 		Pointer to the second string to be compared
 * @param size
 * 		The number of characters to be compared
 * @return
 * 		A value indicating the character number (index+1) where the two string differ @n
 * 		0 if equivalent @n
 * 		-1 if either string pointer is @b NULL or @c size is 0
 */
int safe_strncmp(const char *const str1, const char *const str2, size_t size);

/*!
 * Safe case-sensitive string duplication
 * @remarks
 * 		This function returns a pointer to s string that is a duplicate of the
 * 		givne string.
 * @param str
 * 		Pointer to the given string
 * @param maxlen
 * 		The maximum number of characters is the given string
 * @return
 * 		The function returns a pointer to the duplicate string or NULL if
 * 		insufficient memory was available.
 */
char *safe_strndup(const char *str, size_t maxlen);

#ifdef __cplusplus
}
#endif
#endif /* _SAFE_STR_H_ */
