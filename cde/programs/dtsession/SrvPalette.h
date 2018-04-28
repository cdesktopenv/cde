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
/* $XConsortium: SrvPalette.h /main/4 1995/10/30 09:40:38 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SrvPalette.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Session Restoration for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _srvpalette_h
#define _srvpalette_h
 
/* 
 *  #include statements 
 */


/* 
 *  #define statements 
 */

/* 
 * typedef statements 
 */
/*
 *  External variables  
 */


/*  
 *  External Interface  
 */


extern int InitializeDtcolor( Display *, short ) ;
extern int CheckMonitor( Display *) ;


#endif /*_srvpalette_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
