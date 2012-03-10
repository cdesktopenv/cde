//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_new.C /main/3 1995/10/23 10:41:51 rswiston $ 			 				
/* @(#)tt_new.C	1.13 @(#)
 *
 * tt_new.cc
 *
 * Copyright (c) 1991 by Sun Microsystems, Inc.
 * 
 * This is the _Tt_allocated class.  It\'s called tt_new.cc for historical
 * reasons.
 */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "util/tt_new.h"
#include "util/tt_port.h"

void *_Tt_allocated::
operator new(size_t s)
{
	void *p = malloc(s);
	if (!p) {
		_tt_syslog( 0, LOG_ERR, strerror(ENOMEM) );
	}
	return (void *)p;
}

void _Tt_allocated::
operator  delete(void *p)
{
	free((char *)p);
}
