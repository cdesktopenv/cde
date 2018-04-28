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
/* $XConsortium: asciiIn_filters.h /main/4 1996/06/11 17:26:27 cde-hal $ */

#ifndef _filters_h
#define _filters_h 1

#include "utility/funcs.h"
#include "api/info_lib.h"

char* get_oid(info_lib*, char* base_nm, char* locator_line); // with byte count and \t
char* get_oid_2(info_lib*, char* base_str, char* locator); // without byte count and \t


#endif
