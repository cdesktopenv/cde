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
/* $XConsortium: MainWin.h /main/4 1995/10/30 13:11:00 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        MainWin.h
 **
 **   Project:     DT 3.0
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _mainwin_h
#define _mainwin_h

/* external variable definitions */

extern char * _dt_version;

/* External Interface */


extern void init_mainWindow( Widget shell) ;
extern void callCancels( void ) ;
extern void activateCB_exitBtn( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern void restoreMain( Widget shell, XrmDatabase db) ;
extern void saveMain( int fd) ;


#endif /* _mainwin_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
