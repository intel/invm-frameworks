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

#ifndef __WINDOWS__
#include <features.h>
#endif

#ifndef _LIBINTEL_I18N_H_
#define _LIBINTEL_I18N_H_

#ifndef LC_MESSAGES
#define LC_MESSAGES 1729
#endif

#define I18N_STRING_LEN 4096
typedef char I18N_STRING[I18N_STRING_LEN];

#ifdef __cplusplus
extern "C" {
#endif

#define I18N_SUCCESS 0
#define I18N_FAIL -1
#define I18N_NOMEMORY -2

#define gettext Intel_gettext
#define textdomain Intel_textdomain
#define bindtextdomain Intel_bindtextdomain

#ifdef __WINDOWS__
extern char *gettext(const char *);
extern char *textdomain(const I18N_STRING);
extern char *bindtextdomain(const char *, const I18N_STRING);
#else
extern char *gettext(const char *) __THROW __attribute_format_arg__ (1);
extern char *textdomain(const I18N_STRING) __THROW __attribute_format_arg__ (1);
extern char *bindtextdomain(const char *, const I18N_STRING) __THROW __attribute_format_arg__ (1);
#endif

#ifdef __cplusplus
}
#endif


#endif /*_LIBINTEL_I18N_H__*/
