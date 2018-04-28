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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: types.h /main/1 1996/07/29 17:10:18 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef types_INCLUDED
#define types_INCLUDED 1

#include <limits.h>
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#if UINT_MAX >= 0xffffffffL /* 2^32 - 1 */
typedef unsigned int Unsigned32;
#else
typedef unsigned long Unsigned32;
#endif

// Number holds values between 0 and 99999999 (eight nines).
typedef Unsigned32 Number;
typedef Unsigned32 Offset;
typedef Unsigned32 Index;

#ifdef SP_MULTI_BYTE

#ifdef SP_WCHAR_T_USHORT
typedef wchar_t Char;
#else
typedef unsigned short Char;
#endif

#if INT_MAX > 65535L
typedef int Xchar;
#else /* INT_MAX <= 65535L */
typedef long Xchar;
#endif /* INT_MAX <= 65535L */

#else /* not SP_MULTI_BYTE */

typedef unsigned char Char;
// This holds any value of type Char plus InputSource:eE (= -1).
typedef int Xchar;

#endif /* not SP_MULTI_BYTE */

typedef Unsigned32 UnivChar;
typedef Unsigned32 WideChar;

// A character in a syntax reference character set.
// We might want to compile with wide syntax reference characters
// (since they're cheap) but not with wide document characters.
typedef Unsigned32 SyntaxChar;

typedef unsigned short CharClassIndex;

typedef unsigned Token;

#ifdef SP_MULTI_BYTE
typedef unsigned short EquivCode;
#else
typedef unsigned char EquivCode;
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* not types_INCLUDED */
