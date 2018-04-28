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
 *	$XConsortium: util_file.c /main/4 1995/11/06 18:54:05 rswiston $
 *
 * @(#)util_file.c	1.24 19 Apr 1995	cde_app_builder/src/libAButil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * File: util_file.c
 */

#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ab_private/abio.h>
#include "utilP.h"

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/
#define	AB_EXT_LENGTH 3

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

/*
 * removes all buffering from a file.  This is useful for debugging
 * to avoid losing data, and to keep stdout and stderr messages from
 * getting jumbled (ordered is basically guaranteed if both streams
 * are unbuffered).  
 *
 * This *is* a performance hit, so don't use this except when debugging.
 * Note that stderr is always unbuffered.
 */
int
util_unbuffer_file(FILE *fp)
{
    int		iReturn= 0;
    int		iRC= 0;		/* return code */
    int		fileMode= 0;

    setbuf(fp, NULL);
    /* this is too much - actually waits for sync to disk, and runs
     * RIDICULOUSLY SLOW.
     */
    /*
    if ((iRC= fileMode= fcntl(fileno(fp), F_GETFL)) >= 0)
    {
        fileMode |= O_DSYNC;
        iRC= fcntl(fileno(fp), F_SETFL, fileMode);
    }
    */
    iReturn= iRC;

    return iReturn;
}


BOOL
util_file_exists(STRING fileName)
{
    struct stat	fileInfo;
    return (stat(fileName, &fileInfo) == 0);
}


/* Returns the file size, or  ERR_ value (<0) on error
 *
 */
long
util_file_size(STRING fileName)
{
    struct stat	fileInfo;
    if (stat(fileName, &fileInfo) != 0)
    {
	return ERR_FILE_NOT_FOUND;
    }
    return (long)fileInfo.st_size;
}


/*
 * Extension should not contain it's leading "."
 *
 * if extension is NULL or the empty string, returns TRUE if the name
 * has no extension
 */
BOOL
util_file_name_has_extension(STRING fileName, STRING extension)
{
    BOOL	hasExtension= FALSE;

    if (extension == NULL)
    {
	hasExtension = ( (fileName == NULL) || (strlen(fileName) == 0) );
    }
    else
    {
	if (fileName == NULL)
	{
	    hasExtension= FALSE;
	}
	else
	{
	    char *dotPtr= strrchr(fileName, '.');
	    if (dotPtr == NULL)
	    {
		hasExtension= FALSE;
	    }
	    else
	    {
	        hasExtension= util_streq(dotPtr+1, extension);
	    }
	}
    }

    return hasExtension;
}


BOOL
util_file_name_has_ab_extension(STRING fileName)
{
    BOOL	hasExtension= FALSE;
    char	*dotPtr= strrchr(fileName, '.');

    if (dotPtr != NULL)
    {
	hasExtension= (strncmp(dotPtr, ".bi", 3) == 0);
    }
    return hasExtension;
}


STRING
util_get_file_name_from_path(
		STRING	path,
		STRING	fileNameBuf,
		int	fileNameBufSize
)
{
    char	*slashPtr= strrchr(path, '/');
    if (slashPtr == NULL)
    {
	strncpy(fileNameBuf, path, fileNameBufSize);
    }
    else
    {
	strncpy(fileNameBuf, slashPtr+1, fileNameBufSize);
    }
    fileNameBuf[fileNameBufSize-1]= 0;
    return fileNameBuf;
}


/*
 * If path is not absolute, directory is "."
 *
 * The directory name returned does NOT have a trailing '/'.  (The
 * only exception being the directory "/" .
 */
STRING
util_get_dir_name_from_path(
		STRING	path,
		STRING	dirNameBuf,
		int	dirNameBufSize
)
{
    char	*slashPtr= strrchr(path, '/');
    assert(dirNameBufSize > 1);
    if (slashPtr == NULL)
    {
	/* no directory name */
	strcpy(dirNameBuf, ".");
    }
    else if (slashPtr == path)
    {
	/* file in / */
	strcpy(dirNameBuf, "/");
    }
    else
    {
	/* directory and file name */
	int	copyCount= 
		    util_min(dirNameBufSize, (((int)(slashPtr - path))+1));
	strncpy(dirNameBuf, path, copyCount);
	dirNameBuf[dirNameBufSize-1]= 0;
	while ((copyCount > 1) && (dirNameBuf[copyCount-1] == '/'))
	{
	    --copyCount;
	}
	dirNameBuf[copyCount]= 0;
    }

    return dirNameBuf;
}


/*
 * Truncates the open file to length bytes.
 *
 * ftruncate() is not defined in POSIX, so the header files don't define
 * it when _POSIX_SOURCE is not defined. We're going to use it, anyway,
 * and this prototype is identical in the header files for all the
 * platforms (Sun/HP/IBM).
 */
#ifdef __cplusplus
extern "C" {
#endif
    extern ftruncate(int filedes, off_t length);
#ifdef __cplusplus
} // extern "C"
#endif
int
util_fdtruncate(int filedes, off_t length)
{
    return ftruncate(filedes, length);
}


BOOL
util_directory_exists(STRING dir_name)
{
    BOOL	exists = FALSE;
    struct stat dir_info;

    if (stat(dir_name, &dir_info) != 0)
    {
	goto epilogue;
    }
    if (S_ISDIR(dir_info.st_mode))
    {
	exists = TRUE;
    }

epilogue:
    return exists;
}


BOOL
util_paths_are_same_file(STRING path1, STRING path2)
{
    BOOL	same_file = FALSE;
    struct stat file_info1;
    struct stat file_info2;

    if (stat(path1, &file_info1) != 0)
    {
	goto epilogue;
    }
    if (stat(path2, &file_info2) != 0)
    {
	goto epilogue;
    }

    same_file = (   (file_info1.st_dev == file_info2.st_dev)
		 && (file_info1.st_ino == file_info2.st_ino) );

epilogue:
    return same_file;
}


/*
 * Converts the path to a relative path from from_dir.  
 *
 * for path and from_dir, NULL, "", or "."  = current directory
 *
 * Returns "." if path and from_dir reference the same directory
 */
int
util_cvt_path_to_relative(
		STRING	path_in,
		STRING	from_dir,
		char	*buf,
		int	buf_size
)
{
#define path_is_dot(arg_path) \
            (util_strempty(arg_path) || util_streq(arg_path, "."))
	
    int		return_value = 0;
    BOOL	found_relative= FALSE;
    STRING	path = NULL;
    BOOL	more_path = TRUE;
    char	cwd[MAXPATHLEN]= "";
    STRING	from= NULL;
    char	*rightmost_slash= NULL;
    char	*last_rightmost_slash= NULL;

    if (   (path_is_dot(from_dir) && path_is_dot(path_in))
	|| (util_streq(from_dir, path_in))
       )
    {
	/* the strings are equivalent! */
	strcpy(buf, ".");
	return return_value;
    }

    /*
     * Determine "from" dir.
     */
    if (path_is_dot(from_dir))
    {
	from = ".";
    }
    else
    {
	from = from_dir;
    }

    /*
     * Determine the directory we are trying to convert.
     */
    if (path_is_dot(path_in))
    {
	if (getcwd(cwd, MAXPATHLEN) == NULL)
	{
	    return_value = ERR;
	    goto epilogue;
	}
	path = cwd;
    }
    else
    {
	path = path_in;
    }

    if (!util_directory_exists(from))
    {
	return_value = ERR;
	goto epilogue;
    }

    rightmost_slash = NULL;
    last_rightmost_slash = NULL;
    more_path = TRUE;
    while ((!found_relative) && more_path)
    {
	if (util_paths_are_same_file(from, path))
	{
	    found_relative = TRUE;
	    break;
	}

	/*
	 * Get the name of the next dir up
	 */
	rightmost_slash= strrchr(path, '/');
 	if (last_rightmost_slash != NULL)
	{
	    *last_rightmost_slash = '/';
	}
	last_rightmost_slash= rightmost_slash;

	/*
	 * Put in a NULL, so that util_paths_are_same_file is happy
	 */
	if (rightmost_slash == NULL)
	{
	    more_path = FALSE;
	}
	else
	{
	    (*rightmost_slash) = 0;
	}
    }

    if (rightmost_slash != NULL)
    {
	*rightmost_slash = '/';
    }

    if (found_relative)
    {
	if (rightmost_slash == NULL)
	{
	    /* they are the same damn file (directory)! */
	    strncpy(buf, ".", buf_size);
	    buf[buf_size-1]= 0;
	}
	else
	{
	    while ((*rightmost_slash == '/') && (*rightmost_slash != 0))
	    {
	        ++rightmost_slash;
	    }
	    strncpy(buf, rightmost_slash, buf_size);
	    buf[buf_size-1]= 0;
	}
    }
    else
    {
	strncpy(buf, path, buf_size);
	buf[buf_size-1]= 0;
    }

epilogue:
    return return_value;
#undef path_is_dot
}

/* This routine will create a directory hierarchy in the
 * current working directory if the hierarchy does not 
 * already exist.  If mkdir fails, errno will be set and
 * a negative value will be returned.
 */  
int     
util_mkdir_hier(
    STRING path 
) 
{
    STRING	slash_ptr = NULL;
    int		ret = 0;	

    /* As an example: path -> "x/y/z", so slash_ptr points
     * to the same string.
     */
    slash_ptr = path;
    while (slash_ptr != NULL)
    {
	/* 1) x/y/z	2) x/y/z	3) x/y/z
	 *     ^	      ^		       ^	
	 *     |	      |		       |
	 *  slash_ptr	   slash_ptr	slash_ptr == NULL
	 */
	slash_ptr = strchr(slash_ptr, '/');

        /* 1) x NULL y/z  2) x/y NULL z
         *       ^ 		  ^
         *       | 		  |
         *    slash_ptr 	slash_ptr
         */ 
	if (slash_ptr != NULL) 
	{
	   *slash_ptr = 0; 
	}

	/* Make the directory named: 
	 *	1) "x" 	  2) "x/y"    3) "x/y/z"
	 * in the cwd 
	 */
	ret = mkdir(path, 0777);

	/* If there was an error return -1 */
	if ( ret != 0 && errno != EEXIST )
	{
	    return ret;
	}
	
	if ( slash_ptr != NULL )
	{
            /* 1) x/y/z	     2) x/y/z	   3) slash_ptr == NULL
             *     ^   		   ^
             *     |  		   |
             *   slash_ptr  	slash_ptr
             */  
	    *slash_ptr = '/';

	    /* If there are multiple slashes, skip them. 
             * 1) x/y/z       2) x/y/z
	     *      ^		     ^    	   
             *      |  		     | 
             *   slash_ptr   	slash_ptr
             */   
	    while (*slash_ptr == '/')
	    {
		++slash_ptr;
	    }
	}
    }
	return 0;
}	

BOOL
util_path_is_absolute(
    STRING	dir
)
{
    BOOL	val = FALSE;

    if ( dir[0] == '/' )
	val = TRUE;

    return val;
}

/* This routine is passed in an absolute path name (from the file
 * chooser) and derives the module or project name for the ABObj
 * struct. The obj name is passed back in the objname parameter.
 * This routine assumes that objname already has allocated space.
 */
int
util_derive_name_from_path(
    char    *fullpath,
    char    *objname
)
{
    char        *filename, *name;
    int         len = 0;

    if ( util_file_name_has_ab_extension(fullpath) )
    {
        /*
         * Check return value of strrchr before adding 1 to it
         */
        if (filename = strrchr(fullpath, '/'))
            name = (STRING)strdup(filename + 1);        
	else
            name = (STRING)strdup(fullpath);
 
        len = strlen(name) - (AB_EXT_LENGTH + 1);
        strncpy(objname, name, len);
        objname[len] = '\0';
        free(name);
    }
    else
    {
        /*
         * Check return value of strrchr before adding 1 to it
         */
        if (filename = strrchr(fullpath, '/'))
            strcpy(objname, filename + 1);
        else
            strcpy(objname, fullpath);
    }
    return 0;
}

/* This routine is passed in a name (from the project or module 
 * name dialog).  It checks the name for the ".bil" or ".bip"
 * extension and strips it off if the name has it.  The project 
 * or module name is passed back in the new_name parameter. This 
 * routine assumes that new_name already has allocated space.
 */
int
util_check_name(
    STRING	name,
    STRING	new_name
)
{
    int		len = 0;
    
    if ( util_file_name_has_ab_extension(name) )
    {
	len = strlen(name) - (AB_EXT_LENGTH + 1);
	strncpy(new_name, name, len);
	new_name[len] = '\0'; 
    }
    else
    {
	strcpy(new_name, name);
    }
    return 0;
}


BOOL
util_file_is_regular_file(STRING filename)
{
    BOOL        IsRegFile = FALSE;
    struct stat file_info;

    if (stat(filename, &file_info) != 0)
    {
        goto epilogue;
    }
    if (S_ISREG(file_info.st_mode))
    {
        IsRegFile = TRUE;
    }

epilogue:
    return IsRegFile;
}

BOOL
util_file_is_directory(STRING filename)
{
    BOOL        IsDir = FALSE;
    struct stat file_info;

    if (stat(filename, &file_info) != 0)
    {
        goto epilogue;
    }
    if (S_ISDIR(file_info.st_mode))
    {
        IsDir = TRUE;
    }

epilogue:
    return IsDir;
}


/*
 * from fopen() man page: legal types are:
 *
 *	r, rb, w, wb, a, ab, 
 *	r+, r+b, rb+, w+, w+b, wb+, a+, a+b, ab+
 *
 * The 'b' option is ignored.
 *
 */
FILE *
util_fopen_locked(const char *filename, const char *accessType)
{
    FILE	*file = NULL;
    char	char1 = accessType[0];
    BOOL	charPlus = 
		    (   (accessType[1] != 0) 
		     && ((accessType[1] == '+') || (accessType[2] == '+')));
    BOOL	truncateFile = FALSE;
    int		lockType = -1;

    /*
     * Open the file
     * If a truncated open, open the existing file, first. That way, we
     * can get a write lock before actually doing the truncate.
     */
    switch (accessType[0])
    {
	case 'r':
	    file = fopen(filename, accessType);
	    if (charPlus)
	    {
		lockType = F_WRLCK;
	    }
	    else
	    {
		lockType = F_RDLCK;
	    }
	break;

	case 'w':
	    errno = 0;
	    file = fopen(filename, "r+");	/* use existing, first! */
	    if ((file == NULL) && (errno == ENOENT))
	    {
		file = fopen(filename, accessType);
	    }
	    lockType = F_WRLCK;
	    truncateFile = TRUE;
	break;

	case 'a':
	    file = fopen(filename, accessType);
	    lockType = F_WRLCK;
	break;

	default:
	    errno = 0;		/* file is NULL */
	break;
    }

    /*
     * Get the appropriate lock on the file.
     * Truncate the file, if necessary.
     */
    if ((file != NULL) && (lockType != -1))
    {
	if (   (util_flock(file, TRUE, lockType, 0, 0) >= 0)
	    && truncateFile)
	{
	    util_ftruncate(file, 0, accessType);
	}
    }

    return file;
}


int
util_flock(FILE *file, BOOL wait, int lockType, off_t offset, off_t length)
{
    struct flock	lock;
    int			fcntlParam = (wait? F_SETLKW:F_SETLK);

    lock.l_type = lockType;
    lock.l_whence = SEEK_SET;
    lock.l_start = offset;
    lock.l_len = length;
    lock.l_pid = (pid_t)-1;

#ifdef DEBUG
    if (debugging() && wait)
    {
	if (fcntl(fileno(file), F_SETLK, (void*)&lock) == 0)
	{
	    /* got the lock */
	    return 0;
	}
	else
	{
	    /* didn't get the lock - we're going to block */
	    util_dprintf(1, "Waiting for lock [%s]...\n",
		(fcntlParam == F_RDLCK? "READ":
		    (fcntlParam == F_WRLCK? "WRITE":
			"BAD TYPE"))
	    );
	}
    }
#endif /* DEBUG */

    if (fcntl(fileno(file), fcntlParam, (void*)&lock) != 0)
    {
        return -1;
    }

    return 0;
}


int
util_funlock(FILE *file, off_t offset, off_t length)
{
    struct flock	lock;

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = offset;
    lock.l_len = length;
    lock.l_pid = (pid_t)-1;

    if (fcntl(fileno(file), F_SETLKW, (void*)&lock) != 0)
    {
	return -1;
    }

    return 0;
}


int
util_ftruncate(FILE *file, off_t length, const char *accessType)
{
    int		fd = fileno(file);
    off_t	offset = ftell(file);

    util_fdtruncate(fd, length);

    /*
     *  Associate the stream with the file descriptor fildes.
     */
    fdopen(fd, accessType);

    /*
     * Perform a seek on the stream, just to make sure it's in sync.
     */
    if (offset > length)
    {
	fseek(file, 0, SEEK_END);
    }
    else
    {
	fseek(file, offset, SEEK_SET);
    }

    return 0;
}

