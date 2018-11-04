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
//%%  $XConsortium: prop.C /main/3 1995/10/20 16:26:03 rswiston $ 			 				
/*
 *
 * prop.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include "prop.h"
#include "ttdbck.h"

// A Prop just holds the name and its list of values.
// We probably have 600 classes that do this by now.

Prop::
Prop(_Tt_string propname)
{
	_name = propname;
	_values = new _Tt_string_list();
}

void Prop::
print(FILE *f) const
{
	fprintf(f,"Prop<");
	_name->print(f);
	fprintf(f,"=<");
	_values->print(_tt_string_print,f);
	fprintf(f,">>\n");
}

_Tt_string
_tt_prop_name(_Tt_object_ptr &o)
{
	return(((Prop *)o.c_pointer())->name());
}
