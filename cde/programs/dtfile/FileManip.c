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
/* $TOG: FileManip.c /main/10 1999/12/09 13:06:10 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           FileManip.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    This module does the copy, move and rename commands.
 *                   Included are functions to control command execution,
 *                   move a directory, get the directory portion of a pathname,
 *                   get the file name portion of a pathname, and check if a
 *                   folder is to be moved within itself.
 *
 *   FUNCTIONS: Check
 *		CheckAccess
 *		CopyDir
 *		DName
 *		FileManip
 *		FileOperationError
 *		MoveDir
 *		defined
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#if defined(SVR4) || defined(sco)
#  if defined(USL) || defined(sco) || defined(__uxp__)
#    include <sys/param.h>
#    include <sys/types.h>
#  endif
#  ifdef sco
#    include <sys/fs/s5param.h>
#    define ROOTINO S5ROOTINO
#  else
#    include <sys/fs/ufs_fs.h>
#    define ROOTINO UFSROOTINO
#  endif	/* sco */
#else
#  if defined(linux) || defined(CSRG_BASED)
#    define ROOTINO 2
#  endif
#  include <sys/param.h>
#endif	/* SVR4 || sco */

#include <sys/types.h>

#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

/** The following ifdefs need to be straightened out
 ** They are a mess.
 **/
#ifdef sun
#include <unistd.h>
#include <limits.h>
#ifndef SVR4
#include <ufs/fs.h>
#endif
#else
#ifdef __hp_osf
#include <unistd.h>
#include <limits.h>
#include <ufs/fs.h>
#include <sys/access.h>
#else
#ifdef __ultrix
#include <unistd.h>
#include <limits.h>
#include <ufs/fs.h>
#else
#include <unistd.h>
#include <limits.h>
#ifdef __hpux
#include <unistd.h>
#endif
#endif /* __ultrix */
#endif /* __hp_osf */
#endif /* sun */

#ifdef __osf__
#include <ufs/fs.h>
#endif

#include <Xm/Xm.h>

#include <Xm/MwmUtil.h>

#include <Dt/DtP.h>                     /* required for DtDirPaths type */
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/SharedProcs.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Help.h"
#include "SharedMsgs.h"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif

/*  Local Function Definitions  */
static char * MOVE_CMD = "/bin/mv";
static char * LINK_CMD = "/bin/ln";
static char * REMOVE_CMD = "/bin/rm";
static char * DTCOPY = CDE_INSTALLATION_TOP "/bin/dtfile_copy";


/************************************************************************
 *
 *  CheckAccess
 *
 ************************************************************************/
int
CheckAccess(
        char *fname,
        int what)
{
    int access_priv;
    uid_t save_ruid;
    gid_t save_rgid;

#if defined(__hp_osf) || defined(__ultrix)
/*--------------------------------------------------------------------
 * access code for __hp_osf, __ultrix
 *------------------------------------------------------------------*/

      setreuid(geteuid(),-1);
      return access (fname, what);

#else
#ifdef BLS
/*--------------------------------------------------------------------
 * access code for BLS
 *------------------------------------------------------------------*/

      setresuid(geteuid(),-1,-1);
      return access (fname, what);

#else /* the rest of the OS's */

   save_ruid = getuid();

#ifdef _AIX
   setreuid(geteuid(),-1);
#else
   setuid(geteuid());
#endif /* _AIX */

   save_rgid = getgid();

#ifdef _AIX
   setregid(getegid(),-1);
#else
   setgid(getegid());
#endif /* _AIX */

   access_priv = access (fname, what);

#ifdef _AIX
   setreuid(save_ruid,-1);
   setregid(save_rgid,-1);
#else
   setuid(save_ruid);
   setgid(save_rgid);
#endif /* _AIX */

   return access_priv;
#endif /* BLS */
#endif /* Apollo & OSF */
}


/************************************************************************
 *
 *  FileOperationError
 *	Display an error message.
 *
 ************************************************************************/
void
FileOperationError(
        Widget w,
        char *message1,
        char *message2 )
{
   char *message_buf;
   char * title;
   char * tmpStr;

  if (message2 != NULL)
   {
     message_buf = XtMalloc(strlen(message1) + strlen(message2) + 1);
     (void) sprintf(message_buf,message1, message2);
   }
   else
   {
     message_buf = XtMalloc(strlen(message1) + 1);
     (void) sprintf(message_buf, "%s", message1);
   }

   /*  Display an error dialog  */
   tmpStr = GetSharedMessage(FILE_MANIPULATION_ERROR_TITLE);
   title = XtNewString(tmpStr);
   _DtMessage (w, title, message_buf, NULL, HelpRequestCB);
   XtFree(title);
   XtFree(message_buf);
}


/************************************************************************
 *
 *  DName
 *	Returns the file name of its argument.
 *      Keep looking thru the string until a "/" is found which still
 *      has some characters after it.
 *
 ************************************************************************/
char *
DName(
        register char *name )
{
   char * p;
   char * q;

   p = q = name;

   while (1)
   {
      q = DtStrchr(q, '/');
      if ((q) && *(q+1))
         p = q + 1;

      if (q == NULL)
         break;

      q++;
   }

   return(p);
}


/************************************************************************
 *
 *  Check
 *
 ************************************************************************/
static int
Check(
        Widget w,
        register char *spth,
        register ino_t dinode,
        int mode,
        void (*errorHandler)() )
{
   struct stat sbuf;
   char filename [MAX_PATH];
   char * msg;
   char * tmpStr;

   sbuf.st_ino = 0;

   (void) strcpy (filename, spth);

   while (sbuf.st_ino != ROOTINO)
   {
      if (lstat (filename, &sbuf) < 0)
      {
         if (errorHandler)
         {
            tmpStr = (GETMESSAGE(11,33, "Cannot open %s"));
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, filename);
            XtFree(msg);
         }
         return (False);
      }

      if (sbuf.st_ino == dinode)
      {
         if (errorHandler)
         {
            if (mode == COPY_FILE)
              tmpStr = GETMESSAGE(11,35, "Cannot copy a folder into itself.");
            else
              tmpStr = GETMESSAGE(11,16, "A folder cannot be moved into itself.\n%s");
            msg = XtNewString(tmpStr);
            if (mode == COPY_FILE)
              (*errorHandler) (w, msg, NULL);
            else
              (*errorHandler) (w, msg, filename);
            XtFree(msg);
         }
         return(1);
      }

      (void) strcat (filename, "/..");
   }

   return(0);
}


/************************************************************************
 *
 *  MoveDir
 *
 ************************************************************************/

static Boolean
MoveDir(
        Widget w,
        register char *source,
        register char *target,
        struct stat *sourceStatInfo,
        void (*errorHandler)(),
        char ** targetRtn ,
        int type )
{
   static char *pname = "MoveDir";
   register char *p;

   char * targetDir;            /* original target dir path */
   char *link_path;
   int link_result;

   struct stat  s1;             /* status of from file */
   struct stat  s2;             /* status of to file   */
   char * cptr;
   int len, val, val1;

   static char buf [BUF_SIZE];  /* generic buffer */
   char filename [MAX_PATH];    /* buffer to hold the full file name */
   char * msg;
   char * tmpStr;
   int child_pid, rc;

   /* Copy target so we have it for an error dialog if we need it */
   targetDir = XtNewString(target);
   *targetRtn = NULL;

   if ((val = stat (target, &s2)) < 0)
      val = lstat(target, &s2);

   /* Check if move to itself */
   if( sourceStatInfo->st_dev == s2.st_dev)
   {
     if (Check (w, target, sourceStatInfo->st_ino, MOVE_FILE, NULL ))
       return (False);
   }

   if (val >= 0) /* target exists */
   {
      if ((s2.st_mode & S_IFMT) != S_IFDIR) /* target not directory */
      {
         if (errorHandler)
         {
            char * tmpStr;
            tmpStr = GetSharedMessage(CANT_OVERWRITE_ERROR);
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, target);
            XtFree(msg);
         }
         XtFree(targetDir);
         return (False);
      }

      (void) strcpy (buf, target);
      target = buf;
      *targetRtn = buf;

      DtLastChar(buf, &cptr, &len);
      if ((len != 1) || (*cptr != '/'))
         (void) strcat (buf, "/");
      (void) strcat (buf, DName (source));

      if (lstat (target, &s2) >= 0) /* new target exists */
      {
         if (errorHandler)
         {
            char * tmpStr;

            tmpStr = GetSharedMessage(CANT_OVERWRITE_ERROR);
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, target);
            XtFree(msg);
         }
         XtFree(targetDir);
         return (False);
      }
   }


   p = DName (source);

   /*  don't rename these  */
   DtLastChar(p, &cptr, &len);

   if (!strcmp (p, ".") || !strcmp (p, "..") ||
       !strcmp (p, "")  || ((len == 1) && (*cptr == '/')))
   {
      if (errorHandler)
      {
         char * tmpStr;

         tmpStr = (GETMESSAGE(11,32, "Cannot rename %s"));
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, p);
         XtFree(msg);
      }
      XtFree(targetDir);
      return (False);
   }


   /*  parent doesn't exist  */
   if((val = stat (_DtPName (source), &s1)) < 0)
      val = lstat (_DtPName (source), &s1);

   if((val1 = stat (_DtPName (target), &s2)) < 0)
      val1 = lstat (_DtPName (target), &s2);

   if (val < 0 || val1 < 0)
   {
      if (errorHandler)
      {
         tmpStr = GETMESSAGE(11, 14, "Cannot find the folders location.");
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, NULL);
         XtFree(msg);
      }
      XtFree(targetDir);
      return (False);
   }


   /*  check for target parent not writeable  */
   if (CheckAccess(_DtPName (target), W_OK) == -1)
   {
      if (errorHandler)
      {
         char * tmpStr;

         tmpStr = GetSharedMessage(CANT_WRITE_ERROR);
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, targetDir);
         XtFree(msg);
      }
      XtFree(targetDir);
      return (False);
   }


   /* check for source parent not writeable */
   if (CheckAccess(_DtPName (source), W_OK) == -1)
   {
      if (errorHandler)
      {
         char * tmpStr;

         tmpStr = GetSharedMessage(CANT_WRITE_ERROR);
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, source);
         XtFree(msg);
      }
      XtFree(targetDir);
      return (False);
   }
   /*
     if (((sourceStatInfo->st_mode & S_IFMT) == S_IFDIR) &&
     (CheckAccess(source, W_OK) != 0))
     {
     if (errorHandler)
     {
     char * tmpStr;

     tmpStr=GETMESSAGE(11, 57,"You do not have permission to move the folder\n%s\nWrite permission is required.");
     msg = XtMalloc(strlen(tmpStr) + strlen(source) + 2);
     sprintf(msg, tmpStr, source);
     (*errorHandler) (w, msg, source);
     XtFree(msg);
     }
     XtFree(targetDir);
     return (False);
     }
     */
   /*  if parents are not on the same device, do a copy & delete */
   if (s1.st_dev != s2.st_dev)
   {
      /* Determine correct Geometry Placement fo Move Dialog */
      /* @@@ ... to be added */

      child_pid = fork();
      if (child_pid == -1)
      {
         if (errorHandler)
         {
            tmpStr = GETMESSAGE(11, 39, "Cannot create child process.\nThe maximum number of processes for this system has been reached.\nStop some of the processes or programs that are currently\nrunning and then retry this function.");
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, NULL);
            XtFree(msg);
         }
         XtFree(targetDir);
         return False;
      }

      if (child_pid == 0)
      {
	 DBGFORK(("%s:  child forked\n", pname));

         /* pass in geometry, and other command lines params when available */
	 if(type == TRASH_DIRECTORY)
           rc = execlp(DTCOPY, "dtfile_copy", "-move", "-confirmReplace",
		 "-confirmErrors", "-popDown","-checkPerms", source, target, 0);
	 else
           rc = execlp(DTCOPY, "dtfile_copy", "-move", "-confirmReplace",
		 "-confirmErrors", "-popDown", source, target, 0);

         /* call errorhandler */
         perror ("Could not exec child process \"dtfile_copy\"");

	 DBGFORK(("%s:  child exiting\n", pname));

         exit (1);
      }

      DBGFORK(("%s:  forked child<%d>\n", pname, child_pid));


      XtFree(targetDir);
      return (True);
   }

   link_path = _DtFollowLink(source);


   if (s1.st_ino != s2.st_ino)
   { /*  different parent inodes  */
     (void) lstat (source, &s1); /* get source dir ino */


     if (Check (w, _DtPName (target), s1.st_ino, MOVE_FILE, errorHandler))
     { /* move into self */
       XtFree(targetDir);
       return(False);
     }
   }

/*   This part of code was implemented with the idea that the links
     to be treated differently.  So, it has to be uncommented whenever
     links are handled differently (i.e., moving a link shall move the
     absolute object.

   if(strcmp(link_path, source) != 0)
   {
     if (RunFileCommand (MOVE_CMD, link_path, target, NULL) == 0)
     {
       XtFree(targetDir);
       return (True);
     }
   }
   else
*/
   {
     if (RunFileCommand (MOVE_CMD, source, target, NULL) == 0)
     {
       XtFree(targetDir);
       return (True);
     }
   }

   XtFree(targetDir);
   return (False);
}


/************************************************************************
 *
 *  CopyDir
 *
 ************************************************************************/
static Boolean
CopyDir(
        Widget w,
        int mode,
        register char *from,
        register char *to,
        Boolean  isContainer,
        struct stat *s1,
        void (*errorHandler)(),
        Boolean checkForBusyDir,
        int type )
{
   static char *pname = "CopyDir";
   char * cptr;
   int len;
   char target [MAX_PATH];	/* buffer to hold the full file name */
   char target_dir [MAX_PATH], target_file [MAX_PATH];
   struct stat  s2;             /* status of to file   */
   int child_pid, rc, target_rc;
   char *msg, *tmpStr;

   /* Check if source is readable */
   if (CheckAccess(from, R_OK) == -1)
   {
      if (errorHandler)
      {
         tmpStr = GetSharedMessage(CANT_READ_ERROR);
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, from);
         XtFree(msg);
      }
      return (False);
   }

   /* generate target name */
   /* "to" can be something to copy the source into (isContainer=TRUE)     */
   /*    or it can be the full path of the destination (isContainer=FALSE) */
   /* the former case is probably more common (e.g. a drag&drop copy)      */
   /*    whereas the second case occurs when, for example, the menu is     */
   /*    to copy directory /u/joe/a to /u/joe/b (not doable with d&d)      */
   (void) strcpy (target, to);
   if (isContainer)
   {
      DtLastChar(to, &cptr, &len);
      if ((len != 1) || (*cptr != '/'))
          (void) strcat (target, "/");
      (void) strcat (target, DName (from));
   }
   split_path(target, target_dir, target_file);

   /* Check if target directory exists */
   if ((target_rc = stat (target, &s2)) < 0)
      target_rc = lstat(target, &s2);

   if (target_rc >= 0)
   {
      /* target exists:
       * make sure it's a directory */

      if ((s2.st_mode & S_IFMT) != S_IFDIR) 	/* target not directory */
      {
         if (errorHandler)
         {
            tmpStr = GetSharedMessage(CANT_OVERWRITE_ERROR);
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, target);
            XtFree(msg);
         }
         return (False);
      }
   }
   else
   {
     /* target does not exist:
      * make sure the "to" directory exists and is writable */
     if ((rc = stat (target_dir, &s2)) < 0)
        rc = lstat(target_dir, &s2);

     if (rc < 0 || (s2.st_mode & S_IFMT) != S_IFDIR)
     {
        if (errorHandler)
        {
           tmpStr = GETMESSAGE(11, 14, "Cannot find the folders location.");
           msg = XtNewString(tmpStr);
           (*errorHandler) (w, msg, NULL);
           XtFree(msg);
        }
        return (False);
     }

     if (CheckAccess(target_dir, W_OK) == -1)
     {
        if (errorHandler)
        {
           tmpStr = GetSharedMessage(CANT_WRITE_ERROR);
           msg = XtNewString(tmpStr);
           (*errorHandler) (w, msg, to);
           XtFree(msg);
        }
        return (False);
     }
   }

   /* Determine if we are attempting a copy into self */
   if (s1->st_dev == s2.st_dev)
   {
     if (target_rc >= 0)
     {
        if (Check (w, to, s1->st_ino, COPY_FILE, errorHandler))
           return False;
     }
     else  /* destination dir does not exist, look at its proposed parent */
     {
        if (Check (w, target_dir, s1->st_ino, COPY_FILE, errorHandler))
           return False;
     }
   }

   /* Determine correct Geometry Placement fo Copy Dialog */
   /* @@@ ... to be added */

   /* If all the above checks have passed, then fork off the copy dialog */

   child_pid = fork();
   if (child_pid == -1)
   {
      if (errorHandler)
      {
         tmpStr = GETMESSAGE(11, 39, "Cannot create child process.\nThe maximum number of processes for this system has been reached.\nStop some of the processes or programs that are currently\nrunning and then retry this function.");
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, NULL);
         XtFree(msg);
      }
      return False;
   }

   if (child_pid == 0)
   {
      DBGFORK(("%s:  child forked\n", pname));

      /* pass in geometry, and other command lines params when available */
      if (mode == MERGE_DIR)
        /* merge source & target directories */
        rc = execlp(DTCOPY, "dtfile_copy",
                     "-dontDelete", "-forceCopies", "-copyTop",
                     "-confirmReplace", "-confirmErrors", "-popDown",
                     from, target, 0);
      else
         /* replace target dir */
         rc = execlp(DTCOPY, "dtfile_copy",
                     "-forceCopies", "-copyTop",
                     "-confirmErrors", "-popDown",
                     from, target, 0);

      /* call errorhandler */
      perror ("Could not exec child process \"dtfile_copy\"");

      DBGFORK(("%s:  child exiting\n", pname));

      exit (1);
   }

   DBGFORK(("%s:  forked child<%d>\n", pname, child_pid));


   return TRUE;
}


/************************************************************************
 *
 *  FileManip
 *
 ************************************************************************/
Boolean
FileManip(
        Widget w,
        int mode,
        register char *from,
        register char *to,
        Boolean  isContainer,          /* described in function CopyDir */
        void (*errorHandler)(),
        Boolean checkForBusyDir,
        int type )
{
   register int fold;
   register int fnew;
   register int n;
   Boolean copy_dir_return;
   Boolean move_dir_return;
   void (*oldInt)();
   void (*oldQuit)();
   void (*oldPipe)();
   void (*oldTerm)();
   char * cptr;
   int len;
   Boolean restricted = False;
   Boolean fileExists = False;

   struct stat s1;      /* status of from file e.g. lstat info */
   struct stat s4;      /* status of from file e.g. stat info  */
   struct stat s2;      /* status of to file e.g.   stat info  */
   struct stat s3;      /* status of to file e.g.   lstat info */

   char buf [BLOCK_SIZE];		/* generic buffer */
   char filename [MAX_PATH];		/* buffer to hold the full file name */
   char * msg;
   int link_result;
   char * realTarget;
   char * tmpStr;


   /* Check the <from> part of the command:
    *
    * Report error if <from> doesn't exist.
    * Else error if <from> hasn't read access.
    */

   DPRINTF(("FileManip: mode %d type %d from \"%s\" to \"%s\"\n", mode, type, from, to));
   if (stat (from, &s1) < 0)
   {
      if (lstat (from, &s1) < 0)
      {
         if (errorHandler)
         {
            tmpStr = (GETMESSAGE(11,28, "%s cannot be found."));
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, from);
            XtFree(msg);
         }
         return (False);
      }
   }

   /* We will check if we need to initiate a copy of a directory */
   if ((s1.st_mode & S_IFMT) == S_IFDIR)	/* from is a directory */
   {
      if (mode == COPY_FILE  ||  mode == MERGE_DIR)
      {
         oldInt = signal (SIGINT, SIG_IGN);
         oldQuit = signal (SIGQUIT, SIG_IGN);
         oldPipe = signal (SIGPIPE, SIG_IGN);
         oldTerm = signal (SIGTERM, SIG_IGN);

         copy_dir_return = CopyDir(w, mode, from, to, isContainer, &s1,
                                   errorHandler, checkForBusyDir, type);

         (void) signal (SIGINT, oldInt);
         (void) signal (SIGQUIT, oldQuit);
         (void) signal (SIGPIPE, oldPipe);
         (void) signal (SIGTERM, oldTerm);

         return (copy_dir_return);
      }


      /*  If the directory has more than one open view or open views  */
      /*  of sub directories.  Then do not allow the move or rename.  */

      if (mode == MOVE_FILE)
      {
         if (checkForBusyDir && DirectoryBusy (from))
         {
            if (errorHandler)
            {
               char message_buf[512];
               char * tmpStr;

               message_buf[0] = '\0';
               tmpStr = (GETMESSAGE(11,30, "Cannot move or rename the folder %s.\nAll File Manager views displayed for a folder or its sub-folders\nmust be closed before a folder can be moved or renamed."));
               sprintf (message_buf, tmpStr, from);
               (*errorHandler) (w, message_buf, NULL);
            }
            return (False);
         }


         oldInt = signal (SIGINT, SIG_IGN);
         oldQuit = signal (SIGQUIT, SIG_IGN);
         oldPipe = signal (SIGPIPE, SIG_IGN);
         oldTerm = signal (SIGTERM, SIG_IGN);

         move_dir_return = MoveDir (w, from, to, &s1, errorHandler, &realTarget,type);

         (void) signal (SIGINT, oldInt);
         (void) signal (SIGQUIT, oldQuit);
         (void) signal (SIGPIPE, oldPipe);
         (void) signal (SIGTERM, oldTerm);

         return (move_dir_return);
      }

      if (mode == LINK_FILE)
      {
         /* Need to append the directory name on */
         (void) strcpy(filename, to);
         if(type == DESKTOP)
         {
             char *tmp, *ptr;

             tmp = (char *)XtMalloc(strlen(filename) + 1);
             strcpy(tmp, filename);
             /* get the workspace number first */
             ptr = strrchr(tmp, '/');
             *ptr = '\0';
             /* now get the Desktop */
             ptr = strrchr(tmp, '/');

             /* if we don't get "/Desktop" then there is another filename
                attached to the end of the to name passed in */
             if(strcmp(ptr, "/Desktop") != 0)
                restricted = True;
             XtFree(tmp);
         }

         if( (!restricted && type != TRASH_DIRECTORY) && isContainer)
         {
            DtLastChar(to, &cptr, &len);
            if ((len != 1) || (*cptr != '/'))
               (void) strcat(filename, "/");
         }

         if(strcmp(from, "/.") == 0 || strcmp(from, "/") == 0)
         {
            (void) strcat(filename, home_host_name);
            (void) strcat(filename, ":");
            (void) strcat(filename, root_title);
         }
         else if ( (!restricted && type != TRASH_DIRECTORY) && isContainer)
           (void) strcat(filename, DName(from));

	 to = filename;

         if (((link_result = symlink(from, to)) != 0) && errorHandler)
         {
            if(type == NOT_DESKTOP || errno != EEXIST )
            {
               char * tmpStr;

               tmpStr = GetSharedMessage(CANT_CREATE_ERROR);
               msg = XtNewString(tmpStr);
               (*errorHandler) (w, msg, to);
               XtFree(msg);
            }
         }

         return(link_result == 0 ? True : False);
      }
   }

   if (CheckAccess(from, R_OK) == -1)
   {
      /*
       * A move operation does not require read permission, but a copy does.
       */
      if (mode == COPY_FILE)
      {
         if (errorHandler)
         {
            char * tmpStr;

            tmpStr = GetSharedMessage(CANT_READ_ERROR);
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, from);
            XtFree(msg);
         }
         return (False);
      }
   }


   /* Here <from> is a file (not a directory).
    * Check the <to> part of the command:
    *
    * <To> can either be an existing file,
    *                    an existing directory,
    *                    or a new file in an existing directory.
    */

   if (lstat (to, &s2) >= 0) 			   /* <to> exists */
   {
      if ((stat (to, &s3) >= 0) &&
#if defined(__hp_osf) || (__ultrix) || defined(__osf__) || defined(linux) || \
	defined(CSRG_BASED)
           (((s3.st_mode & S_IFMT) == S_IFDIR)          /* if is a directory */
           || ((s3.st_mode & S_IFMT) == S_IFSOCK)) )    /* or a net special */
#else
#if defined(SVR4) || defined(_AIX) || defined(sco)
           ((s3.st_mode & S_IFMT) == S_IFDIR) )         /* if is a directory */
#else  /* (__hpux) */
           (((s3.st_mode & S_IFMT) == S_IFDIR)          /* if is a directory */
           || ((s3.st_mode & S_IFMT) == S_IFNWK)) )     /* or a net special */
#endif
#endif
      {						   /* then get file name */
	    (void) strcpy (filename, to);

            DtLastChar(to, &cptr, &len);
            if ((len != 1) || (*cptr != '/'))
		(void) strcat (filename, "/");

	    (void) strcat (filename, DName (from));

	    to = filename;
      }

      if (lstat (to, &s2) >= 0)		           /* reverify <to> exists */
      {
         if ((stat (to, &s3) >= 0) &&
             ((s3.st_mode & S_IFMT) == S_IFDIR))   /* if is a directory */
         {
            if (errorHandler)
            {
               char * tmpStr;

               tmpStr = GetSharedMessage(CANT_OVERWRITE_ERROR);
               msg = XtNewString(tmpStr);
               (*errorHandler) (w, msg, to);
               XtFree(msg);
            }
            return (False);
         }


         /*  <from> = <to> NOOP.  */

         if (s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino)
            return (True);


         /*  no write permission  */
         if (CheckAccess(to, W_OK) == -1)
         {
            char *link_path;

            link_path = _DtFollowLink(to);
	    if(strcmp(to,link_path) == 0)
	    {
               if (errorHandler)
               {
                  char * tmpStr;

                  tmpStr = GetSharedMessage(CANT_OVERWRITE_ERROR);
                  msg = XtNewString(tmpStr);
                  (*errorHandler) (w, msg, to);
                  XtFree(msg);
               }
               return (False);
	    }
         }


         /*  unlink failed  */

         if (unlink (to) < 0)
         {
            if (errorHandler)
            {
               char * tmpStr;

               tmpStr = GetSharedMessage(CANT_OVERWRITE_ERROR);
               msg = XtNewString(tmpStr);
               (*errorHandler) (w, msg, to);
               XtFree(msg);
            }
            return (False);
         }
         fileExists = True;
      }
   }


   /* Here <from> is a file and <to> doesn't exist.
    *
    * If not a copy, link the files.  If the link succeeds, unlink
    *   <from> and return.
    * Copy <from> to <to>.
    * If the copy cmd is not specified, unlink <from>.
    */

   if (mode != COPY_FILE)
   {
      /* Try to maintain symbolic links, except when we're doing a link! */
      if (((s1.st_mode & S_IFMT) == S_IFLNK) && (mode != LINK_FILE))
      {
         char link_path[MAX_PATH + 1];
         int link_len;

         if ((link_len = readlink(from, link_path, MAX_PATH)) > 0)
         {
            link_path[link_len] = '\0';
            link_result = symlink(link_path, to);
         }
         else
         {
            /* Fail-safe; do it the hard way */
            if (mode == MOVE_FILE)
               if (RunFileCommand(MOVE_CMD, from, to, NULL) == 0)
                  return(True);
            else
               if (RunFileCommand(LINK_CMD, "-s", from, to) == 0)
                  return(True);

            link_result = (-1);
         }
      }
      else
      {
         if (mode == LINK_FILE)
            link_result = symlink(from, to);
         else
         {
            char *link_path;

            link_path = _DtFollowLink(from);

            if(strcmp(link_path, from) != 0)
                 link_result = symlink(link_path, to);
            else
                 link_result = link (from, to);
         }
      }

      /* If this was a link, then time to bail out */
      if (mode == LINK_FILE)
      {
         if ((link_result != 0) && errorHandler)
         {
            char * tmpStr;

            tmpStr = GetSharedMessage(CANT_CREATE_ERROR);
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, to);
            XtFree(msg);
         }

         return(link_result == 0 ? True : False);
      }

      /* Unlink source only if this was a move request */
      if ((mode == MOVE_FILE) && (link_result >= 0))
      {
         if (unlink (from) < 0)
         {
            if (errorHandler)
            {
               char * tmpStr;

               tmpStr = GetSharedMessage(CANT_DELETE_ERROR);
               msg = XtNewString(tmpStr);
               (*errorHandler) (w, msg, from);
               XtFree(msg);
            }
            (void) unlink (to);
            return (False);
         }

         return (True);
      }
   }

   /*  unable to read <from>  */

   if ((fold = open (from, O_RDONLY)) < 0)
   {
      if (errorHandler)
      {
         char * tmpStr;

         tmpStr = GetSharedMessage(CANT_READ_ERROR);
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, from);
         XtFree(msg);
      }
      return (False);
   }


   /*  unable create <to>  */


   /* We use the stat buffer info not lstat info */
   (void) stat (from, &s4);

   if ((fnew = creat (to, (int) s4.st_mode)) < 0)
   {
      if (errorHandler)
      {
         char * tmpStr;

         tmpStr = GetSharedMessage(CANT_CREATE_ERROR);
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, to);
         XtFree(msg);
      }
      (void) close (fold);
      return (False);
   }

   /*  do the copy  */


   while (n = read (fold, buf, BLOCK_SIZE))
   {
     int result;

     if (n < 0)
     {
       if (errorHandler)
       {
         tmpStr = (GETMESSAGE(11,31, "Error while reading %s"));
         msg = XtNewString(tmpStr);
         (*errorHandler) (w, msg, to);
         XtFree(msg);
       }
       (void) close (fold);
       (void) close (fnew);
       return (False);
     }

     errno = 0;
     result = write(fnew, buf, n);
     if (result != n)
     {
       (void) close (fold);
       (void) close (fnew);
       if(errno)
       {
         char * strerrormsg = NULL;
         char * catmsg = NULL;
         char * samsg = NULL;
         char errnoMsg[25];
         Boolean unknown = False;
         int bufLen;

         switch (errno)
         {
#ifdef EDQUOT
             case EDQUOT:
             {
                 if(mode == COPY_FILE)
                   tmpStr = (GETMESSAGE(11,51, "Unable to copy the file/folder because\nthe disk quota will be exceeded on the disk\nyou are copying it to."));
                 else
                   tmpStr = (GETMESSAGE(11,52, "Unable to move the file/folder because\nthe disk quota will be exceeded on the disk\nyou are moving it to."));
                 catmsg = XtNewString(tmpStr);
                 break;
             }
#endif
             case ENOSPC:
                 {
                 if(mode == COPY_FILE)
                   tmpStr = (GETMESSAGE(11,42, "No space available on the\ndevice you are copying to.\n\n"));
                 else
                   tmpStr = (GETMESSAGE(11,43, "No space available on the\ndevice you are moving to.\n"));
                 catmsg = XtNewString(tmpStr);
                 break;
                 }
             case EAGAIN:
                 sprintf(errnoMsg, "EAGAIN:  ");
                 strerrormsg = strerror(errno);
                 break;
             case EBADF:
                 sprintf(errnoMsg, "EBADF:  ");
                 strerrormsg = strerror(errno);
                 break;
             case EDEADLK:
                 sprintf(errnoMsg, "EDEADLK:  ");
                 strerrormsg = strerror(errno);
                 break;
             case EINTR:
                 sprintf(errnoMsg, "EINTR:  ");
                 strerrormsg = strerror(errno);
                 break;
             case EIO:
                 sprintf(errnoMsg, "EIO:   ");
                 strerrormsg = strerror(errno);
                 break;
             case ENOLCK:
                 sprintf(errnoMsg, "ENOLCK:  ");
                 strerrormsg = strerror(errno);
                 break;
             case EPIPE:
                 sprintf(errnoMsg, "EPIPE:  ");
                 strerrormsg = strerror(errno);
                 break;
             default:
                 unknown = True;
                 sprintf(errnoMsg, "%s", GETMESSAGE(11,56, "(Unknown):"));
                 strerrormsg = strerror(errno);
                 break;
         }

         /* If catmsg is NULL then one of the miscellanous error's occured.
          * Set up a generic error message which will output the internal
          * error message.
          */
         if(catmsg == NULL)
         {
           if(mode == COPY_FILE)
             tmpStr = (GETMESSAGE(11,53, "The copy of the file/folder failed\ndue to some internal error. The internal\nerror given is:"));
           else
             tmpStr = (GETMESSAGE(11,54, "The move of the file/folder failed\ndue to some internal error. The internal\nerror given is:"));
           catmsg = XtNewString(tmpStr);
           tmpStr = (GETMESSAGE(11,55, "Please see your System Adminstrator"));
           samsg = XtNewString(tmpStr);
         }

         /* Build a concatination of the possible message parts */
         bufLen = (strerrormsg ? strlen(strerrormsg) +
                   strlen(errnoMsg) + strlen(samsg) : 0) +
                   strlen(catmsg) + 10;
         msg = XtMalloc(bufLen);
         strcpy (msg, catmsg);
         if (strerrormsg)
         {
           strcat (msg, "\n\n");
           strcat (msg, errnoMsg);
           if(unknown)
             strcat (msg, "  ");
           strcat (msg, strerrormsg);
           strcat (msg, "\n\n");
           strcat (msg, samsg);
         }

         (*errorHandler) (w, msg, to);
         XtFree(msg);
         XtFree(catmsg);
       }
       unlink(to);
       return (False);
     }
   }

   (void) close (fold);
   (void) close (fnew);


   /*  unlink <from> if not copy  */

   if (mode == MOVE_FILE)
   {
      if (unlink (from) < 0)
      {
         if (errorHandler)
         {
            char *tmpStr, *ptr;


            ptr = strrchr(from, '/') + 1;
            tmpStr = (GETMESSAGE(11,38, "You do not have permission to move %s\nHowever, you can copy the object.\nTo copy an object:\n  - press and hold the <Ctrl> key, and\n  - drag the object with your mouse.\nOr\n  - use 'Copy To' in the 'Selected' menu popup of the menu bar."));
            msg = XtNewString(tmpStr);
            (*errorHandler) (w, msg, ptr);
            XtFree(msg);
            unlink(to);
         }
         return (False);
      }
   }


   /*
    * WARNING: this is different from how the shell behaves.  If you use
    *          a shell to copy over an existing file, the file keeps its
    *          original owner and group; for some historical reason,
    *          dtfile does it differently.
    *   UPDATE:  This is no longer the case as of 10/31/94, we change the
    *            file to the original owner and group now. This is to fix
    *            a bug.
    *          Also, this call originally occurred after we opened/created
    *          the file, but before we closed it.  This caused problems
    *          if the user was running as root, and was copying across
    *          an nfs link, since root access is not typically carried
    *          across an nfs mount.  The result was that we were able to
    *          create the file, copy to it, but when we tried to close it,
    *          because the file was now owned by root on the other system,
    *          we could not close the file; thus, the file ended up empty!
    */

   if (mode == COPY_FILE && fileExists)
   {
      /*  set for user  */
      (void) chmod (to, s3.st_mode);
      (void) chown (to, s3.st_uid, s3.st_gid);
   }
   else
   {
      /*  set for user  */
      (void) chown (to, getuid(), getgid());
   }

   return (True);
}
