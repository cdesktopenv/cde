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
/* $TOG: Parse.c /main/5 1998/01/12 16:47:01 cshi $ */
/*****************************************************************************
 *
 *   File:         Parse.c
 *
 *   Project:	    CDE
 *
 *   Description:  This file contains the parsing functions for Front Panel
 *                 file information.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 *
 *****************************************************************************/

#include <Dt/DtP.h>
#include <Dt/DbReader.h>
#include <Dt/UserMsg.h>

#include "WmGlobal.h"
#include "WmParse.h"
#include "DataBaseLoad.h"
#include "Parse.h"


/************************************************************************
 *
 *  StringToString
 *
 ************************************************************************/

Boolean
StringToString (char * parse_source,
                void ** parse_return)

{
   *parse_return = (void *) strdup (parse_source);

   return (True);
}




/************************************************************************
 *
 *  StrintToInt
 *
 ************************************************************************/

Boolean
StringToInt (char * parse_source,
             void ** parse_return)

{
   char * source_ptr = parse_source;
   long    value = 0;
   char   chr;


   while (chr = *source_ptr++) 
   {
      if (chr >= '0' && chr <= '9') 
      {
         value *= 10;
         value += chr - '0';
      }
      else
      {
         _DtSimpleError (panel.app_name, DtError, NULL, 
                        "Invalid Integer -- %s", parse_source);
         return (False);
      }
   }

   *parse_return = (void *) value;
   return (True);
}




/************************************************************************
 *
 *  StringToBoolean
 *
 ************************************************************************/

Boolean
StringToBoolean (char * parse_source,
                 void ** parse_return)

{
   _DtWmParseToLower(parse_source);

   if (strcmp (parse_source, "true") == 0)
      *parse_return = (void *) True;
   else if (strcmp (parse_source, "false") == 0)
      *parse_return = (void *) False;
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Boolean -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToResolution
 *
 ************************************************************************/

Boolean
StringToResolution (char * parse_source,
                    void ** parse_return)

{
   _DtWmParseToLower (parse_source);
   
   if (strcmp (parse_source, resolution_types[HIGH]) == 0)
      *parse_return = (void *) HIGH;
   else if (strcmp (parse_source, resolution_types[MEDIUM]) == 0)
      *parse_return = (void *) MEDIUM;
   else if (strcmp (parse_source, resolution_types[LOW]) == 0)
      *parse_return = (void *) LOW;
   else if (strcmp (parse_source, resolution_types[MATCH_DISPLAY]) == 0)
      *parse_return = (void *) MATCH_DISPLAY;
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Resolution -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToControlBehavior 
 *
 ************************************************************************/

Boolean
StringToControlBehavior (char * parse_source,
                         void ** parse_return)

{
   _DtWmParseToLower (parse_source);

   if (strcmp (parse_source, "double_click") == 0)
      *parse_return = (void *) DOUBLE_CLICK;
   else if (strcmp (parse_source, "single_click") == 0)
      *parse_return = (void *) SINGLE_CLICK;
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Control Behavior -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToGeometry
 *
 ************************************************************************/

Boolean
StringToGeometry (char * parse_source,
                  void ** parse_return)

{
   GeometryData *value;
   int mask;
   int x, y, width, height;


   x = y = width = height = 0;

   mask = XParseGeometry ((char *) parse_source, &x, &y, 
                          (unsigned int *) &width, (unsigned int *) &height);

   if (mask)
   {
      /* Allocate space for the geometry structure  */

      value = (GeometryData *) XtMalloc (sizeof (GeometryData));

      value->flags = mask;
      value->x = x;
      value->y = y;
      value->width = width;
      value->height = height;
       
      *parse_return = (void *) value;
   }
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Geometry -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToAction
 *
 ************************************************************************/


Boolean
StringToAction (char * parse_source,
                void ** parse_return)


{
   PanelActionData * action_data;
   unsigned char * string, * source, * head_ptr;


   head_ptr = source = (unsigned char *) strdup (parse_source);

   if ((string = _DtWmParseNextTokenC (&source, False)) != NULL)
   {
      action_data = (PanelActionData *) XtMalloc (sizeof (PanelActionData));

      action_data->action_name = strdup ((char *) string);
      action_data->action_label = NULL;
      action_data->aap = NULL;
      action_data->count = 0;

      while ((string = _DtWmParseNextTokenC (&source, False)) != NULL)
      {
         action_data->count++;
         action_data->aap =
            (DtActionArg *) XtRealloc ((char *) action_data->aap,
			            (sizeof (DtActionArg) * action_data->count));
         action_data->aap[action_data->count-1].argClass = DtACTION_FILE;

         action_data->aap[action_data->count-1].u.file.name =
						 strdup((char *)string);
      }
   }
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Action -- %s", parse_source);
      return (False);
   }

   free ((char *) head_ptr);
   *parse_return = (void *) action_data;
   return (True);
}




/************************************************************************
 *
 *  StringToControlType
 *
 ************************************************************************/

Boolean
StringToControlType (char * parse_source,
                     void ** parse_return)

{
   _DtWmParseToLower (parse_source);

   if (strcmp (parse_source, control_types[CONTROL_BLANK]) == 0)
      *parse_return = (void *) CONTROL_BLANK;
   else if (strcmp (parse_source, control_types[CONTROL_BUSY]) == 0)
      *parse_return = (void *) CONTROL_BUSY;
   else if (strcmp (parse_source, control_types[CONTROL_ICON]) == 0)
      *parse_return = (void *) CONTROL_ICON;
   else if (strcmp (parse_source, control_types[CONTROL_CLIENT]) == 0)
      *parse_return = (void *) CONTROL_CLIENT;
   else if (strcmp (parse_source, control_types[CONTROL_CLOCK]) == 0)
      *parse_return = (void *) CONTROL_CLOCK;
   else if (strcmp (parse_source, control_types[CONTROL_DATE]) == 0)
      *parse_return = (void *) CONTROL_DATE;
   else if (strcmp (parse_source, control_types[CONTROL_FILE]) == 0)
      *parse_return = (void *) CONTROL_FILE;
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Control Type -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToMonitorType
 *
 ************************************************************************/

Boolean
StringToMonitorType (char * parse_source,
                     void ** parse_return)

{
   _DtWmParseToLower (parse_source);

   if (strcmp (parse_source, monitor_types[MONITOR_NONE]) == 0)
      *parse_return = (void *) MONITOR_NONE;
   else if (strcmp (parse_source, monitor_types[MONITOR_MAIL]) == 0)
      *parse_return = (void *) MONITOR_MAIL;
   else if (strcmp (parse_source, monitor_types[MONITOR_FILE]) == 0)
      *parse_return = (void *) MONITOR_FILE;
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Monitor Type -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToControlContainerType
 *
 ************************************************************************/

Boolean
StringToControlContainerType (char * parse_source,
                              void ** parse_return)

{
   if (strcmp (parse_source, entry_types[BOX]) == 0)
      *parse_return = (void *) BOX;
   else if (strcmp (parse_source, entry_types[SUBPANEL]) == 0)
      *parse_return = (void *) SUBPANEL;
   else if (strcmp (parse_source, entry_types[SWITCH]) == 0)
      *parse_return = (void *) SWITCH;
   else
   {
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Control Container Type -- %s", parse_source);
      return (False);
   }

   return (True);
}




/************************************************************************
 *
 *  StringToPositionHints
 *
 ************************************************************************/

Boolean
StringToPositionHints (char * parse_source,
                       void ** parse_return)

{
   Boolean status;

   _DtWmParseToLower (parse_source);

   if (strcmp (parse_source, "first") == 0)
      parse_source = "0";
   else if (strcmp (parse_source, "last") == 0)
      parse_source = "100";

   status = StringToInt (parse_source, parse_return);

   if ((long) *parse_return < 0 || (long) *parse_return > 100)
   {
      
      _DtSimpleError (panel.app_name, DtError, NULL, 
                     "Invalid Position Hints value -- %d",
                     (long) *parse_return);
      return (False);
   }

   return ( True );
}


/************************************************************************
 *
 *  StringToFileName
 *     Converts net file format to a file format.
 *
 ************************************************************************/

Boolean
StringToFileName (char * parse_source,
                  void ** parse_return)

{
   return (StringToString(parse_source, parse_return));
}




/************************************************************************
 *
 *  FreeString
 *
 ************************************************************************/

void
FreeString (void ** parse_value)

{
    XtFree ((char *) *parse_value);
}




/************************************************************************
 *
 *  FreeGeometry
 *
 ************************************************************************/

void
FreeGeometry (void ** parse_value)

{
   XtFree ((char *) *parse_value);
}




/************************************************************************
 *
 *  FreeAction
 *
 ************************************************************************/

void
FreeAction (void ** parse_value)
{
   PanelActionData * actionData = (PanelActionData *) *parse_value;
   int i;

   XtFree (actionData->action_name);

   for (i = 0; i < actionData->count; i++)
     XtFree ((char *) actionData->aap[i].u.file.name);

   XtFree ((char *) actionData->aap);

   XtFree ((char *) *parse_value);
}
