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
 *	$XConsortium: dtb_utils.c /main/3 1995/11/06 18:04:26 rswiston $
 *
 *	@(#)dtb_utils.c	1.1 20 Sep 1994	cde_app_builder/src/abmf
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
 *  dtb_utils.c - partial set of dtb_utils functions
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ab_private/util.h>
#include "dtb_utils.h"

/*
 * Map Xt/Xm symbols to symbols we have defined
 */
#define String	STRING
#define Boolean	BOOL
#define False	FALSE
#define True	TRUE
#define min(a,b)	util_min(a,b)

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/
/*
 * Private functions used for finding paths
 */
static int  determine_exe_dir(char *argv0, char *buf, int bufSize);
static int determine_exe_dir_from_argv(char *argv0, char *buf, int bufSize);
static int determine_exe_dir_from_path (char *argv0, char *buf, int bufSize);
static Boolean path_is_executable(char *path, uid_t euid, gid_t egid);


/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/
/*
 * Directory where the binary for this process whate loaded from
 */
static char                             *dtb_exe_dir = (char *)NULL;


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
void
dtb_save_command(char *argv0)
{
    char        exe_dir[MAXPATHLEN+1];

    /*
     * Save the path to the executable
     */
    if (determine_exe_dir(argv0, exe_dir, MAXPATHLEN+1) >= 0)
    {
        dtb_exe_dir = (char *)malloc(strlen(exe_dir)+1);
        if (dtb_exe_dir != NULL)
        {
            strcpy(dtb_exe_dir, exe_dir);
        }
    }
}


/*
 * Returns the directory that the executable for this process was loaded 
 * from.
 */
String
dtb_get_exe_dir(void)
{
    return dtb_exe_dir;
}


/*
 * Determines the directory the executable for this process was loaded from.
 */
static int 
determine_exe_dir(char *argv0, char *buf, int bufSize)
{
    Boolean	foundDir= False;

    if ((buf == NULL) || (bufSize < 1))
    {
	return -1;
    }
    
    if (determine_exe_dir_from_argv(argv0, buf, bufSize) >= 0)
    {
	foundDir = True;
    }

    if (!foundDir)
    {
	if (determine_exe_dir_from_path(argv0, buf, bufSize) >= 0)
	{
	    foundDir = True;
	}
    }

    /*
     * Convert relative path to absolute, so that directory changes will
     * not affect us.
     */
    if (foundDir && (buf[0] != '/'))
    {
	char	cwd[MAXPATHLEN+1];
	char	*env_pwd = NULL;
	char	*path_prefix = NULL;
	char	abs_exe_dir[MAXPATHLEN+1];

	if (getcwd(cwd, MAXPATHLEN+1) != NULL)
	{
	    path_prefix = cwd;
	}
	else
	{
	    env_pwd = getenv("PWD");
	    if (env_pwd != NULL)
	    {
		path_prefix = env_pwd;
	    }
	}

	if (path_prefix != NULL)
	{
	    strcpy(abs_exe_dir, path_prefix);
	    if (strcmp(buf, ".") != 0)
	    {
		strcat(abs_exe_dir, "/");
		strcat(abs_exe_dir, buf);
	    }
	    strcpy(buf, abs_exe_dir);
	}
    }

    return foundDir? 0:-1;
}


/*
 *  Looks for absolute path in arv[0].
 */
static int
determine_exe_dir_from_argv(char *argv0, char *buf, int bufSize)
{
    int		i= 0;
    Boolean	foundit= False;

    for (i= strlen(argv0)-1; (i >= 0) && (argv0[i] != '/'); --i)
    {
    }

    if (i >= 0)
    {
	int	maxStringSize= min(i, bufSize);
	strncpy(buf, argv0, maxStringSize);
	buf[min(maxStringSize, bufSize-1)]= 0;
	foundit = True;
    }

    return foundit? 0:-1;
}


/*
 * Assumes: bufSize > 0
 */
static int
determine_exe_dir_from_path (char *argv0, char *buf, int bufSize)
{
    Boolean	foundDir= False;
    Boolean	moreDirs= True;
    char	*szExeName= argv0;
    int		iExeNameLen= strlen(szExeName);
    char	*szTemp= NULL;
    char	szPathVar[MAXPATHLEN+1];
    int		iPathVarLen= 0;
    char	szCurrentPath[MAXPATHLEN+1];
    int		iCurrentPathLen= 0;
    int		iCurrentPathStart= 0;
    int		i = 0;
    uid_t	euid= geteuid();
    uid_t	egid= getegid();

    szTemp= getenv("PATH");
    if (szTemp == NULL)
    {
	moreDirs= False;
    }
    else
    {
	strncpy(szPathVar, szTemp, MAXPATHLEN);
	szPathVar[MAXPATHLEN]= 0;
	iPathVarLen= strlen(szPathVar);
    }

    while ((!foundDir) && (moreDirs))
    {
	/* find the current directory name */
	for (i= iCurrentPathStart; (i < iPathVarLen) && (szPathVar[i] != ':'); 
	    )
    	{
	    ++i;
	}
	iCurrentPathLen= i - iCurrentPathStart;
	if ((iCurrentPathLen + iExeNameLen + 2) > MAXPATHLEN)
	{
	    iCurrentPathLen= MAXPATHLEN - (iExeNameLen + 2);
	}

	/* create a possible path to the executable */
	strncpy(szCurrentPath, &szPathVar[iCurrentPathStart], iCurrentPathLen);
	szCurrentPath[iCurrentPathLen]= 0;
	strcat(szCurrentPath, "/");
	strcat(szCurrentPath, szExeName);

	/* see if the executable exists (and we can execute it) */
	if (path_is_executable(szCurrentPath, euid, egid))
	{
	    foundDir= True;
	}

	/* skip past the current directory name */
	if (!foundDir)
	{
	    iCurrentPathStart+= iCurrentPathLen;
	    while (   (iCurrentPathStart < iPathVarLen) 
		   && (szPathVar[iCurrentPathStart] != ':') )
	    {
		++iCurrentPathStart;	/* find : */
	    }
	    if (iCurrentPathStart < iPathVarLen) 
	    {
		++iCurrentPathStart;	/* skip : */
	    }
	    if (iCurrentPathStart >= iPathVarLen)
	    {
		moreDirs= False;
	    }
	}
    } /* while !foundDir */

    if (foundDir)
    {
	szCurrentPath[iCurrentPathLen]= 0;
	strncpy(buf, szCurrentPath, bufSize);
	buf[bufSize-1]= 0;
    }
    return foundDir? 0:-1;
}


/*
 * returns False is path does not exist or is not executable
 */
static Boolean
path_is_executable(char *path, uid_t euid, gid_t egid)
{
    Boolean	bExecutable= False;
    struct stat	sStat;

    if (stat(path, &sStat) == 0)
    {
	Boolean	bDetermined= False;

	if (!bDetermined)
	{
	    if (!S_ISREG(sStat.st_mode))
	    {
		/* not a regular file */
		bDetermined= True;
		bExecutable= False;
	    }
	}

	if (!bDetermined)
	{
	    if (   (euid == 0) 
	        && (   ((sStat.st_mode & S_IXOTH) != 0)
		    || ((sStat.st_mode & S_IXGRP) != 0)
		    || ((sStat.st_mode & S_IXUSR) != 0) )
	       )
	    {
		bDetermined= True;
		bExecutable= True;
	    }
	}

	if (!bDetermined)
	{
	    if (   (((sStat.st_mode & S_IXOTH) != 0)    )
		|| (((sStat.st_mode & S_IXGRP) != 0) && (sStat.st_gid == egid))
		|| (((sStat.st_mode & S_IXUSR) != 0) && (sStat.st_gid == euid))
	       )
	    {
		bDetermined= True;
	        bExecutable= True;
	    }
	}
    } /* if stat */

    return bExecutable;
}

