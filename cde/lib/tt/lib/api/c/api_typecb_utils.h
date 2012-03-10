/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: api_typecb_utils.h /main/3 1995/10/23 09:55:34 rswiston $ 			 				 */
/* @(#)api_typecb_utils.h	1.2 93/07/30 SMI
 *
 * api_typecb_utils.h
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 * 
 * Utilities declarations for _Tt_typecb
 */
#ifndef _API_TYPECB__UTILS_H
#define _API_TYPECB_UTILS_H
#include "util/tt_object.h"
#include "util/tt_list.h"
#include "util/tt_table.h"
#include "util/tt_string.h"

declare_list_of(_Tt_typecb)
declare_table_of(_Tt_typecb)

_Tt_string _tt_typecb_key(_Tt_object_ptr &o);
_Tt_string _tt_typecb_key(const _Tt_string &tid, int opnum);

#endif				/* MP_PATTERN_UTILS_H */
