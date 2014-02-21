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
/* $TOG: Directory.c /main/18 1999/12/09 13:05:34 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Directory.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Directory processing functions used by the File Browser.
 *
 *   FUNCTIONS: CheckDesktop
 *              CheckDesktopPipeCallback
 *              CheckDesktopProcess
 *              CheckListCmp
 *              DirectoryBeginModify
 *              DirectoryBusy
 *              DirectoryEndModify
 *              DirectoryFileModified
 *              DirectoryGone
 *              DirectoryModifyTime
 *              FileData2toFileData
 *              FileWindowMapUnmap
 *              FindDirectory
 *              FreeDirectory
 *              FreeFileData
 *              GetDirectoryLogicalType
 *              GetDirectoryPositionInfo
 *              GetLongName
 *              InitializeDirectoryRead
 *              InitializePositionFileName
 *              PipeReadFileData
 *              PipeReadPositionInfo
 *              PipeWriteFileData
 *              PipeWritePositionInfo
 *              ReadDir
 *              ReadDirectory
 *              ReadDirectoryFiles
 *              ReadDirectoryProcess
 *              ReadFileData
 *              ReadFileData2
 *              ReaddirPipeCallback
 *              RereadDirectory
 *              ScheduleActivity
 *              ScheduleDirectoryActivity
 *              SetDirectoryPositionInfo
 *              SkipRefresh
 *              SomeWindowMapped
 *              StickyProcIdle
 *              TimerEvent
 *              TimerEventBrokenLinks
 *              TimerEventProcess
 *              TimerPipeCallback
 *              UpdateAllProcess
 *              UpdateCachedDirectories
 *              UpdateDirectory
 *              UpdateDirectorySet
 *              UpdateSomeProcess
 *              WritePosInfoPipeCallback
 *              WritePosInfoProcess
 *              _ReadDir
 *              SelectDesktopFile
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include <Xm/Xm.h>

#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Dts.h>
#include <Dt/Icon.h>
#include <Tt/tttk.h>

#include "Encaps.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "IconicPath.h"
#include "Main.h"
#include "SharedMsgs.h"
#include "SharedProcs.h"
#include "Prefs.h"

extern Boolean removingTrash;


/*--------------------------------------------------------------------
 * Constants and Types
 *------------------------------------------------------------------*/

/* File modes */
#define OPTION_OFF '-'
#define WRITE_PRIV 'w'
#define READ_PRIV  'r'
#define EXEC_PRIV  'x'

/* prefix for the name of the position info file */
#define POSITION_FILE_PREFIX  ".!dt"

/* kinds of messages sent through the pipe */
#define PIPEMSG_ERROR               1
#define PIPEMSG_FILEDATA            2
#define PIPEMSG_DONE                3
#define PIPEMSG_PATH_LOGICAL_TYPES  4
#define PIPEMSG_POSITION_INFO       5
#define PIPEMSG_FILEDATA2           6
#define PIPEMSG_FILEDATA3           7
#define PIPEMSG_DESKTOP_REMOVED     8
#define PIPEMSG_DESKTOP_CHANGED     9

#ifndef	FILEDATABUF
#define	FILEDATABUF 50
#endif /* FILEDATABUF */

#define	NILL '\0'

/*
 * Background activities, ordered by priority:
 * (activity_idle must be the last one in the list!)
 */
typedef enum
{
  activity_writing_posinfo,  /* writing position information file */
  activity_reading,          /* reading the directory */
  activity_update_all,       /* updating the directory */
  activity_update_some,      /* updating selected files */
  activity_checking_links,   /* checking for broken links */
  activity_checking_desktop, /* checking desktop objects */
  activity_checking_dir,     /* checking if the directory has changed */
  activity_idle              /* no background activity */
} ActivityStatus;

/*  The internal directory structure and directory set list  */

typedef struct
{
   FileMgrData * file_mgr_data;
   Boolean       mapped;
} DirectoryView;

typedef struct
{
   char           * host_name;
   char           * directory_name;
   char           * path_name;
   char           * tt_path_name;
   Boolean          viewed;
   ActivityStatus   activity;
   Boolean          busy[activity_idle];
   Boolean          errmsg_needed;
   int              errnum;
   time_t           modify_time;
   int              last_check;
   Boolean          link_check_needed;
   int              file_count;
   FileData       * file_data;
   FileData       * new_data;
   FileData       * dir_data;
   int              path_count;
   char          ** path_logical_types;
   int              position_count;
   PositionInfo   * position_info;
   int              modify_begin;
   Boolean          was_up_to_date;
   int              modified_count;
   char          ** modified_list;
   int              numOfViews;
   DirectoryView  * directoryView;
} Directory;


/* data for keeping track of sticky background procs */
typedef struct _spd
{
   pid_t child;
   int pipe_s2m_fd;
   int pipe_m2s_fd;
   Boolean idle;
   struct _spd *next;
} StickyProcDesc;

/* data for callback routines that handle background processes */
typedef struct
{
   Directory *directory;
   pid_t child;
   StickyProcDesc *sticky_proc;
   ActivityStatus activity;
} PipeCallbackData;


/* background procedure */
typedef int (*DirBackgroundProc)(int, Directory *, ActivityStatus);

extern void _DtFlushIconFileCache(String path);


/*--------------------------------------------------------------------
 * Static Function Declarations
 *------------------------------------------------------------------*/

static void TimerEvent(
                        XtPointer client_data,
                        XtIntervalId *id);
static void ScheduleActivity(
			Directory *directory);
static int WritePosInfoProcess(
			int pipe_fd,
			Directory *directory,
			ActivityStatus activity);
static int ReadDirectoryProcess(
			int pipe_fd,
			Directory *directory,
			ActivityStatus activity);
static int UpdateAllProcess(
			int pipe_fd,
			Directory *directory,
			ActivityStatus activity);
static int UpdateSomeProcess(
			int pipe_fd,
			Directory *directory,
			ActivityStatus activity);
static int TimerEventProcess(
			int pipe_fd,
			Directory *directory,
			ActivityStatus activity);
static int CheckDesktopProcess(
			int pipe_fd,
			Directory *directory,
			ActivityStatus activity);
static void WritePosInfoPipeCallback(
			XtPointer client_data,
			int *fd,
			XtInputId *id);
static void ReaddirPipeCallback(
			XtPointer client_data,
			int *fd,
			XtInputId *id);
static void TimerPipeCallback(
			XtPointer client_data,
			int *fd,
			XtInputId *id);
static void CheckDesktopPipeCallback(
			XtPointer client_data,
			int *fd,
			XtInputId *id);
static void SelectDesktopFile(FileMgrData *fmd);



/*--------------------------------------------------------------------
 * Static Data
 *------------------------------------------------------------------*/

int maxDirectoryProcesses = 10;
int maxRereadProcesses = 5;
int maxRereadProcsPerTick = 1;

XtIntervalId checkBrokenLinkTimerId = None;

static Directory ** directory_set = NULL;
static int          directory_count = 0;
static int          directory_set_size = 0;
static char       * positionFileName = NULL;
static XtAppContext app_context = None;
static int          tickTime = 0;
static int          ticksBetweenLinkChecks = 0;
static Boolean      timer_suspended = False;
static int          tick_count = 0;
static int          lastLinkCheckTick = 0;
static Directory  dummy_dir_struct =
{
  "dummy_host",
  "dummy_directory",
  "dummy_path",
  NULL,
  False,
  activity_idle
};
static Directory *dummy_directory = &dummy_dir_struct;

static struct
{
   DirBackgroundProc main;
   XtInputCallbackProc callback;
   Boolean sticky;
   StickyProcDesc *sticky_procs;
} ActivityTable[] =
{
  { WritePosInfoProcess,  WritePosInfoPipeCallback, False,NULL },/* writing_posinfo*/
  { ReadDirectoryProcess, ReaddirPipeCallback, False,NULL },     /* reading */
  { UpdateAllProcess,     ReaddirPipeCallback, False,NULL },     /* update_all */
  { UpdateSomeProcess,    ReaddirPipeCallback, False,NULL },     /* update_some */
  { TimerEventProcess,    TimerPipeCallback, False,NULL },       /* checking_links */
  { CheckDesktopProcess,  CheckDesktopPipeCallback, False,NULL },/* checking_desktop */
  { TimerEventProcess,    TimerPipeCallback, True, NULL },  /* checking_dir */
  { NULL,                 NULL, False, NULL }               /* idle */
};


/*====================================================================
 *
 * Initialization routines
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  InitializePositionFileName
 *	Initialize the name under which the position info is stored.
 *------------------------------------------------------------------*/

static void
InitializePositionFileName(void)
{
   struct passwd * pwInfo;

   /* Determine the name under which the position info is stored */
   if (positionFileName == NULL)
   {

      pwInfo = getpwuid(getuid());
      positionFileName = XtMalloc(strlen(pwInfo->pw_name) +
                                  strlen(POSITION_FILE_PREFIX) + 1);
      sprintf(positionFileName, "%s%s", POSITION_FILE_PREFIX, pwInfo->pw_name);
   }
}


/*--------------------------------------------------------------------
 *  InitializeDirectoryRead
 *	Set up a timer used to automatically check the read in
 *	directories to see if they have been modified.
 *------------------------------------------------------------------*/

void
InitializeDirectoryRead(
        Widget widget )

{
   /* remeber application context */
   app_context = XtWidgetToApplicationContext(widget);

   /* start timer to check for modified directories and broken links */
   tick_count = lastLinkCheckTick = 0;

   if (rereadTime != 0)
   {
     tickTime = rereadTime;
     ticksBetweenLinkChecks = checkBrokenLink / rereadTime;
   }
   else if (checkBrokenLink != 0)
   {
     tickTime = checkBrokenLink;
     ticksBetweenLinkChecks = 1;
   }
   else
     tickTime = 0;

   if (tickTime != 0)
      XtAppAddTimeOut (app_context, tickTime * 1000, TimerEvent, NULL);

   /* start timer to check for broken desktop objects */
   if( desktop_data->numIconsUsed > 0
       && checkBrokenLink != 0
     )
   {
     checkBrokenLinkTimerId = XtAppAddTimeOut( app_context,
                                               checkBrokenLink * 1000,
                                               TimerEventBrokenLinks,
                                               NULL);
   }
   else
   {
     checkBrokenLinkTimerId = None;
   }
}


/*====================================================================
 *
 * Utiltiy functions
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  FindDirectory
 *    Given a host & directory name, find the directory in our cache.
 *------------------------------------------------------------------*/

static Directory *
FindDirectory(
        char *host_name,
        char *directory_name)
{
   int i;

   /* See if the directory is in the directory set.  First, compare    */
   /* the names from the directory entries ONLY.  There will be one    */
   /* directory entry for every directory of a different name.  This   */
   /* may mean that there is more than one directory entry for a       */
   /* single directory (ie. if there is a directory that is a link or  */
   /* a mount point in the system).                                    */
   /* If this doesn't succeed, we may be getting a ToolTalk resolved   */
   /* name. So run the comparison again but this time compare the      */
   /* incoming name to the tt_path_name in the directory entries.      */
   /* This algorithm has a limitation in that if ToolTalk has resolved */
   /* a path name, the match will occur for the first entry in the     */
   /* directory set whose tt_path_name matches our name, this MAY NOT  */
   /* be the directory where the activity originated.  The user should */
   /* only notice in the case where automatic refresh is turned off.   */
   for (i = 0; i < directory_count; i++)
   {
      if (strcmp (host_name, directory_set[i]->host_name) == 0 &&
          strcmp (directory_name, directory_set[i]->directory_name) == 0)
      {
         return directory_set[i];
      }
   }

   for (i = 0; i < directory_count; i++)
   {
      if (directory_set[i]->tt_path_name != NULL &&
          strcmp (host_name, home_host_name) == 0 &&
          strcmp (directory_name, directory_set[i]->tt_path_name) == 0)
      {
         return directory_set[i];
      }
   }

   /* not found */
   return NULL;
}


/*--------------------------------------------------------------------
 *  DirectoryGone
 *    Check if a directory has been removed from the cache.
 *------------------------------------------------------------------*/

static Boolean
DirectoryGone(
        Directory *directory)
{
   int i;

   for (i = 0; i < directory_count; i++)
      if (directory_set[i] == directory)
         return False;

   return True;
}


/*--------------------------------------------------------------------
 *  FreeFileData
 *	Free FileData structure.
 *------------------------------------------------------------------*/

void
FreeFileData(
        FileData *file_data,
        Boolean free_all)
{
   XtFree(file_data->file_name);
   file_data->file_name = NULL;

   XtFree(file_data->action_name);
   file_data->action_name = NULL;

   DtDtsFreeDataType(file_data->logical_type);
   file_data->logical_type = NULL;

   if ( file_data->final_link != NULL &&
        file_data->final_link != file_data->link)
   {
      XtFree(file_data->final_link);
      file_data->final_link = NULL;
   }

   if (file_data->link != NULL )
   {
     XtFree(file_data->link);
     file_data->link = NULL;
   }

   if (free_all)
      XtFree((char *)file_data);
}


/*--------------------------------------------------------------------
 *  FreeDirectory
 *	Free Directory structure.
 *------------------------------------------------------------------*/

static void
FreeDirectory(
        Directory *directory)
{
   int i;
   FileData *file_data, *next_file_data;

   if( directory == NULL )
     return;

   XtFree (directory->host_name);
   directory->host_name = NULL;

   XtFree (directory->directory_name);
   directory->directory_name = NULL;

   XtFree (directory->path_name);
   directory->path_name = NULL;

   XtFree (directory->tt_path_name);
   directory->tt_path_name = NULL;

   for (i=0; i < directory->path_count; i++)
      DtDtsFreeDataType(directory->path_logical_types[i]);
   XtFree ((char *) directory->path_logical_types);
   directory->path_logical_types = NULL;

   for (i = 0; i < directory->position_count; i++)
      XtFree(directory->position_info[i].name);
   XtFree ((char *) directory->position_info);
   directory->position_info = NULL;

   for (i = 0; i < directory->modified_count; i++)
      XtFree(directory->modified_list[i]);
   XtFree ((char *) directory->modified_list);
   directory->modified_list = NULL;

   XtFree ((char *) directory->directoryView);
   directory->directoryView = NULL;

   if (directory->dir_data)
   {
      FreeFileData(directory->dir_data, True);
      directory->dir_data = NULL;
   }

   file_data = directory->file_data;
   while (file_data != NULL)
   {
      next_file_data = file_data->next;
      FreeFileData(file_data, True);
      file_data = next_file_data;
   }
   directory->file_data = NULL;

   XtFree((char *) directory);
}


/*--------------------------------------------------------------------
 *  SomeWindowMapped
 *    Check if any cached directory is currently being viewed in
 *    a window that is mapped (not iconified).  (If there is none,
 *    we won't need to set a refresh timer).
 *------------------------------------------------------------------*/

static Boolean
SomeWindowMapped(void)

{
   int i, j;

   for (i = 0; i < directory_count; i++)
      for (j = 0; j < directory_set[i]->numOfViews; j++)
         if (directory_set[i]->directoryView[j].mapped)
            return True;

   return False;
}


/*====================================================================
 *
 * Routines for reading a directory
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  PipeWrite...
 *  PipeRead...
 *      Directories are read in a background process that is connected
 *      to the main dtfile process by a pipe.
 *	The routines below are used to send directory entry information
 *      through the pipe from the background to the main process.
 *------------------------------------------------------------------*/

/* write FileData to the pipe */
void
PipeWriteFileData(
	int fd,
	FileData *file_data)
{
   write(fd, file_data, sizeof(FileData));
   PipeWriteString(fd, file_data->file_name);
   PipeWriteString(fd, file_data->action_name);  /* @@@ ??? */
   PipeWriteString(fd, file_data->logical_type);
   if (file_data->link)
      PipeWriteString(fd, file_data->link);
   if (file_data->final_link)
      PipeWriteString(fd, file_data->final_link);
}


/* read FileData from the pipe */
static FileData *
PipeReadFileData(
	int fd)
{
   FileData *file_data;
   int n;

   file_data = (FileData *)XtCalloc(1,sizeof(FileData));
   n = PipeRead(fd, file_data, sizeof(FileData));
   if (n < sizeof(FileData))
   {
      fprintf(stderr, "PipeReadFileData: n = %d, expected %ld\n",
              n, (long)sizeof(FileData));
   }
   file_data->file_name = PipeReadString(fd);
   file_data->action_name = PipeReadString(fd);
   file_data->logical_type = PipeReadString(fd);
   if (file_data->link)
      file_data->link = PipeReadString(fd);
   if (file_data->final_link)
      file_data->final_link = PipeReadString(fd);

   /* return the file data */
   return file_data;
}

FileData *
FileData2toFileData(
	FileData2 *file_data2,
	int *l)
{
   FileData *file_data;
   int n;
   char file_name_buf[MAXPATHLEN];
   char action_name_buf[MAXPATHLEN];
   char logical_type_buf[MAXPATHLEN];
   char link_buf[MAXPATHLEN];
   char final_link_buf[MAXPATHLEN];
   char *textptr = file_data2->text;

   file_data = (FileData *)XtCalloc(1,sizeof(FileData));

   strncpy(file_name_buf, textptr, n = file_data2->file_name);
   file_name_buf[n] = NILL;
   textptr += n;

   strncpy(action_name_buf, textptr, n = file_data2->action_name);
   action_name_buf[n] = NILL;
   textptr += n;

   strncpy(logical_type_buf, textptr, n = file_data2->logical_type);
   logical_type_buf[n] = NILL;
   textptr += n;

   strncpy(link_buf, textptr, n = file_data2->link);
   link_buf[n] = NILL;
   textptr += n;

   strncpy(final_link_buf, textptr, n = file_data2->final_link);
   final_link_buf[n] = NILL;
   textptr += n;

   file_data->next		= NULL;
   file_data->file_name		= XtNewString(file_name_buf);
   file_data->action_name	= file_data2->action_name
				? XtNewString(action_name_buf)
				: NULL;
   file_data->physical_type	= file_data2->physical_type;
   file_data->logical_type	= XtNewString(logical_type_buf);
   file_data->errnum		= file_data2->errnum;
   file_data->stat		= file_data2->stat;

   file_data->link		= file_data2->link
				? XtNewString(link_buf)
				: NULL;

   file_data->final_link	= file_data2->final_link
				? XtNewString(final_link_buf)
				: NULL;
   file_data->is_subdir		= file_data2->is_subdir;
   file_data->is_broken		= file_data2->is_broken;

   *l = sizeof(*file_data2) - sizeof(file_data2->text)
         + file_data2->file_name + file_data2->action_name
         + file_data2->logical_type + file_data2->link
         + file_data2->final_link;

   *l = (*l + sizeof(char *) - 1) & ~(sizeof(char *) - 1);

   /* return the file data */
   return file_data;
}

/* write PositionInfo to the pipe */
void
PipeWritePositionInfo(
	int fd,
	PositionInfo *position_info)
{
   PipeWriteString(fd, position_info->name);
   write(fd, &position_info->x, sizeof(Position));
   write(fd, &position_info->y, sizeof(Position));
   write(fd, &position_info->stacking_order, sizeof(int));
}


/* read PositionInfo from the pipe */
static void
PipeReadPositionInfo(
	int fd,
	PositionInfo *position_info)
{
   position_info->name = PipeReadString(fd);
   PipeRead(fd, &position_info->x, sizeof(Position));
   PipeRead(fd, &position_info->y, sizeof(Position));
   PipeRead(fd, &position_info->stacking_order, sizeof(int));
}

/*--------------------------------------------------------------------
 *  ReadFileData
 *    Given a path name, return FileData for a file.
 *------------------------------------------------------------------*/

FileData *
ReadFileData(
	char *full_directory_name,
	char *file_name)
{
   FileData *file_data;
   char full_file_name[MAX_PATH];
   char link_file_name[MAX_PATH];
   char link_path[MAX_PATH];
   int link_count;
   char ** link_list;
   int link_len;
   char * end;
   char * ptr;
   Boolean recursive_link_found;
   struct stat stat_buf;
   struct stat stat_buf2;
   int stat_result;
   int stat_errno;
   int i;

   /*  Allocate a new file structure.  */
   file_data = (FileData *)XtMalloc(sizeof(FileData));

   /* get the full name of the file */
   strcpy (full_file_name, full_directory_name);
   if (file_name)
   {
      /* append file name to the directory */
      if (strcmp(full_directory_name,"/") != 0)
         strcat (full_file_name, "/");
      strcat (full_file_name, file_name);
   }
   else
   {
      /* no file name passed: use last component of directory */
      file_name = strrchr(full_file_name, '/');
      if (file_name > full_file_name)
         file_name++;
      else
         file_name = NULL;
   }

   /* Follow symbolic links to their ultimate destination */
   link_count = 0;
   link_list = NULL;
   recursive_link_found = False;
   strcpy(link_file_name, full_file_name);

   stat_result = lstat (link_file_name, &stat_buf);
   if (stat_result == 0 && (stat_buf.st_mode & S_IFMT) == S_IFLNK)
   {
     while ((link_len = readlink(link_file_name, link_path, MAX_PATH)) > 0)
     {
       link_path[link_len] = '\0';
       link_list = (char **)XtRealloc((char *)link_list, sizeof(char *) *
                                      (link_count + 2));

       /* Force the link to be an absolute path, if necessary */
       if (link_path[0] != '/')
       {
         /* Relative paths are relative to the current directory */
         end = strrchr(link_file_name, '/') + 1;
         *end = '\0';
         strcat(link_file_name, link_path);
       }
       else
         strcpy(link_file_name, link_path);

       /* Check for a recursive loop; abort if found */
       for (i = 0; i < link_count; i++)
       {
         if (strcmp(link_file_name, link_list[i]) == 0)
         {
           /* Back up to last non-recursive portion */
           strcpy(link_file_name, link_list[link_count - 1]);
           recursive_link_found = True;
           break;
         }
       }

       if (recursive_link_found)
         break;

       link_list[link_count++] = XtNewString(link_file_name);
       link_list[link_count] = NULL;
     }

     /* try to stat the file that the link points to */
     if (stat (link_file_name, &stat_buf2) == 0)
     {
       /* replace lstat result with the stat */
       memcpy(&stat_buf, &stat_buf2, sizeof(struct stat));
     }
   }
   stat_errno = errno;

   /* fill in the FileData structure with the information we found */
   file_data->next = NULL;
   file_data->file_name = XtNewString(file_name? file_name: ".");
   file_data->logical_type = NULL;
   file_data->is_subdir = False;
   file_data->action_name = NULL;

   if (link_list)
   {
      file_data->link = XtNewString( link_list[0] );
      file_data->final_link = XtNewString( link_list[link_count - 1] );
      for (i = 0; i < link_count; i++)
         XtFree(link_list[i]);
      XtFree((char *)link_list);
   } else
      file_data->link = file_data->final_link = NULL;

   if (stat_result == 0)
   {
      file_data->errnum = 0;
      file_data->stat = stat_buf;

      /*  Find and set the physical type of the file  */

      if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
      {
         file_data->physical_type = DtDIRECTORY;
         if (file_name == NULL ||
             strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0)
         {
            file_data->is_subdir = True;
         }
      }
      else if ((stat_buf.st_mode & S_IFMT) == S_IFREG)
      {
         if ((stat_buf.st_mode & S_IXUSR) ||
             (stat_buf.st_mode & S_IXGRP) ||
             (stat_buf.st_mode & S_IXOTH))
            file_data->physical_type = DtEXECUTABLE;
         else
            file_data->physical_type = DtDATA;
      }
      else
         file_data->physical_type = DtDATA;

      /*  Find and set the logical type of the file  */
      if ((stat_buf.st_mode & S_IFMT) == S_IFLNK)
      {
         file_data->is_broken = True;
         if (recursive_link_found)
            file_data->logical_type = XtNewString(LT_RECURSIVE_LINK);
         else
            file_data->logical_type = XtNewString(LT_BROKEN_LINK);
      }
      else
      {
         file_data->is_broken = False;
         if (file_data->link)
            file_data->logical_type = (char *) DtDtsDataToDataType(
                                                 file_data->link,
                                                 NULL, 0, &stat_buf,
                                                 file_data->final_link, NULL,
                                                 NULL);
         else
            file_data->logical_type = (char *) DtDtsDataToDataType(
                                                 full_file_name,
                                                 NULL, 0, &stat_buf,
                                                 NULL, NULL,
                                                 NULL);
#if defined( DATATYPE_IS_FIXED )
#else
         /* The problem here is there isn't a way for user to mask
            only the OWNER READ bit in the MODE field of dtfile.dt file.
            If the MODE field set to d&!r Then all READ permission
            (S_IRUSR, S_IRGRP and S_IROTH)
            bits has to be off in order for the above data typing to work.
            Also data typing is unable to detect when the directory is not
            the owners and only has execute permission by that owner.
            The work around is manually checking it ourselves.
            When the data typing code is fixed, please remove this check.
         */
         if( S_ISDIR( stat_buf.st_mode ) &&
	    (strcmp (file_data->logical_type, LT_DIRECTORY) == 0))
         {
           if( strcmp( file_name, ".." ) != 0
               && strcmp( file_name, "." ) != 0 )
           {
             char * fullPathName;

             if( file_data->link )
               fullPathName = file_data->link;
             else
               fullPathName = full_file_name;

             if( access( fullPathName, R_OK ) != 0 )
             {
               XtFree( file_data->logical_type );
               file_data->logical_type = XtNewString( LT_FOLDER_LOCK );
             }
             else if( access( fullPathName, W_OK ) != 0 )
             {
               XtFree( file_data->logical_type );
               file_data->logical_type = XtNewString( LT_NON_WRITABLE_FOLDER );
             }
           }
         }
#endif
      }

      if(DtActionExists(file_data->logical_type))
      {
         file_data->action_name = (char *)DtActionLabel(file_data->file_name);
      }
      else
      {
         char *ptr = DtDtsDataTypeToAttributeValue(file_data->logical_type,
                                                   DtDTS_DA_LABEL,
                                                   NULL);
         if (ptr)
         {
            file_data->action_name = XtNewString(ptr);
            DtDtsFreeAttributeValue(ptr);
         }
      }
   }
   else
   {
      /* couldn't stat the file */
      file_data->errnum = stat_errno;
      memset(&file_data->stat, 0, sizeof(file_data->stat));
      file_data->physical_type = DtUNKNOWN;
      file_data->is_broken = True;
      file_data->logical_type = XtNewString(DtDEFAULT_DATA_FT_NAME);
   }

   return file_data;
}


/*--------------------------------------------------------------------
 *  ReadFileData2
 *    Given a path name, return FileData for a file.
 *------------------------------------------------------------------*/

int
ReadFileData2(
	FileData2 *file_data2,
	char *full_directory_name,
	char *file_name,
        Boolean IsToolBox)
{
   char full_file_name[MAX_PATH];
   char link_file_name[MAX_PATH];
   char link_path[MAX_PATH];
   char file_name_buf[MAXPATHLEN];
   char action_name_buf[MAXPATHLEN];
   char logical_type_buf[MAXPATHLEN];
   char link_buf[MAXPATHLEN];
   char final_link_buf[MAXPATHLEN];
   int link_count;
   char ** link_list;
   int link_len;
   char * end;
   char * ptr;
   Boolean recursive_link_found;
   struct stat stat_buf;
   int stat_result;
   int stat_errno;
   int i;

   /*  Allocate a new file structure.  */

   /* get the full name of the file */
   strcpy (full_file_name, full_directory_name);

   if (file_name)
   {
      /* append file name to the directory */
      if (strcmp(full_directory_name,"/") != 0)
         strcat (full_file_name, "/");
      strcat (full_file_name, file_name);
   }
   else
   {
      /* no file name passed: use last component of directory */
      file_name = strrchr(full_file_name, '/');
      if (file_name > full_file_name)
         file_name++;
      else
         file_name = NULL;
   }

   /* Follow symbolic links to their ultimate destination */
   link_count = 0;
   link_list = NULL;
   recursive_link_found = False;
   strcpy(link_file_name, full_file_name);

   stat_result = lstat (link_file_name, &stat_buf);
   if ((stat_buf.st_mode & S_IFMT) == S_IFLNK)
   {
     while ((link_len = readlink(link_file_name, link_path, MAX_PATH)) > 0)
     {
       link_path[link_len] = NILL;
       link_list = (char **)XtRealloc((char *)link_list, sizeof(char *) *
                                      (link_count + 2));

       /* Force the link to be an absolute path, if necessary */
       if (link_path[0] != '/')
       {
         /* Relative paths are relative to the current directory */
         end = strrchr(link_file_name, '/') + 1;
         *end = '\0';
         strcat(link_file_name, link_path);
       }
       else
         strcpy(link_file_name, link_path);

       /* Check for a recursive loop; abort if found */
       for (i = 0; i < link_count; i++)
       {
         if (strcmp(link_file_name, link_list[i]) == 0)
         {
           /* Back up to last non-recursive portion */
           strcpy(link_file_name, link_list[link_count - 1]);
           recursive_link_found = True;
           break;
         }
       }

       if (recursive_link_found)
         break;

       link_list[link_count++] = XtNewString(link_file_name);
       link_list[link_count] = NULL;
     }

     if ((stat_result = stat (link_file_name, &stat_buf)) != 0)
     {
      /* probably a broken link; try lstat */
       stat_result = lstat (full_file_name, &stat_buf);
       strcpy(link_file_name, full_file_name);
     }
   }
   stat_errno = errno;

   /* fill in the FileData2 structure with the information we found */
   file_data2->next = NULL;
   strcpy(file_name_buf, (file_name ? file_name : "."));
   logical_type_buf[0] = NILL;
   file_data2->is_subdir = False;
   action_name_buf[0] = NILL;

   if (link_list)
   {
      strcpy(link_buf, link_list[0]);
      strcpy(final_link_buf, link_list[link_count - 1]);
      for (i = 0; i < link_count; i++) {
         XtFree(link_list[i]);
      }
      XtFree((char *)link_list);
   } else {
      final_link_buf[0] = NILL;
      link_buf[0] = NILL;
   }

   if (stat_result == 0)
   {
      file_data2->errnum = 0;
      file_data2->stat = stat_buf;

      /*  Find and set the physical type of the file  */

      if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
      {
         file_data2->physical_type = DtDIRECTORY;
         if (file_name == NULL ||
             strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0)
         {
            file_data2->is_subdir = True;
         }
      }
      else if ((stat_buf.st_mode & S_IFMT) == S_IFREG)
      {
         if ((stat_buf.st_mode & S_IXUSR) ||
             (stat_buf.st_mode & S_IXGRP) ||
             (stat_buf.st_mode & S_IXOTH))
            file_data2->physical_type = DtEXECUTABLE;
         else
            file_data2->physical_type = DtDATA;
      }
      else
         file_data2->physical_type = DtDATA;

      /*  Find and set the logical type of the file  */
      if ((stat_buf.st_mode & S_IFMT) == S_IFLNK)
      {
         file_data2->is_broken = True;
         if (recursive_link_found)
            strcpy(logical_type_buf, LT_RECURSIVE_LINK);
         else
            strcpy(logical_type_buf, LT_BROKEN_LINK);
      }
      else
      {
         char *ptr;

         file_data2->is_broken = False;
         if (link_buf[0] == NILL)
            ptr = (char *) DtDtsDataToDataType(  full_file_name,
                                                 NULL, 0, &stat_buf,
                                                 NULL, NULL,
                                                 NULL);
         else
            ptr = (char *) DtDtsDataToDataType(  link_buf,
                                                 NULL, 0, &stat_buf,
                                                 final_link_buf, NULL,
                                                 NULL);

#if defined( DATATYPE_IS_FIXED )
         strcpy(logical_type_buf, ptr);
         free(ptr);
#else
         /* The problem here is there isn't a way for user to mask
            only the OWNER READ bit in the MODE field of dtfile.dt file.
            If the MODE field set to d&!r Then all READ permission
            (S_IRUSR, S_IRGRP and S_IROTH)
            bits has to be off in order for the above data typing to work.
            Also data typing is unable to detect when the directory is not
            the owners and only has execute permission by that owner.
            The work around is manually checking it ourselves.
            When the data typing code is fixed, please remove this check.
         */
         if( !IsToolBox && S_ISDIR( stat_buf.st_mode ) &&
	    (strcmp (ptr, LT_DIRECTORY) == 0))
         {
           if( strcmp( file_name, ".." ) != 0
               && strcmp( file_name, "." ) != 0 )
           {
             char * fullPathName;

             if( link_buf[0] == NILL )
               fullPathName = full_file_name;
             else
               fullPathName = link_buf;

             if( access( fullPathName, R_OK ) != 0 )
             {
               free( ptr ); /* Don't use XtFree. This pointer is being kept by tooltalk */
               strcpy( logical_type_buf, LT_FOLDER_LOCK );
             }
             else if( access( fullPathName, W_OK ) != 0 )
             {
               free( ptr ); /* Don't use XtFree. This pointer is being kept by tooltalk */
               strcpy( logical_type_buf, LT_NON_WRITABLE_FOLDER );
             }
             else
             {
               strcpy( logical_type_buf, ptr );
               free( ptr ); /* Don't use XtFree. This pointer is being kept by tooltalk */
             }
           }
           else
           {
             strcpy( logical_type_buf, ptr );
             free( ptr ); /* Don't use XtFree. This pointer is being kept by tooltalk */
           }
         }
         else
         {
           strcpy( logical_type_buf, ptr );
           free( ptr ); /* Don't use XtFree. This pointer is being kept by tooltalk */
         }
#endif
      }

      if( DtActionExists(logical_type_buf) )
      {
        char *ptr = (char *)DtActionLabel(file_name_buf);
        strcpy(action_name_buf, ptr);
        free(ptr);
      }
      else
      {
        char *ptr = DtDtsDataTypeToAttributeValue(logical_type_buf,
                                                  DtDTS_DA_LABEL,
                                                  NULL);
        if (ptr)
        {
          strcpy(action_name_buf, ptr);
          DtDtsFreeAttributeValue(ptr);
        }
      }
   }
   else
   {
      /* couldn't stat the file */
      file_data2->errnum = stat_errno;
      memset(&file_data2->stat, 0, sizeof(file_data2->stat));
      file_data2->physical_type = DtUNKNOWN;
      file_data2->is_broken = True;
      strcpy(logical_type_buf, DtDEFAULT_DATA_FT_NAME);
   }

	strcpy(file_data2->text, file_name_buf);
	file_data2->file_name = strlen(file_name_buf);

	strcat(file_data2->text, action_name_buf);
	file_data2->action_name = strlen(action_name_buf);

	strcat(file_data2->text, logical_type_buf);
	file_data2->logical_type = strlen(logical_type_buf);

	strcat(file_data2->text, link_buf);
	file_data2->link = strlen(link_buf);

	strcat(file_data2->text, final_link_buf);
	file_data2->final_link = strlen(final_link_buf);

	i = sizeof(*file_data2) - sizeof(file_data2->text)
		+ file_data2->file_name + file_data2->action_name
		+ file_data2->logical_type + file_data2->link
		+ file_data2->final_link;

	i = (i + sizeof(char *) - 1) & ~(sizeof(char *) - 1);

	/*
	 * This data marshalling operation relies on char[BUFSIZ]
	 * being large enough for all the text pieces.  However,
	 * BUFSIZ has nothing to do with the above operations so
	 * we'll do this assert for now.
	 */
	assert( (i <= sizeof(FileData2)) );

	return i;
}

/*--------------------------------------------------------------------
 *  GetTTPath
 *      Resolves the links in the path.
 *------------------------------------------------------------------*/

char *
GetTTPath(char *path)
{
   Tt_message dummy_msg;
   char *tmp, *tt_path;
   Tt_status tt_status;

   dummy_msg = tt_message_create();
   tt_status = tt_message_file_set(dummy_msg, path);
   tmp = tt_message_file(dummy_msg);

   tt_path = XtNewString(tmp);

   tt_free(tmp);
   tt_message_destroy(dummy_msg);

   return tt_path;
}


static int
ReadDirectoryProcess(
        int pipe_fd,
        Directory *directory,
	ActivityStatus activity)
{
#ifdef DT_PERFORMANCE
   struct timeval update_time_s;
   struct timeval update_time_f;
#endif
   char *host_name = directory->host_name;
   char *directory_name = directory->directory_name;
   struct stat stat_buf;
   long modify_time;
   int path_count;
   char **path_logical_types;
   char *full_directory_name;
   char *tt_path;
   DIR *dirp;
   struct dirent * dp;
   Boolean inDtDir;
   FileData *file_data;
   FileData2 file_data2;
   Boolean done;
   Boolean update_due;
   short file_data_count = 0;
   int i;
   char * ptr;
   char * namePtr;
   char file_name[MAX_PATH];
   int position_count;
   FILE * fptr;
   int x, y, stacking_order;
   short pipe_msg;
   int rc;
   char file_data_buffer[FILEDATABUF * sizeof(FileData2)];
   char *file_data_buf_ptr = file_data_buffer;
   char *file_data_count_ptr;
   struct timeval time1, time2;
   long diff;
   char *ptrOrig;

   DPRINTF(("ReadDirectoryProcess(%d, \"%s\", \"%s\")\n",
            pipe_fd, host_name, directory_name));

   /*
    * Get the logical data type of all components of the path;
    * We need only the last path component for (1) the tree root icon,
    * and (2) the current directory icon; the other path components
    * are needed for the iconic path icons.
    */
   path_count = 0;
   path_logical_types = NULL;

   /* Don't muck with original string */
   ptrOrig = ptr = XtNewString(directory_name);

   for (;;)
   {
      Tt_status tt_status;

      if (ptr != NULL)
         *ptr = '\0';

      if (ptrOrig[0] == '\0')
         namePtr = "/";
      else
         namePtr = ptrOrig;

      /* get logical type of next path component */
      full_directory_name = ResolveLocalPathName( host_name,
                                                  namePtr,
                                                  NULL,
                                                  home_host_name,
                                                  &tt_status );
      if( TT_OK != tt_status )
        break;

      DtEliminateDots (full_directory_name);
      path_logical_types = (char **) XtRealloc((char *)path_logical_types,
                                               (path_count + 1)*sizeof(char *));
      path_logical_types[path_count] =
         (char *) DtDtsDataToDataType(full_directory_name, NULL, 0, NULL, NULL,
                                      NULL, NULL);
#if defined( DATATYPE_IS_FIXED )
#else
      {
        if( stat( full_directory_name, &stat_buf ) == 0 )
        {
          if( S_ISDIR( stat_buf.st_mode ) &&
	    (strcmp (path_logical_types[path_count], LT_DIRECTORY) == 0))
          {
            if( access( full_directory_name, R_OK ) != 0 )
            {
              XtFree( path_logical_types[path_count] );
              path_logical_types[path_count] = XtNewString( LT_FOLDER_LOCK );
            }
            else if( access( full_directory_name, W_OK ) != 0 )
            {
              XtFree( path_logical_types[path_count] );
              path_logical_types[path_count] = XtNewString( LT_NON_WRITABLE_FOLDER );
            }
          }
        }
      }
#endif
      DPRINTF2(("ReadDirectoryProcess: path '%s', fullname '%s', type %s\n",
                namePtr, full_directory_name, path_logical_types[path_count]));

      XtFree( full_directory_name );
      path_count++;

      if (ptr == NULL)
        break;

      /* restore '/' */
      *ptr = '/';

      /* find next component */
      if (strcmp(ptr, "/") == 0)
         break;
      ptr = DtStrchr(ptr + 1, '/');
   }
   XtFree(ptrOrig);

   /* get the full name of the current directory */
   {
     Tt_status tt_status;
     full_directory_name = ResolveLocalPathName( host_name,
                                                 directory_name,
                                                 NULL,
                                                 home_host_name,
                                                 &tt_status );
     /* It's ok not to check for tt_status.
        The code below will handle it properly.
     */
   }

   /* send the path_logical_types back through the pipe */
   pipe_msg = PIPEMSG_PATH_LOGICAL_TYPES;
   DPRINTF(("ReadDirectoryProcess: sending %d path_logical_types\n",
            path_count));
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &path_count, sizeof(int));
   for(i = 0; i < path_count; i++)
   {
     PipeWriteString(pipe_fd, path_logical_types[i]);
     XtFree((char *) path_logical_types[i]);
   }
   XtFree((char *) path_logical_types);

   /* send the tt_path */
   tt_path = GetTTPath(full_directory_name);
   PipeWriteString(pipe_fd, tt_path);
   XtFree(tt_path);

   /*
    * Stat the directory to get its timestamp.
    * Also check if we have read and execute/search permisssion.
    */
   if (CheckAccess(full_directory_name, R_OK | X_OK) != 0 ||
       stat(full_directory_name, &stat_buf) != 0)
   {
      /* send an error code back through the pipe */
      pipe_msg = PIPEMSG_ERROR;
      rc = errno;
      modify_time = 0;
      DPRINTF(("ReadDirectoryProcess: sending errno %d (stat failed)\n", rc));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &rc, sizeof(int));
      write(pipe_fd, &modify_time, sizeof(long));
      return 1;
   }

   modify_time = stat_buf.st_mtime;

   /*
    * We never want to display the '~/.dt/Desktop' directory, so when we
    * are working with the .dt directory, add a special check for a
    * directory named 'Desktop'.
    */
   if ((ptr = strrchr(full_directory_name, '/')) &&
       (strcmp(ptr, "/.dt") == 0))
      inDtDir = True;
   else
      inDtDir = False;

   /* try to open the directory */
   dirp = opendir (full_directory_name);
   if (dirp == NULL)
   {
      /* send an error code back through the pipe */
      pipe_msg = PIPEMSG_ERROR;
      rc = errno;
      DPRINTF(("ReadDirectoryProcess: sending errno %d (opendir failed)\n",
               rc));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &rc, sizeof(int));
      write(pipe_fd, &modify_time, sizeof(long));
      XtFree( full_directory_name );
      return 1;
   }

   /*  Loop through the directory entries and update the file list  */

#ifdef DT_PERFORMANCE
   printf("  begin reading directory: %s\n", full_directory_name);
   gettimeofday(&update_time_s, NULL);
#endif

	/*
	 *	FILEDATA3 creates a buffer of static FileData2 structures,
	 *	then sends FILEDATABUF worth of FileData2 structs to the parent.
	 *	FILEDATABUF appears to work the best when set to 50.
     *
     *  We send data to the parent at least every half seconds, even if
     *  less than FILEDATABUF worth of FileData2 structs have been read.
     *  This is to ensure that the file count in the status line gets
     *  updated every half seconds, even if the file system is slow.
	 */

   /* initialize pointer into file data buffer */
#  define PIPEMSG_HDR_LEN (2*sizeof(short) + sizeof(int))
   file_data_buf_ptr = file_data_buffer + PIPEMSG_HDR_LEN;

   /* get current time */
   gettimeofday(&time1, NULL);

   done = False;
   do
   {
     int len = 0;

     if ((dp = readdir (dirp)) != NULL)
     {
       Boolean IsToolBox;
       /* if Desktop skip */
       if (inDtDir && (strcmp(dp->d_name, "Desktop") == 0))
         continue;

       /* get the info */
       if(directory->directoryView && directory->directoryView->file_mgr_data)
	 IsToolBox = directory->directoryView->file_mgr_data->toolbox;
       else
	 IsToolBox = False;
       len = ReadFileData2((FileData2 *)file_data_buf_ptr,
                           full_directory_name, dp->d_name,IsToolBox);
       file_data_buf_ptr += len;
       file_data_count++;
     }
     else
       done = True;

     /* check if 0.4 seconds have passed since the last status line update */
     gettimeofday(&time2, NULL);
     diff = 1024*(time2.tv_sec - time1.tv_sec);
     diff += time2.tv_usec/1024;
     diff -= time1.tv_usec/1024;
     update_due = (diff >= 400);

     /* check if we need to send the buffered data now */
     if (file_data_count == FILEDATABUF ||
         file_data_count > 0 && (done || update_due))
     {
       if (update_due)
         file_data_count |= 0x8000;
       len = file_data_buf_ptr - (file_data_buffer + PIPEMSG_HDR_LEN);

       /* now send the file data through the pipe */
       *(short *)file_data_buffer = PIPEMSG_FILEDATA3;
       *(short *)(file_data_buffer + sizeof(short)) = file_data_count;
       *(int *)(file_data_buffer + 2*sizeof(short)) = len;
       write(pipe_fd, file_data_buffer,
             file_data_buf_ptr - file_data_buffer);

       /* reset pointer to file data buffer, file count and time stamp */
       file_data_buf_ptr = file_data_buffer + PIPEMSG_HDR_LEN;
       file_data_count = 0;
       if (update_due)
         time1 = time2;
     }
   } while (!done);

#ifdef DT_PERFORMANCE
   gettimeofday(&update_time_f, NULL);
   if (update_time_s.tv_usec > update_time_f.tv_usec) {
      update_time_f.tv_usec += 1000000;
      update_time_f.tv_sec--;
   }
   printf("    finished reading: %s, time: %ld.%ld\n\n", full_directory_name, update_time_f.tv_sec - update_time_s.tv_sec, update_time_f.tv_usec - update_time_s.tv_usec);
#endif


   /* load position info, if available */

   /* construct full name of the position info file */
   if (strcmp(full_directory_name,"/") != 0)
     sprintf( file_name, "%s/%s", full_directory_name, positionFileName );
   else
     sprintf( file_name, "%s%s", full_directory_name, positionFileName );

   /* read the count from the position info file */
   position_count = 0;
   if ((fptr = fopen(file_name, "r")) != NULL)
   {
     PositionInfo * position_info;
     fscanf(fptr, "%d\n", &position_count);

     if (position_count > 0)
     {
       /* allocate position info array */
       position_info = (PositionInfo *)
         XtMalloc(position_count * sizeof(PositionInfo));

       /* read the position info from the file */
       i = 0;
       while (i < position_count)
       {
         if( fgets( file_name, MAX_PATH, fptr ) != NULL &&
             fscanf(fptr, "%d %d %d\n", &x, &y, &stacking_order ) == 3  )
         {
           int len = strlen(file_name);
           file_name[len-1] = 0x0;
           position_info[i].name = XtNewString(file_name);
           position_info[i].x = x,
           position_info[i].y = y,
           position_info[i].stacking_order = stacking_order;
           i++;
         }
         else
           break;
       }
       position_count = i;
     }

     fclose(fptr);

     /* send the position info back through the pipe */
     pipe_msg = PIPEMSG_POSITION_INFO;
     DPRINTF(("ReadDirectoryProcess: sending %d position_info\n",
              position_count));
     write(pipe_fd, &pipe_msg, sizeof(short));
     write(pipe_fd, &position_count, sizeof(int));
     for (i = 0; i < position_count; i++)
     {
       PipeWriteString( pipe_fd, position_info[i].name );
       XtFree( position_info[i].name );
       write( pipe_fd, &(position_info[i].x), sizeof(Position));
       write( pipe_fd, &(position_info[i].y), sizeof(Position));
       write( pipe_fd, &(position_info[i].stacking_order), sizeof(int));

     }
     XtFree( (char *)position_info );
   }

   XtFree(full_directory_name);
   closedir (dirp);

   /* send a 'done' msg through the pipe */
   DPRINTF(("ReadDirectoryProcess: sending DONE\n"));
   pipe_msg = PIPEMSG_DONE;
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &modify_time, sizeof(long));
   return 0;
}


/*--------------------------------------------------------------------
 *  UpdateAllProcess
 *    Main routine of the background process that checks the directory
 *    for new files or files that have disapeared.
 *------------------------------------------------------------------*/

static int
UpdateAllProcess(
                 int pipe_fd,
                 Directory *directory,
                 ActivityStatus activity)
{
   char *host_name = directory->host_name;
   char *directory_name = directory->directory_name;
   char *full_directory_name;
   struct stat stat_buf;
   long modify_time;
   DIR *dirp;
   struct dirent * dp;
   Boolean inDtDir;
   FileData *file_data;
   FileData *old_data, **old_pp;
   FileData2 file_data2;
   char *ptr;
   short pipe_msg;
   int n, i, rc=0;
   Tt_status tt_status;
   char **path_logical_types;
   int path_count;
   char *ptrOrig;

   DPRINTF(("UpdateAllProcess(%d, \"%s\", \"%s\")\n",
            pipe_fd, host_name, directory_name));

   /* if modified list contains "." or ".." arrange for the path_logical_types
      to get updated */
   if(directory->modified_count == 0 &&
      FindDirectory(directory->host_name, directory_name))
   {
       rc = 1;
   }
   else
   {
       for (i = 0; i < directory->modified_count; i++)
       {
           if (strcmp(directory->modified_list[i],".") == 0 ||
               strcmp(directory->modified_list[i],"..") == 0 )
           {
               rc = 1;
               break;
           }
       }
   }
   if(rc)
   {
      char *tt_path;

      path_count = 0;
      path_logical_types = NULL;
      ptrOrig = ptr = XtNewString(directory_name);
      for (;;)
      {
         Tt_status tt_status;
         char *namePtr;

         if (ptr != NULL)
            *ptr = '\0';

         if (ptrOrig[0] == '\0')
            namePtr = "/";
         else
            namePtr = ptrOrig;

         /* get logical type of next path component */
         full_directory_name = ResolveLocalPathName( host_name,
                                                  namePtr,
                                                  NULL,
                                                  home_host_name,
                                                  &tt_status );
         if( TT_OK != tt_status )
           break;

         DtEliminateDots (full_directory_name);
         path_logical_types = (char **) XtRealloc((char *)path_logical_types,
                                               (path_count + 1)*sizeof(char *));
         path_logical_types[path_count] =
           (char *)DtDtsDataToDataType(full_directory_name, NULL, 0, NULL, NULL,
                                      NULL, NULL);
#if defined( DATATYPE_IS_FIXED )
#else
      {
        if( stat( full_directory_name, &stat_buf ) == 0 )
        {
          if( S_ISDIR( stat_buf.st_mode ) &&
	    (strcmp (path_logical_types[path_count], LT_DIRECTORY) == 0))
          {
            if( access( full_directory_name, R_OK ) != 0 )
            {
              XtFree( path_logical_types[path_count] );
              path_logical_types[path_count] = XtNewString( LT_FOLDER_LOCK );
            }
            else if( access( full_directory_name, W_OK ) != 0 )
            {
              XtFree( path_logical_types[path_count] );
              path_logical_types[path_count] = XtNewString( LT_NON_WRITABLE_FOLDER );
            }
          }
        }
      }
#endif

         DPRINTF2(("ReadDirectoryProcess: path '%s', fullname '%s', type %s\n",
                namePtr, full_directory_name, path_logical_types[path_count]));

         XtFree(full_directory_name);
         path_count++;

         if (ptr == NULL)
           break;

         /* restore '/' */
         *ptr = '/';

         /* find next component */
         if (strcmp(ptr, "/") == 0)
            break;
         ptr = DtStrchr(ptr + 1, '/');
      }
      XtFree(ptrOrig);

      /* get the full name of the current directory */
      full_directory_name = ResolveLocalPathName( host_name,
                                                 directory_name,
                                                 NULL,
                                                 home_host_name,
                                                 &tt_status );
      /* It's ok not to check for tt_status.
         The code below will handle it properly.
      */

      /* send the path_logical_types back through the pipe */
      pipe_msg = PIPEMSG_PATH_LOGICAL_TYPES;
      DPRINTF(("ReadDirectoryProcess: sending %d path_logical_types\n",
            path_count));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &path_count, sizeof(int));
      for(i = 0; i < path_count; i++)
      {
        PipeWriteString(pipe_fd, path_logical_types[i]);
        XtFree((char *) path_logical_types[i]);
      }
      XtFree((char *) path_logical_types);

      /* send the tt_path */
      tt_path = GetTTPath(full_directory_name);
      PipeWriteString(pipe_fd, tt_path);
      XtFree(tt_path);
   }
   else
   {
        full_directory_name = ResolveLocalPathName( host_name,
                                                 directory_name,
                                                 NULL,
                                                 home_host_name,
                                                 &tt_status );
   }

   if( TT_OK != tt_status )
   {
      pipe_msg = PIPEMSG_ERROR;
      rc = -1;
      modify_time = 0;
      DPRINTF(("UpdateAllProcess: sending errno %d (tooltalk failed)\n", rc));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &rc, sizeof(int));
      write(pipe_fd, &modify_time, sizeof(long));
      return 1;
   }
   (void) DtEliminateDots (full_directory_name);

   /*
    * Stat the directory to get its timestamp.
    * Also check if we still have read and execute/search permisssion.
    */
   if (CheckAccess(full_directory_name, R_OK | X_OK) != 0 ||
       stat(full_directory_name, &stat_buf) != 0)
   {
      /* send an error code back through the pipe */
      pipe_msg = PIPEMSG_ERROR;
      rc = errno;
      modify_time = 0;
      DPRINTF(("UpdateAllProcess: sending errno %d (stat failed)\n", rc));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &rc, sizeof(int));
      write(pipe_fd, &modify_time, sizeof(long));
      XtFree( full_directory_name );
      return 1;
   }
   modify_time = stat_buf.st_mtime;

   /* check if we are in the .dt directory */
   if ((ptr = strrchr(full_directory_name, '/')) &&
       (strcmp(ptr, "/.dt") == 0))
   {
      inDtDir = True;
   }
   else
      inDtDir = False;

   /* try to open the directory */
   dirp = opendir (full_directory_name);
   if (dirp == NULL)
   {
      /* send an error code back through the pipe */
      pipe_msg = PIPEMSG_ERROR;
      rc = errno;
      DPRINTF(("UpdateAllProcess: sending errno %d (opendir failed)\n", rc));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &rc, sizeof(int));
      write(pipe_fd, &modify_time, sizeof(long));
      XtFree( full_directory_name );
      return 1;
   }

   /*  Loop through the directory entries and update the file list  */
   while (dp = readdir (dirp))
   {
      /* if Desktop skip */
      if (inDtDir && (strcmp(dp->d_name, "Desktop") == 0))
         continue;

      /* check if we already know this file */
      for (old_pp = &directory->file_data;
           (old_data = *old_pp) != NULL;
           old_pp = &old_data->next)
      {
         if (strcmp(dp->d_name, old_data->file_name) == 0)
         {
             char *tname;
             struct stat sbuf;
 
             /* check modified times */
             tname = XtMalloc(strlen(full_directory_name)+strlen(dp->d_name)+2);
             sprintf(tname,"%s/%s",full_directory_name,dp->d_name);
             if((lstat(tname,&sbuf)>=0)&&sbuf.st_mtime!=old_data->stat.st_mtime)
                 old_data = NULL;
             XtFree(tname);
             if(old_data == NULL)
                 break;

            /* check if this file appears on the modified list */
            for (i = 0; i < directory->modified_count; i++)
               if (strcmp(dp->d_name, directory->modified_list[i]) == 0)
               {
                 /*
                  * This file is on the modified list.
                  * Pretend we didn't find it to force a refresh of this file.
                  */
                 old_data = NULL;
                 break;
               }
            break;
         }
      }

      /* If this is a known file, remember we saw it and continue. */
      if (old_data != NULL)
      {
         /*
          * We remove the file from the old file list; thus, when we are done,
          * the files on the old file list will be onew that no longer exist.
          */
         *old_pp = old_data->next;
         continue;
      }

      /* this is a new file */
      DPRINTF(("UpdateAllProcess: found new file \"%s\"\n", dp->d_name));

      /* Fix for incorrect icons in App Manager */
      {
	Boolean IsToolBox;

        if(directory->directoryView && directory->directoryView->file_mgr_data)
          IsToolBox = directory->directoryView->file_mgr_data->toolbox;
        else
          IsToolBox = False;
        ReadFileData2(&file_data2, full_directory_name, dp->d_name,IsToolBox);
      }
      file_data = FileData2toFileData(&file_data2, &n);

      /* now send the file data through the pipe */
      pipe_msg = PIPEMSG_FILEDATA;
      write(pipe_fd, &pipe_msg, sizeof(short));
      PipeWriteFileData(pipe_fd, file_data);

      FreeFileData(file_data, True);
   }

   /* all files left in the old file list no longer exist */
   for (old_data = directory->file_data;
        old_data;
        old_data = old_data->next)
   {
      DPRINTF(("UpdateAllProcess: file gone \"%s\"\n", old_data->file_name));
      old_data->errnum = ENOENT;
      pipe_msg = PIPEMSG_FILEDATA;
      write(pipe_fd, &pipe_msg, sizeof(short));
      PipeWriteFileData(pipe_fd, old_data);
   }

   /* free storage */
   XtFree(full_directory_name);

   /* send a 'done' msg through the pipe */
   DPRINTF(("UpdateAllProcess: sending DONE\n"));
   pipe_msg = PIPEMSG_DONE;
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &modify_time, sizeof(long));
   return 0;
}


/*--------------------------------------------------------------------
 *  UpdateSomeProcess
 *    Main routine of the background process that updates a selected
 *    list of directory entries.
 *------------------------------------------------------------------*/

static int
UpdateSomeProcess(
        int pipe_fd,
        Directory *directory,
	ActivityStatus activity)
{
   char *host_name = directory->host_name;
   char *directory_name = directory->directory_name;
   char *full_directory_name;
   struct stat stat_buf;
   long modify_time;
   FileData *file_data;
   FileData2 file_data2;
   short pipe_msg;
   int i;
   int rc;
   Boolean IsToolBox;

   DPRINTF(("UpdateSomeProcess(%d, \"%s\", \"%s\")\n",
            pipe_fd, host_name, directory_name));


   /* get the full name of the current directory */
   {
     Tt_status tt_status;

     full_directory_name = ResolveLocalPathName( host_name,
                                                 directory_name,
                                                 NULL,
                                                 home_host_name,
                                                 &tt_status );
     if( TT_OK != tt_status )
     {
       pipe_msg = PIPEMSG_ERROR;
       rc = -1;
       modify_time = 0;
       DPRINTF(("UpdateSomeProcess: sending errno %d (stat failed)\n", rc));
       write(pipe_fd, &pipe_msg, sizeof(short));
       write(pipe_fd, &rc, sizeof(int));
       write(pipe_fd, &modify_time, sizeof(long));
       return 1;
     }
   }
   (void) DtEliminateDots (full_directory_name);

   /* stat the directory to get the timestamp */
   if (stat(full_directory_name, &stat_buf) < 0 ||
       ! (stat_buf.st_mode & S_IXUSR) )
   {
      /* send an error code back through the pipe */
      pipe_msg = PIPEMSG_ERROR;
      rc = errno;
      modify_time = 0;
      DPRINTF(("UpdateSomeProcess: sending errno %d (stat failed)\n", rc));
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &rc, sizeof(int));
      write(pipe_fd, &modify_time, sizeof(long));
      XtFree( full_directory_name );
      return 1;
   }
   modify_time = stat_buf.st_mtime;

   /*  Loop through the list of modified files  */
   for (i = 0; i < directory->modified_count; i++)
   {
      /* get the info */

      if(directory->directoryView && directory->directoryView->file_mgr_data)
        IsToolBox = directory->directoryView->file_mgr_data->toolbox;
      else
        IsToolBox = False;

      ReadFileData2(&file_data2, full_directory_name,
                               directory->modified_list[i],IsToolBox);

      /* now send the file data through the pipe */
      pipe_msg = PIPEMSG_FILEDATA2;
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &file_data2, sizeof(FileData2));
   }

   XtFree(full_directory_name);

   /* send a 'done' msg through the pipe */
   DPRINTF(("UpdateSomeProcess: sending DONE\n"));
   pipe_msg = PIPEMSG_DONE;
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &modify_time, sizeof(long));
   return 0;
}


/*--------------------------------------------------------------------
 *  ReaddirPipeCallback
 *	Callback routine that reads directory entry information sent
 *	through the pipe from the background process.
 *------------------------------------------------------------------*/

static void
ReaddirPipeCallback(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   static int whined_fd = 0;
   PipeCallbackData *pipe_data = (PipeCallbackData *)client_data;
   Directory *directory = pipe_data->directory;
   FileMgrData *file_mgr_data;
   FileMgrRec *file_mgr_rec;
   ActivityStatus activity;
   Boolean done;
   short msg;
   Boolean update_due;
   FileData *new_data, **new_nextp;
   FileData *old_data, **old_nextp;
   char *ptr;
   char *err_msg;
   int i, n;
   int rc;
   long modify_time;
   char dirname[MAX_PATH];
   short file_data_count;


   /* verify that the directory still exists */
   if (DirectoryGone(directory))
   {
      /*
       * The directory is no longer present:
       * close the pipe and kill the reader.
       */
      close(*fd);
      XtRemoveInput(*id);
      kill(pipe_data->child, SIGKILL);
      XtFree( client_data );
      ScheduleActivity(NULL);
      return;
   }

   /* read the next msg from the pipe */
   msg = -1;
   n = PipeRead(*fd, &msg, sizeof(short));
   activity = directory->activity;
   done = False;

   switch (msg)
   {
      case PIPEMSG_PATH_LOGICAL_TYPES:
         /* get the number of path components */
         PipeRead(*fd, &n, sizeof(int));

         /* free logical types */
         for (i = 0; i < directory->path_count; i++)
           XtFree(directory->path_logical_types[i]);

         /* allocate array of the right size */
         if (directory->path_count != n)
         {
            directory->path_count = n;
            directory->path_logical_types = (char **)
              XtRealloc((char *)directory->path_logical_types,
                        n*sizeof(char *));
         }

         /* get new logical types */
         for (i = 0; i < directory->path_count; i++)
           directory->path_logical_types[i] = PipeReadString(*fd);

         /* get the tt_path */
         XtFree(directory->tt_path_name);
         directory->tt_path_name = PipeReadString(*fd);

         /* update all views */
         for (i = 0; i < directory->numOfViews; i++)
         {
            file_mgr_data = directory->directoryView[i].file_mgr_data;
            file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
            UpdateHeaders(file_mgr_rec, file_mgr_data, True);
            XmUpdateDisplay(file_mgr_rec->file_window);
         }
         XSync(XtDisplay(toplevel), False);
         break;

          case PIPEMSG_FILEDATA:
          case PIPEMSG_FILEDATA2:
          case PIPEMSG_FILEDATA3:

         if (msg == PIPEMSG_FILEDATA)
         {
           new_data = PipeReadFileData(*fd);
         }
         else if (msg == PIPEMSG_FILEDATA2)
         {
           FileData2 file_data2;
           int n;

           n = PipeRead(*fd, &file_data2, sizeof(FileData2));
           if (n < sizeof(FileData2)) {
             perror("PipeRead");
             fprintf(stderr, "PipeReadFileData2: n = %d, expected %ld\n",
                     n, (long)sizeof(FileData2));
           }

           new_data = FileData2toFileData(&file_data2, &n);
           }

         if (msg == PIPEMSG_FILEDATA3)
         {
           int file_data_length;
           int n;
           char file_data_buffer[FILEDATABUF * sizeof(FileData2)];
           char *file_data_buf_ptr;


           n = PipeRead(*fd, &file_data_count, sizeof(short));
           n = PipeRead(*fd, &file_data_length, sizeof(int));

           if (file_data_count & 0x8000)
           {
             file_data_count &= 0x7fff;
             update_due = True;
           }
           else
             update_due = False;

           for (new_nextp = &directory->new_data;
                *new_nextp;
                new_nextp = &(*new_nextp)->next)
             ;

           n = PipeRead(*fd, file_data_buffer, file_data_length);
           file_data_buf_ptr = file_data_buffer;

           for (i = 0; i < file_data_count; i++)
           {
             /* get next FileData out of buffer */
             new_data =
               FileData2toFileData((FileData2 *)file_data_buf_ptr, &n);
             file_data_buf_ptr += n;

             /* append new_data to end of list */
             *new_nextp = new_data;
             new_data->next = NULL;
             new_nextp = &new_data->next;
           }
         }
         else
         {
           /* append new_data to end of list */
           file_data_count = 1;
           update_due = False;
           for (new_nextp = &directory->new_data;
                *new_nextp;
                new_nextp = &(*new_nextp)->next)
             ;
           *new_nextp = new_data;
           new_data->next = NULL;
         }

         if (activity == activity_reading)
         {
           /* update file counts in all views */
           for (i = 0; i < directory->numOfViews; i++)
           {
             file_mgr_data = directory->directoryView[i].file_mgr_data;
             file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
             file_mgr_data->busy_detail += file_data_count;
             if (update_due &&
                 (file_mgr_data->busy_status == initiating_readdir ||
                  file_mgr_data->busy_status == busy_readdir) &&
                 file_mgr_data->busy_detail > 2)
             {
               if (file_mgr_data->show_status_line)
               {
                 char buf[256];
                 XmString label_string;
                 Arg args[2];

                 GetStatusMsg(file_mgr_data, buf);
                 label_string =
                   XmStringCreateLocalized(buf);
                 XtSetArg (args[0], XmNlabelString, label_string);
                 XtSetValues(file_mgr_rec->status_line, args, 1);
                 XmStringFree(label_string);
               }
               else if (file_mgr_data->show_iconic_path)
               {
                 DtUpdateIconicPath(file_mgr_rec, file_mgr_data, False);
               }
               else if (file_mgr_data->show_current_dir)
               {
                 DrawCurrentDirectory(file_mgr_rec->current_directory,
                                      file_mgr_rec, file_mgr_data);
               }
             }
           }
         }
         break;

      case PIPEMSG_POSITION_INFO:
         /* free old position info names */
         for (i = 0; i < directory->position_count; i++)
            XtFree(directory->position_info[i].name);

         /* get number of positions and realloc array, if necessary */
         PipeRead(*fd, &n, sizeof(int));
         if (directory->position_count != n)
         {
            directory->position_count = n;
            directory->position_info = (PositionInfo *) XtRealloc(
                     (char *)directory->position_info, n*sizeof(PositionInfo));
         }

         /* read new position info */
         for (i = 0; i < n; i++)
            PipeReadPositionInfo(*fd, &directory->position_info[i]);
         break;

      case PIPEMSG_DONE:
         PipeRead(*fd, &modify_time, sizeof(long));
         directory->errnum = 0;
         directory->errmsg_needed = False;
         done = True;
         break;

      case PIPEMSG_ERROR:
         PipeRead(*fd, &rc, sizeof(int));
         PipeRead(*fd, &modify_time, sizeof(long));
         if (rc != directory->errnum)
         {
            directory->errnum = rc;
            directory->errmsg_needed = True;
         }
         done = True;
         break;

      default:
	 if (whined_fd != *fd)
	 {
	     whined_fd = *fd;
             fprintf(stderr,
	       "ReaddirPipeCallback: badmsg=%d, ppid=%d pid=%d fd=%d activ'y=%d\n",
	       msg, getppid(), getpid(), *fd, activity);
	 }
         directory->errnum = -1;
         directory->errmsg_needed = False;
         done = True;
   }

   /* check if we are done */
   if (done)
   {
#ifdef DT_PERFORMANCE
   /* Aloke Gupta: As suggested by Dana Dao */
      _DtPerfChkpntMsgSend("Done  Read Directory");
#endif
      DPRINTF(("ReaddirPipeCallback: done, errno %d, time %ld\n",
               directory->errnum, modify_time));

      /* close the pipe and cancel the callback */
      close(*fd);
      XtRemoveInput(*id);

      /*
       * @@@ what if a drag is active ???
       */

      /*
       * For files in the old list that still exist in the new
       * one we need to re-use the old FileData structures.
       * Reason: the code in GetFileData relies on this to
       * preserve the position_info and selection list.
       * The following loops through the new list of files
       * and replaces entries that also exist in the old list.
       */
      for (new_nextp = &directory->new_data;
           (new_data = *new_nextp) != NULL;
           new_nextp = &new_data->next)
      {
         for (old_nextp = &directory->file_data;
              (old_data = *old_nextp) != NULL;
              old_nextp = &old_data->next)
         {
           if( strcmp(old_data->file_name, new_data->file_name) == 0 )
            {
               *old_nextp = old_data->next;

               FreeFileData(old_data, False);
               memcpy(old_data, new_data, sizeof(FileData));

               XtFree((char *)new_data);
               *new_nextp = new_data = old_data;

               break;
            }
         }
      }

      /*
       * If this was a complete re-read, we free all FileData still left
       * in the old list.  Otherwise, if this was just a partial update,
       * we append the old data that's still left to the end of the
       * new list.
       */
      if (activity == activity_reading)
      {
         /* This was a complete re-read: free all old data still left. */
         while (directory->file_data)
         {
            old_data = directory->file_data;
            directory->file_data = old_data->next;
            FreeFileData(old_data, True);
         }

         /* replace the old list by the new list */
         directory->file_data = directory->new_data;
         directory->new_data = NULL;
      }
      else
      {
         FileData * tmp_ptr = NULL;

         /* remove any directory entries that no longer exist
            in the new list.
         */
         new_nextp = &directory->new_data;
         while ((new_data = *new_nextp) != NULL)
         {
            if (new_data->errnum == ENOENT)
            {
               *new_nextp = new_data->next;
               FreeFileData(new_data, True);
            }
            else
            {
               tmp_ptr = *new_nextp;
               new_nextp = &new_data->next;
            }
         }

         /* remove any directory entries that no longer exist
            in the old list.
         */
         old_nextp = &directory->file_data;
         while ((old_data = *old_nextp) != NULL)
         {
            if (old_data->errnum == ENOENT)
            {
               *old_nextp = old_data->next;
               FreeFileData(old_data, True);
            }
            else
               old_nextp = &old_data->next;
         }

         /* Append the old list to the end of the new list
            Replace the old list pointer with the new list pointer
         */
         if( tmp_ptr != NULL )
         {
            tmp_ptr->next = directory->file_data;
            directory->file_data = directory->new_data;
            directory->new_data = NULL;
         }
      }

      /* update the file count */
      directory->file_count = 0;
      for (new_data = directory->file_data; new_data; new_data = new_data->next)
         directory->file_count++;

      /* update directory timestamp */
      if (activity == activity_reading ||
          activity == activity_update_all ||
          directory->was_up_to_date)
      {
         if (modify_time != 0)
            directory->modify_time = modify_time;
      }

      /* flush the icon cache */  /* @@@ Why? What does this do? @@@ */
      strcpy (dirname, directory->path_name);
      DtEliminateDots(dirname);
      /* We will not attempt to flush the icon cache until this */
      /* function has been fixed.                               */

      _DtFlushIconFileCache(dirname);

      /* reset busy flags */
      directory->busy[activity] = False;
      directory->activity = activity_idle;
      directory->was_up_to_date = True;
      directory->link_check_needed = False;


      /*
       * Fill dir_data field with information on the directory itself.
       * This data will be read when querying this view's top directory,
       * if the parent directory isn't already cached (tree mode)
       */
      for (new_data = directory->file_data;
           new_data != NULL;
           new_data = new_data->next)
        if (strcmp(new_data->file_name, ".") == 0)
        {

	    /*
	     * Found current directory information, now we make
	     * dir_data info from "." info
	     */

	    /* If we already have allocated space for dir_data free it */
	    if ( directory->dir_data != NULL )
	      FreeFileData(directory->dir_data, True);

	    directory->dir_data = (FileData *)XtMalloc(sizeof(FileData));

	    memcpy(directory->dir_data, new_data, sizeof(FileData));

	    /*
	     * Doctor up some of the information fields so that this doesn't
	     * seem to be a "." entry
	     */
	    directory->dir_data->next = NULL;
	    directory->dir_data->file_name =
	                         XtNewString(DName(directory->directory_name));
	    directory->dir_data->action_name = NULL;
	    if (directory->path_count > 0)
	    {
	      directory->dir_data->logical_type = XtNewString(
	             directory->path_logical_types[directory->path_count - 1]);
	    }
	    else
	      directory->dir_data->logical_type = NULL;
	    directory->dir_data->link = NULL;
	    directory->dir_data->final_link = NULL;
	    directory->dir_data->is_subdir = True;

	    break;
        }

      /* cause all views on this directory to be redrawn */
      for (i = 0; i < directory->numOfViews; i++)
      {
         file_mgr_data = directory->directoryView[i].file_mgr_data;
         file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
         FileMgrRedisplayFiles(file_mgr_rec, file_mgr_data, False);
 	 if(file_mgr_data->desktop_file)
 	 {
             SelectDesktopFile(file_mgr_data);
             XtFree(file_mgr_data->desktop_file);
             file_mgr_data->desktop_file = NULL;
         }
      }
      XtFree(client_data);

      /* schedule the next background activity */
      ScheduleActivity(directory);
   }
}


/*--------------------------------------------------------------------
 *  ReadDirectoryFiles
 *    This routine is called to read a directory if the directory
 *    wasn't found in the cached, or if the directory has to be
 *    re-read because it changed.  This routine schedules a background
 *    process to be started that will do the actual work.
 *------------------------------------------------------------------*/

static void
ReadDirectoryFiles(
        Widget w,
        Directory *directory)
{
   FileMgrData *file_mgr_data;
   FileMgrRec *file_mgr_rec;
   int i;

#ifdef DT_PERFORMANCE
   /* Aloke Gupta */
      _DtPerfChkpntMsgSend("Begin Read Directory");
#endif

   /* make sure positionFileName is initialized */
   if (positionFileName == NULL)
      InitializePositionFileName();

   /* mark the directory busy reading */
   directory->busy[activity_reading] = True;

   /* arrange for background process to be started */
   ScheduleActivity(directory);

   /* make sure all views on this directory are marked busy */
   for (i = 0; i < directory->numOfViews; i++)
   {
      file_mgr_data = directory->directoryView[i].file_mgr_data;
      if (file_mgr_data->busy_status == not_busy)
      {
         file_mgr_data->busy_status = busy_readdir;
         file_mgr_data->busy_detail = 0;
         file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
         FileMgrRedisplayFiles(file_mgr_rec, file_mgr_data, False);
      }
      file_mgr_data->busy_status = busy_readdir;
   }
   return;
}


/*--------------------------------------------------------------------
 *  ReadDirectory
 *    Given a directory name, see if the directory is already cached.
 *    If so, return the file data list, otherwise, read the directory.
 *------------------------------------------------------------------*/

static Boolean
ReadDirectory(
        Widget w,
        char *host_name,
        char *directory_name,
        FileData **file_data,
        int *file_count,
        FileMgrData *file_mgr_data)
{
   Directory *directory;
   int i;
   char *err_msg;

   /* initialize return values */
   if (file_data != NULL)
   {
      *file_count = 0;
      *file_data = NULL;
   }

   /* see if the directory is already in the cache */
   directory = FindDirectory(host_name, directory_name);

   if ((directory != NULL) &&
       (strcmp(directory_name, directory->directory_name) == 0))
   {
      /* The directory is already in the cache. */
      directory->viewed = True;

      /* Look for the view in the view list */
      for (i = 0; i < directory->numOfViews; i++)
         if (directory->directoryView[i].file_mgr_data == file_mgr_data)
            break;

      /* If view not found, add to the view list */
      if (i == directory->numOfViews)
      {
         directory->directoryView = (DirectoryView *)
                                 XtRealloc ((char *) directory->directoryView,
                                            sizeof(DirectoryView) * (i + 1));
         directory->numOfViews++;
         directory->directoryView[i].file_mgr_data = file_mgr_data;
      }

      /* set mapped flag for the view */
      directory->directoryView[i].mapped = file_mgr_data->mapped;

      /* check if we need to popup an error message */
      if (directory->errmsg_needed &&
          !directory->busy[activity_reading] &&
          w != NULL)
      {
         err_msg = XtNewString(GetSharedMessage(CANNOT_READ_DIRECTORY_ERROR));
         FileOperationError (w, err_msg, directory_name);
         XtFree(err_msg);
         directory->errmsg_needed = False;
      }

      DPRINTF2(("ReadDirectory(\"%s\", \"%s\") returns cached\n",
                host_name, directory_name));
   }

   else
   {
      Tt_status tt_status;

      /* The directory is not yet in the cache. */

      /*  Expand the directory set array, if necessary.  */
      if (directory_count == directory_set_size)
      {
         directory_set_size += 10;
         directory_set = (Directory **) XtRealloc((char *)directory_set,
                                    sizeof(Directory **) * directory_set_size);
      }


      /*  Create and initialize a new directory entry  */
      directory_set[directory_count] = directory =
                                  (Directory *) XtMalloc (sizeof (Directory));
      directory_count++;

      directory->host_name = XtNewString (host_name);
      directory->directory_name = XtNewString (directory_name);
      directory->path_name = ResolveLocalPathName (host_name,
                                                   directory_name,
                                                   NULL,
                                                   home_host_name,
                                                   &tt_status );
      if (directory->path_name == NULL)
      {
         directory->path_name = (char *) XtMalloc(sizeof(char));
         directory->path_name[0]='\0';
      }
      directory->tt_path_name = NULL;
      directory->viewed = True;
      directory->file_count = 0;
      directory->numOfViews = 1;
      directory->errnum = 0;
      directory->errmsg_needed = False;
      directory->last_check = 0;
      directory->link_check_needed = False;
      directory->file_count = 0;
      directory->file_data = NULL;
      directory->new_data = NULL;
      directory->dir_data = NULL;
      directory->path_count = 0;
      directory->path_logical_types = NULL;
      directory->position_count = 0;
      directory->position_info = NULL;
      directory->modify_begin = 0;
      directory->modified_count = 0;
      directory->was_up_to_date = True;
      directory->modified_list = NULL;
      directory->activity = activity_idle;
      for (i = 0; i < activity_idle; i++)
        directory->busy[i] = False;

      directory->directoryView = (DirectoryView *)
                                       XtMalloc (sizeof(DirectoryView));
      directory->directoryView[0].file_mgr_data = file_mgr_data;
      directory->directoryView[0].mapped = file_mgr_data->mapped;

      /*  Open the directory for reading and read the files.  */
      ReadDirectoryFiles (w, directory);
   }

   /* Restart refresh timer, if necessary */
   if (file_mgr_data->mapped && timer_suspended)
   {
      XtAppAddTimeOut(app_context, tickTime * 1000, TimerEvent, NULL);
      timer_suspended = False;
   }

   /* return the file data */
   if (file_data != NULL && !directory->busy[activity_reading])
   {
      *file_count = directory->file_count;
      *file_data = directory->file_data;
   }

   return directory->busy[activity_reading];
}


/*--------------------------------------------------------------------
 *  _ReadDir
 *    Internal routine that recursively read a directory plus
 *    subdirectories down to a depth given by read_level.
 *------------------------------------------------------------------*/

static int
_ReadDir(
  Widget w,
  FileMgrData *file_mgr_data,
  char *host_name,
  char *directory_name,
  FileViewData *dp,         /* directory info */
  int level,                /* tree level of this directory */
  int read_level,           /* deepest level to be read */
  char **branch_list)       /* list of tree branches to expand */
/*
 * Recursively read a directory plus subdirectories down to a depth
 * given by read_level.
 */
{
  char subdir_name[MAX_PATH];
  FileData *fp, *file_data;
  FileViewData **lp, *ip;
  int i, j, n, rc;
  TreeShow ts;
  Boolean busy_reading;

  DPRINTF2(("_ReadDir(\"%s\", \"%s\"): level %d, read_level %d\n",
            host_name, directory_name, level, read_level));

  /* initialize list of descendents and counts */
  if (dp)
  {
    dp->desc = NULL;
    dp->ndir = dp->nfile = 0;
    lp = &dp->desc;
  } else
    ip = NULL;

  /* Read the directory content */
  busy_reading = ReadDirectory(w, host_name, directory_name,
		               &file_data, &n, file_mgr_data);
  if (busy_reading)
  {
    file_mgr_data->busy_status = busy_readdir;
    return 0;
  }

  if (n <= 0)
    return -1;

  level++;

  for (i = 0, fp = file_data; i < n && fp; i++, fp = fp->next) {

    /* initialize new dir entry */
    if (dp)
    {
       ip = (FileViewData *)XtMalloc(sizeof(FileViewData));
       memset(ip, 0, sizeof(FileViewData));
       ip->file_data = fp;
       ip->parent = dp;
       ip->ts = tsNotRead;
    }

    /* read subdirectory */
    if (fp->is_subdir)
    {
      /* construct sub directory name */
      strcpy(subdir_name, directory_name);
      if (subdir_name[strlen(subdir_name) - 1] != '/')
        strcat(subdir_name, "/");
      strcat(subdir_name, fp->file_name);

      /* see if we know this entry from branch_list */
      if (!QueryBranchList(file_mgr_data, branch_list, subdir_name, &ts))
        /* not known: assume we shouldn't read this subdir */
        ts = tsNotRead;

      if (level < read_level || ts != tsNotRead) {

        rc = _ReadDir(w, file_mgr_data, host_name, subdir_name, ip,
                      level, read_level, branch_list);
        if (ip == NULL)
         ;
        else if (rc)
          ip->ts = tsError;
        else if (ts >= tsReading)
          ip->ts = ts;
        else if (level >= file_mgr_data->tree_show_level)
          ip->ts = tsNone;
        else if (file_mgr_data->tree_files == TREE_FILES_ALWAYS)
          ip->ts = tsAll;
        else
          ip->ts = tsDirs;
      }
    }

    /* add new entry to linked list */
    if (dp)
    {
      *lp = ip;
      lp = &ip->next;
    }
  }

  return 0;
}


/*--------------------------------------------------------------------
 *  ReadDir
 *    This is the main external entry point for reading directories.
 *------------------------------------------------------------------*/

int
ReadDir(
  Widget w,
  FileMgrData *file_mgr_data,
  char *host_name,
  char *directory_name,
  FileViewData *dp,         /* directory info */
  int level,                /* tree level of this directory */
  int read_level,           /* deepest level to be read */
  char **branch_list)       /* list of tree branches to expand */
{
   /* initially assume we are not busy */
   if (file_mgr_data->busy_status == not_busy)
      file_mgr_data->busy_detail = 0;

   file_mgr_data->busy_status = initiating_readdir;

   /* first pass: just check if any directory we need is busy */
   _ReadDir(w, file_mgr_data, host_name, directory_name, NULL, level,
            read_level, branch_list);

   /* if a directory we need is busy, return now */
   if (file_mgr_data->busy_status == busy_readdir)
      return 0;

   /*
    * All directories wee need are available.
    * Make a second pass for real.
    */
   file_mgr_data->busy_status = not_busy;
   return _ReadDir(w, file_mgr_data, host_name, directory_name, dp, level,
                   read_level, branch_list);
}


/*====================================================================
 *
 *  Routines that update the directory cache
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  FileWindowMapUnmap
 *    Update mapped flag in view lists.
 *------------------------------------------------------------------*/

void
FileWindowMapUnmap(
        FileMgrData *file_mgr_data)

{
   int i, j;

   for (i = 0; i < directory_count; i++)
   {
      for (j = 0; j < directory_set[i]->numOfViews; j++)
      {
         if (file_mgr_data == directory_set[i]->directoryView[j].file_mgr_data)
         {
            directory_set[i]->directoryView[j].mapped = file_mgr_data->mapped;
            break;
         }
      }
   }

   if (file_mgr_data->mapped && timer_suspended)
   {
      XtAppAddTimeOut(app_context, tickTime * 1000, TimerEvent, NULL);
      timer_suspended = False;
   }
}


/*--------------------------------------------------------------------
 *  RereadDirectory
 *    Read a directory already cached and update its contents.
 *------------------------------------------------------------------*/

void
RereadDirectory(
        Widget w,
        char *host_name,
        char *directory_name )
{
   Directory *directory;

   DPRINTF(("RereadDirectory(%s, %s)\n", host_name, directory_name));

   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);
   if (directory != NULL)
   {
      /* reset errnum to make sure we'll get an error message */
      directory->errnum = 0;

      /* Read the directory. */
      if (!directory->busy[activity_reading])
         ReadDirectoryFiles(w, directory);
   }
}


/*--------------------------------------------------------------------
 *  UpdateDirectory
 *    Check if any files were added or deleted in a directory
 *    and update the directory contents accordingly.
 *------------------------------------------------------------------*/

void
UpdateDirectory(
        Widget w,
        char *host_name,
        char *directory_name )
{
   Directory *directory;

   DPRINTF(("UpdateDirectory(%s, %s)\n", host_name, directory_name));

   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);
   if (directory != NULL)
   {
      /* arrange for directory contents to be checked */
      if (!directory->busy[activity_update_all])
      {
         directory->busy[activity_update_all] = True;
         ScheduleActivity(directory);
      }
   }
}


/*====================================================================
 *
 *  Directory modification routines:
 *
 *  The following routines are provided to avoid unnecessary
 *  re-reads of whole directories.  For example, if the user
 *  renames a file, it's only necessary to remove the old file
 *  from the directory and add it back under its new name; there
 *  is no need to read the whole directory again.  Similarly,
 *  when a file is dropped on a directory, it's only necessary
 *  to add the one new file to the directory.
 *
 *  To accomplish this, the routines that rename or copy files
 *  make the following calls:
 *
 *    DirectoryBeginModify():  called before doing the operation
 *    DirectoryFileModified(): called once for each affected file
 *    DirectoryEndModify():    called when the operation is completed
 *
 *  The routines remember which files were modified, and when
 *  DirectoryEndModify is called, a background process is started,
 *  that re-stats and types just those files.
 *
 *  A complication arises from automatic re-reads triggered by
 *  a periodic timer (routine TimerEvent).  Since renaming or
 *  copying files changes the timestamp on the directory, the
 *  automatic re-read would re-read the whole directory soon after
 *  the operation is done, nullifying our efforts to avoid
 *  unnecessary re-reads.  Therefore:
 *
 *    - We don't do any automatic re-reads between calls to
 *      DirectoryBeginModify and DirectoryEndModify.
 *
 *    - If the directory timestamp hadn't changed at the time
 *      of the DirectoryBeginModify, then when the directory
 *      update triggered by DirectoryEndModify finishes, we
 *      set the modify_time in the directory_set to the current
 *      timestamp of the directory.  This means that the next
 *      automatic re-read won't be triggered unless the directory
 *      is modified again after the DirectoryEndModify.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  DirectoryAbortModify
 *    Decrement the modify_begin counter.
 *------------------------------------------------------------------*/

void
DirectoryAbortModify(
        char *host_name,
        char *directory_name)
{
   Directory *directory;

   DPRINTF(("DirectoryAbortModify(%s, %s)\n", host_name, directory_name));

   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);
   if (directory != NULL)
   {
      directory->modify_begin--;

      if (directory->modify_begin == 0)
         directory->was_up_to_date = True;

      DPRINTF(("   modify_begin %d, up_to_date %d\n",
               directory->modify_begin, directory->was_up_to_date));
   }
}


/*--------------------------------------------------------------------
 *  DirectoryBeginModify
 *    Increment the modify_begin counter to suspend automatic
 *    re-reads until DirectoryEndModify is called.
 *------------------------------------------------------------------*/

void
DirectoryBeginModify(
        char *host_name,
        char *directory_name)
{
   Directory *directory;

   DPRINTF(("DirectoryBeginModify(%s, %s)\n", host_name, directory_name));

   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);
   if (directory != NULL)
   {
      if (directory->modify_begin == 0)
         /* until we know better, assume the directory changed */
         directory->was_up_to_date = False;

      /* increment the modify_begin counter */
      directory->modify_begin++;

      DPRINTF(("   modify_begin %d, up_to_date %d\n",
               directory->modify_begin, directory->was_up_to_date));
   }
}


/*--------------------------------------------------------------------
 *  DirectoryModifyTime
 *    This routine should be called after DirectoryBeginModify and
 *    before doing any operation on the directory.  The parameter
 *    modify_time should be the current timestamp of the directory.
 *    By comparing the value to the modify_time stored in the
 *    directory set we decide whether the directory had already
 *    changed before the update operation began.
 *    Note: the reason for supplying a separate call for this check,
 *    instead of doing it inside DirectoryBeginModify(), is that we
 *    want to do the stat call that determines the current timestamp
 *    of the directory in a background process.  The background
 *    process that we start fo do the actual update is a convenient
 *    place to do this.
 *------------------------------------------------------------------*/

void
DirectoryModifyTime(
        char *host_name,
        char *directory_name,
        long modify_time)
{
   Directory *directory;

   DPRINTF(("DirectoryModifyTime(%s, %s)\n", host_name, directory_name));

#ifdef SMART_DIR_UPDATE
   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);

   if (directory != NULL)
   {
      /* mark directory up-to-date if unchanged since last read */
      if (modify_time <= directory->modify_time)
         directory->was_up_to_date = True;
      DPRINTF(("   modify_begin %d, up_to_date %d\n",
               directory->modify_begin, directory->was_up_to_date));
   }
#endif
}


/*--------------------------------------------------------------------
 *  DirectoryFileModified
 *    This routine is called when we know that a file in a directory
 *    has been modified, added or removed.  The file name is added
 *    to the list of modified files.  The next time an update
 *    background process is started, it will check all the files
 *    on the modfied list and update the corresponding FileData.
 *------------------------------------------------------------------*/

void
DirectoryFileModified(
        char *host_name,
        char *directory_name,
        char *file_name)
{
   Directory *directory;
   int i;

   DPRINTF(("DirectoryFileModified(%s, %s, %s)\n",
            host_name, directory_name, file_name));

   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);
   if (directory != NULL)
   {
      /* see if the file is already on the list */
      for( i = 0; i < directory->modified_count; ++i )
        if( strcmp( directory->modified_list[i], file_name ) == 0 )
          return;

      /* add the file to the modified_list */
      i = directory->modified_count++;
      directory->modified_list = (char **)
        XtRealloc((char *)directory->modified_list, (i + 1)*sizeof(char *));
      directory->modified_list[i] = XtNewString(file_name);
   }
}


/*--------------------------------------------------------------------
 *  DirectoryEndModify
 *    Start an update background process (will check all the files
 *    on the modfied list and update the corresponding FileData).
 *------------------------------------------------------------------*/

void
DirectoryEndModify(
        char *host_name,
        char *directory_name)
{
   Directory *directory;

   DPRINTF(("DirectoryEndModify(%s, %s)\n", host_name, directory_name));

   /*  Find the directory set entry.  */
   directory = FindDirectory(host_name, directory_name);

   /* arrange for an update background process to be scheduled */
   if (directory != NULL)
   {
      directory->modify_begin--;
      DPRINTF(("   modify_begin %d, up_to_date %d, modified_count %d\n",
               directory->modify_begin,
               directory->was_up_to_date,
               directory->modified_count));
      if (directory->modified_count > 0)
      {
         Directory *subdir;
         char subdir_name[MAX_PATH + 1];
         char *p;
         int i;

         /*
          * If any of the modifed files is a subdirectory that we have
          * cached, schedule an activity_checking_dir to make sure that
          * the subdirectory is still readable.
          */
         strcpy(subdir_name, directory_name);
         p = subdir_name + strlen(subdir_name);
         if (p[-1] != '/')
           *p++ = '/';

         for (i = 0; i < directory->modified_count; i++)
         {
            strcpy(p, directory->modified_list[i]);
            subdir = FindDirectory(host_name, subdir_name);
            if (subdir)
            {
               DPRINTF(("   schedule check for subdir \"%s\"\n",
                        directory->modified_list[i]));
               subdir->busy[activity_checking_dir] = True;
               ScheduleActivity(subdir);
            }
         }

#ifdef SMART_DIR_UPDATE
         /* schedule a partial update of the modfied directory */
         if (directory->was_up_to_date)
            directory->busy[activity_update_some] = True;
         else
            directory->busy[activity_update_all] = True;
#else
         /* schedule a full update of the modfied directory */
         directory->busy[activity_update_all] = True;
#endif
         ScheduleActivity(directory);
      }
   }
}


/*--------------------------------------------------------------------
 *  UpdateDirectorySet
 *    We call this when we do a database update. It loops through
 *    the directory_set list and rereads each directory.
 *------------------------------------------------------------------*/

void
UpdateDirectorySet( void )
{
   int i;

   DPRINTF(("UpdateDirectorySet ...\n"));

   for (i = 0; i < directory_count; i++)
      if (!directory_set[i]->busy[activity_reading])
         ReadDirectoryFiles (NULL, directory_set[i]);
}



/*--------------------------------------------------------------------
 *  UpdateCachedDirectories
 *    Update view list for all cached directories.
 *    Throw out any directories that are no longer being viewed.
 *------------------------------------------------------------------*/

void
UpdateCachedDirectories(
        View **view_set,
        int view_count)
{
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   int i, j, k, n;
   Directory *directory;

   /*
    * First step:
    *   clear the view list in all directory set entries
    */
   for (i = 0; i < directory_count; i++)
   {
      if( !(strcmp(directory_set[i]->directory_name, trash_dir) == 0) )
      {
         XtFree ((char *) directory_set[i]->directoryView);
         directory_set[i]->numOfViews = 0;
         directory_set[i]->directoryView = NULL;
         directory_set[i]->viewed = False;
      }
   }


   /*
    * Second step:
    *   reconstruct view lists by adding each directory found in the view
    *   set to the view list for the corresponding directory set entry
    */
   for (j = 0; j < view_count; j++)
   {
      dialog_data = (DialogData *) view_set[j]->dialog_data;
      file_mgr_data = (FileMgrData *) dialog_data->data;

      /* loop through all direcories in this view */
      for (k = 0; k < file_mgr_data->directory_count; k++)
      {
         /* find the directory in the directory set */
         directory = FindDirectory(view_set[j]->host_name,
                                   file_mgr_data->directory_set[k]->name);

         /* we expect the directory to be found; if not, something is wrong */
         if (directory == NULL)
         {
            fprintf(stderr, "Warning: %s:%s not found in directory set.\n",
                    view_set[j]->host_name,
                    file_mgr_data->directory_set[k]->name);
            continue;
         }

         /* add the directory to the view list */
         n = directory->numOfViews;
         directory->directoryView = (DirectoryView *)
                                  XtRealloc ((char *) directory->directoryView,
                                             sizeof(DirectoryView) * (n + 1));
         directory->directoryView[n].file_mgr_data = file_mgr_data;
         directory->directoryView[n].mapped = file_mgr_data->mapped;
         directory->numOfViews++;
         directory->viewed = True;
      }
   }


   /*
    * Third step:
    *   remove all directories that have empty view lists
    */
   i = 0;
   while (i < directory_count)
   {
      if (directory_set[i]->numOfViews > 0 ||
          strcmp(directory_set[i]->directory_name, trash_dir) == 0)
      {
         /* Keep this directory in the directory set. */
         i++;
      }
      else
      {
         /* Delete the file data and remove from the directory set. */

         DPRINTF(("UpdateCachedDirectories: removing %s:%s\n",
                  directory_set[i]->host_name,
                  directory_set[i]->directory_name));

         FreeDirectory(directory_set[i]);

         for (k = i; k < directory_count - 1; k++)
            directory_set[k] = directory_set[k + 1];

         directory_count--;
      }
   }

   /* Restart refresh timer, if necessary */
   if (timer_suspended && SomeWindowMapped())
   {
      XtAppAddTimeOut(app_context, tickTime * 1000, TimerEvent, NULL);
      timer_suspended = False;
   }
}


/*====================================================================
 *
 * Routines that return directory data
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * GetLongName
 *   Return a string that contains file information similar to "ls -l",
 *   including: permissions, owner, modified time, link (if any).
 *   Used for "view by attributes"
 *
 *   Example:
 *     -rw-r--r--  dld  staff  108314  Jul 26 15:16:36 1993 Directory.c
 *
 *------------------------------------------------------------------*/

char *
GetLongName(
        FileData *file_data )
{
#ifdef NLS16
   struct tm * tms;
   char time_string[100];
#else
   char * time_string;
#endif /* NLS16 */
   char link_path[MAX_PATH + 5];
   static gid_t group_id = (gid_t)-1;
   static uid_t user_id = (uid_t)-1;
   struct group * group_data;
   struct passwd * user_data;
   static char group_name[20];
   static char user_name[20];
   char * long_name;
   time_t long_modify_time;
   char permission;
   char usr_read_priv, usr_write_priv, usr_exec_priv;
   char grp_read_priv, grp_write_priv, grp_exec_priv;
   char oth_read_priv, oth_write_priv, oth_exec_priv;

   /*  Generate the long list name.  */
   long_name = (char *) XtMalloc(sizeof(char) * (MAX_PATH * 3));
   long_name[0]='\0';

   /* Initially, assume their is not a soft link */
   link_path[0] = '\0';

   if (file_data->errnum == 0)
   {
     if (file_data->stat.st_gid != group_id)
     {
       group_id = file_data->stat.st_gid;
       group_data = getgrgid (file_data->stat.st_gid);
       if (group_data)
       {
         strcpy (group_name, group_data->gr_name);
         if (strlen (group_name) == 0)
            strcpy (group_name, "root");
       }
       else
         strcpy (group_name, "root");
     }

     if (file_data->stat.st_uid != user_id)
     {
       user_id = file_data->stat.st_uid;
       user_data = getpwuid (file_data->stat.st_uid);
       /* Initially, assume their is not a user name */
       user_name[0] = '\0';
       if (user_data)
         strcpy (user_name, user_data->pw_name);
       else
         sprintf(user_name,"%ld",(long)user_id);
     }
   }
   else
   {
      char error_msg[1024];
      int msg_len;

      /* determine how much space we have for an error message */
      long_modify_time = 747616435;
                         /* just needed to determine the length of a date */
#ifdef NLS16
      tms = localtime(&long_modify_time);
      strftime( time_string, 100,
                GetSharedMessage(DIRECTORY_DATE_FORMAT),
                tms);
#else
      time_string = ctime ((time_t *)&long_modify_time);
      time_string[strlen(time_string)-1] = 0x0;
      time_string += 4;
#endif
      msg_len = 10 + 3 + 9 + 1 + 9 + 1 + 9 + 1 + strlen(time_string);

      /* generate the error message */
      strcpy(error_msg, "(");
      strncpy(error_msg + 1, strerror(file_data->errnum), msg_len - 2);
      error_msg[msg_len - 1] = '\0';
      strcat(error_msg, ")");

      sprintf( long_name, "%-28.28s  %s  %9d %s",
                            file_data->file_name,
                            time_string,
                            0, error_msg );

      return (long_name);
   }


   /* Build the permission string  */
   switch( file_data->stat.st_mode & S_IFMT )
   {
   case S_IFDIR:
     permission = 'd';
     break;
   case S_IFCHR:
     permission = 'c';
     break;
   case S_IFBLK:
     permission = 'b';
     break;
   case S_IFLNK:
     permission = 'l';
     break;
   default :
     permission = OPTION_OFF;
     break;
   }

   if (file_data->stat.st_mode & S_IRUSR) usr_read_priv = READ_PRIV;
   else usr_read_priv = OPTION_OFF;

   if (file_data->stat.st_mode & S_IWUSR) usr_write_priv = WRITE_PRIV;
   else usr_write_priv = OPTION_OFF;

   if (file_data->stat.st_mode & S_IXUSR) usr_exec_priv = EXEC_PRIV;
   else usr_exec_priv = OPTION_OFF;


   if (file_data->stat.st_mode & S_IRGRP) grp_read_priv = READ_PRIV;
   else grp_read_priv = OPTION_OFF;

   if (file_data->stat.st_mode & S_IWGRP) grp_write_priv = WRITE_PRIV;
   else grp_write_priv = OPTION_OFF;

   if (file_data->stat.st_mode & S_IXGRP) grp_exec_priv = EXEC_PRIV;
   else grp_exec_priv = OPTION_OFF;


   if (file_data->stat.st_mode & S_IROTH) oth_read_priv = READ_PRIV;
   else oth_read_priv = OPTION_OFF;

   if (file_data->stat.st_mode & S_IWOTH) oth_write_priv = WRITE_PRIV;
   else oth_write_priv = OPTION_OFF;

   if (file_data->stat.st_mode & S_IXOTH) oth_exec_priv = EXEC_PRIV;
   else oth_exec_priv = OPTION_OFF;


   long_modify_time = file_data->stat.st_mtime;
#ifdef NLS16
   tms = localtime(&long_modify_time);
   strftime( time_string, 100,
             GetSharedMessage(DIRECTORY_DATE_FORMAT),
             tms);
#else
   time_string = ctime ((time_t *)&long_modify_time);
   time_string[strlen(time_string)-1] = 0x0;
   time_string += 4;
#endif

   /* Fill in the name of where the link goes */
   if (file_data->link)
   {
     strcpy( link_path, " -> " );
     strcpy( link_path + 4, file_data->link );
   }

   {
#define NAME_PRECISION 28
     int len = strlen( file_data->file_name );
     if( len > NAME_PRECISION )
     {
       int i;
       char name[NAME_PRECISION];
       sprintf( name, "%-20.20s (...) ", file_data->file_name );

       sprintf( long_name, "%-28.28s  %s  %9ld  %c%c%c%c%c%c%c%c%c%c  %-9s  %-9s  %s",
                            name,
                            time_string,
                            (long)file_data->stat.st_size,
                            permission,
                            usr_read_priv, usr_write_priv, usr_exec_priv,
                            grp_read_priv, grp_write_priv, grp_exec_priv,
                            oth_read_priv, oth_write_priv, oth_exec_priv,
                            user_name, group_name,
                            link_path );
     }
     else
     {
       sprintf( long_name, "%-28.28s  %s  %9ld  %c%c%c%c%c%c%c%c%c%c  %-9s  %-9s  %s",
                            file_data->file_name,
                            time_string,
                            (long)file_data->stat.st_size,
                            permission,
                            usr_read_priv, usr_write_priv, usr_exec_priv,
                            grp_read_priv, grp_write_priv, grp_exec_priv,
                            oth_read_priv, oth_write_priv, oth_exec_priv,
                            user_name, group_name,
                            link_path );
     }
   }



   return (long_name);
}



/*--------------------------------------------------------------------
 *  DirectoryBusy
 *    See if path has a directory view of it or if any sub-directories
 *    of it are viewed.   The path parameter is of the for /foo/bar
 *------------------------------------------------------------------*/

Boolean
DirectoryBusy(
        char *path )
{
   FileMgrData * file_mgr_data;
   FileViewData  * sub_root;
   int i, j, k;
   int len = strlen(path);

   for (i = 0; i < directory_count; i++)
   {
      /* check if this directory is equal to 'path' or a subdir of 'path' */
      if (directory_set[i]->viewed &&
          (strcmp (directory_set[i]->path_name, path) == 0 ||
           strncmp (directory_set[i]->path_name, path,len) == 0 &&
             directory_set[i]->path_name[len] == '/'
           ||
           directory_set[i]->tt_path_name != NULL &&
           (strcmp (directory_set[i]->tt_path_name, path) == 0 ||
            strncmp (directory_set[i]->tt_path_name, path,len) == 0 &&
              directory_set[i]->tt_path_name[len] == '/')))
      {
         /* check the views in the view list */
         for (j = 0; j < directory_set[i]->numOfViews; j++)
         {
            file_mgr_data = directory_set[i]->directoryView[j].file_mgr_data;

            /* find the dir in the directory set for this view */
            for (k = 0; k < file_mgr_data->directory_count; k++)
               if (strcmp(file_mgr_data->directory_set[k]->name,
                          directory_set[i]->directory_name) == 0)
               {
                  break;
               }
            if (k == file_mgr_data->directory_count)
              continue;  /* not found ... something must be wrong! */

            /*
             * Check if this directory is acutally visible.
             * If the directory is in a tree branch that is not currently
             * expanded, it is not visible and would not be considered busy.
             */

            /* the tree root is always considered busy */
            if (k == 0)
              return True;

            /* a subdir is considered busy if it is visible and at least
             * partially expanded */
            sub_root = file_mgr_data->directory_set[k]->sub_root;
            if (sub_root->displayed &&
                (sub_root->ts == tsDirs && sub_root->ndir > 0  ||
                 sub_root->ts == tsAll &&
                   sub_root->ndir + sub_root->nfile > 0))
            {
              return True;
            }
         }
      }
   }

   return (False);
}


/*--------------------------------------------------------------------
 *  GetDirectoryLogicalType
 *     Get logical type for the iconic path.
 *------------------------------------------------------------------*/

char *
GetDirectoryLogicalType(
	FileMgrData *file_mgr_data,
	char *path)
{
   int len;
   int n;
   Directory *directory;
   char *ptr;

   /* 'path' must be a prefix of the current directory */
   len = strlen(path);
   if (strncmp(file_mgr_data->current_directory, path, len) != 0 ||
       (len > 1 &&
       file_mgr_data->current_directory[len] != '/' &&
       file_mgr_data->current_directory[len] != '\0'))
   {
      DPRINTF(("GetDirectoryLogicalType(%s): len %d, cur_dir %s\n",
                path, len, file_mgr_data->current_directory));
      return NULL;
   }

   /*  Find the directory set entry.  */
   directory = FindDirectory(file_mgr_data->host,
                             file_mgr_data->current_directory);
   if ((directory != NULL) &&
       (strcmp(file_mgr_data->current_directory,
               directory->directory_name) == 0))
   {
      /* if we don't have path_logical_types yet, we don't know */
      if (directory->path_logical_types == NULL)
         return NULL;

      /* count the number of components in path */
      if (strcmp(path, "/") == 0)
         n = 0;
      else
      {
         n = 1;
         ptr = path + 1;
         while ((ptr = DtStrchr(ptr, '/')) != NULL)
         {
            ptr = ptr + 1;
            if (*ptr == '\0')
              break;
            else
              n++;
         }
      }

      DPRINTF2(("GetDirectoryLogicalType(%s): n %d, type %s\n",
                path, n, directory->path_logical_types[n]));

      /* return type form path_logical_types array */
      return directory->path_logical_types[n];
   }

   /* directory not found in directory_set */

   return NULL;
}


/*====================================================================
 *
 * Routines for accessing position information
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  GetDirectoryPositionInfo
 *     Get cached position info
 *------------------------------------------------------------------*/

int
GetDirectoryPositionInfo(
        char *host_name,
        char *directory_name,
        PositionInfo **position_info)
{
   Directory *directory;

   directory = FindDirectory(host_name, directory_name);
   if (directory == NULL)
      return -1;

   *position_info = directory->position_info;

   return directory->position_count;
}


/*--------------------------------------------------------------------
 *  WritePosInfoProcess
 *    Main routine of the background process that writes the
 *    postion information file.
 *------------------------------------------------------------------*/

static int
WritePosInfoProcess(
        int pipe_fd,
	Directory *directory,
	ActivityStatus activity)
{
   char *fileName;
   int position_count = directory->position_count;
   PositionInfo *position_info = directory->position_info;
   FILE *f;
   int i, rc;
   Tt_status tt_status;

   /* construct the full file name */
   fileName = ResolveLocalPathName( directory->host_name,
                                    directory->directory_name, positionFileName,
                                    home_host_name, &tt_status );
   /* Don't have to check for tt_status
      directory->host_name is home_host_name and ResolveLocalPathName will
      always return a good path
   */
   DPRINTF(("WritePosInfoProcess: count %d, file %s\n",
            position_count, fileName));

   /* Remove old files, if no position information for this view */
   if (position_count <= 0)
      rc = unlink(fileName);
   else
   {
      /* open the file for writing */
      f = fopen(fileName, "w");

      if (f == NULL)
      {
         /* Assume read-only directory, if we can't open the file */
         rc = 0;
      }
      else
      {
         chmod(fileName, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

         fprintf(f, "%d\n", position_count);
         for (i = 0; i < position_count; i++)
         {
            fprintf(f, "%s\n%d %d %d\n",
                    position_info[i].name,
                    position_info[i].x,
                    position_info[i].y,
                    position_info[i].stacking_order);
         }

         fclose(f);
         rc = 0;
      }
   }

   /* send result back thorugh the pipe */
   DPRINTF(("WritePosInfoProcess: done (rc %d)\n", rc));
   write(pipe_fd, &rc, sizeof(int));
   XtFree( fileName );
   return 0;
}


/*--------------------------------------------------------------------
 *  WritePosInfoPipeCallback
 *    Callback routine that reads the return code sent through the
 *    pipe from the WritePosInfoProcess background process.
 *------------------------------------------------------------------*/

static void
WritePosInfoPipeCallback(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   PipeCallbackData *pipe_data = (PipeCallbackData *)client_data;
   Directory *directory = pipe_data->directory;
   int rc;
   int i;

   /* get return code from the pipe */
   rc = -1;
   PipeRead(*fd, &rc, sizeof(int));

   /* close the pipe and cancel the callback */
   close(*fd);
   XtFree( client_data );
   XtRemoveInput(*id);

   /* verify that the directory still exists */
   if (DirectoryGone(directory))
   {
      ScheduleActivity(NULL);
      return;
   }

   DPRINTF(("WritePosInfoPipeCallback: rc %d\n", rc));

   /* reset the busy flag and schedule new work, if any */
   directory->busy[activity_writing_posinfo] = False;
   directory->activity = activity_idle;
   ScheduleActivity(directory);
}


/*--------------------------------------------------------------------
 *  SetDirectoryPositionInfo
 *     Update cached position info.  This routine schedules a
 *     background process that writes the modified information
 *     to the position info file.
 *------------------------------------------------------------------*/

int
SetDirectoryPositionInfo(
        char *host_name,
        char *directory_name,
        int position_count,
        PositionInfo *position_info)
{
   Directory *directory;
   Boolean unchanged;
   int i, j;

   /* find the directory */
   directory = FindDirectory(host_name, directory_name);
   if (directory == NULL)
      return -1;

   /* check if anything has changed */
   if (directory->position_count == position_count)
   {
      unchanged = True;
      for (i = 0; i < position_count && unchanged; i++)
      {
         for (j = 0; j < position_count; j++)
            if (strcmp(position_info[i].name,
                       directory->position_info[j].name) == 0)
            {
               break;
            }

         if (j == position_count ||
             position_info[i].x != directory->position_info[j].x ||
             position_info[i].y != directory->position_info[j].y ||
             position_info[i].stacking_order !=
                                    directory->position_info[j].stacking_order)
         {
            unchanged = False;
         }
      }

      /* if nothing changed, don't do anything */
      if (unchanged)
         return 0;
   }

   /* free old position info  names*/
   for (i = 0; i < directory->position_count; i++)
      XtFree(directory->position_info[i].name);

   /* realloc array, if necessary */
   if (directory->position_count != position_count)
   {
      directory->position_count = position_count;
      directory->position_info =
            (PositionInfo *) XtRealloc((char *)directory->position_info,
                                        position_count * sizeof(PositionInfo));
   }

   /* replace old position info */
   for (i = 0; i < position_count; i++)
   {
      directory->position_info[i].name = XtNewString(position_info[i].name);
      directory->position_info[i].x = position_info[i].x;
      directory->position_info[i].y = position_info[i].y;
      directory->position_info[i].stacking_order =
                                              position_info[i].stacking_order;
   }

   /* make sure positionFileName is initialized */
   if (positionFileName == NULL)
      InitializePositionFileName();

   /* start background process that writes the position info file */
   directory->busy[activity_writing_posinfo] = True;
   ScheduleActivity(directory);

   return 0;
}


/*====================================================================
 *
 * Timer functions
 *   These function are periodically called to scan all cached
 *   directories to see if any have been modified since last read.
 *   If any have, a background process is scheduled to re-read
 *   the directory.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * SkipRefresh:
 *   Decide whether to skip an automatic re-read.
 *   (We don't do re-reads on directories that aren't currently
 *    being viewed and on the trash directory, if trash is currently
 *    being emptied.)
 *------------------------------------------------------------------*/

static Boolean
SkipRefresh(
	Directory *directory)
{
   int i;

   /* don't refresh while the directory is being modified */
   if (directory->modify_begin > 0)
      return True;

   /* verify that the directory is still being viewed */
   if (!directory->viewed)
      return True;

   for (i = 0; i < directory->numOfViews; i++)
      if (directory->directoryView[i].mapped)
         break;
   if (i == directory->numOfViews)
      return True;

   /* if trash is being emptied and this is the trash dir, skip it */
   if (removingTrash && strcmp(directory->directory_name, trash_dir) == 0)
      return True;

   return False;
}


/*--------------------------------------------------------------------
 * TimerEventProcess
 *   Main routine for the background process that checks directory
 *   timestamps and the status of links.
 *------------------------------------------------------------------*/

static int
TimerEventProcess(
        int pipe_fd,
	Directory *directory,
	ActivityStatus activity)
{
   struct stat stat_buf;
   long modify_time;
   Boolean link_changed;
   Boolean was_broken;
   FileData *file_data;
   char full_name[MAX_PATH];
   char *namep;
   short pipe_msg;
   int prev_link_kind;
   int cur_link_kind;
   int link_rc;
   int rc;

   /*
    * Do a stat on the directory to get its last-modified time.
    * Also check if we  still have read and execute/search permisssion.
    *
    * Note:
    *  It is important to get the timstamp in exactly the same way that the
    *  ReadDirectoryProcess does it; otherwise, we might get into a loop,
    *  where TimerEventProcess detects that the directory has changed
    *  and triggers ReadDirectoryProcess, but ReadDirectoryProcess won't
    *  be able to get a new timestamp and update the directory structure,
    *  so the next time TimerEventProcess runs it will trigger another
    *  ReadDirectoryProcess, and so on ...
    */
   if (CheckAccess(directory->path_name, R_OK | X_OK) != 0 ||
       stat(directory->path_name, &stat_buf) != 0)
   {
      /* stat or access failed */
      rc = errno;
      modify_time = 0;
   }
   else
   {
      /* stat succeeded and the directory is still readable */
      rc = 0;
      modify_time = stat_buf.st_mtime;
   }

   /*
    * If requested, also check if any links broken.
    *
    * Again: it is important that we determine the kind of link
    * (valid, recursive, or broken) in exactly the same way that
    * ReadDirectoryProcess does it (see comment above)!
    */
   link_changed = False;
   if (rc == 0 && activity == activity_checking_links)
   {
      strcpy(full_name, directory->path_name);
      namep = full_name + strlen(full_name);
      if (namep[-1] != '/')
        *namep++ = '/';

      /* check all links */
      for (file_data = directory->file_data;
           file_data && !link_changed;
           file_data = file_data->next)
      {
         /* Only worry about links */
         if (file_data->link == NULL)
            continue;

         /* Check if the file is still a symbolic link */
         strcpy(namep, file_data->file_name);
         link_rc = lstat(full_name, &stat_buf);
         if (link_rc != 0 || (stat_buf.st_mode & S_IFMT) != S_IFLNK)
         {
            /* no longer a link */
            link_changed = True;
            break;
         }

         /* Check what kind of link this was the last time we looked:
          * a normal link (1), a recursive link (2), or an otherwise
          * broken link (3) */
         if (strcmp(file_data->logical_type, LT_BROKEN_LINK) == 0)
            prev_link_kind = 3;
         else if (strcmp(file_data->logical_type, LT_RECURSIVE_LINK) == 0)
            prev_link_kind = 2;
         else
            prev_link_kind = 1;

         /* Check what kind of link it is now */
         if (_DtFollowLink(full_name) == NULL)
            cur_link_kind = 2;  /* recursive link */
         else if (stat(full_name, &stat_buf) != 0)
            cur_link_kind = 3;  /* broken link */
         else
            cur_link_kind = 1;  /* a valid link */

         /* now we can tell if the link has changed */
         if (prev_link_kind != cur_link_kind)
           link_changed = True;
      }
   }

   /* send result back through the pipe */
   write(pipe_fd, &rc, sizeof(int));
   write(pipe_fd, &modify_time, sizeof(long));
   write(pipe_fd, &link_changed, sizeof(Boolean));
   return 0;
}


/*--------------------------------------------------------------------
 * StickyProcIdle:
 *   Mark sticky background process as idle.  If there are too
 *   may idle sticky procs, cause some of them to exit.
 *------------------------------------------------------------------*/

static void
StickyProcIdle(
   ActivityStatus activity,
   StickyProcDesc *sticky_proc,
   int max_procs)
{
   StickyProcDesc *p, **lp;
   int i, n;

   /* mark the process as idle */
   sticky_proc->idle = True;

   /* if there are too many idle procs, make some of them go away */
   n = 0;
   lp = &ActivityTable[activity].sticky_procs;
   for (p = *lp; p; p = *lp)
   {
      if (!p->idle)
         lp = &p->next;
      else if (n < max_procs)
      {
         n++;
         lp = &p->next;
      }
      else
      {
         DPRINTF2(("StickyProcIdle: end sticky proc %ld\n", (long)p->child));
         PipeWriteString(p->pipe_m2s_fd, NULL);
         close(p->pipe_s2m_fd);
         close(p->pipe_m2s_fd);
         *lp = p->next;
         XtFree((char *)p);
      }
   }
}


/*--------------------------------------------------------------------
 * TimerPipeCallback
 *   Callback routine that reads information sent through the
 *   pipe from the TimerEventProcess background process.
 *------------------------------------------------------------------*/

static void
TimerPipeCallback(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   PipeCallbackData *pipe_data = (PipeCallbackData *)client_data;
   Directory *directory = pipe_data->directory;
   int rc;
   long modify_time;
   Boolean link_changed;
   int i;

   /* get return code from the pipe */
   rc = -1;
   PipeRead(*fd, &rc, sizeof(int));
   PipeRead(*fd, &modify_time, sizeof(long));
   PipeRead(*fd, &link_changed, sizeof(Boolean));

   /* close the pipe and cancel the callback */
   if (pipe_data->sticky_proc)
      StickyProcIdle(pipe_data->activity, pipe_data->sticky_proc,
                     maxRereadProcsPerTick);
   else
      close(*fd);
   XtFree( client_data );
   XtRemoveInput(*id);

   /* verify that the directory still exists */
   if (DirectoryGone(directory))
   {
      ScheduleActivity(NULL);
      return;
   }

   DPRINTF2((
     "TimerPipeCallback: rc %d (was %d), time %ld (was %ld), link change %d\n",
     rc, directory->errnum, modify_time, (long)directory->modify_time, link_changed));

   /* reset the busy flag and schedule new work, if any */
   directory->busy[directory->activity] = False;
   directory->activity = activity_idle;
   ScheduleActivity(directory);

   /* if directory-read already in progress, nothing more to do here */
   if (directory->busy[activity_reading] ||
       directory->busy[activity_update_all])
      return;

   /* skip this directory if it is no longer being viewed */
   if (SkipRefresh(directory))
      return;

   /* if the directory was modified or links changed, re-read it */
   if (rc == 0)
   {
      if (link_changed)
      {
         DPRINTF(("TimerPipeCallback: %s link changed\n",
                  directory->directory_name));
         ReadDirectoryFiles(NULL, directory);
      }
      else if (modify_time != directory->modify_time || directory->errnum != 0)
      {
         DPRINTF(("TimerPipeCallback: %s modified\n",
                  directory->directory_name));
         directory->busy[activity_update_all] = True;
         ScheduleActivity(directory);
      }
   }
   else
   {
      if (directory->errnum == 0)
      {
         directory->errnum = rc;
         directory->errmsg_needed = True;
         ReadDirectoryFiles(NULL, directory);
      }
   }
}


/*--------------------------------------------------------------------
 * CheckDesktopProcess
 *   Main routine for the background process that checks each desktop
 *   objects to see if the file that it refers to has disappeared
 *   or has changed type.
 *------------------------------------------------------------------*/

static int
CheckDesktopProcess(
        int pipe_fd,
	Directory *directory,
	ActivityStatus activity)
{
   int i, n;
   DesktopRec *desktopWindow;
   FileViewData *file_view_data;
   char *full_path;
   Tt_status tt_status;
   struct stat stat_buf;
   short pipe_msg;
   FileData2 file_data2;
   FileData *old_data, *new_data;

   for (i = 0; i < desktop_data->numIconsUsed; i++)
   {
      desktopWindow = desktop_data->desktopWindows[i];
      file_view_data = desktopWindow->file_view_data;

      full_path = ResolveLocalPathName( desktopWindow->host,
                                        desktopWindow->dir_linked_to,
                                        desktopWindow->file_name,
                                        home_host_name, &tt_status);

      /* Check if the file still exists */
      errno = 0;
      if (lstat(full_path, &stat_buf) < 0)
      {
         /* the real file no longer exists */
         DPRINTF2((
           "CheckDesktopProcess: sending PIPEMSG_DESKTOP_REMOVED for %s\n",
           full_path));
         pipe_msg = PIPEMSG_DESKTOP_REMOVED;
         write(pipe_fd, &pipe_msg, sizeof(short));
         PipeWriteString(pipe_fd, desktopWindow->host);
         PipeWriteString(pipe_fd, desktopWindow->dir_linked_to);
         PipeWriteString(pipe_fd, desktopWindow->file_name);
      }
      else
      {
         Boolean IsToolBox;
         /* See if the type has changed */
         old_data = file_view_data->file_data;

         if(directory->directoryView && directory->directoryView->file_mgr_data)
             IsToolBox = directory->directoryView->file_mgr_data->toolbox;
         else
             IsToolBox = False;

         ReadFileData2(&file_data2, full_path, NULL,IsToolBox);
         new_data = FileData2toFileData(&file_data2, &n);

         if (new_data->physical_type != old_data->physical_type ||
             strcmp(new_data->logical_type, old_data->logical_type) != 0)
         {
            /* the type has changed */
            DPRINTF2((
              "CheckDesktopProcess: sending PIPEMSG_DESKTOP_CHANGED for %s\n",
              full_path));
            DPRINTF2((
              "  old type %d %s, new type %d %s\n",
              old_data->physical_type, old_data->logical_type,
              new_data->physical_type, new_data->logical_type));

            pipe_msg = PIPEMSG_DESKTOP_CHANGED;
            write(pipe_fd, &pipe_msg, sizeof(short));
            PipeWriteString(pipe_fd, desktopWindow->host);
            PipeWriteString(pipe_fd, desktopWindow->dir_linked_to);
            PipeWriteString(pipe_fd, desktopWindow->file_name);

            PipeWriteFileData(pipe_fd, new_data);
         }

         FreeFileData(new_data, True);
      }
      XtFree(full_path);
      full_path = NULL;
   }

   /* send a 'done' msg through the pipe */
   DPRINTF2(("CheckDesktopProcess: sending DONE\n"));
   pipe_msg = PIPEMSG_DONE;
   write(pipe_fd, &pipe_msg, sizeof(short));
   return 0;
}


/*--------------------------------------------------------------------
 * CheckDesktopPipeCallback
 *   Callback routine that reads information sent through the
 *   pipe from the CheckDesktopProcess background process.
 *------------------------------------------------------------------*/

static void
CheckDesktopPipeCallback(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   PipeCallbackData *pipe_data = (PipeCallbackData *)client_data;
   Directory *directory = pipe_data->directory;
   short msg;
   char *host, *dir_linked_to, *file_name;
   FileData *new_data, *old_data;
   Boolean found;
   DesktopRec *desktopWindow;
   int i, n;

   /* read the next msg from the pipe */
   msg = -1;
   n = PipeRead(*fd, &msg, sizeof(short));

   if (msg == PIPEMSG_DESKTOP_REMOVED ||
       msg == PIPEMSG_DESKTOP_CHANGED)
   {
      /* get information from pipe */
      host = PipeReadString(*fd);
      dir_linked_to = PipeReadString(*fd);
      file_name = PipeReadString(*fd);
      if (msg == PIPEMSG_DESKTOP_CHANGED)
         new_data = PipeReadFileData(*fd);
      else
         new_data = NULL;

      DPRINTF2((
        "CheckDesktopPipeCallback: msg %d: host %s, dir %s, name %s\n",
        msg, host, dir_linked_to, file_name));


      /* find the desktop object */
      found = False;
      for (i = 0; i < desktop_data->numIconsUsed; i++)
      {
         desktopWindow = desktop_data->desktopWindows[i];

         if (strcmp(host, desktopWindow->host) == 0 &&
             strcmp(dir_linked_to, desktopWindow->dir_linked_to) == 0 &&
             strcmp(file_name, desktopWindow->file_name) == 0)
         {
            found = True;
            break;
         }
      }

      /* remove or update the desktop object, if found */
      if (! found)
      {
        /* nothing to do */
      }
      else if (msg == PIPEMSG_DESKTOP_REMOVED)
      {
         /* remove the desktop object */
         DesktopObjectRemoved(desktopWindow);
      }
      else /* msg == PIPEMSG_DESKTOP_CHANGED */
      {
         /* replace file data */
         old_data = desktopWindow->file_view_data->file_data;
         FreeFileData(old_data, False);
         memcpy(old_data, new_data, sizeof(FileData));
         XtFree((char *)new_data);
         new_data = NULL;

         /* update the desktop object */
         DesktopObjectChanged(desktopWindow);
      }

      /* free storage */
      XtFree(host);
      XtFree(dir_linked_to);
      XtFree(file_name);
      if (new_data)
        FreeFileData(new_data, True);
   }

   else if (msg == PIPEMSG_DONE)
   {
      /* close the pipe and cancel the callback */
      close(*fd);
      XtFree( client_data );
      XtRemoveInput(*id);

      /* reset the busy flag and schedule new work, if any */
      directory->busy[directory->activity] = False;
      directory->activity = activity_idle;
      ScheduleActivity(directory);
   }
}


/*--------------------------------------------------------------------
 *
 * CheckDesktop
 *   Arrange for a CheckDesktopProcess background process to be
 *   started (checks each desktop objects to see if the file that
 *   it refers to has disappeared or has changed type).
 *
 *------------------------------------------------------------------*/

void
CheckDesktop( void )
{
  dummy_directory->busy[activity_checking_desktop] = True;
  ScheduleActivity(dummy_directory);
}


/*--------------------------------------------------------------------
 * TimerEvent
 *   This routine is called periodically.  It schedules a
 *   TimerEventProcess background process to be started for every
 *   directory in the cache.
 *------------------------------------------------------------------*/

/* comparison routine for qsort */
static int
CheckListCmp(
        int *p1,
        int *p2 )
{
   return directory_set[*p1]->last_check - directory_set[*p2]->last_check;
}


static void
TimerEvent(
        XtPointer client_data,
        XtIntervalId *id )
{
   static int *check_list = NULL;
   static int check_alloc = 0;
   int i, j, n;


   DPRINTF2(("Directory::TimerEvent\n"));

   if (dragActive)
   {
      /*
       * Don't change any directories while a drag is active.
       *
       * Reason: drag callbacks are called with a pointer to a FileViewData
       * structure; if a directory is reread while a drag is active,
       * the pointer would become invalid, causing unpredictable behavior.
       *
       * Schedule the next TimerEvent in 1/2 second, so that check will
       * be done soon after the drag is finished.
       */
      XtAppAddTimeOut (app_context, 500, TimerEvent, NULL);
      return;
   }

   /* update tick count */
   tick_count++;

   /* determine if we should also check for broken links this time */
   if (checkBrokenLink > 0 &&
       tick_count >= lastLinkCheckTick + ticksBetweenLinkChecks)
   {
     /* set link_check_needed flag on all directores */
     for (i = 0; i < directory_count; i++)
     {
        /* skip this directory if no view is mapped */
        if (SkipRefresh(directory_set[i]))
           continue;

        /* if a check is already in progress, don't start another one */
        if (directory_set[i]->busy[activity_checking_links])
           continue;

        /* arrange for background process to be scheduled */
        directory_set[i]->link_check_needed = True;
     }

     lastLinkCheckTick = tick_count;
   }

   /* make sure check_list array is big enough */
   if (directory_count > check_alloc)
   {
      check_alloc = directory_count + 5;
      check_list =
         (int *)XtRealloc((char *)check_list, check_alloc*sizeof(int));
   }

   /* get a list of all directories that need to be checked */
   n = 0;
   for (i = 0; i < directory_count; i++)
   {
      /* skip this directory if no view is mapped */
      if (SkipRefresh(directory_set[i]))
         continue;

      /* if a stat is already in progress, don't start another one */
      if (directory_set[i]->busy[activity_checking_dir] ||
          directory_set[i]->busy[activity_checking_links])
         continue;

      /* add this directory to the check list */
      check_list[n++] = i;
   }

   /*
    * Next we want to schedule a background process to be started
    * for each directory in the check_list.  However, the variable
    * maxRereadProcsPerTick puts a limit on the number of such
    * background processes started per clock tick (i.e., per call
    * to this routine).  Hence we sort check_list by last_check
    * (records the tick count when a directory was last read or
    * checked) and schedule backround processes on those dirs that
    * haven't been checked in the longest time.
    */
   qsort(check_list, n, sizeof(int), (int (*)())CheckListCmp);

   /* arrange for background process to be started */
   for (j = 0; j < n && j < maxRereadProcsPerTick; j++)
   {
      i = check_list[j];
      if (directory_set[i]->link_check_needed)
      {
         directory_set[i]->link_check_needed = False;
         directory_set[i]->busy[activity_checking_links] = True;
      }
      else
         directory_set[i]->busy[activity_checking_dir] = True;
      ScheduleActivity(directory_set[i]);
      directory_set[i]->last_check = tick_count;
   }

   /*  Reset the timeout for the next interval.  */
   if (SomeWindowMapped())
      XtAppAddTimeOut(app_context, tickTime * 1000, TimerEvent, NULL);
   else
      timer_suspended = True;
}


/*--------------------------------------------------------------------
 * TimerEventBrokenLinks
 *   This routine is called periodically.  It checks whether any
 *   desktop object is broken (i.e., the object it refers to no
 *   longer exists.
 *------------------------------------------------------------------*/

void
TimerEventBrokenLinks(
        XtPointer client_data,
        XtIntervalId *id )
{
   int i;

   DPRINTF2(("Directory::TimerEventBrokenLinks\n"));

   if (!dragActive)
   {
      /* go check the desktop objects */
      if (desktop_data->numIconsUsed > 0)
         CheckDesktop();
   }

   /*  Reset the timeout for the next interval.  */
   if (desktop_data->numIconsUsed > 0)
   {
     checkBrokenLinkTimerId = XtAppAddTimeOut( app_context,
                                               checkBrokenLink * 1000,
                                               TimerEventBrokenLinks,
                                               NULL );
   }
   else
   {
     checkBrokenLinkTimerId = None;
   }
}


/*====================================================================
 *
 * Background process scheduler
 *
 *   The routines below schedule background activity, making sure
 *   that there aren't too many processes running at the same time.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  ScheduleDirectoryActivity
 *    If there is any work to do for a directory, and if there is
 *    no backgroud process currently running for that directory,
 *    then fork a process to do the work.
 *------------------------------------------------------------------*/

static void
ScheduleDirectoryActivity(
   Directory *directory)
{
   static char *pname = "ScheduleActivity";
   ActivityStatus activity;
   PipeCallbackData *pipe_data;
   Boolean all_views_active;
   Boolean this_view_active;
   int i, j, k;
   int n_active, n_checking;
   int save_last_check;
   FileMgrData *file_mgr_data;
   Boolean sticky;
   StickyProcDesc *p;
   int pipe_s2m_fd[2];  /* for msgs from backgroundnd proc (slave to master) */
   int pipe_m2s_fd[2];  /* for msgs to backgroundnd proc (master to slave) */
   pid_t pid;
   char *s;
   int rc;

   /* If already active, don't start anything new. */
   if (directory->activity != activity_idle)
      return;

   /* Decide what to do next */
   for (activity = 0; activity < activity_idle; activity++)
      if (directory->busy[activity])
         break;

   /* If nothing to do, return */
   if (activity == activity_idle)
      return;

   DPRINTF2(("ScheduleActivity: activity %d, busy %c%c%c%c%c%c%c, dir %s\n",
             directory->activity,
             directory->busy[activity_writing_posinfo]? 'W': '-',
             directory->busy[activity_reading]?         'R': '-',
             directory->busy[activity_update_all]?      'A': '-',
             directory->busy[activity_update_some]?     'U': '-',
             directory->busy[activity_checking_links]?  'B': '-',
             directory->busy[activity_checking_desktop]? 'D': '-',
             directory->busy[activity_checking_dir]?    'C': '-',
             directory->directory_name));

   /* Don't start more than a certain number of background processed */
   n_active = 0;
   n_checking = 0;
   for (j = 0; j < directory_count; j++)
   {
      if (directory_set[j]->activity != activity_idle)
         n_active++;
      if (directory_set[j]->activity == activity_checking_links ||
          directory_set[j]->activity == activity_checking_dir)
         n_checking++;
   }
   if (dummy_directory->activity != activity_idle)
   {
      n_active++;
      n_checking++;
   }
   if (n_active >= maxDirectoryProcesses ||
       n_checking >= maxRereadProcesses)
   {
      DPRINTF2(("ScheduleActivity:  too many processes\n"));
      return;
   }

   /*
    * We don't want to start more than one background process per view.
    * In tree mode one view may show more than one directory.
    * Hence we go through the view list for this directory and for each
    * view, we check if the same view appears on the view list of some
    * other directory that currently has active background activity.
    * If all vies on this directory have other activity, then we won't
    * start anything new.
    */
   if (directory->numOfViews > 0)
   {
     all_views_active = True;
     for (i = 0; i < directory->numOfViews; i++)
     {
      /* get file_mgr_data for this view */
       file_mgr_data = directory->directoryView[i].file_mgr_data;

      /* see if the same view appears in the view list of a non-idle dir */
       this_view_active = False;
       for (j = 0; j < directory_count && !this_view_active; j++)
       {
         /* we are only interested in directories that are not idle */
         if (directory_set[j]->activity == activity_idle)
           continue;

         /* see if the view appears in the view list */
         for (k = 0; k < directory_set[j]->numOfViews; k++)
         {
           if (directory_set[j]->directoryView[k].file_mgr_data ==
               file_mgr_data)
           {
             this_view_active = True;
             break;
           }
         }
       }

       if (!this_view_active)
       {
         all_views_active = False;
         break;
       }
     }

     if (all_views_active)
     {
       DPRINTF2(("ScheduleActivity:  all views busy\n"));
       return;
     }
   }

   /* now we are ready to start the next activity */
   directory->activity = activity;
   if (activity == activity_reading ||
       activity == activity_update_all ||
       activity == activity_checking_dir ||
       activity == activity_checking_links)
   {
      save_last_check = directory->last_check;
      directory->last_check = tick_count;
   }

   /*
    * Special optimization for periodic background processes
    * (currently only used for activity_checking_dir):
    * Since this is done frequently, we don't want to fork new process each
    * time.  Hence, instead of exiting when it's done, the background process
    * is "sticky", i.e., it will stay around waiting for a message on stdin,
    * so it can be re-used the next time around.  A linked list of sticky
    * procs that are currently active is maintained in the ActivityTable.
    */
   sticky = ActivityTable[activity].sticky;
   if (sticky)
   {
      /* see if we can find an idle sticky proc that can do the work */
      for (p = ActivityTable[activity].sticky_procs; p; p = p->next)
         if (p->idle)
            break;
   }
   else
      p = NULL;

   if (p)
   {
      /* We found an idle sticky proc that can be used */
      DPRINTF2(("ScheduleActivity:  use sticky proc %ld\n", (long)p->child));

      /* Send the directory name to the sticky proc */

      if (PipeWriteString(p->pipe_m2s_fd, directory->path_name) < 0) {
        StickyProcDesc *d;

        /* the pipe is broken, remove the old proc then start a new one */
        for (d = ActivityTable[activity].sticky_procs; d && p; d = d->next) {
          if (d == p)
          {
            /* the proc listed 1st is dead, remove it */
            ActivityTable[activity].sticky_procs = p->next;
            XtFree((void *)p);
            p = NULL;
          }
          else if (d->next == p)
          {
            /* the process "p" is dead, remove it */
            d->next = p->next;
            XtFree((void *)p);
            p = NULL;
          }
        }
      }
      else
      {
        pipe_s2m_fd[0] = p->pipe_s2m_fd;
        pid = p->child;
        p->idle = False;
      }
   }


   if (!p)
   {
      /* Need to fork a new background process */

      /* create a pipe for reading data from the background proc */
      pipe(pipe_s2m_fd);

      /* creating a new sticky proc? */
      if (sticky)
      {
         /* also need a pipe for sending msgs to the sticky proc */
         pipe(pipe_m2s_fd);

         /* add entry to the list of sticky procs */
         p = (StickyProcDesc *) XtMalloc(sizeof(StickyProcDesc));
         p->next = ActivityTable[activity].sticky_procs;
         ActivityTable[activity].sticky_procs = p;

         p->pipe_s2m_fd = pipe_s2m_fd[0];
         p->pipe_m2s_fd = pipe_m2s_fd[1];
         p->idle = False;
      }

      /* fork a background process */
      pid = fork();

      if (pid == -1)
      {
          DBGFORK(("%s:  fork failed for activity %d: %s\n",
		    pname, activity, strerror(errno)));

          fprintf(stderr,
		"%s:  fork failed, ppid %d, pid %d, activity %d: error %d=%s\n",
		pname, getppid(), getpid(), activity, errno, strerror(errno));

	  directory->activity = activity_idle;
          directory->last_check = save_last_check;

          /* close unused pipe connections */
          close(pipe_s2m_fd[0]);    /* child won't read from this pipe */
          close(pipe_s2m_fd[1]);    /* parent won't write to this pipe */
          if (sticky)
	  {
             close(pipe_m2s_fd[1]); /* child won't write to this pipe */
             close(pipe_m2s_fd[0]); /* parent won't read from this pipe */
             p->pipe_s2m_fd = 0;
             p->pipe_m2s_fd = 0;
             p->idle = True;
	  }
	  return;
      }

      if (pid == 0)
      {
         /* child process */
         pid = getpid();
         DBGFORK(("%s:  child activity %d, s2m %d, m2s %d\n",
		  pname, activity, pipe_s2m_fd[1], pipe_m2s_fd[0]));

         /* close unused pipe connections */
         close(pipe_s2m_fd[0]);    /* child won't read from this pipe */
         if (sticky)
            close(pipe_m2s_fd[1]); /* child won't write to this pipe */

         /* run main routine for this activity from ActivityTable */
         for (;;)
         {
            rc = (*ActivityTable[activity].main)(pipe_s2m_fd[1],
                                                 directory, activity);
            if (!sticky || rc != 0)
               break;

            /* wait for a message in the pipe */
            s = PipeReadString(pipe_m2s_fd[0]);
            if (s == NULL)
               break;

            XtFree(directory->path_name);
            directory->path_name = s;

            DPRINTF2(("StickyActivity:  activity %d, dir %s\n", activity, s));
         }

         /* close pipes and end this process */
         close(pipe_s2m_fd[1]);
         if (sticky)
            close(pipe_m2s_fd[0]);

         DBGFORK(("%s:  completed activity %d, (rc %d)\n",pname, activity, rc));

         exit(rc);
      }

      DBGFORK(("%s:  forked child<%d> for activity %d, s2m %d, m2s %d\n",
		  pname, pid, activity, pipe_s2m_fd[0], pipe_m2s_fd[1]));

      /* parent process */
      if (sticky)
         p->child = pid;

      /*
       * If a directory read or update was started:
       * clear the modifile_list, now that the
       * background process has it's own copy.
       */
      if (activity == activity_reading ||
          activity == activity_update_all ||
          activity == activity_update_some)
      {
         for (i = 0; i < directory->modified_count; i++)
            XtFree(directory->modified_list[i]);
         XtFree((char *)directory->modified_list);

         directory->modified_count = 0;
         directory->modified_list = NULL;
      }

      /* close unused pipe connections */
      close(pipe_s2m_fd[1]);    /* parent won't write to this pipe */
      if (sticky)
         close(pipe_m2s_fd[0]); /* parent won't read from this pipe */

   }

   /* set up callback to get the pipe data */
   DPRINTF2(("ScheduleActivity:  setting up pipe callback\n"));
   pipe_data = (PipeCallbackData *)XtMalloc(sizeof(PipeCallbackData));
   pipe_data->directory = directory;
   pipe_data->child = pid;
   pipe_data->sticky_proc = p;
   pipe_data->activity = activity;

   XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                 pipe_s2m_fd[0], (XtPointer)XtInputReadMask,
                 ActivityTable[activity].callback, (XtPointer)pipe_data);
}


/*--------------------------------------------------------------------
 *  ScheduleActivity
 *    See if any new background work should be started.
 *------------------------------------------------------------------*/

static void
ScheduleActivity(
   Directory *directory)
{
   int i;

   /* first try to schedule new activity for this directory */
   if (directory != NULL)
     ScheduleDirectoryActivity(directory);

   /* see if there is anything else we can schedule now */
   if (directory == NULL || directory->activity == activity_idle)
   {
      for (i = 0; i < directory_count; i++)
         if (directory_set[i] != directory)
            ScheduleDirectoryActivity(directory_set[i]);
   }
   ScheduleDirectoryActivity(dummy_directory);
 }

static void
SelectDesktopFile(
FileMgrData *file_mgr_data)
{
    DirectorySet *directory_data;
    FileViewData *file_view_data;
    int j;

    directory_data = file_mgr_data->directory_set[0];

    for (j = 0; j < directory_data->file_count; j++)
    {
        file_view_data = directory_data->file_view_data[j];

        if (file_view_data->filtered != True &&
            strcmp(file_mgr_data->desktop_file,
                   file_view_data->file_data->file_name) == 0)
        {
            SelectFile (file_mgr_data, file_view_data);
            break;
        }
    }
    ActivateSingleSelect(file_mgr_data->file_mgr_rec,
                         file_mgr_data->selection_list[0]->file_data->logical_type);
    PositionFileView(file_view_data, file_mgr_data);
}
