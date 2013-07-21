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
 * File:         DbReader.c $TOG: DbReader.c /main/9 1998/05/14 11:16:57 mgreess $
 * Language:     C
 *
 * (c) Copyright 1991, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <Dt/DtP.h>
#include <Dt/UserMsg.h>
#include <Dt/Connect.h>
#include <Dt/DbUtil.h>
#include <Dt/DbReader.h>
#include <Dt/DtsDb.h>
#include <Dt/DtNlUtils.h>
#include <Tt/tt_c.h>
#include "DtSvcLock.h"

#define TRUE	1
#define FALSE	0

/*
 * The default size of an input buffer
 */
#define MAX_LINE_LENGTH	1024

/* Structure to hold $variable definitions */
typedef struct {
   char * varName;
   char * value;
} VariableEntry;


static int variableCount;
static VariableEntry * variableSet;

/*
 * The following variables are used to keep track of the 
 * number of fields that have been allocated.  These are
 * needed to preallocate a reasonably large number of
 * fields in advance because DtDbRead supports a 
 * 'DTUNLIMITEDFIELDS' number of fields.
 */
#define NUMBER_FIELDS_DEFAULT		16
#define NUMBER_FIELDS_INCREMENT		8

/*
 * The following variables are used in ReadNextEntry.
 * They are global because there space is malloc'ed
 * in _DtDbRead but potentially realloc'ed in some
 * other function (e.g. ResolveVariableReferenc).
 */

/********    Static Function Declarations    ********/
static int ReadNextEntry( 
                        Boolean * versionCanBeSet,
                        FILE *fd,
                        DtDtsDbField ** fieldPtr,
                        DtDbRecordDesc * recordDescriptions,
			int numRecordDescriptions,
                        int * returnRrecordIndex,
			char *file_name,
			int *NumberFieldsAllocated,
			char **tmpBuffer,
			char **tmpLine) ;
static void ResolveVariableReference( 
                        char *varName,
                        int varNameLen,
                        char **buf,
                        int *buf_size,
                        int start,
                        int len,
                        int *buf_len,
			int escape,
			int brackets) ;
static char *read_line( 
			char string[],
			int n,
        		FILE *fd) ;
static void clean_line( 
                        char **string) ;
void _DtDbFillVariables (
			char **line ) ;
static Boolean get_variable ( 
			char *key,
			int key_len,
			char **value,
			int escape,
			int brackets,
			char *var) ;
static void ClearVariables( void ) ;
static Boolean DefineVariable( 
                        Boolean * versionCanBeSet,
                        char *varString,
                        char * fileName,
			int *variableSetSize) ;
static void SplitField(
                        DtDtsDbField ** fields,
                        int * slotToUse,
                        char * ptr,
			int *NumberFieldsAllocated) ;
static int MatchKeyword(
                        char * line,
                        DtDbRecordDesc * recordDescriptions,
                        int numRecordDescriptions);
static void FreeDbField(
                        DtDtsDbField * fields) ;
static void InitializeLocalizedStrings( void ) ;

/********    End Static Function Declarations    ********/

#define VERSION_QUERY_STR "$DtDbVersion"
#define VERSION_KEYWORD   "DtDbVersion"
#define VERSION_ID        "1.0"


/*
 * Pointers to localized strings.
 */
static char * incompleteDefn;
static char * multiLineDefn;
static char * tooManyFields;
static char * emptyTypesDirs;
static char * noStartSymbol;
static char * invalidStartSymbol;
static char * missingStartSymbol;
static char * cantSetVersion;
static char * invalidVersion;

/********************
 *
 * Function Name:  _DtDbPathIdToString
 *
 * Description:
 *
 * 	This function maps a path Id, supplied by DtDbLoad during the
 *	loading of a database, into the associated string value.
 *
 * Synopsis:
 *
 *	path = _DtDbPathIdToString (pathId);
 *
 *	char * path;		A pointer to the string containing the path
 *				associated witht the Id.  This memory is
 *				owned by the caller, and should be freed up
 *                              when no longer needed.
 *	DtDbPathId pathId;	The Id for the path.
 *
 ***********************/
   
char * 
_DtDbPathIdToString(
        DtDbPathId pathId )
{
   char * path;
   char * newPath;

   if ((path = XrmQuarkToString((XrmQuark)pathId)))
   {
      /* Allocate some storage for the string */
      newPath = (char *) XtNewString(path);
      return(newPath);
   }

   /* Bogus Id */
   return(NULL);
}

/********************
 *
 * Function Name:  DtDbRead
 *
 * Description:
 *
 * 	This function loads a collection of database records, located within
 *      database files. The database files are located within
 *      the specified set of directories, and match the specified file
 *      suffix.  This function does not interpretation of the database
 *      entries, other than to break each field into a keyword/value pair.
 *      For each record loaded, it will be passed to a converter function,
 *      which is responsible for parsing the fields within the record, and
 *      then adding the entry to the appropriate in-memory database structure.
 *
 *      If the database files to be loaded are located in the standard
 *      Dt database locations, then you should call _DtGetDatabaseDirPaths() 
 *      to obtain the 'dirs' info.
 *
 * Synopsis:
 *
 *	_DtDbRead (dirs, suffix, recordDescriptions, numRecordDescriptions);
 *
 *	DtDirPaths *dirs;	A structure containing the names of all the
 *				directories to search for DB files.
 *	char *suffix;		A filename suffix that identifies which files
 *				in the "dirs" directories are DB files to
 *				be loaded.
 *      DtDbRecordDesc ** recordDescriptions
 *                              An array, where each entry describes a
 *                              database record which should be loaded during
 *                              this pass of _DtDbRead().  See the definition
 *                              of this structure, for more details.
 *
 *      int numRecordDescriptions
 *                              The number of entries in the above array.
 *
 ***********************/

void
_DtDbRead (
        DtDirPaths *dirs,
        char *suffix,
	DtDbRecordDesc * recordDescriptions,
        int numRecordDescriptions )
{
   
/* LOCAL VARIABLES */
   
   DtDirPaths *db_files;/* An array of string pointers which point to the
			   names of all the files to be read. */
   FILE *fd;		/* File descriptor for the file being processed. */
   DtDtsDbField *entryFields;/* An array of keyword/value string pairs,
                               representing the record information. */
   XrmQuark pathIndex;  /* Index associated with db file */
   DtDirPaths tmp_dirs; /* Temporary list of directories. */
   int nextIndex = 0;
   int i, j;
   int recordIndex;
   Boolean rejectionStatus;
   static Boolean first_time = True;
   static char local_host[MAXHOSTNAMELEN];
   Boolean versionCanBeSet;
   char * verString;
   int NumberFieldsAllocated = 0;
   char *tmpBuffer = NULL;	/* Buffer to store a field value that may
				   grow to an unlimited size */
   char *tmpLine = NULL; 	/* Temporary input line */

/* CODE */

   _DtSvcProcessLock();
   if (first_time) 
   {
      first_time = False;
      InitializeLocalizedStrings ();
      DtGetShortHostname (local_host, MAXHOSTNAMELEN);
   }
   _DtSvcProcessUnlock();

   /* Verify the incoming parameters */
   if (dirs == NULL)
      return;

   tmpLine = XtMalloc (MAX_LINE_LENGTH);
   tmpBuffer = XtMalloc (MAX_LINE_LENGTH);

   tmp_dirs.paths = (char **) XtMalloc (2 * sizeof (char *));
   tmp_dirs.dirs  = (char **) XtMalloc (2 * sizeof (char *));
   tmp_dirs.dirs[1] = tmp_dirs.paths[1] = NULL;

   /*
    * The fields for each record will temporarily be stored
    * in the variable 'entryFields'.  Instead of allocating
    * space for this on an as needed basis and then freeing
    * the space after a record is input, search the record 
    * descriptions for the largest number of fields and
    * set entryFields to this number of fields.  Before this
    * function returns, entryField will be free'd.
    */
   NumberFieldsAllocated = NUMBER_FIELDS_DEFAULT;
   for (i = 0; i < numRecordDescriptions; i++) 
   {
      if (recordDescriptions[i].maxFields > NumberFieldsAllocated)
	 NumberFieldsAllocated = recordDescriptions[i].maxFields;
   }
   entryFields = (DtDtsDbField *) XtMalloc ((NumberFieldsAllocated + 1) *
					    sizeof (DtDtsDbField));

   /* Load all of the database files in each of the specified directories. */
   for (i=0; dirs->dirs[i] != NULL; i++) 
   {
      char *host = NULL,
	   *dir = NULL,
	   *pch = NULL,
	   *tmp = NULL,
	   *host_prefix = NULL;

      /* Locate all matching db file in the current directory */
      tmp_dirs.dirs[0]  = (char *) dirs->dirs[i]; 
      tmp_dirs.paths[0] = (char *) dirs->paths[i];
      db_files = _DtFindMatchingFiles(&tmp_dirs, suffix, True);
      nextIndex=0;

      /* Determine the "host_prefix" for this directory. */
      tmp  = (char *) XtNewString (dirs->dirs[i]);
      host = tmp;
      if ((pch = DtStrchr (tmp, ':')) != NULL)
      {
         char * netfile;
         Tt_status status;
         int charLen;
         char * lastChar;
         char * hpx;

	 dir = pch + 1;
	 *pch = '\0';
	 if (strcmp (local_host, host)) 
	 {
            netfile = tt_host_file_netfile(host, "/");
            if ((status = tt_ptr_error(netfile)) == TT_OK)
            {
               hpx = tt_netfile_file(netfile);
               tt_free(netfile);
               status = tt_ptr_error(hpx);
            }

            if (status != TT_OK)
	       host_prefix = NULL;
            else if (hpx)
            {
               DtLastChar(hpx, &lastChar, &charLen);
               if ((charLen != 1) || (*lastChar != '/'))
               {
                  host_prefix = XtMalloc(strlen(hpx) + 2);
                  strcpy(host_prefix, hpx);
                  strcat(host_prefix, "/");
               }
               else
                  host_prefix = XtNewString(hpx);

               tt_free(hpx);
            }
	 }
	 else {
	    /*
	     * The database host is the same as the invoking host.
	     */
	    host_prefix = NULL;
	 }
      }
      XtFree ((char *) tmp);

      /* Open each of the db files, and extract any matching records */
      while (db_files->paths[nextIndex] != NULL) 
      {
	 /* If we can't open the next db file, just skip it. */
	 if ((fd = fopen (db_files->paths[nextIndex], "r")) == NULL) 
	 {
	    nextIndex++;
	    continue;;
	 }

	 /* Variables do not transfer across database files */
	 ClearVariables();
         versionCanBeSet = True;
	 pathIndex = XrmStringToQuark(db_files->dirs[nextIndex]);
	 
	 /* Read and process each entry in the file. */
	 while (ReadNextEntry (&versionCanBeSet, fd, &entryFields, 
                               recordDescriptions, numRecordDescriptions, 
                               &recordIndex, db_files->dirs[nextIndex], 
			       &NumberFieldsAllocated, &tmpBuffer, &tmpLine)) 
	 {
	    /* 
             * Invoke all of the callbacks registered for this record type,
             * allowing each one to parse the entry, and add it to the
             * appropriate in-memory database. 
             */
            for (j = 0, rejectionStatus = False;
                 recordDescriptions[recordIndex].converters[j]; j++)
            {
	       rejectionStatus |= 
                     (*recordDescriptions[recordIndex].converters[j]) ( 
                                      entryFields, pathIndex, host_prefix,
                                      rejectionStatus);
            }

            /* Free up the record information */
            FreeDbField(entryFields);
	 }
	 (void)fclose (fd);
	 nextIndex++;
      }

      _DtFreeDatabaseDirPaths (db_files);
      if (host_prefix != NULL)
	 XtFree (host_prefix);
   
   }

   XtFree ((char *) entryFields);
   XtFree (tmpLine);
   XtFree (tmpBuffer);
   XtFree ((char *) tmp_dirs.paths);
   XtFree ((char *) tmp_dirs.dirs);
}

/*********************
 *
 * Function Name:  get_variable
 *
 * Description:
 *
 *	Given a string, this function determines if the string is one of
 *      local variables or an environment variable and if it is one of
 *      these, it returns the actual vaulue of the variable.
 *
 *      The local database variables (set VAR=something) take precedence.
 *
 * Synopsis:
 *
 *	ret_value = get_variable ( key, key_len, value )
 *
 *      Boolean ret_value;	True if the variable is found, False
 *				otherwise.
 *
 *	char *key;  		The variable name to search for.
 *
 *      int key_len;		The length of the key string.
 *
 *      char **value;		The value of the variable;
 * 
 *      char *var;             Caller allocates space for character array
 *
 *	MODIFIED:	value
 *
 ***********************/

static
Boolean
get_variable ( 
		char *key,
		int key_len,
		char **value,
		int escape,
		int bracketSeen,
		char *var)
{
   char **ppchar;
   int i;
   extern char **environ;  
   char *tmp;

   _DtSvcProcessLock();
   for (i = 0; i < variableCount; i++)
      if ((strncmp(key, variableSet[i].varName, (size_t)key_len) != 0) ||
          ((int)strlen(variableSet[i].varName) != key_len))
         continue;
      else 
      {
	 *value = variableSet[i].value;
	 _DtSvcProcessUnlock();
	 return (True);
      }
   _DtSvcProcessUnlock();

   if(!use_in_memory_db)
   {
   /* 
    * The variable name was not found in the variable list, so look
    * for it in the environment data.
    */
       for (i=0, ppchar = environ; *ppchar; *ppchar++, i++) 
       {
	  if ((tmp = (char *) DtStrrchr (*ppchar, '=')) != NULL)
	  {
	     int  evar_len = tmp - *ppchar;
	     if ( (key_len == evar_len) 
	       && (strncmp (key, *ppchar, key_len) == 0)) 
	     {
		*value = tmp + 1;
		return (True);
	     }
	  }
       }
	*value = NULL;
	return(False);
   }
   else
   {
	/* just put it back as "${value}" */
	if(escape)
	{
		strcat(var, "\\");
	}
	if( bracketSeen )
	{
	       strcat(var, "${");
	}
	else
	{
		strcat(var, "$");
	}
	strncat(var, key, key_len);
	if( bracketSeen )
	{
		strcat(var, "}");
      	}

       *value = var;
       return (True);
   }
}

/*********************
 *
 * Function Name:  read_line
 *
 * Description:
 *
 *	This function reads in one line from a database file
 *      and returns the string.  This is a modified version
 *      of K&R's "fgets" (Second Edition). Note that the EOLN
 *      mark is replaced with a '\0'.
 *
 * Synopsis:
 *
 *	ret_value = read_line (string, n, fd);
 *
 *	char *ret_vaulue;  	NULL if EOF, string otherwise 
 *
 *	char string[];  	The array to put the line into.
 *
 *	int n; 			Max number of chars to read.
 *
 *	FILE * fd;		The file descriptor to read from.
 *
 ***********************/

static
char*
read_line (
	char string[],			/* MODIFIED */
	int n,
        FILE *fd )
{
   if ((fgets (string, n, fd)) == NULL)
      return (NULL);
   else {
      string[strlen (string) - 1] = '\000';
      return (string);
   }
}

/*********************
 *
 * Function Name:  clean_line
 *
 * Description:
 *
 *	This function removes the slashes from a string unless the slash 
 *      is preceeded by a slash.  It also remove whitespace from the end
 *	of the string.
 *
 * Synopsis:
 *
 *	(void) = clean_line (string)
 *
 *	char *string;  		The array to put the line into.
 *
 *      MODIFIED:	string
 *
 ***********************/

static
void
clean_line (
	char **string )
{
   int i, j=0, len;
   char *ch=*string;
   char *ret_string;
   char *head;
   char *tail;
   int _found;

   ret_string = (char *) *string;

   /*
    * Don't search \ by "byte by byte"
    * \ may be in the 2nd of IBM-932 char.
    */
   {
      int _i;
      int _clen;

      _found = 0;
      for( _i = 0; _i < strlen( ret_string ); _i += _clen ) {
          _clen = mblen( &(ret_string[_i]), MB_CUR_MAX );
          /*
	   * If found invalid char, go ahead.
	   */
	  if ( _clen == -1 ) {
	      _clen = 1;
	      continue;
	  }
          if ( _clen == 0 )
             break;
          if ( ( _clen == 1 ) && ( ret_string[_i] == '\\' ) ) {
             _found = 1;
             break;
          }
      } 
   }
   if ( _found )
   {
      while (*ch != '\0') 
      {
#ifdef NLS16
	 if (!is_multibyte)
#endif
	    len = 1;
#ifdef NLS16
	 else
	    len = mblen (ch, MB_CUR_MAX);
#endif
	 /*
	  * We have to take care of the case when mblen() returns -1
	  */
	 if ( (len == 1) || ( len == -1 ) )
	 {
	    if (use_in_memory_db || *ch != '\\') 
	    {
	       ret_string[j++] = *ch;
	       ch++;
	    }
	    else 
	    {
	       /* Get next char. */
	       ch++;
#ifdef NLS16
	       if (!is_multibyte)
#endif
		  len = 1;
#ifdef NLS16
	       else
		  len = mblen (ch, MB_CUR_MAX);
#endif
	       /*
		* We have to take care of the case when mblen() returns -1
		*/
	       if ( (len == 1) || ( len == -1 ) )
	       {
		  ret_string[j++] = *ch;
		  /* Skip this char., unless its the terminater */
		  if (*ch != '\0')
		     ch++;
	       }
	       else 
	       { 
		  for (i=0; i < len; i++, ch++)
		     ret_string[j++] = *ch;
	       }
	    }
	 }
	 else 
	 { 
	    for (i=0; i < len; i++, ch++)
	    ret_string[j++] = *ch;
	 }
      }
      ret_string[j] = '\0';
   }

   /*
    * Now remove whitespace from the end of the line.
    *
    */
   tail = head = ret_string;

#ifdef NLS16
   /*
    * If dealing with single byte characters only first
    * check the last character and if it is not a space
    * (normally the case) then return.
    */
   if (_DtNl_is_multibyte == False)
#endif /* NLS16 */
   {
      i = strlen (ret_string) - 1;
      while (i >= 0 && isspace((unsigned char)ret_string[i]))
	 ret_string[i--] = '\000';
      return;
   }

   /*
    * Need to parse the mulit-byte character line.
    */
   while (*head != '\0') 
   {
#ifdef NLS16
      if (is_multibyte && ((len = mblen (head, MB_CUR_MAX)) > 1))
      {
	 for (i=0; i < len; i++, head++);
	 tail = head;
      }
      else
#endif /* NLS16 */
      {
	 /* This is a bugfix for AIX. Need to return true in case of one-byte
	  * Katakana code.
	  */
	 if (((isgraph (*head)) || (iscntrl (*head))) && (!isspace ((unsigned
char)*head)))
	    tail = head;
	 head++;
      }
   }

   if (*tail != '\0') 
   {
      tail++;
      *tail = '\0';
   }
}
/*********************
 *
 * Function Name:  _DtDbFillVariables
 *
 * Description:
 *
 *	This function scans a string for variable definitions
 *      and if found, gets the variable substitued for the
 *      actual value.
 *
 * Synopsis:
 *
 *	(void) = _DtDbFillVariables (line)
 *
 *	char **line;  	The string to scan and replace.
 *
 *      MODIFIED:	line
 *
 ***********************/

void
_DtDbFillVariables (
	char **line )
{
   int i;
   int len;
   int lineLen;
   int lineSize;
   char bracketSeen;
   int	brackets = FALSE;
   char *start;
   char *variableStart;
   char *variableEnd;
   char *replacementStart;
   char *replacementEnd;
   int currentOffset;
   Boolean escape = False;

   start = *line;
   lineLen = strlen (*line)+1;
   lineSize = MAX_LINE_LENGTH;

   while (*start != '\0') 
   {
      escape = False;
      /* Determine the size of this character */
#ifdef NLS16
      if (!is_multibyte)
#endif
	 len = 1;
#ifdef NLS16
      else
	 len = mblen(start, MB_CUR_MAX);
#endif

      if (len > 1) 
      {
	 /* Move passed the multi-byte char. */
	 for (i = 0; (i < len) && (*start != '\0'); start++);
	 continue;
      }

      if (*start == '\\')
      {
	 /* Discard the escape character, and use the next one */
	 start++;
	 escape=True;

	 /* Get the size of the new character */
#ifdef NLS16
	 if (!is_multibyte)
	    len = 1;
	 else
	    len = mblen(start, MB_CUR_MAX);
#else
	 len = 1;
#endif
	 if (len > 1) 
	 {
	    /* Move passed the multi-byte char. */
	    for (i = 0; (i < len) && (*start != '\0'); start++);
	    continue;
	 }
      }
      if (*start == '$' && !escape)
      {
	 /* Variable name reference */
	 /* Skip the optional left bracket character */
	 replacementEnd = replacementStart = start;
	 replacementEnd++;
	 if (
#ifdef NLS16
	  (!is_multibyte || (mblen(replacementEnd, MB_CUR_MAX)==1)) &&
#endif
	     (*replacementEnd == '{'))
	 {
	    replacementEnd++;
	    bracketSeen = TRUE;
	 }
	 else
	    bracketSeen = FALSE;

	 /* 
	  * Extract the variable name.
	  * Composed of alphanumeric and _ 
	  */
	 variableStart = replacementEnd;
#ifdef NLS16
	 if (!is_multibyte)
#endif
	 {
	   while (isalnum(*replacementEnd) || (*replacementEnd == '_'))
	       replacementEnd++;
	 }
#ifdef NLS16
	 else
	 {
	    while (1)
	    {
	       len = mblen(replacementEnd, MB_CUR_MAX);
	       if ((len == 0) || 
		   ((len == 1) && !isalnum(*replacementEnd) &&
		    (*replacementEnd != '_')))
	       {
		  break;
	       }
	       replacementEnd += len;
	    }
	 }
#endif
	 variableEnd = replacementEnd;

	 /* 
	  * Strip optional right bracket, if left bracket was 
	  * seen earlier.
	  */
	 if (bracketSeen && 
#ifdef NLS16
	  (!is_multibyte || (mblen(replacementEnd, MB_CUR_MAX)==1)) &&
#endif
	     (*replacementEnd == '}'))
	{
		bracketSeen = FALSE;
		brackets = TRUE;
		replacementEnd ++;
	}

	if(!bracketSeen)
	{
	 /* Replace with the variable definition */
	 currentOffset = start - *line;
	 ResolveVariableReference(variableStart,
				  variableEnd - variableStart,
				  line,
				  &lineSize,
				  currentOffset,
				  replacementEnd - replacementStart,
				  &lineLen,
				  escape,
				  brackets);
	 /* Resync, in case entry_line was realloc'ed */
	 start = *line + currentOffset;
	 brackets = FALSE;
	}
	 if(*start != 0)
		start++;
      }
      else if (*start != '\0')
	 start++;
   }
   if(!use_in_memory_db)
   {
	clean_line(line);
   }
}

/*********************
 *
 * Function Name:  ReadNextEntry
 *
 * Description:
 *
 *	This function reads one definition out of the specified file.
 *	It returns the definition as an array of keyword/value strings. 
 *      This function is responsible for handling syntactic things like string 
 *      variables and comments.
 *
 ***********************/
   
#define AT_START 0              /* 0 = at start,     '{' symbol not found */
#define NO_START_SYMBOL 1       /* 1 = not at start, '{' symbol not found */
#define START_SYMBOL 2          /* 2 = not at start, '{' symbol found     */

static int 
ReadNextEntry(
        Boolean * versionCanBeSet,
        FILE *fd,
        DtDtsDbField ** fieldPtr,
        DtDbRecordDesc * recordDescriptions,
	int numRecordDescriptions,
        int * returnRecordIndex,
	char *file_name,
	int *NumberFieldsAllocated,
	char **tmpBuffer,
	char **tmpLine) 
{
   int i;
   int len;
   int indx = 0;
   Boolean multi_line = False;	/* Previous line. */
   Boolean line_cont = False;	/* Current line. */
   Boolean remove_defn = False; /* The definition has an error, free it. */
   char *start;
   char *last;
   char *save;
   char *error_buffer;
   short startDef;
   char * errorName;
   char * version;
   int variableSetSize = 0;

   while (1) 
   {
      (*tmpLine)[0] = '\000';

      remove_defn = False;
      multi_line = False;
      start = NULL;
      indx = 0;

      if (read_line (*tmpLine, MAX_LINE_LENGTH, fd) == NULL) 
      { 
	 /* EOF */
	 break;
      }

      /* Strip off leading white space */
      start = *tmpLine;
      while (
#ifdef NLS16
             (!is_multibyte || (mblen(start, MB_CUR_MAX) == 1)) &&
#endif
             isspace((unsigned char)*start))
      {
         start++;
      }

      /* Skip empty lines or comment lines */
      if ((*start == '\0') || 
          (
#ifdef NLS16
           (!is_multibyte || (mblen(start, MB_CUR_MAX) == 1)) &&
#endif
          (*start == '#')))
         continue;

      /* Check for a variable definition */
      if ((strncmp(start, "set ", 4) == 0) &&
          (strlen(start) >= 4))
      {
         if (!DefineVariable(versionCanBeSet, start + 4, file_name, &variableSetSize))
         {
            /*
             * An attempt was made to redefine the Db version; this
             * is only allowed as the first keyword in a file.  We
             * will ignore this file.  An error msg was already reported.
             */
            return(False);
         }

         /* 
          * Once we've read something other than a comment, then the version 
          * can no longer be changed.  We also need to verify that the 
          * specified version string is valid.
          */
         if (*versionCanBeSet)
         {
            *versionCanBeSet = False;
            version = XtNewString(VERSION_QUERY_STR);
	    if (strchr (version, '$'))
	       _DtDbFillVariables(&version);
            if ((strlen(version) > 0) && (strcmp(version, VERSION_ID))) 
            { 
               /* Bad version number; give error, and ignore this file */
	       error_buffer = XtMalloc(MAXPATHLEN);
               sprintf(error_buffer, invalidVersion, file_name);
               _DtSimpleError(DtProgName, DtError, NULL, "%s", error_buffer);
               XtFree(version);
	       if (error_buffer) XtFree(error_buffer);
               return(False);
            }

            XtFree(version);
         }
         continue;
      }

      /* Do here also, in case we did not encounter any "set" commands */
      *versionCanBeSet = False;

      if ((*returnRecordIndex = MatchKeyword(start, recordDescriptions,
                                             numRecordDescriptions)) < 0)
      {
	 /* Skip this definition */
	 continue;
      }

      /* 
       * At this point, the beginning of a definition has been recognized.
       * Must now get the data for the fields.
       */
      clean_line (tmpLine);
      if (strchr (*tmpLine, '$'))
	 _DtDbFillVariables (tmpLine);

      /*
       * Before spliting tmpLine into the record name and value,
       * remove leading whitespace.  Variable 'start' may not
       * be valid because tmpLine could have been realloc'ed
       * by _DtDbFillVariables.
       */
      start = *tmpLine;
      while (
#ifdef NLS16
             (!is_multibyte || (mblen(start, MB_CUR_MAX) == 1)) &&
#endif
             isspace((unsigned char)*start))
      {
         start++;
      }

      SplitField(fieldPtr, &indx, start, NumberFieldsAllocated);

      line_cont = False;   	/* True if the current line has a continuation 
				 * character, False otherwise. */

      multi_line = False;  	/* True if the previous line was continued.  
				 * This means the current line must be 
				 * appended to the previous line. */
      startDef = AT_START;
                    /* AT_START =            at start, '{' symbol not found */
                    /* NO_START_SYMBOL = not at start, '{' symbol not found */
                    /* START_SYMBOL =    not at start, '{' symbol found     */

      (*tmpBuffer)[0] = '\000';

      while (1) 
      {
	 (*tmpLine)[0] = '\000';

	 if (read_line (*tmpLine, MAX_LINE_LENGTH, fd) == NULL) /* EOF */ 
	 { 
	    if (multi_line || (indx >=1)) 
	    {
	       error_buffer = XtMalloc(MAXPATHLEN);
               errorName = (*fieldPtr)[0].fieldValue ? 
                      (*fieldPtr)[0].fieldValue : 
		      XrmQuarkToString((*fieldPtr)[0].fieldName);
	       (void) sprintf (error_buffer, incompleteDefn, errorName, 
			       file_name);
	       _DtSimpleError (DtProgName, DtError, NULL, "%s", error_buffer);
	       if (error_buffer) XtFree(error_buffer);
	       remove_defn = True;
	    }
	    break;
	 }

	 /* Fill varibles now. */
         if (strchr (*tmpLine, '$'))
	    _DtDbFillVariables (tmpLine);

	 /* Strip off leading white space */
	 start = *tmpLine;
	 while (
#ifdef NLS16
		(!is_multibyte || (mblen(start, MB_CUR_MAX) == 1)) &&
#endif
		isspace((unsigned char)*start))
	 {
	    start++;
	 }

	 /* Skip empty lines or comment lines */
	 if ((*start == '\0') || 
	     (
#ifdef NLS16
	      (!is_multibyte || (mblen(start, MB_CUR_MAX) == 1)) &&
#endif
	     (*start == '#')))
	    continue;

	 /* Check for a variable definition */
	 if ((strncmp(start, "set ", 4) == 0) &&
	     (strlen(start) >= 4))
	 {
	    if (!DefineVariable(versionCanBeSet, start + 4, file_name, &variableSetSize))
            {
               /*
                * An attempt was made to redefine the Db version; this
                * is only allowed as the first keyword in a file.  We
                * will ignore this file.  An error msg was already reported.
                */
               remove_defn = True;
               break;
            }
	    continue;
	 }

	 /* Check for the start of a definition ("{") */
	 if ( strncmp(start, "{", 1) == 0 )
	 {
	    /* if '{' found in the middle of definition, ignore defn */
	    if ( startDef != AT_START  )
	    {
	       error_buffer = XtMalloc(MAXPATHLEN);
               errorName = (*fieldPtr)[0].fieldValue ? 
                      (*fieldPtr)[0].fieldValue : 
		      XrmQuarkToString ((*fieldPtr)[0].fieldName);
	       (void) sprintf (error_buffer, invalidStartSymbol, errorName, 
			       file_name);
	       _DtSimpleError (DtProgName, DtError, NULL, "%s", error_buffer);
	       if (error_buffer) XtFree(error_buffer);
               remove_defn = True;
               break;
	    }

	    startDef = START_SYMBOL;     /* not at start, '{' symbol found  */
	    continue;
	 }

	 /* we are no longer at definition start, note if start symbol found */
	 if ( startDef == AT_START ) 
	 {
	    if (error_buffer) XtMalloc(MAXPATHLEN);
            errorName = (*fieldPtr)[0].fieldValue ? 
                   (*fieldPtr)[0].fieldValue : 
		   XrmQuarkToString ((*fieldPtr)[0].fieldName);
	    (void) sprintf (error_buffer, missingStartSymbol, errorName, 
	                    file_name);
	    _DtSimpleError (DtProgName, DtError, NULL, "%s", error_buffer);
	    if (error_buffer) XtFree(error_buffer);
            remove_defn = True;
            break;
	 }

	 /* Check for the end of a definition ("}") */
	 if ( strncmp(start, "}", 1) == 0 )
	 {
            if (multi_line) 
	    {
	       error_buffer = XtMalloc(MAXPATHLEN);
               errorName = (*fieldPtr)[0].fieldValue ? 
                      (*fieldPtr)[0].fieldValue : 
		      XrmQuarkToString ((*fieldPtr)[0].fieldName);
	       (void) sprintf (error_buffer, multiLineDefn, errorName, 
			       file_name);
	       _DtSimpleError (DtProgName, DtError, NULL, "%s", error_buffer);
	       if (error_buffer) XtFree(error_buffer);
	       remove_defn = True;
	    }
	    /* if '}' end found with no starting '{', ignore defn */
	    if ( strncmp(start, "}", 1) == 0 && startDef != START_SYMBOL )
	    {
	       error_buffer = XtMalloc(MAXPATHLEN);
               errorName = (*fieldPtr)[0].fieldValue ? 
                      (*fieldPtr)[0].fieldValue :
		      XrmQuarkToString ((*fieldPtr)[0].fieldName);
	       (void) sprintf (error_buffer, noStartSymbol, errorName, 
			       file_name);
	       _DtSimpleError (DtProgName, DtError, NULL, "%s", error_buffer);
	       if (error_buffer) XtFree(error_buffer);
	       remove_defn = True;
	    }
	    break;
	 }


	 /* 
	  * At this point, "line" contains data for a field.  If "indx" 
	  * is already equal to "num_fields - 1" (the last entry is a 
	  * NULL), then something has gone wrong with this definition - 
	  * like a field is missing the continuation mark.
	  */
         if ((recordDescriptions[*returnRecordIndex].maxFields != 
                DTUNLIMITEDFIELDS) &&
             (indx > recordDescriptions[*returnRecordIndex].maxFields))
	 {
	    error_buffer = XtMalloc(MAXPATHLEN);
            errorName = (*fieldPtr)[0].fieldValue ? 
                   (*fieldPtr)[0].fieldValue :
		   XrmQuarkToString ((*fieldPtr)[0].fieldName);
            (void)sprintf (error_buffer, tooManyFields, errorName, file_name);
            _DtSimpleError (DtProgName, DtError, NULL, "%s", error_buffer);
	    if (error_buffer) XtFree(error_buffer);
	    remove_defn = True;
	    break;
	 }

         line_cont = False;
	 save = start;

	 /* Need to see if this line is part of a multi_line field. */
	 if ((last = DtStrrchr (start, '\\')) != NULL) 
	 {
	    /* 
	     * Need to see if 'last' is the last non-whitespace char. 
	     * Strip off leading white space.
	     */
	    start = last + 1; /* Move passed the slash. */
	    while (
#ifdef NLS16
		   (!is_multibyte || (mblen(start, MB_CUR_MAX) == 1)) &&
#endif
		   isspace((unsigned char)*start))
	    {
	       start++;
	    }
	    if (*start == '\0') 
	    {
	       /* 
		* Only whitespace followed the slash, so remove it
		* unless the slash was escaped.
		*/
               if (
#ifdef NLS16
                   !_is_previous_single(save, last) ||
#endif
                   *(last-1) != '\\')
	       {
		  *last = '\0';  /* Replace the slash with EOLine */
		  line_cont = True;
	       }
	    }
	 }

         start = save;
	 if (multi_line) 
	 {
	    /*
	     * This is a multi-line field - the previous line was continued 
	     * so append the current line to the previous line "tmpBuffer".
	     */
	    int tmp_len = strlen (*tmpBuffer) + strlen (start) + 2;
	    if (tmp_len >= MAX_LINE_LENGTH)
	       *tmpBuffer = (char *) XtRealloc ((char *) *tmpBuffer, tmp_len); 
	    (void) strcat (*tmpBuffer, start);

	    if (!line_cont) 
	    {
	       /* 
		* This line is not continued so the field is complete -
		* save the data.
		*/
	       clean_line (tmpBuffer);
               SplitField(fieldPtr, &indx, *tmpBuffer, NumberFieldsAllocated);
	       multi_line = False;
	    }
	 }
	 else 
	 {
	    /*
	     * Previous line was not continued.
	     */
	    if (line_cont) 
	    {
	       /*
		* This line has a continuation mark so save it in "tmpBuffer".
		*/
	       int tmp_len = strlen (start) + 2;

	       if (tmp_len >= MAX_LINE_LENGTH)
		  *tmpBuffer = (char *) XtRealloc ((char *) *tmpBuffer, tmp_len); 
	       (void) strcpy (*tmpBuffer, start);
	       multi_line = True;
	    }
	    else 
	    {
	       /*
		* This field is a single-line field.  Save it.
		*/
	       clean_line (&start);
               SplitField(fieldPtr, &indx, start, NumberFieldsAllocated);
	    }
	 }
      }

      if (remove_defn) 
      {
	 for (i = 0; i < indx; i++) 
         {
	    if ((*fieldPtr)[i].fieldValue) 
	       XtFree ((char *) (*fieldPtr)[i].fieldValue);
         }
      }
      else 
      {
         /* Add terminating NULL to fieldPtr array */
         SplitField(fieldPtr, &indx, NULL, NumberFieldsAllocated);

	 return (TRUE);
      }
   }
   return(FALSE);
}

/*********************
 *
 * Function Name:  ResolveVariableReference
 *
 * Description:
 *
 *	This function takes a variable name, and searches the list of
 *      defined variables for a match.  If a match is found, then the
 *      associated value is inserted into the buffer; otherwise, nothing
 *      is inserted into the buffer.  The buffer will be grown, if needed.
 *
 *      The buffer being worked with here is the buffer containing the
 *      raw input line.  When a variable replacement occurs, the
 *      variable reference (${name}) will be removed, and replaced with
 *      the value for the variable reference.  This may require us to
 *      grow the input buffer.  We also may have to shift up or down any
 *      data in the buffer which is after this variable reference.
 *
 *      If the variable name is not found, the environment variables are
 *      searched and the same replacement mechanism will occurr.
 *
 * Synopsis:
 *
 *	ResolveVariableReference (varName, varNameLen, buf, buf_size, 
 *                                start, len, buf_len);
 *
 *      char * varName;         Name of variable to search for.
 *      int    varNameLen;      Length of the variable name
 *	char **buf;		Pointer to a buffer, into which the value
 *				will be placced.
 *	int * buf_size;		Pointer to size of buffer.
 *      int   start;            Where in buffer to place value
 *      int   len;              Size of area to replace
 *	int * buf_len;	        Pointer to # of chars in the buf.
 *
 ***********************/

static void 
ResolveVariableReference(
        char *varName,
        int varNameLen,
        char **buf,
        int *buf_size,
        int start,
        int len,
        int *buf_len,
	int escape,
	int brackets )
{
   int j;
   int valueLen;
   int delta;
   char *value;
   char *var;

   var = (char *) XtMalloc(varNameLen+5);
   *var = '\0';
   if ((get_variable (varName, varNameLen, &value, escape, brackets, var)))
   {
      valueLen = (int)strlen(value);

      /*
       * The way in which we do our work will be dependent upon whether
       * the replacement value is longer, shorter or equal in size to the
       * variable reference.
       */
      if (valueLen == len)
      {
         /* Same len; simply overwrite with the new value */
         (void)strncpy(*buf + start, value, (size_t)valueLen);
      }
      else if (valueLen < len)
      {
         /*
          * The replacement value is shorter than the reference currently
          * in the input line; write in the replacement value, then shift
          * down the remaining contents of the buffer, overwriting the
          * leftover portion of the variable reference.
          */
         (void)strncpy(*buf + start, value, (size_t)valueLen);
         (void)strcpy(*buf + start + valueLen, *buf + start + len);
         *buf_len -= (len - valueLen);
      }
      else
      {
         /*
          * The replacement value is longer than the variable reference;
          * we may need to grow the input line.  In any case, before we
          * can copy in the replacement value, we need to shift the
          * remainder of the input line up, so that we will no overwrite
          * it.
          */
         delta = valueLen - len;
         if (*buf_len + delta >= *buf_size)
         {
            /* Grow the buffer */
            *buf_size += delta + 100;
            *buf = (char *)XtRealloc((void *)*buf, (size_t)*buf_size+1);
         }

         /* Shift the remainder of the input line */
         for (j = *buf_len - 1; j >= start; j--)
            (*buf)[j + delta] = (*buf)[j];

         (void)strncpy(*buf + start, value, (size_t)valueLen);
         *buf_len += delta;
      }
      XtFree((char *) var);
      return;
   }

   /* If no match is found, then replace the reference with nothing */
   (void)strcpy(*buf + start, *buf + start + len);
   *buf_len -= len;
   XtFree((char *) var);
}


/*********************
 *
 * Function Name:  ClearVariables
 *
 * Description:
 *
 *	This function clears out the structure containing all of the
 *      currently defined variables.
 *
 * Synopsis:
 *
 *	ClearVariables ();
 *
 ***********************/

static void 
ClearVariables( void )
{
   int i;

   _DtSvcProcessLock();
   for (i = 0; i < variableCount; i++)
   {
      free((void *)variableSet[i].varName);
      free((void *)variableSet[i].value);
   }

   variableCount = 0;
   _DtSvcProcessUnlock();
}

/*********************
 *
 * Function Name:  DefineVariable
 *
 * Description:
 *
 *	This function takes a string of format "name = value", and
 *      parses out the name and value fields, and adds them to the
 *      list of defined variables.  If 'name' already has a definiation,
 *      then it will be replaces.  Any bogus entries are ignored.
 *
 *      If an attempt is made to change the database version variable
 *      when it is not allowed, 'False' will be returned; in all other
 *      cases 'True' is returned.
 *
 * Synopsis:
 *
 *	DefineVariable (versionCanBeSet, varString, fileName);
 *
 *      char * varString;       Variable definition string
 *
 ***********************/

static Boolean 
DefineVariable(
        Boolean * versionCanBeSet,
        char *varString,
        char * fileName,
	int *variableSetSize)
{
   char * nameStart;
   char * valStart;
   int i;
   int len;
   char *errorBuffer;

   /* Find the start of the variable name */
   nameStart = varString;
   while (
#ifdef NLS16
          (!is_multibyte || (mblen(nameStart, MB_CUR_MAX) == 1)) &&
#endif
          isspace((unsigned char)*nameStart))
   {
      nameStart++;
   }

   /* Ignore lines of format: "set      " */
   if (*nameStart == '\0')
      return(True);

   /* 
    * Find the end of the variable name.
    * Variable names are composed of alphanumeric characters, and '_'.
    */
   valStart = nameStart;
   while (1)
   {
#ifdef NLS16
      if (!is_multibyte)
#endif
         len = 1;
#ifdef NLS16
      else
         len = mblen(valStart, MB_CUR_MAX);
#endif

      if ((len == 0) ||
          ((len == 1) && 
           ((*valStart == '=') || (!isalnum(*valStart) &&(*valStart != '_')))))
      {
         break;
      }

      valStart += len;
   }

   /* 
    * Skip the '=' and any preceding whitespace.  We would have only
    * broken from the above loop because of a single byte character,
    * so we don't need to check the character length here.
    */
   if (*valStart == '=')
   {
      *valStart = '\0';
      valStart++;
   }
   else
   {
      *valStart = '\0';
      valStart++;

      /* Skip leading spaces */
      while (
#ifdef NLS16
             (!is_multibyte || (mblen(valStart, MB_CUR_MAX) == 1)) &&
#endif
             isspace((unsigned char)*valStart))
      {
         valStart++;
      }

      /* Bogus entry if next character is not a '=' */
#ifdef NLS16
      if (!is_multibyte)
#endif
         len = 1;
#ifdef NLS16
      else
         len = mblen(valStart, MB_CUR_MAX);
#endif

      if (((len == 1) && (*valStart != '=')) || (len != 1))
         return(True);

      /* Skip the '=' */
      valStart++;
   }

   /* 
    * Don't allow the version to be changed, if the 'versionCanBeSet'
    * flag disallows it.
    */
   if (strcmp(VERSION_KEYWORD, nameStart) == 0)
   {
      if (!*versionCanBeSet)
      {
         /* Log an error, and ignore the definition */
	 errorBuffer = XtMalloc(MAXPATHLEN);
         sprintf(errorBuffer, cantSetVersion, fileName);
         _DtSimpleError(DtProgName, DtError, NULL, "%s", errorBuffer);
	 if (errorBuffer) XtFree(errorBuffer);
         return(False);
      }
   }

   /* Find the definition string; skip leading spaces */
   while (
#ifdef NLS16
          (!is_multibyte || (mblen(valStart, MB_CUR_MAX) == 1)) &&
#endif
          isspace((unsigned char)*valStart))
   {
      valStart++;
   }

   /* If a definition already exists for this variable, replace it */
   _DtSvcProcessLock();
   for (i = 0; i < variableCount; i++)
   {
      if (strcmp(nameStart, variableSet[i].varName))
         continue;

      free((void *)variableSet[i].value);
      variableSet[i].value = malloc((size_t)(strlen(valStart) + 1));
      (void)strcpy(variableSet[i].value, valStart);
      _DtSvcProcessUnlock();
      return(True);
   }

   /* New entry; allocate space, if needed */
   if (variableCount >= *variableSetSize)
   {
      *variableSetSize += 10;
      if (variableSet == NULL)
      {
         variableSet = (VariableEntry *) 
                      malloc(sizeof(VariableEntry) * (*variableSetSize));
      }
      else
      {
         variableSet = (VariableEntry *) realloc((void *)variableSet, 
                      (size_t)(sizeof(VariableEntry) * (*variableSetSize)));
      }
   }

   variableSet[variableCount].varName = 
            (char *)malloc((size_t)strlen(nameStart) + 1);
   (void)strcpy(variableSet[variableCount].varName, nameStart);
   variableSet[variableCount].value = 
            (char *)malloc((size_t)strlen(valStart) + 1);
   (void)strcpy(variableSet[variableCount].value, valStart);
   variableCount++;
   _DtSvcProcessUnlock();
   return(True);
}


/*
 * Given a field string, made up of a keyword and a value, split them into
 * the two pieces, and store into the indicated structure array.  The space 
 * in the array will be allocated here, as will copies of the strings.
 */
static void 
SplitField(
        DtDtsDbField ** fields,
        int * slotToUse,
        char * ptr,
	int *NumberFieldsAllocated)
{
   char * nextChar;
   char savedChar;

   if (*slotToUse >= *NumberFieldsAllocated) 
   {
      *fields = (DtDtsDbField *)XtRealloc((char *)*fields,
					  sizeof(DtDtsDbField) * 
					  (*NumberFieldsAllocated +
					  NUMBER_FIELDS_INCREMENT));
      *NumberFieldsAllocated += NUMBER_FIELDS_INCREMENT;
   }

   (*fields)[*slotToUse].fieldName = 0;
   (*fields)[*slotToUse].fieldValue = NULL;

   /* A NULL ptr is how we force a NULL entry to terminate the array */
   nextChar = ptr;
   if (ptr != NULL)
   {
      /* Find the end of the fieldName ('\0' or whitespace) */
      while (*nextChar)
      {
         if (
#ifdef NLS16
             (!is_multibyte || (mblen(nextChar, MB_CUR_MAX) == 1)) &&
#endif
             isspace((unsigned char)*nextChar))
         {
            break;
         }
         nextChar++;
      }

      /* Save a copy of the field name */
      savedChar = *nextChar;
      *nextChar = '\0';
      (*fields)[*slotToUse].fieldName = XrmStringToQuark (ptr);
      *nextChar = savedChar;

      /* 
       * The remainder of the string is the field value, with leading
       * whitespace stripped out.
       */
      while (*nextChar)
      {
         if (
#ifdef NLS16
             (!is_multibyte || (mblen(nextChar, MB_CUR_MAX) == 1)) &&
#endif
             isspace((unsigned char)*nextChar))
         {
            nextChar++;
         }
         else
            break;
      }

      if (*nextChar)
      {
         (*fields)[*slotToUse].fieldValue = XtMalloc(strlen(nextChar) + 1);
         strcpy((*fields)[*slotToUse].fieldValue, nextChar);
      }
   }

   (*slotToUse)++;
}


/*
 * This function will take a line read from a database file (with leading
 * white space stripped off), and will attempt to match the keyword portion
 * of it (i.e. the first block of characters terminated by a white space
 * or a NULL) against the set of valid keywords specified in the array of
 * recordDescriptions.  The index of the matching record is returned, or
 * -1, if no match is found.
 */
static int 
MatchKeyword(
        char * line,
        DtDbRecordDesc * recordDescriptions,
        int numRecordDescriptions )
{
   char * keyword;
   char * nextChar;
   int i;

   for (i = 0; i < numRecordDescriptions; i++)
   {
      keyword = recordDescriptions[i].recordKeyword;
      if (keyword && (strncmp(line, keyword, strlen(keyword)) == 0))
      {
         /* 
          * A preliminary match was found.  Now, verify that the keywords
          * match completely, by making sure that the next character in
          * the line is either a white space or a NULL; i.e. "foo" and
          * "foobar" would pass the preliminary test above, but would fail
          * the second half of the test.
          */
         nextChar = line + strlen(keyword);
         if ((*nextChar == '\0') || (
#ifdef NLS16
                (!is_multibyte || (mblen(nextChar, MB_CUR_MAX) == 1)) &&
#endif
              isspace((unsigned char)*nextChar)))
         {
            /* A match! */
            return (i);
         }
      }
   }
 
   return (-1);
}

/*
 * This function frees up all of the entries for a db record, including the
 * array holding the field information.  The array is terminated by an
 * entry with both fieldName and fieldValue set to NULL.
 */
static void 
FreeDbField(
        DtDtsDbField * fields )
{
   int i = 0;

   while (fields[i].fieldName || fields[i].fieldValue)
   {
      if (fields[i].fieldValue)
         XtFree(fields[i].fieldValue);

      i++;
   }
}


/*********************
 *
 * Function Name:  InitializeLocalizedStrings
 *
 * Description: Initializes the localized strings.
 *
 * Modified:
 *
 *  char * incompleteDefn;		- Set to the default value.
 *
 *  char * multiLineDefn;		- Set to the default value.
 *
 *  char * tooManyFields;		- Set to the default value.
 *
 *  char * emptyTypesDirs		- Set to the default value.
 *
 *  char * noStartSymbol;               - Set to the default value.
 *
 *  char * invalidStartSymbol;          - Set to the default value.
 *
 *  char * cantSetVersion;              - Set to the default value.
 *
 *  char * invalidVersion;              - Set to the default value.
 *
 ***********************/

static
void
InitializeLocalizedStrings ( void )
{
   incompleteDefn = strdup (((char *) Dt11GETMESSAGE (4, 8, "The definition \"%s\" in the file\n  \"%s\"\n  is incomplete. The definition may be missing\n  the \"}\" field.\n")));
   
   multiLineDefn = strdup (((char *) Dt11GETMESSAGE (4, 3, "A multi-line field in the definition \"%s\"\n  in the file\n  \"%s\"\n  is incomplete.  A \"\\\" character may be missing\n  in the multi-line field.\n")));

   tooManyFields = strdup (((char *) Dt11GETMESSAGE (4, 4, "The definition \"%s\" in the file\n  \"%s\"\n  has too many fields.\n")));

   emptyTypesDirs = strdup (((char *) Dt11GETMESSAGE (4, 5, "An attempt to read the action and filetypes databases failed.\n  This may be caused by the resource \"Dt.TypesDirs\" being incorrectly set.\n")));

   noStartSymbol = strdup (((char *) Dt11GETMESSAGE (4, 6, "The definition \"%s\" in the file\n  \"%s\"\n  has an end symbol '}' with no start symbol '{'.\n")));

   invalidStartSymbol = strdup (((char *) Dt11GETMESSAGE (4, 7, "The definition \"%s\" in the file\n  \"%s\"\n  has an unexpected start symbol '{'.\n")));

   missingStartSymbol = strdup (((char *) Dt11GETMESSAGE (4, 9, "The definition \"%s\" in the file\n  \"%s\"\n  is missing a start symbol '{'.\n")));

   cantSetVersion = strdup (((char *) Dt11GETMESSAGE (4, 10, "The DtDbVersion variable can only be set\nat the beginning of the file.  The remainder of the file\n'%s' is being ignored.\n")));

   invalidVersion = strdup (((char *) Dt11GETMESSAGE (4, 11, "The file '%s'\ncontains an invalid DtDbVersion identifier.\n")));

}
