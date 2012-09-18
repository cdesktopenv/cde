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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: MultiView.c /main/5 1996/10/15 10:08:30 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           MultiView.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Contains the shared code for managing multiple toplevel
 *                   main application window.
 *
 *   FUNCTIONS: CreateMenuBtn
 *		_DtCreateMenuSystem
 *		_DtGlobalRestoreMenuSensitivity
 *		_DtGlobalSetMenuSensitivity
 *		_DtGlobalUpdateMenuItemState
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

 

#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <time.h>

#include <Xm/XmP.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleBG.h>
#include <Xm/MenuShellP.h>

#include <X11/ShellP.h>

#include <Dt/DtP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Connect.h>

#include <Xm/XmPrivate.h> /* _XmGetActiveTopLevelMenu */

#include "Encaps.h"
#include "MultiView.h"


/********    Static Function Declarations    ********/

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
static Widget CreateMenuBtn(
                        Widget parent,
                        Boolean isPushButton,
                        MenuDesc * menuDesc,
                        int btnIndex) ;

/********    End Static Function Declarations    ********/

extern Widget * findBtn;
extern Widget * change_directoryBtn;
extern Widget * preferencesBtn;
extern Widget * defaultEnvBtn;
extern Widget * filterBtn;

Widget
_DtCreateMenuSystem(
        Widget parent,
        char * menuBarName,
        XtCallbackProc helpCallback,
        XtPointer helpData,
        Boolean createSharedComponents,
        MenuDesc * menuDesc,
        int numMenuComponents,
        void (*getMenuDataProc)(),
        void (*setMenuSensitivity)(),
        void (*restoreMenuSensitivity)() )

{
   int n;
   int i;
   Arg args[20];
   Widget menubar;
   Widget lastCascadeBtn = NULL;
   Widget lastMenuPane = NULL;
   char lastMenuPaneType = MENU_PANE;
   Boolean firstMenuPane = True;
   Widget child;
   XmString labelString;
   int pulldownIndex = 1;
   int menuPaneIndex = 1;
   int menuBtnIndex = 1;

   /* Use our own menu control functions, if none specified */
   if (setMenuSensitivity == NULL)
      setMenuSensitivity = _DtGlobalSetMenuSensitivity;
   if (restoreMenuSensitivity == NULL)
      restoreMenuSensitivity = _DtGlobalRestoreMenuSensitivity;

   /*  Create the pulldown menu  */
   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL);	n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
   menubar = XmCreateMenuBar(parent, menuBarName, args, n);
   XtManageChild(menubar);
   if (helpCallback)
      XtAddCallback(menubar, XmNhelpCallback, helpCallback, helpData);

   for (i = 0; i < numMenuComponents; i++)
   {
      n = 0;

      switch (menuDesc[i].type)
      {
         case MENU_PULLDOWN_BUTTON:
         {
            labelString = XmStringCreateLocalized(menuDesc[i].label);
            XtSetArg(args[n], XmNlabelString, labelString);   n++;
            if (menuDesc[i].mnemonic)
            {
               XtSetArg(args[n], XmNmnemonic, menuDesc[i].mnemonic[0]);   n++;
            }
            XtSetArg(args[n], XmNmarginWidth, 6);   n++;
            child = XmCreateCascadeButtonGadget(menubar, menuDesc[i].name, 
                                                args, n);
            if (menuDesc[i].helpCallback)
            {
               XtAddCallback(child, XmNhelpCallback, 
                             menuDesc[i].helpCallback, menuDesc[i].helpData);
            }
            if (menuDesc[i].isHelpBtn)
            {
               XtSetArg(args[0], XmNmenuHelpWidget, child);
               XtSetValues(menubar, args, 1);
            }
            XtManageChild(child);
            XmStringFree(labelString);
            lastCascadeBtn = menuDesc[i].widget = child;
            pulldownIndex++;
            break;
         }

         case SHARED_MENU_PANE:
         case MENU_PANE:
         {
            if (createSharedComponents || (menuDesc[i].type == MENU_PANE))
            {
               child = XmCreatePulldownMenu(menubar, menuDesc[i].name, NULL, 0);

               if (firstMenuPane)
               {
                  XtAddCallback(XtParent(child), XmNpopupCallback, 
                                setMenuSensitivity, 
                                (XtPointer)getMenuDataProc);
                  XtAddCallback(XtParent(child), XmNpopdownCallback, 
                                restoreMenuSensitivity, 
                                (XtPointer)getMenuDataProc);
                  firstMenuPane = False;
               }

               if (menuDesc[i].helpCallback)
               {
                  XtAddCallback(child, XmNhelpCallback, 
                             menuDesc[i].helpCallback, menuDesc[i].helpData);
               }

               menuDesc[i].widget = child;
            }

            lastMenuPane = menuDesc[i].widget;
            lastMenuPaneType = menuDesc[i].type;
            XtSetArg(args[0], XmNsubMenuId, lastMenuPane);
            XtSetValues(lastCascadeBtn, args, 1);
            menuBtnIndex = 1;
            menuPaneIndex++;
            break;
         }

         case MENU_TOGGLE_BUTTON:
         case MENU_BUTTON:
         {
            /* Need to always make btns for non-shared menupanes */
            if ((createSharedComponents) || (lastMenuPaneType == MENU_PANE))
            {
               child = CreateMenuBtn (lastMenuPane, 
                                      (menuDesc[i].type == MENU_BUTTON),
                                      menuDesc + i, menuBtnIndex);
               XtManageChild(child);
               menuDesc[i].widget = child;
            }
            menuBtnIndex++;
            break;
         }

         case MENU_SEPARATOR:
         {
            /* Need to always make components for non-shared menupanes */
            if ((createSharedComponents) || (lastMenuPaneType == MENU_PANE))
            {
               child = XmCreateSeparatorGadget(lastMenuPane, menuDesc[i].name, 
                                               NULL, 0);
               XtManageChild(child);
               menuDesc[i].widget = child;
            }
            break;
         }
      }
   }


   return(menubar);
}



/* ARGSUSED */
void
_DtGlobalSetMenuSensitivity(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )

{
   Arg menuPaneArgs[1];
   Widget mbar;
   XmMenuShellWidget mshell = (XmMenuShellWidget) w;
   void (*getMenuData)() = (void(*)())client_data;
   XtPointer mainRecord;
   MenuDesc * menuDesc;
   int menuItemCount;
   unsigned int * globalMenuStates;
   unsigned int * viewMenuStates;
   int i;

   _XmGetActiveTopLevelMenu(mshell->composite.children[0], &mbar);
   (*getMenuData)(mbar, &mainRecord, &menuDesc, &menuItemCount,
                  &globalMenuStates, &viewMenuStates);
   XtSetArg(menuPaneArgs[0], XmNuserData, mainRecord);

   for (i = 0; i < menuItemCount; i++)
   {
      switch (menuDesc[i].type)
      {
         case SHARED_MENU_PANE:
         case MENU_PANE:
         {
            /* Need to do this to get help working */
            XtSetValues(menuDesc[i].widget, menuPaneArgs, 1);
            break;
         }

         case MENU_BUTTON:
         {
            if ((menuDesc[i].maskBit) &&
               ((*globalMenuStates & menuDesc[i].maskBit) != 
                (*viewMenuStates & menuDesc[i].maskBit)))
            {
               _DtGlobalUpdateMenuItemState(menuDesc[i].widget, 
                                         menuDesc[i].maskBit, 
                                         *viewMenuStates & menuDesc[i].maskBit,
                                         globalMenuStates);
            }
            break;
         }
      }
   }
}


/* ARGSUSED */
void
_DtGlobalRestoreMenuSensitivity(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )

{
   Widget mbar;
   XmMenuShellWidget mshell = (XmMenuShellWidget) w;
   void (*getMenuData)() = (void (*)())client_data;
   XtPointer mainRecord;
   MenuDesc * menuDesc;
   int menuItemCount;
   unsigned int * globalMenuStates;
   unsigned int * viewMenuStates;
   int i;

   _XmGetActiveTopLevelMenu(mshell->composite.children[0], &mbar);
   (*getMenuData)(mbar, &mainRecord, &menuDesc, &menuItemCount,
                  &globalMenuStates, &viewMenuStates);

   for (i = 0; i < menuItemCount; i++)
   {
      if (menuDesc[i].type == MENU_BUTTON)
      {
         if ((menuDesc[i].maskBit) && 
             ((*globalMenuStates & menuDesc[i].maskBit) == 0))
         {
            _DtGlobalUpdateMenuItemState(menuDesc[i].widget, 
                                      menuDesc[i].maskBit, 
                                      True,
                                      globalMenuStates);
         }
      }
   }
}


void
_DtGlobalUpdateMenuItemState(
        Widget btn,
        unsigned int mask,
        unsigned int on,
        unsigned int * globalMenuStates )

{
   if (on != 0)
   {
      XtSetSensitive(btn, True);
      *globalMenuStates |= mask;
   }
   else
   {
      if(btn != *findBtn             &&
         btn != *change_directoryBtn && 
         btn != *preferencesBtn      && 
         btn != *defaultEnvBtn       && 
         btn != *filterBtn               ) 
      {
        XtSetSensitive(btn, False);
        *globalMenuStates &= ~mask;
      }
   }
}


static Widget
CreateMenuBtn (
   Widget parent,
   Boolean isPushButton,
   MenuDesc * menuDesc,
   int btnIndex )
{
   int n;
   Arg args[10];
   Widget child;
   XmString labelString;

   n = 0;
   labelString = XmStringCreateLocalized(menuDesc->label);
   XtSetArg(args[n], XmNlabelString, labelString);		n++;
   XtSetArg(args[n], XmNmnemonic, menuDesc->mnemonic[0]);	n++;
   if (isPushButton)
      child = XmCreatePushButtonGadget(parent, menuDesc->name, args, n);
   else
   {
      XtSetArg(args[n], XmNvisibleWhenOff,TRUE);n++;
      child = XmCreateToggleButtonGadget(parent, menuDesc->name, args, n);
   }
   if (menuDesc->helpCallback)
   {
      XtAddCallback(child, XmNhelpCallback, menuDesc->helpCallback, 
                    menuDesc->helpData);
   }
   if (menuDesc->activateCallback)
   {
      if (isPushButton)
      {
         XtAddCallback(child, XmNactivateCallback, menuDesc->activateCallback, 
                       menuDesc->activateData);
      }
      else
      {
         XtAddCallback(child, XmNvalueChangedCallback, 
                       menuDesc->activateCallback, menuDesc->activateData);
      }
   }
   XmStringFree(labelString);
   return (child);
}
