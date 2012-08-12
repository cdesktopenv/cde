/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
