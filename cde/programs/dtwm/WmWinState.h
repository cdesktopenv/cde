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
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.1
*/ 
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void SetClientState (ClientData *pCD, int newState, Time setTime);
extern void SetClientStateWithEventMask (ClientData *pCD, int newState, Time setTime, unsigned int event_mask);
extern void ConfigureNewState (ClientData *pcd);
extern void SetClientWMState (ClientData *pCD, int wmState, int mwmState);
extern void MapClientWindows (ClientData *pCD);
extern void ShowIconForMinimizedClient (WmWorkspaceData *pWS, ClientData *pCD);
extern void ShowAllIconsForMinimizedClient (ClientData *pCD);
extern void SlideSubpanelBackIn (ClientData *pCD, Widget wSubpanel);
