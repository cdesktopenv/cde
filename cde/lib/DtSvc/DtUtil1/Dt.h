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
/* $TOG: Dt.h /main/12 1999/10/18 14:49:57 samborn $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993-1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _Dt_Dt_h
#define _Dt_Dt_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* CDE Version information */

#define DtVERSION     		2
#define DtREVISION    		2
#define DtUPDATE_LEVEL    	3

#define DtVERSION_NUMBER	(DtVERSION * 10000 +  \
				DtREVISION * 100 + \
				DtUPDATE_LEVEL)

#define DtVERSION_STRING "CDE Version 2.2.3"


/* 
 * CDE Version information 
 */

externalref const int DtVersion;
externalref const char *DtVersionString;


/*
 * Functions
 */

extern Boolean DtInitialize(
		Display		*display,
		Widget		widget,
		char		*name,
		char		*tool_class);

extern Boolean DtAppInitialize(
		XtAppContext	app_context,
		Display		*display,
		Widget		widget,
		char		*name,
		char		*tool_class);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_Dt_h */
