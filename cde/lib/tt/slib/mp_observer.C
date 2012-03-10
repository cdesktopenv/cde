//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_observer.C /main/3 1995/10/23 11:50:12 rswiston $ 			 				
/*
 *
 * mp_observer.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp_observer.h"


// 
// Methods for _Tt_observer objects which just serve as records that
// hold the message fields in a _Tt_message object that can be
// different for static observers. See _Tt_s_message documentation for
// more details.
//


_Tt_observer::
_Tt_observer()
{
	_ptid = (char *)0;
	_reliability = TT_DISCARD;
	_opnum = -1;
}


_Tt_observer::
_Tt_observer(_Tt_string ptid, int opnum,
	     Tt_disposition reliability, Tt_scope s)
{
	_ptid = ptid;
	_reliability = reliability;
	_opnum = opnum;
	_scope = s;
}


_Tt_observer::
~_Tt_observer()
{
}





