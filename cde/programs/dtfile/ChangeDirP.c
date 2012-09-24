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
/* $TOG: ChangeDirP.c /main/8 1998/04/01 15:05:07 rafi $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           ChangeDirP.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Processing functions for the change directory display
 *                   line and the current directory dialog.
 *
 *   FUNCTIONS: ABS
 *		CheckCurrentDirectorySelect
 *		CurrentDirChange
 *		CurrentDirClose
 *		CurrentDirDropCallback
 *		CurrentDirExposed
 *		CurrentDirIconCallback
 *		CurrentDirSelected
 *		CurrentDirectoryIconMotion
 *		DrawCurrentDirectory
 *		GetStatusMsg
 *		ResizeFastText
 *		ShowChangeDirDialog
 *		ShowFastChangeDir
 *		TimerEvent
 *		draw_imagestring
 *		get_text_pieces
 *		get_textwidth
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <limits.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#include <Xm/TextFP.h>
#include <Xm/PushBG.h>
#include <Xm/DragDrop.h>
#include <Xm/RowColumn.h>
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Connect.h>
#include <Dt/FileM.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include "Desktop.h"
#include "FileMgr.h"
#include "Main.h"
#include "ChangeDir.h"
#include "Prefs.h"


/********    Static Function Declarations    ********/

static void CurrentDirChange(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data,
                        XtPointer call_data) ;
static void CurrentDirClose(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;
static void CheckCurrentDirectorySelect(
                        FileMgrData *file_mgr_data ) ;
static void TimerEvent(
                        XtPointer client_data,
                        XtIntervalId *id );
static void ResizeFastText(
                        FileMgrRec *file_mgr_rec,
                        FileMgrData *file_mgr_data,
                        short columns) ;
static int get_textwidth(
			FileMgrData *fmd,
			char *str,
			int len);
static void draw_imagestring(
                        Display *display,
                        Drawable d,
                        FileMgrData *fmd,
                        GC gc,
                        int x, int y,
                        char *text,
                        int bytes);

/********    End Static Function Declarations    ********/

/* absolute value macro */
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

/* layout constants */
#define CUR_DIR_SPACING 5


/* local global varible to determine whether the user has double clicked
   or not */
static Boolean doubleClick = False;

/*--------------------------------------------------------------------
 * get_text_pieces:
 *
 *   Given a width available for the current directory text,
 *   determines the text to be drawn in one of three formats:
 *
 *     /path         (non-restricted directory)
 *     /.../path     (restricted directory)
 *     .../subpath   (if the full path wouldn't fit)
 *
 *   Returns the length (in chars) and width (in pixels) of each of
 *   the two components of the text: 
 *   (1) prefix "/..." or "...", and (2) path or subpath.
 *   Returns True if the path was chopped because it wouldn't fit.
 *
 *------------------------------------------------------------------*/

static Boolean
get_text_pieces(
	FileMgrData *file_mgr_data,
	int width,
	char buf[],
	int *host_len_p,
	int *host_pixels_p,
	int *prefix_len_p,
	int *prefix_pixels_p,
	int *path_len_p,
	int *path_pixels_p)
{
   Boolean chopped = False;
   int prefix_len;
   int prefix_pixels;
   int path_len;
   int path_pixels;
   char *path_begin;
   char *next_part = NULL;

   *host_len_p = *host_pixels_p = 0;

   /* if restricted directory, path prefix is "/..." */
   if (file_mgr_data->restricted_directory)
   {
      strcpy(buf, "/...");
      prefix_len = strlen("/...");
      prefix_pixels = get_textwidth (file_mgr_data, "/...", prefix_len);
      path_begin = file_mgr_data->current_directory +
                       strlen(file_mgr_data->restricted_directory);
      if (*path_begin == '\0')
         path_begin = "/";
      else if (*path_begin != '/')
          -- path_begin;

   }
   else
   {
      prefix_len = 0;
      prefix_pixels = 0;
      path_begin = file_mgr_data->current_directory;
   }

   /* calculate path length & width */
   path_len = strlen(path_begin);
   path_pixels = get_textwidth (file_mgr_data, path_begin, path_len);

   /* if whole path doesn't fit, we need to chop off pieces until it does */
   if (prefix_pixels + path_pixels > width)
   {
      chopped = True;

      /* change the path prefix */
      strcpy(buf, "...");
      prefix_len = strlen("...");
      prefix_pixels = get_textwidth (file_mgr_data, "...", prefix_len);

      do
      {
         /* chop off the next piece (everything up to the next '/') */
         next_part = NULL;
         next_part = DtStrchr(path_begin + 1, '/');
         if (next_part == NULL)
         {
           /* Got here only when the last directory is still too
              long to display
           */
           break;
         }

         /* calculate new path length */
         path_begin = next_part;
         path_len = strlen(path_begin);
         path_pixels = get_textwidth (file_mgr_data, path_begin, path_len);

         /* keep going until it fits */
      } while (prefix_pixels + path_pixels > width);
   }

   /* add the path to the buffer */
   if( NULL == next_part )
   {
     /* Got here only when the last directory is still too
        long to display
      */
     int len = path_len;
     char saved_char;

     while( 0 != len )
     {
       /* Going back one character at a time to see if it fit
       */
       path_pixels = get_textwidth( file_mgr_data, path_begin, len );
       if( prefix_pixels + path_pixels < width )
       {
         break;
       }
       --len;
     }

     if( 0 == len )
     {
       buf[prefix_len] = 0x0;
       *path_len_p = 0;
       *path_pixels_p = 0;
     }
     else
     {
       strncpy (buf + prefix_len, path_begin, len);
       *path_len_p      = len;
       *path_pixels_p   = path_pixels;
     }
   }
   else
   {
     strcpy (buf + prefix_len, path_begin);
     *path_len_p      = path_len;
     *path_pixels_p   = path_pixels;
   }

   /* return values */
   *prefix_len_p    = prefix_len;
   *prefix_pixels_p = prefix_pixels;
   return chopped;
}


/************************************************************************
 *
 *  ShowChangeDirDialog
 *	Callback functions invoked from the Change Directory... menu
 *	item.  This function displays the change directory dialog.
 *
 ************************************************************************/

/*ARGSUSED*/
void
ShowChangeDirDialog(
        Widget w,
        XtPointer client_data,
        XtPointer callback )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   ChangeDirData * change_dir_data;
   ChangeDirRec * change_dir_rec;
   Arg args[1];
   Widget mbar;
   char *tempStr, *tmpStr;


   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   if (w)
   {
      if((XtArgVal)client_data == FM_POPUP)
         mbar = XtParent(w);
      else
         mbar = (Widget) XmGetPostedFromWidget(XtParent(w));

      XmUpdateDisplay (w);
      XtSetArg(args[0], XmNuserData, &file_mgr_rec);
      XtGetValues(mbar, args, 1);

      /* Ignore accelerators when we're insensitive */
      if ((file_mgr_rec->menuStates & CHANGEDIR) == 0)
      {
        XSetInputFocus(XtDisplay(w),
                       XtWindow(file_mgr_rec->change_directoryBtn_child),
                       RevertToParent, CurrentTime);
        return;
      }
   }
   else
   {
      /* Done only during a restore session */
      file_mgr_rec = (FileMgrRec *)client_data;
   }

   /* Got an accelerator after we were unposted */
   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;

   file_mgr_data = (FileMgrData *) dialog_data->data;
   change_dir_data = (ChangeDirData *) file_mgr_data->change_dir->data;
   change_dir_data->file_mgr_rec = (XtPointer) file_mgr_rec;

   file_mgr_rec->menuStates &= ~CHANGEDIR;

   _DtShowDialog (file_mgr_rec->shell, (Widget)NULL, (XtPointer)file_mgr_rec,
               file_mgr_data->change_dir,
	       CurrentDirChange, (XtPointer)file_mgr_rec,
               CurrentDirClose, (XtPointer)file_mgr_rec, (char *)NULL,
	       False, False, (char *)NULL, (XClassHint *)NULL);

   /* Save a ptr to file_mgr_rec in the find dialogs structure */
   change_dir_rec = (ChangeDirRec *)_DtGetDialogInstance(
                                                   file_mgr_data->change_dir);

   if(file_mgr_data->title != NULL &&
               strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
      tmpStr = GETMESSAGE(2,15, "Go To");
      tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
      sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
      tmpStr = (GETMESSAGE(2,17, "File Manager - Go To"));
      tempStr = XtNewString(tmpStr);
   }
   XtSetArg (args[0], XmNtitle, tempStr);
   XtSetValues (XtParent (change_dir_rec->change_dir), args, 1);
   XtFree(tempStr);

   file_mgr_rec->change_directoryBtn_child=XtParent (change_dir_rec->change_dir);
}




/************************************************************************
 *
 *  CurrentDirSelected
 *	When a Button1 selection occurs on the current directory line,
 *	see if it occurred within the directory path, highlight the
 *	selected sub-path, or if the sub-path was already highlighted,
 *	set the current directory to the path and dehighlight.
 *
 ************************************************************************/

void
CurrentDirSelected(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec  *file_mgr_rec = (FileMgrRec *) client_data;
   DialogData  *dialog_data;
   FileMgrData *file_mgr_data;
   char buf[2*MAX_PATH];
   char host_name[MAX_PATH];
   Boolean chopped;
   int host_len;
   int host_pixels;
   int prefix_len;
   int prefix_pixels;
   int path_len;
   int path_pixels;

   XmDrawnButtonCallbackStruct *button_data;
   XButtonEvent *event;
   Dimension width, highlight, shadow, margin;
   Arg args[4];

   int left_margin;
   int begin_x;
   int end_x;
   int len;
   int i;
   char *ptr;
   char *new_select;
   int swidth;
   static XtIntervalId TimerId;

   /* if doubleClick is true than we have a double click so we want
    * to change to the new directory that the user double clicked on
    */
   if (doubleClick)
      XtRemoveTimeOut(TimerId);

   button_data = (XmDrawnButtonCallbackStruct *) call_data;
   event = (XButtonEvent *) button_data->event;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   /* Get layout values */
   XtSetArg (args[0], XmNwidth, &width);
   XtSetArg (args[1], XmNhighlightThickness, &highlight);
   XtGetValues (w, args, 2);
   XtSetArg (args[0], XmNshadowThickness, &shadow);
   XtSetArg (args[1], XmNmarginWidth, &margin);
   XtGetValues (file_mgr_rec->current_directory_text, args, 2);
   left_margin = highlight + shadow + margin;

   /*  Get the starting and ending locations of the current  */
   /*  directory text.                                       */
   chopped = get_text_pieces(file_mgr_data, width - 2*left_margin,
               buf, &host_len, &host_pixels, &prefix_len, &prefix_pixels,
               &path_len, &path_pixels);

   begin_x = left_margin;
   end_x = begin_x + host_pixels + prefix_pixels + path_pixels;

   /*  Get the selected path  */
   if (event->x < begin_x || event->x >= end_x)
   {
      /* click outside the directory text: nothing selected */
      new_select = NULL;
   }
   else if (event->x < begin_x + host_pixels ||
            event->x < begin_x + host_pixels + prefix_pixels && !chopped)
   {
      /* click on host name or "/..." prefix: root selected */
      if (file_mgr_data->restricted_directory)
         new_select = XtNewString(file_mgr_data->restricted_directory);
      else
         new_select = XtNewString("/");
   }
   else if (event->x < begin_x + host_pixels + prefix_pixels && chopped)
   {
      /* click on "..." prefix: directory above the visible piece selected */
      len = strlen(file_mgr_data->current_directory) - path_len;
      new_select = (char *) XtMalloc(len + 1);
      memcpy(new_select, file_mgr_data->current_directory, len);
      new_select[len] = 0;
   }
   else /* event->x >= begin_x + host_pixels + prefix_pixels */
   {
      /* click on the path: determine which subdirectory selected */
      begin_x += host_pixels + prefix_pixels;
      i = host_len + prefix_len;
      swidth = get_textwidth(file_mgr_data, "/", strlen("/")) / 2;

      while (begin_x - swidth < event->x)
      {
         /* find next '/' in path */
         ptr = DtStrchr(buf + i, '/');
         if (ptr == NULL)
         {
           i = host_len + prefix_len + path_len + 1;
           break;
         }

         /* get x-position of next path component */
         len = ((ptr + 1) - buf) - i;
         begin_x += get_textwidth(file_mgr_data, buf + i, len);
         i += len;
      }

         /* if we have a restricted diretory and i == 5 ("/.../") then we want
            the len to be the restricted directory */
      if (file_mgr_data->restricted_directory &&  i == 5)
         len = strlen(file_mgr_data->restricted_directory);
      else
         len = strlen(file_mgr_data->current_directory)
             - (host_len + prefix_len + path_len - i) - 1;
      new_select = (char *) XtMalloc(len + 1);
      if (len == 0)
         strcpy(new_select, "/");
      else
      {
         memcpy(new_select, file_mgr_data->current_directory, len);
         new_select[len] = 0;
      }
   }

   /* in restricted mode, don't allow going above the user's home dir */
   if (new_select != NULL && restrictMode)
   {
      /* check if new_select is the same as or a subdirectory of $HOME */
      len = strlen(users_home_dir);
      if (strncmp(new_select, users_home_dir, len) != 0
           || new_select[len] != '\0' && new_select[len] != '/')
      {
         /* change new_select to $HOME */
         XtFree(new_select);
         new_select = XtNewString(users_home_dir);
      }
   }


   /*  If the path is the same as what is already selected,  */
   /*  free cd_select, set the directory to the selected     */
   /*  directory, redraw the directory display.              */

   /*  If the path was different, set cd_select to the       */
   /*  selected directory and redraw the directory.          */

   if (new_select != NULL && file_mgr_data->cd_select != NULL &&
       strcmp(new_select, file_mgr_data->cd_select) == 0)
   {
      XtFree (file_mgr_data->cd_select);
      file_mgr_data->cd_select = NULL;

      strcpy(buf, new_select);
      strcpy(host_name, file_mgr_data->host);

      if (strcmp (buf, file_mgr_data->current_directory) == 0)
         FileMgrReread (file_mgr_rec);
      else
         ShowNewDirectory (file_mgr_data, host_name, buf);

      XtFree (new_select);
   }
   else
   {
      XtFree (file_mgr_data->cd_select);
      file_mgr_data->cd_select = new_select;
   }

   if (doubleClick)
   {
      doubleClick = False;
      DrawCurrentDirectory (w, file_mgr_rec, file_mgr_data);
   }
   else
   {
      doubleClick = True;
      TimerId = XtAppAddTimeOut (XtWidgetToApplicationContext (w), 
                                 XtGetMultiClickTime(XtDisplay(w)),
                                 (XtTimerCallbackProc) TimerEvent, 
                                 (XtPointer) file_mgr_rec);
   }
}



/************************************************************************
 *
 *  CurrentDirDropCallback
 *	Callback function invoked upon an action on the change view drop.
 *
 ************************************************************************/

void
CurrentDirDropCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   XmAnyCallbackStruct * callback;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   char host_name[MAX_PATH];

   callback = (XmAnyCallbackStruct *) call_data;
   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   if (callback->reason == XmCR_DEFAULT_ACTION)
   {
      strcpy(host_name, file_mgr_data->host);
      ShowNewDirectory (file_mgr_data, host_name,
                        _DtPName (file_mgr_data->current_directory));
   }
}


/************************************************************************
 *
 *  CurrentDirIconCallback
 *	Callback function invoked upon an action occuring on an icon.
 *
 ************************************************************************/

void
CurrentDirIconCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   XmAnyCallbackStruct * callback;
   XButtonEvent        * event;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;

   callback = (XmAnyCallbackStruct *) call_data;
   event = (XButtonEvent *) callback->event;
   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   if (callback->reason == XmCR_DRAG)
   {
      /* Do nothing if a Button 1 drag is already ramping up */
      if (B1DragPossible)
         return;

      /* Save starting X and Y, for threshold detection */
      initialDragX = event->x;
      initialDragY = event->y;

      /* Flag that a Button 2 drag is ramping up */
      B2DragPossible = True;

      UnpostTextField(file_mgr_data);
   }
   else if (callback->reason == XmCR_ARM)
   {
      /* Do nothing if a Button 2 drag is already ramping up */
      if (B2DragPossible)
         return;

      /* Save starting X and Y, for threshold detection */
      initialDragX = event->x;
      initialDragY = event->y;

      /* Flag that a Button 1 drag is ramping up */
      B1DragPossible = True;
      
      /* but since we're in the current directory icon we don't want to
         process on the button up */
      ProcessBtnUpCD = False;

      UnpostTextField(file_mgr_data);
   }
   else if (callback->reason == XmCR_DEFAULT_ACTION)
   {
      /* We now know that a drag operation won't be starting up */
      B1DragPossible = False;
      B2DragPossible = False;

      UnpostTextField(file_mgr_data);

      /* Default action is to reread the directory */
      FileMgrReread (file_mgr_rec);
   }
   else if ((callback->reason == XmCR_SELECT) || 
            (callback->reason == XmCR_DISARM) ||
            (callback->reason == XmCR_DROP))
   {
      /* We now know that a drag operation won't be starting up */
      B1DragPossible = False;
      B2DragPossible = False;
   }
}


/*
 * This function processes motion events anytime a B1 or B2 drag operation
 * has the potential of starting.  When the drag threshold is surpassed,
 * a drag operation will be started.
 */

void
CurrentDirectoryIconMotion(
                        Widget w,
                        XtPointer client_data,
                        XEvent *event)
{

   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   Pixmap drag_pixmap;
   char * type_set;
   char * file_set;
   int diffX, diffY;
   Widget dirIcon;
   DtIconGadget iconG;


   if ((B1DragPossible && (event->xmotion.state & Button1Mask)) ||
       (B2DragPossible && (event->xmotion.state & Button2Mask)))
   {
      /* Have we passed the drag threshold? */
      diffX = initialDragX - event->xmotion.x;
      diffY = initialDragY - event->xmotion.y;
  
      if ((ABS(diffX) >= dragThreshold) || (ABS(diffY) >= dragThreshold))
      {
         dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
         file_mgr_data = (FileMgrData *) dialog_data->data;

	 initiating_view = (XtPointer) file_mgr_data;
         dirIcon = file_mgr_rec->current_directory_icon;
	 StartDrag(dirIcon, NULL, event);
      }
   }
}


/************************************************************************
 *
 *  CurrentDirExposed
 *	Callback functions invoked from the current directory display
 *	drawn button.  This function extracts some structures and calls
 *	the directory display function.
 *
 ************************************************************************/

void
CurrentDirExposed(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);

   /*  Check to see if the view has been closed  */

   if (dialog_data == NULL) return;

   file_mgr_data = (FileMgrData *) dialog_data->data;

   if (file_mgr_data->cd_normal_gc != 0)
      DrawCurrentDirectory (w, file_mgr_rec, file_mgr_data);
}




/************************************************************************
 *
 * GetStatusMsg
 *   Construct the status message (normally "x Items, y Hidden").
 *   Returns True if the message is deemed "important".
 *
 *   @@@ Note 2/17/95: messages below containing "Item(s)" should really
 *   be just "Item", but the message catalog is frozen at this time, so
 *   we can't fix this now.  (In practice, it doesn't matter, because
 *   the item count should always be > 1, since every directory shoule
 *   contain at least two files "." and "..")
 *
 ************************************************************************/

Boolean
GetStatusMsg(
        FileMgrData *file_mgr_data,
        char *buf )
{
   int n_files;
   int n_hidden;
   int i, j;
   TreeShow ts;
   FileViewData **file_view_data;

   /*
    * If we are currently busy reading a directory, display a progress
    * message instead of the normal "x Items, y Hidden" message.
    */
   if (file_mgr_data->busy_status == initiating_readdir ||
       file_mgr_data->busy_status == busy_readdir)
   {
      if (file_mgr_data->busy_detail == 0)
         sprintf (buf, (GETMESSAGE(3,5, "Reading ...")));
      else if (file_mgr_data->busy_detail == 1)
        sprintf (buf, (GETMESSAGE(3,11, "%d Item(s)...")),
                 file_mgr_data->busy_detail);
      else
        sprintf (buf, (GETMESSAGE(3,9, "%3d Items ...")),
                 file_mgr_data->busy_detail);

      return True;  /* this message deemed important! */
   }

   else if (file_mgr_data->show_type == MULTIPLE_DIRECTORY)
   {
      /*
       * In tree mode, we only show a count of the hidden files
       * in branches that are currently expanded.
       * The idea is we want to show how many additional files would
       * show up if the user turned on the "Show Hidden" option.
       */
      n_hidden = 0;
      for (i = 0; i < file_mgr_data->directory_count; i++)
      {
         file_view_data = file_mgr_data->directory_set[i]->file_view_data;
         if (file_view_data == NULL)
            continue;

         ts = file_mgr_data->directory_set[i]->sub_root->ts;
         if (ts < tsDirs)
            continue;   /* this branch is not expanded */

         for (j = 0; j < file_mgr_data->directory_set[i]->file_count; j++)
         {
            if (file_view_data[j]->filtered &&
                (ts == tsAll || file_view_data[j]->file_data->is_subdir))
            {
               n_hidden++;
            }
         }
      }
      sprintf (buf, (GETMESSAGE(3,6, "%d Hidden")), n_hidden);

      return False;
   }

   else
   {
      /*
       * In flat mode, we only show a total count of all files
       * and a count of hidden files.
       */
      n_files = file_mgr_data->directory_set[0]->file_count;
      if( n_files == 0 )
        sprintf( buf, (GETMESSAGE(11,31, "Error while reading %s")), file_mgr_data->current_directory );
      else if( file_mgr_data == trashFileMgrData )
      {
        n_hidden = file_mgr_data->directory_set[0]->filtered_file_count;
        sprintf (buf, (GETMESSAGE(3,10, "%d Item(s)")),
                 n_files - n_hidden);
      }
      else
      {
        n_files -= file_mgr_data->directory_set[0]->invisible_file_count;
        n_hidden = file_mgr_data->directory_set[0]->filtered_file_count -
          file_mgr_data->directory_set[0]->invisible_file_count;
        if (n_files == 1)
          sprintf (buf, (GETMESSAGE(3,12, "%d Item(s) %d Hidden")),
                   n_files, n_hidden);
        else
          sprintf (buf, (GETMESSAGE(3,7, "%d Items %d Hidden")),
                 n_files, n_hidden);
      }

      return False;
   }
}


/************************************************************************
 *
 *  DrawCurrentDirectory
 *	Draw the current directory display area, including the hostname,
 *	the current directory, any highlighted sub-path of the directory,
 *	the file count and the number of selected files.
 *
 ************************************************************************/

void
DrawCurrentDirectory(
        Widget w,
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data )
{
   Arg args[8];
   Dimension width, height, highlight, shadow, margin, twidth;
   XFontSetExtents *extents;
   int font_height;
   int font_yoffset;
   int top_margin;
   int left_margin;
   char buf[2*MAX_PATH];
   char msg[21+MAX_PATH];
   Boolean chopped;
   int host_len;
   int host_pixels;
   int prefix_len;
   int prefix_pixels;
   int path_len;
   int path_pixels;
   int draw_x;
   int draw_y;
   int dir_width;
   int msg_width;
   short columns;
   Boolean msg_drawn;

   /* Get layout values */
   XtSetArg (args[0], XmNwidth, &width);
   XtSetArg (args[1], XmNheight, &height);
   XtSetArg (args[2], XmNhighlightThickness, &highlight);
   XtGetValues (w, args, 3);
   XtSetArg (args[0], XmNshadowThickness, &shadow);
   XtSetArg (args[1], XmNmarginWidth, &margin);
   XtGetValues (file_mgr_rec->current_directory_text, args, 2);

   if(file_mgr_data->cd_fonttype == XmFONT_IS_FONTSET) {
       extents = XExtentsOfFontSet(file_mgr_data->cd_fontset);
       font_yoffset = -(extents->max_logical_extent.y);
       font_height = extents->max_logical_extent.height;
   }
   else
   {
       font_yoffset = file_mgr_data->cd_font->ascent;
       font_height = file_mgr_data->cd_font->ascent +
                       file_mgr_data->cd_font->descent;
   }
   top_margin = (height > (Dimension)font_height)? (Dimension)(height - font_height + 1)/(Dimension)2: 0;
   left_margin = highlight + shadow + margin;

   /* Ensure the area is cleared out. */
   XClearArea (XtDisplay (w), XtWindow (w),
               highlight, highlight,
               width - 2*highlight, height - 2*highlight,
               False);

   /*
    * If there is no status line and no iconic path,
    * we will want yo draw the "x Files y Hidden" message here.
    */
   if (!file_mgr_data->show_iconic_path && !file_mgr_data->show_status_line)
   {
      /*
       * GetStatusMsg() returns True if the status msg is "important".
       * In this case, make sure we leave room for it
       */
      msg_drawn = GetStatusMsg(file_mgr_data, msg);
      msg_width = get_textwidth (file_mgr_data, msg, strlen(msg));
   }
   else
      msg_drawn = False;

   draw_x = left_margin;
   if( draw_x < 0 || (Dimension) draw_x > width ) /* Make sure that it's in bound */
     draw_x = 0;

   draw_y = top_margin + font_yoffset;
   if( draw_y < 0 || (Dimension) draw_y > height ) /* Make sure that it's in bound */
   {
     draw_y = height-5;
   }

   /* get the text pieces */
   dir_width = width - 2*left_margin;
   if (msg_drawn)
      dir_width -= CUR_DIR_SPACING + msg_width;
   chopped = get_text_pieces(file_mgr_data, dir_width,
               buf, &host_len, &host_pixels, &prefix_len, &prefix_pixels,
               &path_len, &path_pixels);

   /*
    * go check and change the file_mgr_data->cd_select, make sure its not
    * longer than the file_mgr_data->current_directory.
    */
   CheckCurrentDirectorySelect(file_mgr_data);

   /* draw the host and paths */
   if (!file_mgr_data->fast_cd_enabled)
   {
      draw_imagestring (XtDisplay (w), XtWindow (w), file_mgr_data,
			file_mgr_data->cd_normal_gc, draw_x, draw_y,
			buf, host_len + prefix_len + path_len);
   }
   else if (file_mgr_data->restricted_directory)
   {
      draw_imagestring (XtDisplay (w), XtWindow (w), file_mgr_data,
                        file_mgr_data->cd_normal_gc, draw_x, draw_y,
                        buf, host_len + prefix_len);
   }

   /*
    * If there is no status line and no iconic path, and we have
    * room left, draw the "x Files y Hidden" message here.
    */
   if (!file_mgr_data->show_iconic_path && !file_mgr_data->show_status_line)
   {
      /* determine where the message could begin */
      if (!file_mgr_data->fast_cd_enabled)
         draw_x += host_pixels + prefix_pixels + path_pixels;
      else
      {
         XtSetArg(args[0], XmNwidth, &twidth);
         XtGetValues(file_mgr_rec->current_directory_text, args, 1);
         if (file_mgr_data->restricted_directory && !chopped)
            draw_x += host_pixels + prefix_pixels + twidth;
         else
            draw_x += host_pixels + twidth;
      }

      /* if there is enough space left, draw the message */
      if ((Dimension)(draw_x + CUR_DIR_SPACING + msg_width + left_margin) <= width)
      {
         draw_x = width - left_margin - msg_width;
         draw_imagestring (XtDisplay (w), XtWindow (w), file_mgr_data,
                           file_mgr_data->cd_normal_gc, draw_x, draw_y,
                           msg, strlen(msg));
         draw_x -= shadow + CUR_DIR_SPACING;  /* right edge of shadow */
         msg_drawn = True;
      }
      else
         msg_drawn = False;
   }

   if (!msg_drawn)
      draw_x = width - (highlight + shadow);

   /* draw the shadow */
   if (!file_mgr_data->fast_cd_enabled)
   {
      int shadow_width = draw_x - highlight;
      int shadow_height = height - 2*highlight;
      XmTextFieldWidget tf =
             (XmTextFieldWidget)file_mgr_rec->current_directory_text;

      XmeDrawShadows(XtDisplay(w), XtWindow(w),
                     tf->primitive.top_shadow_GC,
                     tf->primitive.bottom_shadow_GC,
                     highlight, highlight, shadow_width, shadow_height,
                     shadow, XmSHADOW_IN);
   }
}



/************************************************************************
 *
 *  CurrentDirChange
 *	Callback functions invoked from the current directory dialog's
 *	apply button being pressed.  This function updates and redisplays
 *	the current directory information.
 *
 ************************************************************************/

static void
CurrentDirChange(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   ChangeDirData * new_change_dir_data;
   ChangeDirData * old_change_dir_data;
   char path[MAX_PATH];
   char host_name[MAX_PATH];
   char * ptr;


   /*  Get a pointer file manager's data structure, free up the   */
   /*  old current directory and copy in a new one.  Free up the  */
   /*  old selected sub-path, update the file manager's internal  */
   /*  data, and redraw the directory.                            */

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   if (file_mgr_data->cd_select != NULL)
   {
      XtFree (file_mgr_data->cd_select);
      file_mgr_data->cd_select = NULL;
   }

   old_change_dir_data = (ChangeDirData *) file_mgr_data->change_dir->data;
   new_change_dir_data = (ChangeDirData *) new_dialog_data->data;
   
   new_change_dir_data->host_name = XtNewString(old_change_dir_data->host_name);
   new_change_dir_data->file_mgr_rec = old_change_dir_data->file_mgr_rec;

   _DtHideDialog(old_dialog_data, False);

   new_change_dir_data->displayed = False;

   file_mgr_data->change_dir->data = (XtPointer) new_change_dir_data;
   new_dialog_data->data = (XtPointer) old_change_dir_data;
   _DtFreeDialogData (new_dialog_data);
   file_mgr_rec->menuStates |= CHANGEDIR;


   /*  Process call_data into a hostname and directory name.  */

   ShowNewDirectory (file_mgr_data,
                     ((ChangeDirData *)file_mgr_data->change_dir->data)->host_name,
                     call_data);
}




/************************************************************************
 *
 *  CurrentDirClose
 *	Callback functions invoked from the current directory dialog's
 *	close button being pressed.  This function resensitizes the
 *	Change Directory... menu item.
 *
 ************************************************************************/

/*ARGSUSED*/
static void
CurrentDirClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )
{
   FileMgrRec * file_mgr_rec = (FileMgrRec *) client_data;

   _DtFreeDialogData (new_dialog_data);
   file_mgr_rec->menuStates |= CHANGEDIR;
}

/************************************************************************
 *
 *  CheckCurrentDirectorySelect
 *           Before calling DrawCurrentDirectorySelect() this function
 *           makes sure that the fm->cd_select isn't longer than the
 *           fm->current_directory.  If it is it reconfigures fm->cd_select
 *           to hold no more than what fm->current_directory is.
 *
 ************************************************************************/
static void
CheckCurrentDirectorySelect( 
         FileMgrData *file_mgr_data )
{
   int length_cd, length_cd_s;
   char *str, *ptr;

   if (file_mgr_data            == NULL ||
       file_mgr_data->cd_select == NULL ||
       file_mgr_data->current_directory == NULL)
       return;

   /* get the true lengths of current_directory and current_directory_select */
   length_cd = strlen(file_mgr_data->current_directory);
   length_cd_s = strlen(file_mgr_data->cd_select);
   
   /* if cd is larger than cd_select than we have now problem */
   if(length_cd >= length_cd_s)
      return;

   /* we need to recalculate the cd_select */
   str = XtNewString(file_mgr_data->cd_select);
   while(1)
   {
      ptr = strrchr(str, '/');
      *ptr = '\0';
      length_cd_s = strlen(str);
      if(length_cd > length_cd_s)
      {
         XtFree(file_mgr_data->cd_select);
         file_mgr_data->cd_select = (char *)XtMalloc(strlen(str) + 1);
         strcpy(file_mgr_data->cd_select, str);
         XtFree(str);
         return;
      }
   }
}

/************************************************************************
 *
 *  ShowFastChangeDir
 *      Post the fast change to text widget.
 *
 ***************************************************************************/
void
ShowFastChangeDir(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data )
{
   char *textString;
   Arg args[16];
   Dimension width, height;
   Dimension shadow, highlight, margin;
   char buf[2*MAX_PATH];
   Boolean chopped;
   int host_len;
   int host_pixels;
   int prefix_len;
   int prefix_pixels;
   int path_len;
   int path_pixels;
   int begin_x;
   int left_margin;

   doubleClick = False;

   XtRemoveAllCallbacks (file_mgr_rec->current_directory, XmNexposeCallback);

   XtFree (file_mgr_data->cd_select);
   file_mgr_data->cd_select = NULL;

   file_mgr_data->fast_cd_enabled = True;

   /* if not a toolbox, just put the current directory in text widget */
   if (file_mgr_data->restricted_directory == NULL)
   {
      if (strcmp(file_mgr_data->host, home_host_name) == 0)
         textString = XtNewString(file_mgr_data->current_directory);
      else
         textString = DtCreateContextString(file_mgr_data->host,
                                            file_mgr_data->current_directory,
                                            NULL);
   }
   else  /* is a toolbox, so put the subset of what the toolbox is in the text*/
   {
      char *ptr;

      ptr = file_mgr_data->current_directory +
                        strlen(file_mgr_data->restricted_directory);
      if (strcmp(ptr, "") == 0)
         textString = XtNewString("/");
      else
         textString = XtNewString(ptr);
   }
   begin_x = get_textwidth (file_mgr_data, textString, strlen (textString));

   /* Get layout values */
   XtSetArg (args[0], XmNwidth, &width);
   XtSetArg (args[1], XmNhighlightThickness, &highlight);
   XtGetValues (file_mgr_rec->current_directory, args, 2);
   XtSetArg (args[0], XmNshadowThickness, &shadow);
   XtSetArg (args[1], XmNmarginWidth, &margin);
   XtGetValues (file_mgr_rec->current_directory_text, args, 2);
   left_margin = highlight + shadow + margin;

   if(file_mgr_data->restricted_directory == NULL)
      XtSetArg (args[0], XmNleftOffset, 0);
   else
   {
      XtSetArg (args[0], XmNshadowThickness, &shadow);
      XtSetArg (args[1], XmNhighlightThickness, &highlight);
      XtGetValues(file_mgr_rec->current_directory_text, args, 2);
      chopped =
        get_text_pieces(file_mgr_data, width - 2*left_margin,
                  buf, &host_len, &host_pixels, &prefix_len, &prefix_pixels,
                  &path_len, &path_pixels);
      begin_x = left_margin + host_pixels - shadow - highlight;
      if (!chopped)
        begin_x += prefix_pixels;
      XtSetArg (args[0], XmNleftOffset, begin_x);
   }
   XtSetArg (args[1], XmNvalue, textString);
   XtSetValues(file_mgr_rec->current_directory_text, args, 2);
   XtSetArg (args[0], XmNcursorPosition, strlen(textString));
   XtSetValues(file_mgr_rec->current_directory_text, args, 1);

   XtSetArg (args[0], XmNallowShellResize, False);
   XtSetValues(file_mgr_rec->shell, args, 1);
   XtManageChild(file_mgr_rec->current_directory_text);
   XtSetArg (args[0], XmNallowShellResize, True);
   XtSetValues(file_mgr_rec->shell, args, 1);
   XRaiseWindow(XtDisplay(file_mgr_rec->current_directory_text),
                XtWindow(file_mgr_rec->current_directory_text));
   XmUpdateDisplay(file_mgr_rec->current_directory_text);
   XmProcessTraversal(file_mgr_rec->current_directory_text,
                         XmTRAVERSE_CURRENT);
   XtFree(textString);
   XtAddCallback (file_mgr_rec->current_directory, XmNexposeCallback,
					  CurrentDirExposed, file_mgr_rec);
}

/************************************************************************
 *
 *  TimerEvent - timeout for double click on current Directory line.  If
 *      we get here we know it was a single click so lets post the
 *      fast change to text widget.
 *
 ***************************************************************************/
static void
TimerEvent(
        XtPointer client_data,
        XtIntervalId *id )
{
   FileMgrRec *file_mgr_rec = (FileMgrRec *)client_data;
   DialogData  * dialog_data;
   FileMgrData *file_mgr_data;

   doubleClick = False;

   /* Got an accelerator after we were unposted */
   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;
   file_mgr_data = (FileMgrData *) dialog_data->data;

   ShowFastChangeDir(file_mgr_rec, file_mgr_data);
}

/************************************************************************
 *
 *  ResizeFastText - resizes the fast change text widget due to changes
 *       in the size of the FileManager window.
 *
 *************************************************************************/
static void
ResizeFastText(
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data,
        short columns)
{
   Arg args[2];
   Dimension width;
   int left_offset;

   /* nothing to do if not managed */
   if (!XtIsManaged(file_mgr_rec->current_directory_text))
      return;

   /* get width of current directory line */
   XtSetArg (args[0], XmNwidth, &width);
   XtGetValues (file_mgr_rec->current_directory, args, 1);

   /* get offset of the text widget */
   XtSetArg(args[0], XmNleftOffset, &left_offset);
   XtGetValues(file_mgr_rec->current_directory_text, args, 1);

   /* set text widget width = current_directory width minus left offset */
   XtSetArg (args[0], XmNwidth, width - left_offset);
   XtSetValues (file_mgr_rec->current_directory_text, args, 1);
}


/*--------------------------------------------------------------------
 * get_textwidth
 *------------------------------------------------------------------*/

/* use Xmb functions if XFontSet is used. */
static int
get_textwidth( FileMgrData *fmd,
               char *str,
               int len)
{
    int w = 0;

    switch(fmd->cd_fonttype)
    {
        case XmFONT_IS_FONTSET:
            w = XmbTextEscapement(fmd->cd_fontset, str, len);
            break;
        case XmFONT_IS_FONT:
            w = XTextWidth(fmd->cd_font, str, len);
        default:
            break;
    }
    return(w);
}

static void
draw_imagestring( Display *display,
                  Drawable d,
                  FileMgrData *fmd,
                  GC gc,
                  int x, int y,
                  char *text,
                  int bytes)
{
    switch(fmd->cd_fonttype)
    {
        case XmFONT_IS_FONTSET:
            XmbDrawImageString(display, d, fmd->cd_fontset, gc, x, y, text,
                               bytes);
            break;
        case XmFONT_IS_FONT:
            XDrawImageString(display, d, gc, x, y, text, bytes);
        default:
            break;
    }
}

