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
/*****************************************************************************
 *
 *   File:         PopupMenu.h
 *
 *   Project:      CDE
 *
 *   Description:  This file contains defines and declarations needed
 *                 by PopupMenu.c and UI.c
 *
 *
 ****************************************************************************/

#ifndef _popup_menu_h
#define _popup_menu_h

#include "DataBaseLoad.h"


extern void CreatePopupMenu ( Widget );
extern void PostPopupMenu ( Widget, XtPointer, XEvent * );
extern void CreateWorkspacePopupMenu ( Widget , SwitchData *);
extern void PostWorkspacePopupMenu ( Widget, XtPointer, XEvent * );


#endif /* _popup_menu_h */
/*  DON"T ADD ANYTHING AFTER THIS #endif  */
