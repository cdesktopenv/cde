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
 * File:         FileUtil.h $XConsortium: FileUtil.h /main/4 1995/10/26 15:21:50 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _FileUtil_h
#define _FileUtil_h

extern int _DtCreateDirs( char *path,
                         int mode) ;

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


extern int _DtIsOpenableDir( char *path) ;

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


extern int 
_DtIsOpenableDirContext(
        char *path,
        char **ret_path ) ;

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


extern char * _DtReaddirLstat( 
                        char *dir_name,
                        DIR *dirp,
                        struct stat *st_buf) ;

/******************
 *
 * Function Name:  _DtReaddirLstat
 *
 * Description:
 *
 *	This function reads the next entry out of a directory that has
 *	been opened with opendir and returns lstat information on it.  
 *	For more information on reading a directory, see directory(3C).
 *	For more information on lstat information, see stat(2).
 *
 *	This function returns a pointer to the full pathname of the directory
 *	entry.  This memory is owned by this function and must not be 
 *	freed.  If the caller wants to keep the filename, it must make its
 *	own copy.  When the end of the directory is encountered, NULL is
 *	returned.
 *
 * Synopsis:
 *
 *	dir_entry = _DtReaddirLstat (dir_name, dirp, st_buf);
 *
 *	char *dir_entry;	The name of the current entry within the
 *				directory.
 *
 *	char *dir_name;		The full path name of the directory.
 *
 *	DIR *dirp;		A pointer to the directory [obtained from
 *				opendir(3C)].
 *
 *	struct stat *st_buf;	The lstat(2) information.
 *
 ******************/

#endif /* _FileUtil_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
