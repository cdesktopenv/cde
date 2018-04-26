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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_isstrerror.C /main/3 1995/10/23 12:03:24 rswiston $ 			 				
//%%	$XConsortium: tt_isstrerror.C /main/3 1995/10/23 12:03:24 rswiston $							
//%%									
//%%	RESTRICTED CONFIDENTIAL INFORMATION:				
//%%									
//%%	The information in this document is subject to special		
//%%	restrictions in a confidential disclosure agreement between	
//%%	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this	
//%%	document outside HP, IBM, Sun, USL, SCO, or Univel without	
//%%	Sun's specific written approval.  This document and all copies	
//%%	and derivative works thereof must be returned or destroyed at	
//%%	Sun's request.							
//%%									
//%%	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.	
//%%									
/*
 * @(#)tt_isstrerror.C	1.3 94/11/17 SMI 
 *
 * tt_isstrerror.C - interpret iserrno values
 *
 * Copyright (c) 1994 by Sun Microsystems, Inc.
 */

/*
 * _tt_isstrerror() is a extended version of strerror(3) that
 * also interprets the ISAM iserrno codes.
 * It's in slib because:
 * - it shouldn't go in libtt because nobody there calls
 *   ISAM directly
 * - it can't go in libisam without major restructuring of
 *   the mini_isam build, making it dependent on the libtt
 *   internationalization code
 * - both things that do call ISAM, rpc.ttdbserverd and
 *   ttdbck, do link with slib.
 */

#include"tt_isstrerror.h"

#include"util/tt_gettext.h"
#include <string.h>

/* use message set 2, TTMSET_SLIB */

const char *
_tt_isstrerror(int an_errno)
{
	switch(an_errno) {
	      case 100:
		return catgets(_ttcatd, 2, 14,"duplicate ISAM record");
	      case 101:
		return catgets(_ttcatd, 2, 15,"ISAM file not open");
	      case 102:
		return catgets(_ttcatd, 2, 16,"illegal ISAM argument");
	      case 103:
		return catgets(_ttcatd, 2, 17,"illegal ISAM key descriptor");
	      case 104:
		return catgets(_ttcatd, 2, 18,"too many ISAM files open");
	      case 105:
		return catgets(_ttcatd, 2, 19,"bad ISAM file format");
	      case 106:
		return catgets(_ttcatd, 2, 20,"ISAM non-exclusive access");
	      case 107:
		return catgets(_ttcatd, 2, 21,"ISAM record locked");
	      case 108:
		return catgets(_ttcatd, 2, 22,"ISAM key already exists");
	      case 109:
		return catgets(_ttcatd, 2, 23,"ISAM key is primary key");
	      case 110:
		return catgets(_ttcatd, 2, 24,"end or beginning of ISAM file");
	      case 111:
		return catgets(_ttcatd, 2, 25,"no ISAM record found");
	      case 112:
		return catgets(_ttcatd, 2, 26,"no current ISAM record");
	      case 113:
		return catgets(_ttcatd, 2, 27,"ISAM file locked");
	      case 114:
		return catgets(_ttcatd, 2, 28,"ISAM file name too long");
	      case 116:
		return catgets(_ttcatd, 2, 29,
			       "ISAM cannot allocate memory");
	      case 1117:
		return catgets(_ttcatd, 2, 30, "ISAM RPC timeout");
	      case 1118:
		return catgets(_ttcatd, 2, 31,
			       "Broken ISAM TCP/IP connection");
	      case 1119:
		return catgets(_ttcatd, 2, 32,
			       "Cannot connect to ISAM server");
	      case 1120:
		return catgets(_ttcatd, 2, 33, "Cannot import ISAM data");
	      case 1121:
		return catgets(_ttcatd, 2, 34, "no local SAM daemon");
	      case 1122:
		return catgets(_ttcatd, 2, 35, "ISAM internal fatal error");
	      case 1123:
		return catgets(_ttcatd, 2, 36, "ISAM Locale/LANG mismatch");
	      default:
		// NetISAM can also return system errno values.
		return strerror(an_errno);
	}
}

