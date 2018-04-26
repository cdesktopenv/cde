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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tttar_string_map.C /main/3 1995/10/20 17:01:27 rswiston $ 			 				
/*
 * tttar_string_map.cc - Implements mappings of old specs to new specs.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include "tttar_string_map.h"
#include "util/tt_iostream.h"

implement_list_of(Lstar_string_map)
implement_table_of(Lstar_string_map)

_Tt_string
Lstar_string_map_old_string(_Tt_object_ptr &o)
{
	return(((Lstar_string_map *)o.c_pointer())->old_string());
}
     
/*
 * Lstar_string_map::Lstar_string_map
 */
Lstar_string_map::
Lstar_string_map()
{
}

/*
 * Lstar_string_map::~Lstar_string_map()
 */
Lstar_string_map::
~Lstar_string_map()
{
}

/*
 * Lstar_string_map::print()
 */
void Lstar_string_map::
print( FILE *fs ) const
{
	this->_old_string->print(fs);
	fprintf( fs, " (" );
	this->_extra->print(fs);
	fprintf( fs, " ) -> " );
	this->_new_string->print(fs);
}
