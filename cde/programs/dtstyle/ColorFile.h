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
/* $XConsortium: ColorFile.h /main/4 1995/10/30 13:08:30 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ColorFile.h
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
#ifndef _colorfile_h
#define _colorfile_h

/* External variables */

#define DT_PAL_DIR  "/.dt/palettes/"
/* 
extern char *DT_PAL_DIR = "/.dt/palettes/"; 
*/

/* External Interface */


extern void ReadPalette( char *directory, 
			 char *in_filename,
#if NeedWidePrototypes
			 int length );
#else
			 short length );
#endif
extern void AddSystemPath( void );
extern void AddDirectories( char *text );
extern void AddHomePath( void );
#if NeedWidePrototypes
extern Boolean ReadPaletteLoop( int startup );
#else
extern Boolean ReadPaletteLoop( Boolean startup );
#endif
extern void BackdropDialog( Widget parent) ;
extern void CheckWorkspace( void ) ;
extern void restoreBackdrop( Widget shell, XrmDatabase db) ;
extern void saveBackdrop( int fd) ;
extern void CheckFileType( void );


#endif /* _colorfile_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
