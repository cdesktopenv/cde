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
/* $XConsortium: Print.c /main/4 1995/11/01 11:29:33 rswiston $ */
/*****************************************************************************
 *
 *   File:         Print.c
 *
 *   Project:	    DT
 *
 *   Description:  This file contains the function which prints out the
 *                 front panel contents.  It is used as a client for this
 *                 purpose.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 *
 ****************************************************************************/

#include <Dt/DtP.h>                /* required for DtDirPaths type */
#include <Dt/DbReader.h>            /* required for DtDbPathId type */
#include <Dt/WsmM.h>
#include <Dt/IconFile.h>
#include <Dt/Dt.h>
#include "DataBaseLoad.h"
#include "Parse.h"
#include <stdint.h>

#ifdef __STDC__
const char *szWM_TOOL_CLASS = DtWM_TOOL_CLASS;
#else
char *szWM_TOOL_CLASS = DtWM_TOOL_CLASS;
#endif /* __STDC__ */



String unpost_arrow_image;
String unpost_monitor_arrow_image;

/************************************************************************
 *
 *  CheckOtherMonitorsOn
 *      Due to a dependancy, CheckOtherMonitorsOn must be defined.  This
 *      is a stubbed procedure that ensures that the procedure is defined.
 *
 ************************************************************************/
Boolean
CheckOtherMonitorsOn(SubpanelData * subpanel_data)
{
   return False;
}


/************************************************************************
 *
 *  ToggleDefaultControl
 *      Due to a dependancy, ToggleDefaultControl must be defined.  This
 *      is a stubbed procedure that ensures that the procedure is defined.
 *
 ************************************************************************/


void
ToggleDefaultControl (ControlData  * main_control_data,
                      SubpanelData * subpanel_data,
                      ControlData  * control_data)


{
}




/************************************************************************
 *
 *  GetIconName
 *      Get the file name for an icon by extracting the panel resolution
 *      and then looking up the image name.
 *
 ************************************************************************/


String
GetIconName (String       image_name, 
             unsigned int icon_size)


{
   String       return_name;
   Screen     * screen = XtScreen (panel.shell);

   /*  Get name.  */

   return_name = _DtGetIconFileName (screen, image_name, NULL, NULL, icon_size);

   if (return_name == NULL)
       return_name = _DtGetIconFileName (screen, image_name, NULL, NULL,
		                         DtUNSPECIFIED);
   if (return_name == NULL)
       return_name = XtNewString (image_name);


   /*  Return value to be freed by caller.  */

   return (return_name);
}




/************************************************************************
 *
 *  PrintFrontPanelContents
 *      Using the information from the loading of the database
 *      print out the contents of the database.
 *
 ************************************************************************/

void

PrintFrontPanelContents(void)

{
   BoxData * box_data;
   SwitchData * switch_data;
   ControlData * control_data, * switch_control_data;
   SubpanelData * subpanel_data;
   int switch_position = POSITION_FIRST;
   int i,j, k;


   /*  print out the component tree  */
   
   printf ("PANEL	%s\n", (char *) panel.element_values[0].parsed_value);

   for (i = 0; i < panel.box_data_count; i++)
   {
      box_data = panel.box_data[i];

      if (box_data->switch_data != NULL)
      {
         switch_data = box_data->switch_data;
         switch_position = (int) (intptr_t) box_data->switch_data->element_values[SWITCH_POSITION_HINTS].parsed_value;
      }
      else
         switch_data = NULL;


      printf ("   BOX	%s\n", 
              (char *) box_data->element_values[0].parsed_value);

      for (j = 0; j < box_data->control_data_count; j++)
      {
         control_data = box_data->control_data[j];

         if (switch_data != NULL &&
             switch_position < (int) (intptr_t) (control_data->element_values[CONTROL_POSITION_HINTS].parsed_value))
         {
            printf ("      SWITCH	%s\n",
                 (char *) switch_data->element_values[0].parsed_value);

            switch_position = POSITION_LAST;

            for (k = 0; k < switch_data->control_data_count; k++)
            {
		switch_control_data = switch_data->control_data[k];

                printf ("         CONTROL	%s\n", (char *)
			switch_control_data->element_values[0].parsed_value);

            }
         }

	 printf ("      CONTROL	%s\n",
                 (char *) control_data->element_values[0].parsed_value);

         if (control_data->subpanel_data != NULL)
	 {
            subpanel_data = control_data->subpanel_data;

            printf("         SUBPANEL	%s\n", (char *)
		    subpanel_data->element_values[0].parsed_value);
      
            for (k = 0; k < subpanel_data->control_data_count; k++)
            {
                control_data = subpanel_data->control_data[k];

                printf ("               CONTROL	%s\n", (char *)
			control_data->element_values[0].parsed_value);
            }
	 }
      }
   }
}




/************************************************************************
 *
 *  The main program for the fron panel print function.
 *
 ************************************************************************/

int
main (int argc,
      char         **argv)


{
    XtAppContext    appContext;
    Widget		widget;

   /* This call is required to have values to pass to DtAppInitialize */
    widget = XtAppInitialize( &appContext, "Dtfplist",
                              NULL, 0, &argc, argv, NULL, NULL, 0);

   /* This is required initialization so that FrontPanelReadDatabases()
    * procedure complete successfully.
    */
    DtAppInitialize( appContext, XtDisplay(widget), widget,
		     argv[0], (char *)szWM_TOOL_CLASS);

    
   /* Load the database for use in printing. If it is able to load print
    * contents of front panel.
    */
    DtDbLoad();

    panel.app_name = strdup(argv[0]);
    if (FrontPanelReadDatabases ())
    {
      /* Print out the contents of the .fp database */
       PrintFrontPanelContents ();
    }
    else
    {
      /* NEEDS TO BE LOCALIZED */
       printf ("PANEL not found. Error in reading database.\n");
    }

}
