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
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * $XConsortium: SmProperty.h /main/2 1996/02/08 11:28:28 barstow $
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmProperty.h
 **
 **  Project:     DT Session Manager (dtsession)
 **
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _smproperty_h
#define _smproperty_h

Status GetStandardProperties(
        Window 			window,
	int			screen,
        int 			*argc,			/* RETURNED */
        char 			***argv,		/* RETURNED */
        char 			**clientMachine,	/* RETURNED */
	Boolean 		*xsmpClient);		/* RETURNED */

#endif /*_smproperty_h */
