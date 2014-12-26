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
/*
 * $TOG: DbUtil.c /main/13 1998/04/09 17:47:56 mgreess $
 *
 * (c) Copyright 1988, 1989, 1990, 1991, 1992, 1993 
 *     by Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <dirent.h>

#include <ctype.h>
#include <string.h>
#ifdef NLS16
#include <limits.h>
#endif
#include <sys/stat.h>
#include <sys/param.h>		/* MAXPATHLEN, MAXHOSTNAMELEN */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#define X_INCLUDE_DIRENT_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Dt/DtP.h>
#include <Dt/Connect.h>
#include <Dt/FileUtil.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Action.h>
#include <Dt/ActionP.h>
#include <Dt/ActionDbP.h>
#include <Dt/ActionUtilP.h>
#include <Dt/DbUtil.h>
#include <Dt/Utility.h>

#include <Dt/ActionDb.h>

#ifndef S_ISLNK
/* This macro is normally defined in stat.h, but not on USL systems. */
#  define S_ISLNK(_M) ((_M & S_IFMT)==S_IFLNK)   /* test for symbolic link */
#endif

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif

#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP "/etc/opt/dt"
#endif


#define TRUE		1
#define FALSE		0

#define FILE_INCREMENT 20

/* The following string holds the default value of the Dt database 
 * search path.  This default search path has the following major
 * components:
 *
 *     $HOME/.dt/types[/%L]        A location for the user's personal
 *                                  actions and filetypes.
 *
 *     <config-location>/appconfig/types[/%L]      
 *                                  The DT location for system-wide
 *                                  customizations.
 *
 *     <top-of-dt>/types/[%L]          The DT location for default
 *                                  system-wide actions and filetypes.
 */
static char DTDATABASESEARCHPATH_DEFAULT[] = 
  "%s/.dt/types/%%L,"
  "%s/.dt/types,"
  CDE_CONFIGURATION_TOP "/appconfig/types/%%L,"
  CDE_CONFIGURATION_TOP "/appconfig/types,"
  CDE_INSTALLATION_TOP "/appconfig/types/%%L,"
  CDE_INSTALLATION_TOP "/appconfig/types";


/****   Substitution records used by XtFindFile() in _DtExpandLang()   ****/
static SubstitutionRec langSubstitutions[] =
{
    {'L', (char *)NULL},
    {'l', (char *)NULL},
    {'t', (char *)NULL},
    {'c', (char *)NULL}
};
static int nLangSubstitutions = XtNumber(langSubstitutions);


/********    Static Function Declarations    ********/

static Boolean __testPath(
			String str );
static void __setupLangSubstitutions(
			void );
static void __freeLangSubstitutions(
			void );
static char *_DtExpandLang(
        		char *string ) ;
static char _DtIsDir( 
                        char *path,
                        char *name) ;
static void _DtFreeDirVector( 
                        char **dir_vector) ;
static void __swap( 
			int i ,
        		DtDirPaths *data );
static void _DtSortFiles( 
			int low,
			int n, 
        		DtDirPaths *data) ;

/********    End Static Function Declarations    ********/

/******************
 *
 * Function Name:  __testPath
 *
 * Description:
 *
 *	This function is needed by XtFindFile().  Always returns True.
 *
 * Synopsis:
 *
 *	path = XtFindFile(..., __testPath);
 *
 ******************/

static Boolean
__testPath(String str)
{
    return True;
}

/******************
 *
 * Function Name:  __setupLangSubstitutions
 *
 * Description:
 *
 *	This function initializes langSubstitutions[] for use by
 *	XtFindFile().
 *
 * Synopsis:
 *
 *	__setupLangSubstitutions();
 *
 ******************/

static void
__setupLangSubstitutions(void)
{
    char *lang;
    char *languagePart;
    char *territoryPart;
    char *codesetPart;
    char *tlPtr, *ttPtr, *tcPtr, *endPtr;

/*
 * We should really be calling setlocale to determine the "default"
 * locale but setlocale's return value is not standardized across
 * the various vendor platforms nor is it consistent within differnt
 * revs of individual OS's. (e.g. its changing between HP-UX 9.0 and
 * HP-UX 10.0).   The "right" call would be the following line:
 *
 *  if ((lang = getenv ("LANG")) || (lang = setlocale(LC_C_TYPE,NULL)))
 *
 * Here we hard code the default to "C" instead of leaving it NULL.
 */
    languagePart = territoryPart = codesetPart = (char *)NULL;
    if ((lang = getenv ("LANG")) == (char *)NULL)
	lang = "C";

    lang = XtNewString(lang); /* free'd in __freeLangSubstitutions() */

    tlPtr = lang;
    endPtr = (char *)NULL;
    if ((ttPtr = DtStrchr(tlPtr, '_')) != (char *)NULL)
	ttPtr++;

    if ((tcPtr = DtStrchr(ttPtr ? ttPtr : tlPtr, '.')) != (char *)NULL)
    {
	endPtr = tcPtr++;
	if (*tcPtr != '\0')
	    codesetPart =
		XtNewString(tcPtr); /* free'd in __freeLangSubstitutions() */
    }

    if (ttPtr)
    {
	if (endPtr)
	{
	    int ttLen = endPtr - ttPtr;

	    if (ttLen > 0)
	    {
		/* free'd in __freeLangSubstitutions() */
		territoryPart = (char *)XtMalloc((ttLen + 1) * sizeof(char));
		strncpy(territoryPart, ttPtr, ttLen);
		territoryPart[ttLen] = '\0';
	    }
	}
	else territoryPart =
		 XtNewString(ttPtr); /* free'd in __freeLangSubstitutions() */

	endPtr = ttPtr - 1;
    }

    if (endPtr)
    {
	int tlLen = endPtr - tlPtr;

	if (tlLen > 0)
	{
	    /* free'd in __freeLangSubstitutions() */
	    languagePart = (char *)XtMalloc((tlLen + 1) * sizeof(char));
	    strncpy(languagePart, tlPtr, tlLen);
	    languagePart[tlLen] = '\0';
	}
    }
    else languagePart =
	     XtNewString(tlPtr); /* free'd in __freeLangSubstitutions() */

    langSubstitutions[0].substitution = lang;
    langSubstitutions[1].substitution = languagePart;
    langSubstitutions[2].substitution = territoryPart;
    langSubstitutions[3].substitution = codesetPart;
}

/******************
 *
 * Function Name:  __freeLangSubstitutions
 *
 * Description:
 *
 *	This function free's the strings allocated by
 *	__setupLangSubstitutions and placed into langSubstitutions[]
 *
 * Synopsis:
 *
 *	__freeLangSubstitutions();
 *
 ******************/

static void
__freeLangSubstitutions(void)
{
    int i;

    for (i = 0; i < nLangSubstitutions; i++)
	XtFree(langSubstitutions[i].substitution);
}

/******************
 *
 * Function Name:  _DtExpandLang
 *
 * Description:
 *	
 *	This function takes the string "string", and performs the following
 *	replacements:
 *		%L : contents of LANG environment variable.
 *		%l : language part of the LANG environment variable.
 *		%t : territory part of the LANG environment variable.
 *		%c : codeset part of the LANG environment variable.
 *		%% : % (e.g. %%L would be replaced by %L, and
 *			no substitution would be performed on %L)
 *
 *	If $LANG is not defined, the $LANG is assumed to be "C".
 *
 * Synopsis:
 *
 *	ret_string = _DtExpandLang (string);
 *
 *	char *ret_string;	Returns NULL if "string" is NULL, or it points
 *				to the expanded string.
 *
 *	char *string;		The comma-separated pathnames to expand.
 *
 * Note: The caller is responsible for free'ing the returned string.
 *
 ******************/

static char *
_DtExpandLang(
        char *string )
{
    char *thisPath;
    char *newPath;
    char *modPath;
    int pathLen, maxPathLen;
    int nColons;
    char *tmpPtr, *tmpPtr1;
    char *newString;
    char *tokPtr;

    if (string == NULL)
	return (NULL);

    /*
     *  We're going to use XtFindFile() to perform the replacements;
     *  the colon character ':' is used as a delimiter in XtFindFile,
     *  so we escape all colon characters in our string before
     *  passing it along.
     */
    for (nColons = 0, tmpPtr = string;
	 (tmpPtr = DtStrchr(tmpPtr, ':')) != (char *)NULL;
	 nColons++, tmpPtr++)
	/* EMPTY */
	;

    newString =
	(char *)XtCalloc(1, (strlen(string) + nColons + 1) * sizeof(char));
    for (tmpPtr = string;
	 (tmpPtr1 = DtStrchr(tmpPtr, ':')) != (char *)NULL;
	 tmpPtr = tmpPtr1 + 1)
    {
	strncat(newString, tmpPtr, tmpPtr1 - tmpPtr);
	strcat(newString, "%:");
    }
    strcat(newString, tmpPtr);

    __setupLangSubstitutions();

    /*
     *  XtFindFile() assumes that the string into which it's making
     *  substitutions is a path, and therefore it assumes that the
     *  length of the string does not exceed MAXPATHLEN.  Since
     *  our string is a series of paths, it CAN exceed MAXPATHLEN.
     *  So, we split our string into individual paths which we then
     *  pass off to XtFindFile().
     */
    pathLen = maxPathLen = 0;
    newPath = (char *)NULL;
    for (thisPath = DtStrtok_r(newString, ",", &tokPtr);
	 thisPath != (char *)NULL;
	 thisPath = DtStrtok_r((char *)NULL, ",", &tokPtr))
    {
	modPath = XtFindFile(thisPath, langSubstitutions,
			     nLangSubstitutions, __testPath);
	if (modPath)
	{
	    char *origPath = modPath;
	    int modLen;

	    /*
	     *  For some reason, XtFindFile() collapses all '/'
	     *  characters EXCEPT at the beginning of the path!
	     *  For backwards compatibility, we collapse those here.
	     */
	    if (*modPath == '/')
	    {
		while (*(modPath + 1) == '/')
		    modPath++;
	    }
	    modLen = strlen(modPath);

	    if (pathLen + modLen + 2 > maxPathLen)
	    {
		maxPathLen =
		    ((pathLen + modLen + 2 + MAXPATHLEN) / MAXPATHLEN) *
		    MAXPATHLEN;
		newPath =
		    (char *)XtRealloc(newPath, maxPathLen * sizeof(char));
	    }

	    if (pathLen > 0)
		newPath[pathLen++] = ',';
	    strcpy(&(newPath[pathLen]), modPath);
	    pathLen += modLen;

	    XtFree(origPath);
	}
    }

    __freeLangSubstitutions();
    XtFree(newString);

    return newPath;
}


/******************
 *
 * Function Name:  _DtIsDir
 *
 * Description:
 *
 *	This function tests a pathname to see if it is a directory.
 *	The path name is received in two pieces, which makes it easy
 *	for the calling function to test a bunch of files in a directory
 *	to see if any are subdirectories.
 *
 *	This function does NOT handle Softbench-style pathnames with 
 *	embedded hostnames.
 *
 * Synopsis:
 *
 *	dir = _DtIsDir (path, name);
 *
 *	char dir;		Returns 0 if the item is not a directory, 
 *				1 if it is.
 *	char *path;		The first part of the pathname.  Typically
 *				the directory containing the item of interest.
 *	char *name;		The second half of the pathname.  Typically
 *				the name of the item of interest.
 *
 ******************/

static char 
_DtIsDir(
        char *path,
        char *name )
{
   struct stat stat_buf;
   char *stat_name;
   
   stat_name = XtMalloc ((Cardinal)(strlen(path) + strlen(name) + 2));
   (void)strcpy (stat_name, path);
   (void)strcat (stat_name, "/");
   (void)strcat (stat_name, name);
   
   if(stat (stat_name, &stat_buf))
   {
	stat_buf.st_mode = 0;
   }
   XtFree (stat_name);
   
   if (stat_buf.st_mode & S_IFDIR)
      return (TRUE);
   else
      return (FALSE);
}


/******************************
 *
 * Function Name:  _DtFreeDirVector
 *
 * Description:
 *
 * 	This function frees a database-directory string vector.
 *
 * Synoposis:
 *
 *	FreeDatabaseDirs (dirs);
 *
 *	char **dirs;		The string vector to free.
 *
 ********************************/

static void 
_DtFreeDirVector(
        char **dir_vector )
{
   char **v;
   
   if (dir_vector)
   {
      for (v = dir_vector; *v != NULL; v++)
         XtFree ((char *)*v);
   
      XtFree ((char *)dir_vector);
   }
}


/******************************
 *
 * Function Name:  __swap
 *
 * Description:
 *
 * 	This function exchanges two elements in an array of DtDirPaths.
 *
 * Synoposis:
 *
 *	__swap (i, data);
 *
 *	int i;			The base index to change.
 *	DtDirPaths *data;	The data to change.
 *
 ********************************/

static void
__swap( 
	int i ,
        DtDirPaths *data )
{
   char *tmp;

   /* The "names" field of the structure is not touched because
    * this field is "NULL" for all of the entries.
    */
   tmp = data->dirs[i]; 
   data->dirs[i] = data->dirs[i+1]; data->dirs[i+1] = tmp;

   tmp = data->paths[i]; 
   data->paths[i] = data->paths[i+1]; data->paths[i+1] = tmp;
}


/******************************
 *
 * Function Name:  _DtSortFiles
 *
 * Description:
 *
 * 	Given an index, an array of "char" data and the number of elements to
 *      sort, this function sorts the data.  The sorting algorithm is based
 *      on a bubble sort because the number of elements is usually less than
 *      ten.
 *
 * Synoposis:
 *
 *	_DtSortFiles (index, n, data);
 *
 *      int low;		The base of the array to begin the sorting.
 *      int n;			The number of elements to sort.
 *	DtDirPaths *data;	The data to sort.
 *
 ********************************/

static void
_DtSortFiles( 
	int low,
	int n,
        DtDirPaths *data )
{
   int i, j;
   int high = low + n;  

   /* 
    * This sorting routine needs to be able to sort any portion of
    * an array - it does not always start at element '0'.
    */

   for (i = low; i < (high - 1); i++) 
      for (j = low; j < (high - 1); j++) 
#ifndef NO_MESSAGE_CATALOG
         if ((strcoll (data->paths[j], data->paths[j+1])) > 0)
#else
         if ((strcmp  (data->paths[j], data->paths[j+1])) > 0)
#endif
	    __swap (j, data);
}


/******************
 *
 * Function Name:  _DtFindMatchingFiles
 *
 * Description:
 *
 *	This function takes a string vector of directory names (which
 *	are in "host:/path/file" format) and a filename suffix and
 *      finds all of the files in those directories with the specified
 *	suffix.  It returns a string vector of the filenames.
 *
 *      You will typically first call _DtGetDatabaseDirPaths() to get the 
 *      'dirs' info.
 *
 *      Use _DtFreeDatabaseDirPaths() to free up the return structure.
 *
 * Synopsis:
 *
 *	filev = _DtFindMatchingFiles (dirs, suffix, sort_files);
 *
 *	DtDirPaths *filev;	A structure containing the names
 *				of all the files that were found.
 *	DtDirPaths *dirs;	A structure of directories to be
 *				searched.
 *	char *suffix;		The suffix string which is compared
 *				to the end of the filenames.  This
 *				string must contain a "." if it is
 *				part of the suffix you want to match
 *				on (e.g. ".c").
 *      Boolean sort_files;     Should the files within a directory be sorted.
 *
 *
 ******************/

DtDirPaths * 
_DtFindMatchingFiles(
        DtDirPaths *dirs,
        char *suffix,
	Boolean sort_files )
{
/* LOCAL VARIABLES */
   
   register DtDirPaths *files;	/* An array of pointers to the filenames which 
			   	have been found. */
   int max_files;	/* The total number of filenames that can be 
			   stored in the "files" array before it must
			   be reallocd. */
   int num_found;	/* The number of files which have been found. */
   register DIR *dirp;		/* Variables for walking through the directory
			   	entries. */
   char * next_file;
   struct dirent *dp = NULL;
   char *file_suffix;
   int suffixLen, nameLen;
   int nextIndex;
   register char * next_path;
   int files_in_this_directory;
   int base;

   _Xreaddirparams dirEntryBuf;
   struct dirent *result;

/* CODE */   
   if (dirs == NULL)
      return(NULL);
   
   files = (DtDirPaths *) XtMalloc((Cardinal)(sizeof(DtDirPaths)));
   files->dirs = (char **) XtMalloc(sizeof(char *) * FILE_INCREMENT);
   files->paths = (char **) XtMalloc(sizeof(char *) * FILE_INCREMENT);
   max_files = FILE_INCREMENT;
   num_found = 0;
   nextIndex = 0;
   
   /* Process each one of the directories in priority order. */
   while (dirs->paths[nextIndex] != NULL) {

      next_path = dirs->paths[nextIndex];
      dirp = opendir (next_path);
      base = num_found;
      
      files_in_this_directory = 0;
      while ((result = _XReaddir(dirp, dirEntryBuf)) != NULL) {

	 /* Check the name to see if it matches the suffix and is
	    a file. */
	 if (strlen (result->d_name) >= strlen(suffix)) 
         {
	    /* Find the end of the name and compare it to the suffix. */
            /* Get the number of chars (not bytes) in each string */
            suffixLen = DtCharCount(suffix);
            nameLen = DtCharCount(result->d_name);
            file_suffix = _DtGetNthChar(result->d_name, nameLen - suffixLen);
            if (file_suffix && (strcmp(file_suffix, suffix) == 0) && 
                !_DtIsDir((char *)next_path, (char *)result->d_name))
            {
	       
	       /* The file is a match.  See if there is room in the array
		  or whether we need to realloc.  The "-1" is to save room
		  for the terminating NULL pointer. */
	       if (num_found == max_files - 1) {
		  files->dirs = (char **) XtRealloc ((char *)files->dirs, 
                     (Cardinal)(sizeof(char *) * (max_files + FILE_INCREMENT)));
		  files->paths = (char **) XtRealloc ((char *)files->paths, 
                     (Cardinal)(sizeof(char *) * (max_files + FILE_INCREMENT)));
		  max_files += FILE_INCREMENT;
	       }
	       
	       /* Get some memory and copy the filename to the array. */
               files->dirs[num_found] = next_file = (char *) 
                   XtMalloc((Cardinal)(strlen(dirs->dirs[nextIndex]) + 
                             strlen (result->d_name) + 2));
	       (void)strcpy(next_file, dirs->dirs[nextIndex]);
	       (void)strcat(next_file, "/");
	       (void)strcat(next_file, result->d_name);

               files->paths[num_found] = next_file = (char *) 
                   XtMalloc((Cardinal)(strlen(next_path) + 
                             strlen (result->d_name) + 2));
	       (void)strcpy(next_file, next_path);
	       (void)strcat(next_file, "/");
	       (void)strcat(next_file, result->d_name);
	
	       num_found++;
	       files_in_this_directory++;

	    }
	 }
      }
      closedir (dirp);
      if (sort_files && (files_in_this_directory > 1))
         _DtSortFiles (base, files_in_this_directory, files);
      nextIndex++;
   }
   files->dirs[num_found] = NULL;
   files->paths[num_found] = NULL;
   return (files);
   
}

/******************************************************************************
 *
 * _DtDbGetDataBaseEnv( )
 * ------------------------
 * This function provides a PRIVATE API for internal manipulation of the
 * DTDATABASEDIRPATH environment variable before loading the databases.
 * 	-- used by the front panel code in dtwm.
 *
 * If the environment variable it returns a default path.
 *
 * NOTE: This function returns a freshly malloc'ed string.  It is up to 
 *	 the caller to free it.
 *       
 ******************************************************************************/

char *
_DtDbGetDataBaseEnv( void )
{
   char *nwh_dir;
   char *temp_buf;
   char *temp_s;
   int slen = 0;

   nwh_dir = getenv ("HOME");
   /* 
    * Get the DTDATABASESEARCHPATH environment variable.  If it is not set,
    * create the default value.
    */
   if (( temp_s = getenv ("DTDATABASESEARCHPATH")))
      if ( *temp_s != 0 ) return XtNewString(temp_s);

   slen = (2 * strlen(nwh_dir)) + strlen(DTDATABASESEARCHPATH_DEFAULT) + 1;
   temp_buf = XtCalloc(1, slen);
   snprintf (temp_buf, slen - 1,
             DTDATABASESEARCHPATH_DEFAULT, nwh_dir, nwh_dir);
   return temp_buf;

}

/******************************
 *
 * Function Name:  _DtGetDatabaseDirPaths
 *
 * Description:
 *
 *	This function returns a structure containing the external
 *      and internal forms for all of the database directories that must be 
 *      searched for Dt database files.  
 *      The structure is freed using _DtFreeDatabaseDirPaths().
 *
 *	The directories are all guaranteed to be fully-specified names;
 *	i.e. host:/path/dir.  
 *
 *      THIS IS TYPICALLY CALLED BEFORE USING ANY OF THE FOLLOWING:
 *
 *         DtReadDatabases()
 *         DtPrepareToolboxDirs()
 *         _DtDbRead()
 *         _DtFindMatchingFiles()
 *
 * Synoposis:
 *
 *	DtDirPaths * _DtGetDatabaseDirPaths ();
 *
 ********************************/

DtDirPaths * 
_DtGetDatabaseDirPaths( void )
{
   XrmValue resource_value;
   char *rep_type;
   char *dir_string, *remote_hosts;
   char *nwh_host;		/* Holds the host portion of the user's
				   network-home. */
   char **dir_vector;		/* The list of directories are turned into
				   a vector of strings.  This points to the
				   start of the vector. */
   char **hosts_vector;
   char **next_dir;		/* A pointer used to walk through dir_vector. */
   char **next_host;
   char *dir;                   /* Points to next dir being processed */
   int valid_dirs;		/* A count of the number of valid directories
				   found. */
   char *home;
   char *nextc;
   DtDirPaths * ret_paths;
   char * internal;
   int i;
   char *tmp_dir_string;

   /* Get our host name, and the user's home directory */
   nwh_host = _DtGetLocalHostName ();
   tmp_dir_string = _DtDbGetDataBaseEnv();
   dir_string = _DtExpandLang (tmp_dir_string);
   XtFree (tmp_dir_string);

   /* Prepare the input vector and the two output vectors. */
   dir_vector = _DtVectorizeInPlace (dir_string, ',');
   ret_paths = (DtDirPaths *)XtMalloc(sizeof(DtDirPaths));
   ret_paths->dirs = NULL;
   ret_paths->paths = NULL;
   valid_dirs = 0;

   for (next_dir = dir_vector; *next_dir != NULL; next_dir++) {

       if (DtStrchr (*next_dir, '/') == NULL){
	   /* It must be a relative path. */
           /* Ignore relative paths */
           continue;
       }

       /* If the name is not a valid directory, get rid of it. */
       if (!_DtIsOpenableDirContext (*next_dir, &internal)) {
	   continue;
       }
       else {
	   /* If not already in the list, add it to the structure. */
           for (i = 0; i < valid_dirs; i++)
           {
              if (strcmp(ret_paths->paths[i], internal) == 0)
              {
                 break;
              }
           }

           if (i == valid_dirs)
           {
	      valid_dirs++;
	      ret_paths->dirs = (char **) XtRealloc ((char *)ret_paths->dirs, 
				  (Cardinal) (sizeof (char *) * valid_dirs));

              /* Make sure the directory name is fully-qualified with a host
	         component. */
              if (DtStrchr (*next_dir, ':') != NULL)
	         dir = XtNewString(*next_dir);
            
	      /* If there is no host component, see if there is 
		 an absolute path. */
              else if (
#ifdef NLS16
		   (!is_multibyte || (mblen(*next_dir, MB_LEN_MAX) == 1)) &&
#endif
		   (**next_dir == '/')) {
	         dir = XtMalloc ((Cardinal) (strlen (nwh_host) + 2 + 
			                     strlen (*next_dir)));
		 (void) sprintf (dir, "%s:%s", nwh_host, *next_dir);
              }
	      else 
	         dir = XtNewString(*next_dir);

	      ret_paths->dirs[valid_dirs - 1] = dir;
	      ret_paths->paths = (char **) XtRealloc ((char *)ret_paths->paths, 
				  (Cardinal) (sizeof (char *) * valid_dirs));
	      ret_paths->paths[valid_dirs - 1] = internal;
           }
	   else {
	      XtFree(internal);
	   }
       }
   }

   /* The three vectors must be NULL terminated. */
   ret_paths->dirs = (char **) XtRealloc ((char *)ret_paths->dirs, 
					  (Cardinal) (sizeof (char *) * 
						      (valid_dirs + 1)));
   ret_paths->dirs[valid_dirs] = NULL;
   ret_paths->paths = (char **) XtRealloc ((char *)ret_paths->paths, 
					   (Cardinal) (sizeof (char *) * 
						       (valid_dirs + 1)));
   ret_paths->paths[valid_dirs] = NULL;
   
   XtFree ((char *) dir_string);
   XtFree ((char *) nwh_host);
   XtFree ((char *) dir_vector);
   return(ret_paths);
}


/***************************
 * void _DtFreeDatabaseDirPaths (paths)
 *
 *   DtDirPaths * paths;
 *
 * This function will free up each of the arrays within the directory
 * information structure, and will then free the structure itself.
 *
 **************************/

void 
_DtFreeDatabaseDirPaths(
        DtDirPaths *paths )
{
   _DtFreeDirVector(paths->dirs);
   _DtFreeDirVector(paths->paths);
   XtFree((char *)paths);
}

