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
 *+SNOTICE
 *
 *	$XConsortium: Common.h /main/4 1996/04/21 19:44:30 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
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
 *+ENOTICE
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <unistd.h>
#include <nl_types.h>

// XPG3 compatible. NL_CAT_LOCALE is set to non-zero in XPG4. Use NL_CAT_LOCALE
// for all catopen() calls.
#if defined(sun) && (_XOPEN_VERSION == 3) 
#undef NL_CAT_LOCALE
#define NL_CAT_LOCALE 0

// If NL_CAT_LOCALE is not defined in other platforms, set it to 0
#elif !defined(NL_CAT_LOCALE)	
#define NL_CAT_LOCALE 0
#endif

#endif   // _COMMON_H
