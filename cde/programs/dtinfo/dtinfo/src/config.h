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
/*	Copyright (c) 1994,1995 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

/* -*-c++-*-
 * $XConsortium: config.h /main/7 1996/09/27 19:03:04 drk $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include <Exceptions.hh>

/*
  Folio Configuration File
*/

#define CLASS_NAME	"Dtinfo"

#define MOTIF
//#define UseFJMTF

#ifdef MOTIF
#define LOOK_AND_FEEL "Motif"
#define X_WINDOWS
#endif

#ifdef X_WINDOWS
#define WINDOW_SYSTEM_NAME "X"
#endif

#ifndef Internationalize
inline
char *gettext(char *text)
{
  return text ;
}
#endif


// NOTE: Things below probably belong in a types.h file

// typedef void *UIHandle ;

// the following can't be enum because they are
// definitely defined in other include files
#define TRUE 1
#define FALSE 0
//typedef unsigned char bool;

typedef char  int8;
typedef short int16;
typedef int   int32;
typedef unsigned char  u_int8;
typedef unsigned short u_int16;
typedef unsigned int   u_int32;
typedef unsigned int   u_int;
typedef unsigned char  u_char;

// NOTE: let's find a good home for this 
// NOTE: ( arbitrary? value ) - jbm

// allow downscaling by 4 point sizes
#define MIN_DOWNSCALE 4

// NOTE: VERY, VERY, VERY temporary!!

  
#define STR_TO_OID(STRING) \
  ObjectId (oid_t ((u_int32) Atomizer (STRING)))

#define WAutoManage 1
#define WPopup 1
#define self	*this

#ifndef STRINGIFY
#if defined(__STDC__) || defined(hpux) || defined(__osf__)
#define STRINGIFY(S) #S
#else
#define STRINGIFY(S) "S"
#endif
#endif

#ifndef CONCAT
#if defined(__STDC__) || defined(hpux) || defined(__osf__)
#define CONCAT(A,B) A##B
#define CONCAT3(A,B,C) A##B##C
#else
#define CONCAT(A,B) A/**/B
#define CONCAT3(A,B,C) A/**/B/**/C
#endif
#endif

#ifdef DEBUG
#define ON_DEBUG(stmt)	stmt
#else
#define ON_DEBUG(stmt)
#endif

#include <stddef.h>
#include <stdlib.h>

// for assertion checking

#ifndef DEBUG
# ifndef NDEBUG
#  define NDEBUG
# endif
#endif

// for debugging statements if desired
#ifdef DEBUG
#include <iostream>
using namespace std;
#include <stdio.h>
#endif

#ifdef _lucid_
extern "C" {
void exit(const int);
}
#endif
#include <assert.h>

#ifdef USES_OLIAS_FONT
#include "olias_font.h"
#endif

// these should be moved to a "debug.h" type file

// The BOGUS preprocessor is substituting OBJ in the string.
// NOTE: how to #ifdef this?

#ifdef __STDC__
#define PRINT_OBJECT(OBJ) \
cout << #OBJ << "= (0x" << hex(this) << ")" << endl

#define PRINT_POINTER(PTR) \
cout << "  " << #PTR << " = 0x" << hex(PTR) << endl

#define PRINT_INT(INT) \
cout << "  " << #INT << " = " << INT << endl

#define PRINT_BITS(BITS) \
  { long bits = BITS; cout  << "  " << #BITS << " = "; \
    for (int i = 0; i < sizeof(BITS) * 8; i++, bits <<= 1) \
      (bits & (1L << sizeof(BITS) * 8)) ? cout << '1' : cout << '0'; \
    cout << endl; } 
#else
#define PRINT_OBJECT(OBJ) \
  printf ("OBJ (0x%p) = \n", this)
#define PRINT_POINTER(PTR) \
  printf ("  PTR = 0x%p\n", PTR)
#define PRINT_INT(INT) \
  printf ("  INT = %d\n", INT)
#define PRINT_BITS(BITS) \
  { long bits = BITS; printf ("  BITS = "); \
    for (int i = 0; i < sizeof(BITS) * 8; i++, bits <<= 1) \
      (bits & (1L << sizeof(BITS) * 8)) ? putchar('1') : putchar('0'); \
    putchar ('\n'); } 
#endif
