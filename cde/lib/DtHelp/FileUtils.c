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
/* $TOG: FileUtils.c /main/8 1998/07/28 15:37:38 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	FileUtils.c
 **
 **   Project:     File locating
 **
 **   Description: Locates files (volumes) accessible via the 
 **             known paths
 **
 **   NOTE: this file must remain free of Xt & Xm calls.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h> /* MAXPATHLEN */

#include <Dt/Help.h>

/*
 * private includes
 */
#include "bufioI.h"  /* for AccessI.h */
#include "Access.h"     /* CompressPathname */
#include "AccessI.h"    /* ExpandPathname */
#include "HelpP.h"      /* for DtDEFAULT_xxx */
#include "HelposI.h"    /* for search path access */
#include "StringFuncsI.h"
#include "FileUtilsI.h"
#include "Lock.h"

/******** constants *********/
#define LANG_C_STR     "C"
#define DIR_SLASH      '/'
#define EOS            '\0'

/******** types *********/

/******** public global variables *********/
const char * _DtHelpFileSuffixList[3] = {
                    DtHelpSDL_VOL_SUFFIX,
                    DtHelpCCDF_VOL_SUFFIX,
                    NULL };

/******** variables *********/
#ifdef	not_used
static const char * PeriodStr = ".";
#endif
static const char * DirSlashStr = "/";
static const char * PathSeparator = ":";
static const _DtHelpCeDirStruct DefCacheDir = { NULL, ENOTDIR, 0, NULL };

static	_DtHelpCeDirStruct *CachedDirs = NULL;

/******** functions *********/
#define MyNewString(s) (NULL != s ? strdup((char *)s) : NULL)
#define MyRealloc(p,s) (NULL != (p) ? realloc((char *)p,s) : malloc(s) )
#define MyMalloc(s)    malloc(s)


/*****************************************************************************
 * Function:   MyFree()
 *
 *  locate the '.' of the filename extension, if present
 *
 *****************************************************************************/
static void MyFree(char * ptr)
{
   if(ptr) free(ptr);
}


#ifdef	not_used
/*****************************************************************************
 * Function:   GetExtension()
 *
 *  locate the '.' of the filename extension, if present
 *
 *****************************************************************************/
static char * GetExtension(
            char *         filename)
{
    char * ext;
    if (_DtHelpCeStrrchr(filename,PeriodStr,MB_CUR_MAX,&ext) == 0 ) return ext;
    else return "";          /* do NOT return a NULL*/
}



/*****************************************************************************
 * Function:   SpecialStrcmp()
 *
 * Tests the args for NULL pointers. If both are NULL or if
 * both aren't NULL and are the same string, then returns 0.
 * If one arg is NULL and other isn't, or if strings are
 * different, returns -1 or +1.
 *
 *****************************************************************************/
static int SpecialStrcmp(
    const char *    str1,
    const char *    str2)
{
           if(NULL == str1)
           {
              if(NULL == str2) return 0;   /* str1 == str2 */
              return -1;                   /* str1 < str2 */
           }
           if(NULL == str2) return 1;      /* str1 > str2 */
           return(strcmp(str1,str2));      /* str1 ? str2 */
}
#endif




/************************************************************************
 * Function: _DtHelpFileTraceLinks (pathName)
 *
 * Purpose:  Traces pathname through all symbolic links
 *           until a real file is found or the link is
 *           found to be invalid.
 *
 * Returns:  True if file found at end of link, False if not
 *
 * Memory:  pathName must point to a malloc'd string.  The string
 *          may be freed by the function and pathName assigned a pointer
 *          to a different string specifying a different path to the same file.
 *          If a file is found, foundPath is set to the same pointer
 *          as pathName, otherwise it is set to NULL.
 ***********************************************************************/
Boolean 
_DtHelpFileTraceLinks (
             char * *  pPathName)
{
   int    result = 0;
   char   curBuf;
   char * linkPath;
   char * filePath;
   char   buf [2][MAXPATHLEN+2];      /* 2K+ bytes on stack */

   if ( NULL == *pPathName ) return False;         /* RETURN */

   /* init */
   strcpy(buf[0],*pPathName);
   linkPath = buf[0];        /* will be assigned to filePath below */
   curBuf = 1;               /* next valid buf */

   /* find out if this path is a symbolic link */
   while ( result >= 0 )
   {
      /* exchange buffer ptrs and toggle index */
      filePath = linkPath;
      linkPath = buf[curBuf % 2];
      curBuf++;

      /* get the link info */
      result = readlink (filePath, linkPath, MAXPATHLEN);

     /* check for the result of the readlink call */
     if (result == -1)
     {
        /* if newPath is not a symbolic link, errno != EINVAL */
        if (errno != EINVAL)
           return False;                    /* RETURN */

        /* filePath is not a sym link ==> a real file or directory */
        /* so return filePath in caller-owned memory */
        if ( curBuf != 1 )  /* curBuf == 1 when pPathName is a file */
        {
	   /*
	    * pPathName had memory allocated before this function was called.
	    * only increase the memory if needed.
	    */
           if ( strlen (*pPathName) < strlen(filePath) )
                *pPathName = (char *)realloc((void *)*pPathName, (sizeof(char)
                                * (strlen(filePath) +1)));

           strcpy(*pPathName, filePath);
        }
        /* printf("actual is: %s\n", filePath);  ** DBG */
        return True;                        /* RETURN */
     }  /* if an error */
     else
     {  /* no error--handle the link */

        /* if the path is absolute, just take it as such */
        linkPath [result] = EOS;       /* for safety */

        /* is path relative to current directory? */
        if ( linkPath[0] != DIR_SLASH )
        {
           char * slash = NULL;

           /* get last slash in the current file path */
           if(_DtHelpCeStrrchr(filePath,DirSlashStr,MB_CUR_MAX,&slash) == 0)
           { /* there is a path comonent in filePath; use it with linkPath */
              strcpy(++slash,linkPath);
              strcpy(linkPath,filePath);       /* leave result in linkPath */
           }
        } /* if path is relative */
        /* printf("traced to: %s\n", linkPath);  ** DBG */
     }  /* if no error */
   }  /* while result >= 0 */
   return False;                             /* RETURN */
}




/************************************************************************
 * Function: _DtHelpFileTraceToFile (pathName, accessMode, foundPath)
 *
 * Memory:  pPathName must point to a malloc'd string.  The string
 *          may be freed by the function and pathName assigned a pointer
 *          to a different string specifying a different path to the same file.
 *          If a file is found, foundPath is set to the same pointer
 *          as pathName, otherwise it is set to NULL.
 * Returns:
 *   True:  file found
 *   False: file not found or error
 ***********************************************************************/
Boolean 
_DtHelpFileTraceToFile (
             char * *  pPathName,
             int       accessMode,
             char * *  pFoundPath)
{
   struct stat status;
   char * pathName = *pPathName;  /* avoid indirection */

   *pFoundPath = NULL;
   if ( pathName == NULL || pathName[0] == EOS )
      return False;
 
   /* if it's a file, trace its links */
   if (   access (pathName, accessMode) == 0 
       && stat (pathName, &status) == 0
       && S_ISREG(status.st_mode) )    /* a file */
   {
      /* trace any links */
      if ( _DtHelpFileTraceLinks(pPathName) == False ) 
      {
          /* don't free pPathName here */
          return False;                           /* RETURN: no file */
      }
      pathName = *pPathName;
      
      /* find out if its an accessible file */
      if (   pathName != NULL 
          && pathName[0] != EOS
          && access (pathName, accessMode) == 0 
          && stat (pathName, &status) == 0
  	           && S_ISREG(status.st_mode))    /* a file */
      {
         /* point foundPath at the path */
         *pFoundPath = pathName;     
         return True;               /* RETURN:  its a file */
      }  /* if a valid path */
   } /* if a path */

#if 0
   printf("Unknown file: %s\n", pathName);
   printf("Access: %d, stat: %d, IS_REG: %d, mode: %x\n",
             access (pathName, accessMode),
             stat (pathName, &status),
             S_ISREG(status.st_mode),
             status.st_mode);
#endif

   /* its not a file */
   *pFoundPath = NULL;
   return False;
}




/******************************************************************************
 * Function: int _DtHelpFileGetSearchPaths ()
 *
 * Parameters:
 *        paths:          caller array size _DtHELP_FILE_NUM_PATHS in which
 *                        to store ptrs to the private path strings
 *        searchHomeDir:  boolean flag
 *
 * Memory:
 *        the memory pointed to by the array is NOT owned by the
 *        caller and should not be freed or modified
 *
 * Purpose:	make the search paths available
 * 
 *****************************************************************************/
void _DtHelpFileGetSearchPaths(
         char *  paths[],
         Boolean searchHomeDir)
{
static char * pathsSet[_DtHELP_FILE_NUM_PATHS];
    char tmpPath[MAXPATHLEN + 2];

    /* get user's home directory; is used in _DtHELP_FILE_USER_PATH as well */
    if (NULL == pathsSet[_DtHELP_FILE_HOME_PATH])
    {
       _DtHelpOSGetHomeDirName(tmpPath, sizeof(tmpPath));
       pathsSet[_DtHELP_FILE_HOME_PATH] = strdup(tmpPath);
    }
    if (searchHomeDir) 
       paths[_DtHELP_FILE_HOME_PATH] = pathsSet[_DtHELP_FILE_HOME_PATH];
    else 
       paths[_DtHELP_FILE_HOME_PATH] = NULL;

    /* generate the user path */
    if (NULL == pathsSet[_DtHELP_FILE_USER_PATH])
       pathsSet[_DtHELP_FILE_USER_PATH] = _DtHelpGetUserSearchPath();
    paths[_DtHELP_FILE_USER_PATH] = pathsSet[_DtHELP_FILE_USER_PATH];

    /* get the system search path */
    if (NULL == pathsSet[_DtHELP_FILE_SYS_PATH])
       pathsSet[_DtHELP_FILE_SYS_PATH] = _DtHelpGetSystemSearchPath();
    paths[_DtHELP_FILE_SYS_PATH] = pathsSet[_DtHELP_FILE_SYS_PATH];
}



/******************************************************************************
 * Function: char * _DtHelpFileLocate ()
 *
 * Parameters:
 *        type:           subdirectories to search (%T)
 *        base:           basename of the file
 *        suffix:         extension of the file to find (%S)
 *        searchCurDir:   boolean flag
 *        accessMode:     constant value from access(2)
 *
 * Returns:	malloc'd path of the located file or NULL if none located
 *
 * errno Values:
 *		EINVAL
 *
 * Purpose:	Scans all paths of given type looking for a matching file
 *              If file contains a valid absolute path, that is also
 *              acceptible.
 * 
 * FIX: merge _DtHelpFileLocate() and _DtHelpFileListScanPaths()
 *****************************************************************************/
char * _DtHelpFileLocate (
	char *        type,
        char *        filespec,
        const char *  suffixList[],
        Boolean       searchCurDir,
        int           accessMode)
{
    char * loc;
    char * ptr;
    char * pathName;
    char * curPath;
    char * base;
    int    curPathIndex;
    char * foundPath;
    const char empty = 0;
    const char * sufList[2];
#define NUM_BUGS 1
    _DtSubstitutionRec bugFixSubs [NUM_BUGS];
    char * paths[_DtHELP_FILE_NUM_PATHS];
    char tmpPath[MAXPATHLEN + 2];
    const char * * pSuffix;
    char * eos = NULL;
    char * slash = NULL;

    /* test args */
    if (NULL == filespec) return NULL;

    /* init suffix list to empty if not specified */
    if (suffixList == NULL) 
    { 
       sufList[0] = &empty; 
       sufList[1] = NULL;
       suffixList = sufList; /* override initial argument setting */
    }

    /*** first look for file as specified ***/
    /* if filespec begins with . or .. then stop after the cwd path */
    if (   (   MB_CUR_MAX == 1 
            || mblen(filespec, MB_CUR_MAX) == 1)	/* 1st char is 1 byte */
         && *filespec == '/')  			/* and its a / */
    {
       /* _DtHelpFileTraceToFile() needs a malloc'd string */
       /* 10: leaves room for add'l suffixes */
       pathName = MyMalloc(sizeof(char) * (strlen(filespec)+10)); 
       pathName = strcpy(pathName,filespec);
       _DtHelpCeCompressPathname(pathName); /* compress out relative paths */
       if ( _DtHelpFileTraceToFile(&pathName,accessMode,&foundPath) )
          return foundPath;                         /* RETURN */

       /* test all suffixes */
       eos = pathName + strlen(pathName);
       for ( pSuffix = suffixList; NULL != *pSuffix; pSuffix++ )
       {
          strcpy(eos,(char *) *pSuffix);
          /*recall: _DtHelpFileTraceToFile() requires pathName to be malloc'd*/
          if ( _DtHelpFileTraceToFile(&pathName,accessMode,&foundPath) )
             return foundPath;                           /* RETURN: found */
       } /* for all suffixes */

       MyFree(pathName);
    }

    /*** second, check if its relative to the current directory ***/
    /* if filespec begins with . or .. then stop after the cwd path */
    if (    searchCurDir
         || (      MB_CUR_MAX == 1 
                || mblen(filespec, MB_CUR_MAX) == 1)  /* 1st char is 1 byte */
             && *filespec == '.')  		      /* and its a . */
    {     /* we're looking at a cwd-relative path; ignore others */
       /*** this is monstrously inefficient--but it shouldn't get called often ***/

       /* get user's current working directory */
       /* JET - CERT VU#575804 */
       if (getcwd(tmpPath, MAXPATHLEN - 1) == NULL) return NULL; /* RETURN: error */
       
       /* make path end in a slash */
       eos = tmpPath + strlen(tmpPath);
       _DtHelpCeStrrchr(tmpPath,DirSlashStr,MB_CUR_MAX,&slash);
       if ( slash != (eos - 1) ) { *eos++ = DIR_SLASH; *eos = EOS; }

       /* make a malloc'd copy of the path with room to grow */
       slash = filespec + strlen(filespec);
       pathName = malloc(sizeof(char) * 
                    ((eos-tmpPath) + (slash-filespec) + 50) ); /* 50: arbitrary */
       if (NULL == pathName) return NULL;		/* RETURN: error */
       strcpy(pathName,tmpPath);

       /* cat on the relative path */
       strcat(pathName,filespec);

       /* compress out any relative paths */
       _DtHelpCeCompressPathname(pathName);

       /* see if we find the file now */
       /* recall: _DtHelpFileTraceToFile() requires pathName to be malloc'd */
       if ( _DtHelpFileTraceToFile(&pathName,accessMode,&foundPath) )
          return foundPath;                             /* RETURN: found */

       /* test all suffixes */
       eos = pathName + strlen(pathName);
       for ( pSuffix = suffixList; NULL != *pSuffix; pSuffix++ )
       {
         strcpy(eos,(char *) *pSuffix);
         /* recall: _DtHelpFileTraceToFile() requires pathName to be malloc'd */
         if ( _DtHelpFileTraceToFile(&pathName,accessMode,&foundPath) )
            return foundPath;                           /* RETURN: found */
       } /* for all suffixes */
       MyFree(pathName);
       return NULL;					/* RETURN: error */
    }  /* filespec is a relative path or search cur dir */

    /*** third look in search path directories ***/

    /* get the search paths */
    _DtHelpFileGetSearchPaths( paths, False );

    /*** prep variables to pass through the path search loop ***/
    /* we're not looking at a cwd-relative path and
       we know that 'filespec' isn't a valid path to a volume
       (from _DtHelpFileTraceToFile), so just pick off the 
       basename of the spec */
    base = filespec;
    if ( _DtHelpCeStrrchr(filespec,DirSlashStr,MB_CUR_MAX,&ptr) == 0 )
        base = ++ptr;      /* begin past the slash */

    /* Have to support %H explicitly */
    bugFixSubs[0].match = 'H';
    bugFixSubs[0].substitution = base;

    /* get the LANG value */
    loc = _DtHelpGetLocale();
    if (NULL == loc || EOS == loc[0]) loc = strdup(LANG_C_STR);

    /* outer loop is once for each path */
    foundPath = NULL;
    for ( curPathIndex = 0;
          curPathIndex < _DtHELP_FILE_NUM_PATHS && NULL == foundPath; 
          curPathIndex++ )
    {
       curPath = paths[curPathIndex];
       if (NULL == curPath) continue;			/* continue */
    
       /* look for the file in that path */
       if (NULL != curPath) do
       {
          /* look for next subpath separator and insert and EOS if found */
	  if (_DtHelpCeStrchr(curPath,PathSeparator,MB_CUR_MAX,&ptr)==0)
	      *ptr = EOS;

          /* compress that path */
          /* JET - CERT VU#575804 */
          strncpy(tmpPath, curPath, MAXPATHLEN);

          _DtHelpCeCompressPathname(tmpPath);

          /* test all suffixes */
          for ( pSuffix = suffixList, foundPath = NULL;
                NULL == foundPath && NULL != *pSuffix; 
                pSuffix++ )
          {
             /* generate the (directory) path using all the variables and fix 
                it up to remove the unwanted stuff involving the filename */
             pathName = _DtHelpCeExpandPathname (curPath, base, type, 
                            (char *) *pSuffix, loc, bugFixSubs, NUM_BUGS);
  
             if (   _DtHelpFileTraceToFile(&pathName,accessMode,&foundPath)==False
                 && NULL != pathName)
                free(pathName);

          } /* for all suffixes */
 
          /* restore the subpath separator and advance past it */
          if (ptr) *ptr++ = *PathSeparator;

	  curPath = ptr;
       } while (curPath && *curPath && NULL == foundPath); 
        /* do while more subpaths */

   }  /* for all paths */

   MyFree(loc);
   return foundPath;
}

/******************************************************************************
 * Function: int _DtHelpCeCheckAndCacheDir (char *dir)
 *
 * Parameters:
 *        dir	Specifies the directory to test.
 *
 * Returns:	0	if the directory exists.
 *		ENOTDIR	if the directory is invalid.
 *
 * Purpose:	To check a directory only once and remember the result.
 * 
 *****************************************************************************/
int
_DtHelpCeCheckAndCacheDir (char *dir)
{
    int		         result  = ENOTDIR;
    _DtHelpCeDirStruct	*curDir  = CachedDirs;
    _DtHelpCeDirStruct	*prevDir = NULL;
    struct stat		 buf;

    _DtHelpProcessLock();

    if (dir == NULL || *dir == '\0')
	return ENOTDIR;

    /*
     * search the cached directories
     */
    while (curDir != NULL && strcmp(curDir->dir_name, dir))
      {
	prevDir = curDir;
	curDir  = curDir->next_dir;
      }

    /*
     * was the directory found in the cache? If so, return the type.
     */
    if (curDir != NULL)
	result = curDir->type;
    else
      {
	/*
	 * new directory - malloc room for this entry.
	 */
	result = ENOMEM;
	curDir = (_DtHelpCeDirStruct *) malloc(sizeof(_DtHelpCeDirStruct));
	if (curDir != NULL)
	  {
	    /*
	     * initialize the new entry. I.E. type starts out ENOTDIR.
	     */
	    *curDir = DefCacheDir;
	    curDir->dir_name = strdup(dir);
	    if (curDir->dir_name != NULL)
	      {
		/*
		 * put this entry in the list
		 */
		if (prevDir != NULL)
		    prevDir->next_dir = curDir;
		else
		    CachedDirs = curDir;

		/*
		 * is this a directory?
		 */
		if (access(dir, R_OK) == 0 && 
				stat(dir, &buf) == 0 && S_ISDIR(buf.st_mode))
		    curDir->type = 0;

		/*
		 * return the result of the tests.
		 */
		result = curDir->type;
	      }
	    else
	        free(curDir);
	  }
      }

    /*
     * This should never happen, but just in case the directory
     * can't be cached, go ahead and check it anyway.
     */
    if (result == ENOMEM && access(dir, R_OK) == 0 && 
				stat(dir, &buf) == 0 && S_ISDIR(buf.st_mode))
	result = 0;

    _DtHelpProcessUnlock();
    return result;
}

#ifdef	not_done
/******************************************************************************
 * Function: _DtHelpCeDirStruct *_DtHelpCeGetCachedDirs (void)
 *
 * Parameters:	none.
 *
 * Returns:	A pointer to the cached directories.
 *
 * Purpose:	To allow access to the cached directories.
 * 
 *****************************************************************************/
_DtHelpCeDirStruct *
_DtHelpCeGetCachedDirs (void)
{
    return CachedDirs;
}
#endif /* not_done */
