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
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.2
*/ 
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void PlaceWindowInteractively (ClientData *pcd);
extern void DoPlacement (ClientData *pcd);
extern void SetupPlacement (ClientData *pcd);
extern void HandlePlacementKeyEvent (ClientData *pcd, XKeyEvent *pev);
extern void HandlePlacementButtonEvent (XButtonEvent *pev);
extern void HandlePlacementMotionEvent (ClientData *pcd, XMotionEvent *pev);
extern void StartInteractiveSizing (ClientData *pcd, Time time);
extern Bool IsRepeatedKeyEvent (Display *dpy, XEvent *pEvent, char *pOldEvent);
