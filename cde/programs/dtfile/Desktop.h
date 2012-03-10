/* $XConsortium: Desktop.h /main/4 1996/10/03 17:04:47 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Desktop.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the desktop.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Desktop_h
#define _Desktop_h
#include "FileMgr.h"

#define MAX_CACHED_ICONS 20

#define INVALID_TYPE 999

typedef struct
{
   Widget shell;
   Widget frame;
   Widget drawA;
   Widget iconGadget;
   Widget text;

   Position root_x, root_y;
   Pixel background;

   char *workspace_name;
   int workspace_num;

   unsigned char view;
   unsigned char order;
   unsigned char direction;
   unsigned char positionEnabled;

   char * restricted_directory;
   char * title;
   char * helpVol;
   Boolean toolbox;
   Boolean registered;

   char *host;
   char *dir_linked_to;
   char *file_name;
   FileViewData *file_view_data;
} DesktopRec;

typedef struct
{
   char *name;
   int number;
   int files_selected;
   DesktopRec **selectedDTWindows;
   DialogData * primaryHelpDialog;
   int secondaryHelpDialogCount;
   DialogData ** secondaryHelpDialogList;
} WorkspaceRec;

typedef struct
{
   Widget popup;
   Widget removeDT;
   Widget openFolder;
   Widget rename;
   char * action_pane_file_type;
} PopupRec;

typedef struct
{
   int numIconsUsed;
   int numCachedIcons;
   int popup_name_count;
   PopupRec *popupMenu;
   int numWorkspaces;
   WorkspaceRec ** workspaceData;
   DesktopRec ** desktopWindows;
   XButtonEvent event;
} DesktopData, *DesktopDataPtr;

extern DesktopData *desktop_data;

#endif /* _Desktop_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
