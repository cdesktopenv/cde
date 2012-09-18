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
/* $TOG: Common.c /main/5 1998/05/07 13:27:44 rafi $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Common.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   DESCRIPTION:    Source file for some shared dialog code
 *
 *   FUNCTIONS: AddString
 *		CancelOut
 *		CvtStringListToString
 *		CvtStringToStringList
 *		VFTextChangeSpace
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <time.h>
#include <limits.h>


#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/TextF.h>

#include <Dt/DtP.h>                     /* required for DtDirPaths type */

#include <codelibs/shellutils.h>

#include <Dt/DtNlUtils.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Prefs.h"
#include "Common.h"


#define PADDING 10
static char * DELETE_SEPARATOR = "\n       ";


/************************************************************************
 *
 *  CvtStringToStringList()
 *	Convert a resource string of format:
 *
 *           <item>, <item>, <item>, ...
 *
 *      into an array of strings.
 *
 ************************************************************************/ 

void
CvtStringToStringList(
        String string,
        String **listPtr,
        int *countPtr )
{
   int num_items = 0;
   String * items = NULL;
   int size_item_array = 0;
   String ptr = (String)_DtStripSpaces(string);
   String end;

   while ((ptr != NULL) && (*ptr != '\0'))
   {
      if (num_items >= size_item_array)
      {
         size_item_array += 20;
         items = (String *) 
            XtRealloc((char *) items, sizeof(String) * size_item_array);
      }

      end = DtStrchr(ptr, ',');
      if (end)
         *end = '\0';

      items[num_items] = strcpy(XtMalloc(strlen(ptr)+1), ptr);
      num_items++;
      if (end != NULL)
         ptr = (String)_DtStripSpaces(end + 1);
      else
         break;
   }

   *countPtr = num_items;
   *listPtr= items;
}


/************************************************************************
 *
 *  CvtStringListToString
 *     Convert an array of strings into a single string, which can
 *     then be written out to a resource file.
 *
 ************************************************************************/

String
CvtStringListToString(
        String *list,
        int count )
{
   int i, offset;
   int stringSize = 0;
   String string = NULL;


   /* 
    * Function for writing an array of string values.
    * Write all of the information out as a single string, which
    * we'll parse when we read it back in at a later point in time.
    * The format for this string is:
    *
    *    <item>, <item>, <item>, ...
    */

   if (count > 0)
   {
      for (i = 0, offset = 0; i < count; i++)
      {
         if ((offset + strlen(list[i]) + 3) >= stringSize)
         {
            /* Grow the buffer */
            if ((strlen(list[i]) + 3) > 1024)
               stringSize += strlen(list[i]) + 1024;
            else
               stringSize += 1024;
            string = XtRealloc((char *) string, stringSize);
         }
         
         if (i == 0)
            (void) sprintf (string + offset, "%s", list[i]);
         else
            (void) sprintf (string + offset, ", %s", list[i]);
         offset = strlen(string);
      }
   }

   return(string);
}


/************************************************************************
 *
 * XtActionProc - VFTextChangeSpace() - this is the callback which gets
 *        called when a user type 'Space' in a text widget which has
 *        this translation tied to it.
 *
 ***********************************************************************/
void
VFTextChangeSpace(
        Widget text,
        XEvent *event,
        XtPointer params,
        XtPointer num_params)
{
   char *value;
   char *currentDirectoryText;
   char path[MAX_PATH];
   char *test;
   char **temp;
   int length, shortest, val = 0;
   int i,j;
   Boolean match;

   /* get the current text from the widget */
   value = (char *)XmTextFieldGetString(text);
   currentDirectoryText = GetRestrictedDirectory(text);

   /* Extract the path */
   path[0] = '\0';

   if(currentDirectoryText == NULL)
   {
      strcpy (path, value);
   }
   else
   {
      if (strcmp(currentDirectoryText, "/") == 0)
         sprintf(path, "%s%s", currentDirectoryText, value);
      else
         sprintf(path, "%s/%s", currentDirectoryText, value);
   }
  
   /* add a '*' at the end of the path so the shellscan will no to return
    * all possible matches.
    */
   test = (char *)XtMalloc(strlen(path) + strlen("*") + 1);
   sprintf(test, "%s%s", path, "*");

   /* do a shellscan to get all possible matches to the path */
   temp = (char **)shellscan(test, &val, 0);
   if(val == 1 && strcmp(*temp, test) != 0)
   {
      /* there was one and only one match */
      if(currentDirectoryText == NULL)
         XmTextFieldSetString(text, *temp);
      else
         XmTextFieldSetString(text, *temp + strlen(currentDirectoryText));
      XmTextFieldSetInsertionPosition(text, XmTextFieldGetLastPosition(text));
   }
   else if (val > 1)
   {
      /* more than one filename matches the path */
      length = strlen(path);

      /* first find the shortest of the matches */
      shortest = strlen(temp[0]);
      for(i = 1; i < val; i++)
      {
         j = strlen(temp[i]);
         if(j < shortest)
            shortest = j;
      }
            
      /* find the most characters which will match in all patterns found.
       * i.e. if I have /use, /user, /users, and /used, the most chars
       * which match are /use
       */
      match = True;
      for(i=0; i < shortest; i++)
      {
         for(j=0; j < val - 1; j++)
         {
            if(temp[j][length + i] != temp[j + 1][length + i])
            {
               match = False;
               break;
            }
         }
         if(match == False)
            break;
      }
      temp[0][length + i] = '\0';

      if(currentDirectoryText == NULL)
         XmTextFieldSetString(text, *temp);
      else
         XmTextFieldSetString(text, *temp + strlen(currentDirectoryText));
   
      XmTextFieldSetInsertionPosition(text, XmTextFieldGetLastPosition(text));
 
      /* ring the bell so the users knows the filename is not complete */
      XBell(XtDisplay(text), 100);
   }
   else
      /* no matches found */
      XBell(XtDisplay(text), 100);
      
   XtFree(test);
}

/************************************************************************
 *
 * XtActionProc - CancelOut() - this is the callback which gets
 *        called when a user type 'Space' in a text widget which has
 *        this translation tied to it.
 *
 ***********************************************************************/
void
CancelOut(
        Widget widget,
        XEvent *event,
        XtPointer params,
        XtPointer num_params)
{
   FileMgrData *file_mgr_data;
   FileMgrRec *file_mgr_rec;
   Arg args[2];
 
   if(strcmp(widget->core.name, "nameChangeT") == 0)
   {
      FileViewData *fileViewData;

      file_mgr_data = ReturnDesktopPtr(XtParent(widget));
      XtUnmanageChild(widget);
      XtDestroyWidget(widget);
      file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
      file_mgr_rec->menuStates |= RENAME;
      if(file_mgr_data->selected_file_count == 1)
      {
         fileViewData = file_mgr_data->selection_list[0];
         XmProcessTraversal(fileViewData->widget, XmTRAVERSE_CURRENT);
      }
   }
   else if(strcmp(widget->core.name, "nameChangeT_DT") == 0)
   {
      UnpostDTTextField ();
   }
   else if(strcmp(widget->core.name, FAST_RENAME) == 0)
   {
      file_mgr_data =
             ReturnDesktopPtr(XtParent(XtParent(XtParent(XtParent(widget)))));
      file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;

      XmProcessTraversal(file_mgr_rec->file_window, XmTRAVERSE_CURRENT);
      XtSetArg (args[0], XmNallowShellResize, False);
      XtSetValues(file_mgr_rec->shell, args, 1);
      XtUnmanageChild(file_mgr_rec->current_directory_text);
      XtSetArg (args[0], XmNallowShellResize, True);
      XtSetValues(file_mgr_rec->shell, args, 1);
      file_mgr_data->fast_cd_enabled = False;
   }
}


/*
 * This is a convenience function used to build an error string, which
 * is then passed to an error dialog creation function.  It will add
 * the header string to the beginning, the first time this is called
 * for a particular error string; i.e. when buf == NULL.  The string
 * it creates is owned by the caller, and should be freed up when no
 * longer needed.
 */

void
AddString(
        String *buf,
        int *size,
        String string,
        String header )
{
   int delta;
   int buflen = 0;
   Boolean addHeader = False;

   delta = strlen(DELETE_SEPARATOR) + strlen(string) + 1;
   if (*buf != NULL)
      buflen = strlen(*buf);
   if ((buflen + delta) >= *size)
   {
      if ((*buf == NULL) && header)
         addHeader = True;
      *size += delta + 512;
      if (*buf == NULL)
      {
        *buf = XtMalloc(*size);
        (*buf)[0]='\0';
      }
      else
        *buf = XtRealloc(*buf, *size);

      if (addHeader)
         (void) strcpy(*buf, header);
   }
   (void) strcat(*buf, DELETE_SEPARATOR);
   (void) strcat(*buf, string);
}
