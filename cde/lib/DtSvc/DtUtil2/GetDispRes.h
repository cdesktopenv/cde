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
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/****************************<+>*************************************
 **
 **   File:     EnvControl.h
 **
 **   RCS:	$XConsortium: GetDispRes.h /main/4 1995/10/26 15:22:11 rswiston $
 **   Project:  HP DT Runtime Library
 **
 **   Description: Defines structures, and parameters used
 **                for communication with the environment
 **
 **   (c) Copyright 1992 by Hewlett-Packard Company
 **
 ****************************<+>*************************************/

/********    Function Declarations    ********/

int _DtGetDisplayResolution(
                        Display *disp,
                        int screen) ;

/********    End Function Declarations    ********/

/**********************************************************************
 * Resolution threshold values (width of screen in pixels) 
 **********************************************************************/

#define   _DT_HIGH_RES_MIN	       1176
#define	  _DT_MED_RES_MIN		851
#define	  _DT_LOW_RES_MIN		512

/**********************************************************************
 * Resolution types of a given screen
 **********************************************************************/
#define   NO_RES_DISPLAY        0
#define   LOW_RES_DISPLAY       1
#define   VGA_RES_DISPLAY       2
#define   MED_RES_DISPLAY       3
#define   HIGH_RES_DISPLAY      4
#define   ALL_RES_DISPLAY       5

/****************************        eof       **********************/
