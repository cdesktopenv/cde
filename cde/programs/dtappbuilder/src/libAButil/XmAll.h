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
 *	$XConsortium: XmAll.h /main/4 1995/11/06 18:49:40 rswiston $
 *
 * @(#)XmAll.h	1.5 01 Feb 1994	cde_app_builder/src/libAButil
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

#ifndef _AB_XMALL_H_
#define _AB_XMALL_H_
/*
 * XmAll.h - include standard header files, adding symbols for AB
 * 		files.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <Xm/XmAll.h>

/*
 * Make sure we don't redefine types defined in standard X include files
 */
#define _AB_BOOL_DEFINED_
#define _AB_BYTE_DEFINED_

#endif /* _AB_XMALL_H_ */
