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
/************************************************************************
 *
 *  File:	 Session.c
 *
 *  Project:     CDE
 *
 *  Description: This file contains the session management code for the
 *               CDE front panel.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 *
 ************************************************************************/

#include <Dt/WsmP.h>

#include <Dt/Control.h>
#include <Dt/ControlP.h>

#include "DataBaseLoad.h"


#include <pwd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>



extern void ToggleDefaultControl(ControlData *, SubpanelData *, ControlData *);
extern Boolean CheckOtherMonitorsOn(SubpanelData *);



/*  This define is used as the file name and location of the session  */
/*  file needed by the front panel to store its across session data.  */

static char * SESSION_FILE = "/.dt/sessions/dtwmfp.session";
static char * TYPES_DIR = "/.dt/types/fp_dynamic/";


extern String unpost_arrow_image;
extern String unpost_monitor_arrow_image;

#define MAX_PATH        1024


/************************************************************************
 *
 *  SessionFileGetName
 *	Generate the name of the session file to be used to read and
 *	write the front panel session information.
 *
 ************************************************************************/
 
static char *
SessionFileGetName (void)
{
   char * home_dir;
   char * file_name;
   struct passwd * pw_info;
   

   /*  Get the home directory used to build the path to the session file.  */

   if ((home_dir = (char *)getenv("HOME")) != NULL || strlen (home_dir) == 0)
   {
      pw_info = getpwuid (getuid());
      home_dir = pw_info->pw_dir;
   }

   file_name = XtMalloc (strlen (home_dir) + strlen (SESSION_FILE) + 1);
   sprintf (file_name, "%s%s", home_dir, SESSION_FILE);
   
   return (file_name);
}




/************************************************************************
 *
 *  SessionRestoreData
 *	This function reads the front panels session file and applies
 *	the data to the already created front panel.
 *
 ************************************************************************/

void
SessionRestoreData (void)
{
   FILE * fd;

   char * session_file;
   
   BoxData * box_data;
   ControlData * control_data;

   SubpanelData ** subpanel_data = NULL;
   int subpanel_count = 0;
   int max_subpanel_count = 0;
   
   int subpanel_posted_count = 0;
   int subpanel_default_count = 0;

   char name_buffer1[256];
   char name_buffer2[256];
   char name_buffer3[256];

   struct stat stat_info;

   int     parent_component_type;
   int     component_type;
   int     delete_value;
   Boolean delete;

   int dynamic_data_count;
   
   int subpanel_x;
   int subpanel_y;
   int subpanel_torn;

   DtWmHints vHints;
   
   Boolean have_it;
   
   Dimension width;
   Dimension new_width;
   Widget    parent;
   
   int mapped;

   int i, j, k, l;
   Arg al[2];

   
   session_file = SessionFileGetName ();


   if ((fd = fopen (session_file, "r")) != NULL)
   {
      /*  Get the full list of subpanels  */
      
      for (i = 0; i < panel.box_data_count; i++)
      {
         box_data = panel.box_data[i];
	 
	 for (j = 0; j < box_data->control_data_count; j++)
	 {
	    control_data = box_data->control_data[j];
	    
	    if (control_data->subpanel_data != NULL)
	    {
               if (subpanel_count == max_subpanel_count)
	       {
		  max_subpanel_count += 10;
		  subpanel_data = (SubpanelData **)
		    XtRealloc ((char *) subpanel_data, 
		               sizeof (SubpanelData *) * max_subpanel_count);
	       }

	       subpanel_data[subpanel_count] = control_data->subpanel_data;
	       subpanel_count++;
	    }
	 }
      }


      /*  Get the count of the number of subpanels to be posted.  */

      fscanf (fd, "%d", &subpanel_posted_count);	 


      /*  If the count is greater than 0, read in the list of subpanel  */
      /*  names, compare them with the list of subpanels and post the   */
      /*  appropriate ones.                                             */
	 
      for (i = 0; i < subpanel_posted_count; i++)
      {
         fscanf (fd, "%s%d%d%d", 
                 name_buffer1, &subpanel_x, &subpanel_y, &subpanel_torn);
	    
         for (j = 0; j < subpanel_count; j++)
         {
            if (strcmp (name_buffer1,
                        subpanel_data[j]->
                        element_values[SUBPANEL_NAME].parsed_value) == 0)
            {
               /*  If the subpanel is torn, modify the window manager    */
	       /*  behavior hints so that it can be positioned properly  */
	       /*  and does not slide up.                                */
	       
               if (subpanel_torn != 0)
               {
                  subpanel_data[j]->torn = True;

                  vHints.flags = 
		     DtWM_HINTS_BEHAVIORS | DtWM_HINTS_ATTACH_WINDOW;
                  vHints.behaviors = 
		     DtWM_BEHAVIOR_PANEL | DtWM_BEHAVIOR_SUBPANEL | 
		                           DtWM_BEHAVIOR_SUB_RESTORED;
                  vHints.attachWindow = XtWindow (panel.shell);

                  _DtWsmSetDtWmHints (XtDisplay (panel.shell),
                                      XtWindow (subpanel_data[j]->shell), 
				      &vHints);
               }


               /*  Manage and position the form and shell of the subpanel  */
	       /*  Note: this is a close copy of the ArrowCB posting       */
	       /*  function logic.                                         */
	       
               XtSetMappedWhenManaged (subpanel_data[j]->shell, False);
               XtManageChild (subpanel_data[j]->form);

               XtSetArg (al[0], XmNx, subpanel_x);
               XtSetArg (al[1], XmNy, subpanel_y);
               XtSetValues (subpanel_data[j]->form, al, 2);

               XtManageChild (subpanel_data[j]->shell);
               XtSetMappedWhenManaged (subpanel_data[j]->shell, True);

               if (!CheckOtherMonitorsOn(subpanel_data[j]))
                  XtSetArg (al[0], XmNimageName, unpost_arrow_image);
               else
                  XtSetArg (al[0], XmNimageName, unpost_monitor_arrow_image);
               XtSetValues(subpanel_data[j]->parent_control_data->arrow, al, 1);
            }
         }
      }	 


      /*  Get the count of the number of subpanel controls to be  */
      /*  toggled into the main panel.                            */
	 
      fscanf (fd, "%d", &subpanel_default_count);


      /*  If the count is greater than 0, read in the list of subpanel  */
      /*  names, and control_names and compare them with the list of    */
      /*  subpanels and their controls.  Toggle the proper control.     */

      for (i = 0; i < subpanel_default_count; i++)
      {
         fscanf (fd, "%s%s", name_buffer1, name_buffer2);
	    
         for (j = 0; j < subpanel_count; j++)
         {
            if (strcmp (name_buffer1,
                        subpanel_data[j]->
                        element_values[SUBPANEL_NAME].parsed_value) == 0)
            {
               for (k = 0; k < subpanel_data[j]->control_data_count; k++)
               {
                  if (strcmp (name_buffer2,
                              subpanel_data[j]->control_data[k]->
                              element_values[CONTROL_NAME].parsed_value) == 0)
                  {
                     /*  use the difference in width of the old and new  */
		     /*  main controls to adjust the parent form.  This  */
		     /*  should not be necessary but form is buggy.      */

                     width = XtWidth(subpanel_data[j]->parent_control_data->icon);

                     ToggleDefaultControl(subpanel_data[j]->parent_control_data,
		                          subpanel_data[j],
                                          subpanel_data[j]->control_data[k]);

                     new_width = XtWidth (subpanel_data[j]->parent_control_data->icon);

                     parent = XtParent (subpanel_data[j]->parent_control_data->icon);
                     XtSetArg (al[0], XmNwidth, XtWidth (parent) + new_width - width);
                     XtSetValues (parent, al, 1);
                  }
               }
            }
         }
      }	 

   
      /*  Read in the count of the number of dynamic component records  */
      
      fscanf (fd, "%d", &panel.dynamic_data_count);
      panel.max_dynamic_data_count = panel.dynamic_data_count;

      if (panel.dynamic_data_count > 0) {
	panel.dynamic_data_list = (DynamicComponent **)
	  XtMalloc (sizeof (DynamicComponent *) * panel.max_dynamic_data_count);
      }

      dynamic_data_count = 0;
      

      /*  Loop through and read all of the records  */
      
      for (i = 0; i < panel.dynamic_data_count; i++)
      {
         fscanf(fd, "%s%s%d%s%d%d", name_buffer1, name_buffer2, &component_type,
	        name_buffer3, &parent_component_type, &delete_value);


         /*  Stat the file name to see if it still exists.  If not,    */
	 /*  continue at the next iteration of the loop.  This has     */
	 /*  the effect of not putting the data into the dynamic list  */
	 /*  so that it will not be written on the next shutdown.      */
	 
	 if (lstat (name_buffer1, &stat_info) != 0)
	    continue;


         delete = False;
         if (delete_value == 1) delete = True;


         /*  Loop through the subpanels and verify that each of the    */
	 /*  records matches component within the hierarchy.  If not,  */
	 /*  do not put the record in the list.                        */
      
         for (j = 0; j < subpanel_count; j++)
         {
            have_it = False;
	    

            /*  If the data is a reference to a deleted component, put   */
	    /*  it in the list.  If it is a subpanel, verify that there  */
	    /*  is a subpanel of that name and a main control parent of  */
	    /*  the correct name.  If it is a control, reloop through    */
	    /*  the subpanel set a look for a control which matches the  */
	    /*  name and a subpanel which matches the parent name.       */

            if (delete)
	       have_it = True;
            else if (component_type == SUBPANEL && 
                     strcmp (subpanel_data[j]->
                             element_values[SUBPANEL_NAME].parsed_value,
                             name_buffer2) == 0 &&
                     strcmp (subpanel_data[j]->
                             element_values[SUBPANEL_CONTAINER_NAME].parsed_value,
                             name_buffer3) == 0)
            {
	       have_it = True;
            }
            else
            {
               if (parent_component_type == SUBPANEL)
               {
                  for (k = 0; k < subpanel_count; k++)
                  {
                     if (strcmp (subpanel_data[k]->
                         element_values[SUBPANEL_NAME].parsed_value,
                         name_buffer3) == 0)
                     {
                        for (l = 0; l < subpanel_data[k]->control_data_count; l++)
                        {
                           if (strcmp (subpanel_data[k]->control_data[l]->
                               element_values[CONTROL_NAME].parsed_value,
                               name_buffer2) == 0)
                           {
                              have_it = True;
                              break;
                           }
                        }
                     }

                     if (have_it) 
		        break;
                  }
               }
               else if (parent_component_type == BOX)
	       {
                  for (k = 0; k < panel.box_data_count; k++)
                  {
                     if (strcmp (panel.box_data[k]->
                         element_values[BOX_NAME].parsed_value,
                         name_buffer3) == 0)
                     {
                        for (l = 0; l < panel.box_data[k]->control_data_count; l++)
                        {
                           if (strcmp (panel.box_data[k]->control_data[l]->
                               element_values[CONTROL_NAME].parsed_value,
                               name_buffer2) == 0)
                           {
                              have_it = True;
                              break;
                           }
                        }
                     }

                     if (have_it) 
		        break;
                  }
               }
	    }	    

            if (have_it)
            {
               panel.dynamic_data_list[dynamic_data_count] = 
                  (DynamicComponent *) XtMalloc (sizeof (DynamicComponent));

               panel.dynamic_data_list[dynamic_data_count]->file_name = XtNewString (name_buffer1);
               panel.dynamic_data_list[dynamic_data_count]->component_name = XtNewString (name_buffer2);
               panel.dynamic_data_list[dynamic_data_count]->component_type = component_type;
               panel.dynamic_data_list[dynamic_data_count]->parent_name = XtNewString (name_buffer3);
               panel.dynamic_data_list[dynamic_data_count]->parent_type = parent_component_type;
               panel.dynamic_data_list[dynamic_data_count]->delete = delete;

               dynamic_data_count++;

               break;
            }
         }
      }

      panel.dynamic_data_count = dynamic_data_count;

   
      /*  Read whether the front panel is iconic or not and set the  */
      /*  hints on the shell appropriately.                          */
      
      if (fscanf (fd, "%d", &mapped) == 1)
      {
	 if (mapped == 0)
            XIconifyWindow (XtDisplay(panel.shell), XtWindow(panel.shell),
	                    XScreenNumberOfScreen (XtScreen (panel.shell)));
      }

      fclose (fd);
   }


   if (subpanel_data)
      XtFree ((char *) subpanel_data); 

   XtFree (session_file);
}





/************************************************************************
 *
 *  WmFPSessionSaveData
 *	This function calculates and save the front panels session data
 *	needed for the next restart.  It is called from the window manager
 *	in its SaveResources function.
 *
 ************************************************************************/

void
WmFrontPanelSessionSaveData (void)
{
   FILE * fd;

   char * session_file;
   Boolean written = False;
   
   BoxData * box_data;
   ControlData * control_data;

   SubpanelData ** subpanel_data = NULL;
   int subpanel_count = 0;
   int max_subpanel_count = 0;
   
   int subpanel_posted_count = 0;
   int subpanel_default_count = 0;

   XWindowAttributes window_attributes;

   Arg al[2];

   int i,j;


   /*  Set the session file name and open the file  */

   session_file = SessionFileGetName ();

   if ((fd = fopen (session_file, "w")) != NULL)
   {

      /*  Get the full list of subpanels  */
      
      for (i = 0; i < panel.box_data_count; i++)
      {
         box_data = panel.box_data[i];
	 
	 for (j = 0; j < box_data->control_data_count; j++)
	 {
	    control_data = box_data->control_data[j];
	    
	    if (control_data->subpanel_data != NULL)
	    {
               if (subpanel_count == max_subpanel_count)
	       {
		  max_subpanel_count += 10;
		  subpanel_data = (SubpanelData **)
		    XtRealloc ((char *) subpanel_data, 
		               sizeof (SubpanelData *) * max_subpanel_count);
	       }

	       subpanel_data[subpanel_count] = control_data->subpanel_data;

               if (XtIsManaged (subpanel_data[subpanel_count]->shell))
	          subpanel_posted_count++;

	       if (subpanel_data[subpanel_count]->default_control !=
	           subpanel_data[subpanel_count]->parent_control_data)
		  subpanel_default_count++;

	       subpanel_count++;
	    }
	 }
      }
       

      /*  Save off the list of subpanels that are posted  */

      fprintf (fd, "%d\n", subpanel_posted_count);

      if (subpanel_posted_count > 0)
      {
         for (i = 0; i < subpanel_count; i++)
         {
            if (XtIsManaged (subpanel_data[i]->shell))
               fprintf (fd, "%s %d %d %d\n",
	                 (char *) subpanel_data[i]->
                         element_values[SUBPANEL_NAME].parsed_value,
                         XtX (subpanel_data[i]->shell),
                         XtY (subpanel_data[i]->shell),
			 (int) subpanel_data[i]->torn);
         }
      }
            

      /*  Save off the controls that have been toggled from the  */
      /*  subpanel into the main panel.                          */

      fprintf (fd, "%d\n", subpanel_default_count);

      if (subpanel_default_count > 0)
      {
         for (i = 0; i < subpanel_count; i++)
         {
            if (subpanel_data[i]->default_control !=
                subpanel_data[i]->parent_control_data)
            {
               fprintf (fd, "%s	%s\n", 
	                  (char *) subpanel_data[i]->
	                            element_values[SUBPANEL_NAME].parsed_value,
	                  (char *) subpanel_data[i]->default_control->
	                            element_values[CONTROL_NAME].parsed_value);
	       
	    }
         }
      }

   
      /*  Save off the dynamic component information  */
      
      fprintf (fd, "%d\n", panel.dynamic_data_count);
      
      for (i = 0; i < panel.dynamic_data_count; i++)
      {
         fprintf (fd, "%s %s %d %s %d %d\n", 
                  panel.dynamic_data_list[i]->file_name,
                  panel.dynamic_data_list[i]->component_name,
                  panel.dynamic_data_list[i]->component_type,
                  panel.dynamic_data_list[i]->parent_name,
                  panel.dynamic_data_list[i]->parent_type,
		  (int) panel.dynamic_data_list[i]->delete);
      }


      /*  Save off whether the front panel is iconfied or not.  */
      
      XGetWindowAttributes (XtDisplay (panel.shell), XtWindow (panel.shell),
                            &window_attributes);

      if (window_attributes.map_state == IsUnmapped)
         fprintf (fd, "%d\n", 0);
      else
         fprintf (fd, "%d\n", 1);

      fflush (fd);
      fclose (fd);
   }


   if (subpanel_data)
      XtFree ((char *) subpanel_data);

   XtFree (session_file);
}




/************************************************************************
 *
 *  SessionAddFileData
 *	This function adds a new session file record to the list 
 *	attached to panel.  It is called when a new control or 
 *	subpanel is dynamically added.
 *
 *  Inputs: file_name - the name of the file the component is stored in
 *          component_name - the value of the name field of the component
 *          component_type - a value of CONTROL or SUBPANEL
 *          parent_name - the container of the component
 *          parent_type - a value of SUBPANEL or CONTROL
 *          delete - a Boolean indicating whether the component has been
 *                   deleted.  This requires special handling on startup.
 *
 ************************************************************************/

void
SessionAddFileData (char  * file_name,
                    char  * component_name,
                    int     component_type,
                    char  * parent_name,
                    int     parent_type,
                    Boolean delete)


{
   int count = panel.dynamic_data_count;

   if (count == panel.max_dynamic_data_count)
   {
       panel.max_dynamic_data_count += 10;
       panel.dynamic_data_list = (DynamicComponent **)
          XtRealloc ((char *) panel.dynamic_data_list,
             sizeof (DynamicComponent *) * panel.max_dynamic_data_count);
   }

   panel.dynamic_data_list[count] = 
      (DynamicComponent *) XtMalloc (sizeof (DynamicComponent));

   panel.dynamic_data_list[count]->file_name = XtNewString (file_name);
   panel.dynamic_data_list[count]->component_name = XtNewString (component_name);
   panel.dynamic_data_list[count]->component_type = component_type;
   panel.dynamic_data_list[count]->parent_name = XtNewString (parent_name);
   panel.dynamic_data_list[count]->parent_type = parent_type;
   panel.dynamic_data_list[count]->delete = (int) delete;
   panel.dynamic_data_count++;
}




/************************************************************************
 *
 *  SessionDeleteFileData
 *	This function deletes a session file record from the list 
 *	attached to panel.  It is called when a componentent is deleted
 *	from the front panel.
 *
 *  Inputs: file_name - The name of the file containing the component
 *          definition.  This is used to look up in the session list.
 *
 ************************************************************************/

void
SessionDeleteFileData (char * file_name)


{
   int i, j;
   

   /*  Loop through the dynamic data list and find the matching record  */
   /*  Free the data attached to the record and the record.  Then move  */
   /*  the list up to fill the empty space.                             */
   
   for (i = 0; i < panel.dynamic_data_count; i++)
   {
      if (strcmp (file_name, panel.dynamic_data_list[i]->file_name) == 0)
      {
         XtFree ((char *) panel.dynamic_data_list[i]->file_name);
         XtFree ((char *) panel.dynamic_data_list[i]->component_name);
         XtFree ((char *) panel.dynamic_data_list[i]->parent_name);
	 XtFree ((char *) panel.dynamic_data_list[i]);
	 
	 for (j = i; j < panel.dynamic_data_count - 1; j++)
            panel.dynamic_data_list[j] = panel.dynamic_data_list[j + 1];

         panel.dynamic_data_count--;

         break;
      }
   }
}




/************************************************************************
 *
 *  SessionFileNameLookup
 *	This function locates and returns the name of a dynamic component
 *	.fp file.
 *
 *  Inputs: component_name - the name of the dynamic component.
 *	    component_type - the type (CONTROL, SUBPANEL) of the component.
 *          parent_name - the name of the container of the component.
 *          parent_type - the type (SUBPANEL, CONTROL) of the parent.
 *
 *  Outputs: The file name of the dynamic component file or NULL if not found.
 *
 ************************************************************************/

char *
SessionFileNameLookup (char * component_name,
                       int    component_type,
                       char * parent_name,
                       int    parent_type)


{
   int i;
   

   /*  Loop through the dynamic data list and find the matching record  */
   /*  Return the file name when found or NULL if not found             */
   /*  the list up to fill the empty space.                             */
   
   for (i = 0; i < panel.dynamic_data_count; i++)
   {
      if (strcmp (component_name, 
                  panel.dynamic_data_list[i]->component_name) == 0     &&
	  component_type == panel.dynamic_data_list[i]->component_type &&
	  strcmp (parent_name, 
                  panel.dynamic_data_list[i]->parent_name) == 0        &&
	  parent_type == panel.dynamic_data_list[i]->parent_type)
      {
         return (panel.dynamic_data_list[i]->file_name);
      }
   }

   return ((char *) NULL);
}

/************************************************************************
 *
 *  SessionDeleteAll
 *	This function deletes all of the session file records from the list 
 *	attached to panel.  It is called when a request is made to restore
 *	the default front panel.
 *
 *  Inputs: None
 *
 ************************************************************************/

void
SessionDeleteAll(void)
{
   DIR *dir;
   struct dirent *entry;          /* directory entry */
   char srcname[MAX_PATH];
   int srclen;
   char * home_dir;
   char * fp_dir;
   struct passwd * pw_info;
   int i;


   /*  Get the home directory used to build the path to the session file.  */

   if ((home_dir = (char *)getenv("HOME")) != NULL || strlen (home_dir) == 0)
   {
      pw_info = getpwuid (getuid());
      home_dir = pw_info->pw_dir;
   }

  /* Add path to fp_dynamic directory */
   fp_dir = XtMalloc (strlen(home_dir) + strlen(TYPES_DIR) + 1);
   sprintf (fp_dir, "%s%s", home_dir, TYPES_DIR);

    
  /* Open the fp_dynamic directory */
   dir = opendir(fp_dir);

   if (dir == NULL)
     return;

  /* prepare source name */
   strcpy(srcname, fp_dir);
   srclen = strlen(fp_dir);
   if (srcname[srclen - 1] != '/')
     srcname[srclen++] = '/';

  /*  Loop through all the files in the fp_dyamic directory and
   *  remove them.
   */

   while ((entry = readdir(dir)) != NULL)
   {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
         continue;
      strcpy(srcname + srclen, entry->d_name);
      remove (srcname);
   }
   
   closedir(dir);
}

