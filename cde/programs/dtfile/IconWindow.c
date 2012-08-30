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
/* $XConsortium: IconWindow.c /main/13 1996/10/15 10:07:48 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           IconWindow.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Processing functions for callbacks from the
 *                   icon display area.
 *
 *   FUNCTIONS: CheckForBtnUp
 *		DrawHighlight
 *		DrawRectangle
 *		DrawShadowTh
 *		DrawUnhighlight
 *		FM_XmHighlightBorder
 *		FM_XmUnhighlightBorder
 *		FileWindowExposeCallback
 *		FileWindowInputCallback
 *		FileWindowMapCallback
 *		FileWindowResizeCallback
 *		GetSelectionRectangle
 *		ProcessSelection
 *		WidgetRectToRegion
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/TextF.h>
#include <Xm/VirtKeys.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Action.h>

#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <Xm/PrimitiveP.h>  /* For HighlightBorder & UnhighlightBorder */

#include "Encaps.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Prefs.h"
#include "Common.h"
#include "Filter.h"

                           /* For HightlightBorder & UnhighlightBorder */
externalref XmPrimitiveClassRec xmPrimitiveClassRec;

Boolean TurnOnTextName;

/********    Static Function Declarations    ********/

static Boolean CheckForBtnUp(
                        Display *display,
                        XEvent *event,
                        XtPointer arg) ;
static void GetSelectionRectangle(
                        Display *display,
                        Window window,
                        XButtonEvent *event,
                        FileMgrRec *file_mgr_rec,
                        FileMgrData *file_mgr_data,
                        Boolean toggleSelect) ;
static void DrawRectangle(
                        Display *display,
                        Window window,
                        GC gc,
                        int x1,
                        int y1,
                        int x2,
                        int y2) ;
static void ProcessSelection(
                        FileMgrData * file_mgr_data,
                        Widget icon,
                        Boolean toggleSelect,
                        XButtonEvent *event) ;

static void FM_XmHighlightBorder(Widget w);
static void FM_XmUnhighlightBorder(Widget w);

static XmGadget InputForGadget(
			Widget wid,
			int x,
			int y ) ;


/********    End Static Function Declarations    ********/


/************************************************************************
 *
 *  FileWindowExposeCallback
 *	Callback function invoked upon an exposure event occuring
 *	in the file display area.
 *
 ************************************************************************/

void
FileWindowExposeCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   XmDrawingAreaCallbackStruct * da_cbs;
   register int i;

   file_mgr_rec = (FileMgrRec *) client_data;
   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);

   /*  Check for the view already being closed  */

   if (dialog_data == NULL) return;

   file_mgr_data = (FileMgrData *) dialog_data->data;
   da_cbs = (XmDrawingAreaCallbackStruct * )call_data;

   /* if necessary, redraw tree lines */
   if (da_cbs->reason == XmCR_EXPOSE &&
       file_mgr_data->show_type == MULTIPLE_DIRECTORY)
   {
     XExposeEvent *event = (XExposeEvent *)da_cbs->event;
     if (event && event->type == Expose)
       RedrawTreeLines(w, event->x, event->y, event->width, event->height,
                       event->count, file_mgr_rec, file_mgr_data);
     else
       RedrawTreeLines(w, 0, 0, 999999, 999999, 0, file_mgr_rec, file_mgr_data);
   }

}




/************************************************************************
 *
 *  FileWindowResizeCallback
 *	Callback function invoked upon a resize of the  file display area.
 *
 ************************************************************************/

void
FileWindowResizeCallback(
        Widget w,
        XtPointer client_data,
        XEvent *event )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;

   if (event->type == ConfigureNotify)
   {
      XConfigureEvent * cevent = (XConfigureEvent *) event;

      file_mgr_rec = (FileMgrRec *) client_data;

      /*
       * If the incoming size is different than the current size of the
       * scrolled window, then ignore this event, since it means that our
       * size has changed since this configure notify event was generated;
       * we'll wait for the next one to arrive, and try again.
       */
      if ((file_mgr_rec->scroll_window->core.height != cevent->height) ||
          (file_mgr_rec->scroll_window->core.width != cevent->width))
      {
         return;
      }

      dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
      if(dialog_data != NULL)
      {
         file_mgr_data = (FileMgrData *) dialog_data->data;

         LayoutFileIcons (file_mgr_rec, file_mgr_data, False, False);
      }
   }
}


/************************************************************************
 *
 *  FileWindowMapCallback
 *      Callback function invoked upon a map or unmap of a file
 *      manager window.
 *
 ************************************************************************/

void
FileWindowMapCallback(
        Widget w,
        XtPointer client_data,
        XEvent *event )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;

   file_mgr_rec = (FileMgrRec *) client_data;
   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   if(dialog_data != NULL)
   {
      file_mgr_data = (FileMgrData *) dialog_data->data;

      if (event->type == MapNotify)
      {
         file_mgr_data->mapped = True;
         FileWindowMapUnmap( file_mgr_data );
      }
      else if (event->type == UnmapNotify)
      {
         file_mgr_data->mapped = False;
         FileWindowMapUnmap( file_mgr_data );
      }
   }
}



/************************************************************************
 *
 *  FileWindowInputCallback
 *	Callback function invoked upon input occuring in the 
 *	file display area, file icon, or within a desktop icon.
 *
 ************************************************************************/

void
FileWindowInputCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data)
{
   static Boolean first = True;
   static KeySym PlusKeysym, MinusKeySym;
   static KeySym ActivateKeysym, SpaceKeysym;
   static XmKeyBinding SelectBindings;
   static int NumSelectKeys = 0;

   FileMgrRec  * file_mgr_rec;
   XmAnyCallbackStruct * callback;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   DirectorySet * directoryData;
   XEvent *new_event;
   XButtonPressedEvent *event;
   XKeyEvent *kevent;
   Widget eventIcon;
   int i, j;
   FileViewData * fileViewData = NULL;
   Arg args[10];
   DesktopRec * desktopRec;
   KeySym keysym;
   Modifiers modif;
   int offset;
   Boolean found;

   /* Do nothing if a Button 2 drag is already ramping up */
   if (B2DragPossible)
      return;

   /* Determine if this is a desktop icon or a regular icon */
   file_mgr_rec = NULL;
   desktopRec = NULL;
   for (i = 0; i < desktop_data->numIconsUsed; i++)
   {
      if (desktop_data->desktopWindows[i] == (DesktopRec *)client_data)
      {
         desktopRec = (DesktopRec *)client_data;
         break;
      }
   }
   if (desktopRec == NULL)
   {
      file_mgr_rec = (FileMgrRec *) client_data;
      dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
      file_mgr_data = (FileMgrData *) dialog_data->data;
   }
   else
     file_mgr_data = NULL;
   
   callback = (XmAnyCallbackStruct *) call_data;
   new_event = (XEvent *)callback->event;

   if (new_event->type == ButtonPress)
   {
      XButtonEvent *bevent = (XButtonEvent *)new_event;
      desktop_data->event = *bevent;
   }
   else if (new_event->type == KeyPress || new_event->type == KeyRelease)
   {
      if(first)
      {
         /* set up the the keysyms we are looking for */
         PlusKeysym = XStringToKeysym("plus");
         MinusKeySym = XStringToKeysym("minus");

	 NumSelectKeys = XmeVirtualToActualKeysyms(XtDisplay(w), 
						   osfXK_Select, 
						   &SelectBindings);

	 ActivateKeysym = XK_Return;
         SpaceKeysym = XStringToKeysym("space");
         first = False;
      }

      kevent = (XKeyEvent *)new_event;
      if (kevent->state & ShiftMask)
         offset = 1;
      else
         offset = 0;

      /*
      keysym = XLookupKeysym(kevent, offset);
      */
      XmTranslateKey( kevent->display, kevent->keycode, offset?ShiftMask:0,
                      &modif, &keysym);
   }

   /* in tree mode '+' and '-' keys expand and collaps tree branches */
   if (!desktopRec && new_event->type == KeyPress &&
        (keysym == PlusKeysym || keysym == MinusKeySym))
   {
      /* ignore +/- keys if not in tree mode */
      if (file_mgr_data->show_type != MULTIPLE_DIRECTORY)
         return;

      /* get the gadget that received the input */
      if (keybdFocusPolicy == XmEXPLICIT)
         eventIcon = (Widget) XmGetFocusWidget(w);
      else
         eventIcon = (Widget) InputForGadget(w, kevent->x, kevent->y);
      if (eventIcon == NULL)
         return;

      /* find the widget in the file_list */
      found = False;
      for (i = -1; i < file_mgr_data->directory_count && !found; i++)
      {
         for (j = 0; j < file_mgr_data->directory_set[i]->file_count; j++)
         {
            fileViewData = file_mgr_data->directory_set[i]->file_view_data[j];
            if (fileViewData->widget == eventIcon ||
                fileViewData->treebtn == eventIcon)
            {
               found = True;
               break;
            }
         }
      }

      /* if found, expand the branch */
      if (found && fileViewData->file_data->is_subdir)
      {
         XmDropSiteStartUpdate(w);
         DirTreeExpand(file_mgr_data, fileViewData, (keysym == PlusKeysym));
         DrawCurrentDirectory (file_mgr_rec->current_directory,
                               file_mgr_rec, file_mgr_data);
         LayoutFileIcons(file_mgr_rec, file_mgr_data, False, True);
         XmDropSiteEndUpdate(w);
         RedrawTreeLines(w, -w->core.x, -w->core.y,
                         XtParent(w)->core.width, XtParent(w)->core.height,
                         0, file_mgr_rec, file_mgr_data);
      }
   }

   else if (new_event->type == ButtonPress || (new_event->type == KeyPress &&
                                             keybdFocusPolicy == XmEXPLICIT))
   {
      /* Any button event unposts the rename text field */
      if (desktopRec)
         UnpostDTTextField();
      else if( file_mgr_data )
      {
        if( file_mgr_data->fast_cd_enabled )
          UnpostTextPath( file_mgr_data );

        UnpostTextField( file_mgr_data );
      }

      if (new_event->type == KeyPress)
      {
         /* We have a KeyPress: need to check to see if the press is
            "return", "space", "select". If it is, need to select the Icon
            the focus is in. */

         if (keysym == XK_Escape)
         {
            /* an escape unposts the name change text widget */
            if(desktopRec)
               UnpostDTTextField();
            else
               UnpostTextField(file_mgr_data);
            return;
         }
         else if (    keysym != ActivateKeysym
                   && keysym != SpaceKeysym
                   && keysym != osfXK_Delete
                   && keysym != osfXK_EndLine
                   && keysym != osfXK_BeginLine
                   && keysym != osfXK_PageLeft
                   && keysym != osfXK_PageUp
                   && keysym != osfXK_PageDown
                   && keysym != osfXK_PageRight
                   && keysym != osfXK_Menu )
         {
	   Boolean select = FALSE;
	   int i;

	   for (i = 0; !select && (i < NumSelectKeys); i++)
	     select = (keysym == SelectBindings[i].keysym);

	   if (!select)
	     return;
         }
    
         if (desktopRec)
            eventIcon = desktopRec->iconGadget;
         else
            eventIcon = (Widget) XmGetFocusWidget(file_mgr_rec->file_window);
         event = (XButtonPressedEvent *)new_event;
      }
      else 
      {
         event = (XButtonPressedEvent *)new_event;
         if(event->button != Button1)
         {
            if(event->button == bMenuButton && desktopRec == NULL)
               FmPopup (NULL, NULL, (XEvent *)event, file_mgr_data);
            return;
         }
         eventIcon = (Widget) InputForGadget(w, event->x, event->y);
      }

      /*
       * If the event occurred in an icon, then we are either doing
       * a selection or a drag.  If it started in the file window, then
       * we can only be doing a region selection.
       */
      if (eventIcon != NULL)
      {
         /*
          * If this icon is currently selected, then we can't yet tell
          * whether the user is starting a drag operation, or simply
          * redoing the icon selection; we must wait for either the
          * drag threshold to be surpassed, or the button up event.
          */

         /* Map the icon into its fileViewData structure */
         /* Check for desktop icon first */
         if (desktopRec)
         {
            for (i = 0; i<desktop_data->numIconsUsed; i++)
            {
               if (desktop_data->desktopWindows[i]->iconGadget == eventIcon)
               {
                 fileViewData = desktop_data->desktopWindows[i]->file_view_data;
                 break;
               }
            }
         }
         else
         {
            /* Not a desktop icon */
            XtSetArg(args[0], XmNuserData, (XtPointer) &directoryData);
            XtGetValues(eventIcon, args, 1);
            if( directoryData != NULL )
            { 
              for (i = 0; i < directoryData->file_count; i++)
              {
                if (directoryData->file_view_data[i]->widget == eventIcon &&
                    directoryData->file_view_data[i]->displayed)
                {
                  fileViewData = directoryData->file_view_data[i];
                  break;
                }
              }
            }
         }

         if (((file_mgr_rec && FileIsSelected(file_mgr_data, fileViewData)) ||
             (desktopRec && DTFileIsSelected(desktopRec, fileViewData))) &&
             event->type == ButtonPress)
         {
            /* Need to wait to determine what to do */
            ProcessBtnUp = True;
            B1DragPossible = True;
            initialDragX = event->x;
            initialDragY = event->y;

            if((file_mgr_rec && FileIsSelected(file_mgr_data, fileViewData)) &&
                                        file_mgr_data->selected_file_count > 1)
            {
               if (PositioningEnabledInView(file_mgr_data))
               {
                 ObjectPtr top;
                 FileViewData ** selection_list;

                  top = GetTopOfStack(file_mgr_data);
                  if(top != NULL && top->file_view_data != fileViewData)
                  {
                     ObjectPtr savetop, next, prev;
         
                     savetop = top;
                     while(top != NULL && top->file_view_data != fileViewData)
                        top = top->next;

                     if(top != NULL)
                     {
                        prev = top->prev;
                        next = top->next;
                        if (prev)
                           prev->next = next;
                        if (next)
                           next->prev = prev;
                        top->prev = NULL;
                        top->next = savetop;
                        top->stacking_order = 1;
                        savetop->prev = top;
                        top = savetop;
                        for(i = 2; top->prev != NULL; i++)
                        {
                           top->stacking_order = i;
                           if(top->next == NULL)
                              break;
                           top = top->next;
                        }

                        top = GetTopOfStack(file_mgr_data);
                        RedrawOneGadget(top->file_view_data->widget,NULL,NULL);
                     }
                  }

		        /* now make it the top of the selection list */
                  /* Force selection list order to match stacking order */
                  selection_list = (FileViewData **)XtMalloc(
                                    sizeof(FileViewData *) * 
                                    (file_mgr_data->selected_file_count + 1));

                  i = 0;
                  if(top->file_view_data != NULL)
                     selection_list[i++] = top->file_view_data;
                  for(j = 0;i < file_mgr_data->selected_file_count; j++,i++)
                  {
                      if(top->file_view_data != 
					file_mgr_data->selection_list[j])
                         selection_list[i] = file_mgr_data->selection_list[j];
                      else
                         i--;
                  }
                  selection_list[i] = NULL;
                  XtFree((char *)file_mgr_data->selection_list);
                  file_mgr_data->selection_list = selection_list;
               }
            }
         }
         else
         {
            /*
             * The icon is not currently selected, so we will select it.
             * It is still possible that the user may initiate a drag.
             */
            ProcessBtnUp = False;
            if (event->type == ButtonPress)
            {
               B1DragPossible = True;
               initialDragX = event->x;
               initialDragY = event->y;
            } 
            else
            {
               B1DragPossible = False;
               initialDragX = -1;
               initialDragY = -1;
            }

            switch( keysym )
            {
              case osfXK_Delete:
                {
                  DtActionArg * action_args;
                  int arg_count;

                  if( desktopRec == NULL
                      && file_mgr_rec != NULL )
                  {
                    if( file_mgr_data->selected_file_count
                        && fileViewData
                        && FileIsSelected(file_mgr_data, fileViewData) )
                    {
                      _DtBuildActionArgsWithSelectedFiles(
                                file_mgr_data->selection_list,
                                file_mgr_data->selected_file_count,
                                &action_args, &arg_count );
                    }
                    else
                    {
                      _DtBuildActionArgsWithSelectedFiles( &fileViewData, 1,
                                                           &action_args,
                                                           &arg_count );
                    }
                    DtActionInvoke( file_mgr_rec->shell, TRASH_ACTION,
                                    action_args, arg_count, NULL, NULL,
                                    trash_dir, True, NULL, NULL );
                  }
                  else if( desktopRec != NULL )
                  {
                    int selectedCount = desktop_data->workspaceData[desktopRec->workspace_num - 1]->files_selected;
                    if( selectedCount )
                    {
                      _DtBuildActionArgsWithDTSelectedFiles(
                           desktop_data->workspaceData[desktopRec->workspace_num - 1]->selectedDTWindows,
                           selectedCount,
                           &action_args, &arg_count );
                    }
                    else
                    {
                      _DtBuildActionArgsWithSelectedFiles( &fileViewData, 1,
                                                           &action_args,
                                                           &arg_count );
                    }
                    DtActionInvoke( desktopRec->shell, TRASH_ACTION,
                                    action_args, arg_count, NULL, NULL,
                                    trash_dir, True, NULL, NULL );
                  }
                  _DtFreeActionArgs( action_args, arg_count );
                }
                break;
              case osfXK_Menu:
                if(file_mgr_data)  /* if not, then it is a desktop object */
                  FmPopup( NULL, (XtPointer)fileViewData,
			  (XEvent *)event, file_mgr_data );
                break;
              case osfXK_EndLine:
                if( !desktopRec
                    && file_mgr_data->show_type != MULTIPLE_DIRECTORY )
                {
                  Widget child = NULL;

                  child = XmGetFocusWidget(file_mgr_rec->file_window);
                  if( child )
                  {
                    FileViewData * focusIcon = NULL;
                    FileViewData **order_list;
                    int order_count;

                    order_list = ((IconLayoutData *)file_mgr_data->layout_data)->order_list;
                    order_count = ((IconLayoutData *)file_mgr_data->layout_data)->order_count;

                    for (i = 0; i < order_count; ++i)
                    {
                      if (order_list[i]->filtered)
                        continue;

                      if( order_list[i]->widget == child
                          && !order_list[i]->need_update )
                      {
                        int j;
                        int y = order_list[i]->y;

                        focusIcon = order_list[i];

                        for( j = i+1; j < order_count; ++j )
                        {
                          if (order_list[j]->filtered)
                            continue;

                          if( order_list[j]->y == y )
                            focusIcon = order_list[j];
                          else
                            break;
                        }
                        break;
                      }
                    }
                    XmProcessTraversal( focusIcon->widget, XmTRAVERSE_CURRENT );
                  }
                }
                break;
              case osfXK_BeginLine:
                  if( !desktopRec
                      && file_mgr_data->show_type != MULTIPLE_DIRECTORY )
                {
                  Widget child = NULL;

                  child = XmGetFocusWidget(file_mgr_rec->file_window);
                  if( child )
                  {
                    FileViewData * focusIcon = NULL;
                    FileViewData **order_list;
                    int order_count;

                    order_list = ((IconLayoutData *)file_mgr_data->layout_data)->order_list;
                    order_count = ((IconLayoutData *)file_mgr_data->layout_data)->order_count;

                    for (i = 0; i < order_count; ++i)
                    {
                      if (order_list[i]->filtered)
                        continue;

                      if( order_list[i]->widget == child
                          && !order_list[i]->need_update )
                      {
                        int j;
                        int y = order_list[i]->y;

                        focusIcon = order_list[i];

                        for( j = i-1; j >= 0; --j )
                        {
                          if (order_list[j]->filtered)
                            continue;

                          if( order_list[j]->y == y )
                            focusIcon = order_list[j];
                          else
                            break;
                        }
                        break;
                      }
                    }
                    XmProcessTraversal( focusIcon->widget, XmTRAVERSE_CURRENT );
                  }
                }
                break;
              case osfXK_PageUp:
                if( !desktopRec )
                {
                  if( VerticalScrollbarIsVisible(
                              file_mgr_rec->vertical_scroll_bar,
                              file_mgr_rec->scroll_window))
                  {
 		    String p;
 
 		    p = (String) XtMalloc(sizeof(char *));
 		    *p = (char) 0;
		    XtCallActionProc( file_mgr_rec->vertical_scroll_bar,
				      "PageUpOrLeft", new_event,
				      (char **) &p, 1 );
 		    XtFree(p);
		  }
                }
                break;
              case osfXK_PageDown:
                if( !desktopRec )
                {
                  if( VerticalScrollbarIsVisible(
                              file_mgr_rec->vertical_scroll_bar,
                              file_mgr_rec->scroll_window))
                  {
 		    String p;
 
 		    p = (String) XtMalloc(sizeof(char *));
 		    *p = (char)0;
		    XtCallActionProc( file_mgr_rec->vertical_scroll_bar,
				      "PageDownOrRight", new_event, &p, 1 );
 		    XtFree(p);
                  }
                }
                break;
              default:
                if (desktopRec)
                  ProcessDTSelection(desktopRec, (XButtonEvent *)event);
                else
                  ProcessSelection( file_mgr_data, eventIcon,
                                    ((event->state & ControlMask) != 0),
                                    (XButtonEvent *)event);
                break;
            }
         }
      }
      else
      {
         /* In file window - start region select */
         /* There are no region selects for desktop icons */
         ProcessBtnUp = False;
         B1DragPossible = False;
         initialDragX = -1;
         initialDragY = -1;
         if (desktopRec == NULL)
         {
            ProcessSelection(file_mgr_data, NULL,
                             ((event->state & ControlMask) != 0),
                             (XButtonEvent *)event);
         }
      }
   }
   else if(new_event->type == ButtonRelease || (new_event->type == KeyRelease &&
                                             keybdFocusPolicy == XmEXPLICIT))
   {
      if(new_event->type == KeyRelease)
      {
         if (keysym != osfXK_Select)
            return;

         event = (XButtonPressedEvent *)new_event;
      }
      else
      {
         event = (XButtonPressedEvent *)new_event;
         if(event->button != Button1)
            return;
      }
      /*
       * When a button 1 up event is received, we will first see if the
       * select occurred in the icon's title; if so, then we will post
       * the rename text field.  If the icon in which the button down
       * occurred was not already selected, then all of the selection
       * work was done at button 1 down time, so there is nothing left
       * for us to do but return.  However, if the icon was already
       * selected, then we could not do any select processing at button
       * down time, because the user may have been trying to initate
       * a drag request; this can't be determined until either the drag
       * threshold is surpassed, or the button up occurs.
       */
      if (((initialDragX >= 0) && (initialDragY >= 0)) &&
         (eventIcon = (Widget) InputForGadget(w, initialDragX, 
					      initialDragY)))
      {
         /*
          * If processing was postponed until the button up, then do it now.
          */
         B1DragPossible = False;
         if (ProcessBtnUp)
         {
            XEvent tmpEvent;
            
            tmpEvent.xbutton = *event;
            tmpEvent.xbutton.x = initialDragX;
            tmpEvent.xbutton.y = initialDragY;

            if (desktopRec)
            {
               ProcessDTSelection(desktopRec, (XButtonEvent *)&tmpEvent);
            }
            else
            {
               ProcessSelection(file_mgr_data, eventIcon,
                                ((event->state & ControlMask) != 0),
                                (XButtonEvent *)&tmpEvent);
            }
         }


         /*
          * If the select occurred in the title portion of the icon, then
          * put up a text edit field, which can be used to change the name
          * of the icon.  The last field '0' means its not on the Desktop.
          */
         if (_DtIconSelectInTitle(eventIcon, (Position)initialDragX, 
                                  (Position)initialDragY) && ProcessBtnUpCD)
         {
            /* Map the icon into its fileViewData structure */
            /* Check for desktop icon first */
            if (desktopRec)
            {
               for (i = 0; i<desktop_data->numIconsUsed; i++)
               {
                  if (desktop_data->desktopWindows[i]->iconGadget == eventIcon)
                  {
                    fileViewData = 
                         desktop_data->desktopWindows[i]->file_view_data;
                    break;
                  }
               }
            }
            else
            {
               /* Not a desktop icon */
               XtSetArg(args[0], XmNuserData, (XtPointer) &directoryData);
               XtGetValues(eventIcon, args, 1);
               for (i = 0; i < directoryData->file_count; i++)
               {
                  if (directoryData->file_view_data[i]->widget == eventIcon &&
                      directoryData->file_view_data[i]->displayed)
                  {
                     fileViewData = directoryData->file_view_data[i];
                     break;
                  }
               }
            }

            if (desktopRec)
            {
               CreateNameChangeDialog(eventIcon, fileViewData,
                                      (XtPointer)desktopRec, DESKTOP);
            }
            else if( file_mgr_data->view != BY_NAME )
            {
               /* We don't allow users to rename when they are viewing a
                  directory in 'By Name Only' mode.
                  Because clicking on the label is the only way for them
                  to select a file. Popup a text widget to allow rename 
                  would be very confusing
               */
               if( file_mgr_data->view != BY_ATTRIBUTES
                   || ( file_mgr_data->view == BY_ATTRIBUTES
                        && TurnOnTextName ) )
                 CreateNameChangeDialog(eventIcon, fileViewData,
                                        (XtPointer)file_mgr_data, NOT_DESKTOP);
            }
         }

      }
      else
      {
         /* Clean up */
         B1DragPossible = False;
         B2DragPossible = False;
         ProcessBtnUp = False;
      }
      ProcessBtnUpCD = True;
   }
   else if(new_event->type == KeyPress)
   {
      if (keysym == XK_Escape)
      {
         /* an escape unposts the name change text widget */
         if(desktopRec)
            UnpostDTTextField();
         else
            UnpostTextField(file_mgr_data);
         return;
      }
   }
}


/*
 * When a select operation terminates, we need to find the real button
 * up event, so that we can get the correct (x,y) for where the select
 * operation ended.
 */

static Boolean
CheckForBtnUp(
        Display *display,
        XEvent *event,
        XtPointer arg )
{
   if ((event->type == ButtonRelease) && (event->xbutton.button == Button1))
      return(True);

   return(False);
}


/************************************************************************
 *
 *  GetSelectionRectangle
 *
 ************************************************************************/

static void
GetSelectionRectangle(
        Display *display,
        Window window,
        XButtonEvent *event,
        FileMgrRec *file_mgr_rec,
        FileMgrData *file_mgr_data, 
        Boolean toggleSelect)
{
   Screen * screen;
   static GC draw_gc = 0;
   XGCValues gc_values;
   Region region;
   Region widget_region;
   Region intersect_region;
   Region redraw_region;

   static XRectangle return_rect;

   int virt_x, virt_y;
   int virt_width, virt_height;
   int x1, y1;
   int x2, y2;
   int x3, y3;
   int x4, y4;
   int old_x, old_y;
   int temp, j, i;
   int pixmap_x, pixmap_y, pixmap_x1, pixmap_y1;
   int directory_count;
    
   Window root, sub_win;
   int  win_x, win_y;
   unsigned int mask;
   XEvent btnUpEvent;
   XmManagerWidget file_window;
   Dimension current_file_window_width, sw_width;
   Dimension current_file_window_height, sw_height;
   Dimension file_window_width, file_window_height;
   Dimension max_width, max_height;
   Dimension min_width, min_height;
   Widget hb, vb;
   int vb_val_ret = 0;
   int vb_slide_size = 0;
   int vb_inc_ret = 0;
   int vb_pg_inc_ret = 0;
   int hb_val_ret = 0;
   int hb_slide_size = 0;
   int hb_inc_ret = 0;
   int hb_pg_inc_ret = 0;
   Boolean vsbVisible = False;
   Boolean hsbVisible = False;
   DirectorySet ** directory_set;
   FileViewData *file_view_data;
   
   Position x,y;
   Dimension highlightThickness, shadowThickness;
   Dimension width, height;
   Dimension marginWidth, marginHeight;
   Dimension pixmapWidth, pixmapHeight;
   Dimension stringWidth, stringHeight;
   unsigned char pixmapPosition;
   unsigned char alignment;

   Arg args[10];
    
   /*  Get the horizontal and vertical scrollbars.  */
   XtSetArg (args[0], XmNhorizontalScrollBar, &hb);
   XtSetArg (args[1], XmNverticalScrollBar, &vb);
   XtSetArg (args[2], XmNwidth, &sw_width);
   XtSetArg (args[3], XmNheight, &sw_height);
   XtGetValues (file_mgr_rec->scroll_window, args, 4);

   file_window = (XmManagerWidget)file_mgr_rec->file_window;
   XtSetArg (args[0], XmNwidth, &file_window_width);
   XtSetArg (args[1], XmNheight, &file_window_height);
   XtGetValues ((Widget)file_window, args, 2);
   file_window_width--;
   file_window_height--;

   if (VerticalScrollbarIsVisible(vb, file_mgr_rec->scroll_window))
   {
       vsbVisible = True;
       current_file_window_width = sw_width - vb->core.width - 12;
   }
   else
   {
       current_file_window_width = sw_width - 9;
   }

   if (HorizontalScrollbarIsVisible(hb, file_mgr_rec->scroll_window))
   {
       hsbVisible = True;
       current_file_window_height = sw_height - hb->core.height - 12;
   }
   else
   {
       current_file_window_height = sw_height - 9;
   }

   if(vsbVisible)
   {
       XmScrollBarGetValues(vb, &vb_val_ret, &vb_slide_size,
                                          &vb_inc_ret, &vb_pg_inc_ret);
       min_height = vb_val_ret;
       max_height = current_file_window_height + min_height;
   }
   else
   {
       max_height = current_file_window_height;
       min_height = 0;
   }

   if(hsbVisible)
   {
       XmScrollBarGetValues(hb, &hb_val_ret, &hb_slide_size,
                                          &hb_inc_ret, &hb_pg_inc_ret);
       min_width = hb_val_ret;
       max_width = current_file_window_width + min_width;
   }
   else
   {
       max_width = current_file_window_width;
       min_width = 0;
   }

   /*  Get the gc to be used to draw the bounding rectangle  */

   if (!draw_gc)
   {
     unsigned int white_pixel;

     screen = XDefaultScreenOfDisplay (display);
     gc_values.function = GXxor;

     white_pixel = WhitePixelOfScreen (screen);
     if( white_pixel )
     {
       gc_values.foreground = white_pixel;
       gc_values.background = BlackPixelOfScreen (screen);
     }
     else
     {
       gc_values.background = white_pixel;
       gc_values.foreground = BlackPixelOfScreen (screen);
     }

     draw_gc = 
       XCreateGC (display, window,
                  GCForeground | GCBackground | GCFunction, &gc_values);
   }

   widget_region = XCreateRegion();
   intersect_region = XCreateRegion();
   redraw_region = XCreateRegion();

   x1 = x2 = old_x = event->x;
   y1 = y2 = old_y = event->y;

   DrawRectangle (display, window, draw_gc, x1, y1, x2, y2);

   while (1)		/*  Set up a loop to catch the select up point.  */
   {
      XQueryPointer (display,  window, &root, &sub_win,
                      &win_x, &win_y, &x2, &y2, &mask);

      if ((mask & Button1Mask) == 0)
      {
         /* Undraw the last rectangle we drew */
         x2 = old_x;
         y2 = old_y;
         DrawRectangle (display, window, draw_gc, x1, y1, x2, y2);

         /*
          * The real bound of the rectangle is contained in the button up
          * event; you can't use the pointer position, because if the system
          * is slow, then by the time we query the mouse, the button may have
          * been long ago release, and the pointer subsequently moved.
          */
         if (XPeekIfEvent(display, &btnUpEvent,(Bool (*)())CheckForBtnUp, NULL))
         {
            x2 = btnUpEvent.xbutton.x;
            y2 = btnUpEvent.xbutton.y;
         }
         break;
      }

      if( x2 < 0 ) x2 = 0;
      if( y2 < 0 ) y2 = 0;

      if ((x2 > old_x && !hsbVisible && (Dimension)old_x < file_window_width) || 
          (x2 > old_x && hsbVisible && (Dimension)(hb_val_ret + 
                                 hb_slide_size) < (Dimension)(file_window_width + 1)) || 
          (x2 < old_x && !hsbVisible && old_x > 0) || 
                     (x2 < old_x && hsbVisible && hb_val_ret >= 0) ||
          (y2 > old_y && !vsbVisible && (Dimension)old_y < file_window_height) || 
          (y2 > old_y && vsbVisible && (Dimension)(vb_val_ret + 
                                vb_slide_size) < (Dimension)(file_window_height + 1)) ||
          (y2 < old_y && !vsbVisible && old_y > 0) || 
                     (y2 < old_y && vsbVisible && vb_val_ret >= 0))
      {
         DrawRectangle (display, window, draw_gc, x1, y1, old_x, old_y);
         if((Dimension)x2 >= max_width)
         {
            if(hsbVisible)
            {
               hb_val_ret += (x2 - max_width);
               if((Dimension)(hb_val_ret + hb_slide_size) <= (Dimension)(file_window_width + 1))
               {
                  XmScrollBarSetValues(hb, hb_val_ret, hb_slide_size,
                                             hb_inc_ret, hb_pg_inc_ret, True);
                  min_width = (x2 - max_width) + min_width;
                  max_width = x2;
                  XmUpdateDisplay ((Widget)file_window);
               }
               else if((Dimension)(hb_val_ret + hb_slide_size) > (Dimension)(file_window_width + 1) && 
                    (Dimension)(hb_val_ret - (x2 - max_width)) < 
                                         (Dimension)(file_window_width - hb_slide_size)) 
               {
                  XmScrollBarSetValues(hb, 
                               file_window_width - hb_slide_size + 1, 
                               hb_slide_size, hb_inc_ret, hb_pg_inc_ret, True);
                  temp = (file_window_width - hb_slide_size + 1) - hb_val_ret; 
                  hb_val_ret = file_window_width - hb_slide_size + 1;
                  min_width = temp + min_width;
                  max_width = temp + max_width;
                  XmUpdateDisplay ((Widget)file_window);
                  x2 = file_window_width;
               }
               else
               {
                  hb_val_ret = file_window_width - hb_slide_size + 1;
                  x2 = max_width = file_window_width;
                  min_width = max_width - current_file_window_width;
               }
            }
            else
            {
               if(vsbVisible)
               {
                  if((Dimension)x2 > current_file_window_width)
                     x2 = current_file_window_width; 
               }
               else
               {
                  if((Dimension)x2 > file_window_width)
                     x2 = file_window_width; 
               }
            }
         }
         else if((Dimension)x2 < min_width)
         {
            if(hsbVisible)
            {
               if(min_width > 0)
               {
                  hb_val_ret -= (min_width - x2);
                  if(hb_val_ret >= 0)
                  {
                     XmScrollBarSetValues(hb, hb_val_ret, hb_slide_size,
                                          hb_inc_ret, hb_pg_inc_ret, True);
                     max_width = max_width - (min_width - x2);
                     min_width = x2;
                     XmUpdateDisplay (file_mgr_rec->file_window);
                  }
                  else if((hb_val_ret < 0) &&
                       ((int)(hb_val_ret + (min_width - x2)) >= (int)0))
                  {
                     XmScrollBarSetValues(hb, 0, hb_slide_size, 
                                         hb_inc_ret, hb_pg_inc_ret, True);
                     max_width = current_file_window_width;
                     XmUpdateDisplay ((Widget)file_window);
                     x2 = hb_val_ret = min_width = 0;
                  }
                  else
                  {
                     hb_val_ret = min_width = x2 = 0;
                     max_width = current_file_window_width;
                  }
               }
               else
               {
                  if(x2 < 0)
                     x2 = 0;
               }
            }
            else
            {
               if(x2 < 0)
                  x2 = 0;
            }
         }

         if((Dimension)y2 > max_height)
         {
            if(vsbVisible)
            {
               vb_val_ret += (y2 - max_height);
               if((Dimension)(vb_val_ret + vb_slide_size) <= (Dimension)(file_window_height + 1))
               {
                  XmScrollBarSetValues(vb, vb_val_ret, vb_slide_size,
                                             vb_inc_ret, vb_pg_inc_ret, True);
                  min_height = (y2 - max_height) + min_height;
                  max_height = y2;
                  XmUpdateDisplay ((Widget)file_window);
               }
               else if((Dimension)(vb_val_ret + vb_slide_size) > (Dimension)(file_window_height + 1) &&
                    (Dimension)(vb_val_ret - (y2 - max_height)) < 
                                       (Dimension)(file_window_height + 1 - vb_slide_size))
               {
                  XmScrollBarSetValues(vb, 
                               file_window_height + 1 - vb_slide_size,
                               vb_slide_size, vb_inc_ret, vb_pg_inc_ret, True);
                  temp = (file_window_height + 1 - vb_slide_size) - vb_val_ret;
                  vb_val_ret = file_window_height + 1 - vb_slide_size;
                  min_height = temp + min_height;
                  max_height = temp + max_height;
                  XmUpdateDisplay ((Widget)file_window);
                  y2 = file_window_height;
               }
               else
               {
                  vb_val_ret = file_window_height + 1 - vb_slide_size;
                  y2 = max_height = file_window_height;
                  min_height = max_height - current_file_window_height;
               }
            }
            else
            {
               if(hsbVisible)
               {
                  if((Dimension)y2 > current_file_window_height)
                     y2 = current_file_window_height; 
               }
               else
               {
                  if((Dimension)y2 > file_window_height)
                     y2 = file_window_height; 
               }
            }
         }
         else if((Dimension)y2 < min_height)
         {
            if(vsbVisible)
            {
               if(min_height > 0)
               {
                  vb_val_ret -= (min_height - y2);
                  if(vb_val_ret >= 0)
                  {
                     XmScrollBarSetValues(vb, vb_val_ret, vb_slide_size,
                                          vb_inc_ret, vb_pg_inc_ret, True);
                     max_height = max_height - (min_height - y2);
                     min_height = y2;
                     XmUpdateDisplay (file_mgr_rec->file_window);
                  }
                  else if(vb_val_ret < 0 &&
                       ((int)(vb_val_ret + (min_height - y2)) >= (int)0))
                  {
                     XmScrollBarSetValues(vb, 0, vb_slide_size,
                                         vb_inc_ret, vb_pg_inc_ret, True);
                     max_height = current_file_window_height;
                     XmUpdateDisplay ((Widget)file_window);
                     y2 = vb_val_ret = min_height = 0;
                  }
                  else
                  {
                     vb_val_ret = min_height = y2 = 0;
                     max_height = current_file_window_height;
                  }
               }
               else
               {
                  if(y2 < 0)
                     y2 = 0;
               }
            }
            else
            {
               if(y2 < 0)
                  y2 = 0;
            }
         }
         old_x = x2;
         old_y = y2;
         x3 = x1;
         x4 = x2;
         y3 = y1;
         y4 = y2;
         if (x4 < x3)
         {
            temp = x3;
            x3 = x4;
            x4 = temp;
         }

         if (y4 < y3)
         {
            temp = y3;
            y3 = y4;
            y4 = temp;
         }
      
         virt_x = x3;
         virt_y = y3;
         virt_width = x4 - x3 + 1;
         virt_height = y4 - y3 + 1;


         return_rect.x = (short)virt_x;
         return_rect.y = (short)virt_y;
         return_rect.width = (unsigned short)virt_width;
         return_rect.height = (unsigned short)virt_height;
         region = XCreateRegion();
         XUnionRectWithRegion (&return_rect, region, region);

         /*  Run through the gadget set to cause each icon within the  */
         /*  rectangle to be selected.                                 */

         if (file_mgr_data->show_type == SINGLE_DIRECTORY)
            directory_count = 1;
         else
            directory_count = file_mgr_data->directory_count;
   
         directory_set = file_mgr_data->directory_set;


         for (i = 0; i < directory_count; i++)
         {
            for (j = 0; j < directory_set[i]->file_count; j++)
            {
               DtIconGadget  g;
   
               file_view_data = directory_set[i]->order_list[j];
      
               if (!file_view_data->displayed || file_view_data->need_update)
                  continue;

               g = (DtIconGadget) file_view_data->widget;

               highlightThickness = g->gadget.highlight_thickness;
               shadowThickness = g->gadget.shadow_thickness;

               x = g->rectangle.x;
               y = g->rectangle.y;
      
               marginWidth = g->icon.cache->margin_width;
               marginHeight = g->icon.cache->margin_height;
               pixmapPosition = g->icon.cache->pixmap_position;
               alignment = g->icon.cache->alignment;

               pixmapWidth = g->icon.pixmap_width;
               pixmapHeight = g->icon.pixmap_height;
               stringWidth = g->icon.string_width;
               stringHeight = g->icon.cache->string_height;

               switch ((int) pixmapPosition)
               {
                  case XmPIXMAP_TOP:
                     if (alignment == XmALIGNMENT_BEGINNING)
                        pixmap_x = x + highlightThickness;
                     else if(alignment == XmALIGNMENT_CENTER)
                     {
                        if(stringWidth >= pixmapWidth)
                           pixmap_x = x + highlightThickness + 
                                              ((Dimension)(stringWidth - pixmapWidth)/(Dimension)2);
                        else
                           pixmap_x = x + highlightThickness;
                     }
                     break;
                  case XmPIXMAP_LEFT:
                  default:
                     pixmap_x = x + highlightThickness;
                     break;
               }
               pixmap_y = y + pixmapHeight;
               pixmap_x = pixmap_x + pixmapWidth;
/*
               pixmap_x1 = pixmap_x + pixmapWidth;
               pixmap_y1 = pixmap_y + pixmapHeight;
*/

               if(XPointInRegion(region, pixmap_x, pixmap_y)
/*
                   && XPointInRegion(region, pixmap_x1, pixmap_y1)
*/
                  )
               {
                  if (toggleSelect == True &&
                      FileIsSelected (file_mgr_data, file_view_data))
                     DeselectFile (file_mgr_data, file_view_data, True);
                  else
                  {
                     if(!FileIsSelected (file_mgr_data, file_view_data))
                        SelectFile (file_mgr_data, file_view_data);
                  }
               }
               else if (FileIsSelected (file_mgr_data, file_view_data))
                  DeselectFile (file_mgr_data, file_view_data, True);
            }
         }
         DrawRectangle (display, window, draw_gc, x1, y1, x2, y2);
         XDestroyRegion (region);
      }
   }

   XDestroyRegion (widget_region);
   XDestroyRegion (intersect_region);
   XDestroyRegion (redraw_region);

   XSync (display, 0);
   return;
}


/************************************************************************
 *
 *  DrawRectangle
 *	DrawRectangle takes two points as input and draws a rectangle with
 *      a line width and height of 1 around the rectangle generated
 *      by the two points.  The rectangle is drawn XOR so that it
 *      can be erased.
 *
 ************************************************************************/

static void
DrawRectangle(
        Display *display,
        Window window,
        GC gc,
        int x1,
        int y1,
        int x2,
        int y2 )
{
   int x;
   int y;
   unsigned int width;
   unsigned int height;

   if (x2 >= x1) 
   {
      x = x1;
      width = x2 - x1;
   }
   else
   {
      x = x2;
      width = x1 - x2;
   }

   if (y2 >= y1)
   {
      y = y1;
      height = y2 - y1;
   }
   else
   {
      y = y2;
      height = y1 - y2;
   }

   XDrawRectangle (display, window, gc, x, y, width, height);

   if (width > 2 && height > 2)
      XDrawRectangle (display, window, gc, x + 1, y + 1, width - 2, height - 2);
}


/*
 * Given a widget, create a region describing the area occupied by the
 * widget; it may be a simple rectangle, or a compound region composed
 * of two rectangles (the pixmap and the label).
 */

void 
WidgetRectToRegion (
   FileMgrData * file_mgr_data,
   Widget w,
   Region region)
{
   XRectangle widget_rect;
   XRectangle pRect, lRect;
   unsigned char flags;
   static Region temp_region = NULL;

   /* Empty the region */
   if (temp_region == NULL)
      temp_region = XCreateRegion();
   XSubtractRegion(temp_region, temp_region, region);

   if (w == NULL)
      return;

   /* Tree mode and attribute views do not support compound icons */
   if ((file_mgr_data->show_type != SINGLE_DIRECTORY) ||
       (file_mgr_data->view == BY_ATTRIBUTES))
   {
      widget_rect.x = (short)w->core.x;
      widget_rect.y = (short)w->core.y;
      widget_rect.width = (unsigned short)w->core.width;
      widget_rect.height = (unsigned short)w->core.height;

      XUnionRectWithRegion(&widget_rect, region, region);
   }
   else
   {
      _DtIconGetIconRects((DtIconGadget)w, &flags, &pRect, &lRect);

      if (flags & XmPIXMAP_RECT)
         XUnionRectWithRegion(&pRect, region, region);

      if (flags & XmLABEL_RECT)
         XUnionRectWithRegion(&lRect, region, region);
   }
}


static void 
ProcessSelection (
   FileMgrData * file_mgr_data,
   Widget icon,
   Boolean toggleSelect,
   XButtonEvent * event)
{
   XRectangle * selection_rect;
   XRectangle   widget_rect;
   DirectorySet ** directory_set;
   int directory_count;
   register int i, j;
   Region region;
   Region widget_region;
   Region intersect_region;
   Region redraw_region;
   FileMgrRec * file_mgr_rec;
   FileViewData * file_view_data;

   file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;

   /* User is starting a new selection; clear all old ones */
   if (!toggleSelect)
   {
      DeselectAllFiles (file_mgr_data);
      file_mgr_data->selected_file_count = 0;

      XFlush (XtDisplay (file_mgr_rec->file_window));
      XmUpdateDisplay (file_mgr_rec->file_window);
   }

   /*  Get the rectangle to be used as the selection area  */
   if (icon == NULL && event->type == ButtonPress)
   {
      /* User is starting a region select */
      GetSelectionRectangle (XtDisplay(file_mgr_rec->file_window), 
                                (Window) XtWindow(file_mgr_rec->file_window), 
                                event, file_mgr_rec, file_mgr_data, 
                                toggleSelect);
   }
   else
   {
      /* User did a single select either through keyboard or mouse */
      if(event->type == KeyPress)
      {
         widget_rect.x = (short)icon->core.x;
         widget_rect.y = (short)icon->core.y;
         widget_rect.width = (short)icon->core.width;
         widget_rect.height = (short)icon->core.height;
      }
      else
      {
         widget_rect.x = (short)event->x;
         widget_rect.y = (short)event->y;
         widget_rect.width = 1;
         widget_rect.height = 1;
      } 
      selection_rect = &widget_rect;

      region = XCreateRegion();
      widget_region = XCreateRegion();
      intersect_region = XCreateRegion();
      redraw_region = XCreateRegion();

      XUnionRectWithRegion (selection_rect, region, region);


      /*  Run through the gadget set to cause each icon within the  */
      /*  rectangle to be selected.                                 */

      if (file_mgr_data->show_type == SINGLE_DIRECTORY)
      {
         i = 0;
         directory_count = 1;
      }
      else
      {
         directory_count = file_mgr_data->directory_count;
         i = -1;
      }

      directory_set = file_mgr_data->directory_set;


      for (; i < directory_count; i++)
      {
         if (PositioningEnabledInView(file_mgr_data))
         {
            ObjectPtr top = GetTopOfStack(file_mgr_data);
            int next_position = 0;
   
            /*
             * Stacking order requires us to handle this specially.  Objects
             * must be searched in top to bottom order when determining if
             * they are in the selection rectangle, but they must be drawn
             * bottom to top, to guarantee that the visuals are correct.
             */
   
            /* Find set of selected objects */
            while (top)
            {
               if(top->file_view_data == NULL)
               {
                  top = top->next;
                  continue;
               }
   
               if (!top->file_view_data->displayed)
               {
                  top = top->next;
                  continue;
               }
   
               WidgetRectToRegion(file_mgr_data, top->file_view_data->widget, 
                                  widget_region);
               XIntersectRegion(region, widget_region, intersect_region);
   
               if (!XEmptyRegion(intersect_region))
               {
                  if (toggleSelect && 
                      FileIsSelected (file_mgr_data, top->file_view_data))
                  {
                     DeselectFile(file_mgr_data, top->file_view_data, True);
   
                     XUnionRegion(widget_region, redraw_region, redraw_region);
                  }
                  else
                  {
                     /*
                      * We don't want to change the icon yet, because we
                      * need to do it in bottom to top order.
                      */
                     next_position++;
                     RepositionUpInStack(file_mgr_data, top->stacking_order,
                                         next_position);
                  }
   
                  /* Higher item 'cover' lower ones */
                  XSubtractRegion(region, widget_region, region);
               }
   
               top = top->next;
            }
   
            /* Now, select any newly selected files */
            if (next_position > 0)
            {
               /* Find where to start the redraw from */
               top = GetTopOfStack(file_mgr_data);
               while (next_position - 1 > 0)
               {
                  top = top->next;
                  next_position--;
               }
   
               while(top)
               {
                  if(top->file_view_data != NULL)
                     SelectFile(file_mgr_data, top->file_view_data);
                  top = top->prev;
               }
            }
   
            if (!XEmptyRegion(redraw_region))
               RepaintDesktop(file_mgr_data, NULL, redraw_region);
   
            RegisterDesktopHotspots(file_mgr_data, file_mgr_rec);
   
         }
         else
         {
            for (j = 0; j < directory_set[i]->file_count; j++)
            {
               file_view_data = directory_set[i]->order_list[j];
   
               if (!file_view_data->displayed)
                  continue;
   
               WidgetRectToRegion(file_mgr_data, file_view_data->widget,
                                  widget_region);
               XIntersectRegion (region, widget_region, intersect_region);
        
               if (!XEmptyRegion (intersect_region))
               {
                  if (toggleSelect == True && 
                      FileIsSelected (file_mgr_data, file_view_data))
                     DeselectFile (file_mgr_data, file_view_data, True);
                  else
                  {
                     SelectFile (file_mgr_data, file_view_data);
                     if(file_mgr_data->view == BY_ATTRIBUTES)
                     {
                        /* 
                         * Need to determine whether the click was in the
                         *  file or before.
                         */
                        XRectangle textExtent;
                        Region tmp_region;
                        XmFontList fontList;
                        XmString fileNameString;
                        DtIconGadget g = (DtIconGadget)file_view_data->widget;

                        _DtIconGetTextExtent_r(file_view_data->widget,
					       &textExtent);
   
                        XtVaGetValues( file_view_data->widget,
                                       XmNfontList,
                                       &fontList,
                                       NULL );

                        fileNameString = XmStringCreateLocalized( file_view_data->file_data->file_name );
                        textExtent.width = XmStringWidth( fontList, fileNameString );
                        XmStringFree( fileNameString );
                        textExtent.height =
                          (unsigned short)file_view_data->widget->core.height;

                        textExtent.x = (short)file_view_data->widget->core.x
                          + g->icon.pixmap_width
                          + g->icon.cache->margin_width
                          + g->icon.cache->spacing;

                        textExtent.y = (short)file_view_data->widget->core.y;
   
                        tmp_region = XCreateRegion();
                        XUnionRectWithRegion (&textExtent, 
                                                 tmp_region, tmp_region); 
   
                        XSubtractRegion (widget_region, tmp_region, tmp_region);
   
                        XIntersectRegion (region, tmp_region, intersect_region);
                        if (XEmptyRegion (intersect_region))
                           TurnOnTextName = True; 
                        else
                           TurnOnTextName = False;
                        XDestroyRegion (tmp_region);
                     }
                  }
               }
            }
         }
      }
   
      XDestroyRegion (region);
      XDestroyRegion (widget_region);
      XDestroyRegion (intersect_region);
      XDestroyRegion (redraw_region);
   }

  
   if(file_mgr_data != trashFileMgrData)
   {
      if (file_mgr_data->selected_file_count == 0)
         ActivateNoSelect (file_mgr_rec);
      else if (file_mgr_data->selected_file_count == 1)
      {
         ActivateSingleSelect (file_mgr_rec,
            file_mgr_data->selection_list[0]->file_data->logical_type);
      }
      else
         ActivateMultipleSelect (file_mgr_rec);
   }
   else
      SensitizeTrashBtns();

   ProcessBtnUp = False;
}


void
DrawHighlight (
      Widget w,
      FileViewData *file_view_data,
      FileMgrData *file_mgr_data,
      int type)
{
   int n, num_rectangles;
   Position x,y;
   Dimension highlightThickness, shadowThickness;
   Dimension half_shadowThickness;
   Dimension marginWidth, marginHeight;
   Dimension pixmapWidth, pixmapHeight;
   Dimension stringWidth, stringHeight;
   Dimension spacing;
   XRectangle rect[10];
   unsigned char pixmapPosition;
   unsigned char alignment;
   Position        adj_x, adj_y;
   GC gc ;
   Arg args[20];
   DtIconGadget  g = (DtIconGadget)w;
   ObjectPtr top;

   gc = ((XmManagerWidget)(g->object.parent))->manager.highlight_GC;
#ifdef SHAPE
   if(shapeExtension)
      if(type == DESKTOP)
      {
         DrawShadowTh(w, gc, DESKTOP);
         return;
      }
#endif

   /* in in as placed mode need to cycle the icons so the traversal
      box doesn't chop up the icons */ 
   if(type == NOT_DESKTOP && file_view_data != NULL)
   {
      if (PositioningEnabledInView(file_mgr_data))
      {
         top = GetTopOfStack(file_mgr_data);
         if(top != NULL && top->file_view_data != file_view_data)
         {
            ObjectPtr savetop, next, prev;
            int i;

            savetop = top;
            while(top != NULL && top->file_view_data != file_view_data)
               top = top->next;

            if(top != NULL)
            {
               prev = top->prev;
               next = top->next;
               if (prev)
                  prev->next = next;
               if (next)
                  next->prev = prev;
               top->prev = NULL;
               top->next = savetop;
               top->stacking_order = 1;
               savetop->prev = top;
               top = savetop;
               for(i = 2; top->prev != NULL; i++)
               {
                  top->stacking_order = i;
                  if(top->next == NULL)
                     break;
                  top = top->next;
               }

               RedrawOneGadget(w, NULL, NULL);
            }
         }
      }
   }

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, &highlightThickness);      n++;
   XtSetArg (args[n], XmNshadowThickness, &shadowThickness);      n++;
   XtSetArg (args[n], XmNx, &x);      n++;
   XtSetArg (args[n], XmNy, &y);      n++;
   XtSetArg (args[n], XmNmarginWidth, &marginWidth);      n++;
   XtSetArg (args[n], XmNmarginHeight, &marginHeight);      n++;
   XtSetArg (args[n], XmNpixmapPosition, &pixmapPosition);      n++;
   XtSetArg (args[n], XmNalignment, &alignment);      n++;
   XtGetValues (w, args, n);
   
   spacing = (((DtIconGadget)(g)) -> icon.cache -> spacing);
   pixmapWidth = g->icon.pixmap_width;
   pixmapHeight = g->icon.pixmap_height;
   stringWidth = g->icon.string_width;
   stringHeight = g->icon.cache->string_height;

   adj_x = shadowThickness + highlightThickness + marginWidth;
   adj_y = shadowThickness + highlightThickness + marginHeight;
   
   if(shadowThickness != 0)
     half_shadowThickness = shadowThickness/2;
   else
     half_shadowThickness = 0;

   num_rectangles = 0;
   switch ((int) pixmapPosition)
   {
      case XmPIXMAP_TOP:
         if (alignment == XmALIGNMENT_BEGINNING)
         {
           /* 1 */
            rect[num_rectangles].x = x;
            rect[num_rectangles].y = y;
            rect[num_rectangles].width = pixmapWidth + (2 * adj_x);
            rect[num_rectangles++].height = highlightThickness;

            if(stringWidth >  pixmapWidth)
            {
              /* 2 */
               rect[num_rectangles].x = x + pixmapWidth +
                                            (2 * adj_x) - highlightThickness;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = pixmapHeight + spacing;

              /* 3 */
               rect[num_rectangles].x = x + pixmapWidth +
                                              (2 * adj_x) - highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + spacing;
               rect[num_rectangles].width = stringWidth -
                                           pixmapWidth + highlightThickness;
               rect[num_rectangles++].height = highlightThickness;

              /* 4 */
               rect[num_rectangles].x = x + stringWidth + (2 * adj_x)
                                                          - highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + spacing;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = stringHeight + (2 * adj_y);
            }
            else
            {
              /* 2 */
               rect[num_rectangles].x = x + pixmapWidth +
                                            (2 * adj_x) - highlightThickness;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = pixmapHeight + (2 * adj_y) -
                                                  highlightThickness;

              /* 3 */
               rect[num_rectangles].x = x + stringWidth +
                                              (2 * adj_x) - highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + (2 * adj_y) - 
                                                         highlightThickness;
               rect[num_rectangles].width = pixmapWidth -
                                           stringWidth + highlightThickness;
               rect[num_rectangles++].height = highlightThickness;


              /* 4 */
               rect[num_rectangles].x = x + stringWidth + (2 * adj_x)
                                                          - highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + (2 * adj_y) - 
                                                         highlightThickness;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = spacing + stringHeight + 
                                                           highlightThickness; 
            }

           /* 5 */
            rect[num_rectangles].x = x;
            rect[num_rectangles].y = y + pixmapHeight + stringHeight +
                                spacing + (2 * adj_y) - highlightThickness;
            rect[num_rectangles].width = stringWidth + (2 * adj_y);
            rect[num_rectangles++].height = highlightThickness;

           /* 6 */
            rect[num_rectangles].x = x;
            rect[num_rectangles].y = y;
            rect[num_rectangles].width = highlightThickness;
            rect[num_rectangles++].height = pixmapHeight +
                                   stringHeight + spacing + (2 * adj_y);

         }
         else if (alignment == XmALIGNMENT_CENTER)
         {
            if(stringWidth >  pixmapWidth)
            {
              /* 1 */
               rect[num_rectangles].x = x
                                      + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = pixmapWidth + (2 * adj_x);
               rect[num_rectangles++].height = highlightThickness;

              /* 2 */
               rect[num_rectangles].x = x
                                        + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2
                                        +  pixmapWidth
                                        + (2 * adj_x)
                                        - highlightThickness;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = pixmapHeight + spacing;

              /* 3 */
               rect[num_rectangles].x = x
                                        + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2
                                        +  pixmapWidth
                                        + (2 * adj_x)
                                        - highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + spacing;
               rect[num_rectangles].width = (Dimension)(stringWidth - pixmapWidth)/(Dimension)2
                                            + (Dimension)(stringWidth - pixmapWidth)%(Dimension)2; 
               rect[num_rectangles++].height = highlightThickness;
   
              /* 4 */
               rect[num_rectangles].x = x + stringWidth + (2 * adj_x)
                                                        - highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + spacing;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = stringHeight + (2 * adj_y);
   
              /* 5 */
               rect[num_rectangles].x = x;
               rect[num_rectangles].y = y + pixmapHeight + stringHeight +
                                   spacing + (2 * adj_y) - highlightThickness;
               rect[num_rectangles].width = stringWidth + (2 * adj_y);
               rect[num_rectangles++].height = highlightThickness;
   
              /* 6 */
               rect[num_rectangles].x = x;
               rect[num_rectangles].y = y + pixmapHeight + spacing;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = stringHeight + (2 * adj_y);
   
              /* 7 */
               rect[num_rectangles].x = x;
               rect[num_rectangles].y = y + pixmapHeight + spacing;
               rect[num_rectangles].width = (Dimension)(stringWidth - pixmapWidth)/(Dimension)2
                                            + highlightThickness;
               rect[num_rectangles++].height = highlightThickness;
   
              /* 8 */
               rect[num_rectangles].x = x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = pixmapHeight + spacing;
            }
            else
            {
              /* 1 */
               rect[num_rectangles].x = x;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = pixmapWidth + (2 * adj_x);
               rect[num_rectangles++].height = highlightThickness;

              /* 2 */
               rect[num_rectangles].x = x + pixmapWidth + (2 * adj_x) - 
                                                           highlightThickness;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = pixmapHeight + spacing + 
                                                        2 * highlightThickness;

              /* 3 */
               rect[num_rectangles].x = x +  pixmapWidth + adj_x - 
                            (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 + highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + spacing +
                                                        highlightThickness + 1;
               rect[num_rectangles].width = (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 +
                                                          highlightThickness;
               rect[num_rectangles++].height = highlightThickness;

              /* 4 */
               rect[num_rectangles].x = x  + stringWidth + adj_x +
                           (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 + highlightThickness;
               rect[num_rectangles].y = y + pixmapHeight + spacing
                                                      + highlightThickness + 1;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = stringHeight + (2 * adj_y) 
                                                       - highlightThickness - 1;

              /* 5 */
               rect[num_rectangles].x = x + (Dimension)(pixmapWidth - stringWidth)/(Dimension)2;
               rect[num_rectangles].y = y + pixmapHeight + stringHeight +
                                   spacing + (2 * adj_y) - highlightThickness;
               rect[num_rectangles].width = stringWidth + (2 * adj_y);
               rect[num_rectangles++].height = highlightThickness;

              /* 6 */
               rect[num_rectangles].x = x + (Dimension)(pixmapWidth - stringWidth)/(Dimension)2;
               rect[num_rectangles].y = y + pixmapHeight + spacing +
                                                        highlightThickness + 1;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = stringHeight + (2 * adj_y) 
                                                       - highlightThickness - 1;
 
              /* 7 */
               rect[num_rectangles].x = x;
               rect[num_rectangles].y = y + pixmapHeight + spacing +
                                                        highlightThickness + 1;
               rect[num_rectangles].width = (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 +
                                                            highlightThickness;
               rect[num_rectangles++].height = highlightThickness;

              /* 8 */
               rect[num_rectangles].x = x;
               rect[num_rectangles].y = y;
               rect[num_rectangles].width = highlightThickness;
               rect[num_rectangles++].height = pixmapHeight + spacing + 
                                                        2 * highlightThickness;
            }
         }
         else 
            FM_XmHighlightBorder ((Widget)w);
         break;
      case XmPIXMAP_LEFT:
         if((Dimension)(stringHeight + 4) >= pixmapHeight)
            FM_XmHighlightBorder ((Widget)w);
         else
         {
           /* 1 */
            rect[num_rectangles].x = x;
            rect[num_rectangles].y = y;
            rect[num_rectangles].width = adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2;
            rect[num_rectangles++].height = highlightThickness;

           /* 2 */
            rect[num_rectangles].x = x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2;
            rect[num_rectangles].y = y;
            rect[num_rectangles].width = highlightThickness;
            rect[num_rectangles++].height = (Dimension)(pixmapHeight - stringHeight)/(Dimension)2;

           /* 3 */
            rect[num_rectangles].x = x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2;
            rect[num_rectangles].y = y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2;
            rect[num_rectangles].width = spacing/2 + stringWidth + adj_x;
            rect[num_rectangles++].height = highlightThickness;

           /* 4 */
            rect[num_rectangles].x = x + adj_x + pixmapWidth + spacing +
                                      stringWidth + adj_x - highlightThickness;
            rect[num_rectangles].y = y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2;
            rect[num_rectangles].width = highlightThickness;
            rect[num_rectangles++].height = stringHeight + 2*adj_y;

           /* 5 */
            rect[num_rectangles].x = x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2;
            rect[num_rectangles].y = y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2 +
                               stringHeight + 2*adj_y - highlightThickness;
            rect[num_rectangles].width = spacing/2 + stringWidth + adj_x;
            rect[num_rectangles++].height = highlightThickness;

           /* 6 */
            rect[num_rectangles].x = x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2;
            rect[num_rectangles].y = y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2 +
                                         stringHeight + 2*adj_y;
            rect[num_rectangles].width = highlightThickness;
            rect[num_rectangles++].height = pixmapHeight - stringHeight -
                                            (Dimension)(pixmapHeight - stringHeight)/(Dimension)2;

           /* 7 */
            rect[num_rectangles].x = x;
            rect[num_rectangles].y = y + pixmapHeight + 2*adj_y -
                                                           highlightThickness;
            rect[num_rectangles].width = adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2;
            rect[num_rectangles++].height = highlightThickness;

           /* 8 */
            rect[num_rectangles].x = x;
            rect[num_rectangles].y = y;
            rect[num_rectangles].width = highlightThickness;
            rect[num_rectangles++].height = pixmapHeight + 2*adj_y;
         }
         break;
      case XmPIXMAP_BOTTOM:
      case XmPIXMAP_RIGHT:
         FM_XmHighlightBorder ((Widget)w);
         break;
      default:
         break;
   }
  
   XFillRectangles (XtDisplay (w), XtWindow (w), gc, &rect[0], num_rectangles);
}

void
DrawUnhighlight ( 
      Widget w,
      int type)
{
   int n;
   Position x,y;
   Dimension highlightThickness, shadowThickness;
   Dimension half_shadowThickness;
   Dimension marginWidth, marginHeight;
   Dimension pixmapWidth, pixmapHeight;
   Dimension stringWidth, stringHeight;
   Dimension spacing;
   unsigned char pixmapPosition;
   unsigned char alignment;
   Position        adj_x, adj_y;
   Arg args[20];
   DtIconGadget g = (DtIconGadget)w;

#ifdef SHAPE
   if(shapeExtension)
      if(type == DESKTOP)
      {
         DrawShadowTh(w, NULL, DESKTOP);
         return;
      }
#endif

   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, &highlightThickness);      n++;
   XtSetArg (args[n], XmNshadowThickness, &shadowThickness);      n++;
   XtSetArg (args[n], XmNx, &x);      n++;
   XtSetArg (args[n], XmNy, &y);      n++;
   XtSetArg (args[n], XmNmarginWidth, &marginWidth);      n++;
   XtSetArg (args[n], XmNmarginHeight, &marginHeight);      n++;
   XtSetArg (args[n], XmNpixmapPosition, &pixmapPosition);      n++;
   XtSetArg (args[n], XmNalignment, &alignment);      n++;
   XtGetValues (w, args, n);

   spacing = (((DtIconGadget)(g)) -> icon.cache -> spacing);
   pixmapWidth = g->icon.pixmap_width;
   pixmapHeight = g->icon.pixmap_height;
   stringWidth = g->icon.string_width;
   stringHeight = g->icon.cache->string_height;

   adj_x = shadowThickness + highlightThickness + marginWidth;
   adj_y = shadowThickness + highlightThickness + marginHeight;

   if(shadowThickness != 0)
     half_shadowThickness = shadowThickness/2;
   else
     half_shadowThickness = 0;

   switch ((int) pixmapPosition)
   {
      case XmPIXMAP_TOP:
         if (alignment == XmALIGNMENT_BEGINNING)
         {
           /* 1 */
            XClearArea (XtDisplay (w), XtWindow (w), x, y, 
                       pixmapWidth + (2 * adj_x), highlightThickness, False);

            if(stringWidth >  pixmapWidth)
            {
              /* 2 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                           x + pixmapWidth + (2 * adj_x) - highlightThickness,
                           y, highlightThickness, 
                           pixmapHeight + spacing,
                           False);

              /* 3 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                           x + pixmapWidth + (2 * adj_x) - highlightThickness,
                           y + pixmapHeight + spacing,
                           stringWidth - pixmapWidth + highlightThickness, 
                           highlightThickness, 
                           False);

              /* 4 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                           x + stringWidth + (2 * adj_x) - highlightThickness,
                           y + pixmapHeight + spacing,
                           highlightThickness, 
                           stringHeight + (2 * adj_y),
                           False);
            }
            else
            {
              /* 2 */
               XClearArea (XtDisplay (w), XtWindow (w),
                           x + pixmapWidth + (2 * adj_x) - highlightThickness,
                           y, highlightThickness,
                           pixmapHeight + (2 * adj_y) - half_shadowThickness,
                           False);

              /* 3 */
               XClearArea (XtDisplay (w), XtWindow (w),
                           x + stringWidth + (2 * adj_x) - highlightThickness,
                           y + pixmapHeight + (2 * adj_y) - highlightThickness,
                           pixmapWidth - stringWidth + highlightThickness,
                           highlightThickness,
                           False);

              /* 4 */
               XClearArea (XtDisplay (w), XtWindow (w),
                           x + stringWidth + (2 * adj_x) - highlightThickness,
                           y + pixmapHeight + (2 * adj_y) -
                                                         half_shadowThickness,
                           highlightThickness,
                           spacing + stringHeight + highlightThickness,
                           False);
            }

           /* 5 */
            XClearArea (XtDisplay (w), XtWindow (w), x, 
                           y + pixmapHeight + stringHeight +
                                spacing + (2 * adj_y) - highlightThickness,
                           stringWidth + (2 * adj_y),
                           highlightThickness, 
                           False);

           /* 6 */
            XClearArea (XtDisplay (w), XtWindow (w), x, y,
                           highlightThickness, 
                           pixmapHeight + stringHeight + spacing + (2 * adj_y),
                           False);

         }
         else if (alignment == XmALIGNMENT_CENTER)
         {
            if(stringWidth >  pixmapWidth)
            {
              /* 1 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                           x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2, y,
                           pixmapWidth + (2 * adj_x),
                           highlightThickness, 
                           False);

              /* 2 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                          x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + pixmapWidth +
                                            (2 * adj_x) - highlightThickness,
                          y, highlightThickness, pixmapHeight + spacing,
                          False);

              /* 3 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                          x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + pixmapWidth + 
                                            (2 * adj_x) - highlightThickness,
                          y + pixmapHeight + spacing,
                          (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + 
                                      (Dimension)(stringWidth - pixmapWidth)%(Dimension)2, 
                          highlightThickness,
                          False);

              /* 4 */
               XClearArea (XtDisplay (w), XtWindow (w), 
                          x + stringWidth + (2 * adj_x) - highlightThickness,
                          y + pixmapHeight + spacing, highlightThickness,
                          stringHeight + (2 * adj_y),
                          False);

              /* 5 */
               XClearArea (XtDisplay (w), XtWindow (w), x,
                          y + pixmapHeight + stringHeight +
                                spacing + (2 * adj_y) - highlightThickness,
                          stringWidth + (2 * adj_y), highlightThickness,
                          False);

              /* 6 */
               XClearArea (XtDisplay (w), XtWindow (w), x,
                          y + pixmapHeight + spacing, highlightThickness,
                          stringHeight + (2 * adj_y),
                          False);

              /* 7 */
               XClearArea (XtDisplay (w), XtWindow (w), x,
                          y + pixmapHeight + spacing, 
                          (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + highlightThickness, 
                          highlightThickness,
                          False);

              /* 8 */
               XClearArea (XtDisplay (w), XtWindow (w),
                          x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2,
                          y, highlightThickness, pixmapHeight + spacing,
                          False);
            }
            else
            {
              /* 1 */
               XClearArea (XtDisplay (w), XtWindow (w), x, y,
                          pixmapWidth + (2 * adj_x), highlightThickness,
                          False);

              /* 2 */
               XClearArea (XtDisplay (w), XtWindow (w),
                          x + pixmapWidth + (2 * adj_x) - highlightThickness,
                          y, highlightThickness, 
                          pixmapHeight + spacing + 2 *  highlightThickness,
                          False);

              /* 3 */
               XClearArea (XtDisplay (w), XtWindow (w),
                          x +  pixmapWidth + adj_x + highlightThickness - 
                                       (Dimension)(pixmapWidth - stringWidth)/(Dimension)2,
                          y + pixmapHeight + spacing + highlightThickness + 1,
                          (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 + highlightThickness,
                          highlightThickness,
                          False);

              /* 4 */
               XClearArea (XtDisplay (w), XtWindow (w),
                          x  + stringWidth + (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 +
                                                    adj_x + highlightThickness,
                          y + pixmapHeight + spacing + highlightThickness + 1,
                          highlightThickness,
                          stringHeight + (2 * adj_y) - highlightThickness -1,
                          False);

              /* 5 */
               XClearArea (XtDisplay (w), XtWindow (w),
                          x + (Dimension)(pixmapWidth - stringWidth)/(Dimension)2,
                          y + pixmapHeight + stringHeight +
                                   spacing + (2 * adj_y) - highlightThickness,
                          stringWidth + (2 * adj_y),
                          highlightThickness,
                          False);

              /* 6 */
               XClearArea (XtDisplay (w), XtWindow (w),
                          x + (Dimension)(pixmapWidth - stringWidth)/(Dimension)2,
                          y + pixmapHeight + spacing + highlightThickness + 1,
                          highlightThickness,
                          stringHeight + (2 * adj_y) - highlightThickness - 1,
                          False);

              /* 7 */
               XClearArea (XtDisplay (w), XtWindow (w), x,
                          y + pixmapHeight + spacing + highlightThickness + 1,
                          (Dimension)(pixmapWidth - stringWidth)/(Dimension)2 + highlightThickness,
                          highlightThickness,
                          False);

              /* 8 */
               XClearArea (XtDisplay (w), XtWindow (w), x, y,
                          highlightThickness, 
                          pixmapHeight + spacing + 2 * highlightThickness,
                          False);

            }
         }
         else
            FM_XmUnhighlightBorder ((Widget)w);
         break;
      case XmPIXMAP_LEFT:
         if((Dimension)(stringHeight + 4) >= pixmapHeight)
            FM_XmUnhighlightBorder ((Widget)w);
         else
         {
           /* 1 */
            XClearArea (XtDisplay (w), XtWindow (w), x, y,
                           adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2,
                           highlightThickness,
                           False);

           /* 2 */
            XClearArea (XtDisplay (w), XtWindow (w),
                           x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2,
                           y,
                           highlightThickness,
                           (Dimension)(pixmapHeight - stringHeight)/(Dimension)2,
                           False);

           /* 3 */
            XClearArea (XtDisplay (w), XtWindow (w),
                           x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2,
                           y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2,
                           spacing/2 + stringWidth + adj_x,
                           highlightThickness,
                           False);

           /* 4 */
            XClearArea (XtDisplay (w), XtWindow (w),
                           x + adj_x + pixmapWidth + spacing +
                                      stringWidth + adj_x - highlightThickness,
                           y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2,
                           highlightThickness,
                           stringHeight + 2*adj_y,
                           False);

           /* 5 */
            XClearArea (XtDisplay (w), XtWindow (w),
                           x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2,
                           y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2 +
                               stringHeight + 2*adj_y - highlightThickness,
                           spacing/2 + stringWidth + adj_x,
                           highlightThickness,
                           False);

           /* 6 */
            XClearArea (XtDisplay (w), XtWindow (w),
                           x + adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2,
                           y + (Dimension)(pixmapHeight - stringHeight)/(Dimension)2 +
                                         stringHeight + 2*adj_y,
                           highlightThickness,
                           pixmapHeight - stringHeight -
                                            (Dimension)(pixmapHeight - stringHeight)/(Dimension)2,
                           False);

           /* 7 */
            XClearArea (XtDisplay (w), XtWindow (w), x,
                           y + pixmapHeight + 2*adj_y - highlightThickness,
                           adj_x + pixmapWidth + (Dimension)(spacing+1)/(Dimension)2,
                           highlightThickness,
                           False);

           /* 8 */
            XClearArea (XtDisplay (w), XtWindow (w), x, y,
                           highlightThickness,
                           pixmapHeight + 2*adj_y,
                           False);
         }
         break;
      case XmPIXMAP_BOTTOM:
      case XmPIXMAP_RIGHT:
         FM_XmUnhighlightBorder ((Widget)w);
         break;
      default:
         break;
   }
}
void
DrawShadowTh (
      Widget w,
      GC gc,
      int type)
{
   int n;
   Position x,y;
   Dimension highlightThickness, shadowThickness;
   Dimension width, height;
   Dimension marginWidth, marginHeight;
   Dimension pixmapWidth, pixmapHeight;
   Dimension stringWidth, stringHeight;
   Dimension spacing;
   unsigned char pixmapPosition;
   unsigned char alignment;
   unsigned char shadowType;
   GC top_gc, bottom_gc ;
   Arg args[20];
   XRectangle rect[10];
   DtIconGadget g = (DtIconGadget)w;
   XmManagerWidget mgr = (XmManagerWidget)XtParent(w);
   Boolean         s_out = False,
                   s_etched = False;



   n = 0;
   XtSetArg (args[n], XmNhighlightThickness, &highlightThickness);      n++;
   XtSetArg (args[n], XmNshadowThickness, &shadowThickness);      n++;
   XtSetArg (args[n], XmNx, &x);      n++;
   XtSetArg (args[n], XmNy, &y);      n++;
   XtSetArg (args[n], XmNwidth, &width);      n++;
   XtSetArg (args[n], XmNheight, &height);      n++;
   XtSetArg (args[n], XmNmarginWidth, &marginWidth);      n++;
   XtSetArg (args[n], XmNmarginHeight, &marginHeight);      n++;
   XtSetArg (args[n], XmNpixmapPosition, &pixmapPosition);      n++;
   XtSetArg (args[n], XmNalignment, &alignment);      n++;
   XtSetArg (args[n], XmNshadowType, &shadowType);      n++;
   XtGetValues (w, args, n);

   spacing = (((DtIconGadget)(g)) -> icon.cache -> spacing);
   pixmapWidth = g->icon.pixmap_width;
   pixmapHeight = g->icon.pixmap_height;
   stringWidth = g->icon.string_width;
   stringHeight = g->icon.cache->string_height;

/*      Get shadow attributes.
*/
   s_out = (shadowType == XmSHADOW_OUT || shadowType == XmSHADOW_ETCHED_OUT)
                ? True : False;
   s_etched = (shadowType == XmSHADOW_ETCHED_IN ||
                                          shadowType == XmSHADOW_ETCHED_OUT)
                ? True : False;

   if(gc == NULL)
   {
      if(shadowType == XmSHADOW_ETCHED_OUT)
      {
         top_gc = (s_out)
                ? mgr->manager.top_shadow_GC : mgr->manager.bottom_shadow_GC;
         bottom_gc = (s_out)
                ? mgr->manager.bottom_shadow_GC : mgr->manager.top_shadow_GC;
      }
      else
      {
         bottom_gc = (s_out)
                ? mgr->manager.top_shadow_GC : mgr->manager.bottom_shadow_GC;
         top_gc = (s_out)
                ? mgr->manager.bottom_shadow_GC : mgr->manager.top_shadow_GC;
      }
   }
   else
      top_gc = bottom_gc = gc;

/*      Draw shadow.
*/
   if (!s_etched)
   {
      XmeDrawShadows (XtDisplay(w), XtWindow(w), top_gc, bottom_gc,
		      x + highlightThickness, 
		      y + highlightThickness, 
		      width - 2*highlightThickness,
		      height - 2*highlightThickness,
		      shadowThickness, XmSHADOW_IN);
   }
   else
   {
      int half_shadowThickness;
      int num_rects;

      if (shadowThickness <= 0) return;
      if (shadowThickness > (Dimension)(width / 2)) shadowThickness = width / 2;
      if (shadowThickness > (Dimension)(height / 2)) shadowThickness = height / 2;
      if (shadowThickness <= 0) return;

      shadowThickness = (shadowThickness % 2) ? 
                                    (shadowThickness-1) : (shadowThickness);

      half_shadowThickness = shadowThickness / 2;
      switch ((int) pixmapPosition)
      {
         case XmPIXMAP_TOP:
            if (alignment == XmALIGNMENT_BEGINNING)
            {
	       if(type == NOT_DESKTOP)
	       {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) - half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                        (2 * marginWidth) + shadowThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + marginHeight + 
                                                spacing/2 + shadowThickness +
                                                half_shadowThickness;

                  if(stringWidth >  pixmapWidth)
                  {
                    /* 3 */
                     rect[num_rects].x = x + highlightThickness + 
					 shadowThickness +
                                         pixmapWidth + (2 * marginWidth);
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                                          shadowThickness;
                     rect[num_rects].width = stringWidth - pixmapWidth +
                                          marginWidth - half_shadowThickness;
                     rect[num_rects++].height = half_shadowThickness;

                    /* 4 */
                     rect[num_rects].x = x + stringWidth + (2 * marginWidth)
                                    + shadowThickness + highlightThickness;
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                                          shadowThickness;
                     rect[num_rects].width = half_shadowThickness;
                     rect[num_rects++].height = stringHeight + 
				 (2*marginHeight) + half_shadowThickness;
                  }
                  else
                  {
                    /* 3 */
                     rect[num_rects].x = x + highlightThickness + stringWidth +
                                      (2 * marginWidth) + shadowThickness;
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                         shadowThickness + half_shadowThickness;
                     rect[num_rects].width = pixmapWidth - stringWidth + 
                                           marginHeight - half_shadowThickness;
                     rect[num_rects++].height = half_shadowThickness;

                    /* 4 */
                     rect[num_rects].x = x + highlightThickness + stringWidth +
                                      (2 * marginWidth) + shadowThickness;
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                        shadowThickness + half_shadowThickness;
                     rect[num_rects].width = half_shadowThickness;
                     rect[num_rects++].height = stringHeight + 
                                       (2*marginHeight); 

                  }
                 /* 5 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                    stringHeight + spacing + (2*marginHeight) + shadowThickness;
                  if(stringWidth >  pixmapWidth)
                     rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                         shadowThickness + half_shadowThickness;
                  else
                     rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                         shadowThickness + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 6 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + stringHeight +
                          spacing + (2 * marginHeight) + shadowThickness;
   
                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                  if(stringWidth >  pixmapWidth)
                  {
                    /* 2 */
                     rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                         shadowThickness + (2 * marginWidth) +
                                         half_shadowThickness;
                     rect[num_rects].y = y + highlightThickness;
                     rect[num_rects].width = half_shadowThickness;
                     if(highlightThickness == 0)
                        rect[num_rects++].height = pixmapHeight + marginHeight+ 
                                       shadowThickness + (spacing/2)
                                       + half_shadowThickness;
                     else
                        rect[num_rects++].height = pixmapHeight + marginHeight+ 
                                       shadowThickness;

                    /* 3 */
                     rect[num_rects].x = x + highlightThickness + 
					 shadowThickness +
                                         pixmapWidth + (2 * marginWidth);
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                         shadowThickness + half_shadowThickness;
                     rect[num_rects].width = stringWidth - pixmapWidth +
                                           marginWidth - shadowThickness;
                     rect[num_rects++].height = half_shadowThickness;

                    /* 4 */
                     rect[num_rects].x = x + stringWidth + (2 * marginWidth) +
                                         shadowThickness + half_shadowThickness
                                         + highlightThickness;
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                                          shadowThickness;
                     rect[num_rects].width = half_shadowThickness;
                     if(highlightThickness == 0)
                        rect[num_rects++].height = stringHeight + 
                                       (2*marginHeight) + half_shadowThickness;
                     else
                        rect[num_rects++].height = stringHeight + 
                                         (2*marginHeight) + shadowThickness 
                                         + half_shadowThickness;
                              
                  }
                  else
                  {
                    /* 2 */
                     rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                         shadowThickness + (2 * marginWidth) +
                                         half_shadowThickness;
                     rect[num_rects].y = y + highlightThickness;
                     rect[num_rects].width = half_shadowThickness;
                     rect[num_rects++].height = pixmapHeight + marginHeight + 
                                       shadowThickness + (spacing/2) 
                                       + half_shadowThickness;
                    /* 3 */
                     rect[num_rects].x = x + highlightThickness + stringWidth +
                                 (2 * marginWidth) + shadowThickness +
                                  half_shadowThickness;
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                       2*shadowThickness;
                     rect[num_rects].width = pixmapWidth - stringWidth + 
                                           marginHeight - half_shadowThickness;
                     rect[num_rects++].height = half_shadowThickness;

                    /* 4 */
                     rect[num_rects].x = x + highlightThickness + stringWidth +
                                      (2 * marginWidth) + shadowThickness +
                                      half_shadowThickness;
                     rect[num_rects].y = y + pixmapHeight + 2*marginHeight +
                                                         2*shadowThickness;
                     rect[num_rects].width = half_shadowThickness;
                     rect[num_rects++].height = stringHeight + 
                                       (2*marginHeight) + half_shadowThickness;
                  }

                /* 5 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                                  stringHeight + spacing + (2*marginHeight) +
		                  shadowThickness + half_shadowThickness;
                  if(stringWidth >  pixmapWidth)
                     rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                        half_shadowThickness + shadowThickness;
                  else
                     rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                                           (2*shadowThickness);
                  rect[num_rects++].height = half_shadowThickness;

                 /* 6 */
                  rect[num_rects].x = x + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + stringHeight +
                           spacing + (2 * marginHeight) + half_shadowThickness;
   
                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                          &rect[0], num_rects);
               }
	       else
	       {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                  if(stringWidth >  pixmapWidth)
                  {
                     /* 3 */
                     rect[num_rects].x = x + highlightThickness + 
					 shadowThickness +
                                         pixmapWidth + (2 * marginWidth);
                     rect[num_rects].y = y + highlightThickness +
					 pixmapHeight + marginHeight + 
                                         half_shadowThickness;
                     rect[num_rects].width = stringWidth - pixmapWidth +
                                         marginWidth + half_shadowThickness;
                     rect[num_rects++].height = half_shadowThickness;

                    /* 4 */
                     rect[num_rects].x = x + highlightThickness + 
					 shadowThickness +
                                         pixmapWidth + (2 * marginWidth);
                     rect[num_rects].y = y + highlightThickness +
					 pixmapHeight + marginHeight
					 + shadowThickness;
                     rect[num_rects].width = stringWidth - pixmapWidth +
                                                             marginWidth;
                     rect[num_rects++].height = half_shadowThickness;
                  }

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + stringHeight +
                          spacing + (2 * marginHeight) + shadowThickness +
			  half_shadowThickness;
   
                 /* 6 */
                  rect[num_rects].x = x + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + stringHeight +
                           spacing + (2 * marginHeight) + half_shadowThickness;
   
                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                        (2 * marginWidth) + shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
						   half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(stringWidth >  pixmapWidth)
                  {
                     if(highlightThickness == 0)
                        rect[num_rects++].height = pixmapHeight + marginHeight
					+ spacing/2;
		     else
                        rect[num_rects++].height = pixmapHeight + marginHeight
					                           + spacing/2;
                  }
		  else
		  {
                     if(highlightThickness == 0)
                        rect[num_rects++].height = pixmapHeight + marginHeight
			  + spacing/2 + half_shadowThickness + shadowThickness;
		     else
                        rect[num_rects++].height = pixmapHeight + marginHeight
			                    + spacing/2 + half_shadowThickness;
		  }

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                    shadowThickness + (2 * marginWidth) + half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(stringWidth >  pixmapWidth)
		  {
                     if(highlightThickness == 0)
                        rect[num_rects++].height = pixmapHeight + marginHeight +
						 (spacing/2);
                     else
                        rect[num_rects++].height = pixmapHeight + marginHeight +
					                 half_shadowThickness;
                  }
		  else
		  {
                     if(highlightThickness == 0)
                        rect[num_rects++].height = pixmapHeight + marginHeight +
						 shadowThickness + (spacing/2) 
						 + half_shadowThickness;
                     else
                        rect[num_rects++].height = pixmapHeight + marginHeight +
					shadowThickness + shadowThickness;
		  }

                 /* 3 */
                  if(stringWidth >  pixmapWidth)
		  {
                     rect[num_rects].x = x + stringWidth + (2 * marginWidth)
                                    + shadowThickness + highlightThickness;
                  }
                  else
                  {
                     rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                        (2 * marginWidth) + shadowThickness;
                  }
                  rect[num_rects].y = y + highlightThickness +
				      pixmapHeight + 2*marginHeight +
				      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + highlightThickness +
					 (2*marginHeight) + 2*shadowThickness;

                 /* 4 */
                  if(stringWidth >  pixmapWidth)
		  {
                     rect[num_rects].x = x + stringWidth + (2 * marginWidth) +
                    shadowThickness + half_shadowThickness + highlightThickness;
                  }
                  else
                  {
                     rect[num_rects].x = x + highlightThickness + pixmapWidth +
                    shadowThickness + (2 * marginWidth) + half_shadowThickness;
                  }
                  rect[num_rects].y = y + highlightThickness +
				      pixmapHeight + 2*marginHeight;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight +
					 highlightThickness + 
                                         (2*marginHeight) + shadowThickness +
					 half_shadowThickness;

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness +
						   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                    stringHeight + spacing + (2*marginHeight) + shadowThickness;
                  if(stringWidth >  pixmapWidth)
                     rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                         shadowThickness;
                  else
                     rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                         shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                                  stringHeight + spacing + (2*marginHeight) +
		                  shadowThickness + half_shadowThickness;
                  if(stringWidth >  pixmapWidth)
                     rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                        half_shadowThickness + shadowThickness;
                  else
                     rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                           (2*shadowThickness);
                  rect[num_rects++].height = half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                          &rect[0], num_rects);

	       }
            }
            else if (alignment == XmALIGNMENT_CENTER && type == DESKTOP)
            {
               if(stringWidth >  pixmapWidth)
               {
                  num_rects = 0;

                 /* 1 */
                  rect[num_rects].x = x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 +
                                                           highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + 
                            (Dimension)(stringWidth -pixmapWidth)/(Dimension)2 + half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 +
                                      highlightThickness + shadowThickness +
                                      pixmapWidth + (2 * marginWidth);
                  rect[num_rects].y = y + highlightThickness +
                                         pixmapHeight + marginHeight +
                                         half_shadowThickness;
                  rect[num_rects].width = (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 +
                                          (Dimension)(stringWidth - pixmapWidth)%(Dimension)2 +
                                          marginWidth + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x +  (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + 
                                      highlightThickness + shadowThickness +
                                      pixmapWidth + (2 * marginWidth);
                  rect[num_rects].y = y + highlightThickness +
                                         pixmapHeight + marginHeight
                                         + shadowThickness;
                  rect[num_rects].width = (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 +
                                          (Dimension)(stringWidth - pixmapWidth)%(Dimension)2 +
                                          marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
                                      pixmapHeight + 2*marginHeight +
                                      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + highlightThickness +
                                         (2*marginHeight) + 2*shadowThickness;

                 /* 6 */ rect[num_rects].x = x + highlightThickness + 
                                                  half_shadowThickness; 
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                                                               2*marginHeight;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight +
                                         highlightThickness +
                                         (2*marginHeight) + shadowThickness +
                                         half_shadowThickness;

                 /* 7 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
                                      pixmapHeight + marginHeight +
                                      half_shadowThickness;
                  rect[num_rects].width = (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + 
                                                       half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 8 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
                                      pixmapHeight + marginHeight +
                                      shadowThickness;
                  rect[num_rects].width = (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + 
                                                       shadowThickness;

                  rect[num_rects++].height = half_shadowThickness;

                 /* 9 */
                  rect[num_rects].x = x + (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 +
                                                           highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(highlightThickness == 0)
                     rect[num_rects++].height = pixmapHeight + marginHeight
                                        + spacing/2;
                  else
                     rect[num_rects++].height = pixmapHeight + marginHeight
                                                                   + spacing/2;

                 /* 10 */
                  rect[num_rects].x = x + highlightThickness +
                            (Dimension)(stringWidth -pixmapWidth)/(Dimension)2 + half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(highlightThickness == 0)
                     rect[num_rects++].height = pixmapHeight + marginHeight +
                                                 (spacing/2);
                  else
                     rect[num_rects++].height = pixmapHeight + marginHeight +
                                                         half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                        (Dimension)(stringWidth - pixmapWidth)/(Dimension)2 + 
                                        (2 * marginWidth) + shadowThickness;
                  rect[num_rects].y = y + highlightThickness 
                                                 + half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(highlightThickness == 0)
                     rect[num_rects++].height = pixmapHeight + marginHeight
                                        + spacing/2;
                  else
                     rect[num_rects++].height = pixmapHeight + marginHeight
                                                                   + spacing/2;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                    shadowThickness + (2 * marginWidth) + half_shadowThickness +
                    (Dimension)(stringWidth - pixmapWidth)/(Dimension)2;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(highlightThickness == 0)
                     rect[num_rects++].height = pixmapHeight + marginHeight +
                                                 (spacing/2);
                  else
                     rect[num_rects++].height = pixmapHeight + marginHeight +
                                                         half_shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + stringWidth + (2 * marginWidth)
                                    + shadowThickness + highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
                                      pixmapHeight + 2*marginHeight +
                                      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + highlightThickness +
                                         (2*marginHeight) + 2*shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + stringWidth + (2 * marginWidth) +
                    shadowThickness + half_shadowThickness + highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
                                      pixmapHeight + 2*marginHeight;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight +
                                         highlightThickness +
                                         (2*marginHeight) + shadowThickness +
                                         half_shadowThickness;

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness +
                                                   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                    stringHeight + spacing + (2*marginHeight) + shadowThickness;
                  rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                         shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                                  stringHeight + spacing + (2*marginHeight) +
                                  shadowThickness + half_shadowThickness;
                  rect[num_rects].width = stringWidth + (2 * marginWidth) +
                                        half_shadowThickness + shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                          &rect[0], num_rects);

              }
              else
              {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + 
                                                          half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = 2 * highlightThickness + 
                            pixmapHeight + stringHeight + 2 * marginWidth +
                            spacing;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = 2 * highlightThickness + 
                                            pixmapHeight + stringHeight + 
                                            2 * marginWidth + spacing -   
                                            half_shadowThickness; 

                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                        (2 * marginWidth) + shadowThickness;
                  rect[num_rects].y = y + highlightThickness
                                                 + half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = 2 * highlightThickness + 
                            pixmapHeight + stringHeight + 2 * marginWidth +
                            spacing - half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                    shadowThickness + (2 * marginWidth) + half_shadowThickness;
                    
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = 2 * highlightThickness + 
                                            pixmapHeight + stringHeight + 
                                            2 * marginWidth + spacing; 

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness +
                                                          half_shadowThickness;
                  rect[num_rects].y = y + 2 * highlightThickness + 
                          pixmapHeight + stringHeight + spacing + 
                          2 * marginHeight; 
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + 2 * highlightThickness + 
                          pixmapHeight + stringHeight + spacing + 
                          2 * marginHeight + half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;
                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                          &rect[0], num_rects);
              }
            }
            else
               XmeDrawShadows (XtDisplay(w), XtWindow(w), top_gc, bottom_gc,
			       x + highlightThickness, 
			       y + highlightThickness, 
			       width - 2*highlightThickness,
			       height - 2*highlightThickness,
			       shadowThickness, XmSHADOW_IN);
            break;
         case XmPIXMAP_LEFT:
	    if(type == NOT_DESKTOP)
	    {
	       if((Dimension)(stringHeight+5) >= (Dimension)pixmapHeight)    
	       {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing + 
				       shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + pixmapWidth + stringWidth + spacing + 
				       shadowThickness + 2*marginWidth + 
				       highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight +
                                        shadowThickness + half_shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness + stringHeight +
				   2*marginHeight + shadowThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing + 
				       shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight +
                                        shadowThickness + half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing +
                                       half_shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + 
				   half_shadowThickness + pixmapWidth + 
				   stringWidth + spacing + shadowThickness + 
				   2*marginWidth;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight + 
                                                        2*shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness;
               
                  rect[num_rects].y = y + highlightThickness + stringHeight +
				  2*marginHeight + shadowThickness +
				  half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing +
                                       2*shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight + 
                                                        half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                       &rect[0], num_rects);

	       }
	       else
	       {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) - half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				      2*shadowThickness + 2*marginWidth -
 			              shadowThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					  marginHeight - half_shadowThickness;
                                       
                 /* 3 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*shadowThickness + 2*marginWidth -
 			           shadowThickness;
                  rect[num_rects].y = y + highlightThickness + 
			           ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
				   marginHeight -shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth + 
							   shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*shadowThickness + 3*marginWidth 
				   + stringWidth - half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
				     ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					    half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight +
							  half_shadowThickness;

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*shadowThickness + 2*marginWidth -
 			           shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
				   ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
				   stringHeight + 2*marginHeight +
				   shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth + 
				   shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 6 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*shadowThickness + 2*marginWidth -
 			           shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
				   ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
				   stringHeight + 2*marginHeight +
				   shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					  marginHeight;

                 /* 7 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
			       pixmapHeight + 2*marginHeight + shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) - half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 8 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + 2*marginHeight +
							      shadowThickness;
   
                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness + 
						      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
						         half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*marginWidth + shadowThickness + 
				   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					  marginHeight - shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*marginWidth + shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
				  ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
				   half_shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth +
						   half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   3*marginWidth + 2*shadowThickness +
				   stringWidth;
                  rect[num_rects].y = y + highlightThickness +
				   ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2);
				    
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight +
							 2*shadowThickness;
							  
                 /* 5 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*marginWidth + shadowThickness +
				   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
					 ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					 2*marginHeight + shadowThickness +
					 stringHeight + half_shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth +
				                     shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 6 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*marginWidth + shadowThickness +
				   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness + 
					 ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					 2*marginHeight + shadowThickness +
					 stringHeight + half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					  marginHeight;
                 /* 7 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight + 
					  2*marginHeight + shadowThickness +
					  half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + 2*marginWidth + 
							 2*shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 8 */
                  rect[num_rects].x = x + highlightThickness + 
						      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
						      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + 2*marginHeight + 
					     half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                       &rect[0], num_rects);

	       }
	    }
	    else
	    {
	       if((Dimension)(stringHeight+5) >= pixmapHeight)    
	       {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing + 
				       2*shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing +
                                       shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(pixmapHeight > stringHeight)
                     rect[num_rects++].height = pixmapHeight + 2*marginHeight +
                                                              2*shadowThickness;
                  else
                     rect[num_rects++].height = stringHeight + 2*marginHeight +
                                                              2*shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(pixmapHeight > stringHeight)
                     rect[num_rects++].height = pixmapHeight + 2*marginHeight + 
                                                     half_shadowThickness;
                  else
                     rect[num_rects++].height = stringHeight + 2*marginHeight + 
                                                     half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                       &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + pixmapWidth + stringWidth + spacing + 
				       shadowThickness + 2*marginWidth + 
				       highlightThickness;
                  rect[num_rects].y = y + highlightThickness +
							shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(pixmapHeight > stringHeight)
                     rect[num_rects++].height = pixmapHeight + 2*marginHeight +
                                        shadowThickness;
                  else
                     rect[num_rects++].height = stringHeight + 2*marginHeight +
                                        shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + 
				   half_shadowThickness + pixmapWidth + 
				   stringWidth + spacing + shadowThickness + 
				   2*marginWidth;
                  rect[num_rects].y = y + highlightThickness + 
						   half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  if(pixmapHeight > stringHeight)
                     rect[num_rects++].height = pixmapHeight + 2*marginHeight + 
                                                        shadowThickness;
                  else
                     rect[num_rects++].height = stringHeight + 2*marginHeight + 
                                                        shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness + 
						     half_shadowThickness;
                  if(pixmapHeight > stringHeight)
                     rect[num_rects].y = y + highlightThickness + pixmapHeight +
				   2*marginHeight + shadowThickness;
                  else
                     rect[num_rects].y = y + highlightThickness + stringHeight +
				   2*marginHeight + shadowThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing + 
				       shadowThickness + 2*marginWidth -
				       half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                /* 4 */
                  rect[num_rects].x = x + highlightThickness;

                  if(pixmapHeight > stringHeight)
                     rect[num_rects].y = y + highlightThickness + pixmapHeight +
                                  2*marginHeight + shadowThickness +
                                  half_shadowThickness;
                  else
                     rect[num_rects].y = y + highlightThickness + stringHeight +
                                  2*marginHeight + shadowThickness +
                                  half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + stringWidth + spacing +
                                       2*shadowThickness + 2*marginWidth;
                  rect[num_rects++].height = half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                       &rect[0], num_rects);
               }
	       else
	       {
                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = pixmapWidth + (2 * shadowThickness) +
                                       (2 * marginWidth) + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness +
                                                      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                                         half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + (2 * marginWidth) +
                                                          shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*shadowThickness + 2*marginWidth -
 			           shadowThickness;
                  rect[num_rects].y = y + highlightThickness + 
			           ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
				   marginHeight -shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth + 
							   shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   2*marginWidth + shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                  ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                   half_shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth +
                                                   half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness;
                  rect[num_rects].y = y + highlightThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + 2*marginHeight +
							      2*shadowThickness;

                 /* 6 */
                  rect[num_rects].x = x + highlightThickness + 
						      half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
						      half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = pixmapHeight + 2*marginHeight + 
					     shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), top_gc,
                                                        &rect[0], num_rects);

                  num_rects = 0;
                 /* 1 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				      2*shadowThickness + 2*marginWidth -
 			              shadowThickness;
                  rect[num_rects].y = y + highlightThickness + shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					  marginHeight - half_shadowThickness -
					  shadowThickness;

                 /* 2 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth + 
				   2*marginWidth + shadowThickness + 
				   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
						     half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
					  marginHeight - half_shadowThickness -
					  shadowThickness;

                 /* 3 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   2*shadowThickness + 3*marginWidth
                                   + stringWidth - half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                     ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                            half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight +
                                                          half_shadowThickness;

                 /* 4 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   3*marginWidth + 2*shadowThickness +
                                   stringWidth;
                  rect[num_rects].y = y + highlightThickness +
                                   ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2);

                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = stringHeight + 2*marginHeight +
                                                         2*shadowThickness;

                 /* 5 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   2*shadowThickness + 2*marginWidth -
                                   shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                   ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                   stringHeight + 2*marginHeight +
                                   shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth +
                                   shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                /* 6 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   2*marginWidth + shadowThickness +
                                   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                         ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                         2*marginHeight + shadowThickness +
                                         stringHeight + half_shadowThickness;
                  rect[num_rects].width = stringWidth + marginWidth +
                                                     shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 7 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   2*shadowThickness + 2*marginWidth -
                                   shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                   ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                   stringHeight + 2*marginHeight +
                                   shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                          marginHeight;

                 /* 8 */
                  rect[num_rects].x = x + highlightThickness + pixmapWidth +
                                   2*marginWidth + shadowThickness +
                                   half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                                         ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                         2*marginHeight + shadowThickness +
                                         stringHeight + half_shadowThickness;
                  rect[num_rects].width = half_shadowThickness;
                  rect[num_rects++].height = ((Dimension)(pixmapHeight - stringHeight)/(Dimension)2) +
                                          marginHeight;

                 /* 9 */
                  rect[num_rects].x = x + highlightThickness + shadowThickness;
                  rect[num_rects].y = y + highlightThickness +
                               pixmapHeight + 2*marginHeight + shadowThickness;
                  rect[num_rects].width = pixmapWidth + shadowThickness +
                                       (2 * marginWidth) - half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                 /* 10 */
                  rect[num_rects].x = x + highlightThickness + 
							  half_shadowThickness;
                  rect[num_rects].y = y + highlightThickness + pixmapHeight +
                                          2*marginHeight + shadowThickness +
                                          half_shadowThickness;
                  rect[num_rects].width = pixmapWidth + 2*marginWidth +
                                        shadowThickness + half_shadowThickness;
                  rect[num_rects++].height = half_shadowThickness;

                  XFillRectangles (XtDisplay (w), XtWindow (w), bottom_gc,
                                                       &rect[0], num_rects);
	       }
	    }
            break;
         case XmPIXMAP_BOTTOM:
         case XmPIXMAP_RIGHT:
            XmeDrawShadows (XtDisplay(w), XtWindow(w), top_gc, bottom_gc,
			    x + highlightThickness, 
			    y + highlightThickness, 
			    width - 2*highlightThickness,
			    height - 2*highlightThickness,
			    shadowThickness, XmSHADOW_IN);
            break;
         default:
            break;
      }
   }
}

static void
FM_XmHighlightBorder(
        Widget w )
{
    if(    XmIsPrimitive( w)    ) {
        (*(xmPrimitiveClassRec.primitive_class.border_highlight))( w) ;
    }  else  {
        if(    XmIsGadget( w)    ) {
            (*(xmGadgetClassRec.gadget_class.border_highlight))( w) ;
        }
    }
    return ;
}

static void
FM_XmUnhighlightBorder(
        Widget w )
{
    if(    XmIsPrimitive( w)    )
    {  
        (*(xmPrimitiveClassRec.primitive_class.border_unhighlight))( w) ;
    }
    else
    {   if(    XmIsGadget( w)    )
        {
            (*(xmGadgetClassRec.gadget_class.border_unhighlight))( w) ;
        }
    }
    return ;
}

static XmGadget 
InputForGadget(
        Widget wid,
        int x,
        int y )
{
    Widget widget;

    widget = XmObjectAtPoint (wid, x, y);

    if (!widget  ||  !XtIsSensitive (widget))
	return ((XmGadget) NULL);

   return ((XmGadget) widget);
}
