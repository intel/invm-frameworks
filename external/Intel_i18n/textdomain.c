/*
 * Copyright (c) 2000, 2001 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Keeps track of some basic locale & program info in global variables.
 */


#include <sys/types.h>
#include <sys/param.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libIntel_i18n.h"
#include "libIntel_i18n_local.h"
#include "safe_str.h"

struct domainbinding *__bindings = NULL;
I18N_STRING __current_domainname = "";

struct domainbinding *find_domain_binding(const I18N_STRING domainname);
struct domainbinding *add_domain_binding(const I18N_STRING domainname, const I18N_STRING dirname);
struct domainbinding *set_domain_binding(const I18N_STRING domainname, const I18N_STRING dirname);

/*
 * Remember the domain we are working with
 */
char *textdomain(const I18N_STRING domainname)
{
	// NULL pointer or empty string gives the current setting
	if (!domainname)
	{
		return __current_domainname;
	}
	else
	{
		safe_strncpy(__current_domainname, I18N_STRING_LEN, domainname, I18N_STRING_LEN);
	}

	return __current_domainname;
}


/*
 * This function is used to associate a file system path where messages are kept with
 * a given program "domain" name.
 */
char *bindtextdomain(const I18N_STRING domainname, const I18N_STRING dirname)
{
	struct domainbinding *p = NULL;
	char *returnPath = NULL;

	// they need to pass us a valid domain name for the call to be valid
	if (domainname && *domainname)
	{
		if (!dirname || !*dirname)
		{
			p = find_domain_binding(domainname);
			if (p)
			{
				returnPath = p->path;
			}
		}
		else
		{
			p = set_domain_binding(domainname, dirname);
			if (p)
			{
				returnPath = p->path;
			}
		}
	}

	return returnPath;
}

struct domainbinding *set_domain_binding(const I18N_STRING domainname, const I18N_STRING dirname)
{
	struct domainbinding *p = find_domain_binding(domainname);

	if (!p)
	{
		p = add_domain_binding(domainname, dirname);
	}

	if (p)
	{
		safe_strncpy(p->path, I18N_STRING_LEN, dirname, I18N_STRING_LEN);
		safe_strncpy(p->domainname, I18N_STRING_LEN, domainname, I18N_STRING_LEN);
		p->mohandle.mo.mo_magic = 0; /* invalidate current mapping */
	}

	return p;
}

struct domainbinding *add_domain_binding(const I18N_STRING domainname, const I18N_STRING dirname)
{

	struct domainbinding *p = (struct domainbinding *)malloc(sizeof (struct domainbinding));
	if (!p)
	{
		errno = ENOMEM;
	}
	else
	{
		memset(p, 0, sizeof (struct domainbinding));
		p->next = __bindings;
		__bindings = p;
	}

	return p;
}

struct domainbinding *find_domain_binding(const I18N_STRING domainname)
{
	struct domainbinding *p = NULL;

	for (p = __bindings; p; p = p->next)
	{
		if (strcmp(p->domainname, domainname) == 0)
		{
			break;
		}
	}

	return p;
}
