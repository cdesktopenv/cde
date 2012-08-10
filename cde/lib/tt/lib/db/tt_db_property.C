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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_property.C /main/3 1995/10/23 10:04:30 rswiston $ 			 				
/* 
 * tt_db_property.cc - Implement the _Tt_db_property class
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */


#include "db/tt_db_property.h"

// Constructors and destructors 

_Tt_db_property::
_Tt_db_property ()
{
	values = new _Tt_string_list;
}

_Tt_db_property::
~_Tt_db_property ()
{
}


// whether a property is empty

int _Tt_db_property::
is_empty() const
// consider a property to be empty if it's values list is empty, or if all
// entries in the values list are NULL content member tt_strings.
{
 if (values->is_empty()) return TRUE;  // empty values list case

 // check the list for all NULL content member tt_strings
 _Tt_string_list_cursor values_cursor(values);
 while (values_cursor.next())  if (*values_cursor != (const char*)NULL) return FALSE;

 return TRUE;
} 

   
