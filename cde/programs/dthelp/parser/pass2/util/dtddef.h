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
/* $XConsortium: dtddef.h /main/3 1995/11/08 11:03:31 rswiston $ */
/* Copyright (c) 1988, 1989 Hewlett-Packard Co. */
/* Dtddef.h contains definitions relevant to document-type
definitions */

typedef int M_ELEMENT ;
typedef int M_ARC ;
typedef int M_STATE ;
typedef int M_ANDGROUP ;

/* Possible element content types */
#define M_REGEXP 0
#define M_ANY 1
#define M_NONE 2
#define M_CDATA 3
#define M_RCDATA 4

typedef struct{
  int enptr, start, content, inptr, exptr, parptr, parcount, srefptr ;
  unsigned int smin : 1, emin : 1, useoradd : 1 ;
  } M_ELTSTRUCT ;

typedef struct {
  int paramname, type, kwlist, deftype ;
  M_WCHAR *defval ;
  } M_PARAMETER ;
/* Possible parameter default types */
#define M_NAMEDEF 0
#define M_CURRENT 1
#define M_REQUIRED 2
#define M_NULLDEF 3
#define M_CHANGEDCUR 4

/* Possible parameter types */
#define M_KEYWORD 0
#define M_ID 1
#define M_IDRF 2
#define M_IDRFS 3
#define M_NAMEPAR 4
#define M_NAMES 5
#define M_NMTOKEN 6
#define M_NMSTOKEN 7
#define M_NUMBER 8
#define M_NUMS 9
#define M_NUTOKEN 10
#define M_NUSTOKEN 11
#define M_CDATT 12
#define M_ENTATT 13

typedef struct {
  int sref ;
  int entity ;
  int next ;
  } M_SREF ;

typedef struct {
  unsigned final : 1, datacontent : 1 ;
  unsigned first : 14 ;
  } M_STATESTRUCT ;

typedef struct {
  M_STATE start ;
  M_ANDGROUP next ;
  } M_ANDSTRUCT ;

typedef struct {
  M_ELEMENT element ;
  int next ;
  } M_EXCEPTION ;


typedef struct {
  unsigned label : 15;
  unsigned optional : 1;
  int minim ;
  M_ANDGROUP group ;
  M_STATE to ;
  M_ARC next ;
  } M_ARCSTRUCT ;


