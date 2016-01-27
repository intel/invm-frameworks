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
 * This file contains a non-GNU implementation of gettext with more attractive
 * license terms.  It came from an old version of FreeBSD.  I've simplified it
 * significantly, getting rid of things there to handle obscure cases we won't
 * encounter in our code.  Also made changes so it works with MinGW.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "system.h"

#include "safe_str.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libIntel_i18n.h"
#include <locale.h>
#include "libIntel_i18n_local.h"

static const char *lookup_catalog_file(I18N_STRING, struct domainbinding *);
static u_int32_t flip(u_int32_t, u_int32_t);
static int validate(void *, struct mohandle *);
static int mapit(const char *, struct domainbinding *);
static int unmapit(struct domainbinding *);
static const char *lookup_bsearch(const char *, struct domainbinding *);
static const char *lookup(const char *, struct domainbinding *);
void get_lang_env(I18N_STRING);

/**
 * find the translated file
 */
static const char *lookup_catalog_file(I18N_STRING path, struct domainbinding *db)
{
	struct stat st;

	// make sure the file is there
	if (stat(path, &st) < 0)
	{
		return NULL;
	}

	if (mapit(path, db) == 0)
	{
		return path;
	}

	return NULL;
}

/*
 * Apparently works --didn't have to dig into this so don't know exactly what
 * this is doing.
 */
static u_int32_t
flip(u_int32_t v, u_int32_t magic)
{

	if (magic == MO_MAGIC)
	{
		return v;
	}
	else if (magic == MO_MAGIC_SWAPPED)
	{
		v = ((v >> 24) & 0xff) | ((v >> 8) & 0xff00) |
				((v << 8) & 0xff0000) | ((v << 24) & 0xff000000);
		return v;
	}
	else
	{
		abort();
		/*NOTREACHED*/
		return I18N_FAIL;
	}
}

/*
 * Apparently works --didn't have to dig into this so don't know exactly what
 * this is doing.
 */
static int
validate(void *arg, struct mohandle *mohandle)
{
	char *p;

	p = (char *)arg;
	if (p < (char *)mohandle->addr ||
			p > (char *)mohandle->addr + mohandle->len)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
 * Didn't dig into this function, aside from memory mapping not sure what all else its
 * doing.
 */
int mapit(const I18N_STRING  path, struct domainbinding *db)
{
	int fd;
	struct stat st;
	char *base;
	u_int32_t magic, revision;
	struct moentry *otable, *ttable;
	struct moentry_h *p;
	struct mo *mo;
	size_t l;
	int i;
	const char *v;
	struct mohandle *mohandle = &db->mohandle;

	if (mohandle->addr && mohandle->addr != I18N_MMAP_FAILED &&
			mohandle->mo.mo_magic)
	{
		return 0;
	}

	unmapit(db);

	if (stat(path, &st) < 0)
	{
		goto fail;
	}

	if ((st.st_mode & S_IFMT) != S_IFREG || st.st_size > I18N_MMAP_MAX)
	{
		goto fail;
	}

	fd = open(path, O_RDONLY);

	if (fd < 0)
	{
		goto fail;
	}

	if (read(fd, &magic, sizeof (magic)) != sizeof (magic) ||
			(magic != MO_MAGIC && magic != MO_MAGIC_SWAPPED))
	{
		close(fd);
		goto fail;
	}

	if (read(fd, &revision, sizeof (revision)) != sizeof (revision) ||
			flip(revision, magic) != MO_REVISION)
	{
		close(fd);
		goto fail;
	}

	mohandle->addr = my_mmap(NULL, (size_t)st.st_size, fd, (off_t)0);
	if (!mohandle->addr || mohandle->addr == I18N_MMAP_FAILED)
	{
		close(fd);
		goto fail;
	}

	close(fd);
	mohandle->len = (size_t)st.st_size;

	base = mohandle->addr;
	mo = (struct mo *)mohandle->addr;

	/* flip endian.  do not flip magic number! */
	mohandle->mo.mo_magic = mo->mo_magic;
	mohandle->mo.mo_revision = flip(mo->mo_revision, magic);
	mohandle->mo.mo_nstring = flip(mo->mo_nstring, magic);

	/* validate otable/ttable */
	otable = (struct moentry *)(base + flip(mo->mo_otable, magic));
	ttable = (struct moentry *)(base + flip(mo->mo_ttable, magic));
	if (!validate(otable, mohandle) ||
			!validate(&otable[mohandle->mo.mo_nstring], mohandle))
	{
		unmapit(db);
		goto fail;
	}

	if (!validate(ttable, mohandle) ||
			!validate(&ttable[mohandle->mo.mo_nstring], mohandle))
	{
		unmapit(db);
		goto fail;
	}

	/* allocate [ot]table, and convert to normal pointer representation. */
	l = sizeof (struct moentry_h) * mohandle->mo.mo_nstring;
	mohandle->mo.mo_otable = (struct moentry_h *)malloc(l);
	if (!mohandle->mo.mo_otable)
	{
		unmapit(db);
		goto fail;
	}

	mohandle->mo.mo_ttable = (struct moentry_h *)malloc(l);
	if (!mohandle->mo.mo_ttable)
	{
		unmapit(db);
		goto fail;
	}

	p = mohandle->mo.mo_otable;
	for (i = 0; i < mohandle->mo.mo_nstring; i++)
	{
		p[i].len = flip(otable[i].len, magic);
		p[i].off = base + flip(otable[i].off, magic);

		if (!validate(p[i].off, mohandle) ||
				!validate(p[i].off + p[i].len + 1, mohandle))
		{
			unmapit(db);
			goto fail;
		}
	}

	p = mohandle->mo.mo_ttable;
	for (i = 0; i < mohandle->mo.mo_nstring; i++)
	{
		p[i].len = flip(ttable[i].len, magic);
		p[i].off = base + flip(ttable[i].off, magic);

		if (!validate(p[i].off, mohandle) ||
				!validate(p[i].off + p[i].len + 1, mohandle))
		{
			unmapit(db);
			goto fail;
		}
	}

	/* grab MIME-header and charset field */
	mohandle->mo.mo_header = lookup("", db);
	if (mohandle->mo.mo_header)
	{
		v = safe_strnstrn(mohandle->mo.mo_header, safe_strnlen(mohandle->mo.mo_header, I18N_STRING_LEN),
						"charset=", safe_strnlen("charset=", I18N_STRING_LEN));
	}
	else
	{
		v = NULL;
	}

	if (v)
	{
		mohandle->mo.mo_charset = safe_strndup(v + 8, I18N_STRING_LEN);
		if (!mohandle->mo.mo_charset)
		{
			goto fail;
		}

		char *w = safe_strchr(mohandle->mo.mo_charset, '\n', I18N_STRING_LEN);
		if (w)
		{
			*w = '\0';
		}
	}

	return 0;

	fail:
	return -1;
}

/*
 * Unmap the binary catalog file from memory.
 */
static int
unmapit(struct domainbinding * db)
{
	struct mohandle *mohandle = &db->mohandle;

	/* unmap if there's already mapped region */
	if (mohandle->addr && mohandle->addr != I18N_MMAP_FAILED)
	{
		my_munmap(mohandle->addr, mohandle->len);
	}

	mohandle->addr = NULL;
	if (mohandle->mo.mo_otable)
	{
		free(mohandle->mo.mo_otable);
	}

	if (mohandle->mo.mo_ttable)
	{
		free(mohandle->mo.mo_ttable);
	}

	if (mohandle->mo.mo_charset)
	{
		free(mohandle->mo.mo_charset);
	}

	memset(&mohandle->mo, 0, sizeof (mohandle->mo));
	return 0;
}

/*
 * Do a binary search to find the message in the binary catalog.  If this
 * turns out to be slow there is an option to generate a hash in the binary
 * catalog and add another method here to look things up by the hash.
 */
static const char *lookup_bsearch(const char *msgid, struct domainbinding *db)
{
	int top, bottom, middle, omiddle;
	int n;
	struct mohandle *mohandle = &db->mohandle;

	top = 0;
	bottom = mohandle->mo.mo_nstring;
	omiddle = -1;

	while (1)
	{
		if (top > bottom)
		{
			break;
		}

		middle = (top + bottom) / 2;

		/* avoid possible infinite loop, when the data is not sorted */
		if (omiddle == middle)
		{
			break;
		}

		if (middle < 0 || middle >= mohandle->mo.mo_nstring)
		{
			break;
		}

		n = safe_strncmp(msgid, mohandle->mo.mo_otable[middle].off, I18N_STRING_LEN);
		if (n == 0)
		{
			return (const char *)mohandle->mo.mo_ttable[middle].off;
		}
		else if (n < 0)
		{
			bottom = middle;
		}
		else
		{
			top = middle;
		}
		omiddle = middle;
	}

	return NULL;
}

/*
 * Find a message in the .mo file.  This indirection is here because there are 2 ways to lookup
 * a message hash and binary search.  This version only has binary search so indirection is not
 * presently adding value.
 */
static const char *
lookup(const char *msgid, struct domainbinding *db)
{
	return lookup_bsearch(msgid, db);
}

/*
 * Get the locale info we need to determine which message catalog to use.
 * For POSIX systems this is set in environment variables, for Windows
 * its retrieved via a system call.  This function is returning
 * this in format: language_territory.codeset.  I'm ignoring category
 * because we are just concerned with messages and don't care about stuff like
 * currency, etc.  We might care about time format at some point but ignoring for now.
 */
void get_lang_env(I18N_STRING lang)
{
	get_locale(lang);
}

/*
 * what its all about, retrieve translated text for a given message ID
 */
char *gettext(const char *msgid)
{
	const char *v;
	I18N_STRING catalog_file;
	struct domainbinding *db;

	// must have a message id
	if (msgid == NULL)
	{
		return NULL;
	}

	// get the file system path to the translated file (the .mo)
	// we are only supporting a single default domain so this is
	// not really doing anything
	for (db = __bindings; db; db = db->next)
	{
		if (safe_strncmp(db->domainname, __current_domainname, I18N_STRING_LEN) == 0)
		{
			break;
		}
	}

	// this should never happen just here for debug
	if (!db)
	{
		goto fail;
	}

	get_catalog_file(db, catalog_file);

	if (lookup_catalog_file(catalog_file, db) == NULL)
	{
		goto fail;
	}

	// find the message in the binary catalog
	v = lookup(msgid, db);
	if (v)
	{
		/*
		 * TODO: all iconv() here, if translated text is encoded
		 * differently from currently-selected encoding (locale).
		 * look at Content-type header in *.mo file, in string obtained
		 * by gettext("").
		 */
		msgid = v;
	}
	fail:
	return (char *)msgid;
}
