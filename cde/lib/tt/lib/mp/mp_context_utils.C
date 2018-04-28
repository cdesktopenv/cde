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
//%%  $XConsortium: mp_context_utils.C /main/3 1995/10/23 10:22:52 rswiston $ 			 				
/* -*-C++-*-
 *
 * @(#)mp_context_utils.C	1.4 @(#)
 *
 * Tool Talk Message Passer (MP) - mp_context_utils.cc
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include <mp/mp_context.h>
#include <mp/mp_context_utils.h>
#include <stdio.h>

implement_list_of(_Tt_context)

// _Tt_context_list supports the append_ordered method.  Contexts
// are ordered by slotname.

_Tt_context_list& _Tt_context_list::
append_ordered(const _Tt_context_ptr &e)
{
	
// Application programmers are either going to insert contexts in
// alphabetical order or at random.  So we start at the end, 
// find the last element less than the new one and insert after that.
// If an equal element is found, replace the old one with this one.	
// No indication is returned if replacement is done; if you really care,
// check the count before and after to see if it changed.

	_Tt_context_list_ptr this_ptr = this;
	_Tt_context_list_cursor c(this_ptr);

	while (c.prev()) {
		int t= c->slotName().cmp(e->slotName());
		if (t==0) {
			// c->slotName == e->slotName
			c.remove().insert(e);
			return *this;
		} else if (t<0) {
			// c->slotName < e->slotName
			// insert after here.
			c.insert(e);
			return *this;
		} else {
		        // c->slotName > e->slotName
			// this element greater than new one, look further.
		}
	}
		
	// There are no elements in the list smaller than this one, so
	// put the new one at the beginning.

	push(e);
	return *this;
}
