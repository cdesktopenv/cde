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
/* $XConsortium: SharedMsgs.c /main/4 1995/11/02 14:43:46 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           SharedMsgs.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    This file contains the set of functions for accessing
 *                   the set of shared message strings.
 *
 *   FUNCTIONS: GetSharedMessage
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <time.h>
#ifndef sun /* don't need the nl_types.h file */
#include <nl_types.h>
#endif /* ! sun */

#include <Xm/XmP.h>

#include "Encaps.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "SharedMsgs.h"




/************************************************************************
 *
 *   This function takes the id passed in, and returns a pointer to the
 *   associated error message string.  THIS IS NOT A COPY OF THE STRING,
 *   BUT IS A POINTER TO THE REAL THING.  IF THE CALLER NEEDS A COPY, 
 *   THEN IT SHOULD MAKE ONE!  THE RETURNED STRING MUST NOT BE FREED.
 *
 ************************************************************************/

char *
GetSharedMessage(
        int id )

{
   switch (id)
   {
      case CANNOT_READ_DIRECTORY_ERROR:
         return((GETMESSAGE(30,1, "Cannot read from %s")));
      case DIRECTORY_DATE_FORMAT:
         return ((GETMESSAGE(30,2, "%b %d %H:%M:%S %Y")));
      case FILE_RENAME_ERROR_TITLE:
         return (GETMESSAGE(10, 1, "File Rename Error"));
      case FILE_MANIPULATION_ERROR_TITLE:
         return (GETMESSAGE(11, 6, "File Manipulation Error"));
      case CANNOT_CONNECT_ERROR_1:
         return((GETMESSAGE(30,3, "Cannot move or copy file %s from host %s.\nHost %s may not be properly mounted to your file system.")));
      case CANNOT_CONNECT_ERROR_2:
         return((GETMESSAGE(30,4, "Cannot move or copy file %1$s from host %2$s.\nHost %3$s may not be properly mounted to your file system.")));
      case CANT_READ_ERROR:
         return ((GETMESSAGE(30,5, "Cannot read %s")));
      case CANT_WRITE_ERROR:
         return ((GETMESSAGE(30,6, "Cannot write in folder %s")));
      case CANT_CREATE_ERROR:
         return ((GETMESSAGE(30,25, "Cannot create '%s'\n\nThe most common cause is that you do not have\nthe correct permissions for the involved\nfiles or folders.\n\nTo view permissions, select the object and\nthen select 'Change Permissions...' from the\nSelected or popup menu.")));
      case CANT_DELETE_ERROR:
         return ((GETMESSAGE(30,8, "Cannot delete %s")));
      case CANT_OVERWRITE_ERROR:
         return ((GETMESSAGE(30,9, "A folder or file with the name\n%s\nalready exists and cannot be overwritten.\nEnter a different name.")));
      case ACTION_FT_HELP_TITLE:
         return ((GETMESSAGE(30,10, "Description Of Filetype/Action")));
      case FILES_FOUND_LABEL:
         return (GETMESSAGE(15, 40, "Files Found (by Name):"));
      case LOCAL_RENAME_ONLY_ERROR:
         return ((GETMESSAGE(30,11, "You cannot specify a path as part of the filename.")));
      case NO_DIR_ACCESS_ERROR:
         return ((GETMESSAGE(30,12, "Invalid Find Information.\n\nThe following folder does not exist, or is not accessible:\n\n     %s\n")));
      case NO_FILES_FOUND_ERROR:
        return (GETMESSAGE(15, 27, "No files were found that match the search data."));
      case FIND_ERROR_TITLE:
         return (GETMESSAGE(15,43, "Find Error"));
      case FILE_COPY_ERROR_TITLE:
         return (GETMESSAGE(10, 2, "File Copy Error"));
      case NO_SPACES_ALLOWED_ERROR:
         return (GETMESSAGE(10, 3, "Spaces are not allowed in a file or folder name."));
      case MENU_OVERVIEW_LABEL:
         return ((GETMESSAGE(30,13, "%s Overview")));
      case MENU_TUTORIAL_LABEL:
         return ((GETMESSAGE(30,14, "%s Tutorial")));
      case MENU_ABOUT_LABEL:
         return ((GETMESSAGE(30,15, "About %s")));
      case FILE_MGR_MENU_NAME:
         return ((GETMESSAGE(30,16, "File Manager")));
      case CHANGE_DIR_ERROR_TITLE:
         return (GETMESSAGE(2, 13, "Go To Error"));
      case TRASH_ERROR_TITLE:
         return (GETMESSAGE(27, 3, "Trash Can Error"));
      case FILTER_EDIT_ID_STRING:
         return ("filter_edit");
      case FILTER_ACTIVE_ID_STRING:
         return ("filter_active");
      case UP_ONE_LEVEL_LABEL:
         return ((GETMESSAGE(30,18, "(go up)")));
      case ITEM_HELP_ERROR_TITLE: 
         return ((GETMESSAGE(30,19, "Help On Item Error")));
      case ITEM_HELP_ERROR:
         return ((GETMESSAGE(30,24, "You must select an object within the File Manager.\n'On Item' help only works within the application where\nyou launched it.")));
      case AMITEM_HELP_ERROR:
         return ((GETMESSAGE(30,26, "You must select an object within the Application Manager.\n'On Item' help only works within the application where\nyou launched it.")));
      case FILE_MOVE_ERROR_TITLE:
         return GETMESSAGE(30,21, "File Move Error");
      case FILE_LINK_ERROR_TITLE:
         return GETMESSAGE(30,22, "File Link Error");
   }
}
