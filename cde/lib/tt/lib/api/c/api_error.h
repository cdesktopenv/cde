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
/*%%  $XConsortium: api_error.h /main/3 1995/10/23 09:52:10 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * api_error.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * error mapping class for use between api and spec db
 */
#ifndef _API_ERROR_H
#define _API_ERROR_H

#include "db/tt_db_results.h"
#include "api/c/tt_c.h"

typedef enum api_module {_TT_API_SPEC,	// tt_spec_ calls
		    	 _TT_API_FILE,	// tt_file_ calls
			 _TT_API_FILE_MAP,	// filename mapping API calls
		    	 _TT_API_API,	// unused currently
		    	 _TT_API_UNDEF	// unused currently
} _Tt_api_context;

Tt_status _tt_get_api_error(_Tt_db_results db_result, _Tt_api_context context);

#endif	// _API_ERROR_H
