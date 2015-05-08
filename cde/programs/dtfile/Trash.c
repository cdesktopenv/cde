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
/* $TOG: Trash.c /main/12 1999/12/09 13:07:25 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Trash.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Source file for the trash code.
 *
 *   FUNCTIONS: AddToDeleteList
 *		CheckDeletePermission
 *		CheckDeletePermissionRecur
 *		CloseTrash
 *		ConfirmCancel
 *		ConfirmOk
 *		ConfirmRemove
 *		CreateTrashFilename
 *		CreateTrashMenu
 *		DropOnTrashCan
 *		EmptyTrash
 *		EmptyTrashPipeCB
 *		EmptyTrashProcess
 *		EraseDir
 *		EraseObject
 *		FileFromTrash
 *		FileSysType
 *		InitializeTrash
 *		MatchesSacredDirectory
 *		MessageToFileList
 *		MessagesToFileList
 *		MoveOutOfTrashCan
 *		MoveToTrash
 *		MoveToTrashPipeCB
 *		MoveToTrashProcess
 *		Noop
 *		ReadTrashList
 *		Remove
 *		RemoveCancelCB
 *		RemoveOkCB
 *		Restore
 *		RestoreFromTrash
 *		RestorePipeCB
 *		RestoreProcess
 *		Select_All
 *		SensitizeTrashBtns
 *		TrashCreateDialog
 *		TrashDisplayHandler
 *		TrashEmpty
 *		TrashEmptyHandler
 *		TrashIsInitialized
 *		TrashRemoveHandler
 *		TrashRemoveNoConfirmHandler
 *		TrashRestoreHandler
 *		Unselect_All
 *		UpdateDirectoryOf
 *		VerifyCancel
 *		VerifyCleanup
 *		VerifyOk
 *		WriteEntry
 *		WriteTrashEntries
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <EUSCompat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#if defined(CSRG_BASED)
#include <sys/param.h>
#include <sys/mount.h>
#else
#include <ustat.h>
#endif

#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/MainW.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/MessageB.h>
#include <Xm/MwmUtil.h>
#include <Xm/XmP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <Xm/Protocols.h>

#include <Dt/Action.h>
#include <Dt/DtP.h>
#include <Dt/FileM.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Connect.h>
#include <Dt/Indicator.h>
#include <Dt/EnvControlP.h>
#include <Dt/Wsm.h>
#include <Dt/Dnd.h>
#include <Dt/SharedProcs.h>
#include "DtSvcInternal.h" /* _DtGetMask */

#include <Tt/tttk.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "MultiView.h"
#include "Help.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "SharedMsgs.h"
#include "dtcopy/fsrtns.h"

#define AdditionalHeader (GETMESSAGE(27,98, "(Plus %d additional object(s))"))

/* Trash file errors */
#define NO_FILE_ERROR         0
#define BAD_FILE_ERROR       -1
#define VERIFY_DIR           -2
#define VERIFY_FILE          -3
#define BAD_TRASH_DIRECTORY  -4
#define BAD_TRASH_FILE       -5
#define NO_TRASH_FILE        -6
#define SKIP_FILE            -7
#define BAD_TRASH            -8
#define BAD_FILE_SACRED      -9

/* types of messages sent through the pipe */
#define PIPEMSG_FILEOP_ERROR  1
#define PIPEMSG_OTHER_ERROR   3
#define PIPEMSG_DONE          7
#define PIPEMSG_TARGET_TIME   5
#define PIPEMSG_FILE_MODIFIED 6

/*
 * Structure describing each file in the trash can.
 * Includes the external host:/name format, the original internal
 * /nfs/host/name format, and the internal /nfs/host/new_name describing
 * the temporary location of the file.  The `problem' flag is used to
 * indicate if a problem occurred when the file was physically being
 * removed.
 */

typedef struct {
   Boolean problem;
   String  intNew;
   String  intOrig;
   String  external;
   String  filename;
} TrashEntry;

/* callback data MoveToTrash */
typedef struct
{
   char **file_list;
   int file_count;
   int done_count;
   char **path;
   char **to;
   int *rc;
   Tt_message msg;
#ifdef SUN_PERF
   Tt_message *msg_list ;
   int msg_cnt ;
#endif /* SUN_PERF */
   int child;
} MoveToTrashCBData;

#ifdef SUN_PERF
typedef struct
{
   Tt_message *msg_list ;
   int msg_cnt ;
} Tt_msg_cache ;
#endif /* SUN_PERF */

/* callback data RestoreFromTrash */
typedef struct
{
   char **file_list;
   int file_count;
   char *target_host;
   char *target_dir;
   int *rc;
   Tt_message msg;
   int child;
} RestoreFromTrashCBData;

/* callback data EmptyTrash */
typedef struct
{
   char *trash;
   char *orig;
} DeleteList;

typedef struct
{
   DeleteList *del_list;
   int del_count;
   int removeType;
   int *rc;
   Tt_message msg;
   int child;
} EmptyTrashCBData;


DialogData *trashDialogData;          /* fm data associated with trash dialog */
FileMgrData *trashFileMgrData = NULL;

Boolean removingTrash = False;
Widget trashShell;
DialogData * primaryTrashHelpDialog = NULL;
DialogData ** secondaryTrashHelpDialogList = NULL;
int secondaryTrashHelpDialogCount = 0;

/* Forward prototype */
int EraseDir(char *dir_name);
/* From dtcopy/fsrtns.c */
extern int EmptyDir(char *sourceP, int rm, int force);

                                            /* 'defines' for trash files */
static char * TRASH_DIR = ".dt/Trash";
static char * TRASH_INFO_FILE = ".dt/Trash/.trashinfo";
static char * NEW_TRASH_INFO_FILE = ".dt/.tmptrashinfo";

static char * RM = "/bin/rm";
static char * RM_ARGS = "-rf";

static Widget * selectAllBtn = NULL;
static Widget * restoreBtn = NULL;
static Widget * removeBtn = NULL;

static int trashMenuItemCount = 22;         /* trash menu items */
static MenuDesc * trashMenu = NULL;

static TrashEntry * trashCan = NULL;
static int trashListSize = 0;
static int numTrashItems = 0;

static Boolean trashDialogPosted;

static char * TrashInfoFileName = NULL;
static char * NewTrashInfoFileName = NULL;

static char ** sacred_dir_list = NULL;   /* list of directories in trash path */
static int sacred_dir_list_size = 0;
static int sacred_dir_count = 0;

static Boolean verifyPromptsEnabled;     /* do we prompt the user? */

static Tt_message global;
#ifdef SUN_PERF
static Tt_message *global_msg_list ;
static int global_msg_cnt = 0 ;
#endif /*  SUN_PERF */
static Boolean TrashInitialized = False;

/********    Static Function Declarations    ********/

static String CreateTrashFilename(
			String baseName,
			Boolean uniqueTest);
static void MessageToFileList(
			Tt_message msg,
			char ***file_list,
			int *file_count);
#ifdef SUN_PERF
static void MessagesToFileList(
			Tt_message *msg_list,
 			int msg_cnt,
			char ***file_list,
			int *file_count);
#endif /* SUN_PERF */
static int WriteEntry(
                        FILE *id,
                        String external,
                        String internal) ;
static Boolean MatchesSacredDirectory(
                        String file) ;
static void VerifyCleanup(
                        Widget mbox,
                        Boolean completeDelete) ;
static void VerifyCancel(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void VerifyOk(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static Boolean WriteTrashEntries( void ) ;
static void Select_All(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void Unselect_All(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
void CloseTrash(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static String GetBasePath(
                        String fullPath) ;
static Boolean ReadTrashList( void ) ;
static void RemoveOkCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static void RemoveCancelCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static void ConfirmOk(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static void ConfirmCancel(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static void Noop (
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
static void AddToDeleteList(
                        DeleteList *deleteList,
                        int i,
                        char *filename) ;
static void MoveToTrash(
			char **file_list,
			int file_count,
			Boolean do_verify_checks,
			Tt_message msg) ;
static void RestoreFromTrash(
			char **file_list,
			int file_count,
			char *target_host,
			char *target_dir,
			Tt_message msg,
			Boolean CheckedAlready) ;
static void EmptyTrash(
			DeleteList *del_list,
			int del_count,
			int removeType,
			Tt_message msg) ;
static int CheckDeletePermissionRecur(
                        char *dir);
static int FileSysType(int dev);
static void RestoreVerifyOk(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static void RestoreVerifyCancel(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data ) ;
static int RestoreObject(
                        Widget w,
                        int mode,
                        register char *source,
                        register char *target,
                        Boolean  isContainer,
                        void (*errorHandler)(),
                        Boolean checkForBusyDir,
                        int type ,
                        Boolean CheckedAlready);
static void CreateRestoreDialog(
                        char  *source,
                        char *target);

/********    End Static Function Declarations    ********/

Boolean
TrashIsInitialized( void )
{
  return( TrashInitialized );
}

Boolean
InitializeTrash( Boolean enableVerifyPrompt )
{
  char * ptr;
  struct stat statInfo;

  /* Build the 'TRASH' directory */
  trash_dir = XtMalloc(strlen(users_home_dir) +
                       strlen(TRASH_DIR) +
                       1);
  sprintf(trash_dir, "%s%s", users_home_dir, TRASH_DIR);
  if (stat(trash_dir, &statInfo) < 0)
    mkdir(trash_dir, S_IRUSR | S_IWUSR | S_IXUSR |
          S_IRGRP | S_IWGRP | S_IXGRP |
          S_IROTH | S_IWOTH | S_IXOTH);

  /* build path to .trashinfo file */
  TrashInfoFileName = XtMalloc(strlen(users_home_dir) + strlen(TRASH_INFO_FILE) + 1);
  sprintf(TrashInfoFileName, "%s%s", users_home_dir, TRASH_INFO_FILE);

  /* */
  NewTrashInfoFileName = XtMalloc(strlen(users_home_dir) +
                                  strlen(NEW_TRASH_INFO_FILE)
                                  + 1);
  sprintf(NewTrashInfoFileName, "%s%s", users_home_dir, NEW_TRASH_INFO_FILE);

  /* Keep track of whether to prompt for user verification */
  verifyPromptsEnabled =  enableVerifyPrompt;

  /*
   * Create an array of paths and filenames which we will not allow the
   * user to delete, because deleting any of them will cause trash to
   * stop working.
   */
  sacred_dir_list_size = 5;
  sacred_dir_list = (char **)XtMalloc(sizeof(char *) * sacred_dir_list_size);

  sacred_dir_list[sacred_dir_count++] = XtNewString("/");
  sacred_dir_list[sacred_dir_count++] = XtNewString(TrashInfoFileName);

  ptr = TrashInfoFileName + 1;
  while(ptr = DtStrchr(ptr, '/'))
  {
    /* All parent components of the user's home dir cannot be deleted */
    *ptr = '\0';
    if (sacred_dir_count >= sacred_dir_list_size)
    {
         /* Grow the list */
      sacred_dir_list_size += 5;
      sacred_dir_list = (char **)XtRealloc((char *)sacred_dir_list,
                                           sizeof(char *) * sacred_dir_list_size);
    }
    sacred_dir_list[sacred_dir_count++] = XtNewString(TrashInfoFileName);
    *ptr = '/';
    ptr++;
  }

  /* load and verify existence for files previously left in the trash can */
  TrashInitialized = ReadTrashList( );
  return( TrashInitialized );
}

/************************************************************************
 *
 * TrashCreateDialog
 *   This function must be called before any other interatctions with the
 *   trash can.  It creates the trash dialog, opens the trash file, and
 *   loads information about any files previously left in the trash can.
 *
 *   This function is called from main().
 *
 ************************************************************************/

void
TrashCreateDialog(
        Display *display )
{
   char * tmpStr;
   char * title;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;

   if( ! TrashInitialized )
     return;

   /* Create the trash dialog window */
   /* load trash title */
   tmpStr = GETMESSAGE(27, 9, "Trash Can");
   title = XtNewString(tmpStr);

   /* Initially, no items can be selected */
   if (removeBtn != NULL)
      XtSetSensitive(*removeBtn, False);
   if (restoreBtn != NULL)
      XtSetSensitive(*restoreBtn, False);

   /* retrieve the default data for a file manager dialog -- */
   /* the TrashView flag affects this data                   */
   TrashView = True;
   dialog_data = _DtGetDefaultDialogData (file_mgr_dialog);
   file_mgr_data = (FileMgrData *) dialog_data->data;
   trashFileMgrData = file_mgr_data;
   trashDialogData = dialog_data;

   /* build up directory set for trash directory */
   FileMgrBuildDirectories (file_mgr_data, home_host_name, trash_dir);

   /* initialize trash data */
   file_mgr_data->restricted_directory =
                          XtNewString(file_mgr_data->current_directory);
   file_mgr_data->title = XtNewString(title);
   file_mgr_data->toolbox = False;
   file_mgr_data->width = 300;
   file_mgr_data->height = 300;

   /* load any positioning information */
   LoadPositionInfo(file_mgr_data);

   /* build the trash dialog */
   _DtBuildDialog (NULL, NULL, NULL, dialog_data, NULL, NULL, NULL,
                NULL, NULL, False, special_view, title, NULL);
   TrashView = False;

   XtFree(title);
}



/************************************************************************
 *
 * ReadTrashList
 *   At startup time, we need to read in the trash information file, to
 *   find out what, if any, files were left in the trash the last time
 *   this client was run.  Each entry must be read in and then verified
 *   that it still exists; if it no longer exists, then someone has been
 *   mucking with our trash files, so we will remove it from our knowledge
 *   base.  Otherwise, the file is added to the trash list.
 *
 ************************************************************************/

static Boolean
ReadTrashList( void )
{
   int intSize, extSize, bufSize;
   FILE * trashInfoFileId;
   String external, intName, internal;
   char * trashEntry;
   struct stat statbuf;

   trashInfoFileId = fopen(TrashInfoFileName, "a+");
   if (trashInfoFileId == 0)
   {
     char * msg, * title;

     title = XtNewString(GetSharedMessage(TRASH_ERROR_TITLE));

     msg = XtMalloc( strlen( GETMESSAGE(27,93, "Unable to access this trash information file:\n   %s\n   All trash operations will not be performed.\n   The most common causes are:\n     - Network authentication.\n     - Insufficient disk space.\n     - Wrong permissions $HOME/.dt/Trash.") )
                     + strlen( TrashInfoFileName )
                     + 1 );

     sprintf( msg, GETMESSAGE(27, 93, "Unable to access this trash information file:\n   %s\n   All trash operations will not be performed.\n   The most common causes are:\n     - Network authentication.\n     - Insufficient disk space.\n     - Wrong permissions $HOME/.dt/Trash."), TrashInfoFileName );

     _DtMessage(toplevel, title, msg, NULL, HelpRequestCB);
     XtFree(title);
     XtFree(msg);
     return( False );
   }

   chmod(TrashInfoFileName, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

   bufSize = MAX_PATH * 2;
   trashEntry = (char *)XtCalloc( 1, bufSize );

   /* Extract the size of the two path strings */
   while( fgets( trashEntry, bufSize, trashInfoFileId ) != NULL )
   {
     int len = strlen( trashEntry );

     if( sscanf( trashEntry, "%d %d", &extSize, &intSize ) == 2 )
     {
       external = (char *)XtCalloc( 1, extSize + 1 );
       intName = (char *)XtCalloc( 1, intSize + 1 );

       if( len > extSize + intSize + 3 )
       /* extSize + intSize + 3 is a rough estimate of a trash entry
          a trash entry looks something like the following:

          8 4 /tmp/abc abcd

          extSize is the size of the string '/tmp/abc'
          intSize is the size of the string 'abcd'

          so the len (the string length of the trash entry has AT LEAST to be
          bigger than extSize + intSize + 3 separator spaces
       */
       {
         char * tmpPtr;

         /* sscanf can be used to reduce the code.
            I'm not using it here because I don't have time to research.
            I just want it to work.
         */
         tmpPtr = DtStrchr( trashEntry, ' ' ) + 1;
         tmpPtr = DtStrchr( tmpPtr, ' ' ) + 1;

         /* Extract the full external and partial internal file names */
         memcpy( external, tmpPtr, extSize );
         memcpy( intName, tmpPtr + extSize + 1, intSize );

         /* Create internal/trash name */
         internal = (char *)XtMalloc(strlen(users_home_dir)
                                     + strlen(TRASH_DIR)
                                     + strlen(intName) + 2);
         sprintf(internal, "%s%s/%s", users_home_dir, TRASH_DIR, intName);

         /* Make sure the file still exists */
         if (lstat(internal, &statbuf) < 0)
         {
           /* File no longer exists; ignore it */
           XtFree(internal);
           XtFree(external);
           XtFree(intName);
         }
         else
         {
           /* Add to trash list */
           if (numTrashItems >= trashListSize)
           {
             trashListSize += 10;
             trashCan = (TrashEntry *)XtRealloc((char *) trashCan,
                                                sizeof(TrashEntry) * trashListSize);
           }

           trashCan[numTrashItems].intNew = internal;
           trashCan[numTrashItems].intOrig = XtNewString(external);
           trashCan[numTrashItems].external = external;
           trashCan[numTrashItems].filename = intName;
           numTrashItems++;
         } /* end if file exists */
       }
       else
       {
         XtFree(external);
         XtFree(intName);
       }
     }
   } /* end while */
   fclose(trashInfoFileId);

   return( WriteTrashEntries() );
}



/************************************************************************
 *
 * WriteTrashEntries
 *   This function is responsible for creating a new copy of the trash
 *   information file.  It will open a new, temporary copy, and will
 *   copy in the contents of the trash can.  It will then remove the
 *   old trash information file, and will rename the new one, opening
 *   a handle to it for later use.
 *
 ************************************************************************/

static Boolean
WriteTrashEntries( void )
{
   static String path = NULL;
   FILE * newFile;
   int i;

   newFile = fopen(NewTrashInfoFileName, "w+");
   if (newFile)
   {
      chmod(NewTrashInfoFileName,
            S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

      /* Write all remaining entries */
      for (i = 0; i < numTrashItems; i++)
      {
         if( WriteEntry(newFile, trashCan[i].external, trashCan[i].filename) < 0 )
         {
           fclose(newFile);
           remove(NewTrashInfoFileName);
           return( False );
         }
      }

      /* Remove the original information file, and move the new one */
      (void) fclose(newFile);
      if(-1 == chown(NewTrashInfoFileName, getuid(), getgid())) {
	  return( False );      
      }
      (void) remove(TrashInfoFileName);
      (void) rename(NewTrashInfoFileName, TrashInfoFileName);
      if(-1 == chown(TrashInfoFileName, getuid(), getgid())) {
	  return( False );      
      }
      return( True );
   }
   else
     return( False );
}



/************************************************************************
 *
 * WriteEntry
 *   This function writes an entry to the trash information file.
 *   It expects the full path name for the original file [external]
 *   and only the new file name (without path information) [internal].
 *   If the file ends in '/', then it adds a '.' to the end; this is
 *   a special case, so that when we parse this information later, we
 *   are guaranteed to have a directory and a file name.
 *
 ************************************************************************/

static int
WriteEntry(
        FILE *id,
        String external,
        String internal )
{
  return( fprintf(id, "%ld %ld %s %s\n",
		  (long)strlen(external), (long)strlen(internal),
		  external, internal) );
}



/************************************************************************
 *
 * CreateTrashMenu
 *
 ************************************************************************/

Widget
CreateTrashMenu(
        Widget mainw,
        FileMgrRec *file_mgr_rec )
{
   int j, i;
   Widget menu_bar;
   Arg args[2];
   static Widget * directoryBarBtn;

   /* Create the menubar hierarchy */
   trashMenu = (MenuDesc *)XtMalloc(sizeof(MenuDesc) * trashMenuItemCount);
   j = 0;

   for (i = 0; i < trashMenuItemCount; i++)
   {
      trashMenu[i].helpCallback = TrashHelpRequestCB;
      trashMenu[i].helpData = NULL;
      trashMenu[i].activateCallback = NULL;
      trashMenu[i].activateData = NULL;
      trashMenu[i].maskBit = 0;
      trashMenu[i].isHelpBtn = False;
      trashMenu[i].label = NULL;
      trashMenu[i].mnemonic = NULL;
   }


   /*************************************/
   /* Create the 'File' menu components */
   /*************************************/

   directoryBarBtn = &(trashMenu[j].widget);
   trashMenu[j].type = MENU_PULLDOWN_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,1, "File");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,2, "F");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].name = "file_trash";
   trashMenu[j++].helpData = HELP_TRASH_DIALOG_STR;

   trashMenu[j].type = SHARED_MENU_PANE;
   trashMenu[j].name = "fileMenuTrash";
   trashMenu[j++].helpData = HELP_TRASH_DIALOG_STR;

   selectAllBtn = &(trashMenu[j].widget);
   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,48, "Select All");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,51, "S");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "selectAll";
   trashMenu[j++].activateCallback = Select_All;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,52, "Deselect All");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,55, "D");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "deselectAll";
   trashMenu[j++].activateCallback = Unselect_All;

   trashMenu[j].type = MENU_SEPARATOR;
   trashMenu[j].name = "separator";
   trashMenu[j++].helpCallback = NULL;

   restoreBtn = &(trashMenu[j].widget);
   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(27,24, "Put back");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(27,26, "P");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "putBack";
   trashMenu[j++].activateCallback = Restore;

   removeBtn = &(trashMenu[j].widget);
   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(27,28, "Shred");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(27,30, "h");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "shred";
   trashMenu[j++].activateCallback = ConfirmRemove;

   trashMenu[j].type = MENU_SEPARATOR;
   trashMenu[j].name = "separator";
   trashMenu[j++].helpCallback = NULL;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].maskBit = PREFERENCES;
   trashMenu[j].label = GETMESSAGE(20,141, "Set View Options ...");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,6, "V");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "setPreferences";
   trashMenu[j++].activateCallback = ShowPreferencesDialog;

   trashMenu[j].type = MENU_SEPARATOR;
   trashMenu[j].name = "separator";
   trashMenu[j++].helpCallback = NULL;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,117, "Close");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,118, "C");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "close";
   trashMenu[j++].activateCallback = CloseTrash;


   /*************************************/
   /* Create the 'Help' menu components */
   /*************************************/

   trashMenu[j].type = MENU_PULLDOWN_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,123, "Help");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,9, "H");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].isHelpBtn = True;
   trashMenu[j].name = "help_trash";
   trashMenu[j++].helpData = HELP_TRASH_DIALOG_STR;

   trashMenu[j].type = SHARED_MENU_PANE;
   trashMenu[j].name = "help_pane_trash";
   trashMenu[j++].helpData = HELP_TRASH_DIALOG_STR;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,105, "Overview");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,106, "v");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_HELP_MENU_STR;
   trashMenu[j].activateData = HELP_TRASH_OVERVIEW_TOPIC_STR;
   trashMenu[j].name = "introduction";
   trashMenu[j++].activateCallback = TrashHelpRequestCB;

   trashMenu[j].type = MENU_SEPARATOR;
   trashMenu[j].name = "separator";
   trashMenu[j++].helpCallback = NULL;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,107, "Tasks");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,108, "T");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_HELP_MENU_STR;
   trashMenu[j].activateData = HELP_TRASH_TASKS_TOPIC_STR;
   trashMenu[j].name = "tasks";
   trashMenu[j++].activateCallback = TrashHelpRequestCB;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,109, "Reference");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,110, "R");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_HELP_MENU_STR;
   trashMenu[j].activateData = HELP_TRASH_DIALOG_STR;
   trashMenu[j].name = "reference";
   trashMenu[j++].activateCallback = TrashHelpRequestCB;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,111, "On Item");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,112, "O");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_HELP_MENU_STR;
   trashMenu[j].activateData = HELP_HELP_MODE_STR;
   trashMenu[j].name = "onItem";
   trashMenu[j++].activateCallback = TrashHelpRequestCB;

   trashMenu[j].type = MENU_SEPARATOR;
   trashMenu[j].name = "separator";
   trashMenu[j++].helpCallback = NULL;

   usingHelpTrash = &(trashMenu[j].widget);
   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(20,113, "Using Help");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,114, "U");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_HELP_MENU_STR;
   trashMenu[j].activateData = HELP_HOME_TOPIC;
   trashMenu[j].name = "usingHelp";
   trashMenu[j++].activateCallback = TrashHelpRequestCB;

   trashMenu[j].type = MENU_SEPARATOR;
   trashMenu[j].name = "separator";
   trashMenu[j++].helpCallback = NULL;

   trashMenu[j].type = MENU_BUTTON;
   trashMenu[j].label = GETMESSAGE(27,106, "About Trash Can");
   trashMenu[j].label = XtNewString(trashMenu[j].label);
   trashMenu[j].mnemonic = GETMESSAGE(20,116, "A");
   trashMenu[j].mnemonic = XtNewString(trashMenu[j].mnemonic);
   trashMenu[j].helpData = HELP_HELP_MENU_STR;
   trashMenu[j].activateData = HELP_ABOUT_STR;
   trashMenu[j].name = "version";
   trashMenu[j++].activateCallback = TrashHelpRequestCB;

   menu_bar = _DtCreateMenuSystem(mainw, "menu_bar_trash", TrashHelpRequestCB,
                               HELP_TRASH_DIALOG_STR, True,
                               trashMenu, trashMenuItemCount, NULL,
                               Noop, Noop);

   /* Fine tune the menubar */
   XtSetArg(args[0], XmNmarginWidth, 2);
   XtSetArg(args[1], XmNmarginHeight, 2);
   XtSetValues(menu_bar, args, 2);

   file_mgr_rec->actions = NULL;
   file_mgr_rec->action_pane = NULL;
   file_mgr_rec->directoryBarBtn = *directoryBarBtn;

   return(menu_bar);
}



/************************************************************************
 *
 * TrashDisplayHandler
 *   This is the ICCCM message handler for the message used to display
 *   the trash can window.  If the window is already displayed, then
 *   this call is a no-op.
 *
 *   This handler is triggered by the front panel trash icon and by the
 *   File pulldown 'Show Trash' option.
 *
 ************************************************************************/

void
TrashDisplayHandler(
   Tt_message msg)
{
   Atom   current_ws;
   Window root;

   if (msg != NULL) {
      tt_message_reply( msg );
      tttk_message_destroy( msg );
   }

   if (!TrashInitialized)
   {
     char *tmpStr, *tmpTitle, *tmpMsg;

     tmpStr = GetSharedMessage(TRASH_ERROR_TITLE);
     tmpTitle = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(27, 90, "The 'Trash Can' cannot be initialized.\nPlease check:\n      1)$HOME/.dt/Trash for permissions.\n      2)Available disk space.\n      3)Authentication.");
     tmpMsg = XtNewString(tmpStr);

     _DtMessage(toplevel, tmpTitle, tmpMsg, NULL, HelpRequestCB);

     XtFree(tmpTitle);
     XtFree(tmpMsg);
     return;
   }

#ifdef DEFER_TRASH_CREATION
   if( trashFileMgrData == NULL )
     TrashCreateDialog (XtDisplay(toplevel));
#endif

   /* the encapsulation functions do not set file_mgr_rec until a dialog */
   /* is actually displayed                                              */
   if (trashFileMgrData->file_mgr_rec == 0)
   {
      static Pixmap trash_icon = XmUNSPECIFIED_PIXMAP;
      static Pixmap trash_mask = XmUNSPECIFIED_PIXMAP;

      Pixel background, foreground, top_shadow, bottom_shadow, select;
      Colormap colormap;
      XClassHint classHints;
      FileMgrRec * file_mgr_rec;
      unsigned int width;
      unsigned int height;
      Pixmap pixmap;
      Arg args[3];

      classHints.res_name = trashFileMgrData->title;
      classHints.res_class = DTFILE_CLASS_NAME;

      TrashView = True;
      _DtShowBuiltDialog(NULL,NULL, trashDialogData, NULL, False, &classHints);
      TrashView = False;

      file_mgr_rec = (FileMgrRec *)trashFileMgrData->file_mgr_rec;

      trashShell = file_mgr_rec->shell;

      if (trash_icon == XmUNSPECIFIED_PIXMAP)
      {
         XtSetArg (args[0], XmNbackground, &background);
         XtSetArg (args[1], XmNcolormap,  &colormap);
         XtGetValues (file_mgr_rec->main, args, 2);

         XmGetColors (XtScreen (file_mgr_rec->main), colormap, background,
                      &foreground, &top_shadow, &bottom_shadow, &select);

         pixmap = XmGetPixmap (XtScreen (file_mgr_rec->main), trashIcon,
                                        foreground, background);
         if( pixmap != XmUNSPECIFIED_PIXMAP)
            trash_icon = pixmap;

         /* now let's get the mask for the File Manager */
         pixmap = _DtGetMask (XtScreen (file_mgr_rec->main), trashIcon);
         if( pixmap != XmUNSPECIFIED_PIXMAP)
            trash_mask = pixmap;

         if (trash_icon != XmUNSPECIFIED_PIXMAP)
         {
            XtSetArg (args[0], XmNiconPixmap, trash_icon);
            if(trash_mask != XmUNSPECIFIED_PIXMAP)
            {
               XtSetArg (args[1], XmNiconMask, trash_mask);
               XtSetValues (trashShell, args, 2);
            }
            else
               XtSetValues (trashShell, args, 1);
         }
      }
   }
   else
   {
      XWMHints *wmhints;

      root = RootWindowOfScreen (XtScreen (trashShell));

      /*  Change the hints to reflect the current workspace  */
      /*  and raise the window                               */

      if (DtWsmGetCurrentWorkspace (XtDisplay (trashShell),
                                 root, &current_ws) == Success)
         DtWsmSetWorkspacesOccupied (XtDisplay (trashShell),
                              XtWindow (trashShell), &current_ws, 1);

      /* Set the iconify state */
      /* Remove the iconify hint from the current shell */
      wmhints = XGetWMHints(XtDisplay(trashShell), XtWindow(trashShell));
      wmhints->flags |= IconWindowHint;
      wmhints->initial_state = NormalState;
      XSetWMHints(XtDisplay(trashShell), XtWindow(trashShell), wmhints);
      XFree(wmhints);


      XtPopup (trashShell, XtGrabNone);
      XSync(XtDisplay(trashShell), False);
      XRaiseWindow (XtDisplay (trashShell), XtWindow (trashShell));
      XMapWindow( XtDisplay (trashShell), XtWindow (trashShell) );
      {
        Tt_message msg;
        msg = tt_pnotice_create(TT_SESSION, "DtActivity_Began");
        tt_message_send(msg);
        tttk_message_destroy(msg);
      }
   }

   trashDialogPosted = True;
}


/************************************************************************
 *
 *  ConfirmEmptyCancel
 *
 ************************************************************************/
static XtPointer trash_popup_client_data;
static void
ConfirmEmptyCancel(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XtUnmanageChild((Widget)client_data);
  XmUpdateDisplay((Widget)client_data);
  XtDestroyWidget((Widget)client_data);
}

/************************************************************************
 *
 *  ConfirmEmptyOk
 *
 ************************************************************************/

static void
ConfirmEmptyOk(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  /* destroy dialog */
  XtUnmanageChild((Widget)client_data);
  XmUpdateDisplay((Widget)client_data);
  XtDestroyWidget((Widget)client_data);

  TrashEmpty();
}

/************************************************************************
 *
 * TrashEmptyHandler
 *   This is a message interface used to really remove all of the
 *   trash files.
 *
 *   This handler is only called from an empty trash action.
 *
 ************************************************************************/

void
TrashEmptyHandler(
   Tt_message msg)
{
   if (msg != 0) {
     tt_message_reply( msg );
     tttk_message_destroy( msg );
   }

   if(!TrashInitialized)
   {
     char *tmpStr, *tmpTitle, *tmpMsg;

     tmpStr = GetSharedMessage(TRASH_ERROR_TITLE);
     tmpTitle = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(27, 90, "The 'Trash Can' cannot be initialized.\nPlease check:\n      1)$HOME/.dt/Trash for permissions.\n      2)Available disk space.\n      3)Authentication.");
     tmpMsg = XtNewString(tmpStr);

     _DtMessage(toplevel, tmpTitle, tmpMsg, NULL, HelpRequestCB);

     XtFree(tmpTitle);
     XtFree(tmpMsg);
     return;
   }

   {
      char *tmpStr, *title, *msg;

      tmpStr = GETMESSAGE(27,73, "Shred File(s)");
      title = XtNewString(tmpStr);
      tmpStr = GETMESSAGE(27,84, "When trash objects are shredded, they are permanently\nremoved from your system.\n\nAre you sure you want to shred?\n");
      msg = XtNewString(tmpStr);

      _DtMessageDialog(toplevel, title, msg, NULL, TRUE, ConfirmEmptyCancel,
              ConfirmEmptyOk, NULL, HelpRequestCB, False, QUESTION_DIALOG);

      XtFree(title);
      XtFree(msg);
   }
}



/************************************************************************
 *
 * TrashEmpty
 *   This is a functional interface used to really remove all of the
 *   trash files.
 *
 ************************************************************************/

void
TrashEmpty(void)
{
   if (trashFileMgrData)
   {
/*
      Select_All(*selectAllBtn, NULL, NULL);
      Remove(*removeBtn, NULL, NULL);
*/
      EmptyDir(trashFileMgrData->current_directory,0,0);
   }
}



/************************************************************************
 *
 * Select_All
 *   This is the callback attached to the 'Select All' menu item.  It
 *   will select all of the files currently resident in the trash can.
 *   It will also sensitize the 'Restore' and 'Remove' buttons, as long
 *   as a remove request is not currently in the middle of being processed.
 *
 ************************************************************************/

static void
Select_All(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   XmUpdateDisplay(w);
   SelectAllFiles(trashFileMgrData);
   SensitizeTrashBtns();
}



/************************************************************************
 *
 * Unselect_All
 * This is the callback attached to the 'Unselect All' menu item.  It
 * will unselect all of the items in the trash can, and will clear
 * out our selection array.  It also desensitizes the 'Restore' and
 * 'Remove' buttons.
 *
 ************************************************************************/

static void
Unselect_All(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   XmUpdateDisplay(w);
   DeselectAllFiles(trashFileMgrData);
   SensitizeTrashBtns();
}



/************************************************************************
 *
 * SensitizeTrashBtns
 *
 ************************************************************************/

void
SensitizeTrashBtns(void)
{
#ifdef DEFER_TRASH_CREATION
  if( trashFileMgrData
      && removeBtn
      && restoreBtn )
#endif
  {
    if (trashFileMgrData->selected_file_count > 0)
    {
      XtSetSensitive(*removeBtn, True);
      XtSetSensitive(*restoreBtn, True);
    }
    else
    {
      XtSetSensitive(*removeBtn, False);
      XtSetSensitive(*restoreBtn, False);
    }
  }
}



/************************************************************************
 *
 * TrashRemoveHandler
 *   This is the message handler for the message requesting that
 *   a set of files be added to the trashcan.  Each file name comes in
 *   in /dir/name format; they are all in a single string, separated
 *   by spaces.
 *
 *   Check for spaces in filenames and destop files.  If we pass these
 *   cases, call TrashRemover where the following occurs:
 *
 *   All of the files will be moved to the trash can directory; any
 *   problems encountered will be reported in an error dialog.  Each file
 *   which is successfully moved, will be added to the trash information file.
 *
 ************************************************************************/

void
TrashRemoveHandler(
   Tt_message msg)
{
   char *ptr;
   char *str;
   int number;
   char **file_list;
   int file_count;
#ifdef SUN_PERF
   int time_out = 0 ;
   int msglistsize = 0 ;
#endif /* SUN_PERF */

   /*
    * Note: There used to be a call to tt_message_reply() right here at the
    *  beginning of the routine.  However, the mailer wants to trash files
    *  but wants the notification after the trash has been removed.
    *  Therefore, we now put this off until the work is done or an error
    *  is detected.
    */

   if( !TrashInitialized )
   {
     char *tmpStr, *tmpTitle, *tmpMsg;

     tmpStr = GetSharedMessage(TRASH_ERROR_TITLE);
     tmpTitle = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(27, 90, "The 'Trash Can' cannot be initialized.\nPlease check:\n      1)$HOME/.dt/Trash for permissions.\n      2)Available disk space.\n      3)Authentication.");
     tmpMsg = XtNewString(tmpStr);

     _DtMessage(toplevel, tmpTitle, tmpMsg, NULL, HelpRequestCB);

     XtFree(tmpTitle);
     XtFree(tmpMsg);

     tttk_message_fail( msg, TT_DESKTOP_ENOSPC, 0, 1 );
#ifdef SUN_PERF
     global_msg_cnt = 0 ;
#endif /* SUN_PERF */
     return;
   }
#ifndef SUN_PERF
   MessageToFileList(msg, &file_list, &file_count);
#else
   /*
    * When the Dttrash_Remove handler "TrashRemoveHandler" is invoked for
    * the first time it caches the TT request and then invokes
    * tttk_block_while() with timeout = 0. This in effect causes the next
    * TT request to be processed before tttk_block_while() returns. Now if
    * TT msgs are coming in faster than dtfile is handling them, then the
    * next TT msg. should be another Dttrash_Remove msg. which will cause
    * re-entrance into TrashRemoveHandler. This will continue until the last
    * Dttrash_Remove msg. has been processed (and the recursive
    * TrashRemoveHandler starts unwinding) after which we check if the
    * number of TT msgs are > 0 in which case we fire off the
    * process to move the list of files to Trash.
    */
   global_msg_cnt++ ;
   msglistsize =  sizeof(Tt_message) * global_msg_cnt ;
   global_msg_list = (Tt_message *)XtRealloc((char *)global_msg_list, msglistsize);
   memcpy((char *)&(global_msg_list[global_msg_cnt - 1]),
			(char *)&msg, sizeof(Tt_message)) ;
   tttk_block_while(0, 0, time_out) ;

   /* If we get to this pt. then either :
    * 1. No more Dttrash_Remove msgs. left to process. Thus we should
    * process the TT msg list  and move files to Trash.
    * OR
    * 2. We got a msg. different from Dttrash_Remove. In this case
    * we process the msgs. we have so far. The remaining files to
    * be trashed will be buffered in another list.
    */

   if (global_msg_cnt > 0) {
   MessagesToFileList(global_msg_list, global_msg_cnt,
		&file_list, &file_count);
#endif /* SUN_PERF */

   if (file_count == 0)
   {
      tt_message_reply( msg );
      tttk_message_destroy( msg );
#ifdef SUN_PERF
      global_msg_cnt = 0 ;
#endif /* SUN_PERF */
      return;
   }

   /* return if from the trash dir */
   if (strncmp(file_list[0], trash_dir, strlen(trash_dir)) == 0)
   {
      int i;

      for (i = 0; i < file_count; i++) {
	 XtFree( file_list[i] );
      }
      XtFree( (char *)file_list );
      tt_message_status_set( msg, TT_DESKTOP_EALREADY );
      tt_message_reply( msg );
      tttk_message_destroy( msg );
#ifdef SUN_PERF
      global_msg_cnt = 0 ;
#endif /* SUN_PERF */
      return;
   }

   /* post message if file(s) from desktop */
   {
      Boolean IsToolBox;

      str=(char *)IsAFileOnDesktop2(file_list, file_count, &number,&IsToolBox);
      /* IsToolBox is unused here, but is required to satisfy the prototype */
   }
   if (number == 0)
   {
      MoveToTrash(file_list, file_count, verifyPromptsEnabled, msg);
   }
   else
   {
       char * tmpStr;
       char *title;
       char * template;
       char *message;
       int i;

       /* List will be regenerated by callbacks */
       for (i = 0; i < file_count; i++) {
	  XtFree( file_list[i] );
       }
       XtFree( (char *)file_list );
        file_list  = NULL;
       tmpStr = (GETMESSAGE(27,73, "Shred File(s)"));
       title = XtNewString(tmpStr);
       if (number == 1)
       {
          if (widget_dragged)
             template = (GETMESSAGE(27,85, "The Workspace object you are deleting is a linked copy of\n   an object inside the File Manager at\n   %s\n   If you click OK, both the linked copy and the original object\n   will be deleted.\n   If you just want to delete the Workspace object,\n      1) click Cancel on this dialog\n      2) then select 'Remove From Workspace'\n         from the Workspace object's popup menu."));
          else
             template = (GETMESSAGE(27,102, "A linked copy of the following object is on\nthe Workspace backdrop:\n%s\nMoving the object to the trash will remove it from\nboth the File Manager and Workspace backdrop."));
          message = (char *)XtMalloc(strlen(template) + strlen(str) + 1);
          sprintf(message, template, str);
       }
       else
       {
          if (widget_dragged)
             template = (GETMESSAGE(27,86, "The Workspace objects you are deleting are linked copies of\n   objects inside the File Manager at\n%s\n   If you click OK, both the linked copies and the original objects\n   will be deleted.\n   If you just want to delete the Workspace objects,\n      1) click Cancel on this dialog\n      2) then select 'Remove From Workspace'\n         from the Workspace objects' popup menu."));
          else
             template = (GETMESSAGE(27,103, "A linked copy of the following objects are also on\nthe Workspace backdrop:\n%s\nMoving the objects to the trash will remove them from\nboth the File Manager and Workspace backdrop."));
          message = (char *)XtMalloc(strlen(template) + strlen(str) + 1);
          sprintf(message, template, str);
       }

       /* Really ought to pass to OK and Cancel CBs via client_data */
#ifndef SUN_PERF
       global = msg;
       	_DtMessageDialog(toplevel, title, message, NULL, TRUE,
               	RemoveCancelCB, RemoveOkCB, NULL, HelpRequestCB, False,
               	QUESTION_DIALOG);
#else
	{
		Widget dlog ;
		Arg   args[2] ;
		Tt_msg_cache *msg_cache ;

		msg_cache = (Tt_msg_cache *)XtMalloc(sizeof(Tt_msg_cache)) ;
		msg_cache->msg_cnt = global_msg_cnt ;

		msg_cache->msg_list = (Tt_message *)XtMalloc(sizeof(Tt_message) *
					msg_cache->msg_cnt );
		msg_cache->msg_list[msg_cache->msg_cnt] = NULL ;
		memcpy((char *)msg_cache->msg_list, (char *)global_msg_list,
				sizeof(Tt_message) * msg_cache->msg_cnt ) ;
		dlog = 	(Widget)_DtMessageDialog(toplevel, title, message, NULL, TRUE,
                 	RemoveCancelCB, RemoveOkCB, NULL, HelpRequestCB, False,
                 	QUESTION_DIALOG);
		XtSetArg ( args[0], XmNuserData, msg_cache );
		XtSetValues ( dlog, args, 1);
	}
#endif /* SUN_PERF */

       		XtFree (message);
       		XtFree (title);
       		XtFree (str);
   }
#ifdef SUN_PERF
  global_msg_cnt = 0 ;
  }
#endif /* SUN_PERF */
}



/************************************************************************
 *
 *  RemoveOkCB
 *      Cleanup and unmanage the remove from DT dialog
 *
 ************************************************************************/

static void
RemoveOkCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   char **file_list;
   int file_count;
#ifdef SUN_PERF
   Arg args[1] ;
   Tt_msg_cache *current_msg_cache ;
   int i = 0 ;

   XtSetArg( args[0], XmNuserData, &current_msg_cache );
   XtGetValues( (Widget)client_data, args, 1 );
#endif /* SUN_PERF */

   /* destroy dialog */
   XtUnmanageChild((Widget)client_data);
   XmUpdateDisplay((Widget)client_data);
   XtDestroyWidget((Widget)client_data);

   /* remove files */
#ifndef SUN_PERF
   MessageToFileList(global, &file_list, &file_count);
#else
   MessagesToFileList( current_msg_cache->msg_list, current_msg_cache->msg_cnt,
				&file_list, &file_count);
#endif /* SUN_PERF */

   MoveToTrash(file_list, file_count, verifyPromptsEnabled, NULL);

   /* reread desktop files */
   CheckDesktop();

#ifndef SUN_PERF
   tt_message_reply( global );
   tttk_message_destroy( global );
   global = 0;
#else
   for (i = 0 ; i < current_msg_cache->msg_cnt ; i++) {
    	tt_message_reply( current_msg_cache->msg_list[i] );
   	tttk_message_destroy( current_msg_cache->msg_list[i] );
   }
   XtFree ((char *)current_msg_cache->msg_list);
   XtFree ((char *)current_msg_cache);
#endif /* SUN_PERF */
}



/************************************************************************
 *
 *  RemoveCancelCB
 *      Cleanup and unmanage the remove from DT dialog
 *
 ************************************************************************/

static void
RemoveCancelCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
#ifdef SUN_PERF
   Arg args[1] ;
   Tt_msg_cache *current_msg_cache ;
   int i = 0 ;

   XtSetArg( args[0], XmNuserData, &current_msg_cache );
   XtGetValues( (Widget)client_data, args, 1 );
#endif /* SUN_PERF */

    /* destroy dialog */
    XtUnmanageChild((Widget)client_data);
    XmUpdateDisplay((Widget)client_data);
    XtDestroyWidget((Widget)client_data);

#ifndef SUN_PERF
    tttk_message_fail( global, TT_DESKTOP_ECANCELED, 0, 1 );
    global = 0;
#else
   for (i = 0 ; i < current_msg_cache->msg_cnt ; i++)
    	tttk_message_fail( current_msg_cache->msg_list[i], TT_DESKTOP_ECANCELED, 0, 1 );
   XtFree ((char *)current_msg_cache->msg_list);
   XtFree ((char *)current_msg_cache);
#endif /* SUN_PERF */
}



/************************************************************************
 *
 * TrashRemoveNoConfirmHandler
 *   This function is a message handler.  It will place the specified set
 *   of files into the trash can, without requesting confirmation if, for
 *   instance, a file does not have write permission, or a directory is
 *   not empty.
 *
 ************************************************************************/

void
TrashRemoveNoConfirmHandler(
   Tt_message msg)
{
   char **file_list;
   int file_count;

   if( !TrashInitialized )
   {
     char *tmpStr, *tmpTitle, *tmpMsg;

     tmpStr = GetSharedMessage(TRASH_ERROR_TITLE);
     tmpTitle = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(27, 90, "The 'Trash Can' cannot be initialized.\nPlease check:\n      1)$HOME/.dt/Trash for permissions.\n      2)Available disk space.\n      3)Authentication.");
     tmpMsg = XtNewString(tmpStr);

     _DtMessage(toplevel, tmpTitle, tmpMsg, NULL, HelpRequestCB);

     XtFree(tmpTitle);
     XtFree(tmpMsg);

     if (msg != 0)
       tttk_message_fail( msg, TT_DESKTOP_ENOSPC, 0, 1 );
     return;
   }

   MessageToFileList(msg, &file_list, &file_count);
   MoveToTrash(file_list, file_count, False, msg);
}



/************************************************************************
 *
 * MessageToFileList
 *
 *   Parses a list of file names from a message
 *   and returns an array of strings.
 *
 *   if (no message arguments) {
 *      if (no file attribute) {
 *         return length==0 list
 *      } else {
 *         return length==1 list containing file attribute
 *      }
 *   } else {
 *      if (file attribute) {
 *         file attribute is a directory, prepended onto subsequent filenames
 *      }
 *      if (arg 0 is non-null) {
 *         arg 0 interpreted as a space-separated list of filenames
 *      }
 *      for (args > 0) {
 *         arg interpreted as a filename possibly containing spaces
 *      }
 *   }
 *
 ************************************************************************/

static void
MessageToFileList(
        Tt_message msg,
	char ***file_list,
	int *file_count)
{
   char *dir;
   int args;
   int arg;
   int dirlen;

   *file_count = 0;
   *file_list = 0;

   args = tt_message_args_count( msg );
   if (tt_is_err(tt_int_error( args ))) {
	   args = 0;
   }

   dir = tt_message_file( msg );
   if ((tt_is_err(tt_ptr_error( dir ))) || (dir == 0)) {
	   dir = "";
   } else {
     if (args <= 0) {
        /* No args means use tt_message_file() as is */
       *file_list = (char **)
         XtRealloc((char *)(*file_list),
                   (*file_count + 1)*sizeof(char *));
       (*file_list)[*file_count] = XtNewString(dir);
       (*file_count)++;
       tt_free(dir);
       return;
     }
     if (dir[strlen(dir)] != '/') {
       char *newdir;
       if (strcmp(dir, "/") == 0)
       {
         newdir = (char *)tt_malloc( strlen(dir)+1 );
         strcpy( newdir, dir );
       }
       else
       {
         newdir = (char *)tt_malloc( strlen(dir)+2 );
         strcpy( newdir, dir );
         strcat( newdir, "/" );
       }
       tt_free( dir );
       dir = newdir;
     }
   }
   /* Invariant: dir can now be safely concatenated to form a valid path */
   dirlen = strlen(dir);
   for (arg = 0; arg < args; arg++) {
     char *val;
     char *val2free;
     char *white;
     String file;
     val = tt_message_arg_val( msg, arg );
     if ((tt_is_err(tt_ptr_error( val ))) || (val == 0)) {
       continue;
     }
     val2free = val;
     white = "";
     if (arg == 0) {
       /* Arg 0 is a space-separated list */
       white = " ";
     }
     for (val = DtStrtok(val, white); val; val = DtStrtok(0, white))
     {
       if (strcmp(val, "/") == 0)
       {
         file = (char *)XtMalloc( dirlen + 1 );
         strcpy( file, dir );
       }
       else
       {
         file = (char *)XtMalloc( dirlen + strlen(val) + 1 );
         strcpy( file, dir );
         strcat( file, val );
       }
       *file_list = (char **)
         XtRealloc((char *)(*file_list),
                   (*file_count + 1)*sizeof(char *));
       (*file_list)[*file_count] = DtEliminateDots(file);
       (*file_count)++;
     }
     tt_free( val2free );
   }
   tt_free( dir );
}

#ifdef SUN_PERF
static void
MessagesToFileList(
        Tt_message *msg_list,
        int msg_cnt,
	char ***file_list,
	int *file_count)
{
   char *dir;
   int args;
   int arg;
   int dirlen;
   int num_msgs = 0 ;
   Tt_message msg;

   *file_count = 0;
   *file_list = 0;

 for (num_msgs = 0 ; num_msgs < msg_cnt ; num_msgs++)
 {
   msg = msg_list[num_msgs] ;

   args = tt_message_args_count( msg );
   if (tt_is_err(tt_int_error( args ))) {
	   args = 0;
   }

   dir = tt_message_file( msg );
   if ((tt_is_err(tt_ptr_error( dir ))) || (dir == 0)) {
	   dir = "";
   } else {
     if (args <= 0) {
        /* No args means use tt_message_file() as is */
       *file_list = (char **)
         XtRealloc((char *)(*file_list),
                   (*file_count + 1)*sizeof(char *));
       (*file_list)[*file_count] = XtNewString(dir);
       (*file_count)++;
       tt_free(dir);
       return;
     }
     if (dir[strlen(dir)] != '/') {
       char *newdir;
       if (strcmp(dir, "/") == 0)
       {
         newdir = (char *)tt_malloc( strlen(dir)+1 );
         strcpy( newdir, dir );
       }
       else
       {
         newdir = (char *)tt_malloc( strlen(dir)+2 );
         strcpy( newdir, dir );
         strcat( newdir, "/" );
       }
       tt_free( dir );
       dir = newdir;
     }
   }
   /* Invariant: dir can now be safely concatenated to form a valid path */
   dirlen = strlen(dir);
   for (arg = 0; arg < args; arg++) {
     char *val;
     char *val2free;
     char *white;
     String file;
     val = tt_message_arg_val( msg, arg );
     if ((tt_is_err(tt_ptr_error( val ))) || (val == 0)) {
       continue;
     }
     val2free = val;
     white = "";
     if (arg == 0) {
       /* Arg 0 is a space-separated list */
       white = " ";
     }
     for (val = DtStrtok(val, white); val; val = DtStrtok(0, white))
     {
       if (strcmp(val, "/") == 0)
       {
         file = (char *)XtMalloc( dirlen + 1 );
         strcpy( file, dir );
       }
       else if (strcmp(val, "/") == 0 && dirlen == 0)
       {
         file = (char *)XtMalloc( strlen(val) + 1 );
         strcpy( file, val );
       }
       else
       {
         file = (char *)XtMalloc( dirlen + strlen(val) + 1 );
         strcpy( file, dir );
         strcat( file, val );
       }
       *file_list = (char **)
         XtRealloc((char *)(*file_list),
                   (*file_count + 1)*sizeof(char *));
       (*file_list)[*file_count] = DtEliminateDots(file);
       (*file_count)++;
     }
     tt_free( val2free );
   }
   tt_free( dir );
 }
}
#endif /* SUN_PERF */


/************************************************************************
 *
 * CreateTrashFilename
 *   Create trash directory name.
 *
 ************************************************************************/

static String
CreateTrashFilename(
        String baseName,
        Boolean uniqueTest )
{
   String trashName;
   char * extension = NULL;
   struct stat statInfo;

   /* Create trash path */
   /* Give that name a little extra cushion, just in case */
   trashName = (char *)XtMalloc(strlen(users_home_dir) + strlen(TRASH_DIR) +
                                                         strlen(baseName) + 15);
   sprintf(trashName, "%s%s/%s", users_home_dir, TRASH_DIR, baseName);

   /* Want to find the extension so the new file name created will preserve
      its original datatype.
   */
   extension = strrchr( baseName, '.' );
   if( extension )
   {
     *extension = 0x0;
     ++extension;
   }

   /* If a file by the trash name already exists, keep building new names */
   /* until one by that name doesn't exist and then use it                */
   if (uniqueTest && (lstat(trashName, &statInfo) == 0))
   {
      int i;

      for (i = 1; True ; i++)
      {
         /* Make a duplicate file name */
         if( extension )
            sprintf(trashName, "%s%s/%s_%d.%s", users_home_dir, TRASH_DIR,
                    baseName, i, extension);
         else
            sprintf(trashName, "%s%s/%s_%d", users_home_dir, TRASH_DIR,
                    baseName, i);

         /* Is the filename taken? */
         if (lstat(trashName, &statInfo) != 0)
            break;   /* Nope */
      }

   } /* end if not unique name */

   if( extension )
     *(extension-1) = '.';

   return(trashName);

}



/************************************************************************
 *
 * MatchesSacredDirectory
 *   Checks if the specify filename matches one of the sacred,
 *   non-deleteable files.
 *
 ************************************************************************/

static Boolean
MatchesSacredDirectory(
        String file )
{
   int i;

   /* Don't allow the user to delete any of the sacred directories */
   for (i = 0; i < sacred_dir_count; i++)
   {
      if (!strcmp(file, sacred_dir_list[i]))
         return(True);
   }

   /* Compare against special desktop directories */
   /* remote_sys_dir is currently NULL */
   if (strcmp(file, remote_sys_dir) == 0)
      return(True);

   return(False);
}



/************************************************************************
 *
 * VerifyOk
 *   Callback for 'Delete To Trash' verify dialog Ok push button.
 *
 ************************************************************************/

static void
VerifyOk(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   VerifyCleanup ((Widget)client_data, True);
}



/************************************************************************
 *
 * VerifyCancel
 *   Callback for 'Delete To Trash' verify dialog Cancel push button.
 *
 ************************************************************************/

static void
VerifyCancel(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )

{
   VerifyCleanup ((Widget)client_data, False);
}



/************************************************************************
 *
 * VerifyCleanup
 *   Callback for 'Delete To Trash' verify dialog push buttons.
 *
 *   Called from VerifyOk and VerifyCancel.
 *
 ************************************************************************/

static void
VerifyCleanup(
        Widget mbox,
        Boolean completeDelete )
{
   Arg args[2];
   char ** verifylist;
   int fileCount;
   int i;

   /* Unpost dialog, and retrieve the list of verified files */
   XtUnmanageChild(mbox);
   XmUpdateDisplay(mbox);
   XtSetArg(args[0], XmNuserData, &verifylist);
   XtGetValues(mbox, args, 1);

   /* count the number of files in the list */
   for (fileCount = 0; verifylist[fileCount]; fileCount++)
      ;

   /*
    * Start the background process that moves the files into the trash,
    * if so instructed
    */
   if (completeDelete)
      MoveToTrash(verifylist, fileCount, False, NULL);

   else
   {
      /* Free up the storage we allocated */
      for (i = fileCount; i > 0; i--)
         XtFree(verifylist[i]);
      XtFree((char *)verifylist);
   }

   XtDestroyWidget(mbox);
}



/************************************************************************
 *
 *  ConfirmCancel
 *
 ************************************************************************/
static XtPointer trash_popup_client_data;
static void
ConfirmCancel(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XtUnmanageChild((Widget)client_data);
  XmUpdateDisplay((Widget)client_data);
  XtDestroyWidget((Widget)client_data);

  XtSetSensitive(*removeBtn, True);
  XtSetSensitive(*restoreBtn, True);
  XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_REMOVE], True);
  XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_RESTORE], True);
  trash_popup_client_data = NULL;
}



/************************************************************************
 *
 *  ConfirmOk
 *
 ************************************************************************/

static void
ConfirmOk(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  FileViewData *file_view_data;

  /* destroy dialog */
  XtUnmanageChild((Widget)client_data);
  XmUpdateDisplay((Widget)client_data);
  XtDestroyWidget((Widget)client_data);

  file_view_data = trashFileMgrData->popup_menu_icon;
  if(!file_view_data && trashFileMgrData->selected_file_count)
      file_view_data = trashFileMgrData->selection_list[0];
  trashFileMgrData->popup_menu_icon = NULL;

  if(file_view_data != NULL)
      Remove(*removeBtn, file_view_data, NULL);
}



/************************************************************************
 *
 * ConfirmRemove
 *   This is the callback attached to the 'Remove' menu item.
 *
 ************************************************************************/

void
ConfirmRemove(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  char *tmpStr, *title, *msg;

  /* Desensitize remove & restore buttons, until we're done */
  XtSetSensitive(*removeBtn, False);
  XtSetSensitive(*restoreBtn, False);
  XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_REMOVE], False);
  XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_RESTORE], False);
  XmUpdateDisplay(w);

  trash_popup_client_data = client_data;

  tmpStr = GETMESSAGE(27,73, "Shred File(s)");
  title = XtNewString(tmpStr);
  tmpStr = GETMESSAGE(27,84, "When trash objects are shredded, they are permanently\nremoved from your system.\n\nAre you sure you want to shred?\n");
  msg = XtNewString(tmpStr);

  _DtMessageDialog(toplevel, title, msg, NULL, TRUE, ConfirmCancel, ConfirmOk,
                   NULL, HelpRequestCB, False, QUESTION_DIALOG);

  XtFree(title);
  XtFree(msg);
}



/************************************************************************
 *
 * Remove
 *   While we wait for the background process to complete, we will
 *   desensitize the 'Remove' and 'Restore' menu items, since we don't
 *   handle multiple requests.
 *
 ************************************************************************/

void
Remove(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileViewData *file_view_data;
   Boolean match = False;
   DeleteList *deleteList = NULL;
   int deleteCount;
   int removeType;
   int i;


   /* Remove may be called to remove a file or to remove a trash file;  */
   /* if w is NULL, this is a call to remove a file */
   if (w != NULL)
   {
      removeType = TRASH_FILE;

      /* Count number of items to 'Delete To Trash' */
      if (client_data != NULL)
      {
         /* This was called by trash popup */
         /* need to check to see if it is selected, if it is restore all
            the selected files, else just restore this file */

         file_view_data = (FileViewData *)client_data;
         for (i = 0;  i < trashFileMgrData->selected_file_count; i++)
         {
            if (strcmp(file_view_data->file_data->file_name,
              trashFileMgrData->selection_list[i]->file_data->file_name) == 0)
            {
               match = True;
            }
         }
      }
      else
      {
         if (trashFileMgrData->selected_file_count == 0)
         {
           XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_REMOVE], True);
           XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_RESTORE], True);
           return;
         }

         match = True;
      }

      /* Desensitize remove & restore buttons, until we're done */
      XtSetSensitive(*removeBtn, False);
      XtSetSensitive(*restoreBtn, False);
      XmUpdateDisplay(w);

      if (match)
         deleteCount = trashFileMgrData->selected_file_count;
      else
         deleteCount = 1;

      /* Create the list of things being deleted */
      deleteList = (DeleteList *)XtMalloc(deleteCount * sizeof(DeleteList));

      if (match)
      {
         for (i = 0; i < deleteCount; i++)
         {
            AddToDeleteList(deleteList, i,
                   trashFileMgrData->selection_list[i]->file_data->file_name);
         }
      }
      else
         AddToDeleteList(deleteList, 0, file_view_data->file_data->file_name);

   }
   else
   {
      removeType = REMOVE_FILE;
      deleteList = (DeleteList *)XtMalloc(sizeof(DeleteList));
      deleteCount = 1;
      deleteList[0].trash = XtNewString(client_data);
      deleteList[0].orig = NULL;
   }

   EmptyTrash(deleteList, deleteCount, removeType, NULL);
}



/************************************************************************
 *
 * AddToDeleteList
 *   Locate a file in the trash list and add it to the delete list.
 *
 ************************************************************************/

static void
AddToDeleteList(
   DeleteList *deleteList,
   int i,
   char *filename)
{
   int j;

   /* Locate file in trash list, add entry to delete list */
   for (j = 0; j < numTrashItems; j++)
   {
      if (strcmp(filename, trashCan[j].filename) == 0)
      {
         /* file found in trash list */
         deleteList[i].trash = XtNewString(trashCan[j].intNew);
         deleteList[i].orig = XtNewString(trashCan[j].intOrig);
         return;
         break;
      }
   }

   /* file not found in trash list */
   deleteList[i].trash = CreateTrashFilename(filename, FALSE);
   deleteList[i].orig = NULL;
}



/************************************************************************
 *
 * TrashRestoreHandler
 *   This function is a message handler.  It will restore the specified set
 *   of files from the trash can.
 *
 ************************************************************************/

void
TrashRestoreHandler(
   Tt_message msg)
{
   char **file_list;
   int file_count;

   if( !TrashInitialized )
   {
     char *tmpStr, *tmpTitle, *tmpMsg;

     tmpStr = GetSharedMessage(TRASH_ERROR_TITLE);
     tmpTitle = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(27, 90, "The 'Trash Can' cannot be initialized.\nPlease check:\n      1)$HOME/.dt/Trash for permissions.\n      2)Available disk space.\n      3)Authentication.");
     tmpMsg = XtNewString(tmpStr);

     _DtMessage(toplevel, tmpTitle, tmpMsg, NULL, HelpRequestCB);

     XtFree(tmpTitle);
     XtFree(tmpMsg);

     if (msg != 0) {
       tttk_message_fail( msg, TT_DESKTOP_ENOSPC, 0, 1 );
     }
     return;
   }

   /* Desensitize remove & restore buttons, until we're done */
   XtSetSensitive(*removeBtn, False);
   XtSetSensitive(*restoreBtn, False);

   MessageToFileList(msg, &file_list, &file_count);
   RestoreFromTrash(file_list, file_count, NULL, NULL, msg,False);
}



/************************************************************************
 *
 * Restore
 *   This is the callback attached to the 'Restore' menu item.  It will
 *   remove from the trash all of the selected files, and will restore
 *   them to their original location.  If any problems occur while trying
 *   to restore any of the files, then an error dialog will be posted.
 *
 ************************************************************************/

void
Restore(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileViewData *file_view_data;
   Boolean match = False;
   char **file_list;
   int file_count;
   int i;

   if (client_data != NULL)
   {
      /* This was called by trash popup */
      /* need to check to see if it is selected, if it is restore all
         the selected files, else just restore this file */

       file_view_data = trashFileMgrData->popup_menu_icon;

       /* The object would have been deleted if the following condn is true */
       if(!file_view_data && !trashFileMgrData->selected_file_count)
           return;
       trashFileMgrData->popup_menu_icon = NULL;

      for (i = 0;  i < trashFileMgrData->selected_file_count; i++)
      {
         if (strcmp(file_view_data->file_data->file_name,
              trashFileMgrData->selection_list[i]->file_data->file_name) == 0)
         {
            match = True;
         }
      }
   }
   else
      match = True;

   /* Desensitize remove & restore buttons, until we're done */
   XtSetSensitive(*removeBtn, False);
   XtSetSensitive(*restoreBtn, False);
   XmUpdateDisplay(w);

   if (match)
   {
      file_count = trashFileMgrData->selected_file_count;
      file_list = (char **)XtMalloc(file_count * sizeof(char *));
      for (i = 0; i < file_count; i++)
      {
         file_view_data = trashFileMgrData->selection_list[file_count - 1 - i];
         file_list[i] = XtNewString(file_view_data->file_data->file_name);
      }
   }
   else
   {
      file_count = 1;
      file_list = (char **)XtMalloc(sizeof(char *));
      file_list[0] = XtNewString(file_view_data->file_data->file_name);
   }

   /* Start the background process that will do the restore */
   RestoreFromTrash(file_list, file_count, NULL, NULL, NULL,False);
}



/************************************************************************
 *
 * CloseTrash
 * This is the callback attached to the 'Close' menu item.  It will
 * unpost the trash can window.
 *
 ************************************************************************/

void
CloseTrash(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   int i;

   if (trashDialogPosted)
   {
      XWithdrawWindow(XtDisplay(trashShell), XtWindow(trashShell),
                            XDefaultScreen(XtDisplay(trashShell)));
      XtPopdown(trashShell);
   }


   for (i = 0; i < secondaryTrashHelpDialogCount; i++)
   {
      if (_DtIsDialogShowing(secondaryTrashHelpDialogList[i]))
         _DtHideDialog(secondaryTrashHelpDialogList[i], False);
      _DtFreeDialogData(secondaryTrashHelpDialogList[i]);
   }
   XtFree((char *)secondaryTrashHelpDialogList);
   secondaryTrashHelpDialogList = NULL;
   secondaryTrashHelpDialogCount = 0;

   if (primaryTrashHelpDialog)
   {
      if (_DtIsDialogShowing(primaryTrashHelpDialog))
         _DtHideDialog(primaryTrashHelpDialog, False);
      _DtFreeDialogData(primaryTrashHelpDialog);
   }
   primaryTrashHelpDialog = NULL;

   if (PositionFlagSet(trashFileMgrData))
       SavePositionInfo(trashFileMgrData);

   {
     FileMgrRec * file_mgr_rec = (FileMgrRec *)trashFileMgrData->file_mgr_rec;

     if( (file_mgr_rec->menuStates & PREFERENCES) == 0 )
       file_mgr_rec->menuStates |= PREFERENCES;
   }

   trashDialogPosted = False;
}



/************************************************************************
 *
 * DropOnTrashCan
 *
 ************************************************************************/

void
DropOnTrashCan(
        int file_count,
        char **host_set,
        char **file_set,
        DtDndDropCallbackStruct *drop_parameters)
{
   int arg_count = 0;
   DtActionArg * action_args;
   FileMgrRec * file_mgr_rec = (FileMgrRec *)trashFileMgrData->file_mgr_rec;
   Position drop_x = drop_parameters->x;
   Position drop_y = drop_parameters->y;

   _DtBuildActionArgsWithDroppedFiles(NULL, drop_parameters,
                                      &action_args, &arg_count);

   DtActionInvoke(file_mgr_rec->shell, TRASH_ACTION,
                  action_args, arg_count, NULL, NULL,
                  trashFileMgrData->current_directory, True, NULL, NULL);

   _DtFreeActionArgs(action_args, arg_count);

   RepositionIcons(trashFileMgrData, file_set, file_count, drop_x,
                                                          drop_y, True);
}



/************************************************************************
 *
 * MoveOutOfTrashCan
 *
 ************************************************************************/

void
MoveOutOfTrashCan(
        FileMgrData *file_mgr_data,
        FileMgrRec *file_mgr_rec,
        Window w,
        int file_count,
        char **host_set,
        char **file_set,
        Position drop_x,
        Position drop_y)
{
   int i;
   char *target_host;
   char *target_dir;
   char **file_list;

   /*
    * Get target host and directory
    */
   target_dir = XtNewString(file_mgr_data->current_directory);
   target_host = XtNewString(file_mgr_data->host);

   /*
    * Create file list and call RestoreFromTrash
    */
   file_list = (char **)XtMalloc(file_count * sizeof(char *));

   for (i = 0;  i < file_count; i++)
      file_list[i] = XtNewString(file_set[i]);

   RestoreFromTrash(file_list, file_count, target_host, target_dir, NULL,False);
}



/************************************************************************
 *
 * FileFromTrash
 *   Locate file in trash list based on new internal name.  Return file name.
 *
 ************************************************************************/

Boolean
FileFromTrash(
   char *filename)
{
   Boolean IsTrash = False;

   if (strncmp(users_home_dir, filename, strlen(users_home_dir)) == 0)
   {
      if (strncmp
         ((filename + strlen(users_home_dir)), TRASH_DIR, strlen(TRASH_DIR))
         == 0)
        IsTrash = True;
   }

   return(IsTrash);
}



/************************************************************************
 *
 * Noop
 *   Since the trash does not use shared menupanes, there is no work to
 *   be done during the popup and popdown callbacks; therefore, we use
 *   and empty function.
 *
 ************************************************************************/

static void
Noop (
   Widget w,
   XtPointer clientData,
   XtPointer callData )

{
   ;
}


/*--------------------------------------------------------------------
 * UpdateDirectoryOf:
 *   Arrange for the directory containing a file to be updated
 *------------------------------------------------------------------*/

static void
UpdateDirectoryOf(
   char *path)
{
   char *ptr;
   char host[256];

   /* remove last component from path to get the directory */
   ptr = strrchr(path, '/');
   if (ptr > path)
      *ptr = '\0';

   strcpy(host, home_host_name);

   /* now arrange for the directory to be updated */
   UpdateDirectory(NULL, host, path);

   /* restore the path */
   if (ptr > path)
      *ptr = '/';
}


/*--------------------------------------------------------------------
 * EraseObject, EraseDir
 *   Routines for recursively deleting files and directories
 *------------------------------------------------------------------*/

int
EraseObject(char *file_name)
{
   struct stat stat_buf;

   if (lstat(file_name, &stat_buf) < 0)
      return errno;

   else if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
      return EraseDir(file_name);

   else if (remove(file_name) < 0)
      return errno;

   else
      return 0;
}


int
EraseDir(char *dir_name)
{
   DIR *dir;                      /* open directory */
   struct dirent *entry;          /* directory entry */
   char srcname[MAX_PATH];
   int srclen;
   int rc;

   /* open source directory */
   dir = opendir(dir_name);
   if (dir == NULL)
     return errno;

   /* prepare source name */
   strcpy(srcname, dir_name);
   srclen = strlen(srcname);
   if (srcname[srclen - 1] != '/')
     srcname[srclen++] = '/';

   rc = 0;
   while (rc == 0 && (entry = readdir(dir)) != NULL)
   {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
         continue;
      strcpy(srcname + srclen, entry->d_name);
      rc = EraseObject(srcname);
   }

   closedir(dir);

   if (rc == 0)
   {
     if (rmdir(dir_name) < 0)
     {
       if (unlink(dir_name) < 0 )
       {
         perror(dir_name);
         rc = errno;
       }
     }
   }

   return rc;
}


/*====================================================================
 *
 * MoveToTrashProcess
 *      Run a background process to move files to the trash can.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * MoveToTrashProcess
 *   Main routine of background process for MoveToTrash
 *------------------------------------------------------------------*/

static void
MoveToTrashProcess(
	int pipe_fd,
	char **file_list,
	int file_count,
	Boolean do_verify_checks)
{
   char *baseName;
   char *path;
   char *to;
   short pipe_msg;
   struct stat statInfo;
   DIR * dirp;
   struct dirent * entry;
   Boolean success;
   int i, rc;
   char savechar;

   for (i = 0; i < file_count; i++)
   {
      /* get base name and full path */
      path = XtNewString(file_list[i]);
      baseName = strrchr(file_list[i], '/');
      if (baseName == NULL || path == NULL)
      {
         /* Invalid filename */
         XtFree(path);
         pipe_msg = PIPEMSG_OTHER_ERROR;
         rc = BAD_FILE_ERROR;
         DPRINTF(("MoveToTrashProcess: sending BAD_FILE_ERROR\n"));
         write(pipe_fd, &pipe_msg, sizeof(short));
         write(pipe_fd, &rc, sizeof(int));
         continue;
      }
      if (path && MatchesSacredDirectory(path))
      {
         /* Invalid filename */
         XtFree(path);
         pipe_msg = PIPEMSG_OTHER_ERROR;
         rc = BAD_FILE_SACRED;
         DPRINTF(("MoveToTrashProcess: sending BAD_FILE_SACRED\n"));
         write(pipe_fd, &pipe_msg, sizeof(short));
         write(pipe_fd, &rc, sizeof(int));
         continue;
      }

      baseName++;
      if (do_verify_checks)
      {
         char *tmpstr = (file_list[i] == (baseName - 1))?"/":file_list[i];
         struct stat s1;
         Boolean TrashError = False;

         /* check write permissions for the file */
         if (lstat(path,&s1) < 0)
         {
            rc = NO_TRASH_FILE;
            TrashError = True;
         }
         else
         {
           savechar = *(baseName-1);
           *(baseName-1) = '\0';
           if(CheckDeletePermission(tmpstr, path) != 0)
           {
              /* No write access; display error message */

             if (S_ISDIR(s1.st_mode))
               rc = BAD_TRASH_DIRECTORY;
	     else
               rc = BAD_TRASH_FILE;
	     TrashError = True;
	   }
           *(baseName-1) = savechar;
	 }
	 if(TrashError)
         {
           XtFree(path);
           pipe_msg = PIPEMSG_OTHER_ERROR;
           DPRINTF(("MoveToTrashProcess: sending BAD_TRASH message\n"));
           write(pipe_fd, &pipe_msg, sizeof(short));
           write(pipe_fd, &rc, sizeof(int));
           continue;
         }
         else if (CheckAccess(path, W_OK) != 0 && !S_ISLNK(s1.st_mode))
         {
            /* No write access; ask user for verification */
            XtFree(path);
            pipe_msg = PIPEMSG_OTHER_ERROR;
            rc = VERIFY_FILE;
            DPRINTF(("MoveToTrashProcess: sending VERIFY_FILE\n"));
            write(pipe_fd, &pipe_msg, sizeof(short));
            write(pipe_fd, &rc, sizeof(int));
            continue;
         }

         /*
          * If this is a directory, make sure it's empty, i.e.,
          * contains only ".", "..", and ".!" or ".~" files.
          */
         if (lstat(path, &statInfo) == 0 &&
             (statInfo.st_mode & S_IFMT) == S_IFDIR &&
             (dirp = opendir(path)) != NULL)
         {
            /* read the directory */
            while ((entry = readdir(dirp)) != NULL)
            {
               if ( !(strcmp(entry->d_name, ".") == 0 ||
                      strcmp(entry->d_name, "..") == 0 ||
                      strncmp(entry->d_name, ".!", 2) == 0 ||
                      strncmp(entry->d_name, ".~", 2) == 0) )
               {
                  /* found a real file: directory not empty */
                  break;
               }
            }

            closedir(dirp);

            if (entry != NULL)
            {
               /* Directory is not empty */
               XtFree(path);
               pipe_msg = PIPEMSG_OTHER_ERROR;
               rc = VERIFY_DIR;
               DPRINTF(("MoveToTrashProcess: sending VERIFY_DIR\n"));
               write(pipe_fd, &pipe_msg, sizeof(short));
               write(pipe_fd, &rc, sizeof(int));
               continue;
            }
         }
      } /* end if do_verify_checks */


      to = CreateTrashFilename(baseName, TRUE);

      /* move file to the trash directory */
      success = FileManip((Widget) (intptr_t) pipe_fd, MOVE_FILE, path, to, TRUE,
                          FileOpError, True, TRASH_DIRECTORY);
      if (success)
      {
         pipe_msg = PIPEMSG_DONE;
         DPRINTF(("MoveToTrashProcess: sending DONE\n"));
         write(pipe_fd, &pipe_msg, sizeof(short));
         PipeWriteString(pipe_fd, path);
         PipeWriteString(pipe_fd, to);
      }

      XtFree(path);
      XtFree(to);
   }
}


/*--------------------------------------------------------------------
 * MoveToTrashPipeCB:
 *   Read and process data sent through the pipe.
 *------------------------------------------------------------------*/

static void
MoveToTrashPipeCB(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   MoveToTrashCBData *cb_data = (MoveToTrashCBData *)client_data;
   short pipe_msg;
   int i, j, n, rc;
   char *title, *err_msg, *err_arg;
   int badCount;
   String buf;
   int bufsize;
   int verifyCount;
   String verifybuf;
   int verifybufsize;
   Boolean vfiles;
   Boolean vdirs;
   char **verifylist;
   int fileCount;
   char *baseName;
   char *fileName;
   DesktopRec *desktopWin;
   char *dir_error=NULL,*file_error=NULL,*no_file_error=NULL,*sacred_error=NULL;

   /* read the next msg from the pipe */
   pipe_msg = -1;
   n = PipeRead(*fd, &pipe_msg, sizeof(short));
   DPRINTF(("MoveToTrashPipeCB: n %d, pipe_msg %d\n", n, pipe_msg));

   switch (pipe_msg)
   {
      case PIPEMSG_FILEOP_ERROR:
         PipeRead(*fd, &rc, sizeof(int));
         err_msg = PipeReadString(*fd);
         err_arg = PipeReadString(*fd);

         FileOperationError(toplevel, err_msg, err_arg);

         XtFree(err_msg);
         XtFree(err_arg);

         cb_data->rc[cb_data->done_count++] = BAD_FILE_ERROR;
         break;

      case PIPEMSG_OTHER_ERROR:
         PipeRead(*fd, &rc, sizeof(int));
         cb_data->rc[cb_data->done_count++] = rc;
         break;

      case PIPEMSG_TARGET_TIME:
         break;

      case PIPEMSG_FILE_MODIFIED:
         break;

      case PIPEMSG_DONE:
         i = cb_data->done_count++;
         cb_data->path[i] = PipeReadString(*fd);
         cb_data->to[i] = PipeReadString(*fd);
         cb_data->rc[i] = 0;
         break;

      default:
        fprintf(stderr,
                "Internal error in MoveToTrashPipeCB: bad pipe_msg %d\n",
                pipe_msg);
        /* Don't know how it can get in here, but if it does, we'll try to
         * simulate an error condition.  Without it, the SGI hung up
         */
        while (cb_data->done_count < cb_data->file_count)
           cb_data->rc[cb_data->done_count++] = BAD_FILE_ERROR;

   }


   /* if still more files to be processed, return now to wait for the rest */
   if (cb_data->done_count < cb_data->file_count)
      return;


   /* close the pipe and cancel the callback */
   DPRINTF(("MoveToTrashPipeCB: done\n"));
   close(*fd);
   XtRemoveInput(*id);

   /* process the results */
   badCount = 0;
   buf = NULL;
   bufsize = 0;
   verifyCount = 0;
   verifybuf = NULL;
   verifybufsize = 0;
   vfiles = False;
   vdirs = False;
   verifylist = NULL;
   fileCount = 0;

   for (i = 0; i < cb_data->file_count; i++)
   {
     if (cb_data->rc[i]==BAD_TRASH_DIRECTORY || cb_data->rc[i]==BAD_TRASH_FILE)
     {
        if (badCount < 8)
        {
           char *tmpmsg;
           Boolean errflg = False;
           if(cb_data->rc[i] == BAD_TRASH_DIRECTORY)
           {
              tmpmsg = GETMESSAGE(27, 113,
                 "At least one object in the following Folder(s) or \nthe folder itself cannot be deleted.\nThe most likely cause is that you do not have permissions\nto delete that object.\n\n");
              if(dir_error)
              {
                 dir_error = (char *) XtRealloc(dir_error,strlen(dir_error)+
                                                strlen(cb_data->file_list[i])+5);
                 strcat(dir_error,cb_data->file_list[i]);
                 strcat(dir_error,"\n");
              }
              else
              {
                 dir_error = XtMalloc(strlen(tmpmsg)+
                                      strlen(cb_data->file_list[i])+5);
                 sprintf(dir_error,"%s%s\n",tmpmsg,cb_data->file_list[i]);
              }
           }
           else
           {
              tmpmsg = GETMESSAGE(27, 114,
                                  "The following file(s) cannot be deleted.  The most likely cause \nis that you do not have permissions to delete the file(s).\n\n");
              if(file_error)
              {
                 file_error = (char *) XtRealloc(file_error,strlen(file_error)
                        + strlen(cb_data->file_list[i])+5);
                 strcat(file_error,cb_data->file_list[i]);
                 strcat(file_error,"\n");
              }
              else
              {
                 file_error = XtMalloc(strlen(tmpmsg)+
                                       strlen(cb_data->file_list[i])+5);
                 sprintf(file_error,"%s%s\n",tmpmsg,cb_data->file_list[i]);
              }
           }
           XtFree(cb_data->file_list[i]);
           cb_data->file_list[i] = NULL;
        }
        badCount++;
     }
   else if (cb_data->rc[i] == BAD_FILE_SACRED)
   {
     if (badCount < 8)
     {
       char *tmpmsg = GETMESSAGE(27, 115,
         "The following object(s) cannot be deleted.\nThe desktop cannot function properly without these object(s).\n\n");
       if(sacred_error)
       {
         sacred_error = (char *) XtRealloc(sacred_error,strlen(sacred_error)+
                            strlen(cb_data->file_list[i])+5);
         strcat(sacred_error,cb_data->file_list[i]);
         strcat(sacred_error,"\n");
       }
       else
       {
         sacred_error=XtMalloc(strlen(tmpmsg)+strlen(cb_data->file_list[i])+5);
         sprintf(sacred_error,"%s%s\n",tmpmsg,cb_data->file_list[i]);
       }
       XtFree(cb_data->file_list[i]);
       cb_data->file_list[i] = NULL;
     }
     badCount++;
   }
   else if (cb_data->rc[i] == BAD_FILE_ERROR)
   {
       XtFree(cb_data->file_list[i]);
       cb_data->file_list[i] = NULL;
   }
   else if (cb_data->rc[i] == NO_TRASH_FILE)
   {
     char *tmpmsg;
     tmpmsg = GETMESSAGE(27, 112, "The following object(s) cannot be deleted.\nThe most likely cause is that these object(s)\nhave already been deleted.\n\n");

     if(badCount < 8)
     {
	if(no_file_error)
	{
	  no_file_error = (char *)XtRealloc(no_file_error,strlen(no_file_error)+
			    strlen(cb_data->file_list[i])+5);
	  strcat(no_file_error,cb_data->file_list[i]);
	  strcat(no_file_error,"\n");
	}
	else
	{
	  no_file_error = XtMalloc(strlen(tmpmsg)+
		  strlen(cb_data->file_list[i])+5);
	  sprintf(no_file_error,"%s%s\n",tmpmsg,cb_data->file_list[i]);
	}
     }
     badCount++;
   }
   else if (cb_data->rc[i] == VERIFY_FILE ||
              cb_data->rc[i] == VERIFY_DIR)
   {
       if (verifyCount < 6)
         AddString(&verifybuf, &verifybufsize, cb_data->file_list[i], NULL);
       verifyCount++;

       /*
        * Keep track of whether we have directories only, files only,
        * or a combination, so that we can display the appropriate
        * label in the dialog.
        */
       if (cb_data->rc[i] == VERIFY_FILE)
         vfiles = True;
       else
         vdirs = True;

       /*
        * Add to array which is to be attached to the dialog;
        * the array will ultimately be NULL terminated.
        */
       verifylist = (char **)XtRealloc((char *)verifylist,
                                       sizeof(char *) * (verifyCount + 1));
       verifylist[verifyCount - 1] = cb_data->file_list[i];
     }
     else
     {
       /* Add file to trash list */
       if (numTrashItems >= trashListSize)
       {
         trashListSize += 10;
         trashCan = (TrashEntry *)
           XtRealloc((char *)trashCan, sizeof(TrashEntry) * trashListSize);
       }

       trashCan[numTrashItems].problem = False;
       trashCan[numTrashItems].intNew = cb_data->to[i];
       trashCan[numTrashItems].intOrig = cb_data->path[i];
       trashCan[numTrashItems].external = cb_data->file_list[i];

       /* extract base file name */
       baseName = strrchr(cb_data->to[i], '/');
       baseName++;
       if (*baseName == '\0')
         baseName = ".";
       trashCan[numTrashItems].filename = XtNewString(baseName);

       numTrashItems++;
       fileCount++;

       /* arrange for the source directory to be updated */
       UpdateDirectoryOf(cb_data->path[i]);

       /*
        * If the source file was referenced by a desktop object,
        * we need to remove the destkop object
        */
       for (j = 0; j < desktop_data->numIconsUsed; j++)
       {
         Tt_status tt_status;

         desktopWin = desktop_data->desktopWindows[j];
         fileName = ResolveLocalPathName( desktopWin->host,
                                          desktopWin->dir_linked_to,
                                          desktopWin->file_name,
                                          home_host_name, &tt_status);

         if( TT_OK == tt_status )
         {
           if (strcmp(fileName, cb_data->path[i]) == 0)
#ifndef SUN_PERF
             RemoveDT(desktopWin->shell, (XtPointer)desktopWin, NULL);
#else
             RemoveMovedObjectFromDT(desktopWin->shell, (XtPointer)desktopWin,
			cb_data->file_count, cb_data->file_list);
#endif /* SUN_PERF */

           XtFree(fileName);
         }
       }
     }
   }
   if(dir_error)
   {
     buf = XtMalloc(strlen(dir_error)+3);
     sprintf(buf,"%s\n",dir_error);
     XtFree(dir_error);
   }
   if(file_error)
   {
     if(!buf)
     {
       buf = XtMalloc(strlen(file_error)+3);
       sprintf(buf,"%s\n",file_error);
     }
     else
     {
       buf = XtRealloc(buf,strlen(buf)+strlen(file_error)+3);
       sprintf(buf,"%s%s\n",buf,file_error);
     }
     XtFree(file_error);
   }
   if(no_file_error)
   {
     if(!buf)
     {
       buf = XtMalloc(strlen(no_file_error)+3);
       sprintf(buf,"%s\n",no_file_error);
     }
     else
     {
       buf = XtRealloc(buf,strlen(buf)+strlen(no_file_error)+3);
       sprintf(buf,"%s%s\n",buf,no_file_error);
     }
     XtFree(no_file_error);
   }
   if(sacred_error)
   {
     if(!buf)
     {
       buf = XtMalloc(strlen(sacred_error)+3);
       sprintf(buf,"%s\n",sacred_error);
     }
     else
     {
       buf = XtRealloc(buf,strlen(buf)+strlen(sacred_error)+3);
       sprintf(buf,"%s%s\n",buf,sacred_error);
     }
     XtFree(sacred_error);
   }
   if(buf)
     bufsize = strlen(buf);

   /* Update the trash information file, and the trash window */
   if (fileCount > 0)
   {
      FILE * trashInfoFileId = fopen(TrashInfoFileName, "a+");
      if( trashInfoFileId != NULL )
      {
        for (i = fileCount; i > 0; i--)
        {
          if( WriteEntry(trashInfoFileId,
                         trashCan[numTrashItems - i].external,
                         trashCan[numTrashItems - i].filename) < 0 )
            break;
        }
        fflush(trashInfoFileId);
        fclose(trashInfoFileId);
        if( trashFileMgrData )
          UpdateDirectory(NULL, trashFileMgrData->host,
                          trashFileMgrData->current_directory);
      }
   }

   /* Check for any bad files; post an error dialog */
   if (buf)
   {
      /* send a reply to the message that triggered this operation, if any */
#ifndef SUN_PERF
     if (cb_data->msg != 0) {
       /*
       Until Action is fixed.
       tttk_message_fail( cb_data->msg, TT_DESKTOP_EACCES, 0, 1 );
       */
       tt_message_reply( cb_data->msg );
       tttk_message_destroy( cb_data->msg );
#else
       if (cb_data->msg_cnt > 0) {
       for (i = 0 ; i < cb_data->msg_cnt ; i++) {
           tt_message_reply( cb_data->msg_list[i] ) ;
           tttk_message_destroy( cb_data->msg_list[i] ) ;
        }
       XtFree((char *)cb_data->msg_list) ;
       cb_data->msg_cnt = 0 ;
#endif /* SUN_PERF */
       cb_data->msg = 0;
     }
      /* If more items than can be displayed, let user know */
      if (badCount > 8)
      {
         char extraFiles[256];

         (void) sprintf(extraFiles, AdditionalHeader, badCount - 8);
         AddString(&buf,
                   &bufsize,
                   extraFiles,
                   GETMESSAGE(27,97, "The following objects could not be placed in the trash can:     \n"));
      }

      title = XtNewString(GetSharedMessage(TRASH_ERROR_TITLE));
      _DtMessage(toplevel, title, buf, NULL, HelpRequestCB);
      XtFree(title);
      XtFree ((char *) buf);
   } else {
      /* send a reply to the message that triggered this operation, if any */
#ifndef SUN_PERF
      if (cb_data->msg != 0) {
	 tt_message_reply( cb_data->msg );
	 tttk_message_destroy( cb_data->msg );
#else
        if (cb_data->msg_cnt > 0) {
        for (i = 0 ; i < cb_data->msg_cnt ; i++) {
           tt_message_reply( cb_data->msg_list[i] ) ;
           tttk_message_destroy( cb_data->msg_list[i] ) ;
        }
        XtFree((char *)cb_data->msg_list) ;
        cb_data->msg_cnt = 0 ;
#endif /* SUN_PERF */
	 cb_data->msg = 0;
      }
   }

   /* Check for any files requiring user verification; post a prompt dialog */
   /* XXX Really should fail incoming ToolTalk request if user cancels any */
   if (verifybuf)
   {
      char * tmpbuf;
      char * header;
      Widget dlog;
      Arg args[1];

      /* If more items than can be displayed, let user know */
      if (verifyCount > 6)
      {
         char extraFiles[256];

         (void) sprintf(extraFiles, AdditionalHeader, verifyCount - 6);
         AddString(&verifybuf, &verifybufsize, extraFiles, NULL);
      }

      /*
       * Depending upon what type of files are to be displayed in the
       * dialog, choose the appropriate dialog text.
       */
      if (vfiles && vdirs)
         header = GETMESSAGE(27, 6, "   Each of the following is either a non-empty folder,    \n   or a file for which you do not have write permission.    \n   Do you want to proceed?\n");
      else if (vfiles)
        header = GETMESSAGE(27,104, "You do not have write permission for the following file(s):\nDo you want to proceed?\n");
      else
         header = GETMESSAGE(27,100, "The following folder(s) are not empty.\nDo you want to proceed?\n");

      tmpbuf = XtMalloc(strlen(header) + strlen(verifybuf) + 1);
      sprintf(tmpbuf, "%s%s", header, verifybuf);
      title = XtNewString(GETMESSAGE(27, 4, "Trash Can Warning"));
      dlog = (Widget)_DtMessageDialog(toplevel, title, tmpbuf, NULL, True,
                                  VerifyCancel, VerifyOk, NULL, HelpRequestCB,
                                  False, WARNING_DIALOG);
      XtFree(title);
      XtFree ((char *) verifybuf);
      XtFree (tmpbuf);

      /*
       * Add array as userdata on the dialog.
       * NULL terminate the array.
       */
      verifylist[verifyCount] = NULL;
      XtSetArg(args[0], XmNuserData, verifylist);
      XtSetValues(dlog, args, 1);
   }

   /* free the callback data */
   XtFree((char *)cb_data->file_list);
   XtFree((char *)cb_data->path);
   XtFree((char *)cb_data->to);
   XtFree((char *)cb_data->rc);
   XtFree((char *)cb_data);

   CheckDesktop();
}


/*--------------------------------------------------------------------
 * MoveToTrash:
 *    Start the background process and set up callback for the pipe.
 *------------------------------------------------------------------*/

static void
MoveToTrash(
	char **file_list,
	int file_count,
	Boolean do_verify_checks,
	Tt_message msg)
{
   static char *pname = "MoveToTrash";
   MoveToTrashCBData *cb_data;
   int pipe_fd[2];
   int pid;

   if( !TrashInitialized )
   {
     if (msg != 0) {
       tttk_message_fail( msg, TT_DESKTOP_ENOSPC, 0, 1 );
     }
     return;
   }

   /* set up callback data */
   cb_data = XtNew(MoveToTrashCBData);
   cb_data->file_list = file_list;
   cb_data->file_count = file_count;
   cb_data->done_count = 0;
   cb_data->path = (char **)XtCalloc(file_count, sizeof(char *));
   cb_data->to = (char **)XtCalloc(file_count, sizeof(char *));
   cb_data->rc = (int *)XtCalloc(file_count, sizeof(int));
   cb_data->msg = msg;
#ifdef SUN_PERF
   cb_data->msg_cnt = 0 ;
   if (global_msg_cnt > 0) {
   cb_data->msg_cnt = global_msg_cnt ;
   cb_data->msg_list = (Tt_message *)XtMalloc(sizeof(Tt_message) * cb_data->msg_cnt);
   memcpy((char *)cb_data->msg_list,(char *)global_msg_list, sizeof(Tt_message) *
	cb_data->msg_cnt) ;
   }
#endif /* SUN_PERF */

   /* create a pipe */
   if(-1 == pipe(pipe_fd)) {
       fprintf(stderr,
		"%s:  pipe failed error %d=%s\n",
		pname, errno, strerror(errno));
       return;
   }

   /* fork the process that does the actual work */
   pid = fork();
   if (pid == -1)
   {
       fprintf(stderr,
		"%s:  fork failed, ppid %d, pid %d: error %d=%s\n",
		pname, getppid(), getpid(), errno, strerror(errno));
       return;
   }

   if (pid == 0)
   {
      /* child process */
      DBGFORK(("%s:  child forked, pipe %d\n", pname, pipe_fd[1]));

      close(pipe_fd[0]);  /* child won't read from the pipe */

      MoveToTrashProcess(pipe_fd[1], file_list, file_count, do_verify_checks);
      close(pipe_fd[1]);

      DBGFORK(("%s:  child exiting\n", pname));

      exit(0);
   }

   DBGFORK(("%s:  forked child<%d>, pipe %d\n", pname, pid, pipe_fd[0]));

   /* parent: set up callback to get the pipe data */
   close(pipe_fd[1]);  /* parent won't write the pipe */

   cb_data->child = pid;

   XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                 pipe_fd[0], (XtPointer)XtInputReadMask,
                 MoveToTrashPipeCB, (XtPointer)cb_data);
}


/*====================================================================
 *
 * RestoreFromTrash
 *
 *   Run a background process to restore files from the trash can.
 *
 *   These routines are used both for "normal restores", i.e.,
 *   restores initiated from the Trash menu or through an ICCCM
 *   message, as well as "drag&drop restores", i.e., restores done
 *   by dragging files from the trash can to some other directory.
 *   For normal resores, file_list contains only simple file names
 *   (no paths) and target_dir is NULL.  For drag&drop restores
 *   file_list contains complete path names and target_dir contains
 *   the name of the directoy to which the files should be moved.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * RestoreProcess
 *   Main routine of background process for RestoreFromTrash
 *------------------------------------------------------------------*/

static void
RestoreProcess(
	int pipe_fd,
	char **file_list,
	int file_count,
	char *target_host,
	char *target_dir,
	int *rc,
	Boolean CheckedAlready)
{
   int i, j;
   char *full_dirname;
   char *from, *to;
   char *ptr;
   char buf[MAX_PATH];
   short pipe_msg;
   int status;
   char **RestoreList= NULL;

   /* get full path name of target directory */
   if (target_dir)
   {
      Tt_status tt_status;
      full_dirname = ResolveLocalPathName(target_host, target_dir,
                                          NULL, home_host_name, &tt_status);
      if( TT_OK != tt_status )
      {
        /* send return codes back trough the pipe */
        pipe_msg = PIPEMSG_DONE;
        rc[0] = -1;
        DPRINTF(("RestoreProcess: Unable to Resolve local path name\n"));
        write(pipe_fd, &pipe_msg, sizeof(short));
        write(pipe_fd, rc, sizeof(int));
        return;
      }
   }
   else
      full_dirname = NULL;

   /* restore the files */
   for (i = 0; i < file_count; i++)
   {
      /* Locate file in trash list */
      for (j = 0; j < numTrashItems; j++)
      {
         /* file_list[i] may be a complete path or just a file name */
         if (strcmp(file_list[i], trashCan[j].filename) == 0 ||
             strcmp(file_list[i], trashCan[j].intNew) == 0)
         {
            break;
         }
      }

      /* determine source and target for the move */
      if (target_dir == NULL)
      {
         /* this is a normal restore */
         if (j < numTrashItems)
         {
            from = trashCan[j].intNew;
            to = trashCan[j].intOrig;
         }
         else
         {
            /* can't do a restore if the file wasn't found in the trash list */
            from = to = NULL;
         }
      }
      else
      {
         /* this is a drag&drop from the trash can to target_dir */
         from = file_list[i];
         if (j < numTrashItems)
            ptr = strrchr(trashCan[j].intOrig, '/');
         else
            ptr = strrchr(file_list[i], '/');
         strcpy(buf, full_dirname);
         strcat(buf, ptr);
         to = buf;
      }

      if (to != NULL)
      {

         status = RestoreObject((Widget) (intptr_t) pipe_fd, MOVE_FILE, from,to,
			TRUE, FileOpError, False, NOT_DESKTOP,CheckedAlready);
         /* restore was successful */
         if(status == (int) True)
	   rc[i] = 0;
	 else if(status == (int) False)
	   rc[i] = -1;
	 else
	   rc[i] = SKIP_FILE;
      }
      else
         /* restore failed */
         rc[i] = -1;
   }

   /* send return codes back trough the pipe */
   pipe_msg = PIPEMSG_DONE;
   DPRINTF(("RestoreProcess: sending DONE\n"));
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, rc, file_count * sizeof(int));

   XtFree(full_dirname);
}


/*--------------------------------------------------------------------
 * RestorePipeCB:
 *   Read and process data sent through the pipe.
 *------------------------------------------------------------------*/

static void
RestorePipeCB(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   RestoreFromTrashCBData *cb_data = (RestoreFromTrashCBData *)client_data;
   short pipe_msg;
   int i, j, k, n, rc;
   char *title, *err_msg, *err_arg;
   String buf;
   int bufsize,index;
   int RestoreIndex=0;
   char **ToRestoreList=NULL;
   char **FromRestoreList=NULL;
   char *target_host,*target_dir;
   Tt_status msg;

   /* read the next msg from the pipe */
   pipe_msg = -1;
   n = PipeRead(*fd, &pipe_msg, sizeof(short));
   DPRINTF(("RestorePipeCB: n %d, pipe_msg %d\n", n, pipe_msg));

   switch (pipe_msg)
   {
      case PIPEMSG_FILEOP_ERROR:
         PipeRead(*fd, &rc, sizeof(int));
         err_msg = PipeReadString(*fd);
         err_arg = PipeReadString(*fd);
            FileOperationError(toplevel, err_msg, err_arg);
         /* This call will popup an error dialog.

            FileOperationError(toplevel, err_msg, err_arg);

            It's not appropriate at all to popup an error dialog here.
            i.e. if there're 1000 files, and the file system is full,
                 and we're unable to move, would we want to popup 1000
                 error dialogs. NOT!
         */
         XtFree(err_msg);
         XtFree(err_arg);
         return;
      case PIPEMSG_DONE:
         PipeRead(*fd, cb_data->rc, cb_data->file_count * sizeof(int));
         break;

      default:
        fprintf(stderr,
                "Internal error in RestorePipeCB: bad pipe_msg %d\n",
                pipe_msg);
   }

   /* close the pipe and cancel the callback */
   DPRINTF(("RestorePipeCB: done\n"));
   close(*fd);
   XtRemoveInput(*id);

   buf = NULL;
   bufsize = 0;

   for (i = 0; i < cb_data->file_count; i++)
   {
      /* Locate file in trash list */
      for (j = 0; j < numTrashItems; j++)
      {
         /* file_list[i] may be a complete path or just a file name */
         if (strcmp(cb_data->file_list[i], trashCan[j].filename) == 0 ||
             strcmp(cb_data->file_list[i], trashCan[j].intNew) == 0)
         {
            break;
         }
      }

      if (cb_data->rc[i] == SKIP_FILE)
      {
	 ToRestoreList = (char **) XtRealloc((char *)ToRestoreList,sizeof(char *) *
				    (++RestoreIndex));
	 ToRestoreList[RestoreIndex-1] = XtNewString(trashCan[j].intOrig);
	 FromRestoreList = (char **) XtRealloc((char *)FromRestoreList,sizeof(char *) *
				    RestoreIndex);
	 FromRestoreList[RestoreIndex-1] = XtNewString( trashCan[j].intNew );
	 continue;
      }
      /* Check the return code from the restore */
      else if (cb_data->rc[i] == 0)
      {
         /* restore was successful: remove the file from the trash list */
         if (j < numTrashItems)
         {
            /* arrange for the source directory to be updated */
            UpdateDirectoryOf(trashCan[j].intOrig);

            /* Remove this entry from the trash list */
            XtFree ((char *) trashCan[j].intNew);
            XtFree ((char *) trashCan[j].intOrig);
            XtFree ((char *) trashCan[j].external);
            XtFree ((char *) trashCan[j].filename);
            for (k = j; k < (numTrashItems - 1); k++)
               trashCan[k] = trashCan[k + 1];

            numTrashItems--;
         }
      }
      else
      {
         char *restore_header,*tmpStr = GETMESSAGE(27,101,
                           "The following object(s) could not be put back:\n");

         restore_header = XtNewString(tmpStr);
         if (j < numTrashItems && cb_data->target_dir == NULL)
            AddString(&buf, &bufsize, trashCan[j].external, restore_header);
         else
            AddString(&buf, &bufsize, cb_data->file_list[i], restore_header);
         XtFree(restore_header);
      }
   }

   /* Update the trash information file */
   if( ! WriteTrashEntries() )
   {
     char * tmpStr, * title;

     title = XtNewString(GetSharedMessage(TRASH_ERROR_TITLE));
     tmpStr = XtNewString(GETMESSAGE(27, 88, "Cannot write to a temporary file.\nPerhaps your file system is full.\n"));
     _DtMessage(toplevel, title, tmpStr, NULL, HelpRequestCB);
     XtFree(title);
     XtFree(tmpStr);
   }

   /* send a reply to the message that triggered this operation, if any */
   if (cb_data->msg != 0) {
      tt_message_reply( cb_data->msg );
      tttk_message_destroy( cb_data->msg );
      cb_data->msg = 0;
   }

   /* Report any errors */
   if (buf)
   {
      title = XtNewString(GetSharedMessage(TRASH_ERROR_TITLE));
      _DtMessage(toplevel, title, buf, NULL, HelpRequestCB);
      XtFree(title);
      XtFree ((char *) buf);
   }

   UpdateDirectory(NULL, trashFileMgrData->host,
                   trashFileMgrData->current_directory);
   if (cb_data->target_dir)
      UpdateDirectory(NULL, cb_data->target_host, cb_data->target_dir);

   target_host = XtNewString(cb_data->target_host);
   target_dir  = XtNewString(cb_data->target_dir);

   /* free the callback data */
   for (i = 0; i < cb_data->file_count; i++)
      XtFree(cb_data->file_list[i]);
   XtFree((char *)cb_data->file_list);
   XtFree((char *)cb_data->target_host);
   XtFree((char *)cb_data->target_dir);
   XtFree((char *)cb_data->rc);
   XtFree((char *)cb_data);
   if(ToRestoreList)
   {
     for(i=0;i<RestoreIndex;i++)
     {
       CreateRestoreDialog(FromRestoreList[i],ToRestoreList[i]);
       XtFree(FromRestoreList[i]);
       XtFree(ToRestoreList[i]);
     }
     XtFree((char *) FromRestoreList);
     XtFree((char *) ToRestoreList);
   }
   XtFree(target_host);
   XtFree(target_dir);
}


/*--------------------------------------------------------------------
 * RestoreFromTrash:
 *    Start the background process and set up callback for the pipe.
 *------------------------------------------------------------------*/

static void
RestoreFromTrash(
	char **file_list,
	int file_count,
	char *target_host,
	char *target_dir,
	Tt_message msg,
	Boolean CheckedAlready)
{
   static char *pname = "RestoreFromTrash";
   RestoreFromTrashCBData *cb_data;
   int pipe_fd[2];
   int pid;

   if( !TrashInitialized )
   {
     if (msg != 0) {
       tttk_message_fail( msg, TT_DESKTOP_ENOSPC, 0, 1 );
     }
     return;
   }

   /* set up callback data */
   cb_data = XtNew(RestoreFromTrashCBData);
   cb_data->file_list = file_list;
   cb_data->file_count = file_count;
   cb_data->target_host = target_host;
   cb_data->target_dir = target_dir;
   cb_data->rc = (int *)XtCalloc(file_count, sizeof(int));
   cb_data->msg = msg;

   /* create a pipe */
   if(-1 == pipe(pipe_fd)) {
       fprintf(stderr,
		"%s: pipe failed, error %d=%s\n",
		pname, errno, strerror(errno));
       return;
   }

   /* fork the process that does the actual work */
   pid = fork();
   if (pid == -1)
   {
       fprintf(stderr,
		"%s:  fork failed, ppid %d, pid %d: error %d=%s\n",
		pname, getppid(), getpid(), errno, strerror(errno));
       return;
   }

   if (pid == 0)
   {
      /* child process */
      DBGFORK(("%s:  child forked, pipe %d\n", pname, pipe_fd[1]));

      close(pipe_fd[0]);  /* child won't read from the pipe */

      RestoreProcess(pipe_fd[1], file_list, file_count,
                     target_host, target_dir, cb_data->rc,CheckedAlready);
      close(pipe_fd[1]);

      DBGFORK(("%s:  child exiting\n", pname));

      exit(0);
   }

   DBGFORK(("%s:  forked child<%d>, pipe %d\n", pname, pid, pipe_fd[0]));

   /* parent: set up callback to get the pipe data */
   close(pipe_fd[1]);  /* parent won't write the pipe */

   cb_data->child = pid;

   XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                 pipe_fd[0], (XtPointer)XtInputReadMask,
                 RestorePipeCB, (XtPointer)cb_data);
}



/*====================================================================
 *
 * EmptyTrash
 *   Run a background process to remove files from the trash can.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * EmptyTrashProcess
 *   Main routine of background process for EmptyTrash
 *------------------------------------------------------------------*/

static void
EmptyTrashProcess(
	int pipe_fd,
	DeleteList * del_list,
	int del_count,
	int *rc)
{
   int i;
   short pipe_msg;

   /*
    * Delete all files or directories in the list,
    * as well as any associated annotations.
    */
   for (i = 0; i < del_count; i++)
   {
      /* delete the file */
      rc[i] = EraseObject(del_list[i].trash);
   }

   /* send return codes back trough the pipe */
   pipe_msg = PIPEMSG_DONE;
   DPRINTF(("EmptyTrashProcess: sending DONE\n"));
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, rc, del_count * sizeof(int));
}


/*--------------------------------------------------------------------
 * EmptyTrashPipeCB:
 *   Read and process data sent through the pipe.
 *------------------------------------------------------------------*/

static void
EmptyTrashPipeCB(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   EmptyTrashCBData *cb_data = (EmptyTrashCBData *)client_data;
   short pipe_msg;
   int i, j, k, n, rc;
   char *title, *err_msg, *err_arg;
   int problemCount, itemCount;
   String buf;
   int bufsize;
   char *file_name;

   /* read the next msg from the pipe */
   pipe_msg = -1;
   n = PipeRead(*fd, &pipe_msg, sizeof(short));
   DPRINTF(("EmptyTrashPipeCB: n %d, pipe_msg %d\n", n, pipe_msg));

   switch (pipe_msg)
   {
      case PIPEMSG_FILEOP_ERROR:
         PipeRead(*fd, &rc, sizeof(int));
         err_msg = PipeReadString(*fd);
         err_arg = PipeReadString(*fd);
         /* This call will popup an error dialog.

            FileOperationError(toplevel, err_msg, err_arg);

            It's not appropriate at all to popup an error dialog here.
            i.e. if there're 1000 files, and the file system is full,
                 and we're unable to move, would we want to popup 1000
                 error dialogs. NOT!
         */
         XtFree(err_msg);
         XtFree(err_arg);
         return;

      case PIPEMSG_DONE:
         PipeRead(*fd, cb_data->rc, cb_data->del_count * sizeof(int));
         break;

      default:
        fprintf(stderr,
                "Internal error in EmptyTrashPipeCB: bad pipe_msg %d\n",
                pipe_msg);
   }

   /* close the pipe and cancel the callback */
   DPRINTF(("EmptyTrashPipeCB: done\n"));
   close(*fd);
   XtRemoveInput(*id);

   buf = NULL;
   bufsize = 0;
   problemCount = 0;
   itemCount = 0;

   if (cb_data->removeType == TRASH_FILE)
   {
      for (i = 0; i < cb_data->del_count; i++)
      {
         /* Locate file in trash list */
         for (j = 0; j < numTrashItems; j++)
            if (strcmp(cb_data->del_list[i].trash, trashCan[j].intNew) == 0)
               break;

         /* Check the return code from the erase */
         if (cb_data->rc[i] == 0)
         {
            /* erase was successful: remove the file from the trash list */
            if (j < numTrashItems)
            {
               /* Remove this entry from the trash list */
               XtFree ((char *) trashCan[j].intNew);
               XtFree ((char *) trashCan[j].intOrig);
               XtFree ((char *) trashCan[j].external);
               XtFree ((char *) trashCan[j].filename);
               for (k = j; k < (numTrashItems - 1); k++)
                  trashCan[k] = trashCan[k + 1];

               numTrashItems--;
            }
         }
         else
         {
            /* erase failed */
            problemCount++;
            if (itemCount < 8)
            {
               char * msg = XtNewString(GETMESSAGE(27,96, "The following objects could not be removed from the file system:   \n"));

               if (j < numTrashItems)
                  file_name = trashCan[j].external;
               else
                  file_name = cb_data->del_list[i].trash;
               AddString(&buf, &bufsize, file_name, msg);
               XtFree(msg);
               itemCount++;
            }
         }
      }

      /* Update the trash information file */
      if( ! WriteTrashEntries() )
      {
        char * tmpStr, * title;

        title = XtNewString(GetSharedMessage(TRASH_ERROR_TITLE));
        tmpStr = XtNewString(GETMESSAGE(27, 88, "Cannot write to a temporary file.\nPerhaps your file system is full.\n"));
        _DtMessage(toplevel, title, tmpStr, NULL, HelpRequestCB);
        XtFree(title);
        XtFree(tmpStr);
      }

      /* Report any errors */
      if (problemCount)
      {
         /* If more items than can be displayed, let user know */
         if (itemCount < problemCount)
         {
            char extraFiles[256];
            sprintf(extraFiles, AdditionalHeader, problemCount - itemCount);
            AddString(&buf,
                      &bufsize,
                      extraFiles,
                      GETMESSAGE(27,97, "The following objects could not be placed in the trash can:     \n") );
         }

         title = XtNewString(GetSharedMessage(TRASH_ERROR_TITLE));
         _DtMessage(toplevel, title, buf, NULL, HelpRequestCB);
         XtFree(title);
         XtFree ((char *) buf);
      }

      if (trashFileMgrData->selected_file_count > 0)
      {
        XtSetSensitive(*removeBtn, True);
        XtSetSensitive(*restoreBtn, True);
      }

      XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_REMOVE], True);
      XtSetSensitive(fileMgrPopup.trash_objPopup[BTN_RESTORE], True);

      UpdateDirectory(NULL, trashFileMgrData->host,
                      trashFileMgrData->current_directory);
   }
   else
   {
      if (cb_data->rc[0] == 0)
      {
         /* arrange for the directory containing the file to be updated */
         UpdateDirectoryOf(cb_data->del_list[0].trash);
      }
      else
      {
         /*  put up an error message saying the file couldn't be removed */
         char *buffer;
         char *tmpStr;

         tmpStr = GETMESSAGE(27, 81, "Shred File/Folder Error");
         title = XtNewString(tmpStr);
         tmpStr = GETMESSAGE(27, 82, "   The following file could not be removed from the file system:  \n");
         file_name = cb_data->del_list[0].trash;
         buffer = XtMalloc(strlen(tmpStr) + strlen(file_name) + 1);
         sprintf(buffer, "%s%s\n", tmpStr, file_name);
         _DtMessage(toplevel, title, buffer, NULL, HelpRequestCB);
         XtFree(title);
         XtFree(buffer);
      }
   }


   /* reset removingTrash flag */
   removingTrash = False;

   /* send a reply to the message that triggered this operation, if any */
   if (cb_data->msg != 0) {
      tt_message_reply( cb_data->msg );
      tttk_message_destroy( cb_data->msg );
      cb_data->msg = 0;
   }

   /* free the callback data */
   for (i = 0; i < cb_data->del_count; i++)
   {
      XtFree(cb_data->del_list[i].trash);
      XtFree(cb_data->del_list[i].orig);
   }
   XtFree((char *)cb_data->del_list);
   XtFree((char *)cb_data->rc);
   XtFree((char *)cb_data);
}


/*--------------------------------------------------------------------
 * EmptyTrash:
 *    Start the background process and set up callback for the pipe.
 *------------------------------------------------------------------*/

static void
EmptyTrash(
	DeleteList *del_list,
	int del_count,
	int removeType,
	Tt_message msg)
{
   static char *pname = "EmptyTrash";
   EmptyTrashCBData *cb_data;
   int pipe_fd[2];
   int pid;

   if( !TrashInitialized )
   {
     if (msg != 0) {
       tttk_message_fail( msg, TT_DESKTOP_ENOSPC, 0, 1 );
     }
     return;
   }

   /* set removingTrash flag */
   removingTrash = True;

   /* set up callback data */
   cb_data = XtNew(EmptyTrashCBData);
   cb_data->del_list = del_list;
   cb_data->del_count = del_count;
   cb_data->removeType = removeType;
   cb_data->rc = (int *)XtCalloc(del_count, sizeof(int));
   cb_data->msg = msg;

   /* create a pipe */
   if(-1 == pipe(pipe_fd)) {
       fprintf(stderr,
		"%s: pipe failed, error %d=%s\n",
		pname, errno, strerror(errno));
       return;
   }

   /* fork the process that does the actual work */
   pid = fork();
   if (pid == -1)
   {
       fprintf(stderr,
		"%s:  fork failed, ppid %d, pid %d: error %d=%s\n",
		pname, getppid(), getpid(), errno, strerror(errno));
       return;
   }

   if (pid == 0)
   {
      /* child process */
      DBGFORK(("%s:  child forked, pipe %d\n", pname, pipe_fd[1]));

      close(pipe_fd[0]);  /* child won't read from the pipe */

      EmptyTrashProcess(pipe_fd[1], del_list, del_count, cb_data->rc);
      close(pipe_fd[1]);

      DBGFORK(("%s:  child exiting\n", pname));

      exit(0);
   }

   DBGFORK(("%s:  forked child<%d>, pipe %d\n", pname, pid, pipe_fd[0]));

   /* parent: set up callback to get the pipe data */
   close(pipe_fd[1]);  /* parent won't write the pipe */

   cb_data->child = pid;

   XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                 pipe_fd[0], (XtPointer)XtInputReadMask,
                 EmptyTrashPipeCB, (XtPointer)cb_data);
}

int
CheckDeletePermission(
  char *parentdir,
  char *destinationPath)
{
#if defined(__FreeBSD__) || defined(__OpenBSD__)
  struct statfs statbuf;
#elif defined(__NetBSD__)
  struct statvfs statbuf;
#else
  struct stat statbuf;
#endif
  char fname[1024];

#if defined(__FreeBSD__) || defined(__OpenBSD__)
  if (statfs(parentdir,&statbuf) < 0)  /* does not exist */
#elif defined(__NetBSD__)
  if (statvfs(parentdir,&statbuf) < 0)  /* does not exist */
#else
  if (lstat(parentdir,&statbuf) < 0)  /* does not exist */
#endif
    return -1;

  /* check if we are root */
  if (getuid() == 0)
  {
    /* if NFS, need to check if server trusts root */
#if defined(CSRG_BASED)
    if (!strcmp(statbuf.f_fstypename, "nfs"))  /* Root user and nfs */
#else
    if (FileSysType(statbuf.st_dev) < 0)  /* Root user and nfs */
#endif
    {
       char *tmpfile;
       tmpfile = tempnam(parentdir,"quang");
       if (creat(tmpfile,O_RDONLY)< 0)         /* Create a temporary file */
          return -1;
       if (remove(tmpfile) < 0)                /* Delete the created file */
          return -1;
    }

    /* root user can delete anything */
    return 0;
  }

  /* check for write and execute permisssion on parent dir */
  if (CheckAccess(parentdir, W_OK | X_OK) < 0)
      return -1;

  /* copy destinationPath to tmp buffer */
  strcpy(fname, destinationPath);

  return CheckDeletePermissionRecur(fname);
}


static int
CheckDeletePermissionRecur(
  char *destinationPath)
{
  struct stat statbuf;
  DIR *dirp;
  struct dirent * dp;
  Boolean first_file;
  char *fnamep;

  DPRINTF(("CheckDeletePermissionRecur(\"%s\")\n", destinationPath));

  if (lstat(destinationPath, &statbuf) < 0)
    return -1;  /* probably does not exist */

  if (! S_ISDIR(statbuf.st_mode))
    return 0;   /* no need to check anything more */

  dirp = opendir (destinationPath);
  if (dirp == NULL)
    return -1;  /* could not read directory */


  first_file = True;

  while (dp = readdir (dirp))
  {
    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
    {
      if (first_file)
      {
        /* check for write permission in this directory */
        if (CheckAccess(destinationPath, W_OK|X_OK) < 0)
          return -1;

        /* append a '/' to the end of directory name */
        fnamep = destinationPath + strlen(destinationPath);
        *fnamep++ = '/';

        first_file = False;
      }

      /* append file name to end of directory name */
      strcpy(fnamep, dp->d_name);

      /* recursively check permission on this file */
      if (CheckDeletePermissionRecur(destinationPath))
        return -1;
    }
  }

  return 0;
}

#if !defined(CSRG_BASED)
static int
FileSysType(
   int dev)
{
  struct ustat u1;
  if(ustat(dev,&u1) < 0)
     return -2;
  return u1.f_tinode;
}
#endif

static int
RestoreObject(
        Widget w,
        int mode,
        register char *source,
        register char *target,
        Boolean  isContainer,
        void (*errorHandler)(),
        Boolean checkForBusyDir,
        int type ,
        Boolean CheckedAlready)
{
  struct stat statsrc,stattar;
  Boolean status;
  char *localdir,*chrptr;

  if(!CheckedAlready)
  {
    if(stat(source,&statsrc) < 0)
      if(lstat(source,&statsrc) < 0)
        return 0;

    localdir =  strdup(target);

    chrptr = strrchr(localdir,'/');
    if(chrptr)
    {
      if(chrptr == localdir) /* Must be root folder */
         chrptr = "/";
      else
         *chrptr = '\0';
    }
    if(stat(target,&stattar) >= 0)  /* Target exists  */
    {
       if(CheckDeletePermission(localdir,target))
         return ((int)False);
       else
         return SKIP_FILE;
    }
  }
  return ((int )FileManip((Widget)w, MOVE_FILE, source, target, TRUE,
                                  FileOpError, False, NOT_DESKTOP));
}
static void
CreateRestoreDialog(
   char  *source,
   char *target)
{
    char *tmpbuf,*title;
    Widget dw;
    Arg args[4];
    int n=0;
    char **dirs = (char **) malloc(sizeof(char *) * 2);
    char *header    = GETMESSAGE(27,110,"There is already an object named\n\n%s\n\nSelect Ok if you want to replace the object\nwith the object from the Trash.\n\nSelect Cancel if you want to keep the object.\nIf you then rename the object, you will be able\nto repeat the Put Back and keep both objects.");

    dirs[0] = strdup(source);
    dirs[1] = strdup(target);
    tmpbuf = XtMalloc(strlen(header) + strlen(target) + 1);
    sprintf(tmpbuf, header, target);
    title = XtNewString(GETMESSAGE(27, 109, "Put Back Warning"));
    dw = (Widget)_DtMessageDialog(toplevel, title,tmpbuf,NULL,True,RestoreVerifyCancel,
          RestoreVerifyOk, NULL, HelpRequestCB, False, WARNING_DIALOG);

    XtSetArg(args[n], XmNuserData,dirs); n++;
    XtSetValues(dw,args,n);
    XtFree(tmpbuf);
    XtFree(title);
}

static void
RestoreVerifyOk(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    char **dirs;
    Arg args[4];
    int n=0;
    Widget mbox = (Widget)client_data;

    XtSetArg(args[n],XmNuserData,&dirs); n++;
    XtGetValues(mbox,args,n);

    if(DirectoryBusy(dirs[1]))
    {
       char *msgbuf;
       char * tmpStr,*title;

       title = XtNewString(GETMESSAGE(27,111,"Put Back Error"));
       tmpStr = GETMESSAGE(11,30, "Cannot move or rename the folder %s.\nAll File Manager views displayed for a folder or its sub-folders\nmust be closed before a folder can be moved or renamed.");
       msgbuf = XtMalloc(strlen(tmpStr) + strlen(dirs[1])+1);
       sprintf (msgbuf, tmpStr, dirs[1]);
       _DtMessage(toplevel,title,msgbuf,NULL,HelpRequestCB);
       XtFree(title);
       XtFree(msgbuf);
    }
    else
    {
      int status,j,k;
      struct stat s1;
      char *realTarget,*tptr;
      Boolean moveop;
      char **FileList;

      if(lstat(dirs[1],&s1) < 0)
         goto goback;
      fsErase(dirs[1],&status,0);
      FileList = (char **) XtMalloc(sizeof(char *));
      FileList[0] = XtNewString(dirs[0]);
      RestoreFromTrash(FileList, (int) 1, NULL, NULL, NULL,True);
    }
goback:
    free(dirs[0]);
    free(dirs[1]);
    free(dirs);
    XtDestroyWidget(mbox);
}

static void
RestoreVerifyCancel(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )

{
  Arg args[4];
  int n=0;
  char **dirs;
  Widget mbox = (Widget)client_data;

  XtSetArg(args[n],XmNuserData,&dirs); n++;
  XtGetValues(mbox,args,n);
  free(dirs[0]);
  free(dirs[1]);
  free(dirs);
  XtDestroyWidget(mbox);
}

