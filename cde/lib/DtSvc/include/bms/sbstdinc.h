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
 * File:         sbstdinc.h $TOG: sbstdinc.h /main/7 1998/07/31 17:50:45 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _sbstdinc_h
#define _sbstdinc_h

#include <sys/types.h>
#include <sys/param.h>

#ifdef SVR4
#include <netdb.h>		/* MAXHOSTNAMELEN */
#endif /* SVR4 */

#ifndef howmany
#define howmany(x, y)  (((x)+((y)-1))/(y))   /* From <sys/param.h>, but not an XPG3 file */
#endif

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <nl_types.h>

/************************************************************************/
/* Routines not defined in include files (yet).				*/
/************************************************************************/

/* Domain/OS BSD has bzero(), bcmp(), and bcopy() defined. */
#if !(defined(apollo) && defined(__bsd)) && !defined(CSRG_BASED)

#if defined(__STDC__)
#if !defined(linux) && !defined(__osf__) && !defined(_XFUNCS_H_)
extern void bcopy(char *b1, char *b2, int length);
extern int  bcmp(char *b1, char *b2, int length);
extern void bzero(char *b, int length);
#endif

extern char *mktemp(char *tmplate);
#elif ! defined(__cplusplus)
#if !defined(linux) && !defined(__osf__) && !defined(_XFUNCS_H_)
extern void bcopy();
extern int  bcmp();
extern void bzero();
#endif

extern char *mktemp();
#endif

#endif


#ifndef  SBSTDINC_H_NO_REDEFINE   /* sbstdinc.c turns this on */

/************************************************************************/
/* Routines from <string.h> 						*/
/*  --- These always get redefined so we can catch null ptr deref's     */
/************************************************************************/

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrcat(XeString s1, ConstXeString s2);
#else
extern XeString Xestrcat();
#endif
#ifdef strcat
# undef strcat
#endif
#define strcat Xestrcat

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrncat(XeString s1, ConstXeString s2, size_t n);
#else
extern XeString Xestrncat();
#endif
#ifdef strncat
# undef strncat
#endif
#define strncat Xestrncat

#if defined(__STDC__) || defined(__cplusplus)
extern int Xestrcmp(ConstXeString s1, ConstXeString s2);
#else
extern int Xestrcmp();
#endif
#ifdef strcmp
# undef strcmp
#endif
#define strcmp Xestrcmp

#if defined(__STDC__) || defined(__cplusplus)
extern int Xestrncmp(ConstXeString s1, ConstXeString s2, size_t n);
#else
extern int Xestrncmp();
#endif
#ifdef strncmp
# undef strncmp
#endif
#define strncmp Xestrncmp

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrcpy(XeString s1, ConstXeString s2);
#else
extern XeString Xestrcpy();
#endif
#ifdef strcpy
# undef strcpy
#endif
#define strcpy Xestrcpy

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrncpy(XeString s1, ConstXeString s2, size_t n);
#else
extern XeString Xestrncpy();
#endif
#ifdef strncpy
# undef strncpy
#endif
#define strncpy Xestrncpy

#if defined(__STDC__) || defined(__cplusplus)
extern int Xestrcoll(ConstXeString s1, ConstXeString s2);
#else
extern int Xestrcoll();
#endif
#ifdef strcoll
# undef strcoll
#endif
#define strcoll Xestrcoll

#if defined(__STDC__) || defined(__cplusplus)
extern size_t Xestrxfrm(XeString s1, ConstXeString s2, size_t n);
#else
extern size_t Xestrxfrm();
#endif
#ifdef strxfrm
# undef strxfrm
#endif
#define strxfrm Xestrxfrm

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrchr(ConstXeString s, int c);
#else
extern XeString Xestrchr();
#endif
#ifdef strchr
# undef strchr
#endif
#define strchr Xestrchr

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrpbrk(ConstXeString s1, ConstXeString s2);
#else
extern XeString Xestrpbrk();
#endif
#ifdef strpbrk
# undef strpbrk
#endif
#define strpbrk Xestrpbrk

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrrchr(ConstXeString s, int c);
#else
extern XeString Xestrrchr();
#endif
#ifdef strrchr
# undef strrchr
#endif
#define strrchr Xestrrchr

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrstr(ConstXeString s1, ConstXeString s2);
#else
extern XeString Xestrstr();
#endif
#ifdef strstr
# undef strstr
#endif
#define strstr Xestrstr

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrtok(XeString s1, ConstXeString s2);
#else
extern XeString Xestrtok();
#endif
#ifdef strtok
# undef strtok
#endif
#define strtok Xestrtok

#if defined(__STDC__) || defined(__cplusplus)
extern size_t Xestrlen(ConstXeString s);
#else
extern size_t Xestrlen();
#endif
#ifndef __osf__
# ifdef strlen
#  undef strlen
# endif
# define strlen Xestrlen
#endif

#if defined(__STDC__) || defined(__cplusplus)
extern XeString Xestrdup(ConstXeString s);
#else
extern XeString Xestrdup();
#endif
#ifdef strdup
# undef strdup
#endif
#define strdup Xestrdup

#endif /* ifndef  SBSTDINC_H_NO_REDEFINE */

#endif /*  _sbstdinc_h */ 
