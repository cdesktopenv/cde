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
 * File:         bms.h $XConsortium: bms.h /main/4 1996/06/21 17:20:07 ageorge $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _bms_h
#define _bms_h

#ifdef SVR4
#include <netdb.h>		/* MAXHOSTNAMELEN */
#else
#include <sys/param.h>		/* MAXHOSTNAMELEN */
#endif

/* WARNINGS: 						      */
/* 							      */
/*   1) <bms/sbport.h> MUST included before this file.         */
/*   2) Any <X11/...> includes MUST come before this file.    */
/* ---------------------------------------------------------- */

#ifndef _XtIntrinsic_h		     /* Define "Boolean" if not already */
   typedef char Boolean;	     /* defined from <X11/Intrinsic.h>  */
#endif

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 

extern char *XeToolClass;

extern XeString         XeProgName;                  /* From noXinit.c     */

extern XeString				/* Returns Malloc'ed memory */
XeSBTempPath
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeString);
#else
   ();
#endif

/* DON'T ADD STUFF AFTER THIS #endif */
#endif /* _bms_h */

