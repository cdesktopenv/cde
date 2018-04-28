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
// $XConsortium: Dict_Templates.cc /main/3 1996/06/11 16:42:42 cde-hal $
#define EXPAND_TEMPLATES

#include "Link.hh"
#include "Dict.hh"
#include "DictIter.hh"
#include "UAS_String.hh"

typedef Link<UAS_String, int>	_Link_UAS_String_int_;
typedef Dict<UAS_String, int>	_Dict_UAS_String_int_;
typedef DictIter<UAS_String, int>	_DictIter_UAS_String_int_;
//typedef UAS_Pointer<UAS_String>	_UAS_Pointer_UAS_String;


