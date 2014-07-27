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
 *+SNOTICE
 *
 *	$TOG: EUSCompat.h /main/4 1998/04/03 17:11:57 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _EUSCOMPAT_H
#define _EUSCOMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SunOS)
#if (RELMAJOR==5)
#if (RELMINOR<2)
#define STRCASECMP_NOT_DEFINED
#endif /* RELMINOR */
#endif /* RELMAJOR */
#endif

/*
** System V R4 based systems define the stuff we need in
** sys/types.h. Include that and then we are done.
*/
#if defined(HPUX) || defined(linux) || defined(SunOS) || defined(UNIX_SV) || defined(USL) || defined(__uxp__)
#include <sys/types.h>
#endif

#if defined(sun) && defined(_XOPEN_SOURCE)
#ifndef B_TRUE
#define B_TRUE _B_TRUE
#endif
#ifndef B_FALSE
#define B_FALSE _B_FALSE
#endif
#endif   /* sun && _XOPEN_SOURCE */

/*
** HPUX defines most of what we need, if we set the right
** include options before including the system files.
*/
#if defined(HPUX)

#ifndef _INCLUDE_POSIX_SOURCE
#define _INCLUDE_POSIX_SOURCE
#endif

#ifndef _INCLUDE_XOPEN_SOURCE
#define _INCLUDE_XOPEN_SOURCE
#endif

#ifndef _INCLUDE_AES_SOURCE
#define _INCLUDE_AES_SOURCE
#endif

#ifndef _INCLUDE_HPUX_SOURCE
#define _INCLUDE_HPUX_SOURCE
#endif

#ifndef hpV4
typedef unsigned long ulong_t;
#endif /* hpV4 */
typedef unsigned char uchar_t;
typedef enum {B_FALSE, B_TRUE} boolean_t;

#define _SC_PAGESIZE	_SC_PAGE_SIZE

#ifndef MAXPATHLEN
#include <sys/param.h>
#endif

#define MAXNAMELEN      256

#endif	/* HPUX */

#if defined(linux) || defined(CSRG_BASED)
typedef enum {B_FALSE, B_TRUE} boolean_t;
#define MAXNAMELEN      	256

#if !defined(__FreeBSD__)
#define iconv_t			int
#define iconv_open(a, b) 	((iconv_t) -1)
#define iconv(a, b, c, d, e)	((size_t) 0)
#define iconv_close(a)		(0)
#endif
#endif


/*
** AIX, like HPUX defines most of what we need.
*/
#if defined(AIX)

#ifndef KERNEL
#define KERNEL
#endif

#ifndef _BSD_INCLUDES
#define _BSD_INCLUDES
#endif

#include <sys/types.h>

#define _SC_PAGESIZE	_SC_PAGE_SIZE
#define vfork		fork

#ifndef MAXPATHLEN
#include <sys/param.h>
#endif

#define MAXNAMELEN      256

#ifndef _POWER
typedef enum {B_FALSE, B_TRUE} boolean_t;
#else /* _POWER */
#ifndef B_FALSE
#define B_FALSE 0
#endif
#ifndef B_TRUE
#define B_TRUE 1
#endif
#endif /* _POWER */

#undef BIG_ENDIAN

#endif /* AIX */

#if defined(USL) || defined(__uxp__)

#include <sys/param.h>

#define STRCASECMP_NOT_DEFINED
#if !defined(S_ISLNK)
#define S_ISLNK(mode)     (((mode) & S_IFMT) == S_IFLNK)
#endif
#endif

/*
 * A bug in Solaris 2.1 and the GNU compilers, these are not defined.
 */
#ifdef STRCASECMP_NOT_DEFINED
  extern int strcasecmp(const char *, const char *);
  extern int strncasecmp(const char *, const char *, size_t);
#endif

 /*
 ** 
 */
#if defined(__osf__)

#define vfork		fork

#include <sys/types.h>

#ifndef MAXPATHLEN
#include <sys/param.h>
#endif

#ifndef MAXNAMELEN
#define MAXNAMELEN      256
#endif

typedef enum {B_FALSE, B_TRUE} boolean_t;

#undef BIG_ENDIAN

#endif /* __osf__ */

#ifdef __cplusplus
}
#endif


#endif
