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
/* $TOG: FileOp.c /main/14 1999/12/09 13:06:21 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           FileOp.c
 *
 *   COMPONENT_NAME: Desktop
 *
 *   DESCRIPTION:    File processing functions.
 *
 *   FUNCTIONS: ChangeIconName
 *		ChangeIconNameDT
 *		ChangeIconNameProcess
 *		ChangeIconPipeCB
 *		CreateFileFromBuffer
 *		DisplayDuplicateOpError
 *		DisplayErrorMessage
 *		FileMoveCopy
 *		FileMoveCopyDesktop
 *		FileMoveCopyProcess
 *		FileMoveCopyProcessDesktop
 *		FileOpError
 *		FileOpPipeCB
 *		GetTarget
 *		MakeFile
 *		MakeFilePipeCB
 *		MakeFileProcess
 *		MakeFilesFromBuffers
 *		MakeFilesFromBuffersDT
 *		PipeRead
 *		PipeReadString
 *		PipeWriteErrmsg
 *		PipeWriteString
 *		RemoveIconFromWorkspace
 *		ChangeWorkspaceIconLink
 *		SendModifyMsg
 *		_ChangeIconName
 *		_FileMoveCopy
 *		appendErrorMessage
 *		moveCopyLinkCancel
 *		moveCopyLinkOK
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <time.h>
#include <utime.h>

#ifdef __osf__
#include <unistd.h>
#endif

#ifdef _AIX
#include <sys/select.h>
#endif

#if defined(_AIX)|| defined(hpux)
#include <sys/dir.h>
#else
#ifndef MAXNAMLEN
#define MAXNAMLEN 255
#endif
#endif

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/TextF.h>
#include <Xm/ScrollBar.h>
#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif

#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>
#include <Dt/Action.h>
#include <Dt/Connect.h>
#include <Dt/Wsm.h>
#include <Dt/DtNlUtils.h>
#include <Dt/HourGlass.h>
#include <Dt/SharedProcs.h>

#include <Tt/tttk.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Prefs.h"
#include "Common.h"
#include "Filter.h"
#include "Help.h"
#include "SharedMsgs.h"
#include "sharedFuncs.h"


/* Static Function declarations */

static Boolean CreateFileFromBuffer(
                                    int    pipe_s2m,
                                    char   *directory,
                                    char   *fully_qualified_name,
                                    void   *buffer,
                                    int    size);
static String  appendErrorMessage(
                                    String message,
                                    String new,
                                    String arg);
static void    DisplayErrorMessage(
                 int pipe_s2m,
                 char *errMsg,
                 char *from,
                 char *dir);
static Boolean    IsInParentDir(char *from,char *to);

/* the amount of time we wait for a file move/copy to complete */
/* @@@ should make this a resource */
#define FILE_MOVE_COPY_WAIT_TIME 2


/* types of messages sent through the pipe */
#define PIPEMSG_FILEOP_ERROR     1
#define PIPEMSG_EXIST_ERROR      2
#define PIPEMSG_OTHER_ERROR      3
#define PIPEMSG_CONFIRM          4
#define PIPEMSG_TARGET_TIME      5
#define PIPEMSG_FILE_MODIFIED    6
#define PIPEMSG_DONE             7
#define PIPEMSG_REPLACE_RENAME   8
#define PIPEMSG_REPLACE_RENAME_SAME 9
#define PIPEMSG_REPLACE_MERGE       10
#define PIPEMSG_MULTICOLLIDE        11
#define PIPEMSG_MULTICOLLIDE_SAME   12
#define PIPEMSG_MOVE_TO_SAME_DIR    13

/* the following messages are also defined & used in OverWrite.c */
#define PIPEMSG_CANCEL           101
#define PIPEMSG_PROCEED          102
#define PIPEMSG_MERGE            103
#define PIPEMSG_REPLACE_BUFFER   104
#define PIPEMSG_RENAME_BUFFER    105
#define PIPEMSG_MULTI_PROCEED    106

extern int G_dropx,G_dropy;

/* callback data for file move/copy/link */
typedef struct {
   char *host;
   char *directory;
   char *app_man_dir;
   BufferInfo bufferInfo;
   int first_index;
   Boolean time_sent;
   Boolean operationStatus;
   char *file;
} DirUpdate;

typedef struct
{
   Boolean desktop;
   FileMgrData *file_mgr_data;
   FileMgrRec *file_mgr_rec;
   FileViewData *file_view_data;
   DesktopRec *desktopWindow;
   int pipe_m2s;  /* pipe main to slave */
   int pipe_s2m;  /* pipe slave to main */
   int mode;
   char *host;
   char *directory;
   int file_count;
   DirUpdate *updates;
   void (*finish_callback)();
   XtPointer callback_data;
   int child;
} FileOpCBData;


/* callback data for file rename */
typedef struct
{
   Boolean desktop;
   Widget w;
   FileViewData *file_view_data;
   DesktopRec *desktopWindow;
   char *host_name;
   char *directory_name;
   char *old_name;
   char *new_name;
   int child;
} ChangeIconCBData;


/* callback data for create file/directory */
typedef struct
{
   char *to_host;
   char *to_dir;
   char *to_file;
   void (*finish_callback)();
   XtPointer callback_data;
   int child;
} MakeFileCBData;


static void
DisplayDuplicateOpError(FileOpCBData *cb_data,int index);



/*====================================================================
 *
 * Routine for sending data through a pipe
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * PipeRead:
 *    Read data from the pipe
 *------------------------------------------------------------------*/

int
PipeRead(
	int fd,
	void *buf,
	int len)
{
   static int whined_fd = -1;
   int n = 0;
   int rc;

   while (n < len)
   {
      rc = read(fd, (char *)buf + n, len - n);
      if (rc > 0)
        n += rc;
      else if (rc < 0 && errno == EINTR)
         ;  /* try again */
      else
      {
         if (rc == 0)
	 {
	    if (whined_fd != fd)
	    {
		whined_fd = fd;
                fprintf(stderr,
		  "PipeRead: broken pipe, ppid=%d pid=%d fd=%d\n",
		  getppid(), getpid(), fd);
	    }
	 }
         else
	 {
            perror("dtfile: read failed in PipeRead");
	 }
         n = -1;
         break;
      }
   }

   return n;
}


/*--------------------------------------------------------------------
 * PipeWriteString:
 *   write a string to the pipe
 *------------------------------------------------------------------*/

int
PipeWriteString(
	int fd,
	char *s)
{
   short len, sent = 0;
   void (*oldPipe)();

   oldPipe = (void (*)())signal(SIGPIPE, SIG_IGN);

   if (s == NULL)
      len = 0;
   else
      len = strlen(s);

   if (write(fd, &len, sizeof(short)) < 0) {
      return -1;
   }

   if (len > 0)
      sent = write(fd, s, len);

   signal(SIGPIPE, oldPipe);

   return sent;
}


/*--------------------------------------------------------------------
 * PipeReadString:
 *   read a string from the pipe
 *------------------------------------------------------------------*/

char *
PipeReadString(
	int fd)
{
   short len;
   char *s;
   int n;

   /* get the length */
   if (PipeRead(fd, &len, sizeof(short)) != sizeof(short))
      return NULL;

   if (len == 0)
      return NULL;

   /* get the string */
   s = (char *)XtMalloc(len + 1);
   if (PipeRead(fd, s, len) != len)
   {
      XtFree(s);
      return NULL;
   }

   s[len] = '\0';
   return s;
}


/*--------------------------------------------------------------------
 * PipeWriteErrmsg:
 *   write an error message to the pipe
 *------------------------------------------------------------------*/

static int
PipeWriteErrmsg(
	int fd,
	int rc,
	char *msg,
	char *arg)
{
   short pipe_msg = PIPEMSG_FILEOP_ERROR;

   DPRINTF(("PipeWriteErrmsg: sending error %d \"%s\"\n", rc, msg));

   write(fd, &pipe_msg, sizeof(short));
   write(fd, &rc, sizeof(int));
   PipeWriteString(fd, msg);
   PipeWriteString(fd, arg);

   return 0;
}


/*====================================================================
 *
 * FileMoveCopy
 *     Run a background process to move/copy/link files dropped
 *     on a dtfile window or icon.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * moveCopyLinkCancel:
 *   Cancel-button callback for overwrite confirmation dialog
 *------------------------------------------------------------------*/

int filop_confirm_fd = -1;  /* @@@ can't we pass this in client_data? */

static void
moveCopyLinkCancel(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    const int rc = PIPEMSG_CANCEL;

    /* close the dialog */
    XtUnmanageChild((Widget)client_data);
    XmUpdateDisplay((Widget)client_data);
    XtDestroyWidget((Widget)client_data);

    /* send return code through the pipe to the background proc */
    write(filop_confirm_fd, &rc, sizeof(int));
    filop_confirm_fd = -1;
}


/*--------------------------------------------------------------------
 * moveCopyLinkOK:
 *   Ok-button callback for overwrite confirmation dialog
 *------------------------------------------------------------------*/

static void
moveCopyLinkOK(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    const int rc = PIPEMSG_PROCEED;

    /* close the dialog */
    XtUnmanageChild((Widget)client_data);
    XmUpdateDisplay((Widget)client_data);
    XtDestroyWidget((Widget)client_data);

    /* send affirmative return code through the pipe to the background proc */
    write(filop_confirm_fd, &rc, sizeof(int));
    filop_confirm_fd = -1;
}


/*--------------------------------------------------------------------
 * FileOpError
 *   Error handler for FileManip when called in the background process
 *------------------------------------------------------------------*/

void
FileOpError(
        Widget w,
        char *message1,
        char *message2 )
{
  int pipe_fd = (int)w;   /* @@@ Hack! @@@
                             In the background process we call FileManip with
                             the file descriptor for the pipe instead of a
                             widget id.  We rely on the fact that FileManip
                             never uses the widget as a widget, but only
                             passes it to the error handler. */

  /* write the error message to the pipe */
  PipeWriteErrmsg(pipe_fd, -1, message1, message2);
}


/*--------------------------------------------------------------------
 * SendModifyMsg:
 *   Send a message through the pipe that informs the main process
 *   that we are about to modify a directory or that a file has been
 *   modifed.
 *------------------------------------------------------------------*/

static void
SendModifyMsg(
	int pipe_fd,
	int mode,
	Boolean first,
	char *to,
	DirUpdate *updates,
	int i,
	char *target_file)
{
   short pipe_msg;
   char *dir_path;
   long modify_time;
   struct stat stat_buf;
   int j;

   /* if first operation on target dir, send timestamp */
   if (first)
   {
      if (stat(to, &stat_buf) == 0)
      {
         modify_time = stat_buf.st_mtime;
         pipe_msg = PIPEMSG_TARGET_TIME;
         write(pipe_fd, &pipe_msg, sizeof(short));
         write(pipe_fd, &modify_time, sizeof(long));
      }
   }

   /* if first operation on source dir, get timestamp of source dir */
   modify_time = 0;
   j = updates[i].first_index;
   if (mode == MOVE_FILE && !updates[j].time_sent)
   {
      Tt_status tt_status;

      dir_path = ResolveLocalPathName(updates[i].host,
                                      updates[i].directory,
                                      NULL,
                                      home_host_name,
                                      &tt_status);

      if (stat(dir_path, &stat_buf) == 0)
         modify_time = stat_buf.st_mtime;
      XtFree(dir_path);
      updates[j].time_sent = True;
   }

   /* send the modify message */
   pipe_msg = PIPEMSG_FILE_MODIFIED;
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &i, sizeof(int));
   write(pipe_fd, &modify_time, sizeof(long));
   PipeWriteString(pipe_fd, target_file);
}

/*--------------------------------------------------------------------
 * FileMoveCopyProcess:
 *     Main routine of the background process
 *
 *     This routine has two basic modes of operation:
 *        It is given a list of source files and a target directory,
 *           and performs an operation on the source files. This mode
 *           is used for drag & drop operations where a user can
 *           select multiple source files and then drop them on a
 *           target directory to either move, copy, or copy-as-link
 *           the source files to the target directory. The source
 *           and target directories must be different.
 *        It is given a single source file and a target file. The
 *           target can be either a directory or a file. This mode
 *           is used for menu-intiated move, copy, or copy-as-link
 *           operations. The source and target directories can be
 *           the same.
 *     The source file(s) are given in updates; the destination is
 *           in host, dirctory, to_file. If to_file is NULL, the
 *           first mode (drag & drop) is assumed, otherwise assume
 *           menu-initiated mode.
 *
 *------------------------------------------------------------------*/

static int
FileMoveCopyProcess(
        int pipe_s2m,
        int pipe_m2s,
        char *to_file,
        char *directory,
        char *host,
        DirUpdate *updates,
        int file_count,
        int mode,
        DesktopRec *desktopWindow)
{
   char * target_dir, * from, * to;
   int i, j, rc, result;
   Boolean return_val = False;
   Boolean isContainer;
   long current;
   char *tmpStr, *target_file;
   struct stat stat_buf, from_stat_buf;
   short pipe_msg;
   int sameCount = 0;
   int errorCount = 0;
   int *sameIndex = NULL, *sameConfirmType = NULL;
   Boolean first_op = True;
   Tt_status tt_status;
   char * renamed_file;
   long modify_time;
   String permissionErrors = NULL;
   Boolean targetChecked = FALSE;
   Boolean targetError   = FALSE;
   Boolean CopyError=FALSE,MoveError1=FALSE,MoveError2=FALSE;
   char  *CopyString=NULL,*MoveString1=NULL,*MoveString2=NULL;
   Boolean Same;

   /*  Get the fully qualified destination path.  */
   target_dir = (char *)ResolveLocalPathName(host, directory, NULL, home_host_name, &tt_status);
   if( TT_OK != tt_status )
   {
     char msg1[2*MAX_PATH];
#ifdef sun
     sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_1),
              directory, host, host);
#else
     sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_2),
              directory, host, host);
#endif
     PipeWriteErrmsg(pipe_s2m, -1, msg1, NULL);
     pipe_msg = PIPEMSG_FILEOP_ERROR;
     rc = -1;
     write(pipe_s2m, &pipe_msg, sizeof(short));
     write(pipe_s2m, &rc, sizeof(int));
     return rc;
   }

   DtEliminateDots(target_dir);

   for (i = 0; i < file_count; i++)
   {
       Same = False;

      /* get full name of the source file, if just */
      /* dealing with regular files                */
      if (mode != MAKE_BUFFER)
      {
        from = ResolveLocalPathName(updates[i].host,
                                    updates[i].directory,
                                    updates[i].file,
                                    home_host_name,
                                    &tt_status);
        if (from == NULL)
        {
          char msg1[2*MAX_PATH];

#ifdef sun
          sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_1),
                   updates[i].directory, updates[i].host, updates[i].host);
#else
          sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_2),
                   updates[i].directory, updates[i].host, updates[i].host);
#endif
          PipeWriteErrmsg(pipe_s2m, -1, msg1, NULL);
          continue;
        }

        /* do some extra error checking if a target filename is specified */
        /*   this is meant for the case of a menu-initiated operation     */
        if (to_file != NULL)
        {
          /* Verify that the target directory already exists */
          if ( (stat(target_dir, &stat_buf) != 0) ||
               (! S_ISDIR(stat_buf.st_mode)     ) )
          {
            char *msg;

            msg = XtNewString(
              GETMESSAGE(11,41,"The folder\n%s\ndoes not exist."));
            PipeWriteErrmsg( pipe_s2m, -1, msg, target_dir );
            errorCount++;
            XtFree(msg);
            XtFree(from);
            continue;
          }
        }

        /* check for a drop of a directory onto itself */
        if (updates[i].app_man_dir)
        {
          char *temp_dir = XtNewString(target_dir);

          temp_dir = _DtResolveAppManPath(temp_dir, updates[i].app_man_dir);
          if (mode == MOVE_FILE && strcmp(temp_dir, from) == 0)
          {
            char *msg;

            msg = XtNewString(
              GETMESSAGE(11,16, "A folder cannot be moved into itself.\n%s"));
            PipeWriteErrmsg( pipe_s2m, -1, msg, temp_dir );
            errorCount++;
            XtFree(msg);
            XtFree(from);
            XtFree(temp_dir);
            continue;
          }
          XtFree(temp_dir);
        }
        else if (mode == MOVE_FILE && strcmp(target_dir, from) == 0)
        {
          char *msg;

          msg = XtNewString(
            GETMESSAGE(11,16, "A folder cannot be moved into itself.\n%s"));
          PipeWriteErrmsg( pipe_s2m, -1, msg, target_dir );
          errorCount++;
          XtFree(msg);
          XtFree(from);
          continue;
        }
        else if (mode == MOVE_FILE && stat(from, &stat_buf) == 0 &&
                   S_ISDIR(stat_buf.st_mode) && DirectoryBusy(from))
        {
          char *msg;

          msg = XtNewString( GETMESSAGE(11,30, "Cannot move or rename the folder %s.\nAll File Manager views displayed for a folder or its sub-folders\nmust be closed before a folder can be moved or renamed."));

          PipeWriteErrmsg( pipe_s2m, -1, msg, from );
          errorCount++;
          XtFree(msg);
          XtFree(from);
          continue;
        }
      } /* end if */

     /* for copy operations, check for read permission on the source file */
     /* the error message is appended to a list and dealt with later      */
     /* the current file is not processed                                 */
     if (mode == COPY_FILE)
        if (CheckAccess(from,R_OK) == -1)
        {
	   struct stat sbuf;
	   char *tmpStr;

     /* 'errno' is not giving correct results like 'ENOENT' in order to
         use it here, may be because of the other system calls in 
         CheckAccess() */
	   
	   if (stat(from, &sbuf) < 0)
	      tmpStr = (GETMESSAGE(28,13,"Object:\n\n   %s\n\n does not exist in the file system"));
            else
              tmpStr = GetSharedMessage(CANT_READ_ERROR);

           permissionErrors = appendErrorMessage(permissionErrors, tmpStr, from);
           errorCount++;
           continue;
        }

      if (to_file == NULL)
      {
         if (strcmp(target_dir, "/") == 0)
         {
           target_file = (char *)XtMalloc(strlen(target_dir) +
                                          strlen(updates[i].file) + 1);
           sprintf(target_file, "%s%s", target_dir, updates[i].file);
         }
         else
         {
           target_file = (char *)XtMalloc(strlen(target_dir) +
                                          strlen(updates[i].file) + 2);
           sprintf(target_file, "%s/%s", target_dir, updates[i].file);
         }
      }
      else
      {
         if (strcmp(target_dir, "/") == 0)
         {
           target_file = (char *)XtMalloc(strlen(target_dir) +
                                          strlen(to_file) + 1);
           sprintf(target_file, "%s%s", target_dir, to_file);
         }
         else
         {
           target_file = (char *)XtMalloc(strlen(target_dir) +
                                          strlen(to_file) + 2);
           sprintf(target_file, "%s/%s", target_dir, to_file);
         }
      }
      if (updates[i].app_man_dir)
         target_file = _DtResolveAppManPath(target_file,
                                            updates[i].app_man_dir);
       /* check if source and target files are the same.
        * if they are the same, set a flag noting that they are.  If the
        * the operation is a move, it doesn't make any sense to move it, so
        * send back a MOVE_TO_SAME_DIR msg.
        */

      if ((mode != MAKE_BUFFER) && (strcmp(target_file, from) == 0))
      {
          if (mode == MOVE_FILE && stat(target_file, &stat_buf) == 0)
          {
              if(S_ISDIR(stat_buf.st_mode))
            {
                pipe_msg = PIPEMSG_MOVE_TO_SAME_DIR;
                write(pipe_s2m, &pipe_msg, sizeof(short));
                write(pipe_s2m, &i, sizeof(int));
                errorCount++;

                continue;
            }
          }
          Same = True;
      }

      /* for copy operations, check for write permission on the target    */
      /*    directory                                                     */
      /* for move operations, check for write permission on the source and */
      /*    target directories                                             */
      /* the error message is appended to a list and dealt with later      */
      /* the current file is not processed                                 */
      if (mode == COPY_FILE)
      {
         if (CheckAccess(target_dir,W_OK) == -1)
         {
            CopyError = TRUE;
            errorCount++;
            if(CopyString == NULL)
              CopyString = XtNewString(from);
	    else
	    {
	      CopyString = (char *) XtRealloc(CopyString,strlen(CopyString)+
				    strlen(from)+2);
	      strcat(CopyString,"\n");
	      strcat(CopyString,from);
	    }
            continue;
         }
      }
      else if (mode == MOVE_FILE)
      {
         Boolean error = FALSE;
         if (targetChecked || CheckAccess(target_dir,W_OK) == -1)
         {
            MoveError1 = TRUE;
            if(MoveString1 == NULL)
               MoveString1 = XtNewString(from);
            else
            {
               MoveString1=(char *)XtRealloc(MoveString1,strlen(MoveString1)+
                                    strlen(from)+2);
               strcat(MoveString1,"\n");
               strcat(MoveString1,from);
            }
            error = targetError = TRUE;
            targetChecked = TRUE;
         }
         if (CheckAccess(_DtPName(from),W_OK) == -1)
         {
            MoveError2 = TRUE;
            if(MoveString2 == NULL)
              MoveString2 = XtNewString(from);
            else
            {
              MoveString2 = (char *)XtRealloc(MoveString2,strlen(MoveString2)+
                                    strlen(from)+2);
              strcat(MoveString2,"\n");
              strcat(MoveString2,from);
            }
            error = TRUE;
         }
         if (error || targetError)
         {
            errorCount++;
            continue;
         }
      }

      /* check if target file already exists */
      if (stat(target_file, &stat_buf) == 0)
      {
         /* target file already exists: remember and deal with it later */
        if(mode == MOVE_FILE && S_ISDIR(stat_buf.st_mode)
               && DirectoryBusy(target_file))
        {
          char *msg;

          msg = XtNewString( GETMESSAGE(11,30, "Cannot move or rename the folder %s.\nAll File Manager views displayed for a folder or its sub-folders\nmust be closed before a folder can be moved or renamed."));

          PipeWriteErrmsg( pipe_s2m, -1, msg, target_file );
          errorCount++;
          XtFree(msg);
          XtFree(target_file);
          continue;
        }
         if (sameIndex == NULL)
         {
            sameIndex       = (int *)XtMalloc(file_count*sizeof(int));
            sameConfirmType = (int *)XtMalloc(file_count*sizeof(int));
         }
         sameIndex[sameCount] = i;

	 /* determine how to set the pipe message */
	 if (mode == MAKE_BUFFER)
         {
           sameConfirmType[sameCount] = PIPEMSG_REPLACE_RENAME;
         }
         else
         {
           stat(from, &from_stat_buf);
           if ( S_ISDIR(from_stat_buf.st_mode) &&
                S_ISDIR(stat_buf.st_mode)      &&
                mode == COPY_FILE )
           {
               /* if its a directory and there already is a directory of the
                * same name the user may want to merge the directories into
                * one directory.  But if the directory to be copied is being
                * copied into the same directory it came from, it doesn't make
                * sense to merge.  Set up message to REPLACE_RENAME_SAME
                * indicating it is being copied from and to the same directory
                */
               if(Same)
                   sameConfirmType[sameCount] = PIPEMSG_REPLACE_RENAME_SAME;
               else
                   sameConfirmType[sameCount] = PIPEMSG_REPLACE_MERGE;
           }
           else
           {
               /* If the copy/move/link is to the same directory, set up a
                * different case then when the op is happening from different
                * directories.
                */
               if(Same)
                   sameConfirmType[sameCount] = PIPEMSG_REPLACE_RENAME_SAME;
               else
                   sameConfirmType[sameCount] = PIPEMSG_REPLACE_RENAME;
           }
         } /* endif mode != MAKE_BUFFER */

         sameCount++;

	 if (mode != MAKE_BUFFER)
           XtFree ((char *) from);

         XtFree ((char *) target_file);
         continue;
      } /* endif targetfile already exists */

      if ((isContainer = (to_file == NULL)))
         to = target_dir;
      else
         to = target_file;

      /*
       * Note: it is important that SendModifyMsg is called before any
       * changes are made to either the source or target directory.
       * This is because SendModifyMsg is supposed to obtain and send
       * the time stamp of the directories BEFORE any operation.  If
       * the wrong timestamp is sent, the window will be updated and
       * redrawn twice instead of just once.
       */
      SendModifyMsg(pipe_s2m, mode, first_op, target_dir, updates, i,
                    updates[i].file);
      first_op = False;

      if (mode == MAKE_BUFFER)
      {
        /* Call CreateFileFromBuffer */
        return_val = CreateFileFromBuffer(pipe_s2m, to,
	                                  target_file,
	                                  updates[i].bufferInfo.buf_ptr,
	                                  updates[i].bufferInfo.size);
      }
      else
      {
        if (strncmp(directory, desktop_dir, strlen(desktop_dir)) == 0)
            return_val = FileManip((Widget)pipe_s2m, mode, from, to,
                                    isContainer,
                                    FileOpError, True, DESKTOP);
        else
            return_val = FileManip((Widget)pipe_s2m, mode, from, to,
                                    isContainer,
                                    FileOpError, True, NOT_DESKTOP);
        XtFree( (char *) from );
      }

      XtFree ((char *) target_file);

   } /* end for loop */

   if(CopyError == TRUE)
   {
      String errMsg;
      errMsg = GETMESSAGE(11,48,
          "Cannot copy the following objects to folder \"%s\"\n\n%s\n\nThe most likely cause is that you do not have\nwrite permission for the target folder");
      DisplayErrorMessage(pipe_s2m,errMsg,CopyString,target_dir);
      CopyError = FALSE;
      permissionErrors = NULL;
      XtFree(CopyString);
      CopyString = NULL;
   }
   if(MoveError1 == TRUE)
   {
      String errMsg;
      errMsg = GETMESSAGE(11,49,
          "Cannot move the following objects to folder \"%s\"\n\n%s\n\nThe most likely cause is that you do not have\nwrite permission for the target folder");
      DisplayErrorMessage(pipe_s2m,errMsg,MoveString1,target_dir);
      MoveError1 = FALSE;
      permissionErrors = NULL;
      XtFree(MoveString1);
      MoveString2 = NULL;
   }
   if(MoveError2 == TRUE)
   {
      String errMsg;
      errMsg = GETMESSAGE(11,50,
          "Cannot move the following objects to folder \"%s\"\n\n%s\n\nThe most likely cause is that you do not have\npermission to move source object");
      DisplayErrorMessage(pipe_s2m,errMsg,MoveString2,target_dir);
      MoveError2 = FALSE;
      permissionErrors = NULL;
      XtFree(MoveString2);
      MoveString2 = NULL;
   }


   /* If there were any permissions errors, show the error message */
   if (permissionErrors != NULL)
   {
      PipeWriteErrmsg(pipe_s2m, -1, permissionErrors, NULL);
      XtFree(permissionErrors);
   }


   /*
    * Now deal with with the cases where we found that the target file
    * already exists.
    */
   if (sameCount != 0)
   {

      /*
       * @@@ Note: The code below for sending a target-time pipe message
       * at this point shouldn't really be necessary.
       * The problem is that the target directory time stamp MUST be
       * obtained BEFORE any changes are made to the directory;
       * otherwise, the window will be updated and redrawn twice instead
       * of just once.  Unfortunately, currently, if the user chooses to
       * replace or rename the existing file, the rename or delete
       * operation is done in the main process (inside the
       * replace_rename_ok_callback in Overwrite.c), instead of here in
       * the child process where it belongs (the main process shouldn't
       * do any file system operations because they could block for a
       * long time on a slow server).  If and when overwrite dialog code
       * is fixed, the code below can be deleted; the target-time
       * message will then be sent by the call to SendModifyMsg further
       * below after the call to PipeRead.
       */
      if (first_op)
      {
         if (stat(target_dir, &stat_buf) == 0)
         {
            modify_time = stat_buf.st_mtime;
            pipe_msg = PIPEMSG_TARGET_TIME;
            write(pipe_s2m, &pipe_msg, sizeof(short));
            write(pipe_s2m, &modify_time, sizeof(long));
         }
         first_op = False;
      }

      /* send message to main process to display dialog (and possibly remove/rename files) */
      if (file_count == 1)
      {
         if (to_file == NULL)   /* note target_file here is only file name, above it is full path */
            target_file = updates[sameIndex[0]].file;
         else
            target_file = to_file;
         pipe_msg =  sameConfirmType[0];
         DPRINTF(("FileMoveCopyProcess: sending msg %d\n", pipe_msg));
         write(pipe_s2m, &pipe_msg, sizeof(short));
         DPRINTF(("FileMoveCopyProcess: sending: mode %d directory \"%s\" file \"%s\" \n",
                   mode, directory, target_file));
         write(pipe_s2m, &mode, sizeof(int));
         PipeWriteString(pipe_s2m, directory);
         PipeWriteString(pipe_s2m, target_file);
      }
      else
      {
         int processCount=file_count-errorCount;

         /* If the copy/move/link is to the same directory, set up a
          * different case then when the op is happening from different
          * directories.
          */
         if(Same)
             pipe_msg = PIPEMSG_MULTICOLLIDE_SAME;
         else
             pipe_msg = PIPEMSG_MULTICOLLIDE;

         DPRINTF(("FileMoveCopyProcess: sending msg %d\n", pipe_msg));
         write(pipe_s2m, &pipe_msg, sizeof(short));
         DPRINTF(("FileMoveCopyProcess: sending: mode %d processCount %d sameCount %d directory \"%s\"\n",
                   mode, processCount, sameCount, directory));
         write(pipe_s2m, &mode, sizeof(int));
         write(pipe_s2m, &processCount, sizeof(int));
         write(pipe_s2m, &sameCount, sizeof(int));
         PipeWriteString(pipe_s2m, directory);
         DPRINTF(("FileMoveCopyProcess: sending %d filename strings\n", sameCount));
         for (i = 0;  i < sameCount; i++)
            PipeWriteString(pipe_s2m, updates[sameIndex[i]].file);
      }

      /* wait for reply from main process */
      rc = -1;
      PipeRead(pipe_m2s, &rc, sizeof(int));
      DPRINTF(("FileMoveCopyProcess: woke up after confirm, rc %d\n", rc));
      if (rc != PIPEMSG_CANCEL)
      {
         /* affirmative reply: do the operation */
         for(i = 0; i < sameCount; i++)
         {
            j = sameIndex[i];
            if (rc != PIPEMSG_RENAME_BUFFER)
            {
               SendModifyMsg(pipe_s2m, mode, first_op, target_dir, updates, j,
                             updates[j].file);
               first_op = FALSE;
            }
            if (rc == PIPEMSG_MULTI_PROCEED)
            {
	       int opvalue;
               PipeRead(pipe_m2s, &opvalue, sizeof(int));
	       if(opvalue != 0)
		   continue;
	    }

            if (mode != MAKE_BUFFER)
            {
              from = ResolveLocalPathName( updates[j].host,
                                           updates[j].directory,
                                           updates[j].file,
                                           home_host_name,
                                           &tt_status);
              if( TT_OK != tt_status )
              {
                 char msg1[2*MAX_PATH];

#ifdef sun
                 sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_1),
                          updates[j].directory, updates[j].host,
                          updates[j].host);
#else
                 sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_2),
                          updates[j].directory, updates[j].host,
                          updates[j].host);
#endif
                  PipeWriteErrmsg(pipe_s2m, -1, msg1, NULL);
                   continue;
              }
            } /* endif */



            if ( (isContainer = (to_file == NULL)) )
            {
               to = target_dir;
            }
            else
            {
               to = (char *)XtMalloc(strlen(target_dir) + strlen(to_file) + 2);
               sprintf(to, "%s/%s", target_dir, to_file);
            }



            /* check the return code for type of message and */
            /* perform the appropriate action                */
            switch (rc)
            {
                case PIPEMSG_MERGE:

                    if (strncmp(directory, desktop_dir, strlen(desktop_dir)) == 0)
                      return_val = FileManip((Widget)pipe_s2m, MERGE_DIR, from,
                                             to, isContainer,
                                             FileOpError, True, DESKTOP);
                    else
                      return_val = FileManip((Widget)pipe_s2m, MERGE_DIR, from,
                                             to, isContainer,
                                             FileOpError, True, NOT_DESKTOP);
                    break;

                case PIPEMSG_REPLACE_BUFFER:
                    target_file = (char *)XtMalloc(strlen(to) + strlen(updates[j].file)
                                              + 2);
                    sprintf(target_file, "%s/%s", to, updates[j].file);
                    DPRINTF (("file is %s",updates[j].file));
                    return_val = CreateFileFromBuffer(pipe_s2m,
                                                      to,
                                                      target_file,
                                                      updates[j].bufferInfo.buf_ptr,
                                                      updates[j].bufferInfo.size);
                    XtFree((char *)target_file);
                    target_file = NULL;
                    break;
                case PIPEMSG_RENAME_BUFFER:

                    renamed_file = PipeReadString(pipe_m2s);

                    SendModifyMsg(pipe_s2m, mode, first_op, to, updates, j, renamed_file);
                    target_file = (char *)XtMalloc(strlen(to) + strlen(renamed_file)
                                              + 2);
                    sprintf(target_file, "%s/%s", to, renamed_file);
                    DPRINTF(("file is %s",renamed_file));
                    return_val = CreateFileFromBuffer(pipe_s2m,
                                                      to,
                                                      target_file,
                                                      updates[j].bufferInfo.buf_ptr,
                                                      updates[j].bufferInfo.size);
                    XtFree((char *) target_file);
                     target_file = NULL;

                    break;
                default:
                    if (strncmp(directory, desktop_dir, strlen(desktop_dir)) == 0)
                      return_val = FileManip((Widget)pipe_s2m, mode, from, to,
                                             isContainer,FileOpError, True,
                                             DESKTOP);
                    else
                    {
                        /* if the operation (move/copy/link) is happening from
                         * and to the same folder, we want to create a new name
                         * for the object for which the operation is happening
                         * on.
                         */
                        if(Same)
                        {
                            char path[MAX_PATH], newDir[MAX_PATH],
                            newFile[MAX_PATH];
                            char *toFile;

                            strcpy(path, from);
                            generate_NewPath(path,path);
                            split_path(path, newDir, newFile);
                            toFile = (char *)XtMalloc(strlen(newDir) +
                                                      strlen(newFile) + 3);
                            strcpy(toFile, newDir);
                            strcat(toFile, "/");
                            strcat(toFile, newFile);

                            return_val = FileManip((Widget)pipe_s2m, mode, from,
                                                   toFile, False, FileOpError,
                                                   True, NOT_DESKTOP);
                            XtFree(path);
                            XtFree(toFile);
                        }
                        else
                            return_val = FileManip((Widget)pipe_s2m, mode, from,
                                                   to, isContainer, FileOpError,
                                                   True, NOT_DESKTOP);
                    }
            } /* endswitch */

           if (to_file != NULL)
           {
               XtFree ((char *) to);
               to = NULL;
           }
         }/* end for */
      }/*end if rc != PIPEMSG_CANCEL*/

      /* free sameData */
      XtFree((char *)sameIndex);
      sameIndex = NULL;
      XtFree((char *)sameConfirmType);
      sameConfirmType = NULL;
      sameCount = 0;
   } /* endif sameCount != 0 */


   pipe_msg = PIPEMSG_DONE;
   if (rc != PIPEMSG_CANCEL)
      rc = return_val? 0: -1;
   else
      rc = 0;
   write(pipe_s2m, &pipe_msg, sizeof(short));
   write(pipe_s2m, &rc, sizeof(int));

   XtFree ((char *) target_dir);
   target_dir = NULL;

   return rc;
}


/*--------------------------------------------------------------------
 * FileMoveCopyProcessDesktop:
 *     Main routine of the background process that handles files
 *     dropped in desktop icons.
 *------------------------------------------------------------------*/

static int
FileMoveCopyProcessDesktop(
        int pipe_s2m,
        int pipe_m2s,
        char *directory,
        char *host,
        DirUpdate *updates,
        int file_count,
        int mode,
        DesktopRec *desktopWindow)
{
   char * to, *from;
   int i, rc;
   Boolean first_op = True;
   Boolean return_val = False;
   short pipe_msg;
   Tt_status tt_status;

   /*  Get the fully qualified destination path.  */
   to = ResolveLocalPathName(host, directory, NULL, home_host_name, &tt_status);
   if( TT_OK != tt_status )
   {
     char msg1[2*MAX_PATH];
#ifdef sun
     sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_1),
              directory, host, host);
#else
     sprintf (msg1, GetSharedMessage(CANNOT_CONNECT_ERROR_2),
              directory, host, host);
#endif
     PipeWriteErrmsg(pipe_s2m, -1, msg1, NULL);
     pipe_msg = PIPEMSG_FILEOP_ERROR;
     rc = -1;
     write(pipe_s2m, &pipe_msg, sizeof(short));
     write(pipe_s2m, &rc, sizeof(int));
     return rc;
   }
   DtEliminateDots(to);

   for (i = 0; i < file_count; i++)
   {
      /* get full name of the source file, if just */
      /* dealing with regular files                */
      if (mode != MAKE_BUFFER)
      {
         from = ResolveLocalPathName(updates[i].host,
                                     updates[i].directory,
                                     updates[i].file,
                                     home_host_name,
                                     &tt_status);
         if (from == NULL)
         {
            char msg[2*MAX_PATH];

#ifdef sun
            sprintf (msg, GetSharedMessage(CANNOT_CONNECT_ERROR_1),
                       updates[i].directory, updates[i].host, updates[i].host);
#else
            sprintf (msg, GetSharedMessage(CANNOT_CONNECT_ERROR_2),
                       updates[i].directory, updates[i].host, updates[i].host);
#endif
            PipeWriteErrmsg(pipe_s2m, -1, msg, NULL);
         }
         else if (strcmp(to, from) == 0)
         {
            char msg[2*MAX_PATH];

            sprintf(msg,GETMESSAGE(11,16, "A folder cannot be moved into itself.\n%s"),to);
            PipeWriteErrmsg(pipe_s2m, -1, msg, NULL);
            XtFree ((char *) from);
             from = NULL;
         }
      } /* end if dealing with regular files */


      SendModifyMsg(pipe_s2m, mode, first_op, to, updates, i, updates[i].file);

      return_val = True;

      if (mode == MAKE_BUFFER)
      {
        char *target_file;

        if (strcmp(to, "/") == 0)
        {
           target_file = (char *) XtMalloc(strlen(to) +
                                           strlen(updates[i].file) + 1);
           sprintf(target_file, "%s%s", to, updates[i].file);
        }
        else
        {
           target_file = (char *) XtMalloc(strlen(to) +
                                           strlen(updates[i].file) + 2);
           sprintf(target_file, "%s/%s", to, updates[i].file);
        }

        return_val = CreateFileFromBuffer(pipe_s2m,
                                          to,
                                          target_file,
                                          updates[i].bufferInfo.buf_ptr,
                                          updates[i].bufferInfo.size);
      }
      else
      {
        return_val = FileManip((Widget)pipe_s2m, mode, from, to, TRUE,
                               FileOpError, True, DESKTOP);
        XtFree (from);
        from = NULL;
      }
   }

   pipe_msg = PIPEMSG_DONE;
   rc = return_val? 0: -1;
   write(pipe_s2m, &pipe_msg, sizeof(short));
   write(pipe_s2m, &rc, sizeof(int));

   XtFree ((char *) to);
   return rc;
}


static void
RemoveIconFromWorkspace( char * iconName,
                         char * targetDir )
{
  DesktopRec *desktopWin;
  int i;
  char fileName[MAX_PATH];


  for(i = 0; i < desktop_data->numIconsUsed; i++)
  {
    desktopWin = desktop_data->desktopWindows[i];

    if( strcmp( desktopWin->dir_linked_to, "/" ) == 0 )
      sprintf( fileName, "/%s", desktopWin->file_name );
    else
      sprintf( fileName, "%s/%s", desktopWin->dir_linked_to, desktopWin->file_name );

    if( strcmp( fileName, iconName ) == 0 )
    {
      Window   rootWindow;
      Atom     pCurrent;
      Screen   *currentScreen;
      int      screen;
      char     *workspace_name;

      screen = XDefaultScreen(XtDisplay(desktopWin->shell));
      currentScreen =
        XScreenOfDisplay(XtDisplay(desktopWin->shell), screen);
      rootWindow = RootWindowOfScreen(currentScreen);

      if(DtWsmGetCurrentWorkspace(XtDisplay(desktopWin->shell),
                                  rootWindow, &pCurrent) == Success)
        workspace_name =
          XGetAtomName (XtDisplay(desktopWin->shell), pCurrent);
      else
        workspace_name = XtNewString("One");

      if( strcmp( workspace_name, desktopWin->workspace_name ) == 0 )
      {
        RemoveDT( desktopWin->shell, (XtPointer) desktopWin,
                  (XtPointer)NULL );
      }
      else
      {
        XtFree( desktopWin->dir_linked_to );
        desktopWin->dir_linked_to = XtNewString( targetDir );
      }

      XtFree(workspace_name);
    }
  }
}

static void
ChangeWorkspaceIconLink( FileMgrData *fmd,
                         char * iconName,
                         char * targetDir,
                         char * iconPdir )
{
  DesktopRec *desktopWin;
  char *dirp = NULL;
  int i;
  char fileName[MAX_PATH];


  for(i = 0; i < desktop_data->numIconsUsed; i++)
  {
    desktopWin = desktop_data->desktopWindows[i];

    if( strcmp( desktopWin->dir_linked_to, "/" ) == 0 )
      sprintf( fileName, "/%s", desktopWin->file_name );
    else
      sprintf( fileName, "%s/%s", desktopWin->dir_linked_to, desktopWin->file_name );

    DtEliminateDots(fileName);
    if( strcmp( fileName, iconName ) == 0 )
      dirp = XtNewString(targetDir);
    else if(IsInParentDir(iconName,desktopWin->dir_linked_to))
    {
      int index = strlen(iconPdir);
      char *tptr = desktopWin->dir_linked_to;

      dirp = XtCalloc( 1, strlen(targetDir)+ strlen(&tptr[index])+1);
      sprintf(dirp,"%s%s",targetDir,&tptr[index]);

    }
    if(dirp)
    {
      FileViewData *file_view_data;
      DirectorySet *directory_set;
      FileMgrData  *file_mgr_data;
      Tt_status tt_status;
      char * full_dir_name;

      file_view_data = desktopWin->file_view_data;
      directory_set = (DirectorySet *)file_view_data->directory_set;
      file_mgr_data = (FileMgrData *)directory_set->file_mgr_data;

      if(file_mgr_data)
         fmd = file_mgr_data;
      FreeFileData( file_view_data->file_data, True );
      file_view_data->file_data = NULL;

      XtFree( desktopWin->dir_linked_to );
      desktopWin->dir_linked_to = XtNewString( dirp );
      DtEliminateDots(desktopWin->dir_linked_to);

      if(fmd)
      {
         if (fmd->restricted_directory == NULL)
            desktopWin->restricted_directory = NULL;
         else
            desktopWin->restricted_directory =
                XtNewString(fmd->restricted_directory);

         if (fmd->helpVol == NULL)
            desktopWin->helpVol = NULL;
         else
            desktopWin->helpVol = XtNewString(fmd->helpVol);

        desktopWin->helpVol         = XtNewString( fmd->helpVol );
        desktopWin->view            = fmd->view;
        desktopWin->order           = fmd->order;
        desktopWin->direction       = fmd->direction;
        desktopWin->positionEnabled = fmd->positionEnabled;
        desktopWin->toolbox         = fmd->toolbox;
      }

      full_dir_name = ResolveLocalPathName(desktopWin->host,
                                           dirp,
                                           NULL,
                                           home_host_name,
                                           &tt_status);
      if( TT_OK == tt_status )
      {
        FileData2 file_data2;
        int n;
        Boolean IsToolBox;

        DtEliminateDots (full_dir_name);

        if ( fmd )
          IsToolBox = fmd->toolbox;
        else
          IsToolBox = False;

        if (strcmp(desktopWin->file_name, ".") == 0)
            ReadFileData2(&file_data2, full_dir_name, NULL, IsToolBox);
	else
            ReadFileData2(&file_data2, full_dir_name, desktopWin->file_name,
                          IsToolBox);

        file_view_data->file_data = FileData2toFileData(&file_data2, &n);
        XtFree(full_dir_name);
        full_dir_name = NULL;
      }

      ((DirectorySet *)file_view_data->directory_set)->name =
        XtNewString(dirp);

      SaveDesktopInfo(NORMAL_RESTORE);
      XtFree(dirp);
      dirp = NULL;
    }
  }
}

/*--------------------------------------------------------------------
 * FileOpPipeCB
 *   Read and process data sent through the pipe.
 *------------------------------------------------------------------*/

static void
FileOpPipeCB(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   FileOpCBData *cb_data = (FileOpCBData *)client_data;
   short pipe_msg;
   int i, n, rc;
   char *title, *err_msg, *err_arg;
   char *directory, *file, *target_file;
   long modify_time;
   Boolean done;
   int mode;
   int nSelected, nCollisions;
   String *fileList;
   static int status = 0;

  static ActionAreaItem replace_rename_actionItems[] = {
    {"Ok",     9, 27, NULL,  NULL},    /* changed later based on mode */
    {"Cancel", 9, 28, replace_rename_cancel_callback, NULL},
    {"Help",   9, 29, HelpRequestCB, HELP_FILE_MANAGER_REP_REN},
  };

  ActionAreaDefn replace_renameActions = {
     XtNumber(replace_rename_actionItems),
     1,                      /* Cancel is default action */
     replace_rename_actionItems
  };

  static ActionAreaItem replace_merge_actionItems[] = {
    {"Ok",     9, 27, replace_merge_ok_callback,     NULL},
    {"Cancel", 9, 28, replace_merge_cancel_callback, NULL},  /* changed below depending on mode */
    {"Help",   9, 29, HelpRequestCB, HELP_FILE_MANAGER_REP_MRG},
  };

  ActionAreaDefn replace_mergeActions = {
     XtNumber(replace_merge_actionItems),
     1,                      /* Cancel is default action */
     replace_merge_actionItems
  };

  static ActionAreaItem multicollide_actionItems[] = {
    {"Ok",     9, 27, NULL, NULL}, /* changed later based on Mode */
    {"Cancel", 9, 28, multicollide_cancel_callback, NULL},  /* changed below depending on mode */
    {"Help",   9, 29, HelpRequestCB, HELP_FILE_MANAGER_MULTI},
  };

  ActionAreaDefn multicollideActions = {
     XtNumber(multicollide_actionItems),
     1,                      /* Cancel is default action */
     multicollide_actionItems
  };


   /* Initialize Action Area structures based on mode */
   /* Set the appropriate callback routines           */
   mode = cb_data->mode;
   switch (mode)
   {
     case MAKE_BUFFER:
	  replace_rename_actionItems[0].callback =
	                           buffer_replace_rename_ok_callback;
          multicollide_actionItems[0].callback =
				   buffer_multicollide_ok_callback;
	  break;
     default:
	  replace_rename_actionItems[0].callback =
	                           replace_rename_ok_callback;
          multicollide_actionItems[0].callback =
				   multicollide_ok_callback;
   } /* endswitch */



   /* read the next msg from the pipe */
   pipe_msg = -1;
   n = PipeRead(*fd, &pipe_msg, sizeof(short));
   DPRINTF(("FileOpPipeCB: n %d, pipe_msg %d\n", n, pipe_msg));

   done = False;
   switch (pipe_msg)
   {
      case PIPEMSG_FILEOP_ERROR:
         PipeRead(*fd, &rc, sizeof(int));
         err_msg = PipeReadString(*fd);
         err_arg = PipeReadString(*fd);
         if (cb_data->desktop)
            FileOperationError(cb_data->file_view_data->widget,
                               err_msg, err_arg);
         else
         {
            /* routine can be called with a NULL file_mgr_rec, use the
             * top level widget if this is the case.
             */
            if(cb_data->file_mgr_rec)
               FileOperationError(cb_data->file_mgr_rec->file_window,
                                  err_msg, err_arg);
            else
               FileOperationError(toplevel, err_msg, err_arg);
         }
         XtFree(err_msg);
         XtFree(err_arg);
         if(cb_data->callback_data)
	 {
	   RenameDoneData *rdd = (RenameDoneData *)cb_data->callback_data;
           ResetFlag( rdd->call_struct->dialog_widget,rdd->w);
           ResetFlag( rdd->call_struct->dialog_widget,rdd->call_struct->Cancel);
	 }
         status = PIPEMSG_FILEOP_ERROR;
         break;

      case PIPEMSG_CONFIRM:
         err_msg = PipeReadString(*fd);
         title = XtNewString(GETMESSAGE(9,11, "File Manager - Move/Copy/Link Warning"));
         filop_confirm_fd = cb_data->pipe_m2s;
         _DtMessageDialog(toplevel, title, err_msg, NULL, TRUE,
                          moveCopyLinkCancel, moveCopyLinkOK, NULL,
                          HelpRequestCB, False, QUESTION_DIALOG);
         XtFree(err_msg);
         XtFree(title);
         break;

       case PIPEMSG_REPLACE_RENAME_SAME:
           /* filename collision: display replace/rename dialog */
           PipeRead(*fd, &mode, sizeof(int));
           directory = PipeReadString(*fd);
           file      = PipeReadString(*fd);
           /* routine can be called with a NULL file_mgr_rec, use the
            * top level widget if this is the case.
            */

           /* the object is copying/linking itself to the same folder.  Want
            * to indicate the to the ok dialog and the building of the replace
            * name dialog.
            */
           replace_rename_actionItems[0].data = (XtPointer)True;
           if(cb_data->file_mgr_rec)
               create_replace_rename_dialog(cb_data->file_mgr_rec->shell,
                                            mode, directory, file,
                                            cb_data->pipe_m2s,
                                            replace_renameActions, True);
           else
               create_replace_rename_dialog(toplevel,
                                            mode, directory, file,
                                            cb_data->pipe_m2s,
                                            replace_renameActions, True);
           XtFree(directory);
           XtFree(file);
           break;

      case PIPEMSG_REPLACE_RENAME:
         /* filename collision: display replace/rename dialog */
         PipeRead(*fd, &mode, sizeof(int));
         directory = PipeReadString(*fd);
         file      = PipeReadString(*fd);
         /* routine can be called with a NULL file_mgr_rec, use the
          * top level widget if this is the case.
          */
         replace_rename_actionItems[0].data = (XtPointer)NULL;
         if(cb_data->file_mgr_rec)
            create_replace_rename_dialog(cb_data->file_mgr_rec->shell,
                                         mode, directory, file,
                                         cb_data->pipe_m2s,
                                         replace_renameActions, False);
         else
            create_replace_rename_dialog(toplevel,
                                         mode, directory, file,
                                         cb_data->pipe_m2s,
                                         replace_renameActions, False);
         XtFree(directory);
         XtFree(file);
         break;

      case PIPEMSG_REPLACE_MERGE:
         /* filename collision: display replace/merge dialog */
         PipeRead(*fd, &mode, sizeof(int));
         directory = PipeReadString(*fd);
         file      = PipeReadString(*fd);
         /* routine can be called with a NULL file_mgr_rec, use the
          * top level widget if this is the case.
          */
         if(cb_data->file_mgr_rec)
            create_replace_merge_dialog(cb_data->file_mgr_rec->shell,
                                        mode, directory, file,
                                        cb_data->pipe_m2s,
                                        replace_mergeActions);
         else
            create_replace_merge_dialog(toplevel,
                                        mode, directory, file,
                                        cb_data->pipe_m2s,
                                        replace_mergeActions);
         XtFree(directory);
         XtFree(file);
         break;

       case PIPEMSG_MULTICOLLIDE_SAME:
           /* filename collision: display multicollide dialog */
           PipeRead(*fd, &mode,        sizeof(int));
           PipeRead(*fd, &nSelected,   sizeof(int));
           PipeRead(*fd, &nCollisions, sizeof(int));
           directory = PipeReadString(*fd);
           file      = XtMalloc( MAX_PATH );
           fileList  = (String *) XtMalloc(nCollisions * sizeof(String));   /* de- allocated in dialog's callback functions */
           for (i = 0; i < nCollisions; i++)
           {
               fileList[i] = PipeReadString(*fd);
           }
           /* routine can be called with a NULL file_mgr_rec, use the
            * top level widget if this is the case.
            */

           /* the object is copying/linking itself to the same folder.  Want
            * to indicate the to the ok dialog and the building of the replace
            * name dialog.
            */
           multicollide_actionItems[0].data = (XtPointer)True;
           if(cb_data->file_mgr_rec)
               create_multicollide_dialog(cb_data->file_mgr_rec->shell,
                                          mode, nSelected, nCollisions,
                                          directory, fileList,
                                          cb_data->pipe_m2s,
                                          multicollideActions, True);
           else
               create_multicollide_dialog(toplevel,
                                          mode, nSelected, nCollisions,
                                          directory, fileList,
                                          cb_data->pipe_m2s,
                                          multicollideActions, True);
           XtFree(directory);
           break;

      case PIPEMSG_MULTICOLLIDE:
         /* filename collision: display multicollide dialog */
         PipeRead(*fd, &mode,        sizeof(int));
         PipeRead(*fd, &nSelected,   sizeof(int));
         PipeRead(*fd, &nCollisions, sizeof(int));
         directory = PipeReadString(*fd);
         file      = XtMalloc( MAX_PATH );
         fileList  = (String *) XtMalloc(nCollisions * sizeof(String));   /* de-allocated in dialog's callback functions */
         for (i = 0; i < nCollisions; i++)
         {
            fileList[i] = PipeReadString(*fd);
         }
         /* routine can be called with a NULL file_mgr_rec, use the
          * top level widget if this is the case.
          */
         multicollide_actionItems[0].data = (XtPointer)NULL;
         if(cb_data->file_mgr_rec)
            create_multicollide_dialog(cb_data->file_mgr_rec->shell,
                                       mode, nSelected, nCollisions,
                                       directory, fileList,
                                       cb_data->pipe_m2s,
                                       multicollideActions, False);
         else
            create_multicollide_dialog(toplevel,
                                       mode, nSelected, nCollisions,
                                       directory, fileList,
                                       cb_data->pipe_m2s,
                                       multicollideActions, False);

         XtFree(directory);
         break;

      case PIPEMSG_TARGET_TIME:
         /* get the modify time and update the directory cache */
         PipeRead(*fd, &modify_time, sizeof(long));
         DirectoryModifyTime(cb_data->host, cb_data->directory, modify_time);
         break;

      case PIPEMSG_MOVE_TO_SAME_DIR:
         /* get the update index */
         PipeRead(*fd, &i, sizeof(int));
         cb_data->updates[i].operationStatus = True;
         DisplayDuplicateOpError((void *) cb_data,i);
         status = PIPEMSG_MOVE_TO_SAME_DIR;
         if(cb_data->callback_data)
	 {
	   RenameDoneData *rdd = (RenameDoneData *)cb_data->callback_data;
           ResetFlag( rdd->call_struct->dialog_widget,rdd->w);
           ResetFlag( rdd->call_struct->dialog_widget,rdd->call_struct->Cancel);
	 }
         break;

      case PIPEMSG_FILE_MODIFIED:
         /* get the update index and modify time */
         PipeRead(*fd, &i, sizeof(int));
         PipeRead(*fd, &modify_time, sizeof(long));
         target_file = PipeReadString(*fd);
         DPRINTF (("PIPEMSG_FILE_MODIFIED %s\n", target_file));

         /* mark the file updated in the cached target directory */
         DirectoryFileModified(cb_data->host, cb_data->directory,
                               target_file);

         if (cb_data->mode == MOVE_FILE)
         {
            /* mark the file updated in the cached source directory */
            if (modify_time != 0)
               DirectoryModifyTime(cb_data->updates[i].host,
                                   cb_data->updates[i].directory,
                                   modify_time);
            DirectoryFileModified(cb_data->updates[i].host,
                                  cb_data->updates[i].directory,
                                  cb_data->updates[i].file);
         }
         cb_data->updates[i].operationStatus = True;
	 XtFree(target_file); target_file = NULL;
         break;

      case PIPEMSG_DONE:
         PipeRead(*fd, &rc, sizeof(int));
         done = True;
         break;

      default:
        fprintf(stderr, "Internal error in FileOpPipeCB: bad pipe_msg %d\n",
                pipe_msg);
        rc = -1;
        done = True;
   }

   if (done)
   {
      char tmpDir[MAX_PATH];

      DPRINTF(("FileOpPipeCB: done, rc %d\n", rc));

      /* close the pipe and cancel the callback */
      close(cb_data->pipe_m2s);
      close(cb_data->pipe_s2m);
      if (id != NULL)
         XtRemoveInput(*id);
      else
         *fd = (rc == 0)? 0: -1;

      /* arrange for modified directories to be updated */
      DirectoryEndModify(cb_data->host, cb_data->directory);

      /* Reposition the objects which have been modified */
      if(!cb_data->finish_callback && cb_data->file_mgr_data)
      {
        char **file_set;
        int actual_count=0;
        /* Do this only if it is the current directory and Random placement
           is ON */
        if(cb_data->file_mgr_data->positionEnabled != RANDOM_OFF && strcmp(
            cb_data->directory,cb_data->file_mgr_data->current_directory)==0)
        {
          file_set = (char **) XtCalloc(1,cb_data->file_count*sizeof(char *));
          for(i=0;i<cb_data->file_count;i++)
          {
             if(cb_data->updates[i].operationStatus == True)
                file_set[actual_count++] = cb_data->updates[i].file;
          }
          RepositionIcons(cb_data->file_mgr_data, file_set,actual_count,
             G_dropx, G_dropy, True);
          XtFree((char *)file_set);
          file_set = NULL;
        }
      }

      for(i = 0; i < desktop_data->numWorkspaces; i++)
        DeselectAllDTFiles(desktop_data->workspaceData[i]);

      for (i = 0; i < cb_data->file_count; i++)
      {
         char fileName[MAX_PATH];

         /* Scroll the window to show the created object, since we
            cannot keep scrolling for each object we just do it for
            one object and we do it in case of Drag/Drop, but not
            for Select.MoveTo/CopyTo ... */

         if(!i && cb_data->callback_data == NULL)
         {
           FileMgrData *fmd;

           if(cb_data && cb_data->file_mgr_data)
           {
             fmd = cb_data->file_mgr_data;
             fmd->scrollToThisDirectory = XtNewString(cb_data->directory);
             fmd->scrollToThisFile = XtNewString(cb_data->updates[i].file);
           }
         }
         if (cb_data->updates[i].first_index == i)
         {
            if (cb_data->mode == MOVE_FILE)
            {
              DirectoryEndModify(cb_data->updates[i].host,
                                 cb_data->updates[i].directory);
              if( strcmp( cb_data->updates[i].directory, "/" ) == 0 )
                tmpDir[0] = 0x0;
              else
                sprintf( tmpDir, "%s", cb_data->updates[i].directory );
            }

            if( cb_data->updates[i].host )
              XtFree( cb_data->updates[i].host );
            if( cb_data->updates[i].directory )
              XtFree( cb_data->updates[i].directory );
            if( cb_data->updates[i].app_man_dir )
              XtFree( cb_data->updates[i].app_man_dir );
         }

         if( cb_data->mode == MOVE_FILE )
         {
           sprintf( fileName, "%s/%s", tmpDir, cb_data->updates[i].file );
           if( cb_data->updates[i].operationStatus == True )
           {
             if( status == PIPEMSG_MOVE_TO_SAME_DIR )
             {
               RemoveIconFromWorkspace( fileName, cb_data->directory );
             }
             else if( status != PIPEMSG_FILEOP_ERROR )
             {
               ChangeWorkspaceIconLink(cb_data->file_mgr_data, fileName,
			     cb_data->directory,tmpDir );

	       /* If it is workspace drag and drop and the move operation
		  is not because of  Select.MoveTo menu option  */

	       if(initiating_view == NULL && cb_data->callback_data == NULL)
	       {
                 sprintf( fileName, "%s/%s", cb_data->directory,
			      cb_data->updates[i].file );
                 DtEliminateDots(fileName);
                 RemoveIconFromWorkspace( fileName, cb_data->directory );
	       }
             }
           }
         }

         XtFree(cb_data->updates[i].file);
      }

      status = 0;

      /* call the callback routine */
      if (cb_data->finish_callback)
         (*cb_data->finish_callback)(cb_data->callback_data, rc);

      /* free the callback data */
      XtFree((char *)cb_data->updates);
      XtFree((char *)cb_data->directory);
      XtFree((char *)cb_data->host);
      XtFree(client_data);
   }
}


/*--------------------------------------------------------------------
 * _FileMoveCopy
 *    Start the background process and set up callback for the pipe.
 *------------------------------------------------------------------*/

static Boolean
_FileMoveCopy(
        XtPointer data,
        char *to_file,
        char *directory,
        char *host,
        char **host_set,
        char **file_set,
        BufferInfo *buffer_set,
        int file_count,
        unsigned int modifiers,
        DesktopRec *desktopWindow,
        void (*finish_callback)(),
        XtPointer callback_data)
{
   static char *pname = "_FileMoveCopy";
   int mode;
   FileOpCBData *cb_data;
   DirUpdate *updates;
   int i, j;
   char *ptr = NULL;
   char *source_dir = NULL;
   char *source_file = NULL;
   int pipe_m2s[2];
   int pipe_s2m[2];
   int pid;
   fd_set select_fds;
   int fd;
   struct timeval now, select_end, select_timeout;
   Boolean operation_done;
   int rc;


   /*  Determine the type of operation: move, copy, or link */
   /*  or creating buffers                                  */
   if (buffer_set != NULL)
   {
     mode = MAKE_BUFFER;
   }
   else
   {
     modifiers &= ~Button2Mask;
     if (modifiers == ShiftMask)
       mode = LINK_FILE;
     else if (modifiers == ControlMask)
       mode = COPY_FILE;
     else
       mode = MOVE_FILE;
   }

   /* set up the callback data structure */
   cb_data = XtNew(FileOpCBData);
   cb_data->desktop = (desktopWindow != NULL);
   if (cb_data->desktop)
   {
      cb_data->file_mgr_data = NULL;
      cb_data->file_mgr_rec = NULL;
      cb_data->file_view_data = (FileViewData *)data;
      cb_data->desktopWindow = desktopWindow;
   }
   else
   {
      if(data != NULL)
      {
         cb_data->file_mgr_data = (FileMgrData *)data;
         cb_data->file_mgr_rec =
           (FileMgrRec *) ((FileMgrData *)data)->file_mgr_rec;
      }
      else
      {
         cb_data->file_mgr_data = NULL;
         cb_data->file_mgr_rec = NULL;
      }
      cb_data->file_view_data = NULL;
      cb_data->desktopWindow = NULL;
   }
   cb_data->mode = mode;
   cb_data->host = XtNewString(host);
   cb_data->directory = XtNewString(directory);
   cb_data->finish_callback = finish_callback;
   cb_data->callback_data = callback_data;


   /* mark the target directory as being modified in the directory cache */
   DirectoryBeginModify(host, directory);

   /* make a list of the operations to be done */
   /* Allocate memory for the DirUpdateStructure */
   cb_data->file_count = file_count;
   cb_data->updates =
     updates = (DirUpdate *)XtMalloc(file_count * sizeof(DirUpdate));

   /* Determine whether we are dealing with files or buffers */
   /* This affects how the updates structure is initialized  */
   if (mode == MAKE_BUFFER)
   {
      for (i=0; i< file_count; i++)
      {
        /* just simply set the the updates structure with */
        /* the passed in file names                       */
        updates[i].file = XtNewString(file_set[i]);
        updates[i].bufferInfo.buf_ptr = buffer_set[i].buf_ptr;
        updates[i].bufferInfo.size = buffer_set[i].size;

        /* set unused updates fields to NOOP values */
        updates[i].time_sent = FALSE;
        updates[i].first_index = 0;
        updates[i].host = NULL;
        updates[i].directory = NULL;
        updates[i].app_man_dir = NULL;
      }
    }
    else
    {
      /* Seperate file names, directories, and hosts */
      /* when dealing with real files                */
      for (i=0; i< file_count; i++)
      {
        /* get the name of the source directory */
        ptr = strrchr(file_set[i], '/');
	if (NULL != ptr)
	{
            if (ptr == file_set[i])
              source_dir = "/";
            else
            {
              *ptr = '\0';
              source_dir = file_set[i];
            }
            source_file = ptr + 1;
	}
	else
	{
	    source_dir = strdup(".");
	    source_file = file_set[i];
	}

        /* see if this directory is already in the list */
        for (j = 0; j < i; j++)
          if (strcmp(updates[j].host, host_set[i]) == 0 &&
              strcmp(updates[j].directory, source_dir) == 0)
            break;

        if (j < i)
        {  /* already in the list */
          updates[i].host = updates[j].host;
          updates[i].directory = updates[j].directory;
          updates[i].app_man_dir = updates[j].app_man_dir;
        }
        else
        {  /* not yet in the list */
          updates[i].host = XtNewString(host_set[i]);
          updates[i].directory = XtNewString(source_dir);
          updates[i].app_man_dir = NULL;
          if (!desktopWindow)
          {
             if(data != NULL)
             {
                if (((FileMgrData *)data)->toolbox)
                   updates[i].app_man_dir =
                     XtNewString(((FileMgrData *)data)->restricted_directory);
             }
          }

          /* mark the directory as being modified in the directory cache */
          if (mode == MOVE_FILE)
            DirectoryBeginModify(updates[i].host, updates[i].directory);
        }
        updates[i].first_index = j;
        updates[i].time_sent = False;
        updates[i].operationStatus = False;
        updates[i].file = XtNewString(source_file);

        if (NULL != ptr) *ptr = '/';
      }/* end for loop */
    } /* endif */


   /* create a pipe */
   pipe(pipe_m2s);
   pipe(pipe_s2m);

   /* fork the process that does the actual work */
   pid = fork();
   if (pid == -1)
   {
       DirectoryAbortModify(host, directory);
       for (i=0; i<file_count; i++)
         if (mode == MOVE_FILE && updates[i].first_index == i)
           DirectoryAbortModify(updates[i].host, updates[i].directory);

       fprintf(stderr,
		"%s:  fork failed, ppid %d, pid %d: error %d=%s\n",
		pname, getppid(), getpid(), errno, strerror(errno));
       return False;
   }

   if (pid == 0)
   {
      DBGFORK(("%s:  child forked, m2s %d s2m %d\n",
		pname, pipe_m2s[0], pipe_s2m[1]));

      close(pipe_m2s[1]);  /* won't write to pipe_m2s */
      close(pipe_s2m[0]);  /* won't read from pipe_s2m */

      if (desktopWindow != NULL)
         rc = FileMoveCopyProcessDesktop(pipe_s2m[1], pipe_m2s[0],
                                         directory, host, updates, file_count,
                                         mode, desktopWindow);
      else
         rc = FileMoveCopyProcess(pipe_s2m[1], pipe_m2s[0], to_file, directory,
                                  host, updates, file_count, mode, NULL);

      DBGFORK(("%s:  child exiting\n", pname));
      exit(rc);
   }

   DBGFORK(("%s:  forked child<%d>, m2s %d, s2m %d\n",
		pname, pid, pipe_m2s[1], pipe_s2m[0]));


   /* parent: set up callback to get the pipe data */
   close(pipe_m2s[0]);  /* won't read from pipe_m2s */
   close(pipe_s2m[1]);  /* won't write to pipe_s2m */

   cb_data->pipe_m2s = pipe_m2s[1];
   cb_data->pipe_s2m = pipe_s2m[0];
   cb_data->mode = mode;
#ifdef __osf__
   cb_data->child = pid;
#endif

   /*
    * We wait a certain amount of time for the background process to finish.
    * If it doesn't finish within that time, we do the rest asynchronously.
    */

   /* set up fd set for select */
   FD_ZERO(&select_fds);
   fd = pipe_s2m[0];

   /* compute until what time we want to wait */
   gettimeofday(&select_end, NULL);
   select_end.tv_sec += FILE_MOVE_COPY_WAIT_TIME;

   operation_done = False;
   for (;;)
   {
      /* determine how much time is left */
      gettimeofday(&now, NULL);
      select_timeout.tv_sec = select_end.tv_sec - now.tv_sec;
      select_timeout.tv_usec = select_end.tv_usec - now.tv_usec;
      if (select_timeout.tv_usec < 0)
      {
         select_timeout.tv_sec--;
         select_timeout.tv_usec += 1000000;
      }

      if ((int) select_timeout.tv_sec < 0)
      {
         /* check if our time is up */
         DPRINTF(("FileMoveCopy: timed out; adding input callback\n"));
         XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                       pipe_s2m[0], (XtPointer)XtInputReadMask,
                       FileOpPipeCB, (XtPointer)cb_data);
         break;
      }

      /* do the select */
      FD_SET(fd, &select_fds);
#if defined(__hpux) && (OSMAJORVERSION <= 10) && (OSMINORVERSION < 2)
      rc = select(fd + 1, (int *)&select_fds, NULL, NULL, &select_timeout);
#else
      rc = select(fd + 1, &select_fds, NULL, NULL, &select_timeout);
#endif
      if (rc < 0 && errno != EINTR)
      {
         perror("select failed in FileMoveCopy");
         break;
      }
      else if (rc == 1)
      {
         /* call FileOpPipeCB to read & process the data from the pipe */
         FileOpPipeCB((XtPointer)cb_data, &fd, NULL);
         DPRINTF(("FileMoveCopy: FileOpPipeCB -> fd = %d\n", fd));

         /*
          * If the background process is done, FileOpPipeCB sets fd
          * to zero (in case of success) or -1 (in case of failure).
          */
         if (fd <= 0)
         {
            operation_done = (fd == 0);
            break;
         }
      }
   }

   return operation_done;
}


/*--------------------------------------------------------------------
 * FileMoveCopy, FileMoveCopyDesktop
 *   External entry points for invoking _FileMoveCopy
 *------------------------------------------------------------------*/

Boolean
FileMoveCopy(
        FileMgrData *file_mgr_data,
        char *to_file,
        char *directory,
        char *host,
        char **host_set,
        char **file_set,
        int file_count,
        unsigned int modifiers,
        void (*finish_callback)(),
        XtPointer callback_data)
{
   return _FileMoveCopy( (XtPointer)file_mgr_data, to_file, directory, host,
                         host_set, file_set, NULL, file_count, modifiers, NULL,
                         finish_callback, callback_data);
}


Boolean
FileMoveCopyDesktop(
      FileViewData *file_view_data,
      char * directory,
      char ** host_set,
      char ** file_set,
      int file_count,
      unsigned int modifiers,
      DesktopRec *desktopWindow,
      void (*finish_callback)(),
      XtPointer callback_data)
{
  return _FileMoveCopy ((XtPointer)file_view_data,
                        NULL,
                        directory,
                        home_host_name,
                        host_set,
                        file_set,
                        NULL,
                        file_count,
                        modifiers,
                        desktopWindow,
                        finish_callback,
                        callback_data);
}


/*====================================================================
 *
 * ChangeIconName
 *     Run a background process to rename an object.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 * ChangeIconNameProcess:
 *     Main routine of the background process
 *------------------------------------------------------------------*/

static int
ChangeIconNameProcess(
        int pipe_fd,
        char *host_name,
        char *directory_name,
        char *old_name,
        char *new_name)
{
   char * full_name, * old_full_name, *dir_path;
   struct stat stat_buf;
   long modify_time;
   Boolean success;
   short pipe_msg;
   int rc;
   Tt_status tt_status;

   /* Check for uniqueness */
   full_name = ResolveLocalPathName(host_name, directory_name, new_name, home_host_name, &tt_status);
   if ( TT_OK != tt_status )
   {
      DPRINTF(("ChangeIconNameProcess: sending exist error\n"));
      pipe_msg = PIPEMSG_FILEOP_ERROR;
      write(pipe_fd, &pipe_msg, sizeof(short));
      XtFree(new_name);
      return 1;
   }

   if (lstat(full_name, &stat_buf) == 0)
   {
      /* Name is not unique */
      DPRINTF(("ChangeIconNameProcess: sending exist error\n"));
      pipe_msg = PIPEMSG_EXIST_ERROR;
      write(pipe_fd, &pipe_msg, sizeof(short));

      XtFree(full_name);
      XtFree(new_name);
      return 1;
   }

   /* send a modified message back through the pipe */
   modify_time = 0;
   dir_path = ResolveLocalPathName(host_name, directory_name, NULL, home_host_name, &tt_status);
   if( TT_OK != tt_status )
   {
      DPRINTF(("ChadengeIconNameProcess: sending exist error\n"));
      pipe_msg = PIPEMSG_FILEOP_ERROR;
      write(pipe_fd, &pipe_msg, sizeof(short));
      XtFree(full_name);
      XtFree(new_name);
      return 1;
   }

   if (stat(dir_path, &stat_buf) == 0)
      modify_time = stat_buf.st_mtime;
   XtFree(dir_path);

   /* rename the file */
   old_full_name = ResolveLocalPathName(host_name, directory_name, old_name, home_host_name, &tt_status);
   if( TT_OK != tt_status )
   {
     DPRINTF(("ChangeIconNameProcess: sending exist error\n"));
     pipe_msg = PIPEMSG_FILEOP_ERROR;
     write(pipe_fd, &pipe_msg, sizeof(short));
     XtFree(full_name);
     XtFree(new_name);
     return 1;
   }
   success = FileManip((Widget)pipe_fd, MOVE_FILE, old_full_name, full_name, TRUE,
                       FileOpError, True, NOT_DESKTOP);
   XtFree( old_full_name );
   /* send a 'done' msg through the pipe */
   rc = success? 0: -1;
   if (rc == 0)
   {
     pipe_msg = PIPEMSG_FILE_MODIFIED;
     write(pipe_fd, &pipe_msg, sizeof(short));
     write(pipe_fd, &modify_time, sizeof(long));
   }
   DPRINTF(("ChangeIconNameProcess: sending DONE, rc %d\n", rc));
   pipe_msg = PIPEMSG_DONE;
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &rc, sizeof(int));
   if (rc == 0)
      PipeWriteString(pipe_fd, full_name);
   XtFree( full_name );
   return rc;
}


/*--------------------------------------------------------------------
 * ChangeIconPipeCB:
 *   Read and process data sent through the pipe.
 *------------------------------------------------------------------*/

static void
ChangeIconPipeCB(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   ChangeIconCBData *cb_data = (ChangeIconCBData *)client_data;
   FileViewData *file_view_data = cb_data->file_view_data;
   DesktopRec *desktopWindow = cb_data->desktopWindow;

   Widget msg_widget;
   DirectorySet *directory_set;
   FileMgrData *file_mgr_data = NULL;
   FileMgrRec *file_mgr_rec;
   short pipe_msg;
   int i, j, n;
   int rc;
   char *title, *err_msg, *err_arg;
   long modify_time;
   char *full_name;
   char *tmpStr;
   XmString label;
   Arg args[3];
   Boolean desktop_changed;

   /* get widget for error messages */
   if (cb_data->desktop)
   {
      msg_widget = XtParent(cb_data->w);
   }
   else
   {
      directory_set = (DirectorySet *)file_view_data->directory_set;
      file_mgr_data = (FileMgrData *)directory_set->file_mgr_data;
      file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
      msg_widget = file_mgr_rec->file_window;
   }

   /* read the msg from the pipe */
   pipe_msg = -1;
   n = PipeRead(*fd, &pipe_msg, sizeof(short));
   DPRINTF(("ChangeIconPipeCB: n %d, pipe_msg %d\n", n, pipe_msg));

   if (pipe_msg == PIPEMSG_FILE_MODIFIED)
   {
      /* get modify time */
      PipeRead(*fd, &modify_time, sizeof(long));

      /* mark the old & new files as modified in the directory cache */
      if (modify_time != 0)
         DirectoryModifyTime(cb_data->host_name, cb_data->directory_name,
                             modify_time);
      DirectoryFileModified(cb_data->host_name, cb_data->directory_name,
                            cb_data->old_name);
      DirectoryFileModified(cb_data->host_name, cb_data->directory_name,
                            cb_data->new_name);
      return;
   }

   if (pipe_msg == PIPEMSG_EXIST_ERROR)
   {
      /* Name is not unique */
      title = XtNewString(GetSharedMessage(FILE_RENAME_ERROR_TITLE));
      if (cb_data->desktop)
      {
         tmpStr = GETMESSAGE(28,10, "An object with this name already exists in the original folder\n(The folder this object came from).\nPlease choose a different name.");
         err_msg = XtNewString(tmpStr);
      }
      else
         err_msg = XtNewString(GETMESSAGE(9,9, "A file with this name already exists.\nPlease choose a different name."));

      _DtMessage (msg_widget, title, err_msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(err_msg);
   }

   else if (pipe_msg == PIPEMSG_FILEOP_ERROR)
   {
      /* the rename failed */
      PipeRead(*fd, &rc, sizeof(int));
      err_msg = PipeReadString(*fd);
      err_arg = PipeReadString(*fd);
      FileOperationError(msg_widget, err_msg, err_arg);
      XtFree(err_msg);
      XtFree(err_arg);
   }

   else if (pipe_msg == PIPEMSG_DONE)
   {
      /* get the return code */
      PipeRead(*fd, &rc, sizeof(int));
      if (rc == 0)
      {
         /* the rename was successful */
         full_name = PipeReadString(*fd);

         /* All went well, destroy the text field */
         XtUnmanageChild(cb_data->w);
         XtDestroyWidget(cb_data->w);

         /* Force the icon label to be updated immediately */
         if (cb_data->desktop)
         {
            XmProcessTraversal(desktopWindow->iconGadget, XmTRAVERSE_CURRENT);
            desktopWindow->text = NULL;
            /* we'll catch this icon label in the loop after the else */
         }
         else
         {
            file_mgr_data->renaming = NULL;

            label = XmStringCreateLocalized(cb_data->new_name);
            XtSetArg(args[0], XmNstring, label);
            XtSetValues(file_view_data->widget, args, 1);
            XmStringFree(label);
            XmProcessTraversal(file_view_data->widget, XmTRAVERSE_CURRENT);
            XmUpdateDisplay (file_mgr_rec->file_window);

            /*
             * To prevent the positional data from becoming disassociated with
             * this file, we need to change the name in the positional data
             * structure also.
             */
            for (i = 0; i < file_mgr_data->num_objects; i++)
            {
               if (strcmp(cb_data->old_name,
                          file_mgr_data->object_positions[i]->name) == 0)
               {
                  /* Found a match */
                  XtFree(file_mgr_data->object_positions[i]->name);
                  file_mgr_data->object_positions[i]->name =
                                             XtNewString(cb_data->new_name);
                  break;
               }
            }
         }

         /*
          * Check all desktop windows to see if they were linked to
          * the file we just renamed.  If so, we need to change the
          * link in .dt/Desktop that points to it.
          *
          * Note: We could do this in a background process, but we assume
          * that .dt/Desktop is local or, if it's remote and the remote server
          * is down, the user is screwed anyway.  So we assume it's ok to do
          * blocking operations on .dt/Desktop in the main process.
          * Hence we go for the simpler solution here.
          */
         desktop_changed = False;
         for (i = 0; i < desktop_data->numIconsUsed; i++)
         {
            desktopWindow = desktop_data->desktopWindows[i];

            if (strcmp(cb_data->host_name, desktopWindow->host) == 0 &&
                strcmp(cb_data->directory_name, desktopWindow->dir_linked_to) == 0
                && strcmp(cb_data->old_name, desktopWindow->file_name) == 0)
            {
               XmString label;

               /* Force the icon label to be updated immediately */
               label = XmStringCreateLocalized(cb_data->new_name);
               XtSetArg(args[0], XmNstring, label);
               XtSetValues(desktopWindow->iconGadget, args, 1);
               XmStringFree(label);

               XtFree(desktopWindow->file_name);
               desktopWindow->file_name = XtNewString(cb_data->new_name);

               XtFree(desktopWindow->file_view_data->file_data->file_name);
               desktopWindow->file_view_data->file_data->file_name =
                                           XtNewString(cb_data->new_name);
               if( strcmp( desktopWindow->title, cb_data->old_name ) == 0 )
               {
                 XtFree( desktopWindow->title );
                 desktopWindow->title = XtNewString( cb_data->new_name );
               }
#ifdef SHAPE
               GenerateShape(desktopWindow);
#endif
               RegisterIconDropsDT(desktopWindow);
               XmUpdateDisplay (desktopWindow->iconGadget);

               desktop_changed = True;
            }
         }

         if (desktop_changed)
         {
            SaveDesktopInfo(NORMAL_RESTORE);
            CheckDesktop();
         }

         XtFree(full_name);
      }
   }
   else
      fprintf(stderr, "Internal error in ChangeIconPipeCB: bad pipe_msg %d\n",
              pipe_msg);

   /* arrange for the modified directory to be updated */
   DirectoryEndModify(cb_data->host_name, cb_data->directory_name);

   /* close the pipe and cancel the callback */
   close(*fd);
   XtRemoveInput(*id);

   /* free callback data */
   XtFree(cb_data->host_name);
   XtFree(cb_data->directory_name);
   XtFree(cb_data->old_name);
   XtFree(cb_data->new_name);
   XtFree((char *)cb_data);
}


/*--------------------------------------------------------------------
 * _ChangeIconName:
 *    Start the background process and set up callback for the pipe.
 *------------------------------------------------------------------*/

static void
_ChangeIconName (
   Widget w,
   Boolean desktop,
   FileViewData *file_view_data,
   DesktopRec *desktopWindow,
   char *host_name,
   char *directory_name)
{
   static char *pname = "_ChangeIconName";
   ChangeIconCBData *cb_data;
   Arg args[3];
   char *input_name;
   char *new_name;
   char *old_name;
   char *title;
   char *msg;
   char *tmpStr;
   int i, j;
   int pipe_fd[2];
   int pid;
   int rc;
   int dirNameLength = strlen (directory_name);
   int maxFileNameLength = pathconf (directory_name, _PC_NAME_MAX);
   int length;


   /* get the new name */
   XtSetArg(args[0], XmNvalue, &input_name);
   XtSetArg(args[1], XmNuserData, &old_name);
   XtGetValues(w, args, 2);

   new_name = (char *)_DtStripSpaces(XtNewString(input_name));
   length = strlen (new_name);


   /* new name must be a simple name, no path */
   msg = NULL;
   if (DtStrchr (new_name, '/') != NULL)
      msg = XtNewString(GetSharedMessage(LOCAL_RENAME_ONLY_ERROR));
#ifdef _CHECK_FOR_SPACES
   else if (DtStrchr (new_name, ' ') != NULL ||
            DtStrchr (new_name, '\t') != NULL)
   {
      msg = XtNewString(GetSharedMessage(NO_SPACES_ALLOWED_ERROR));
   }
#endif
   else if (length == 0 || strcmp(new_name, old_name) == 0)
   {
      /* Noop; simply remove the text field */
      XmProcessTraversal(file_view_data->widget, XmTRAVERSE_CURRENT);
      XtFree(new_name);
      if (desktop )
      {
        UnpostDTTextField();
      }
      else
      {
         DirectorySet *directory_set =
            (DirectorySet *)file_view_data->directory_set;
         FileMgrData *file_mgr_data =
            (FileMgrData *)directory_set->file_mgr_data;
         FileMgrRec *file_mgr_rec =
            (FileMgrRec *)file_mgr_data->file_mgr_rec;
         file_mgr_rec->menuStates |= RENAME;
         UnpostTextField( file_mgr_data );
         file_mgr_data->renaming = NULL;
      }
      return;
   }
   /* Ensure the new name has length less than or equal to the maximum
      length that the system allows.
      If maxFileNameLength == -1 the file system is not supporting POSIX, use MAXNAMLEN
   */
   else if( maxFileNameLength < -1  || (  maxFileNameLength == -1 && ( length > MAXNAMLEN  || length + dirNameLength > MAX_PATH ) ) || ( maxFileNameLength > 0 && length > maxFileNameLength ) )
   {
     msg = XtNewString(GetSharedMessage(FILE_RENAME_ERROR_TITLE));
   }


   if (msg != NULL)
   {
      title = XtNewString(GetSharedMessage(FILE_RENAME_ERROR_TITLE));
      _DtMessage (XtParent (w), title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
      XtFree (new_name);
      return;
   }

   /* Check to see if the file has a representitive on the Desktop.  */
   if (desktopWindow == NULL)
   {
      for (i = 0; i < desktop_data->numIconsUsed; i++)
      {
         if (strcmp(host_name,
                    desktop_data->desktopWindows[i]->host) == 0 &&
             strcmp(directory_name,
                    desktop_data->desktopWindows[i]->dir_linked_to) == 0 &&
             strcmp(old_name, desktop_data->desktopWindows[i]->
                                  file_view_data->file_data->file_name) == 0)
         {
            desktopWindow = desktop_data->desktopWindows[i];
            break;
         }
      }
   }

   if (desktopWindow != NULL)
   {
      /*
       * There is a representation of this file on the desktop:
       * check if the there are any objects which match the new_name
       */
      for (j = 0; j < desktop_data->numIconsUsed; j++)
      {
         if (strcmp(new_name, desktop_data->desktopWindows[j]->
                         file_view_data->file_data->file_name) == 0)
         {
            title = XtNewString(GetSharedMessage(FILE_RENAME_ERROR_TITLE));
            if (desktop)
              tmpStr = GETMESSAGE(28,9, "An object with this name already exists on the Workspace.\nPlease choose a different name.");
            else
              tmpStr = GETMESSAGE(9,90, "Name Conflict.\nThis object is out on the Workspace back drop.\nAnother object on the back drop already has the name you\nare trying to enter.\nPlease choose a different name.");
            msg = XtNewString(tmpStr);
            _DtMessage(XtParent (w), title, msg, NULL, HelpRequestCB);
            XtFree(title);
            XtFree(msg);
            XtFree(new_name);
            return;
         }
      }
   }

   /*
    * Now we are ready to start the background process
    * that does the actual rename.
    */

   /* set up callback data */
   cb_data = XtNew(ChangeIconCBData);
   cb_data->w = w;
   cb_data->desktop = desktop;
   cb_data->file_view_data = file_view_data;
   cb_data->desktopWindow = desktopWindow;
   cb_data->host_name = XtNewString(host_name);
   cb_data->directory_name = XtNewString(directory_name);
   cb_data->old_name = XtNewString(old_name);
   cb_data->new_name = new_name;

   /* mark the directory as being modified in the directory cache */
   DirectoryBeginModify(host_name, directory_name);

   /* create a pipe */
   pipe(pipe_fd);

   /* fork the process that does the actual work */
   pid = fork();
   if (pid == -1)
   {
       DirectoryAbortModify(host_name, directory_name);
       fprintf(stderr,
		"%s: fork failed, ppid %d, pid %d: error %d=%s\n",
		pname, getppid(), getpid(), errno, strerror(errno));
       return;
   }

   if (pid == 0)
   {
      DBGFORK(("%s:  child forked, pipe %d\n", pname, pipe_fd[1]));

      close(pipe_fd[0]);  /* child won't read from the pipe */

      rc = ChangeIconNameProcess(pipe_fd[1], host_name, directory_name,
                                 old_name, new_name);
      close(pipe_fd[1]);

      DBGFORK(("%s:  child exiting\n", pname));

      exit(rc);
   }

   DBGFORK(("%s:  forked child<%d>, pipe %d\n", pname, pid, pipe_fd[0]));


   /* parent: set up callback to get the pipe data */
   close(pipe_fd[1]);  /* parent won't write the pipe */

   cb_data->child = pid;

   XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                 pipe_fd[0], (XtPointer)XtInputReadMask,
                 ChangeIconPipeCB, (XtPointer)cb_data);
}


void
ChangeIconName (
   Widget w,
   XtPointer client_data,
   XmTextVerifyCallbackStruct * call_data)
{
   int value, size, increment, page;
   Arg args[3];
   FileMgrData *file_mgr_data = (FileMgrData *)client_data;
   FileMgrRec  *file_mgr_rec = (FileMgrRec *) file_mgr_data->file_mgr_rec;
   DirectorySet *directory_set = (DirectorySet *)(file_mgr_data->renaming->directory_set);

   if (call_data->reason == XmCR_MODIFYING_TEXT_VALUE ||
       call_data->reason == XmCR_MOVING_INSERT_CURSOR  )
   {
      int SWwidth;
         /* x1     - x value of the text widget with respect to scroll window
            x2,y2  - x,y values of the cursor position (new) w.r.t text widget
            x3,y3  - x,y values of the cursor position (previous) w.r.t textW
         */
      Position x1,x2,y2,x3,y3;
         /* width of the text widget */
      Dimension stringWidth;

      XtSetArg(args[0],XmNx,&x1);
      XtGetValues(w,args,1);

      if(XtIsManaged(file_mgr_rec->vertical_scroll_bar))
         SWwidth=(file_mgr_rec->scroll_window->core.width -
			file_mgr_rec->vertical_scroll_bar->core.width);
      else
         SWwidth=file_mgr_rec->scroll_window->core.width;

      XmTextFieldPosToXY(w,call_data->newInsert,&x2,&y2);
      XmTextFieldPosToXY(w,call_data->currInsert,&x3,&y3);

      XtSetArg(args[0],XmNwidth,&stringWidth);
      XtGetValues(w,args,1);
/*
      printf("\n  x2=%d x1=%d x3=%d\n",x2,x1,x3);
*/
	 if ( (Dimension)(call_data->newInsert) <  stringWidth )
	 {
                if( XtIsManaged(file_mgr_rec->horizontal_scroll_bar) )
                {
                    XmScrollBarGetValues( file_mgr_rec->horizontal_scroll_bar,
                                          &value, &size, &increment, &page);
		   /*
                     printf("\n value = %d",value);
		   */
                    /* case where cursor is moved forward */
                    if( (x2-x3) > 0)
                    {
		       int max=0;
                       XtSetArg (args[0], XmNmaximum, &max);
                       XtGetValues ( file_mgr_rec->horizontal_scroll_bar, args, 1);
                       if(   (value < (max-size)) &&
                             ((x1+x2) - value > 0 ) &&
                             ( (Position)(((x1+x2) - value) +
                                 file_mgr_rec->vertical_scroll_bar->core.width) > (Position) SWwidth ) )
	               {
                               if( (value+(x2-x3)) > (max-size) )
                                 value = (max-size);
                               else
                                 value += (x2-x3);
                               XmScrollBarSetValues(
                                                     file_mgr_rec->horizontal_scroll_bar,
                                                     value, size, increment, page,True
                                                   );
                        }
                    }
                    /* case where cursor is moved in reverse direction */
	            else if( (x2-x3) < 0 )
                    {
                        int min=0;
                        XtSetArg (args[0], XmNminimum, &min);
                        XtGetValues ( file_mgr_rec->horizontal_scroll_bar, args, 1);
                        if( (value > min) && ((Position)(x1+x3) < (Position)(value+
                                       file_mgr_rec->vertical_scroll_bar->core.width)) )
                        {
                             if( (x2 <= 0)  || ((value - (x3-x2)) < min) )
                                value = min;
                              else
                                value -= (x3-x2);
                              XmScrollBarSetValues(file_mgr_rec->horizontal_scroll_bar,
                                                   value, size, increment, page,True
                                                  );
                        }
                    }
                }
         }
      return;
      }
   _ChangeIconName (w, False, file_mgr_data->renaming, NULL,
                    file_mgr_data->host, directory_set->name);
}


void
ChangeIconNameDT (
   Widget w,
   XtPointer client_data,
   XtPointer call_data)
{
   DesktopRec * desktopWindow = (DesktopRec *)client_data;

   _ChangeIconName (w, True, desktopWindow->file_view_data, desktopWindow,
                    desktopWindow->host, desktopWindow->dir_linked_to);
}


/*====================================================================
 *
 * MakeFile
 *      Run a background process to create a file or directory.
 *
 *==================================================================*/

/*--------------------------------------------------------------------
 *  GetTarget
 *    Build a host, directory, and file name path to be used as the
 *    destination of a create, copy or move operation.
 *------------------------------------------------------------------*/

void
GetTarget(
        char *from_host,
        char *from_dir,
        char *new_name,
        char *to_host,
        char *to_dir,
        char *to_file)
{
   char *ptr;

   strcpy(to_host, from_host);

   if (strncmp (new_name, "/", 1) == 0)
   {
     strcpy(to_dir, new_name);
   }
   else
   {
     if (strcmp(to_dir, "/") == 0)
       sprintf(to_dir, "%s%s", from_dir, new_name);
     else
       sprintf(to_dir, "%s/%s", from_dir, new_name);
   }

   ptr = strrchr(to_dir, '/');
   *ptr = '\0';
   strcpy(to_file, ptr + 1);
}


/*--------------------------------------------------------------------
 * MakeFileProcess
 *   Main routine of background process for MakeFile
 *------------------------------------------------------------------*/

static int
MakeFileProcess(
        int pipe_fd,
        char *to_host,
        char *to_dir,
        char *to_file,
        unsigned char type)
{
   char *to;
   struct stat stat_buf;
   char *dir_path;
   long modify_time;
   short pipe_msg;
   int fnew;
   int rc;
   Tt_status tt_status;

   /* assume success */
   rc = 0;

   /* get the full name of the file/dir to be created */
   to = ResolveLocalPathName(to_host, to_dir, to_file, home_host_name, &tt_status);
   if (to == NULL)
      rc = EINVAL;

   /* check if a file/dir with the same name already exists */
   else if (lstat (to, &stat_buf) == 0)
      rc = EEXIST;

   /* now create the file/dir */
   else
   {
      /* send a modified message back through the pipe */
      modify_time = 0;
      dir_path = ResolveLocalPathName(to_host, to_dir, NULL, home_host_name, &tt_status);
      if (stat(dir_path, &stat_buf) == 0)
         modify_time = stat_buf.st_mtime;
      XtFree(dir_path);

      pipe_msg = PIPEMSG_FILE_MODIFIED;
      write(pipe_fd, &pipe_msg, sizeof(short));
      write(pipe_fd, &modify_time, sizeof(long));

      /* do the work */
      if (type == DtDIRECTORY)
      {
         if (mkdir (to, (int) DtFILE_DIR_CREATION_MASK) != 0)
            rc = errno;
      }
      else
      {
         unsigned int mode;

         if (type == DtDATA)
            mode = DtFILE_DATA_CREATION_MASK;
         else
            mode = DtFILE_OTHER_CREATION_MASK;

         if ((fnew = creat(to, (int) mode)) < 0)
            rc = errno;
         else
            close(fnew);
      }
   }

   /* send a 'done' msg through the pipe */
   pipe_msg = PIPEMSG_DONE;
   DPRINTF(("MakeFileProcess: sending DONE, rc %d\n", rc));
   write(pipe_fd, &pipe_msg, sizeof(short));
   write(pipe_fd, &rc, sizeof(int));
   PipeWriteString(pipe_fd, to);

   XtFree(to);

   return rc;
}


/*--------------------------------------------------------------------
 * MakeFilePipeCB:
 *   Read and process data sent through the pipe.
 *------------------------------------------------------------------*/

static void
MakeFilePipeCB(
   XtPointer client_data,
   int *fd,
   XtInputId *id)
{
   MakeFileCBData *cb_data = (MakeFileCBData *)client_data;
   short pipe_msg;
   int n;
   long modify_time;
   char *full_name;
   int rc;

   /* read the msg from the pipe */
   pipe_msg = -1;
   n = PipeRead(*fd, &pipe_msg, sizeof(short));

   if (pipe_msg == PIPEMSG_FILE_MODIFIED)
   {
      /* get modify time */
      PipeRead(*fd, &modify_time, sizeof(long));

      /* mark the file updated in the directory cache */
      if (modify_time != 0)
         DirectoryModifyTime(cb_data->to_host, cb_data->to_dir, modify_time);
      DirectoryFileModified(cb_data->to_host, cb_data->to_dir,
                            cb_data->to_file);
      return;
   }

   if (pipe_msg == PIPEMSG_DONE)
   {
      PipeRead(*fd, &rc, sizeof(int));
      full_name = PipeReadString(*fd);
   }
   else
   {
      fprintf(stderr, "Internal error in MakeFilePipeCB: bad pipe_msg %d\n",
              pipe_msg);
      rc = -1;
      full_name = NULL;
   }

   DPRINTF(("MakeFilePipeCB: n %d, pipe_msg %d, rc %d\n", n, pipe_msg, rc));

   /* arrange for the modified directory to be updated */
   DirectoryEndModify(cb_data->to_host, cb_data->to_dir);

   /* close the pipe and cancel the callback */
   close(*fd);
   XtRemoveInput(*id);

   /* Store away the newly created file so we later on we can
      scroll to it.
   */
   {
     MakeFileDoneData * data = (MakeFileDoneData *)cb_data->callback_data;
     DialogCallbackStruct * call_struct = data->call_struct;
     FileMgrData * file_mgr_data = call_struct->file_mgr_data;

     file_mgr_data->scrollToThisDirectory = cb_data->to_dir;
     file_mgr_data->scrollToThisFile = cb_data->to_file;
   }

   /* call the callback routine */
   if (cb_data->finish_callback)
      (*cb_data->finish_callback)(cb_data->callback_data, full_name, rc);

   /* free callback data */
   XtFree(full_name);
   XtFree(cb_data->to_host);

   /* Don't free to_dir and to_file. We used this to remember
      which file so later on we can scroll to it.
   XtFree(cb_data->to_dir);
   XtFree(cb_data->to_file);
   */

   XtFree((char *)cb_data);
}


/*--------------------------------------------------------------------
 * MakeFile:
 *    Start the background process and set up callback for the pipe.
 *------------------------------------------------------------------*/

void
MakeFile(
        Widget w,
        char *host_name,
        char *directory_name,
        char *new_name,
        unsigned char type,
        void (*finish_callback)(),
        XtPointer callback_data)
{
   static char *pname = "MakeFile";
   MakeFileCBData *cb_data;
   char to_host[MAX_PATH];
   char to_dir[MAX_PATH];
   char to_file[MAX_PATH];
   int pipe_fd[2];
   int pid;
   int rc;

   /* get host & path of the target file */
   GetTarget(host_name, directory_name, new_name, to_host, to_dir, to_file);

   /* mark the target directory as being modified in the directory cache */
   DirectoryBeginModify(to_host, to_dir);

   /* parent: set up callback to get the pipe data */
   cb_data = XtNew(MakeFileCBData);
   cb_data->to_host = XtNewString(to_host);
   cb_data->to_dir = XtNewString(to_dir);
   cb_data->to_file = XtNewString(to_file);
   cb_data->finish_callback = finish_callback;
   cb_data->callback_data = callback_data;

   /* create a pipe */
   pipe(pipe_fd);

   /* fork the process that does the actual work */
   pid = fork();
   if (pid == -1)
   {
       DirectoryAbortModify(to_host, to_dir);
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

      rc = MakeFileProcess(pipe_fd[1], to_host, to_dir, to_file, type);
      close(pipe_fd[1]);

      DBGFORK(("%s:  child exiting\n", pname));

      exit(rc);
   }

   DBGFORK(("%s:  forked child<%d>, pipe %d\n", pname, pid, pipe_fd[0]));

   /* parent: set up callback to get the pipe data */
   close(pipe_fd[1]);  /* parent won't write the pipe */

   cb_data->child = pid;

   XtAppAddInput(XtWidgetToApplicationContext(toplevel),
                 pipe_fd[0], (XtPointer)XtInputReadMask,
                 MakeFilePipeCB, (XtPointer)cb_data);
}


/*=============================================================
 *
 * The following routines handle the creation of files from
 * buffers: MakeFilesFromBuffers and MakeFilesFromBuffersDT
 * follow
 *
 *=============================================================*/

Boolean
MakeFilesFromBuffers(
                     FileMgrData *file_mgr_data,
                     char * directory,
                     char *host,
                     char **file_set,
                     char **host_set,
                     BufferInfo *buffer_set,
                     int num_of_buffers,
                     void (*finish_callback)(),
                     XtPointer callback_data)
{
  return _FileMoveCopy ((XtPointer)file_mgr_data, NULL, directory, host,
                        host_set, file_set, buffer_set, num_of_buffers,
                        0, NULL, finish_callback, callback_data);
}


Boolean
MakeFilesFromBuffersDT(
                     FileViewData *file_view_data,
                     char * directory,
                     char **file_set,
                     char **host_set,
                     BufferInfo *buffer_set,
                     int num_of_buffers,
                     DesktopRec *desktopWindow,
                     void (*finish_callback)(),
                     XtPointer callback_data)
{
  return _FileMoveCopy ((XtPointer)file_view_data, NULL, directory,
                        home_host_name,
                        host_set, file_set, buffer_set, num_of_buffers,
                        0, desktopWindow, finish_callback, callback_data);
}


/*====================================================================
 *
 * CreateFileFromBuffer
 *     Routine to create a file from a buffer
 *
 *==================================================================*/


static Boolean
CreateFileFromBuffer(int    pipe_s2m,
	             char   *directory,
	             char   *fully_qualified_name,
	             void   *buffer,
	             int    size)
{
  int fnew;
  int rc=0;
  unsigned int mode;
  Boolean BufferIsExecutable=FALSE;
  char *err_msg, *err_arg, *tmpStr;
  char *format_str, *strerror_str;
  int format_param_len=20;
  int savedError = 0;




  /* Set the permissions depending if buffer is a */
  /* file or an execuatable                       */
  if (_DtIsBufferExecutable(buffer,size))
  {
   mode = S_IRUSR | S_IWUSR | S_IXUSR |
	  S_IRGRP | S_IWGRP | S_IXGRP |
	  S_IROTH | S_IWOTH | S_IXOTH;
  }
  else
  {
   mode = S_IRUSR | S_IWUSR |
	  S_IRGRP | S_IWGRP |
	  S_IROTH | S_IWOTH;
  }

  /* Create the target file */
  if ((fnew = open(fully_qualified_name, O_CREAT| O_WRONLY, mode)) < 0)
  {
    DPRINTF(("CreateBufferFromFile: Could not create %s\n",
	      fully_qualified_name));
    savedError = errno;
    rc = -1;
  }
  else
  {
    /* Write the buffer to the target file */
    if ((rc = write(fnew, buffer, size)) < 0 )
    {
      DPRINTF (("CreateBufferFromFile: Could not write buffer to %s\n",
	         fully_qualified_name));
      savedError=errno;
    }
    else
    {
      close(fnew);
      DPRINTF (("CreateBuffeFromFile: Target file %s created\n",
	         fully_qualified_name));
    }
  }

  /* Handle errors */
  if (rc < 0)
  {
    switch (savedError)
    {
       case EACCES:
	  tmpStr = GetSharedMessage(CANT_CREATE_ERROR);
	  err_msg  = XtNewString(tmpStr);
	  err_arg  = XtNewString(fully_qualified_name);

	  DPRINTF (("CreateBufferFromFile: EACCESS errno is %d\n",errno));
	  break;
       default :
	  err_msg = strerror(savedError);
	  err_arg = NULL;

	  DPRINTF (("CreateBufferFromFile: %s\n", err_msg));
    } /* endswitch */

    /* Write error message on pipe so */
    /* that the parent process will */
    /* display a dialog             */
    PipeWriteErrmsg(pipe_s2m, rc, err_msg, err_arg);
    if (err_msg) XtFree(err_msg);
    if (err_arg) XtFree(err_arg);
    return FALSE;
  }
  else
  {
    return TRUE;
  }

}

/*
 * DisplayDuplicateOpError - Used in FileOpPipeCallback when a duplicate
 * operation like move/copy/link a file onto the same file is performed,
 * this routine gets called and displays an error message.
 */

static void
DisplayDuplicateOpError(
    FileOpCBData *cb_data,
    int index)

{
  char *msgptr,*err_msg,*title,*tchar;
  Widget dialogwidget;

  if (cb_data->mode == MOVE_FILE)
  {
    if(cb_data->callback_data == NULL)
      if(initiating_view == NULL)
        return;
      else
      {
        title = XtCalloc(1,strlen(GETMESSAGE(9,73,"Move"))+
                strlen(GETMESSAGE(9,94,"Error"))+5);
        tchar = XtNewString(GETMESSAGE(9,73,"Move"));
        sprintf(title,"%s %s",tchar,GETMESSAGE(9,94,"Error"));
        XtFree(tchar);
        err_msg = GETMESSAGE(11,135,"Cannot move object %s onto itself");
    }
  }
  else if (cb_data->mode == COPY_FILE)
  {
    title = XtCalloc(1,strlen(GETMESSAGE(9,72,"Copy"))+
		strlen(GETMESSAGE(9,94,"Error"))+5);
    tchar = XtNewString(GETMESSAGE(9,72,"Copy"));
    sprintf(title,"%s %s",tchar,GETMESSAGE(9,94,"Error"));
    XtFree(tchar);
    if(cb_data->callback_data == NULL)
    {
        if(cb_data->file_mgr_data && cb_data->file_mgr_data->toolbox)
        {
            XtFree(title);
            return;
        }
        else
            err_msg = GETMESSAGE(11,136,"Cannot copy object %s onto itself");
    }
    else
    {
      err_msg = GETMESSAGE(11,45,"No Copy Operation performed on object %s.\nYou must change either the Destination Folder\nor the Name for Copy before a copy can be created");
    }
  }
  else
  {
    title = XtCalloc(1,strlen(GETMESSAGE(9,74,"Link"))+
		strlen(GETMESSAGE(9,94,"Error"))+5);
    tchar = XtNewString(GETMESSAGE(9,74,"Link"));
    sprintf(title,"%s %s",tchar,GETMESSAGE(9,94,"Error"));
    XtFree(tchar);
    if(cb_data->callback_data == NULL)
    {
      err_msg = GETMESSAGE(11,137,"Cannot link object %s onto itself");
    }
    else
    {
      err_msg = GETMESSAGE(11,46,"No Link Operation performed on object %s.\nYou must change either the Destination Folder\nor the Name for Copy before a link can be created");
    }
  }

  msgptr = XtCalloc(1,strlen(err_msg)+strlen(cb_data->updates[index].file)+10);
  sprintf(msgptr,err_msg,cb_data->updates[index].file);

  if(cb_data->callback_data)
    dialogwidget  = ((RenameDoneData *)(cb_data->callback_data))->w;
  else
    dialogwidget  = toplevel;
  _DtMessage (dialogwidget,title, msgptr, NULL, HelpRequestCB);
  XtFree(msgptr);
  XtFree(title);
}



/*==============================================================
 *
 *  appendErrorMessage
 *
 *  if "arg" is not null, "new" is assumed to include %s
 *  "message" is re-allocated and so will have a different
 *      address than when the function was called
 *
 *  usage:  errorMsg = appendErrorMessage(errorMsg,errStr,file);
 *
 *==============================================================
 */

static String
appendErrorMessage(String message, String new, String arg)
{
   String newMessage;

   if (arg == NULL)
      newMessage = XtNewString(new);
   else
   {
      newMessage = XtMalloc(strlen(new) + strlen(arg) + 1);
      sprintf(newMessage,new,arg);
   }

   if (message == NULL)
   {
      message = XtRealloc(message, (strlen(newMessage) + 2));
      *message = '\0';
   }
   else
      message = XtRealloc(message, (strlen(message) + strlen(newMessage) + 2));

   strcat(message,newMessage);
   strcat(message,"\n");

   XtFree(newMessage);

   return message;

}  /* end appendErrorMessage */

static void
DisplayErrorMessage(
  int pipe_s2m,
  char *errMsg,
  char *from,
  char *tdir)
{
  char *localstr;

  localstr = (char *) XtMalloc(strlen(errMsg)+strlen(from)+strlen(tdir) + 10 );
  sprintf(localstr,errMsg,tdir,from);
  PipeWriteErrmsg(pipe_s2m, -1, localstr, NULL);
  XtFree(localstr);
}
static Boolean
IsInParentDir(
        char *source_path ,
        char *dest_path )
{
   char filename [MAX_PATH];
   char *cptr;
   int slen = strlen(source_path);

   if(slen > strlen(dest_path))
     return False;
   strcpy (filename, dest_path);
   cptr = strchr(&filename[slen-1],'/');
   if(cptr)
     *cptr = '\0';
   return ((strcmp(source_path,filename) == 0)?True:False);
}
