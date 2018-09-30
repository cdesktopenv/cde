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
/* $TOG: helpgen.c /main/8 1998/04/20 12:52:36 mgreess $ */
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <nl_types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>

#include <Dt/Help.h>
#include <Dt/EnvControlP.h>

#include "HelpP.h"        /* in DtHelp library */
#include "GenUtilsP.h"    /* in DtHelp library */
#include "ObsoleteP.h"    /* in DtHelp library */
#include "bufioI.h"       /* for AccessI.h     */
#include "Access.h"       /* in DtHelp library */
#include "AccessP.h"      /* in DtHelp library */
#include "AccessI.h"      /* in DtHelp library */
#include "AccessCCDFI.h"  /* in DtHelp library */
#include "StringFuncsI.h" /* in DtHelp library */

#ifdef _AIX
#include <LocaleXlate.h>
#endif

#ifndef NL_CAT_LOCALE
static const int NL_CAT_LOCALE = 0;
#endif

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif

#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP "/etc/dt"
#endif

#ifndef	DtSYS_FILE_SEARCH_ENV
#define DtSYS_FILE_SEARCH_ENV          "DTHELPSEARCHPATH"
#endif

#ifndef	DtUSER_FILE_SEARCH_ENV
#define DtUSER_FILE_SEARCH_ENV         "DTHELPUSERSEARCHPATH"
#endif

/*****************************************************************************
 *	defines
 *****************************************************************************/

#define	VOLUME_EXT	".hv"
#define	FAMILY_EXT	".hf"

/*****************************************************************************
 *	static strings.
 *****************************************************************************/
static	const char *ShellCmd    = "sh";
static	const char *UsageStr    =
	"%s -dir <directory> [-generate] [-file <name>] [-lang <language>]\n";
static	const char *TopLocId    = "_hometopic";
static	const char *SlashString = "/";
static	const char *C_String    = "C";
static	const char *DefCharSet  = "C.ISO-8859-1";
static	const char *Family_ext  = FAMILY_EXT;
static	const char *Ext_Hv      = ".hv";
static	const char *Ext_Sdl     = ".sdl";

static	const char *SuperMsg =
	     "%s: Access denied for directory %s\nTry running as super user?\n";
static	const char *GeneralAccess =
			"%s: Unable to access %s - error status number  %d\n";
static	const char *NotDirectory = "%s: Element of %s is not a directory\n";
static	const char *WriteInvalid = "%s: Write to %s invalid\n";
static	const char *defaultTopic = "<TOPIC charset %s>\n";
static	const char *defaultTitle12 =
	"<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>\n%s\n</ANGLE></SIZE></WEIGHT></TYPE>\n";
static	const char *defaultTitle14 =
	"<TITLE><TYPE serif><WEIGHT bold><SIZE 14>\n%s\n</SIZE></WEIGHT></TYPE></TITLE>\n";

static	const char *defaultTextBody =
"<ABBREV>Welcome to the Help Manager</ABBREV> \n\
<PARAGRAPH>Each of the titles listed below represents a <ANGLE italic> \n\
product family</> that has installed and registered its online help.  Each  \n\
title (and icon) is a hyperlink that lists the help within the family.</> \n\
<PARAGRAPH after 0 first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\">To \n\
display a list of the help available for a product family, choose its \n\
title (underlined text) or icon.</PARAGRAPH>  \n\
<PARAGRAPH after 0 first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\">\n\
Within a product  \n\
family, find the help you want to view, then choose its title.</PARAGRAPH> \n\
<PARAGRAPH first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\"> \n\
If you need help while using help windows, press F1.</PARAGRAPH>";

static	const char *defaultAlternate =
"<ABBREV>Welcome to the Help Manager</ABBREV> \n\
<LINK 0 \"Help4Help How-To-Register-Help\"> \n\
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic> \n\
Note:\\ \\ \\ No Help Registered</SIZE></WEIGHT></TYPE></></LINK> \n\
<PARAGRAPH leftindent 3 firstindent 3> \n\
<WEIGHT bold>No product families have registered their online help \n\
files for browsing.</>  Help may be available for some applications by \n\
choosing Help commands directly within the applications.</>";

/*****************************************************************************
 *	global variables.
 *****************************************************************************/
char	 *myName;
char	 *Lang        = NULL;
char	 *ParentName  = "_HOMETOPIC";

char	**TopicList   = NULL;

/* The family search list */
char	**FUserList    = NULL;
char	**FSysList     = NULL;

/* The volume search list */
char	**VUserList    = NULL;
char	**VSysList     = NULL;

char	**FamilyList     = NULL;	/* the names of the unique families */
char	**FullFamilyName = NULL;	/* the fully qualified family names */
char	**VolumeList     = NULL;	/* the names (only) of volume       */
char	**FullVolName    = NULL;	/* the fully qualified volume names */

char      TopicName [MAXPATHLEN + 2];

int	  FamilyNum  = 0;

/* Global Message Catalog file names */
/*****************************************************************************
 *	Private Function Declarations
 *****************************************************************************/
extern	char	*FindFile (char *filename);

/*****************************************************************************
 *	options and resources
 *****************************************************************************/
typedef	struct
{
    char	*dir;
    char	*file;
    char	*lang;
} ApplicationArgs, *ApplicationArgsPtr;

static	ApplicationArgs	App_args =
  {
	NULL,
	"browser",
	NULL,
  };

/*****************************************************************************
 * void MyExit(exit_val, pid)
 *****************************************************************************/
void
MyExit (
    int    exit_val,
    pid_t  pid)
{
  if (pid != ((pid_t) -1))
      (void) kill(pid, SIGKILL);

  exit (exit_val);
}

/*****************************************************************************
 * char *GetMessage(set, n, s)
 *****************************************************************************/
char *
GetMessage (
    int    set,
    int    n,
    char  *s)
{
   char *msg;
   char *lang;
   char  *catFileName=NULL;
   static nl_catd nlmsg_fd;
   static int first = 1;

   if ( first ) 
     {

       /* Setup our default message catalog names if none have been set! */
       /* Setup the short and long versions */
        catFileName = "dthelpgen";
	first = 0;

	if (strcmp (Lang, "C") == 0)
	  /*
	   * If LANG is not set or if LANG=C, then there
	   * is no need to open the message catalog - just
	   * return the built-in string "s".
	   */
	  nlmsg_fd = (nl_catd) -1;
	else
	  nlmsg_fd = catopen(catFileName, NL_CAT_LOCALE);
      }
    msg=catgets(nlmsg_fd,set,n,s);
    return (msg);

}

/*****************************************************************************
 * Boolean *GetPath(filename)
 *****************************************************************************/
Boolean
GetPath (char *filename, short strip, char ***list )
{
    char  *ptr;
    char **next = *list;

    if (strip)
      {
        ptr = strrchr (filename, '/');
        if (ptr)
            *ptr = '\0';
        else
            filename = "./";
      }

    while (next != NULL && *next != NULL && strcmp (*next, filename))
	next++;

    if (next == NULL || *next == NULL)
        *list = (char **) _DtHelpCeAddPtrToArray ((void **) (*list),
							strdup(filename));

    return False;
}

/*****************************************************************************
 * char *GetExtension(filename)
 *     char *filename  - name of file to get the extension from.
 * return  a pointer to the extension of the file name
 *****************************************************************************/
char *
GetExtension(char *filename )
{
    char *ext;

/*
 * WARNING...
 * need multi-byte functionality here
 */
    ext  = strrchr(filename, '.');
    if (ext)
	return(ext); /* safe because ext not in middle of character */

  return(""); /* never returns NULL */
}

/*****************************************************************************
 * Function:   CreateVolumeLink
 *
 *		outTopic	the output stream.
 *		volume_name	Searches for a volume by this name.
 *
 * Reads a volume database and creates a label paragraph entry.
 *
 *****************************************************************************/
int
CreateVolumeLink (
    CanvasHandle	 canvas,
    FILE		*outTopic,
    char		*volume_name )
{
    int          result = -1;
    char	*title      = NULL;
    char	*charSet    = (char *) DefCharSet;
    char	*abstract   = NULL;
    char        *pathName   = NULL;
    VolumeHandle volume = NULL;

    pathName = FindFile (volume_name);
    if (pathName != NULL && _DtHelpCeOpenVolume(canvas,pathName,&volume) == 0)
      {
	if (_DtHelpCeGetVolumeTitle (canvas, volume, &title) == 0)
	    result = 0;
	else if (_DtHelpCeGetTopicTitle(canvas,volume,(char*)TopLocId,&title)
					== True)
	    result = 0;

	if (result == 0)
	  {
	    if (_DtHelpCeGetAsciiVolumeAbstract(canvas,volume,&abstract) == -1)
		abstract = NULL;

	    charSet = _DtHelpCeGetVolumeLocale(volume);
	    if (charSet == NULL)
		charSet = (char *) DefCharSet;
	  }
	_DtHelpCeCloseVolume (canvas, volume);
      }

    if (result == 0)
      {
	fprintf (outTopic, (GetMessage(3, 4, "<CHARACTERSET %s>\n")), charSet);
	fprintf (outTopic,"<LINK 0 \"%s %s\">\n", volume_name, (char*)TopLocId);
	fprintf (outTopic, (GetMessage(3, 5, (char*)defaultTitle12)), title);
	fprintf (outTopic, "</LINK>\n");

	/*
	 * put the abstract information about this
	 * family in the header file
	 */
	fprintf (outTopic, "%s", GetMessage (3, 3, "<P before 1 first 1 left 1>\n"));

	if (abstract != NULL)
	  {
	    fprintf (outTopic, (GetMessage (3, 4, "<CHARACTERSET %s>\n")),
								charSet);
	    fprintf (outTopic, "%s\n", abstract);
	    fprintf (outTopic, "</CHARACTERSET>\n");
	    free (abstract);
	  }
	fprintf (outTopic, "</P>\n</CHARACTERSET>\n");
      }

    if (charSet != DefCharSet)
	free(charSet);

    if (title)
	free ((void *) title);

    return result;
}

/*****************************************************************************
 * Function:   CreateFamily
 *
 *****************************************************************************/
int
CreateFamily (
    CanvasHandle canvas,
    char    *family_name,
    FILE    *out_volume,
    FILE    *out_header,
    FILE    *out_topic )
{
    int		 result = -1;
    int		 count = 0;
    long	 filepos;
    char	*charSet = NULL;
    char	*title = NULL;
    char	*abstract = NULL;
    char	*list = NULL;
    char	*token = NULL;
    char	*ptr;
    char	*bitmap = NULL;
    char	 familyName [20];	/* FAMILY%d */
    char	 bitmapName [MAXPATHLEN + 2];
    char	 bitmapNameTemp [sizeof(bitmapName)];

    XrmDatabase	db;
    char	*resType;
    XrmValue	resValue;

    db = XrmGetFileDatabase (family_name);
    if (db)
      {
	/*
	 * get the title
	 */
	if (XrmGetResource (db, "Family.Title", "family.title",
						&resType, &resValue))
	  {
	    title = (char *) resValue.addr;

	    /*
	     * get the abstract
	     */
	    if (XrmGetResource (db, "Family.Abstract", "family.abstract",
						    &resType, &resValue))
	      {
		abstract = (char *) resValue.addr;

		/*
		 * get the volumes list
		 */
		if (XrmGetResource (db, "Family.Volumes", "family.volumes",
							&resType, &resValue))
		  {
		    list = (char *) resValue.addr;

		    /*
		     * get the character set
		     */
		    if (XrmGetResource (db, "Family.CharSet", "family.charSet",
						    &resType, &resValue))
		      {
			charSet = (char *) resValue.addr;

			/*
			 * get the bitmap (optional)
			 */
			if (XrmGetResource (db,
					"Family.Bitmap", "family.bitmap",
					 &resType, &resValue))
			    bitmap = (char *) resValue.addr;
		      }
		    else
		      {
			fprintf (stderr,
				(GetMessage (1, 14,
				    "%s: character set resource missing\n")),
				family_name);
			return -1;
		      }
		  }
		else
		  {
		    fprintf (stderr,
				(GetMessage (1, 13,
					"%s: volumes resource missing\n")),
				family_name);
		    return -1;
		  }
	      }
	    else
	      {
		fprintf (stderr,
			(GetMessage (1, 12, "%s: abstract resource missing\n")),
			family_name);
		return -1;
	      }
	  }
	else
	  {
	    fprintf (stderr,
			(GetMessage (1, 11, "%s: title resource missing\n")),
			family_name);
	    return -1;
	  }
	
	if (title && abstract && list && charSet)
	  {
	    /*
	     * find out the position of the file pointer
	     */
	    filepos = ftell (out_topic);

	    /*
	     * write out the <TOPIC>
	     */
	    fprintf (out_topic, (GetMessage (3, 1, (char*)defaultTopic)),
							charSet, title);

	    /*
	     * write out the <TITLE>
	     */
	    fprintf (out_topic, (GetMessage (3, 2, (char*)defaultTitle14)),
								title);
	    fprintf (out_topic, "%s", (GetMessage (3, 3, "<P before 1 first 1 left 1>\n")));
	    fprintf (out_topic, "%s\n", abstract);
	    fprintf (out_topic, "</P>\n");

	    do 
	      {
		token = NULL;
		list = _DtHelpCeGetNxtToken(list, &token);
		if (token && *token != '\0' && *token != '\n' &&
				CreateVolumeLink (canvas,out_topic, token) == 0)
		    count++;

		if (token)
                {
		    free ((void *) token);
                    token = NULL;
                }

	      } while (list && *list != '\0');

	    if (count)
	      {
		result = 0;
		sprintf (familyName, "FAMILY%d", FamilyNum);
		fprintf (out_topic, "</PARAGRAPH>\n</TOPIC>\n");

		/*
		 * Put the link information in the header file
		 */
		fprintf (out_header,
			(GetMessage (3, 4, "<CHARACTERSET %s>\n")), charSet);
		fprintf (out_header, "<LINK 0 %s>\n", familyName);
		fprintf (out_header, (GetMessage (3, 5, (char*)defaultTitle12)),
								title);
		fprintf (out_header, "</LINK>\n");

		/*
		 * put the abstract information about this
		 * family in the header file
		 */
		if (NULL != bitmap && *bitmap != '/')
		  {
		    snprintf(bitmapName, sizeof(bitmapName), "%s", family_name);
		    ptr = strrchr (bitmapName, '/');
		    if (ptr)
		      {
			ptr++;
			*ptr = '\0';
			snprintf(bitmapNameTemp, sizeof(bitmapNameTemp), "%s%s", bitmapName, bitmap);
			strcpy(bitmapName, bitmapNameTemp);
			bitmap = bitmapName;
		      }
		    else
			bitmap = NULL;
		  }

		if (NULL != bitmap)
		  {
		    fprintf (out_header,
			(GetMessage (3, 6,
				"<P before 1 first 1 left 1 graphic %s glink %s gtypelink 0>\n")),
			bitmap, familyName);
		  }
		else
		    fprintf (out_header, "%s", GetMessage (3, 3, "<P before 1 first 1 left 1>\n"));
		fprintf (out_header, "%s\n", abstract);
		fprintf (out_header, "</P></CHARACTERSET>\n");

		/*
		 * put the information in the volume file.
		 */
		fprintf (out_volume, "*.%s.filepos: %ld\n",
						familyName, filepos);
		fprintf (out_volume, "*.%s.filename: %s\n",
						familyName, TopicName);
		TopicList = (char **) _DtHelpCeAddPtrToArray (
						(void **) TopicList,
						strdup (familyName));
	      }
	    else
	      {
		/*
		 * rewind back to the original starting position
		 */
		fseek (out_topic, filepos, 0);

		/*
		 * didn't find any volumes for this family.
		 */
		result = -2;
	      }
	  }
	XrmDestroyDatabase (db);
      }

    return result;
}

/*****************************************************************************
 * Function:   CheckFamilyList (name)
 *
 *  See if this family has been seen
 *
 *****************************************************************************/
int
CheckFamilyList (char    *name )
{
    char **listPtr = FamilyList;

    while (listPtr != NULL && *listPtr != NULL)
      {
	if (strcmp (*listPtr, name) == 0)
	    return True;
	listPtr++;
      }

    return False;
}

/*****************************************************************************
 * Function:   AddFamilyToList (name)
 *
 *  add the name to the family list
 *
 *****************************************************************************/
char **
AddFamilyToList (char    *name )
{

    FamilyList = (char **) _DtHelpCeAddPtrToArray ((void **) FamilyList,
							    strdup(name));
    return FamilyList;
}

/*****************************************************************************
 * Function:   ScanDirectory
 *
 *  scan a directory looking for family files.
 *
 *****************************************************************************/
void
ScanDirectory (
    char    *directory,
    long    *ret_time)
{
    DIR    *pDir;
    struct stat buf;

    char    fullName [MAXPATHLEN + 2];
    char   *ptr;
    char   *ext;

    struct dirent *pDirent;

    *ret_time = 0;
    if (stat(directory, &buf) == -1)
	return;

    *ret_time = buf.st_mtime;

    pDir = opendir (directory);
    if (pDir == NULL)
	return;

    snprintf(fullName, sizeof(fullName), "%s%s", directory, SlashString);
    ptr = fullName + strlen (fullName);

    /*
     * skip over the "." and ".." entries.
     */
    (void) readdir (pDir);
    (void) readdir (pDir);
    pDirent = readdir (pDir);
    while (pDirent)
      {
	ext = GetExtension (pDirent->d_name);
	if (strcmp (ext, Family_ext) == 0)
	  {
	    if (CheckFamilyList (pDirent->d_name) == False)
	      {
		AddFamilyToList (pDirent->d_name);

	        strcpy (ptr, pDirent->d_name);
		FullFamilyName = (char **) _DtHelpCeAddPtrToArray(
					(void **)FullFamilyName,
					strdup(fullName));
	      }
	  }
	else if (strcmp(ext, Ext_Hv) == 0 || strcmp(ext, Ext_Sdl) == 0)
	  {
	    strcpy (ptr, pDirent->d_name);
	    VolumeList  = (char **) _DtHelpCeAddPtrToArray((void **)VolumeList,
						strdup(pDirent->d_name));
	    FullVolName = (char **) _DtHelpCeAddPtrToArray((void **)FullVolName,
						strdup(fullName));
	  }

	pDirent = readdir (pDir);
      }

    closedir(pDir);
    return;
}

/*****************************************************************************
 * Function:   FindFile
 *
 *  Resolves the environment variable for all possible paths.
 *
 *****************************************************************************/
char *
FindFile (
    char *filename)
{
    int     i;
    int     trimExt = 0;
    int     different;
    char   *fileExt;
    char   *ext;
    struct stat status;

    fileExt = GetExtension(filename);
    if (*fileExt == '\0')
	trimExt = 1;

    i = 0;
    while (VolumeList != NULL && VolumeList[i] != NULL)
      {
	if (trimExt)
	  {
	    ext = GetExtension(VolumeList[i]);
	    *ext = '\0';
	  }

	different = strcmp(filename, VolumeList[i]);
	if (trimExt)
	   *ext = '.';

	if (!different && access(FullVolName[i], R_OK) == 0
			&& stat(FullVolName[i], &status) == 0
				&& S_ISDIR(status.st_mode) == 0)
	    return (FullVolName[i]);

	i++;
      }

    return NULL;
}

/*****************************************************************************
 * Function:   ExpandPaths
 *
 *  Resolves the environment variable for all possible paths.
 *
 *****************************************************************************/
void
ExpandPaths (
    char   *lang,
    char   *type,
    char   *env_var,
    char   *default_str,
    char ***list)
{
    short strip;
    char *ptr;
    char *hPtr;
    char *src;
    char *pathName;
    char *searchPath;

    searchPath = getenv (env_var);
    if (searchPath == NULL || *searchPath == '\0')
      {
	if (default_str == NULL)
	    return;

	searchPath = default_str;
      }

    searchPath = strdup (searchPath);

    *list = NULL;
    src   = searchPath;
    do 
      {
	ptr = strchr (src, ':'); 
	if (ptr)
	    *ptr = '\0';

	/*
	 * check to see if %H is declared. If so, we're going
	 * to have to trim it before saving as the directory path.
	 */
	strip = False;
	hPtr  = strrchr (src, '%');
	if (hPtr != NULL)
	  {
	    hPtr++;
	    if (*hPtr == 'H')
		strip = True;
	  }

	/*
	 * check to see if the path needs expanding
	 */
	if (NULL != strchr (src, '%'))
	    pathName = _DtHelpCeExpandPathname (src, NULL, type, NULL, lang,
					(_DtSubstitutionRec *) NULL, 0);
	else
	    pathName = strdup(src);

	if (pathName)
	  {
	    GetPath (pathName, strip, list);
	    free (pathName);
	  }

	if (ptr)
	  {
	    *ptr = ':';
	    ptr++;
	  }
	src = ptr;
      } while (src && *src);

    free(searchPath);
}

/*****************************************************************************
 * Function:   CheckTimeStamps
 *
 * Check the time stamps on the volume dir to determine if
 * it needs regenerating.
 *
 *****************************************************************************/
int
CheckTimeStamps (
    XrmDatabase	  db,
    char	**dir_list)
{
    long	 timeVal;
    char        *value;
    struct stat	 buf;

    while (*dir_list != NULL)
      {
	if (stat(*dir_list, &buf) == -1)
	    buf.st_mtime = 0;

	value = _DtHelpCeGetResourceString(db, *dir_list,
						"TimeStamp", "timeStamp");
	timeVal = atol(value);
	if (timeVal != buf.st_mtime)
	    return 1;

	dir_list++;
      }

    return 0;
}

/*****************************************************************************
 * Function:   CheckInfo
 *
 *  Check the information in the volume to determine if it needs regenerating.
 *
 *****************************************************************************/
int
CheckInfo (
    char *file)
{
    int		result = 1;
    char	**list1, **list2;
    char	**volDirList;
    XrmDatabase	db;

    db = XrmGetFileDatabase (file);
    if (db != NULL)
      {
	volDirList = _DtHelpCeGetResourceStringArray(db, NULL,
							"DirList", "dirList");
	if (volDirList != NULL)
	  {
	    result = 0;
	    list1  = volDirList;
	    list2  = FUserList;
	    while (result == 0 && *list1 != NULL
					&& list2 != NULL && *list2 != NULL)
	      {
	        result = strcmp(*list1, *list2);
		list1++;
		list2++;
	      }

	    if (list2 != NULL && *list2 != NULL)
		result = 1;

	    list2  = FSysList;
	    while (result == 0 && *list1 != NULL
					&& list2 != NULL && *list2 != NULL)
	      {
	        result = strcmp(*list1, *list2);
		list1++;
		list2++;
	      }

	    if (list2 != NULL && *list2 != NULL)
		result = 1;

	    list2  = VUserList;
	    while (result == 0 && *list1 != NULL
					&& list2 != NULL && *list2 != NULL)
	      {
	        result = strcmp(*list1, *list2);
		list1++;
		list2++;
	      }

	    if (list2 != NULL && *list2 != NULL)
		result = 1;

	    list2  = VSysList;
	    while (result == 0 && *list1 != NULL
					&& list2 != NULL && *list2 != NULL)
	      {
	        result = strcmp(*list1, *list2);
		list1++;
		list2++;
	      }

	    if (*list1 != NULL || (list2 != NULL && *list2 != NULL))
		result = 1;

	    if (result == 0)
		result = CheckTimeStamps(db, volDirList);

	    _DtHelpCeFreeStringArray(volDirList);
	  }
	XrmDestroyDatabase(db);
      }

    return result;
}

/***************************************************************************** 
 *			Main routine
 *****************************************************************************/
int
main(
    int    argc,
    char  *argv[] )
{
    int      i;
    int      result;
    int      foundFamily;
    int      foundVolumes;
    int      usedUser = 0;
    int      doGen    = 0;

    char     tmpVolume  [MAXPATHLEN + 2];
    char     tmpVolumeTemp[sizeof(tmpVolume)];
    char     tmpVolume2 [MAXPATHLEN + 2];
    char     tmpTopic   [MAXPATHLEN + 2];
    char     tmpHeader  [MAXPATHLEN + 2];
    char     headerName [MAXPATHLEN + 2];
    char     baseName   [MAXPATHLEN + 2];
    char     baseNameTemp[sizeof(baseName)];
    char     tempName   [MAXPATHLEN + 2];
    char   **next;
    char    *charSet;
    char    *topicTitle;
    char    *ptr;
    char    *endDir;

    long    preamble;
    long    modTime;
    FILE   *outVolume;
    FILE   *outTopic;
    FILE   *outHeader;
    pid_t   childPid = (pid_t) -1;
    CanvasHandle canvasHandle;

    myName = strrchr (argv[0], '/');
    if (myName)
	myName++;
    else
	myName = argv[0];

   /*
    * have to do a setlocale here, so that the usage message is in the
    * correct language.
    */
   Lang = getenv ("LANG");

   /*
    * use the default if no lang is specified.
    */
   if (Lang == NULL || *Lang == '\0')
	Lang = (char*)C_String;

    setlocale(LC_ALL, "");
    _DtEnvControl(DT_ENV_SET);

    /*
     * now process the arguments
     */
    for (i = 1; i < argc; i++)
      {
	if (argv[i][0] == '-')
	  {
	    if (argv[i][1] == 'd' && i + 1 < argc)
	        App_args.dir = argv[++i];
	    else if (argv[i][1] == 'f' && i + 1 < argc)
	        App_args.file = argv[++i];
	    else if (argv[i][1] == 'g')
		doGen = 1;
	    else if (argv[i][1] == 'l' && i + 1 < argc)
	        App_args.lang = argv[++i];
	    else
	      {
	        fprintf (stderr, (GetMessage(1,1, ((char*)UsageStr))), myName);
	        exit (1);
	      }
	  }
	else
	  {
	    fprintf (stderr, (GetMessage(1,1, ((char*)UsageStr))), myName);
	    exit (1);
	  }
      }

    /*
     * get the language we are working with.
     */
    if (App_args.lang != NULL)
      {
        /*
         * Set the locale! Since the user has specified a (likely)
	 * different language to do the processing in, we need to
	 * do a setlocale to work with the new language.
         */
	Lang = App_args.lang;
        if (setlocale(LC_ALL, Lang) == NULL)
          {
	    fprintf (stderr, (GetMessage(1, 20,
			"%s: Invalid system language specified - %s\n")),
				myName, Lang);
	    exit (1);
          }
        _DtEnvControl(DT_ENV_SET);
      }
    Lang = strdup(Lang);

    /*
     * get the directory to work in
     */
    if (NULL == App_args.dir)
      {
	fprintf (stderr, (GetMessage(1,1, ((char*)UsageStr))), myName);
	exit (1);
      }

    if (App_args.dir[0] != '/')
      {
	if (getcwd (baseName, MAXPATHLEN) == NULL)
	  {
	    fprintf (stderr, (GetMessage (1, 18,
 "%s: Unable to access current working directory - error status number %d\n")),
				myName, errno);
	    exit (1);
	  }
        snprintf(baseNameTemp, sizeof(baseNameTemp), "%s/%s", baseName, App_args.dir);
        strcpy(baseName, baseNameTemp);
      }
    else
        snprintf(baseName, sizeof(baseName), "%s", App_args.dir);

    /*
     * make sure the directory exists
     */
    ptr = _DtHelpCeExpandPathname (baseName, NULL, "help", NULL, Lang,
					(_DtSubstitutionRec *) NULL, 0);
    if (ptr == NULL || *ptr == '\0')
      {
	fprintf (stderr,
		(GetMessage (1, 15, "%s: Destination directory missing\n")),
			myName);
	exit (1);
      }

    snprintf(tmpVolume, sizeof(tmpVolume), "%s", ptr);
    if (tmpVolume[strlen (tmpVolume) - 1] != '/') {
        snprintf(tmpVolumeTemp, sizeof(tmpVolumeTemp), "%s%s", tmpVolume, SlashString);
        strcpy(tmpVolume, tmpVolumeTemp);
    }

    free (ptr);

    /*
     * march down the path, checking that
     *		1) it exists
     *		2) the caller has access permission.
     *		3) resolve all symbolic links
     */
    endDir = strchr (tmpVolume, '/');
    if (endDir != NULL)
      {
	endDir++;
	endDir = strchr (endDir, '/');
      }
    while (endDir && *endDir != '\0')
      {
	/*
	 * remember the rest of the string (including the slash)
	 * and strip the trailing slash from the directory path.
	 */
	snprintf(tmpVolume2, sizeof(tmpVolume2), "%s", endDir);
	*endDir = '\0';

	/*
	 * trace the path and copy the new string into the old buffer.
	 */
	ptr = _DtHelpCeTracePathName(tmpVolume);
	if (ptr != NULL)
	  {
	    snprintf(tmpVolume, sizeof(tmpVolume), "%s", ptr);
	    free (ptr);
	  }

	if (access (tmpVolume, F_OK) == -1)
	  {
	    switch (errno)
	      {
		case ENOTDIR:
			ptr = GetMessage (1, 2, (char*)NotDirectory);
			fprintf (stderr, ptr, myName, tmpVolume);
			exit (1);

		case EACCES:
			ptr = GetMessage (1, 3, (char*)SuperMsg);
			fprintf (stderr, ptr, myName, tmpVolume);
			exit (1);

		case ENOENT:
			if (mkdir(tmpVolume,
			    (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) == -1
				&& errno != EEXIST && errno != EROFS)
			  {
			    switch (errno)
			      {
				case ENOTDIR:
					ptr = GetMessage(1,2,
							(char*)NotDirectory);
					break;

				case EACCES:
					ptr = GetMessage(1, 3, 
							(char*)SuperMsg);
					break;

				case ENOENT:
					ptr = GetMessage(1, 4,
				    "%s: Element of %s does not exist\n");
					break;

				case ENOSPC:
					ptr = GetMessage (1, 5,
				"%s: File system containing %s is full\n");
					break;

				default:
					ptr = GetMessage(1,6,
							(char*)GeneralAccess);
					break;
			      }
			    fprintf (stderr, ptr, myName, tmpVolume, errno);
			    exit (1);
			  }
			break;
		default:
			ptr = GetMessage (1, 6, (char*)GeneralAccess);
			fprintf (stderr, ptr, myName, tmpVolume, errno);
			exit (1);
	      }
	  }

	/*
	 * point to the end of the string (past where the slash will go)
	 */
	endDir = tmpVolume + strlen(tmpVolume) + 2;

	/*
	 * append the rest of the directory spec that hasn't been checked.
	 */
	strcat (tmpVolume, tmpVolume2);
	endDir = strchr (endDir, '/');
      }


    /*
     * get temporary files for the volume and topic file.
     */
    snprintf(tmpVolumeTemp, sizeof(tmpVolumeTemp), "%s%s", tmpVolume, App_args.file);
    strcpy(tmpVolume, tmpVolumeTemp);

    (void) strcpy (tmpHeader, tmpVolume);
    (void) strcpy (tmpTopic, tmpVolume);

    snprintf(tmpVolumeTemp, sizeof(tmpVolumeTemp), "%s%s", tmpVolume, Ext_Hv);
    strcpy(tmpVolume, tmpVolumeTemp);
    (void) strcat (tmpHeader, "00.ht");
    (void) strcat (tmpTopic , "01.ht");

    result = access (tmpVolume, F_OK);

    /*
     * If it exists, make sure the invoker can write to it.
     */
    if (result == 0)
      {
	if (access (tmpVolume, W_OK) == -1)
	  {
	    if (errno == EROFS)
		ptr = GetMessage (1, 7,
			"%s: File system containing %s is read only\n");
	    else if (errno == EACCES)
		ptr = GetMessage (1, 8,
			"%s: Requires root permission to write to %s\n");
	    else
		ptr = GetMessage (1, 9, (char*)WriteInvalid);

	    fprintf (stderr, ptr, myName, tmpVolume, errno);
	    exit (1);
	  }
      }
    else if (result == -1 && errno != ENOENT)
      {
	ptr = GetMessage (1, 6, (char*)GeneralAccess);
	fprintf (stderr, ptr, myName, tmpVolume, errno);
	exit (1);
      }
    else /* file does not exist */
	doGen = 1;


    /*
     * Find out if we have any paths to check.
     */
    ExpandPaths(Lang, "families", DtUSER_FILE_SEARCH_ENV, NULL, &FUserList);
    ExpandPaths(Lang, "volumes", DtUSER_FILE_SEARCH_ENV, NULL, &VUserList);
    ExpandPaths(Lang, "families", DtSYS_FILE_SEARCH_ENV ,
					DtDEFAULT_SYSTEM_PATH, &FSysList);
    ExpandPaths(Lang, "volumes", DtSYS_FILE_SEARCH_ENV ,
					DtDEFAULT_SYSTEM_PATH, &VSysList);
    if (((FUserList == NULL || *FUserList == NULL) &&
	(FSysList  == NULL || *FSysList  == NULL)) ||
        ((VUserList == NULL || *VUserList == NULL) &&
	(VSysList  == NULL || *VSysList  == NULL)))
      {
	ptr = GetMessage (1, 10, "%s: Search Path empty\n");
	fprintf (stderr, ptr, myName);
	exit (1);
      }

    /*
     * If we already haven't determined that the volume needs (re)generating
     * check the info squirreled away in the old volume.
     */
    if (doGen == 0)
	doGen = CheckInfo(tmpVolume);

    /*
     * the volume doesn't need (re)generating.
     * exit now.
     */
    if (doGen == 0)
	exit(0);

    /*
     * create a canvas for the functions.
     */
    canvasHandle = _DtHelpCeCreateDefCanvas();
    if (canvasHandle == NULL)
      {
	fprintf (stderr, GetMessage(1, 19,"%s: Unable to allocate memory\n"),
								myName);
	MyExit (1, -1);
      }

    /*
     * open the individual files that will hold the browser information.
     * <file>.hv
     */
    outVolume = fopen (tmpVolume, "w");
    if (outVolume == NULL)
      {
	ptr = GetMessage (1, 9, (char*)WriteInvalid);
	fprintf (stderr, ptr, myName, tmpVolume, errno);
        _DtHelpCeDestroyCanvas(canvasHandle);
	MyExit (1, -1);
      }

    /*
     * <file>00.ht
     */
    outTopic = fopen (tmpTopic, "w");
    if (outTopic == NULL)
      {
	ptr = GetMessage (1, 9, (char*)WriteInvalid);
	fprintf (stderr, ptr, myName, tmpTopic, errno);
	(void) unlink (tmpVolume);
        _DtHelpCeDestroyCanvas(canvasHandle);
	MyExit (1, -1);
      }

    /*
     * <file>01.ht
     */
    outHeader = fopen (tmpHeader, "w");
    if (outHeader == NULL)
      {
	ptr = GetMessage (1, 9, (char*)WriteInvalid);
	fprintf (stderr, ptr, myName, tmpHeader, errno);
	(void) unlink (tmpVolume);
	(void) unlink (tmpTopic);
        _DtHelpCeDestroyCanvas(canvasHandle);
	MyExit (1, -1);
      }

    /*
     * fork off the dtksh script that will put up a dialog
     * telling the user that we're building help browser
     * information.
     */
#ifdef	__hpux
    childPid = vfork();
#else
    childPid = fork();
#endif
    /*
     * if this is the child, exec the dthelpgen.ds script.
     */
    if (childPid == 0)
      {
	execlp("dthelpgen.ds", "dthelpgen.ds",
				((char *) 0), ((char *) 0), ((char *) 0));
	_exit(1);
      }

    /*
     * initialize the main topic
     */
    strcpy (TopicName, App_args.file);
    strcat (TopicName, "01.ht");

    strcpy (headerName, App_args.file);
    strcat (headerName, "00.ht");

    /*
     * The original dthelpgen extracts the CDE Standard name from
     * its message catalogue( set 2/msg 1 ).
     * But on IBM ODE, this is a problem. For example,
     * fr_FR's dthelpgen.cat has 
     *    fr_FR.ISO-8859-1 in set 2/msg 1.
     * Correct Fr_FR's message catalogue must have,
     *    fr_FR.IBM-850
     * there. But current IBM ode's Makefile cannot do this. Instead put
     *    fr_FR.ISO-8859-1. ( That is "do nothing" ).
     * To fix this, dthelpgen converts the current IBM LANG to CDE
     * standard name with _DtLcx*() function provided by libDtHelp.a as
     * internal API.
     */
#ifdef _AIX
    {
	_DtXlateDb db = NULL;
	int  ret;
	char plat[_DtPLATFORM_MAX_LEN];
	int  execver;
	int  compver;
	char *ret_stdLocale;
	char *ret_stdLangTerr;
	char *ret_stdCodeset;
	char *ret_stdModifier;

	ret = _DtLcxOpenAllDbs( &db );
	if ( !ret ) {
	    ret = _DtXlateGetXlateEnv( db, plat, &execver, &compver );
	    if ( !ret ) {
		ret =  _DtLcxXlateOpToStd( db, plat, execver,
					DtLCX_OPER_SETLOCALE,
					setlocale( LC_MESSAGES, NULL ),
					&ret_stdLocale, &ret_stdLangTerr,
					&ret_stdCodeset, &ret_stdModifier );
		if ( !ret ) {
		    charSet = strdup( ret_stdLocale );
		} else {
		    charSet = "C.ISO-8859-1";
		}
	    } else {
		charSet = "C.ISO-8859-1";
	    }
	    ret = _DtLcxCloseDb( &db );
	} else {
	    charSet = "C.ISO-8859-1";
	}

    }
#else /* _AIX */
    charSet    = strdup (GetMessage (2, 1, "C.ISO-8859-1"));
#endif /* _AIX */
    topicTitle = strdup (GetMessage (2, 2, "Welcome to Help Manager"));

    fprintf (outHeader, (GetMessage (3, 1, (char*)defaultTopic)),
						charSet, topicTitle);

    fprintf (outHeader, (GetMessage (3, 2, (char*)defaultTitle14)), topicTitle);
    free(topicTitle);

    preamble = ftell (outHeader);
    fprintf (outHeader, "%s\n", GetMessage (2, 3, (char*)defaultTextBody));

    /*
     * loop through the directories looking for all the unique families
     * and -all- the volumes.
     */
    fprintf (outVolume, "!# Last modification time stamp per directory\n");
    next = FUserList;
    while (next != NULL && *next != NULL)
      {
	ScanDirectory(*next, &modTime);
	fprintf (outVolume, "*.%s.timeStamp:   %ld\n" , *next, modTime);
	next++;
      }
    next = FSysList;
    while (next != NULL && *next != NULL)
      {
	ScanDirectory(*next, &modTime);
	fprintf (outVolume, "*.%s.timeStamp:   %ld\n" , *next, modTime);
	next++;
      }

    next = VUserList;
    while (next != NULL && *next != NULL)
      {
	ScanDirectory(*next, &modTime);
	fprintf (outVolume, "*.%s.timeStamp:   %ld\n" , *next, modTime);
	next++;
      }
    next = VSysList;
    while (next != NULL && *next != NULL)
      {
	ScanDirectory(*next, &modTime);
	fprintf (outVolume, "*.%s.timeStamp:   %ld\n" , *next, modTime);
	next++;
      }

    fprintf (outVolume, "*.charSet:	%s\n", charSet);
    free(charSet);
    fprintf (outVolume, "\n!# Topic filenames and offsets\n");

    /*
     * Now create families.
     */
    foundVolumes = 0;
    foundFamily  = 0;
    for (next = FullFamilyName; next != NULL && *next != NULL; next++)
      {
	result = CreateFamily(canvasHandle,*next,outVolume,outHeader,outTopic);
	if (result == 0)
	  {
	    FamilyNum++;
	    foundVolumes = 1;
	    foundFamily  = 1;
	  }
	else if (result == -2)
	    foundFamily  = 1;
      }

    if (foundFamily == 0)
	fprintf(stderr,
		GetMessage(1, 16, "%s: Zero Family files found\n"), myName);
    else if (foundVolumes == 0)
	fprintf (stderr,
		GetMessage (1, 17, "%s: Zero Volume files found\n"), myName);

    /*
     * Clean up
     */
    if (FamilyList != NULL)
	_DtHelpCeFreeStringArray(FamilyList);
    if (FullFamilyName != NULL)
	_DtHelpCeFreeStringArray(FullFamilyName);
    if (VolumeList != NULL)
	_DtHelpCeFreeStringArray(VolumeList);
    if (FullVolName != NULL)
	_DtHelpCeFreeStringArray(FullVolName);

    /*
     * If no family or volume files were found,
     * write out the alternative preamble
     */
    if (foundFamily == 0 || foundVolumes == 0)
      {
	fseek (outHeader, preamble, 0);
	fprintf (outHeader, "%s\n", GetMessage (2, 5, (char*)defaultAlternate));
      }

    /*
     * write the ending message and finish the topic.
     */
    fprintf (outHeader, "</TOPIC>\n");

    /*
     * write out the volume resouces
     */
    fprintf (outVolume, "\n\n!# Top (or home) topic filename and offset\n");
    fprintf (outVolume, "*.%s.filepos:   0\n" , ParentName);
    fprintf (outVolume, "*.%s.filename:  %s\n", ParentName, headerName);

    fprintf (outVolume, "\n\n!# Volume Title\n");
    fprintf (outVolume, "*.title:     %s\n",
                      GetMessage (2, 4, "Help - Top Level"));

    /*
     * top topic
     */
    fprintf (outVolume, "\n\n!# Topic Home Location\n");
    fprintf (outVolume, "*.topTopic:  %s\n", ParentName);

    /*
     * topic hierarchy
     */
    fprintf (outVolume, "\n\n!# Topic Hierarchy\n");
    fprintf (outVolume, "*.%s.parent:  \n", ParentName);
    for (next = TopicList; next && *next; next++)
        fprintf (outVolume, "*.%s.parent: %s\n", *next, ParentName);

    /*
     * topic list
     */
    fprintf (outVolume, "\n\n!# Topic List\n");
    fprintf (outVolume, "*.topicList: %s", ParentName);
    next = TopicList;
    while (next && *next)
      {
        fprintf (outVolume, " \\\n	%s", *next);
	next++;
      }
    fprintf (outVolume, "\n");

    /*
     * The paths used to create this information.
     */
    fprintf (outVolume, "\n\n!# Paths Searched\n");
    fprintf (outVolume, "*.dirList: ");

    next = FUserList;
    while (next != NULL && *next != NULL)
      {
        fprintf (outVolume, " \\\n	%s", *next);
        next++;
      }
    next = FSysList;
    while (next != NULL && *next != NULL)
      {
        fprintf (outVolume, " \\\n	%s", *next);
        next++;
      }
    next = VUserList;
    while (next != NULL && *next != NULL)
      {
        fprintf (outVolume, " \\\n	%s", *next);
        next++;
      }
    next = VSysList;
    while (next != NULL && *next != NULL)
      {
        fprintf (outVolume, " \\\n	%s", *next);
        next++;
      }
    fprintf (outVolume, "\n");

    /*
     * close the volumes.
     */
    fclose (outVolume);
    fclose (outHeader);
    fclose (outTopic);

    if (TopicList != NULL)
        _DtHelpCeFreeStringArray(TopicList);

    if (FUserList != NULL)
        _DtHelpCeFreeStringArray(FUserList);
    if (FSysList != NULL)
        _DtHelpCeFreeStringArray(FSysList);
    if (VUserList != NULL)
        _DtHelpCeFreeStringArray(VUserList);
    if (VSysList != NULL)
        _DtHelpCeFreeStringArray(VSysList);

    _DtHelpCeDestroyCanvas(canvasHandle);

    MyExit (0, childPid);
}
