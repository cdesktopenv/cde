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
//%%  $XConsortium: mp_file.C /main/3 1995/10/23 10:23:33 rswiston $ 			 				
/*
 *
 * @(#)mp_file.C	1.40 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_file.cc
 *
 * Copyright (c) 1990, 1991, 1992 by Sun Microsystems, Inc.
 *
 * Implementation of the _Tt_file class representing a document.
 */

#include <stdio.h>
#include <unistd.h>
#include "api/c/api_api.h"
#include "mp/mp_file.h"
#include "mp/mp_mp.h"
#include "util/tt_string.h"
#include "util/tt_xdr_version.h"

_Tt_qmsg_info::_Tt_qmsg_info()
{
	categories = new _Tt_int_rec_list;
	ptypes = new _Tt_string_list;
	version = TT_QMSG_INFO_VERSION;
	
	id = 0;
	nparts = 0;
	size = 0;
	m_id = 0;
}

_Tt_qmsg_info::~_Tt_qmsg_info()
{
}

bool_t
_Tt_qmsg_info::xdr(
	XDR *xdrs
)
{
	_Tt_xdr_version		xvers(1);

	return    xdr_int( xdrs, &version )
	       && xdr_int( xdrs, &id )
	       && xdr_int( xdrs, &nparts )
	       && xdr_int( xdrs, &size )
	       && categories.xdr( xdrs )
	       && ptypes.xdr( xdrs )
	       && xdr_int( xdrs, &m_id )
	       && sender.xdr( xdrs );
}

_Tt_file::_Tt_file()
{
}

_Tt_file::_Tt_file(
	const _Tt_string &path
) :
	_Tt_db_file( path )
{
}

_Tt_file::~_Tt_file()
{
}

//
//  query - iterate through each node in this file and apply the given filter
//  to it.  Returns one of the following codes:
//	TT_OK	-  successful
//	TT_ERR_DBAVAIL	error occurred while accessing database
//	TT_WRN_STOPPED	filter returns TT_FILTER_STOP
//	TT_ERR_INTERNAL
//

Tt_status
_Tt_file::query(
	_Tt_file_callback	callback,
	Tt_filter_function	filter,
	void                   *context,
	void 		       *accumulator
)
{
	// Apply 'filter' function to each node belonging to this
	// file.  If Filter returns FILTER_STOP then the query
	// function should return immediately.
	
	_Tt_string_list_ptr specIDs = getObjects();
	_Tt_db_results dbStatus = getDBResults();
	switch (dbStatus) { // XXX
	    case TT_DB_OK:
		break;
	    case TT_DB_ERR_ILLEGAL_FILE:
	    default:
		return TT_ERR_INTERNAL;
	}
	_Tt_string_list_cursor specID( specIDs );
	while (specID.next()) {
		if (    (*callback)( filter,
				     (*specID).quote_nulls(),
				     context,
				     accumulator)
		     == TT_FILTER_STOP)
		{
			return TT_WRN_STOPPED;
		}
	}
	return TT_OK;
}

_Tt_string
_Tt_file::networkPath_(
	_Tt_object_ptr &obj
)
{
	return ((_Tt_file *)obj.c_pointer())->getNetworkPath();
}
