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
/* $XConsortium: FilterP.c /main/4 1995/11/02 14:39:12 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           FilterP.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Processing functions for the file filter dialog
 *
 *   FUNCTIONS: FilterChange
 *		FilterClose
 *		NewFileTypeSelected
 *		SelectAllFileTypes
 *		ShowFilterDialog
 *		UnselectAllFileTypes
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>

#include <Dt/Icon.h>

#include <Dt/HourGlass.h>
#include <Dt/FileM.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Common.h"
#include "Filter.h"


/********    Static Function Declarations    ********/

static void FilterChange(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data,
                        XtPointer call_data) ;
static void FilterClose(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;

/********    End Static Function Declarations    ********/



/************************************************************************
 *
 *  ShowFilterDialog
 *	Callback functions invoked from the File Filter ... menu
 *	item.  This function displays the file filter dialog.
 *
 ************************************************************************/

void
ShowFilterDialog(
        Widget w,
        XtPointer client_data,
        XtPointer callback )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   FilterRec * filter_rec;
   Arg args[1];
   Widget mbar;
   char *tmpStr, *tempStr;


   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   if (w)
   {
      mbar = XmGetPostedFromWidget(XtParent(w));
      XmUpdateDisplay (w);
      XtSetArg(args[0], XmNuserData, &file_mgr_rec);
      XtGetValues(mbar, args, 1);

      /* Ignore accelerators when we're insensitive */
      if ((file_mgr_rec->menuStates & FILTER) == 0)
      {
         XSetInputFocus(XtDisplay(w),
                        XtWindow(file_mgr_rec->filterBtn_child),
                        RevertToParent, CurrentTime);
         return;
      }
   }
   else
   {
      /* Done only during a restore session */
      file_mgr_rec = (FileMgrRec *)client_data;
   }


   /* Ignore accelerators received after we're unposted */
   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;
   file_mgr_data = (FileMgrData *) dialog_data->data;

   file_mgr_rec->menuStates &= ~FILTER;

   _DtTurnOnHourGlass (file_mgr_rec->shell);

   _DtShowDialog (file_mgr_rec->shell, NULL, (XtPointer)file_mgr_rec, 
               file_mgr_data->filter_edit, FilterChange,
	       (XtPointer)file_mgr_rec, FilterClose, (XtPointer)file_mgr_rec,
	       NULL, False, False, NULL, NULL);

   filter_rec = (FilterRec *)_DtGetDialogInstance(file_mgr_data->filter_edit);

   if(file_mgr_data->title != NULL && 
               strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
      tmpStr = (GETMESSAGE(13,19, "Set Filter Options"));
      tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
      sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
      tmpStr = (GETMESSAGE(13,24, "File Manager - Set Filter Options"));
      tempStr = XtNewString(tmpStr);
   }
   XtSetArg (args[0], XmNtitle, tempStr);
   XtSetValues (filter_rec->shell, args, 1);
   XtFree(tempStr);

   file_mgr_rec->filterBtn_child=filter_rec->shell;
   _DtTurnOffHourGlass (file_mgr_rec->shell);

   {
     Widget hb = NULL;

     if( XtIsRealized( filter_rec->shell ) )
     {
       /* In order for the set increment to work,
          the top level shell has to be realized.
          This seemed to be a good place for it.
       */
       XtSetArg( args[0], XmNhorizontalScrollBar, &hb );
       XtGetValues( filter_rec->scrolled_window, args, 1 );
       if( hb )
       {
         XtSetArg( args[0], XmNincrement, 40 );
         XtSetValues( hb, args, 1 );
       }
     }
   }
}




/************************************************************************
 *
 *  FilterChange
 *	Callback functions invoked from the file filter dialog's
 *	apply button being pressed.  This function updates and redisplays
 *	the current set of objects being viewed.
 *
 ************************************************************************/

static void
FilterChange(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   FilterData  * filter_data;
   XtPointer save_data;


   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   /*
    * Replace the active values with those just applied in the dialog,
    * and then free up the old active values.
    */

   save_data = file_mgr_data->filter_active->data;
   file_mgr_data->filter_active->data = new_dialog_data->data;
   new_dialog_data->data = save_data;
   _DtFreeDialogData (new_dialog_data);

   filter_data = (FilterData *) file_mgr_data->filter_active->data;
   filter_data->displayed = False;


   /* Refilter the contents of the current directory */

   FileMgrRedisplayFiles (file_mgr_rec, file_mgr_data, False);
}




/************************************************************************
 *
 *  FilterClose
 *	Callback function invoked from the file filter dialog's close
 *	button.  This function resensitizes the menu item, and saves the
 *      interim dialog values.
 *
 ************************************************************************/

static void
FilterClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )
{
   FileMgrRec * file_mgr_rec = (FileMgrRec *) client_data;
   XtPointer save_data;


   /* Free up the old interim values, and save the new ones */

   save_data = old_dialog_data->data;
   old_dialog_data->data = new_dialog_data->data;
   new_dialog_data->data = save_data;
   _DtFreeDialogData (new_dialog_data);


   /* Resensitize the associated menubutton */
   file_mgr_rec->menuStates |= FILTER;
}

void
NewFileTypeSelected (
   Widget  w,
   XtPointer client_data,
   XtPointer callback)
{
   XmAnyCallbackStruct * callback_data = (XmAnyCallbackStruct *) callback;
   Arg args[10];
   Pixel background_color;
   Pixel foreground_color;
   Pixel pixmap_background;
   FilterRec *filter_rec;
   XmManagerWidget mgr;
   FTData * user_data;

   filter_rec = (FilterRec *)client_data;
   mgr = (XmManagerWidget)filter_rec->file_window;

   if (callback_data->reason == XmCR_ACTIVATE)
   {
      XtSetArg (args[0], XmNbackground, &background_color);
      XtSetArg (args[1], XmNforeground, &foreground_color);
      XtSetArg (args[2], XmNtopShadowColor, &pixmap_background);
      XtGetValues ((Widget)mgr, args, 3);

      XtSetArg(args[0], XmNuserData, &user_data);
      XtGetValues(w, args, 1);

      if(user_data->selected)
      {
         /* Draw the old selected icon in normal state */
         XtSetArg (args[0], XmNpixmapForeground, black_pixel);
         XtSetArg (args[1], XmNforeground, foreground_color);
         XtSetArg (args[2], XmNarmColor, white_pixel);

         if (background_color == white_pixel)
         {
            XtSetArg (args[3], XmNbackground, white_pixel);
            XtSetArg (args[4], XmNpixmapBackground, white_pixel);
         }
         else if (background_color == black_pixel)
         {
            XtSetArg (args[3], XmNbackground, black_pixel);
            XtSetArg (args[4], XmNpixmapBackground, white_pixel);
         }
         else
         {
            XtSetArg (args[3], XmNbackground, background_color);
            XtSetArg (args[4], XmNpixmapBackground, pixmap_background);
         }
         XtSetValues(w, args, 5);
         user_data->selected = False;

      }
      else
      {

         /* Draw the selected icon as selected */
         if (background_color == white_pixel)
         {
            XtSetArg(args[0], XmNbackground, black_pixel);
            XtSetArg(args[1], XmNforeground, white_pixel);
         }
         else if (background_color == black_pixel)
         {
            XtSetArg(args[0], XmNbackground, white_pixel);
            XtSetArg(args[1], XmNforeground, black_pixel);
         }
         else
         {
            XtSetArg(args[0], XmNbackground, white_pixel);
            XtSetArg(args[1], XmNforeground, black_pixel);
         }
   
         XtSetArg(args[2], XmNpixmapBackground, white_pixel);
         XtSetArg(args[3], XmNpixmapForeground, black_pixel);
         XtSetArg(args[4], XmNarmColor, white_pixel);
         XtSetValues(w, args, 5);
         user_data->selected = True;
      }
   
   }
   else if (callback_data->reason == XmCR_HIGHLIGHT)
      DrawHighlight(w, NULL, NULL, NOT_DESKTOP);
   else if (callback_data->reason == XmCR_UNHIGHLIGHT)
      DrawUnhighlight(w, NOT_DESKTOP);
   else if (callback_data->reason == XmCR_SHADOW)
      DrawShadowTh(w, NULL, NOT_DESKTOP);
 
}

void
SelectAllFileTypes (
   Widget  w,
   XtPointer client_data,
   XtPointer callback)
{
   Arg args[6];
   FilterRec *filter_rec;
   Pixel background_color;
   XmManagerWidget mgr;
   FTData * user_data;
   int num_of_filetypes, i;

   filter_rec = (FilterRec *)client_data;
   mgr = (XmManagerWidget)filter_rec->file_window;
   num_of_filetypes = mgr->composite.num_children;

   XtSetArg (args[0], XmNbackground, &background_color);
   XtGetValues ((Widget)mgr, args, 1);

   for(i = 0; i < num_of_filetypes; i++)
   {
      XtSetArg(args[0], XmNuserData, &user_data);
      XtGetValues(mgr->composite.children[i], args, 1);

      if(!user_data->selected)
      {
         /* Draw the selected icon as selected */
         if (background_color == white_pixel)
         {
            XtSetArg(args[0], XmNbackground, black_pixel);
            XtSetArg(args[1], XmNforeground, white_pixel);
         }
         else if (background_color == black_pixel)
         {
            XtSetArg(args[0], XmNbackground, white_pixel);
            XtSetArg(args[1], XmNforeground, black_pixel);
         }
         else
         {
            XtSetArg(args[0], XmNbackground, white_pixel);
            XtSetArg(args[1], XmNforeground, black_pixel);
         }

         XtSetArg(args[2], XmNpixmapBackground, white_pixel);
         XtSetArg(args[3], XmNpixmapForeground, black_pixel);
         XtSetArg(args[4], XmNarmColor, white_pixel);
         XtSetValues(mgr->composite.children[i], args, 5);

         user_data->selected = True;
      }
   }

}

void
UnselectAllFileTypes (
   Widget  w,
   XtPointer client_data,
   XtPointer callback)
{
   Arg args[6];
   FilterRec *filter_rec;
   Pixel background_color;
   Pixel foreground_color;
   Pixel pixmap_background;
   XmManagerWidget mgr;
   FTData * user_data;
   int num_of_filetypes, i;

   filter_rec = (FilterRec *)client_data;
   mgr = (XmManagerWidget)filter_rec->file_window;
   num_of_filetypes = mgr->composite.num_children;

   XtSetArg (args[0], XmNbackground, &background_color);
   XtSetArg (args[1], XmNforeground, &foreground_color);
   XtSetArg (args[2], XmNtopShadowColor, &pixmap_background);
   XtGetValues ((Widget)mgr, args, 3);

   for(i = 0; i < num_of_filetypes; i++)
   {
      XtSetArg(args[0], XmNuserData, &user_data);
      XtGetValues(mgr->composite.children[i], args, 1);

      if(user_data->selected)
      {
         /* Draw the old selected icon in normal state */
         XtSetArg (args[0], XmNpixmapForeground, black_pixel);
         XtSetArg (args[1], XmNforeground, foreground_color);
         XtSetArg (args[2], XmNarmColor, white_pixel);

         if (background_color == white_pixel)
         {
            XtSetArg (args[3], XmNbackground, white_pixel);
            XtSetArg (args[4], XmNpixmapBackground, white_pixel);
         }
         else if (background_color == black_pixel)
         {
            XtSetArg (args[3], XmNbackground, black_pixel);
            XtSetArg (args[4], XmNpixmapBackground, white_pixel);
         }
         else
         {
            XtSetArg (args[3], XmNbackground, background_color);
            XtSetArg (args[4], XmNpixmapBackground, pixmap_background);
         }
         XtSetValues(mgr->composite.children[i], args, 5);

         user_data->selected = False;
      }
   }
}
