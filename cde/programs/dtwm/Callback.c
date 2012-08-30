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
/* $XConsortium: Callback.c /main/10 1996/10/30 11:14:26 drk $ */
/*****************************************************************************
 *
 *   File:         Callback.c
 *
 *   Project:	    CDE
 *
 *   Description:  This file contains the user interface behavior processing
 *                 functions for the CDE front panel
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 *
 ****************************************************************************/

#include <sys/param.h>
#include <sys/stat.h>

#include <Dt/DtP.h>
#include <Dt/DbReader.h>
#include "PanelSP.h"
#include <Dt/Control.h>
#include <Dt/ControlP.h>
#include <Dt/IconFile.h>
#include <Dt/Icon.h>
#include <Dt/Dnd.h>
#include <Dt/Dts.h>
#include <Dt/Action.h>
#include <Dt/DtNlUtils.h>
#include <Dt/WsmP.h>
#include <Dt/WsmM.h>
#include <Dt/MacrosP.h>
#include <Dt/SharedProcs.h>

#include <Xm/Form.h>
#include <Xm/ToggleBG.h>
#include <Xm/AtomMgr.h>
#include <Xm/TextF.h>

#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "DataBaseLoad.h"
#include "UI.h"

#include "WmGlobal.h"
#include "WmHelp.h"
#include "WmResNames.h"
#include "WmResParse.h"
#include "WmFunction.h"


/************************************************************************
 *
 *  External and static function declarations.
 *
 ************************************************************************/

extern XtPointer _XmStringUngenerate(XmString, XmStringTag,
				     XmTextType, XmTextType);

extern void SubpanelControlCreate (SubpanelData *, ControlData *, ControlData *,
                                   Widget, Boolean, Boolean);
extern void WmSubpanelPosted (Display *, Window);
extern int PushRecallGetData (char *);
extern void SwitchButtonCreate (SwitchData *, Boolean);
extern void AddControlActionList (ControlData *);

void ArrowCB (Widget, XtPointer, XtPointer); 
void HandleInputCB (Widget, XtPointer, XtPointer); 
void MinimizeInputCB (Widget, XtPointer, XtPointer); 
void MenuInputCB (Widget, XtPointer, XtPointer); 
void SwitchButtonCB (Widget, XtPointer, XtPointer);
void PushCB (Widget, XtPointer, XtPointer);
void SubpanelUnmapCB (Widget, XtPointer, XtPointer); 
void SubpanelTornEventHandler (Widget, XtPointer, XEvent *, Boolean *);
Boolean CheckControlTypeFile (ControlData *);


static void SwitchRenameCancel (Widget, XEvent *, String *, Cardinal *);
static void SwitchRenameCB (Widget, XtPointer, XtPointer);
static void GetValuesFromDataType(char *, char *, SubpanelData *,
			          ElementValue **);

void SwitchRenameLabel (Widget, BoxData *);
void DropCB (Widget, XtPointer, XtPointer); 
void TransferDropCB (Widget, XtPointer, XtPointer); 
void CustomizeDropCB (Widget, XtPointer, XtPointer);
void CustomizeTransferDropCB (Widget, XtPointer, XtPointer); 





/*  Translations and action definitions	                        */
/*	These are used specifically for the text field overlay  */
/*	on the switch button for renaming the workspace.  They  */
/*	are necessary for handling escape key processing.       */

static char translations_escape[] = "<Key>osfCancel:Escape()";

static XtActionsRec action_table[] = {
   {"Escape", SwitchRenameCancel},
};



/************************************************************************
 *
 *  File local globals.
 *
 ************************************************************************/

extern String post_arrow_image;
extern String unpost_arrow_image;
extern String post_monitor_arrow_image;
extern String unpost_monitor_arrow_image;
extern String blank_arrow_image;
extern String dropzone_image;
extern String indicator_on_image;
extern String indicator_off_image;



/************************************************************************
 *
 *  CallWmFunction
 *	Call the specified WmFunction function with appropriate args
 *
 ************************************************************************/


void
CallWmFunction (WmFunction wm_function,
		String     args,
		Widget	   client,
		XEvent   * event)


{
   WmGlobalData * wm_global_data = (WmGlobalData *) panel.global_data;
   ClientData   * wm_client_data = NULL;
   Display      * display = wm_global_data->display;
   Window	  client_window = XtWindow(client);

   /*
    * Find the window manager client data for this client
    */

   XFindContext (display, client_window, wm_global_data->windowContextType,
                    (XtPointer) &wm_client_data);

   if (wm_client_data == NULL)
      return;

   /*  
    * Execute the window manager function
    */

   wm_function (args, wm_client_data, event);
}

/************************************************************************
 *
 *  ArrowCB
 *	Process the callback on the subpanel posting/unposting arrow
 *
 ************************************************************************/


void
ArrowCB (Widget    w,
         XtPointer client_data,
         XtPointer call_data) 


{
   ControlData  * control_data = (ControlData *) client_data;
   SubpanelData * subpanel_data = control_data->subpanel_data;

   Arg al[2];
   int ac;


   /*  If the subpanel's shell is not managed, this is a request to post  */
   /*  the subpanel.  Otherwise it is an unpost request.                  */
   /*  Reset the arrow image after the post or unpost.                    */
   
   if (!XtIsManaged (subpanel_data->shell))
   {
      Position x = XtX (control_data->arrow);
      Position y = XtY (control_data->arrow);


      XtSetMappedWhenManaged (subpanel_data->shell, False);
      XtManageChild (subpanel_data->form);

      XtTranslateCoords (w, 0, 0, &x, &y);
      x--;
      XtSetArg (al[0], XmNx, x);
      XtSetValues (subpanel_data->form, al, 1);

      XtManageChild (subpanel_data->shell);
      XtSetMappedWhenManaged (subpanel_data->shell, True);

      ac = 0;
      XtSetArg (al[ac], XmNimageName, unpost_arrow_image);	ac++;
      XtSetValues (w, al, ac);
   }
   else
   {
      /*  Execute the window manager function to unpost the subpanel  */

      CallWmFunction (F_Kill, NULL, subpanel_data->shell, NULL);
   }
}




/************************************************************************
 *
 *  HandleInputTranslations
 *	Return translation table suitable for HandleInputCB
 *
 ************************************************************************/


XtTranslations
HandleInputTranslations(void)

{
   static XtTranslations handle_translations;

   if (handle_translations == NULL)
   {
      handle_translations = XtParseTranslationTable(
	"<BtnDown>:    DrawingAreaInput() ManagerGadgetArm()\n\
         <BtnUp>:      DrawingAreaInput() ManagerGadgetActivate()\n\
         <Btn1Motion>: DrawingAreaInput() ManagerGadgetButtonMotion()\n\
         <Btn2Motion>: DrawingAreaInput() ManagerGadgetButtonMotion()");
   }
   return handle_translations;
}

/************************************************************************
 *
 *  HandleInputCB
 *	Process button events on the frontpanel handles.
 *
 ************************************************************************/


void
HandleInputCB (Widget    w,
               XtPointer client_data,
               XtPointer call_data)


{
   XmAnyCallbackStruct * callback;

   XEvent       * event;
   XButtonEvent * buttonEvent;
   XMotionEvent * motionEvent;

   callback = (XmAnyCallbackStruct *) call_data;
   event = (XEvent *) callback->event;
   buttonEvent = (XButtonEvent *) event;
   motionEvent = (XMotionEvent *) event;

   if (event->type == ButtonPress)
   {
      if (buttonEvent->button == wmGD.bMenuButton)
      {
         XUngrabPointer (XtDisplay (w), buttonEvent->time);
         XSync (XtDisplay (w), FALSE);
	 CallWmFunction (F_Post_FpMenu, NULL, panel.shell, event);
      }
      else
      {
	 wmGD.preMove = True;
	 wmGD.preMoveX = buttonEvent->x_root;
	 wmGD.preMoveY = buttonEvent->y_root;

	 if (buttonEvent->button == Button1)
         {
	    CallWmFunction (F_Raise, NULL, panel.shell, event);
            XSync (DISPLAY, FALSE);
            XmUpdateDisplay (panel.shell);
         }
      }
   }
   else if (event->type == ButtonRelease)
   {
      wmGD.preMove = False;
   }
   else if (event->type == MotionNotify && wmGD.preMove)
   {
      int diffX, diffY;
      
      diffX = motionEvent->x_root - wmGD.preMoveX;
      if (diffX < 0) diffX = -diffX;
      diffY = motionEvent->y_root - wmGD.preMoveY;
      if (diffY < 0) diffY = -diffY;

      if (diffX >= wmGD.moveThreshold || diffY >= wmGD.moveThreshold)
      {
         XUngrabPointer (XtDisplay (w), motionEvent->time);
         XSync (XtDisplay (w), FALSE);
         CallWmFunction (F_Move, NULL, panel.shell, event);
         wmGD.preMove = False;
      }
   }
}




/************************************************************************
 *
 *  MinimizeInputCB
 *	Process button events on the frontpanel minimize button.
 *
 ************************************************************************/


void
MinimizeInputCB (Widget    w,
                 XtPointer client_data,
                 XtPointer call_data)


{
   XmAnyCallbackStruct * callback;

   XEvent       * event;
   XButtonEvent * bevent;

   extern Pixmap minimize_normal_pixmap;
   extern Pixmap minimize_selected_pixmap;

   Arg al[1];
   

   callback = (XmAnyCallbackStruct *) call_data;
   event = (XEvent *) callback->event;
   bevent = (XButtonEvent *) event;

   /*  On the BSelect button press, change the image to the selected
    *  minimize image.  On the release, check to see if the release
    *  occured within the minimize button before minimizing.
    *  On BMenu button press, post the front panel menu.
    */
   
   if (event->type == ButtonPress) 
   {
      if (bevent->button == Button1)
      {
         XtSetArg (al[0], XmNbackgroundPixmap, minimize_selected_pixmap);
         XtSetValues (w, al, 1);
      }
      else if (bevent->button == wmGD.bMenuButton)
      {
         XUngrabPointer (XtDisplay (w), bevent->time);
         XSync (XtDisplay (w), FALSE);
         CallWmFunction (F_Post_FpMenu, NULL, panel.shell, event);
      }
   }
   else if (event->type == ButtonRelease && bevent->button == Button1)
   {
      if (bevent->x >= 0 && bevent->x <= (Position) XtWidth(w) &&
          bevent->y >= 0 && bevent->y <= (Position) XtHeight(w))
      {
         CallWmFunction (F_Minimize, NULL, panel.shell, event);
      }

      XtSetArg (al[0], XmNbackgroundPixmap, minimize_normal_pixmap);
      XtSetValues (w, al, 1);
   }
}




/************************************************************************
 *
 *  MenuInputCB
 *	Process button events on the front panel menu button.
 *
 ************************************************************************/


void
MenuInputCB (Widget    w,
             XtPointer client_data,
             XtPointer call_data)


{
   XmAnyCallbackStruct * callback;

   XEvent       * event;
   XButtonEvent * bevent;

   callback = (XmAnyCallbackStruct *) call_data;
   event = (XEvent *) callback->event;
   bevent = (XButtonEvent *) event;


   /*  
    * On BSelect or BMenu button press post the front panel system menu
    */

   if (event->type == ButtonPress && 
       (bevent->button == Button1 || bevent->button == wmGD.bMenuButton))
   {
      XUngrabPointer (XtDisplay (w), bevent->time);
      XSync (XtDisplay (w), FALSE);
      CallWmFunction (F_Post_FpMenu, NULL, panel.shell, event);
   }
}



/************************************************************************
 ************************************************************************
 
    The next block of functions are used for the switch button switching,
    renaming functionality, and add and delete of workspaces.
    
 ************************************************************************    
 ************************************************************************/

 

/************************************************************************
 *
 *  WorkspaceAdjustPanelPosition
 *      After a workspace button has been added or deleted, if the panel
 *      has changed sizes, reposition the panel to keep the same relative
 *      positioning.
 *
 *	Inputs: x - the original x coordinate of the panel
 *              y - the original y coordinate of the panel
 *              width - the original width of the panel
 *              height - the original height of the panel
 *
 ************************************************************************/


void
WorkspaceAdjustPanelPosition (Position x,
                              Position y,
                              Dimension width,
                              Dimension height)


{
   Dimension screen_width;
   Dimension screen_height;
   int       panel_center;
   Dimension new_width;
   Dimension new_height;
   Position  new_x;	       
   Position  new_y;	       

   Arg al[2];
   int ac;

   screen_width = WidthOfScreen (XtScreen (panel.shell));
   screen_height = HeightOfScreen (XtScreen (panel.shell));


   /*  Reposition the panel to keep it centered relative to where  */
   /*  it was positioned before the deletion of the button.        */
	 
   ac = 0;

   new_width = XtWidth (panel.shell);

   if (new_width != width || x + (Position) new_width > (Position) screen_width)
   {
      panel_center = x + width / 2 - 1;
      new_x = panel_center - new_width / 2 - 4;

      if (new_x < 0) 
         new_x = 0;
      else if (new_x + (Position) new_width > (Position) screen_width) 
         new_x = screen_width - new_width - 4;

      XtSetArg (al[ac], XmNx, new_x);	ac++;
   }


   /*  Keep the panel to the bottom of the screen, if it was there.  */
   /*  But make sure that it does not go below the bottom.           */

   new_height = XtHeight (panel.shell);

   if (new_height != height || (Dimension)(new_height + y - 1) > screen_height)
   {
      if (new_height < height && y > (Position)(screen_height / 2))
      {
         new_y = y + (Position)(height - new_height - 4);
         XtSetArg (al[ac], XmNy, new_y);	ac++;
      }
      else if ((Dimension)(new_height + y - 1) > screen_height)
      {
         new_y = (Position)(screen_height - new_height - 4);
         XtSetArg (al[ac], XmNy, new_y);	ac++;
      }
   }	 

   if (ac != 0)
      XtSetValues (panel.shell, al, ac);
}
 



/************************************************************************
 *
 *  WorkspaceModifyCB
 *	Called by the workspace manager API to send notification of 
 *	configuration changes to the workspace.  The types of changes
 *	processed by this function are workspace add, delete, and rename.
 *
 ************************************************************************/


void
WorkspaceModifyCB (Widget    w,
                   Atom      atom_name,
                   int       type,
                   XtPointer client_data)


{
   SwitchData         * switch_data = (SwitchData *) client_data;
   BoxData            * box_data = switch_data->box_data;
   DtWsmWorkspaceInfo * workspace_info;

   Position  x;
   Position  y;
   Dimension width;
   Dimension height;
   Dimension screen_width;

	 
   x = XtX (panel.shell);
   y = XtY (panel.shell);
   width = XtWidth (panel.shell);
   height = XtHeight (panel.shell);
   screen_width = WidthOfScreen (XtScreen (panel.shell));
   
   DtWsmGetWorkspaceInfo (XtDisplay (w), RootWindowOfScreen (XtScreen (w)),
                          atom_name, &workspace_info);

   
   switch (type)
   {
      /*  Increase the size of the switch names and button and call  */
      /*  the function to create the new workspace switch button     */

      case DtWSM_REASON_ADD:
      {
         int    switch_count;
	 Atom * atom_names;

         
         /*  Cancel workspace renaming  */

         if (XtIsManaged (box_data->switch_edit))
	    SwitchRenameCancel (box_data->switch_edit, NULL, NULL, NULL);


         DtWsmGetWorkspaceList (XtDisplay (switch_data->rc),
                               RootWindowOfScreen (XtScreen (switch_data->rc)),
                               &atom_names, &switch_count);

         XFree (switch_data->atom_names);
	 switch_data->atom_names = atom_names;

         switch_data->switch_count = switch_count;

         switch_data->switch_names =
            (char **) XtRealloc ((char *) switch_data->switch_names, 
                                 sizeof (char *) * switch_data->switch_count);

         switch_data->switch_names[switch_data->switch_count - 1] = 
            XtNewString (workspace_info->pchTitle);
	 
         switch_data->buttons =
            (Widget *) XtRealloc ((char *) switch_data->buttons,
                                 sizeof (Widget *) * switch_data->switch_count);

         SwitchButtonCreate (switch_data, True);


         /*  When adding a switch buttons, keep the row columns row count  */
	 /*  equal to the requested value with small number of buttons     */

         if (switch_data->switch_count <= (int)
	     (switch_data->element_values[SWITCH_NUMBER_OF_ROWS].parsed_value))
	 {
            Arg al[1];

            panel.switch_row_count = (int)
	      (switch_data->element_values[SWITCH_NUMBER_OF_ROWS].parsed_value);

            XtSetArg (al[0], XmNnumColumns, panel.switch_row_count);
            XtSetValues (switch_data->rc, al, 1);
         }


         /*  If the fp width is greater than the screen, increase the  */
	 /*  row count                                                 */
	 
         if (XtWidth (panel.shell) > screen_width)
	 {
            Arg al[1];
	    
            panel.switch_row_count++;

            XtSetArg (al[0], XmNnumColumns, panel.switch_row_count);
            XtSetValues (switch_data->rc, al, 1);
         }

         WorkspaceAdjustPanelPosition (x, y, width, height);
      }
      break;


      /*  Loop through the current set of atom names, comparing     */
      /*  them with the atom of the workspace to be deleted.  When  */
      /*  free up the associated data and readjust the name, button */
      /*  and atom lists.                                           */

      case DtWSM_REASON_DELETE:
      {
	 int i;
         int j;
         int    switch_count;
	 Atom * atom_names;
         Arg al[1];

         Dimension button_spacing;
         Dimension button_width;

         
         /*  Cancel workspace renaming  */

         if (XtIsManaged (box_data->switch_edit))
	    SwitchRenameCancel (box_data->switch_edit, NULL, NULL, NULL);


         DtWsmGetWorkspaceList (XtDisplay (switch_data->rc),
                                RootWindowOfScreen (XtScreen (switch_data->rc)),
                                &atom_names, &switch_count);

	 for (i = 0; i < switch_data->switch_count; i++)
         {
            if (atom_name == switch_data->atom_names[i])
	    {
               if (switch_data->active_switch > i)
	          switch_data->active_switch--;

               XtFree (switch_data->switch_names[i]);
	       XtDestroyWidget (switch_data->buttons[i]);
               XFree (switch_data->atom_names);
               switch_data->atom_names = atom_names;
               switch_data->switch_count = switch_count;

	       for (j = i; j < switch_data->switch_count; j++)
	       {
		  switch_data->switch_names[j] = switch_data->switch_names[j+1];
                  switch_data->buttons[j] = switch_data->buttons[j + 1];
	       }

               break;
            }
         }


         /*  When deleting a switch button, keep trying to decrease the  */
         /*  row count to the requested value.                           */

         if (panel.switch_row_count > (int)
              (switch_data->element_values[SWITCH_NUMBER_OF_ROWS].parsed_value))
         {
            button_width = XtWidth (switch_data->buttons[0]);
            XtSetArg (al[0], XmNspacing, &button_spacing);
            XtGetValues (switch_data->rc, al, 1);

            if ((int)(screen_width - (width + button_width + button_spacing)) >
                                                                              0)
            {
               panel.switch_row_count--;
               XtSetArg (al[0], XmNnumColumns, panel.switch_row_count);
               XtSetValues (switch_data->rc, al, 1);
            }
         }

         WorkspaceAdjustPanelPosition (x, y, width, height);

      }
      break;

         
      /*  Loop through the current set of atom names, comparing        */
      /*  them with the new atom.  When a match is found, reset the    */
      /*  the switch name to the new one and update the button label.  */

      case DtWSM_REASON_TITLE:
      {
         XmString toggle_string;

	 Arg al[2];
         int i;

	 for (i = 0; i < switch_data->switch_count; i++)
         {
            if (atom_name == switch_data->atom_names[i])
	    {
	       switch_data->switch_names[i] = 
	          XtNewString (workspace_info->pchTitle);

               toggle_string =
                  XmStringCreateLocalized (workspace_info->pchTitle);

               XtSetArg (al[0], XmNstring, toggle_string);
               XtSetValues (switch_data->buttons[i], al, 1);
               XmStringFree (toggle_string);

               XtSetArg (al[0], XmNiconName, workspace_info->pchTitle);
               XtSetArg (al[1], XmNtitle, workspace_info->pchTitle);
               XtSetValues (panel.shell, al, 2);

               break;
	    }
         }
      }
      break;


      /*  Loop through the set of switch atom names to find the one  */
      /*  that is now active.  Unset the old workspace button and    */
      /*  set the new one referenced by the atom.                    */

      case DtWSM_REASON_CURRENT:
      {
         int i;
	 Arg al[2];
   
         for (i = 0; i < switch_data->switch_count; i++)
         {
            if (switch_data->atom_names[i] == atom_name)
            {
	       XtSetArg (al[0], XmNiconName, switch_data->switch_names[i]); 
	       XtSetArg (al[1], XmNtitle, switch_data->switch_names[i]);
	       XtSetValues (panel.shell, al, 2);

               _DtIconSetState (switch_data->buttons[switch_data->active_switch], False, False);
               switch_data->active_switch = i;
               _DtIconSetState (switch_data->buttons[switch_data->active_switch], True, False);
               break;
            }
         }
      }
      break;
   }

   DtWsmFreeWorkspaceInfo (workspace_info);
} 
 


 
/************************************************************************
 *
 *  SwitchButtonCB
 *	Process callbacks on the switch buttons.  This function cause
 *	either the workspace switch to occur or for the switch button
 *	label to become editable.
 *
 ************************************************************************/


void
SwitchButtonCB (Widget    w,
                XtPointer client_data,
                XtPointer call_data) 


{
   DtControlCallbackStruct * callback = (DtControlCallbackStruct *) call_data;
   XEvent * event = callback->event;

   Widget        switch_button = w;
   Widget        old_switch_button;
   BoxData     * box_data;
   SwitchData  * switch_data;
   Atom          atom_name;

   int i;
   int ac;
   Arg al[2];

   if (callback->reason != XmCR_VALUE_CHANGED) return;
   ac = 0;
   XtSetArg (al[ac], XmNuserData, &box_data);   ac++;
   XtGetValues (switch_button, al, ac);

   switch_data = box_data->switch_data;
   old_switch_button = switch_data->buttons[switch_data->active_switch];


   /*   If the selection occured on a non-selected button, find the atom  */
   /*   representing the workspace name of the button that was selected   */
   /*   and use it to call the workspace manager API to switch the        */
   /*   workspace.  The indication on the buttons occurs out of the       */
   /*   called from the workspace manager API.                            */
   /*                                                                     */
   /*  If the selection occured on the current button, set up the name    */
   /*  change editing functions.                                          */
   
   if (switch_button != old_switch_button)
   {   

      /*  See if the workspace name is being edited.  If so, call the  */
      /*  callback function to accept the changes.                     */
   
      if (XtIsManaged (box_data->switch_edit))
      {
         Widget edit_switch_button;
	    
         XtSetArg (al[0], XmNuserData, &edit_switch_button);
         XtGetValues (box_data->switch_edit, al, 1);
         SwitchRenameCB (box_data->switch_edit, 
                         (XtPointer) edit_switch_button, (XtPointer) NULL);

         XmProcessTraversal (w, XmTRAVERSE_CURRENT);
      }


      if (event->xany.type == ButtonRelease)
         switch_data->time_stamp = event->xbutton.time;

      for (i = 0; i < switch_data->switch_count; i++)
      {
         if (switch_button == switch_data->buttons[i])
	 {
            XtSetArg (al[0], XmNiconName, switch_data->switch_names[i]);
            XtSetArg (al[1], XmNtitle, switch_data->switch_names[i]);
            XtSetValues (panel.shell, al, 2);

            DtWsmSetCurrentWorkspace (XtParent (switch_button),
                                      switch_data->atom_names[i]);
            break;
         }
      }

      _DtIconSetState (old_switch_button, False, False);
   }
   else
   {
      /* Don't do anything on double click */

      if (event->xany.type == ButtonRelease)
      {
         if ((event->xbutton.time - switch_data->time_stamp) >
             XtGetMultiClickTime(XtDisplay(panel.shell)))
         {
            switch_data->time_stamp = event->xbutton.time;
            SwitchRenameLabel (w, box_data);
         }
      }

      _DtIconSetState (w, True, False);
   }
}




/************************************************************************
 *
 *  SwitchRenameCancel
 *        This is the XtActionProc that gets called when a user types
 *        escape in a text widget which has this translation tied to it.
 *
 ***********************************************************************/


static void
SwitchRenameCancel (Widget      w,
                    XEvent    * event,
                    String    * params,
                    Cardinal  * num_params)


{
   Widget switch_button;
   Arg al[1];
 

   /*  Extract the toggle button from the text field's user data  */

   XtSetArg (al[0], XmNuserData, &switch_button);
   XtGetValues (w, al, 1);
   

   /*  Remove the callbacks to eliminate double processing  */

   XtRemoveAllCallbacks (w, XmNactivateCallback);


   /*  Unmanage the text widget, set the traversal to the button and exit  */

   XtUnmanageChild (w);
   XmProcessTraversal (switch_button, XmTRAVERSE_CURRENT);
}




/************************************************************************
 *
 *  SwitchRenameCB
 *	Process callbacks on the text field when a switch name is
 *	being edited.
 *
 ************************************************************************/


static void
SwitchRenameCB (Widget    w,
                XtPointer client_data,
                XtPointer call_data) 


{
   Widget   switch_button = (Widget) client_data;
   char   * edit_string;

   BoxData    * box_data;
   SwitchData * switch_data;

   Boolean valid;
   int     current_switch;
   int     i;
	 
   int ac;
   Arg al[2];

   /*  Remove the callbacks to eliminate double processing  */

   XtRemoveCallback (w, XmNactivateCallback, 
                     SwitchRenameCB, (XtPointer) client_data);


   /*  If this callback has been called and the text widget is already  */
   /*  unmanaged, it means that the escape key was pressed, so return   */
   
   if (XtIsManaged(w) == False) return;


   /*  Process the two reasons this callback is invoked.  A <Return>  */
   /*  key has been pressed or the focus is being moved.  Accept the  */
   /*  new name for both conditions.                                  */

   ac = 0;
   XtSetArg (al[ac], XmNuserData, &box_data);   ac++;
   XtGetValues (switch_button, al, ac);

   switch_data = box_data->switch_data;

   for (i = 0; i < switch_data->switch_count; i++)
      if (switch_data->buttons[i] == switch_button)
         current_switch = i;


   /*  Get the new name from the text field  */

   edit_string = (char *) XmTextFieldGetString (w);


   /*  Verify that title is valid and unique and if so, validate     */
   /*  uniqueness and then change the toggle and workspace internal  */
   /*  information.                                                  */

   valid = True;

   _DtStripSpaces (edit_string);

   if (strlen (edit_string) == 0)
      valid = False;
   else
   {
      for (i = 0; i < switch_data->switch_count; i++)
      {
         if (strcmp (switch_data->switch_names[i], edit_string) == 0)
         {
            if (i == current_switch)
            {
               XtUnmanageChild (w);
               XmProcessTraversal (switch_button, XmTRAVERSE_CURRENT);
               XtFree (edit_string);
               return;
            }
            else
            {
               valid = False;
               break;
            }
         }
      }
   }


   /*  If the title is not valid, post and error dialog and  */
   /*  then leave the text field active to allow reediting.  */
   /*  Otherwise, change the title.                          */

   if (!valid)
   {
      char * s1;
      char * s2;

      s1 = FPGETMESSAGE (86, 1, "Workspace Manager - Rename Error");
      s1 = XtNewString (s1);
      s2 = FPGETMESSAGE (86, 2, "Invalid or duplicate workspace name"),
      s2 = XtNewString (s2);

      _DtMessage (XtParent (w), s1, s2, NULL, NULL);

      XtFree (s1);
      XtFree (s2);
   }
   else
      _DtWsmSetWorkspaceTitle (panel.shell, 
                               switch_data->atom_names[current_switch],
                               edit_string);


   /*  Unmanage the text field and exit  */

   XtFree (edit_string);
   XtUnmanageChild (w);
}




/************************************************************************
 *
 *  SwitchRenameLabel
 *	Set up the text field and callbacks needed for renaming a 
 *	workspace.
 *
 ************************************************************************/


void
SwitchRenameLabel (Widget    switch_button,
                   BoxData * box_data)


{
   Widget switch_edit = box_data->switch_edit;

   static Boolean first = True;

   XmString   toggle_string;
   Position   toggle_x;
   Position   toggle_y;
   Dimension  toggle_width;
   Dimension  toggle_height;
   XmFontList toggle_font_list;
   Pixel      toggle_background;

   Position  switch_rc_x;
   Position  switch_rc_y;

   char * edit_string;

   XtTranslations trans_table;

   int    ac;
   Arg    al[15];


   /*  Add the additional actions needed by the panel and  */
   /*  set up translations in main edit widget             */

   if (first)
   {
      first = False;

      XtAppAddActions(XtWidgetToApplicationContext(panel.shell),action_table,1);
      trans_table = XtParseTranslationTable (translations_escape);
      XtOverrideTranslations (switch_edit, trans_table);
   }


   /*  Extract the label and toggle position  */

   ac = 0;
   XtSetArg (al[ac], XmNx, &toggle_x);   ac++;
   XtSetArg (al[ac], XmNy, &toggle_y);   ac++;
   XtSetArg (al[ac], XmNwidth, &toggle_width);   ac++;
   XtSetArg (al[ac], XmNheight, &toggle_height);   ac++;
   XtSetArg (al[ac], XmNstring, &toggle_string);   ac++;
   XtSetArg (al[ac], XmNfontList, &toggle_font_list);   ac++;
   XtGetValues (switch_button, al, ac);


   /*  Extract the switch_rc's position  */

   switch_rc_x = XtX (XtParent (switch_button));
   switch_rc_y = XtY (XtParent (switch_button));


   /*  Convert the xm string into a char string for editing  and  */
   /*  then create the text widget to perform the name editing.   */

   if ((edit_string =
	(char *)_XmStringUngenerate(toggle_string, NULL,
				    XmCHARSET_TEXT, XmCHARSET_TEXT))
       != (char *)NULL)
   {

      /*  Set the switch editing field to the new resource set  */

      ac = 0;
      XtSetArg (al[ac], XmNleftOffset, toggle_x + switch_rc_x);   ac++;
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);   ac++;
      XtSetArg (al[ac], XmNtopOffset, toggle_y + switch_rc_y);   ac++;
      XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);   ac++;
      XtSetArg (al[ac], XmNmarginWidth, 4);   ac++;
      XtSetArg (al[ac], XmNmarginHeight, 0);   ac++;
      XtSetArg (al[ac], XmNfontList, toggle_font_list);   ac++;
      XtSetArg (al[ac], XmNhighlightThickness, 1);   ac++;
      XtSetArg (al[ac], XmNvalue, edit_string);   ac++;
      XtSetArg (al[ac], XmNuserData, switch_button);   ac++;

      XtSetValues (switch_edit, al, ac);

      XtFree(edit_string);

      ac = 0;
      XtSetArg (al[ac], XmNcursorPosition, 
                XmTextFieldGetLastPosition (switch_edit));   ac++;
      XtSetArg (al[ac], XmNwidth, toggle_width);   ac++;
      XtSetArg (al[ac], XmNheight, toggle_height);   ac++;
      XtSetValues (switch_edit, al, ac);


      /*  Add the callbacks for the text input processing  */

      XtAddCallback (switch_edit, XmNactivateCallback, 
                     SwitchRenameCB, (XtPointer) switch_button);

      XtManageChild (switch_edit);

      XRaiseWindow (XtDisplay (switch_edit), XtWindow (switch_edit));
      XSetInputFocus (XtDisplay (switch_edit), XtWindow (panel.shell),
                      RevertToParent, CurrentTime);

      XmProcessTraversal (switch_edit, XmTRAVERSE_CURRENT);
   }

   XmStringFree (toggle_string);
}




/************************************************************************
 ************************************************************************
 
    The next block of functions are used for the push and drop callbacks
    on all of the panel controls.
    
 ************************************************************************    
 ************************************************************************/




/************************************************************************
 *
 *  CheckControlTypeFile
 *	Check the control if it is of type "file" that the file exists
 *      before performing a push or drop action.
 *
 *  Inputs: control_data - the control whose file type and status are
 *                         to be checked.
 *
 ************************************************************************/


Boolean
CheckControlTypeFile (ControlData * control_data)


{
   struct   stat stat_info;
   char   * file_name;
   char   * title;
   char   * msg;
   char   * message;
   

   if ((int) control_data->element_values[CONTROL_TYPE].parsed_value != CONTROL_FILE)
      return (True);
      
   file_name = 
      (char *) control_data->element_values[CONTROL_FILE_NAME].parsed_value;

   if (lstat (file_name, &stat_info) == 0)
      return (True);


   /*  Display an error dialog because the file cannot be found  */

   title = FPGETMESSAGE (86, 9, "Workspace Manager - Icon Action Error");
   title = XtNewString (title);

   msg = FPGETMESSAGE (86, 7, "Cannot find the file");
   message = XtMalloc (strlen (msg) + strlen (file_name) + 2);
   sprintf (message, "%s %s", msg, file_name);

   _DtMessage (panel.shell, title, message, NULL, NULL);

   XtFree (title);
   XtFree (message);	    

   return (False);   
}



/************************************************************************
 *
 *  FreeFunctionArg
 *	Free the function argument
 *
 ************************************************************************/

static void
FreeFunctionArg (
    WmFunction wmFunc,
    void *pArg)
{
    if (pArg != NULL)
    {
	if (wmFunc == F_Action)
	{
	    WmActionArg *pAP = (WmActionArg *) pArg;

	    if (pAP->aap != NULL)
	    {
		int i;

		for (i=0; i < pAP->numArgs; i++)
		{
		    if (pAP->aap[i].u.file.name != NULL)
			XtFree ((char *) pAP->aap[i].u.file.name);
		}
		XtFree ((char *) pAP->aap);
	    }

	    if (pAP->actionName != NULL)
		XtFree ((char *) pAP->actionName);

	    if (pAP->szExecParms != NULL)
		XtFree ((char *) pAP->szExecParms);

	}

	XtFree ((char *) pArg);
    }
}

Boolean
CheckOtherMonitorsOn(SubpanelData * subpanel_data)
{
   int i;
   ControlData * control_data;
   Boolean m_state;

   for (i = -1; i < subpanel_data->control_data_count; i++)
   {
     if (i == -1)
     {
       m_state = _DtControlGetMonitorState(subpanel_data->main_panel_icon_copy);
     }
     else
     {
       control_data = subpanel_data->control_data[i];
       m_state = _DtControlGetMonitorState(control_data->icon);
     }
        
     if (m_state == DtMONITOR_ON) return True;
   }

   return False;
}


/************************************************************************
 *
 *  PushCB
 *	Process the callback on a control.
 *
 ************************************************************************/

void
PushCB (Widget    w,
        XtPointer client_data,
        XtPointer call_data) 

{
   int control_behavior;

   DtControlCallbackStruct * callback = (DtControlCallbackStruct *) call_data;
   XEvent * event = callback->event;
   ControlData * control_data = (ControlData *) client_data;
   int           control_type;

   PanelActionData * push_action;
   Boolean           push_recall;
   Boolean           unpost_subpanel = True;

   BoxData      * box_data;
   SubpanelData * subpanel_data;
   ControlData  * main_control_data;

   char m_state;

   Widget * pWid;

   Widget monitor_icon;
   Arg    al[1];      

   int i, j;
   

   control_behavior = (int) (panel.element_values[PANEL_CONTROL_BEHAVIOR].parsed_value);


   /*  See if the workspace name is being edited.  If so, call the  */
   /*  callback function to accept the changes.                     */
   
   for (i = 0; i < panel.box_data_count; i++)
   {
      if (panel.box_data[i]->switch_edit != NULL)
      {
         if (XtIsManaged (panel.box_data[i]->switch_edit))
         {
            Widget switch_button;
	    
            XtSetArg (al[0], XmNuserData, &switch_button);
            XtGetValues (panel.box_data[i]->switch_edit, al, 1);
            SwitchRenameCB (panel.box_data[i]->switch_edit, 
	                    (XtPointer) switch_button, (XtPointer) NULL);
            XmProcessTraversal (w, XmTRAVERSE_CURRENT);
            break;
         }
      }
   }


   /*  See if the control action occurred out of a subpanel.  If so,  */
   /*  get the subpanel and set the main control data.                */

   if (control_data->parent_type == SUBPANEL)
   {
      subpanel_data = (SubpanelData *) control_data->parent_data;
      main_control_data = subpanel_data->parent_control_data;

      /*  This control may have been toggled from the subpanel.  If  */
      /*  so, make sure the subpanel is not reposted.                */


      if (XtParent (w) != subpanel_data->form)
         unpost_subpanel = False;
   }
   else
   {
      /*  The control may be the main panel icon copy.  If so, find  */
      /*  the subpanel that contains it a set the subpanel data      */

      subpanel_data = NULL;

      for (i = 0; i < panel.box_data_count; i++)
      {
         for (j = 0; j < panel.box_data[i]->control_data_count; j++)
	 {
            ControlData * box_control;

	    box_control = panel.box_data[i]->control_data[j];

	    if (box_control->subpanel_data != NULL &&
	        box_control->subpanel_data->main_panel_icon_copy == w)
	    {
	       subpanel_data = box_control->subpanel_data;
               main_control_data = subpanel_data->parent_control_data;
               break;
	    }
	 }
         if (subpanel_data != NULL)
	    break;
      }
   }


   /*  If the input occurred out of a subpanel, check to see if  */
   /*  the input was an escape key.  If so, unpost the subpanel  */
   /*  and return.                                               */

   if (subpanel_data != NULL && event != NULL)
   {
      if (event->xany.type == KeyPress)
      {
         int count;
	 char buffer[10];
	 int bufsize = 10;
	 KeySym keysym;
	 XComposeStatus compose;
         static Boolean first = True;

         count = XLookupString ((XKeyEvent *)event, buffer, bufsize,
                                &keysym, &compose);

         if (keysym == XK_Escape)
         {
            if (first)
            {
               ArrowCB (main_control_data->arrow,
			(XtPointer)main_control_data, (XtPointer)NULL);
               first = False;
            }
            else
               first = True;

            return;
         }
      }
   }


   /*  This function handles four callback reasons.  Two of them, */
   /*  START and STOP show and remove an hour glass cursor.       */
   /*  MONITOR modifies subpanel visuals when a file change is    */
   /*  detected.  ACTIVATE/DEFAULT_ACTION handle the activation   */
   /*  of a control.                                              */

   if (callback->reason == XmCR_BUSY_START)
   {

      XDefineCursor (XtDisplay (panel.shell), XtWindow (panel.shell),
                     panel.busy_cursor);

      for (i = 0, pWid = M_PopupList (panel.shell);
           i < M_NumPopups (panel.shell); i++)
         XDefineCursor (XtDisplay (panel.shell), XtWindow (pWid[i]),
                        panel.busy_cursor);
      
   }
   else if (callback->reason == XmCR_BUSY_STOP)
   {
      XUndefineCursor (XtDisplay (panel.shell), XtWindow (panel.shell));

      for (i = 0, pWid = M_PopupList (panel.shell);
           i < M_NumPopups (panel.shell); i++)
         XUndefineCursor (XtDisplay (panel.shell), XtWindow (pWid[i]));
      
   }
   else if (callback->reason == XmCR_MONITOR)
   {
      /*  For a monitor file, turn on the monitor indicator for  */
      /*  the subpanel control and turn on the arrow to the      */
      /*  highlighted arrow of the subpanel.                     */
      
      if (subpanel_data != NULL)
      {
         monitor_icon = control_data->indicator;

         m_state = _DtControlGetMonitorState(w);

         if (m_state == DtMONITOR_ON)
            XtSetArg (al[0], XmNimageName, indicator_on_image);
         else /* DtMONITOR_OFF */
            XtSetArg (al[0], XmNimageName, indicator_off_image);

         XtSetValues (monitor_icon, al, 1);

         if (XtIsManaged (subpanel_data->shell))
         {
            if (m_state == DtMONITOR_ON)
            {
               XtSetArg (al[0], XmNimageName, unpost_monitor_arrow_image);
               XtSetValues (main_control_data->arrow, al, 1);
            }
            else
            {
               if (!CheckOtherMonitorsOn(subpanel_data))
               {
                  XtSetArg (al[0], XmNimageName, unpost_arrow_image);
                  XtSetValues (main_control_data->arrow, al, 1);
               }
            }
         }
         else
         {
            if (m_state == DtMONITOR_ON)
            {
               XtSetArg (al[0], XmNimageName, post_monitor_arrow_image);
               XtSetValues (main_control_data->arrow, al, 1);
            }
            else
            {
               if (!CheckOtherMonitorsOn(subpanel_data))
               {
                  XtSetArg (al[0], XmNimageName, post_arrow_image);
                  XtSetValues (main_control_data->arrow, al, 1);
               }
            }
         }

      }
   }
   else if ((control_behavior == DOUBLE_CLICK && 
            callback->reason == XmCR_DEFAULT_ACTION) ||
            (control_behavior == SINGLE_CLICK &&
	     (callback->reason == XmCR_DEFAULT_ACTION || 
	      callback->reason == XmCR_SINGLE_SELECT)))
   {
      control_type = (int) (control_data->element_values[CONTROL_TYPE].parsed_value);
      push_action = (PanelActionData *) (control_data->element_values[CONTROL_PUSH_ACTION].parsed_value);
      push_recall = (Boolean) (control_data->element_values[CONTROL_PUSH_RECALL].parsed_value);

      switch (control_type)
      {
	 case CONTROL_CLIENT:
	 case CONTROL_ICON:
	 case CONTROL_FILE:
	 case CONTROL_DATE:
         {
      
            /*  Turn off the subpanel control monitor indicator  */

            if ((char) control_data->element_values[CONTROL_MONITOR_TYPE].
	               parsed_value != MONITOR_NONE)
	    {
               if (subpanel_data != NULL)
               {
                  monitor_icon = control_data->indicator;
                  XtSetArg (al[0], XmNimageName, indicator_off_image);
                  XtSetValues (monitor_icon, al, 1);
               }
            }


            if (CheckControlTypeFile (control_data) && push_action != NULL)
            {
               _DtControlDoPushAnimation (w);   

               if (push_recall == False)
               {
                  /*  If the control is of type file, build and object  */
		  /*  list that contains the host and file name to      */
		  /*  pass to the invokation.                           */
		  
                  if (control_type == CONTROL_FILE && !control_data->is_action)
		  {
                     DtActionArg * aap;

                     aap = (DtActionArg *) XtCalloc(1,sizeof(DtActionArg));

                     aap->argClass = DtACTION_FILE;

                     aap->u.file.name = (char *) control_data->
                                 element_values[CONTROL_FILE_NAME].parsed_value;

                     DtActionInvoke (panel.shell, 
                                     push_action->action_name,
                                     aap, 1, NULL, NULL, NULL,
                                     1, NULL, NULL);

                     XtFree ((char *) aap);
		  }
		  else
                  {
                     if (strcmp (push_action->action_name, "FPOnItemHelp") == 0)
                     {
			WmDtHelpMode();
                        unpost_subpanel = False;
                     }
                     else
                     {
                        DtActionInvoke (panel.shell, 
			                push_action->action_name,
					NULL, 0, NULL, NULL, NULL,
                                        1, NULL, NULL);
                     }
                  }
               }
               else
	       {
                  WmFunction wm_func;
                  int        function_index;
                  String     func_arg = NULL;
                  String     s1, s1save;
                  char     * client_name;
                  char     * client_title;		  

                  WmGlobalData    * wm_global_data = (WmGlobalData *) panel.global_data;
                  ClientData      * wm_client_data = NULL;
                  WmPushRecallArg   push_argument;


                  /*  Parse out the function and arguments used by the  */
		  /*  window manager to invoke the client.              */
		  
                  s1save = s1 = XtNewString ("f.action");

                  function_index = 
		    ParseWmFunction ((unsigned char **) &s1, CRS_ANY, &wm_func);
		  XtFree (s1save);


                  client_name = 
		     (char *) control_data->element_values[CONTROL_CLIENT_NAME].parsed_value;

                  if (client_name == NULL)
                     client_name = 
		        (char *) control_data->element_values[CONTROL_LABEL].parsed_value;

                  client_title = 
		     (char *) control_data->element_values[CONTROL_LABEL].parsed_value;

                  if (control_type == CONTROL_FILE && ! control_data->is_action)
		  {
                     char * file_name;

                     file_name = (char *) control_data->
			                  element_values[CONTROL_FILE_NAME].parsed_value;

                     s1 = XtMalloc (strlen (push_action->action_name) + strlen (file_name) + 2);
		     strcpy (s1, push_action->action_name);
		     strcat (s1, " ");
		     strcat (s1, file_name);
                  }
                  else
                     s1 = XtNewString (push_action->action_name);

                  ParseWmFunctionArg ((unsigned char **) &s1, function_index, 
		                      wm_func, (void **) &func_arg, 
				      client_name, client_name);


                  /*  Set the push argument to the parsed data  */
		  
                  push_argument.ixReg = PushRecallGetData (client_name);
                  push_argument.wmFunc = wm_func;
                  push_argument.pArgs = func_arg;


                  /*  Get the window manager client data for the panel and  */
		  /*  call the push recall function to get the client run.  */
		  
                  if (subpanel_data != NULL)
                     XFindContext (wm_global_data->display, 
		                   XtWindow (subpanel_data->shell),
                                   wm_global_data->windowContextType,
                                   (XtPointer) &wm_client_data);
                  else
                     XFindContext (wm_global_data->display, 
		                   XtWindow (panel.shell),
                                   wm_global_data->windowContextType,
                                   (XtPointer) &wm_client_data);

                  F_Push_Recall ((String) &push_argument, 
		                 wm_client_data, callback->event);

		  FreeFunctionArg (wm_func, func_arg);
                  XtFree (s1);
    	       }
            }
         }
	 break;


         case CONTROL_BLANK:
         case CONTROL_BUSY:
	 case CONTROL_CLOCK:
            return;
         break;
      }


      /*  Call the unposting function and reset the arrow behavior if the  */
      /*  action occured out of a subpanel.                                */

      if (subpanel_data != NULL && unpost_subpanel &&
          (Boolean) panel.element_values[PANEL_SUBPANEL_UNPOST].parsed_value &&
          subpanel_data->torn == False)
         ArrowCB (main_control_data->arrow,
		  (XtPointer)main_control_data, (XtPointer)NULL);
   }
}




/************************************************************************
 *
 *  DropCB
 *	Process the callback for drops on a control
 *
 ************************************************************************/


void
DropCB (Widget    w,
        XtPointer client_data,
        XtPointer call_data) 


{
   PanelActionData * drop_action = (PanelActionData *) client_data;
   DtDndDropAnimateCallback animate_data =
                         (DtDndDropAnimateCallback) call_data;

   ControlData    * control_data;
   Arg              al[1];
   int              i, j, k, l;
   DtActionArg    * aap;
   char           * save_name = NULL;

   XtSetArg (al[0], XmNuserData, &control_data);
   XtGetValues (w, al, 1);

   if (CheckControlTypeFile (control_data) &&
       (animate_data->dropData->protocol == DtDND_FILENAME_TRANSFER ||
        animate_data->dropData->protocol == DtDND_BUFFER_TRANSFER))
   {
      int numItems;
      int control_type =
         (int)control_data->element_values[CONTROL_TYPE].parsed_value;
      char * control_name =
         (char *)control_data->element_values[CONTROL_FILE_NAME].parsed_value;
      Boolean send_control_name = False;

      _DtControlDoDropAnimation (w);

      drop_action = (PanelActionData *) 
         control_data->element_values[CONTROL_DROP_ACTION].parsed_value;

      if (control_data->operation == XmDROP_MOVE)
      {
         if (control_data->move_action)
         {
            save_name = drop_action->action_name;
            drop_action->action_name = control_data->move_action;
         }
      }
      else if (control_data->operation == XmDROP_COPY)
      {
         if (control_data->copy_action)
         {
            save_name = drop_action->action_name;
            drop_action->action_name = control_data->copy_action;
         }
      }
      else if (control_data->operation == XmDROP_LINK)
      {
         if (control_data->link_action)
         {
            save_name = drop_action->action_name;
            drop_action->action_name = control_data->link_action;
         }
      }

  
      numItems = animate_data->dropData->numItems;
  
      if ((control_type == CONTROL_FILE) &&
          (!control_data->is_action) &&
          (strcmp(control_name, drop_action->action_name) != 0))
      {
        ++numItems;
        send_control_name = True;
      }

      if (animate_data->dropData->protocol == DtDND_FILENAME_TRANSFER)
      {
         char ** file_list = animate_data->dropData->data.files;
         int m = 0;
  
         aap = (DtActionArg *) XtCalloc (numItems, sizeof (DtActionArg));
  
         if (send_control_name)
         {
           aap[m].argClass = DtACTION_FILE;
           aap[m++].u.file.name = 
           (char *)control_data->element_values[CONTROL_FILE_NAME].parsed_value;
         }
  
         for (l = 0; m < numItems; l++, m++)
         {
             aap[m].argClass = DtACTION_FILE;
             aap[m].u.file.name = file_list[l];
         }
      }
      else if (animate_data->dropData->protocol == DtDND_BUFFER_TRANSFER)
      {
         DtDndBuffer * buf_list = animate_data->dropData->data.buffers;
         int m = 0;
  
         aap = (DtActionArg *) XtCalloc (numItems, sizeof (DtActionArg));
  
         if (send_control_name)
         {
           aap[m].argClass = DtACTION_FILE;
           aap[m++].u.file.name = 
           (char *)control_data->element_values[CONTROL_FILE_NAME].parsed_value;
         }
  
         for (l = 0; m < numItems; l++, m++)
         {
             aap[m].argClass = DtACTION_BUFFER;
             aap[m].u.buffer.bp = buf_list[l].bp;
             aap[m].u.buffer.size = buf_list[l].size;
             aap[m].u.buffer.name = buf_list[l].name;
         }
      }
      else
      {
         if (save_name != NULL)
            drop_action->action_name = save_name;

         control_data->operation = 0;

         return;
      }
       
      DtActionInvoke (w, drop_action->action_name, aap, numItems,
                     NULL, NULL, NULL, 1, NULL, NULL);

      XtFree((char *) aap);

      if (save_name != NULL)
         drop_action->action_name = save_name;

      control_data->operation = 0;
   }
}




/************************************************************************
 *
 *  TransferDropCB
 *	Process the callback for drops on a control
 *
 ************************************************************************/


void
TransferDropCB (Widget    w,
                XtPointer client_data,
                XtPointer call_data) 


{
   DtDndTransferCallbackStruct * transfer_data =
                         (DtDndTransferCallbackStruct *) call_data;
   ControlData * control_data;
   Arg           al[1];


   /* Currently only accepts FILE drops */

   if (transfer_data->dropData->protocol == DtDND_FILENAME_TRANSFER ||
       transfer_data->dropData->protocol == DtDND_BUFFER_TRANSFER)
   {
      transfer_data->status = DtDND_SUCCESS;

      XtSetArg (al[0], XmNuserData, &control_data);
      XtGetValues (w, al, 1);

      control_data->operation = transfer_data->operation;

   }
   else
      transfer_data->status = DtDND_FAILURE;

   transfer_data->completeMove = False;
}



/************************************************************************
 ************************************************************************
 
    This block of functions handle the processing of subpanel torn-off
    behavior.  Default behavior for a subpanel is to unpost when a 
    control is selected.  This changes to sticky when the subpanel is torn.
    
 ************************************************************************
 ************************************************************************/


/************************************************************************
 *
 *  SubpanelUnmapCB
 *	Process the callback on the subpanel unposting initiated through
 *	the window manager close menu item.
 *
 ************************************************************************/


void
SubpanelUnmapCB (Widget    w,
                 XtPointer client_data,
                 XtPointer call_data) 


{
   ControlData  * main_control_data = (ControlData *) client_data;
   SubpanelData * subpanel_data = main_control_data->subpanel_data;

   Arg  al[1];
   int  ac;


   ac = 0;
   XtSetArg (al[ac], XmNimageName, post_arrow_image);	ac++;
   XtSetValues (main_control_data->arrow, al, ac);

   XtUnmanageChild (subpanel_data->shell);
   subpanel_data->torn = False;


   /*  Remove the event handler from the subpanel  */

   XtRemoveEventHandler (subpanel_data->shell, StructureNotifyMask, False,
                         (XtEventHandler) SubpanelTornEventHandler,
                         (XtPointer) main_control_data);


   /*  Reset the active traversal control for the front panel  */
   /*  We should not have to do this but for unknown reasons,  */
   /*  the traversal highlight is getting lost.                */
   
   XmProcessTraversal (panel.shell, XmTRAVERSE_NEXT);
   XmProcessTraversal (panel.shell, XmTRAVERSE_PREV);
}




/************************************************************************
 *
 *  SubpanelTornEventHandler
 *	Set a subpanel flag to change its posting behavior when it is
 *	torn off.
 *
 ************************************************************************/

void
SubpanelTornEventHandler (Widget      subpanel_shell,
                          XtPointer   client_data,
                          XEvent    * event,
                          Boolean   * continue_dispatch)

{
   ControlData  * main_control_data = (ControlData *) client_data;
   SubpanelData * subpanel_data = main_control_data->subpanel_data;


   if (subpanel_data->posted_x != 0 &&
       subpanel_data->posted_x != XtX (subpanel_data->shell))
       
      subpanel_data->torn = True;
   else
      subpanel_data->torn = False;

   *continue_dispatch = True;
}





/************************************************************************
 *
 *  StartHelpClient
 *	Start up the help manager to show the help topic
 *
 *  Inputs: widget - the widget for which help was requested
 *          help_volume - the help volume that contains the information
 *          help_topic - the specific help topic.
 *
 ************************************************************************/


void StartHelpClient (Widget   widget,
		      char   * help_volume,
		      char   * help_topic)


{
   /***** TEMPORARY - Add code to actually invoke the Help Client ****/  

   WmDtDisplayTopicHelp (widget, help_volume, help_topic);
}

/************************************************************************
 *
 *  InvokeHelp
 *	Invoke topic help if available otherwise invoke help string if
 *  available
 *
 ************************************************************************/


void InvokeHelp (Widget widget,
		 char * help_topic,
		 char * help_volume,
		 char * help_string)
{

    if (help_volume == NULL)
       help_volume = FP_HELP_VOLUME;

    if (help_topic != NULL)
    {
       if (strcmp (help_volume, FP_HELP_VOLUME) == 0)
          WmDtDisplayTopicHelp (widget, help_volume, help_topic);
       else
          StartHelpClient (widget, help_volume, help_topic);
    }
    else
    {
       if (help_string != NULL)
          WmDtDisplayStringHelp (widget, help_string);
    }
}


/************************************************************************
 *
 *  SubpanelTopicHelpCB
 *	Process the callback for help on the subpanel 
 *
 ************************************************************************/


void SubpanelTopicHelpCB (Widget    widget,
                          XtPointer client_data,
                          XtPointer call_data)

{
    SubpanelData * subpanel_data = (SubpanelData *) client_data;
    char         * help_volume;
    char         * help_topic;
    char         * help_string;

    help_volume =
      (char *) subpanel_data->element_values[SUBPANEL_HELP_VOLUME].parsed_value;
    help_topic =
      (char *) subpanel_data->element_values[SUBPANEL_HELP_TOPIC].parsed_value;
    help_string =
      (char *) subpanel_data->element_values[SUBPANEL_HELP_STRING].parsed_value;

    InvokeHelp(widget, help_topic, help_volume, help_string);

} /* END OF FUNCTION SubpanelTopicHelpCB */




/************************************************************************
 *
 *  SwitchTopicHelpCB
 *	Process the callback for help on the switch 
 *
 ************************************************************************/


void SwitchTopicHelpCB (Widget    widget,
                        XtPointer client_data,
                        XtPointer call_data)

{
    SwitchData * switch_data = (SwitchData *) client_data;
    char       * help_volume;
    char       * help_topic;
    char       * help_string;

    help_volume =
	 (char *) switch_data->element_values[SWITCH_HELP_VOLUME].parsed_value;
    help_topic =
	 (char *) switch_data->element_values[SWITCH_HELP_TOPIC].parsed_value;
    help_string =
	 (char *) switch_data->element_values[SWITCH_HELP_STRING].parsed_value;

    InvokeHelp(widget, help_topic, help_volume, help_string);

} /* END OF FUNCTION SwitchTopicHelpCB */




/************************************************************************
 *
 *  ControlTopicHelpCB
 *	Process the callback for help on the control 
 *
 *************************************************************************/ 


void ControlTopicHelpCB (Widget    widget,
                         XtPointer client_data,
                         XtPointer call_data)


{
    ControlData * control_data = (ControlData *) client_data;
    char        * help_volume;
    char        * help_topic;
    char        * help_string;

    help_volume = 
	(char *) control_data->element_values[CONTROL_HELP_VOLUME].parsed_value;
    help_topic =
	(char *) control_data->element_values[CONTROL_HELP_TOPIC].parsed_value;
    help_string =
	(char *) control_data->element_values[CONTROL_HELP_STRING].parsed_value;

    InvokeHelp(widget, help_topic, help_volume, help_string);

} /* END OF FUNCTION ControlTopicHelpCB */




/************************************************************************
 *
 *  GeneralTopicHelpCB
 *	Process the callback for help on the control 
 *
 ************************************************************************/


void GeneralTopicHelpCB (Widget    widget,
                         XtPointer client_data,
                         XtPointer call_data)


{
    char * help_volume;
    char * help_topic;

    help_volume = FP_HELP_VOLUME;
    help_topic = (char *) client_data;

    WmDtDisplayTopicHelp (widget, help_volume, help_topic);

} /* END OF FUNCTION GeneralTopicHelpCB */




/************************************************************************
 ************************************************************************

    This block of functions handle the initialization and dynamic
    processing for the subpanel installation area.

 ************************************************************************
 ************************************************************************/

/************************************************************************
 *
 *  GetValuesFromDataType
 *    Given and file type index and subpanel data, construct an element
 *    values array for a new control that is initialized to the attributes
 *    of the file type.
 *
 ************************************************************************/


static void
GetValuesFromDataType (char *	       data_type,
		       char *  	       file_name,
		       SubpanelData  * subpanel_data,
		       ElementValue ** element_values)


{
   char * ptr;
   int i;

   *element_values = (ElementValue *) XtMalloc (sizeof(ElementValue) *
                                                       CONTROL_KEYWORD_COUNT);

   for (i = 0; i < CONTROL_KEYWORD_COUNT; i++)
   {
      (*element_values)[i].use_default = True;
      (*element_values)[i].string_value = NULL;
      (*element_values)[i].parsed_value = NULL;
   }

   (*element_values)[CONTROL_FILE_NAME].string_value = XtNewString (file_name);

    ptr = (char *)strrchr(file_name, '/');
    ptr++;

    (*element_values)[CONTROL_NAME].string_value = XtNewString(ptr);
    (*element_values)[CONTROL_TYPE].string_value = XtNewString("file");
    (*element_values)[CONTROL_CONTAINER_TYPE].string_value =
		     				XtNewString("SUBPANEL");
    (*element_values)[CONTROL_CONTAINER_NAME].string_value =
		 XtNewString(subpanel_data->element_values[0].string_value);
    (*element_values)[CONTROL_POSITION_HINTS].string_value =
		 				XtNewString("last");

    InitializeControlFields (*element_values, data_type);
}




/************************************************************************
 *
 *  CustomizeDropCB
 *	Process the callback for the drop on the subpanel drop zone for
 *      dynamic customization
 *
 ************************************************************************/


void
CustomizeDropCB (Widget    w,
        	 XtPointer client_data,
         	 XtPointer call_data) 

{
   BoxData      * box_data;
   ControlData  * main_control_data;
   SubpanelData * subpanel_data;
   DtDndDropAnimateCallbackStruct * animate_data =
                         (DtDndDropAnimateCallbackStruct *) call_data;

   ControlData  * control_data;
   ElementValue * element_values;
   char * data_type;

   Boolean drop_of_fp;
   Boolean bad_control;
   Boolean control_monitor;
   int     position_hints;
   int     count;

   char  * new_control_name;
   char  * control_name;
   Widget  attach_widget;

   int i, j, k;

   int     file_count;
   char ** file_list;

   Arg al[4];
   int ac;
   

   /*  If this is not a transfer drop, return  */
   
   if (animate_data->dropData->protocol != DtDND_FILENAME_TRANSFER)
      return;


   /*  Get the subpanel, box, and main control data  */
   
   XtSetArg (al[0], XmNuserData, &subpanel_data);
   XtGetValues (w, al, 1);

   main_control_data = subpanel_data->parent_control_data;
   box_data = (BoxData *) main_control_data->parent_data;
   
   file_count = animate_data->dropData->numItems;
   file_list = animate_data->dropData->data.files;


   /*  Pop down the subpanel before creating the new control or  */
   /*  mucking with any of the old controls.                     */

   if (subpanel_data->torn == False)
      XtUnmanageChild (subpanel_data->shell);        


   /*  Loop through the set of files (possibly more than one) that  */
   /*  have been dropped.                                           */

   for (i = 0; i < file_count; i++)
   {

      /*  Get the file type of the file and process as either  */
      /*  a "FrontPanel" file or a normal file type.           */

      data_type = DtDtsFileToDataType (file_list[i]);

      if (!strcmp (data_type, "FP"))
      {
         drop_of_fp = True;
         InitParse (file_list[i], &element_values);


         /*  Reset the values of the container name and type to this one  */

         if (element_values[CONTROL_CONTAINER_NAME].use_default == False)
         {
            if (element_values[CONTROL_CONTAINER_NAME].parsed_value != NULL)
               XtFree (element_values[CONTROL_CONTAINER_NAME].parsed_value);
            if (element_values[CONTROL_CONTAINER_NAME].string_value != NULL)
               XtFree (element_values[CONTROL_CONTAINER_NAME].string_value);
         }

         if (element_values[CONTROL_CONTAINER_TYPE].use_default == False)
         {
            if (element_values[CONTROL_CONTAINER_TYPE].string_value != NULL)
               XtFree (element_values[CONTROL_CONTAINER_TYPE].string_value);
         }

         if (element_values[CONTROL_POSITION_HINTS].use_default == False)
         {
            if (element_values[CONTROL_POSITION_HINTS].string_value != NULL)
               XtFree (element_values[CONTROL_POSITION_HINTS].string_value);
         }

         element_values[CONTROL_CONTAINER_NAME].use_default = False;
         element_values[CONTROL_CONTAINER_NAME].string_value = 
            XtNewString (subpanel_data->element_values[SUBPANEL_NAME].parsed_value);
         element_values[CONTROL_CONTAINER_NAME].parsed_value = 
            XtNewString (subpanel_data->element_values[SUBPANEL_NAME].parsed_value);

         element_values[CONTROL_CONTAINER_TYPE].use_default = False;
         element_values[CONTROL_CONTAINER_TYPE].string_value = 
            XtNewString ("SUBPANEL");
         element_values[CONTROL_CONTAINER_TYPE].parsed_value =
	    (char *) SUBPANEL;

         element_values[CONTROL_POSITION_HINTS].use_default = False;
         element_values[CONTROL_POSITION_HINTS].string_value = 
            XtNewString ("last");
         element_values[CONTROL_POSITION_HINTS].parsed_value =
	    (char *) 100;
      }
      else
      {
         drop_of_fp = False;
         GetValuesFromDataType (data_type, file_list[i], 
                                subpanel_data, &element_values);
      }

      DtDtsFreeDataType (data_type);



      /*  Check for naming conflicts and if found, issue an error and   */
      /*  clean up.  Continue processing subsequent files even if this  */
      /*  one is not valid.                                             */
	 
      new_control_name = (char *) element_values[CONTROL_NAME].parsed_value;
      bad_control = False;

      for (j = 0; j < subpanel_data->control_data_count; j++)
      {
         control_name = (char *) subpanel_data->control_data[j]->
                                 element_values[CONTROL_NAME].parsed_value;

         if (strcmp (control_name, new_control_name) == 0)
         {
            ControlData control_data;
            String title, del_ctrl, ctrl_name, del_msg, message;
	 

            /*  Display an error dialog for the unusable drop data  */

            title = FPGETMESSAGE (86, 3, "Workspace Manager - Install Icon Error");
            title = XtNewString (title);

            del_ctrl = FPGETMESSAGE (86, 4, "Install Icon:");
            del_ctrl = XtNewString (del_ctrl);

            del_msg = FPGETMESSAGE (86, 8, "There is already an icon of this name.");
            del_msg = XtNewString (del_msg);

            message = XtMalloc ((strlen(del_ctrl) + strlen(new_control_name) +
                                strlen(del_msg) + 4));
            sprintf(message, "%s %s\n\n%s", del_ctrl, new_control_name, del_msg);

            _DtMessage (panel.shell, title, message, NULL, NULL);

            XtFree (title);	    
            XtFree (del_ctrl);	    
            XtFree (del_msg);	    
            XtFree (message);	    


            /*  Set up a temporary control data structure so that the   */
            /*  appropriate function can be used to free the allocated  */
            /*  element values.                                         */

            control_data.element_values = element_values;
            RemoveEntry ((RecordData *)&control_data, CONTROL);
	 
            bad_control = True;
            break;
         }
      }

      if (bad_control) continue;


      if ((int) element_values[CONTROL_MONITOR_TYPE].parsed_value != MONITOR_NONE)
         control_monitor = True;
      else
         control_monitor = False;


      position_hints = (int)element_values[CONTROL_POSITION_HINTS].parsed_value;


      /*  Initialize the subpanel layout and processing attributes.  */

      if ((int) main_control_data->
           element_values[CONTROL_MONITOR_TYPE].parsed_value != MONITOR_NONE)
      {
         control_monitor = True;
      }


      for (j = 0; j < subpanel_data->control_data_count; j++)
      {
         if ((int) subpanel_data->control_data[j]->
             element_values[CONTROL_MONITOR_TYPE].parsed_value != MONITOR_NONE)
         {
            control_monitor = True;
            break;
         }
      }


      /*  If the new control is a montior, loop through the existing  */
      /*  control set a adjust the left offset resource to ensure     */
      /*  that they are all aligned properly.                         */
	 
      if (control_monitor)
      {
         XtSetArg (al[0], XmNleftOffset, 20);

         for (j = 0; j < subpanel_data->control_data_count; j++)
            XtSetValues (subpanel_data->control_data[j]->icon, al, 1);

         if (subpanel_data->main_panel_icon_copy != NULL)
            XtSetValues (subpanel_data->main_panel_icon_copy, al, 1);
      }


      /*  Get the subpanel control data and position the element   */
      /*  values into the postion hints location.                  */
      /*  The remainder of the positioning code is set up to allow */
      /*  placement in non-last positions even though position is  */
      /*  forced to last.  This is here for future use.            */

      subpanel_data->control_data_count++;
      count = subpanel_data->control_data_count;

      subpanel_data->control_data = 
         (ControlData **) XtRealloc ((char *) subpanel_data->control_data, 
                                     sizeof (ControlData *) * count);

      for (j = count - 2; j >= 0; j--)
      {
         if (position_hints >= 
             (int) subpanel_data->control_data[j]->element_values[CONTROL_POSITION_HINTS].parsed_value)
            break;
      }

      j++;
	 
      for (k = count - 1; k > j; k--)
         subpanel_data->control_data[k] = subpanel_data->control_data[k - 1];

      subpanel_data->control_data[j] = control_data =
         (ControlData *) XtMalloc (sizeof (ControlData));
	 
      control_data->element_values = element_values;
      control_data->parent_data = (XtPointer) subpanel_data;
      control_data->parent_type = SUBPANEL;
      control_data->subpanel_data = NULL;
      control_data->icon = NULL;
      control_data->arrow = NULL;
      control_data->arrow_separator = NULL;
      control_data->indicator = NULL;
      control_data->is_action = False;
      control_data->move_action = NULL;
      control_data->copy_action = NULL;
      control_data->link_action = NULL;
      control_data->operation = 0;

      AddControlActionList (control_data);


      if (j == 0 && subpanel_data->main_panel_icon_copy != NULL)
         attach_widget = subpanel_data->main_panel_icon_copy;
      else
         attach_widget = subpanel_data->control_data[j - 1]->icon;


      /*  Reattach the bottom control so the new bottom will appear  */

      if (j == count - 1)
      {
         ac = 0;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_NONE);        ac++;
         XtSetArg (al[ac], XmNbottomOffset, 0);                        ac++;
         XtSetValues (attach_widget, al, ac);
      }


      SubpanelControlCreate (subpanel_data, main_control_data, 
                             control_data, attach_widget, False, 
                             control_monitor);
	 

      /*  Pad the bottom control */

      if (j == count - 1)
      {
         ac = 0;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);        ac++;
         XtSetArg (al[ac], XmNbottomOffset, 5);                        ac++;
         XtSetValues (control_data->icon, al, ac);
      }

      WriteControlComponentFile (control_data);

   }


   /*  Pop the subpanel back up and return  */

   if (subpanel_data->torn == False)
      ArrowCB (main_control_data->arrow,
	       (XtPointer)main_control_data, (XtPointer)NULL);
}




/************************************************************************
 *
 *  CustomizeTransferDropCB
 *	Process the callback for drops on an install zone
 *
 ************************************************************************/


void
CustomizeTransferDropCB (Widget    w,
                XtPointer client_data,
                XtPointer call_data) 


{
   DtDndTransferCallbackStruct * transfer_data =
                         (DtDndTransferCallbackStruct *) call_data;

   /* Currently only accepts FILE drops */

   if (transfer_data->dropData->protocol == DtDND_FILENAME_TRANSFER)
      transfer_data->status = DtDND_SUCCESS;
   else
      transfer_data->status = DtDND_FAILURE;

   transfer_data->completeMove = False;
}
