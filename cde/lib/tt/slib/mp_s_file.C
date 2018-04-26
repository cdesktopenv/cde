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
//%%  $XConsortium: mp_s_file.C /main/3 1995/10/23 11:54:43 rswiston $ 			 				
/*
 *
 * mp_s_file.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <stdio.h>
#include "mp_s_mp.h"
#include "mp/mp_pattern.h"
#include "mp_s_file.h"

//
// Use parent class (_Tt_file) for construction and destruction.
//
_Tt_s_file::_Tt_s_file()
{
}

_Tt_s_file::_Tt_s_file(
	const _Tt_string &path
)
{
	networkPath = path;
}

_Tt_s_file::~_Tt_s_file()
{
}

Tt_status
_Tt_s_file::s_join(
	_Tt_s_procid_ptr &p
) 
{
	_Tt_pattern_list_cursor		pats(p->patterns());
	int				scopes;
	int				fpatterns_found = 0;

	while (pats.next()) {
		scopes = pats->scopes();
		if (scopes&(1<<TT_FILE)) {
			fpatterns_found = 1;
		}
		if (scopes&(1<<TT_FILE) ||
		    scopes&(1<<TT_FILE_IN_SESSION)) {
			pats->add_file(networkPath);
			_tt_s_mp->mod_file_scope(networkPath, 1);
		}
	}
	if (fpatterns_found) {
		p->add_joined_file(networkPath);
	}
	// note: TT_WRN_NOTFOUND is returned if there are no
	// TT_FILE patterns that were updated. This is just an
	// indication to the client side that it doesn't need
	// to update the database. If patterns were found and
	// this is the first procid in the session to join
	// this file then TT_WRN_STALE_OBJID is returned
	// telling the client side to update the database.
	return((fpatterns_found) ?
	       ((procs_joined() == 1) ? TT_WRN_STALE_OBJID : TT_OK)
	       : TT_WRN_NOTFOUND);
}

Tt_status
_Tt_s_file::s_quit(
	_Tt_s_procid_ptr &p
)
{
	_Tt_pattern_list_cursor		pats(p->patterns());
	int				scopes;
	int				fpatterns_found = 0;
		
	while (pats.next()) {
		scopes = pats->scopes();
		if (scopes&(1<<TT_FILE)) {
			fpatterns_found = 1;
		}
		if (scopes&(1<<TT_FILE) ||
		    scopes&(1<<TT_FILE_IN_SESSION)) {
			pats->del_file(networkPath);
			_tt_s_mp->mod_file_scope(networkPath, 0);
		}
	}
	if (fpatterns_found) {
		p->del_joined_file(networkPath);
	} else {
		return TT_WRN_NOTFOUND;
	}
	if (_tt_s_mp->in_file_scope(networkPath)) {
		return TT_OK;
	} else {
		// Tells _Tt_c_file::c_quit() to delete this session from file
		return TT_WRN_STALE_OBJID;
	}
}

int
_Tt_s_file::procs_joined()
{
	_Tt_s_procid_table_cursor	procs(_tt_s_mp->active_procs);
	int			nprocs = 0;
	
	while (procs.next()) {
		if (procs->joined_to_file(networkPath)) {
			nprocs++;
		}
	}
	return(nprocs);
}
