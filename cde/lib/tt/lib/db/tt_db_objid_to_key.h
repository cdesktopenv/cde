/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_objid_to_key.h /main/3 1995/10/23 10:04:22 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_db_objid_to_key.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc*
 *
 * Extern declarations for object ID to key conversion routine.
 *
 */

#ifndef _TT_DB_OBJID_TO_KEY_H
#define _TT_DB_OBJID_TO_KEY_H

#include <util/tt_string.h>

extern _Tt_string _tt_db_objid_to_key (const _Tt_string &objid);

#endif /* _TT_DB_OBJID_TO_KEY */
