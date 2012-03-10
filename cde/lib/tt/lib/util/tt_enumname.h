/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_enumname.h /main/5 1996/10/28 14:02:12 drk $ 			 				 */
/*
 *
 * tt_enumname.h
 *
 * Declarations for _tt_enumname() functions for each enumeration type
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#if !defined(_TT_ENUMNAME_H)
#define _TT_ENUMNAME_H
#include "api/c/tt_c.h"
#include "util/tt_entry_pt.h"

typedef int _Tt_ce_status;

const char * _tt_enumname(Tt_status x);
const char * _tt_enumname(Tt_filter_action x);
const char * _tt_enumname(Tt_mode x);
const char * _tt_enumname(Tt_scope x);
const char * _tt_enumname(Tt_class x);
const char * _tt_enumname(Tt_category x);
const char * _tt_enumname(Tt_address x);
const char * _tt_enumname(Tt_disposition x);
const char * _tt_enumname(Tt_state x);
const char * _tt_enumname(Tt_feature f);
const char * _tt_enumname(Tt_callback_action x);
const char * _tt_enumname(_Tt_entry_pt x);
const char * _tt_enumname(_Tt_ce_status x);

#endif
