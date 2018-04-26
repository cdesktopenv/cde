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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_map_entry.h /main/3 1995/10/23 10:41:30 rswiston $ 			 				 */
/* @(#)tt_map_entry.h	1.4 @(#)
 * Tool Talk Utility - tt_map_entry.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declares a class for holding a string map entry.  Static
 * member functions are provided for returning a variety of
 * table key values.  Anything that can be reaonably derived
 * from the address field is game for being a new "getAddress"
 * type of function.
 */

#ifndef _TT_MAP_ENTRY_H
#define _TT_MAP_ENTRY_H

#include "util/tt_object.h"
#include "util/tt_path.h"
#include "util/tt_string.h"

class _Tt_map_entry : public _Tt_object {
public:
  _Tt_map_entry ();
  ~_Tt_map_entry ();

  _Tt_string address;
  _Tt_string data;

  static _Tt_string getAddress (_Tt_object_ptr &entry);
  static _Tt_string getPathAddress (_Tt_object_ptr &entry);

};

#endif /* _TT_MAP_ENTRY_H */






