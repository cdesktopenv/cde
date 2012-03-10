/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_create_objid.h /main/3 1995/10/23 10:01:52 rswiston $ 			 				 */
/*
 * tt_db_create_objid.h - Defines a routine for constructing an objid.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "util/tt_string.h"
#include "db/tt_db_key_utils.h"

extern _Tt_string _tt_db_create_objid (const _Tt_db_key_ptr&,
				       const _Tt_string&,
				       const _Tt_string&,
				       const _Tt_string&);
