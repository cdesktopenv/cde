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
//%%  $XConsortium: tt_old_db_message_info.C /main/3 1995/10/23 10:06:11 rswiston $ 			 				
/*
 *
 * @(#)tt_old_db_message_info.C	1.3 30 Jul 1993
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "db/tt_old_db_message_info.h"

_Tt_old_db_message_info::
_Tt_old_db_message_info ()
{
	messageID = 0;
	numParts = 0;
	messageSize = 0;
}

_Tt_old_db_message_info::
~_Tt_old_db_message_info ()
{
}


bool_t _Tt_old_db_message_info::
xdr (XDR *xdrs)
{
	int             dummy_int = 0;
	_Tt_string      dummy_string = "";
	bool_t          results;
	_Tt_xdr_version xvers(1);
	
	results = xdr_int(xdrs, &dummy_int);
	
	if (results) {
		results = xdr_int(xdrs, &messageID);
	}
	
	if (results) {
		results = xdr_int(xdrs, &numParts);
	}
	
	if (results) {
		results = xdr_int(xdrs, &messageSize);
	}
	
	if (results) {
		int count = (ptypes.is_null() ? 0 : ptypes->count());
		results = xdr_int(xdrs, &count);
		
		for (int i=0; results && (i < count); i++) {
			results = xdr_int(xdrs, &dummy_int);
		}
	}
	
	if (results) {
		results = ptypes.xdr(xdrs);
	}
	
	if (results) {
		results = xdr_int(xdrs, &dummy_int);
	}
	
	if (results) {
		results = dummy_string.xdr(xdrs);
	}
	
	return results;
}
