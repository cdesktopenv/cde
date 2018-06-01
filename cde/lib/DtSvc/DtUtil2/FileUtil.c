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
/*
 * File:         FileUtil.c $XConsortium: FileUtil.c /main/6 1996/11/01 10:06:23 drk $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <sys/types.h>		/* stat(2) */
#include <sys/stat.h>		/* stat(2) */
#include <sys/param.h>		/* MAXPATHLEN */
#include <errno.h>		/* errno(2) */

#ifdef __hpux
#include <ndir.h>		/* opendir(), directory(3C) */
#else
#if defined(sun) || defined(CSRG_BASED)
#include <dirent.h>		/* opendir(), directory(3C) */
#else
#include <sys/dir.h>
#endif /* sun || CSRG_BASED */
#endif /* __hpux */

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>	/* Xt stuff */
#include <X11/StringDefs.h>	/* Cardinal */
#include <Dt/DtNlUtils.h>
#include <Dt/ActionUtilP.h>
#include <Tt/tt_c.h>

/******************
 *
 * Function Name:  _DtCreateDirs
 *
 * Description:
 *
 *	This function is passed a directory path to create and the mode
 *	for the directory.  It will create any of the parent directories 
 *	on the path that do not already exist.
 *
 *	This function may fail if any of the directories on the path already
 *	exist and are not writable.  If some component of the path already
 *	exists and is not a directory, a failure will be returned.  
 *
 *	If some component of the path exists as a directory but does not have 
 *	the specified mode, this will NOT cause a failure to be returned.
 *	This implies that if this function is called to create a writeable
 *	directory, it is possible for the function to return successfully
 *	but the directory may not actually be writable.
 *
 * Synopsis:
 *
 *	status = _DtCreateDirs (path, mode);
 *
 *	int status;		Returns 0 on success and -1 on failure.
 *	char *path;		The directory path to create.
 *	int mode;		The file mode for setting any directories
 *				that are created.
 *
 ******************/

int 
_DtCreateDirs(
        char *path,
        int mode )
{
   struct stat st_buf;
   int st_status;
   int ret_status;
   char *parent_path;
   char *temp_s;

   /* If the path already exist, make sure it is a directory. */
   if ((st_status = stat (path, &st_buf)) == 0) {
      if (S_ISDIR (st_buf.st_mode))
	 ret_status = 0;
      else
         ret_status = -1;
   }

   /* If we can't stat it, make sure it is simply because some component
      of the path doesn't exist. */
   else if (errno != ENOENT)
      ret_status = -1;

   else {
      /* Some component of the path doesn't exist.  Recursively make the
	 parent of the current directory, then make the current directory. */
      parent_path = XtNewString (path);
      if ((temp_s = DtStrrchr (parent_path, '/')) != NULL) {
         *temp_s = '\0';
	 (void) _DtCreateDirs (parent_path, mode);
      }
      XtFree (parent_path);

      /* If no error has been encountered, now make the final directory
	 in the path. */
      if ((ret_status = mkdir (path, S_IFDIR)) == 0)
            (void) chmod (path, mode);
   }

   return (ret_status);
}

/******************
 *
 * Function Name:  _DtIsOpenableDirContext
 *
 * Description:
 *
 *      This function takes a path as an argument and determines whether
 *	the path is a directory that can be opened.  This function returns
 *	"1" if the path is an openable directory and "0" if it is not.
 *      In addition, if the calling function passes in another pointer,
 *      we will return the internal representation for the path.
 *
 *	The path can be in the Softbench "context" form of "host:/path/dir".
 *
 * Synopsis:
 *
 *	status = _DtIsOpenableDirContext (cpath, ret_ptr)
 *
 *	int status;		Returns 1 for openable directories, 
 *				0 otherwise.
 *	char *cpath;		The directory name to test.
 *      char ** ret_ptr;        Where to place internal format.
 *
 ******************/

int 
_DtIsOpenableDirContext(
        char *path,
        char **ret_path )
{
   char *real_path = NULL;
   char * tmp_real_path;
   DIR *dirp;
   int ret_status;
   Tt_status status;
   char * host;
   char * filename;
   char * netfile;

   if (ret_path)
      *ret_path = NULL;

   host = _DtHostString(path);
   filename = _DtPathname(path);
   if (host)
   {
      netfile = tt_host_file_netfile(host, filename);
      if ((status = tt_ptr_error(netfile)) == TT_OK)
      {
         tmp_real_path = tt_netfile_file(netfile);
         status = tt_ptr_error(real_path);
         tt_free(netfile);
      }

      if (status != TT_OK) {
         real_path = NULL;
      } else {
	 real_path = XtNewString(tmp_real_path);
	 tt_free(tmp_real_path);
      }

      XtFree(filename);
      XtFree(host);
   }
   else
      real_path = filename;

   if (real_path && ((dirp = opendir (real_path)) != NULL)) 
   {
      closedir (dirp);
      ret_status = 1;
      if (ret_path)
         *ret_path = real_path;
   }
   else
   {
      ret_status = 0;
      if (real_path)
         XtFree(real_path);
   }

   return (ret_status);
}

/******************
 *
 * Function Name:  _DtIsOpenableDir
 *
 * Description:
 *
 *      This function takes a path as an argument and determines whether
 *	the path is a directory that can be opened.  This function returns
 *	"1" if the path is an openable directory and "0" if it is not.
 *
 *	The path can be in the Softbench "context" form of "host:/path/dir".
 *
 * Synopsis:
 *
 *	status = _DtIsOpenableDir (cpath)
 *
 *	int status;		Returns 1 for openable directories, 
 *				0 otherwise.
 *	char *cpath;		The directory name to test.
 *
 ******************/

int 
_DtIsOpenableDir(
        char *path )
{
   return (_DtIsOpenableDirContext(path, NULL));
}
