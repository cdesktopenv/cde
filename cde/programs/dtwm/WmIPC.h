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
 * (c) Copyright 1987, 1988, 1989, 1990, 1992 HEWLETT-PACKARD COMPANY 
 * ALL RIGHTS RESERVED 
*/ 

/********    Public Function Declarations    ********/

extern void dtInitialize( 
                        char *program_name,
                        XtAppContext appContext) ;
extern void dtInitializeMessaging( Widget ) ;
extern void dtCloseIPC( void ) ;
extern void dtReadyNotification( void ) ;
extern void WmStopWaiting( void ) ;
extern void dtSendWorkspaceModifyNotification(
	WmScreenData 		*pSD, 
	Atom 			aWs, 
	int 			iType);
extern void dtSendMarqueeSelectionNotification(
	WmScreenData *pSD,
	int	type,
	Position x,
	Position y,
	Dimension width,
	Dimension height);

/********    End Public Function Declarations    ********/

/****************************   eof    ***************************/
