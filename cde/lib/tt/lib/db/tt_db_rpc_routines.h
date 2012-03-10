/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_rpc_routines.h /main/3 1995/10/23 10:05:32 rswiston $ 			 				 */
/*
 * tt_db_rpc_routines.h - Declares routines for converting TT DB classes to
 *                        RPC arguments.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_RPC_ROUTINES_H
#define _TT_DB_RPC_ROUTINES_H

#include "util/tt_string.h"
#include "db/db_server.h"
#include "db/tt_db_property_utils.h"
#include "db/tt_db_access_utils.h"
#include "db/tt_db_results.h"

extern void _tt_free_rpc_message (const _tt_message&);
extern void _tt_free_rpc_messages (const _tt_message_list&);
extern void _tt_free_rpc_strings (const _tt_string_list&);
extern void _tt_free_rpc_property (const _tt_property&);
extern void _tt_free_rpc_properties (const _tt_property_list&);
extern void _tt_get_rpc_access (const _tt_access&, _Tt_db_access_ptr&);
extern void _tt_get_rpc_strings (const _tt_string_list&, _Tt_string_list_ptr&);
extern void _tt_get_rpc_property (const _tt_property&, _Tt_db_property_ptr&);
extern void _tt_get_rpc_properties (const _tt_property_list&,
				    _Tt_db_property_list_ptr&);
extern void _tt_set_rpc_access (const _Tt_db_access_ptr&, _tt_access&);
extern void _tt_set_rpc_strings (const _Tt_string_list_ptr&, _tt_string_list&);
extern void _tt_set_rpc_property (const _Tt_db_property_ptr&, _tt_property&);
extern void _tt_set_rpc_properties (const _Tt_db_property_list_ptr&,
				    _tt_property_list&);

#endif /* _TT_DB_RPC_ROUTINES_H */
