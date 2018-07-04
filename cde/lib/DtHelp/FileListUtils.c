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
/* $TOG: FileListUtils.c /main/11 1999/10/14 13:17:49 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	FileListUtils.c
 **
 **   Project:    DtHelp library
 **
 **   Description: Locates and lists all files (volumes) accessible via the 
 **             known paths
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
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
#include <sys/stat.h>
#include <sys/param.h> /* MAXPATHLEN */
#include <limits.h>
#define X_INCLUDE_DIRENT_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Dt/Help.h>

/*
 * private includes
 */
#include "bufioI.h"  /* for AccessI.h */
#include "Access.h"
#include "AccessI.h"
#include "StringFuncsI.h"
#include "FileUtilsI.h"
#include "FileListUtilsI.h"
#include "HelposI.h"

/******** constants *********/
#define LANG_C_STR     "C"
#define EOS            '\0'

/******** types *********/

/******** public global variables *********/

/******** variables *********/
static const char * PeriodStr = ".";
static const char * DirSlashStr = "/";
static const char * PathSeparator = ":";

/******** functions *********/


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




/*****************************************************************************
 * Function:   FileInfoMatchesP()
 *
 *  Compares the info of an existing file entry with test file info
 *  and determines whether they match.
 *  Returns: True or False
 *
 *****************************************************************************/
static Boolean FileInfoMatchesP(
   _DtHelpFileEntry file1,
   _DtHelpFileEntry file2,
   int              compareFlags)
{
      /* no duplicate files allowed: compare doc stamps */
      /* note: currently am not comparing nameKey nor returning
          a placement (-1,+1) val from the compare. */
      if (    (   (compareFlags & _DtHELP_FILE_NAME) == 0
               || SpecialStrcmp(file1->fileName,file2->fileName) == 0)
           && (   (compareFlags & _DtHELP_FILE_TITLE) == 0
               || SpecialStrcmp(file1->fileTitle,file2->fileTitle) == 0)
           && (   (compareFlags & _DtHELP_FILE_IDSTR) == 0
               || SpecialStrcmp(file1->docId,file2->docId) == 0)
           && (   (compareFlags & _DtHELP_FILE_TIME) == 0
               || SpecialStrcmp(file1->timeStamp,file2->timeStamp) == 0) )
          return True;
       return False;
}



/*****************************************************************************
 * Function:   _DtHelpFileIsSameP()
 *
 *  Compares the info of two files and determines whether they match.
 *  Returns: True or False
 *
 *****************************************************************************/
Boolean _DtHelpFileIsSameP(
    char *              fileName1,
    char *              fileName2,
    _DtHelpFileInfoProc infoProc,
    int                 compareFlags,
    XtPointer           pDisplayArea)
{
    _DtHelpFileRec file1, file2;
    char *    fileName;
    Boolean   ret;

   /* get filenames without path */
   if (_DtHelpCeStrrchr(fileName1, DirSlashStr, MB_CUR_MAX, &fileName) == 0) 
       fileName1 = fileName + 1;
   if (_DtHelpCeStrrchr(fileName2, DirSlashStr, MB_CUR_MAX, &fileName) == 0) 
       fileName2 = fileName + 1;

    /* init the structs */
    memset(&file1,0,sizeof(file1));
    memset(&file2,0,sizeof(file2));
    file1.fileName = fileName1;
    file2.fileName = fileName2;

    /* get info on the files */
    (*infoProc)(pDisplayArea,fileName1,NULL,NULL,
                          &file1.docId,&file1.timeStamp,&file1.nameKey,
                          NULL,NULL);
    (*infoProc)(pDisplayArea,fileName2,NULL,NULL,
                          &file2.docId,&file2.timeStamp,&file2.nameKey,
                          NULL,NULL);

    ret = FileInfoMatchesP(&file1,&file2,compareFlags);

    XtFree(file1.docId);
    XtFree(file1.timeStamp);
    XtFree(file2.docId);
    XtFree(file2.timeStamp);
    return ret;
}



/*****************************************************************************
 * Function:   ScanDirForFiles
 *
 *  scan a directory looking for files with a matching suffix
 *  returns number of files found in this directory
 *
 *****************************************************************************/
static int ScanDirForFiles(
            char *         dirpath,
            const char *   suffixList[],
            _DtHelpFileList * in_out_list,
            XmFontList *   io_fontList,
            Boolean *      ret_mod,
            int            foundFilesCnt,
            _DtHelpFileInfoProc infoProc,
            XtPointer      pDisplayArea,
            int            compareFlags,
            int            sortFlags,
            _DtHelpFileScanProcCB scanProc,
            XtPointer      clientData)
{
    int     count = 0;
    char    fullName [MAXPATHLEN + 2];
    char   *ptr;
    DIR    *pDir;
    struct dirent *result;
    _Xreaddirparams dirEntryBuf;

    /* open the directory */
    pDir = opendir (dirpath);
    if (pDir == NULL) return 0;                        /* RETURN */

    /* build the pathname */
    snprintf(fullName, sizeof(fullName), "%s%s", dirpath, DirSlashStr);
    ptr = fullName + strlen (fullName);

    /*
     * Scan through the files looking for matching suffixes
     */
    while ((result = _XReaddir(pDir, dirEntryBuf)) != NULL) {
        const char * matchedSuffix;
        char * ext;

	/* Skip over any "." and ".." entries. */
	if ((strcmp(result->d_name, ".") == 0) ||
	    (strcmp(result->d_name, "..") == 0))
	  continue;

        /* get working values */
	ext = GetExtension (result->d_name);
        matchedSuffix = "";      /* default (==>no suffix to match) */

        /* try to match with a suffix, if specified */
        if (NULL != suffixList)
        {
          const char * * pSuffix;
          matchedSuffix = NULL;
          for (pSuffix = suffixList; 
               NULL != *pSuffix && NULL == matchedSuffix; 
               pSuffix++ )
          {
            if (strcmp(ext,*pSuffix) == 0)
              matchedSuffix = *pSuffix;
          }
        }

        /* and a match is found */
	if ( NULL != matchedSuffix ) 
	{
           Boolean mod;

           /* then generate the full path and add to the list */
	   strcpy (ptr, result->d_name);
	   if (_DtHelpFileListAddFile(in_out_list,io_fontList,&mod,
                  fullName,ptr,infoProc,compareFlags,sortFlags,pDisplayArea))
           {
              count++;
              if (scanProc) (*scanProc)(count + foundFilesCnt,clientData);
           }
           *ret_mod |= mod;        /* accumulate mods */
	}
    } /* while more entries */

    /* close the directory stream */
    closedir(pDir);

    return count;
}



/******************************************************************************
 * Function:  _DtHelpFileListGetMatch ()
 *
 * Parameters:
 *              fileListHead     head of the list
 *              fullFilePath     fullFilePath to file to find
 *              infoProc         will deliver info on the file
 *              pDisplayArea     used to interpret vol info
 *
 * Returns:	pointer to matching _DtHelpFileList entry
 *              or NULL if none is found
 *
 * Purpose:	Looks for a file list entry for the file
 *              identified by fullFilePath.
 *              The infoProc routine gets info about the file
 *              to be matched, and uses that when comparing
 *              with the file list.  This allows more
 *              than one file with the same name to be in
 *              the list and be discriminated on the basis
 *              of the info.
 * 
 *****************************************************************************/
_DtHelpFileList _DtHelpFileListGetMatch ( 
        _DtHelpFileList     fileListHead,
        char *              fullFilePath,
        _DtHelpFileInfoProc infoProc,
        int                 compareFlags,
        XtPointer           pDisplayArea)
{
   _DtHelpFileList next;
   _DtHelpFileRec  file;

   /* zero the record */
   memset (&file,0,sizeof(file));

   if (NULL == fileListHead || NULL == fullFilePath) return NULL; /* RETURN */

   /* get filename without path */
   file.fileName = fullFilePath;
   if (_DtHelpCeStrrchr(fullFilePath,DirSlashStr,MB_CUR_MAX,&file.fileName) == 0) 
       file.fileName++;

   /* if an info proc, use it, otherwise make due */
   if (infoProc) 
      (*infoProc)(pDisplayArea,fullFilePath,
                     NULL,NULL,&file.docId,&file.timeStamp,&file.nameKey,
                     NULL,NULL);
   else
      file.nameKey = _DtHelpCeStrHashToKey(file.fileName);

   next =  NULL;
   while ( (next = _DtHelpFileListGetNext(fileListHead, next)) != NULL )
   {
      if ( FileInfoMatchesP(next,&file,compareFlags) )
         break;                   /* next is the matching entry */
   }

   /* fileName is part of fullFilePath */
   XtFree(file.docId);
   XtFree(file.timeStamp);
   return next;
}





/******************************************************************************
 * Function: int _DtHelpFileListGetNext ()
 *
 * Parameters:
 *              fileListHead     head of the list
 *              curFile          current entry in the list
 *
 * Returns:	pointer to next _DtHelpFileList entry
 *              or NULL if end of list has been reached
 *
 * Purpose:	Gets the next file in the list
 * 
 *****************************************************************************/
_DtHelpFileList _DtHelpFileListGetNext ( 
        _DtHelpFileList fileListHead,
        _DtHelpFileList curFile)
{
   /* could do a sanity test on curFile here */
   if ( curFile ) return curFile->next;
   else return fileListHead;
}



/******************************************************************************
 * Function: int _DtHelpFileFreeEntry ()
 *
 * Parameters:
 *
 * Purpose:	free the contents of an entry
 * 
 *****************************************************************************/
void _DtHelpFileFreeEntry (
        _DtHelpFileEntry entry)
{
    if(NULL == entry) return;
    XmStringFree(entry->fileTitleXmStr);
    XtFree(entry->docId);
    XtFree(entry->timeStamp);
    XtFree(entry->fileTitle);
    XtFree((char *) entry->fileName);
    XtFree((char *) entry->fullFilePath);
    XtFree((char *) entry->clientData);
    XtFree((char *) entry);
}



/******************************************************************************
 * Function: int _DtHelpFileListAddFile ()
 *
 * Parameters:
 *
 * Returns:	True if added a new entry
 *              False if entry not added because already present
 *              or an error occurred
 *
 *
 * errno Values:
 *              0        no error
 *		ENOMEM   memory allocation error
 *
 * Purpose:	adds a file to the file list if its not already present
 * 
 *****************************************************************************/
Boolean _DtHelpFileListAddFile (
        _DtHelpFileList * in_out_list,
        XmFontList *    io_fontList,
        Boolean *       ret_mod,
        char *          fullFilePath,
        char *          fileName,
        _DtHelpFileInfoProc infoProc,
        int             compareFlags,
        int             sortFlags,
        XtPointer       pDisplayArea)
{
   _DtHelpFileList next;
   _DtHelpFileList prev;
   _DtHelpFileList newList;
   _DtHelpFileRec  addFile;
   char *       actualPath;
   char         empty = 0;
   int		nameKey = 0;
   typedef int (*_CEStrcollProc)(const char *,const char *);
   extern _CEStrcollProc _DtHelpCeGetStrcollProc();
   _CEStrcollProc strcollfn = _DtHelpCeGetStrcollProc();

   /* init the variable */
   memset(&addFile, 0, sizeof(addFile));

   /* if no full file path, go look for it */
   if (NULL == fullFilePath || fullFilePath[0] == EOS) 
       return False;       /* RETURN : incomplete file spec */

   /* trace any links; _DtHelpFileTraceLinks may chg actualPath ptr */
   actualPath = XtNewString(fullFilePath);
   if ( _DtHelpFileTraceLinks(&actualPath) == False )
   {
      XtFree(actualPath);
      return False;       /* RETURN: invalid file */
   }
   fullFilePath = actualPath;

   /* if no filespec, get it */
   if (NULL == fileName) 
   {
      fileName = fullFilePath;
      if(_DtHelpCeStrrchr(fullFilePath, DirSlashStr, MB_CUR_MAX,&fileName)==0)
          fileName++;
   }
   addFile.fileName = fileName;

   /* if an info proc, use it, otherwise make due */
   if (infoProc) 
   {
      (*infoProc)(pDisplayArea,fullFilePath,
                       &addFile.fileTitle,&addFile.fileTitleXmStr,
                       &addFile.docId,&addFile.timeStamp,&addFile.nameKey,
                       io_fontList,ret_mod);
   }
   else
   { 
      nameKey = _DtHelpCeStrHashToKey(fileName);
   }

   /* look for prior existence and position */
   next = prev = NULL;
   while ( (next = _DtHelpFileListGetNext(*in_out_list, next)) != NULL )
   {
      if ( FileInfoMatchesP(next,&addFile,compareFlags) )
      {
         XtFree(addFile.docId);
         XtFree(addFile.timeStamp);
         XtFree(addFile.fileTitle);
         XmStringFree(addFile.fileTitleXmStr);
         XtFree(fullFilePath);
         return False;                        /* RETURN : repeat entry */
      }

      /* insert lexically according to title */
      /* use case insensitive NLS collating for ordering */
      if (   (sortFlags & _DtHELP_FILE_TITLE) 
          && (*strcollfn) (addFile.fileTitle, next->fileTitle) <= 0 )
         break;          /* BREAK: insert after prev, before next */

      /* FIX: add support for other sorting here */
      /* if (   (sortFlags & _DtHELP_FILE_DIR)  */
      /* if (   (sortFlags & _DtHELP_FILE_TIME)  */
      /* if (   (sortFlags & _DtHELP_FILE_IDSTR)  */
      /* if (   (sortFlags & _DtHELP_FILE_LOCALE)  */

      prev = next;                           /* save ref to prior entry */
   }
   /* if dup not found, fall thru, with prev valid */

   /* no matching path, so create, initialize, and append to list */
   newList = (_DtHelpFileList) XtCalloc(1, sizeof(_DtHelpFileRec));
   if (NULL == newList) 
   {
      XtFree(addFile.docId);
      XtFree(addFile.timeStamp);
      XtFree(addFile.fileTitle);
      XmStringFree(addFile.fileTitleXmStr);
      XtFree(fullFilePath);
      return False;                            /* RETURN : error */
   }

   /* init the contents */
   newList->fullFilePath = fullFilePath; /* copy made earlier */
   newList->fileName = XtNewString(addFile.fileName);
   newList->nameKey = addFile.nameKey;
   newList->docId = (addFile.docId == &empty ? NULL : addFile.docId);
   newList->timeStamp = (addFile.timeStamp == &empty ? NULL : addFile.timeStamp);
   newList->fileTitle = addFile.fileTitle;
   newList->fileTitleXmStr = addFile.fileTitleXmStr;
   if (next) newList->next = next;
   if (prev) prev->next = newList;
   else *in_out_list = newList;

   return True;                                 /* created new entry */
}



/******************************************************************************
 * Function: int _DtHelpFileListScanPaths ()
 *
 * Parameters:
 *        type:           subdirectories to search (%T)
 *        suffix:         extension of the files to find (%S)
 *        searchHomeDir:  boolean flag
 *        in_out_list:    manages list of files generated
 *
 * Returns:	count of files added to the list or -1
 *
 * errno Values:
 *		EINVAL
 *
 * Purpose:	Scans all paths of given type looking for files with the suffixes
 * 
 *****************************************************************************/
int _DtHelpFileListScanPaths (
        _DtHelpFileList * in_out_list,
        XmFontList *   io_fontList,
        Boolean *      ret_mod,
	char *         type,
        const char *   suffixList[],
        Boolean        searchHomeDir,
        _DtHelpFileInfoProc infoProc,
        XtPointer      pDisplayArea,
        int            sysPathCompareFlags,
        int            otherPathCompareFlags,
        int            sortFlags,
        _DtHelpFileScanProcCB scanProc,
        XtPointer      clientData)
{
    char *loc;
    char *ptr;
    char *pathName;
    struct stat status;
    char *curPath;
    int   curPathIndex;
    int   foundFilesCnt;
    int   compareFlags;
    char * paths[_DtHELP_FILE_NUM_PATHS];
    char ** scannedPaths = NULL;
    int     scannedPathsCnt = 0;

    /* get the search paths */
    _DtHelpFileGetSearchPaths( paths, searchHomeDir );

    /* get the locale */
    loc = _DtHelpGetLocale();
    if (NULL == loc || EOS == loc[0]) loc = strdup(LANG_C_STR);

    /* zero found */
    if (scanProc) (*scanProc)(0,clientData);

    /* outer loop is once for each path */
    foundFilesCnt = 0;
    for ( curPathIndex = 0;
          curPathIndex < _DtHELP_FILE_NUM_PATHS; 
          curPathIndex++ )
    {
       curPath = paths[curPathIndex];
       if (NULL == curPath) continue;

       /* set the comparison flags */
       /* This enables implementation of different policies for
          listing files that are found, e.g. only list the first file
          of a given name found in the SYS search path, while listing
          same named files that have different times & ids in the
          USER PATH. */
       compareFlags = sysPathCompareFlags;
       if (_DtHELP_FILE_SYS_PATH != curPathIndex) 
          compareFlags = otherPathCompareFlags;
    
       /* find the files in that path */
       do
       {
          char * slashptr;
          /* look for next subpath separator and insert and EOS if found */
	  if (_DtHelpCeStrchr(curPath,PathSeparator,MB_CUR_MAX,&ptr)==0)
	      *ptr = EOS;

          /* generate the (directory) path using all the variables 
             and fix it up to remove the unwanted stuff involving the filename*/
	  pathName = _DtHelpCeExpandPathname(curPath, 
                                     NULL, type, NULL, loc, NULL, 0);
          if(   pathName 
             && _DtHelpCeStrrchr(pathName,DirSlashStr,MB_CUR_MAX,&slashptr) == 0)
             slashptr[0] = EOS; 

          /* restore the subpath separator and advance past it */
	  if (ptr) *ptr++ = *PathSeparator;

          /* if its a directory; scan for matching files in it */
	  if (     pathName != NULL 
                && *pathName != EOS 
                && access (pathName, R_OK | X_OK) == 0 
                && stat (pathName, &status) == 0
		&& S_ISDIR(status.st_mode))        /* a dir */
          {
              int i;
              /* check that we haven't already scanned it */
              for ( i=0; i<scannedPathsCnt; i++ )
                 if (strcmp(scannedPaths[i],pathName) == 0) break;

              /* scan it if haven't already */
              if ( i == scannedPathsCnt )
              {
                 foundFilesCnt += ScanDirForFiles(
                                    pathName,suffixList,
                                    in_out_list,io_fontList,ret_mod,
                                    foundFilesCnt,infoProc,pDisplayArea,
                                    compareFlags,sortFlags,scanProc,clientData);

                 /* add to list of scanned */
                 scannedPaths = (char **) XtRealloc((char *)scannedPaths,
                                        (scannedPathsCnt+1) * sizeof(char *) );
                 scannedPaths[scannedPathsCnt++] = strdup(pathName);
              }  /* if haven' scanned already */
          }  /* if a directory */
#if 0
          else
          {
             printf("Unknown dir: %s\n", pathName);
             printf("Access: %d, stat: %d, IS_DIR: %d, mode: %x\n",
                access (pathName, R_OK | X_OK),
                stat (pathName, &status),
		S_ISDIR(status.st_mode),
		status.st_mode);
          }
#endif

	  if (pathName) free (pathName);
	  curPath = ptr;
       } while (curPath && *curPath); /* while more subpaths */
   }  /* for all paths */

   /* free all scanned paths */
   while ( scannedPathsCnt-- > 0 ) XtFree(scannedPaths[scannedPathsCnt]);
   XtFree((char *)scannedPaths);

   XtFree(loc);
   return foundFilesCnt;
}

