/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * File:	stringx.h $TOG: stringx.h /main/5 1999/10/15 17:23:52 mgreess $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef __STRINGX_H_
#define __STRINGX_H_

#ifdef DOMAIN_ALLOW_MALLOC_OVERRIDE
#include "/usr/include/apollo/shlib.h"
#endif
#ifdef apollo
#include <stdlib.h>
#endif

#include <stddef.h>
#include <string.h>
#include <codelibs/boolean.h>

#define streq(a,b) (strcmp(a,b) == 0)

/* private buffer variables */
extern void *_strsep_privbuf;
extern void *_strcmbn_privbuf;

#if defined(__cplusplus)
extern "C"
{
#endif
#if defined(__cplusplus) || defined(__STDC__)
#if !defined(__osf__) && !defined(sun)
    /* The DEC C++ compiler rejects this, claiming it has both */
    /* internal and external linkage. */
    char *strnew(size_t len);
    void strfree(const char *s);
#endif
    char *strstrx(char *s1, const char *s2);
    char *strrstrx(char *s1, const char *s2);
    int strwcmp(const char *pattern, const char *str);
    int strwcmpi(const char *pattern, const char *str);
    char *strwpat(const char *pattern);
    char *strend(const char *str);

    unsigned strhash(const char *key);
    unsigned strhashi(const char *key);

    char *strupper(char *str);
    char *strlower(char *str);

#ifdef __cplusplus
    char *strtokx(char *&ptr, const char *sep);
# if !defined(__osf__) && !defined(linux) && !defined(CSRG_BASED)
    char **strsep(const char *str, const char *sep,
	boolean whsp = TRUE, int *num = NULL);
#  if !defined(__osf__)
    const char *strcmbn(const char **vec, const char *sep = " ");
#  endif
# endif

#else /* __STDC__ */
    char *strtokx(char **ptr, const char *sep);
# if !defined(linux) && !defined(CSRG_BASED)
    char **strsep(const char *str, const char *sep,
	boolean whsp, int *num);
#endif
#ifndef __osf__
    const char *strcmbn(const char **vec, const char *sep);
#endif

#endif /* __STDC__ */


#ifdef __OBSOLETE
    size_t nl_strlen(const char *str);	/* __OBSOLETE */
    int strcharsize(const char *str);	/* __OBSOLETE */
#endif /* __OBSOLETE */

#else /* C */

extern void   strfree();
extern char  *strstrx(), strrstrx();
extern int    strwcmp(), strwcmpi();
extern char  *strwpat();
extern char  *strend();
extern char  *strtokx();

extern unsigned   strhash();
extern unsigned   strhashi();

extern char      *strupper(), *strlower();


#ifdef __OBSOLETE
extern int        strcharsize();	/* __OBSOLETE */
extern size_t     nl_strlen();		/* __OBSOLETE */
#endif /* __OBSOLETE */

#endif /* C */
#if defined(__cplusplus)
}

#if defined(apollo) || defined(__aix) || defined(USL) || defined(__uxp__) || defined(__osf__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif /* apollo */

inline char *strnew(size_t len) { return (char*)malloc(len + 1); }
#if defined(sun) || defined(__sun) || defined(USL) || defined(__uxp__)
inline void  strfree(const char *s)
	{ if (s != NULL) free((char *)s); }
#else
inline void  strfree(const char *s)
#if defined(__hpux) || defined(__osf__) || defined(CSRG_BASED)
	{ if (s != NULL) free((void *)s); }
#else
	{ if (s != NULL) free((const void *)s); }
#endif /* __hpux */
#endif
#if defined(bsd)
inline char *strdup(const char *s)
	{ return strcpy((char*)malloc(strlen(s) + 1), s); }
#endif

/* private buffer funcs - we use inlines to handle default args properly */

#else /* C || __STDC__ */

#define strnew(len) ((char*)malloc((len) + 1))

#define strbld strblds

/* macros for funcs that work on top of privbuf versions */

/* for backward compatibility only - __OBSOLETE */
#ifdef __OBSOLETE
#define strpos strstrx /* __OBSOLETE */
#define strrpos strrstrx /* __OBSOLETE */
#endif /* __OBSOLETE */

#endif /* C || __STDC__ */

#endif /* __STRINGX_H_ */
