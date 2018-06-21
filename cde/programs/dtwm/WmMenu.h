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
#ifndef _WM_MENU_H
#define _WM_MENU_H

/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.2
*/ 
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

#define MAKE_MENU(pSD, pCD, mName, iC, aC, moreItems, sysMenu) \
	MakeMenu(pSD, mName, iC, aC, moreItems, sysMenu)
#define CREATE_MENU_WIDGET(pSD, pCD, mName, parent, fTLP, tSpec, moreItems) \
	CreateMenuWidget(pSD, mName, parent, fTLP, tSpec, moreItems)

extern void ActivateCallback (Widget w, caddr_t client_data, 
			      caddr_t call_data);
extern Widget CreateMenuWidget (WmScreenData *pSD,
				String menuName, Widget parent,
				Boolean fTopLevelPane, MenuSpec *topMenuSpec,
				MenuItem *moreMenuItems);
extern void FreeCustomMenuSpec (MenuSpec *menuSpec);
extern void MWarning (char *format, char *message);
extern MenuSpec *MakeMenu (WmScreenData *pSD,
			   String menuName, 
			   Context initialContext, Context accelContext, 
			   MenuItem *moreMenuItems, Boolean fSystemMenu);
extern void PostMenu (MenuSpec *menuSpec, ClientData *pCD, int x, int y, 
		      unsigned int button, Context newContext, long flags, 
		      XEvent *passedInEvent);
extern void TraversalOff (MenuSpec *menuSpec);
extern void TraversalOn (MenuSpec *menuSpec);
extern void UnpostMenu (MenuSpec *menuSpec);
extern MenuSpec *DuplicateMenuSpec (MenuSpec *menuSpec);

#endif /* _WM_MENU_H */
