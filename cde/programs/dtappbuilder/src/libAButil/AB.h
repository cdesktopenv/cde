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

/*
 *	$XConsortium: AB.h /main/4 1995/11/06 18:48:43 rswiston $
 *
 * @(#)AB.h	1.1 11 Feb 1994	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _ABUTIL_AB_H_
#define _ABUTIL_AB_H_
/*
 * <ab_private/AB.h>
 *
 * This file should be included by ALL app builder source files!
 * And it should be included before any other ab include files 
 * ( <ab_private/x.h>, <ab/x.h>, "x.h" ).
 *
 * It sets DEBUG parameters, and affects the entire product
 */


/*
 * Turn on POSIX compliance.  This mostly affect system header files.
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif


/*
 * Set the DEBUG compile-time parameter to be exactly the opposite of
 * NDEBUG
 */
#ifndef NDEBUG
    #ifndef DEBUG
        #define DEBUG 1
    #endif
#else
    #undef DEBUG
#endif


#endif /* _ABUTIL_AB_H_ */
