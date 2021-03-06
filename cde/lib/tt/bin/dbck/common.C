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
//%%  $XConsortium: common.C /main/3 1995/10/20 16:25:17 rswiston $ 			 				
/*
 *
 * common.cc
 *
 * Some utility routines common to all inspect-and-repair tools (but
 * not part of any class.)
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "dbck.h"
     
static int
do_a_directory(_Tt_string path, int (*fn)(_Tt_string))
{
	_Tt_string suffix("TT_DB/");
	
	if (path.right(1)!="/") {
		path = path.cat("/");
	}
	
	if (path.right(suffix.len())!=suffix) {
		path = path.cat(suffix);
	}
	
	return (*fn)(path);
}

int
do_directories(const _Tt_string_list_ptr &dirs, int (*fn)(_Tt_string))
{
	int failcount = 0;
	_Tt_string_list_cursor c(dirs);

	if (dirs->is_empty()) {
		failcount = !do_a_directory(".",fn);
	} else {
		while(c.next()) {
			failcount += !do_a_directory(*c,fn);
		}
	}
	return failcount;
}

		
