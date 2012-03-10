/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_session_utils.h /main/3 1995/10/23 10:29:36 rswiston $ 			 				 */
/*
 *
 * mp_session_utils.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * Declaration of auxilliary data structures for the _Tt_session class
 */
#ifndef MP_SESSION_UTILS_H
#define MP_SESSION_UTILS_H
#include <util/tt_object.h>
#include <util/tt_string.h>
#include <util/tt_table.h>
class _Tt_session;

declare_list_of(_Tt_session)
declare_table_of(_Tt_session)

#endif				/* MP_SESSION_UTILS_H */
