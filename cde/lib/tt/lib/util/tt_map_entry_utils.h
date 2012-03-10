/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_map_entry_utils.h /main/3 1995/10/23 10:41:44 rswiston $ 			 				 */
/* @(#)tt_map_entry_utils.h	1.3 @(#)
 * Tool Talk Utility - tt_map_entry_utils.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declares the _Tt_map_entry utils.
 */

#ifndef _TT_MAP_ENTRY_UTILS_H
#define _TT_MAP_ENTRY_UTILS_H

#include "util/tt_ptr.h"
#include "util/tt_table.h"

class _Tt_map_entry;

declare_derived_ptr_to(_Tt_map_entry, _Tt_object)
declare_table_of(_Tt_map_entry)

#endif /* _TT_MAP_ENTRY_UTILS_H */
