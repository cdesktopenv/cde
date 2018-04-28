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
/* $TOG: SmGlobals.h /main/10 1997/03/20 16:04:11 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmGlobals.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   the global processing routines for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smglobals_h
#define _smglobals_h
 
/* 
 *  #include statements 
 */

/* 
 *  #define statements 
 */
#define SM_DISPLAY_HOME		"display-home"
#define SM_DISPLAY_CURRENT	"display-current"

#define SmNewString(str) strdup(str)

/* 
 * typedef statements 
 */

/*
 *  External variables  
 */

/*  
 *  External Interface  
 */
extern void WaitChildDeath(int i);
extern void InitSMGlobals( void ) ;
extern int SetRestorePath( unsigned int , char **) ;
extern int SetSavePath( Boolean , int ) ;
extern int SetFontSavePath(char *) ;
extern void InitNlsStrings( void ) ;
extern void MoveDirectory( char *, char *, Boolean) ;
extern void SetSystemReady();
extern void SmExit ( int exitStatus);
extern void UndoSetSavePath ( );
extern void SetSIGPIPEToDefault ( );
extern void FixPath ( char * the1stPath );

#endif /*_smglobals_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
