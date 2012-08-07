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
/* $XConsortium: basic.h /main/3 1995/11/08 11:35:48 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */
/* Basic.h has a few very primitive #define's used by all MARKUP programs */
#include <stdlib.h>

/* hpux protects stdio.h from multiple includes thru stdlib.h . */
#include <stdio.h>

#define TRUE 1
#define FALSE 0
typedef unsigned char LOGICAL;

/* End of String marker */
#define M_EOS '\0'

/* Integer null value marker */
#define M_NULLVAL 0

/* 8-bit data characters */
typedef unsigned char M_CHAR8;

/* wide characters */
typedef wchar_t M_WCHAR;

/* For function prototypes */
typedef char * M_POINTER;
#if defined(hpux) || defined(__aix) || defined(sun) || defined(USL) || defined(__uxp__)
/* Old unix stuff
 *#define void int
 *#define M_NOPAR
 */
/* New Ansi style */
#define M_PROTO
#define M_NOPAR void
#else
#define M_PROTO
#define M_NOPAR void
#endif

char *MakeMByteString(
#if defined(M_PROTO)
  const M_WCHAR *from
#endif
  );

M_WCHAR *MakeWideCharString(
#if defined(M_PROTO)
  const char *from
#endif
  );

int w_strcmp(
#if defined(M_PROTO)
  const M_WCHAR *string1, const M_WCHAR *string2
#endif
  );

M_WCHAR *w_strcpy(
#if defined(M_PROTO)
  M_WCHAR *string1, const M_WCHAR *string2
#endif
  );

M_WCHAR *w_strncpy(
#if defined(M_PROTO)
  M_WCHAR *string1, const M_WCHAR *string2, int max
#endif
  );

int w_strlen(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  );

M_WCHAR *w_strchr(
#if defined(M_PROTO)
  M_WCHAR *string, const M_WCHAR chr
#endif
  );

M_WCHAR *w_strstr(
#if defined(M_PROTO)
  M_WCHAR *string1, M_WCHAR *string2
#endif
  );

void *m_malloc(
#if defined(M_PROTO)
  int size, char *msg
#endif
  ) ;
