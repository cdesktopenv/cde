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
/* $XConsortium: MultiView.h /main/4 1995/11/02 14:42:56 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           MultiView.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the shared multi-view functions.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _MultiView_h
#define _MultiView_h

typedef struct {
   char type;
   Boolean isHelpBtn;
   String name;
   String label;
   String mnemonic;
   XtPointer helpData;
   XtCallbackProc helpCallback;
   XtPointer activateData;
   XtCallbackProc activateCallback;
   unsigned int maskBit;
   Widget widget;
} MenuDesc;


/********    Public Function Declarations    ********/

extern Widget _DtCreateMenuSystem(
                        Widget parent ,
                        char * menuBarName,
                        XtCallbackProc helpCallback,
                        XtPointer helpData,
                        Boolean createSharedComponents,
                        MenuDesc * menuDesc,
                        int numMenuComponents,
                        void (*getMenuDataProc)(),
                        void (*setMenuSensitivity)(),
                        void (*restoreMenuSensitivity)()) ;
extern void _DtGlobalUpdateMenuItemState(
                        Widget btn,
                        unsigned int mask,
                        unsigned int on,
                        unsigned int * globalMenuStates) ;
extern void _DtGlobalSetMenuSensitivity(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern void _DtGlobalRestoreMenuSensitivity(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;

/********    End Public Function Declarations    ********/


/* Defines for menu components */
#define MENU_PANE            0
#define MENU_PULLDOWN_BUTTON 1
#define MENU_BUTTON          2
#define MENU_SEPARATOR       3
#define SHARED_MENU_PANE     4
#define MENU_TOGGLE_BUTTON   5


#endif /* _MultiView_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
