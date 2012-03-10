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
