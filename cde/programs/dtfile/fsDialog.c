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
/* $XConsortium: fsDialog.c /main/6 1996/10/08 11:58:08 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           fsDialog.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Containes routines to handle contents of dtfile.config
 *
 *   FUNCTIONS: configFileName
 *		configFileOK
 *		fsDialogAvailable
 *		getFSType
 *		readConfigFile
 *		readDialogData
 *		readFSID
 *		readLine
 *		stricmp
 *		strip
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

/*---------------------------------------------------------------------------------
 *
 *   fsDialogAvailable (String path, dtFSData *fsDialogData)
 *
 *   For the file specified by path, determine if there is a properties
 *      dialog specific to the file system that the file is on.
 *      For example, if the file is in AFS there might be a dialog to edit
 *      the Access Control List. fsDialogData is filled if there is a dialog.
 *   path must not include host:
 *
 *   If any errors are encountered, FALSE is returned and no other action is taken
 *      or message displayed.
 *
 *--------------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

#include <Dt/UserMsg.h>   /* for DtSimpleError */

#include <Xm/Xm.h>
#include "Encaps.h"
#include "ModAttr.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"

/*-----------------------------------------------------
 * Configuration file search order is:
 *
 *       $DTFSCONFIG   (meant to be used for debugging)
 *       $HOME/CONFIGFILENAME
 *       SYSCONFIGFILEDIR/CONFIGFILENAME
 *       DEFCONFIGFILEDIR/$LANG/CONFIGFILENAME
 *       DEFCONFIGFILEDIR/C/CONFIGFILENAME
 *
 * The function configFileName() determines the correct
 *    name. It will apply $LANG where appropriate.
 *
 *-----------------------------------------------------*/

#define SYSCONFIGFILEDIR  CDE_CONFIGURATION_TOP "/config"
#define DEFCONFIGFILEDIR  CDE_INSTALLATION_TOP  "/config"
#define CONFIGFILENAME "dtfile.config"
#define DEFCONFIGFILENAME  CDE_INSTALLATION_TOP  "/config/C/dtfile.config"

/*--------------------------------------------------------
     Platform-specific includes required for getFSType()
 *--------------------------------------------------------*/

#ifdef __aix
#endif

#if defined(sun)
#include <sys/types.h>
#include <sys/statvfs.h>
#endif

#ifdef __hpux
#include <sys/vfs.h>
#endif

/*----------------------
 *   global variables
 *----------------------*/

#define EMSGMAX 1024
char g_errorMessage[EMSGMAX];


/*--------------------
     Local functions
 *--------------------*/

static void    getFSType      (const String      path,
                                     String      fsType,
                                     String      platform);
static void    readConfigFile (const String      fsType,
                               const String      platform,
                                     dtFSData  * fsDialogData,
                                     Boolean   * dialogAvailable);
static String  configFileName (void);
static void    readFSID       (FILE          * fptr,
                               const String    fsType,
                               const String    platform,
                               String          fsID);
static void    readDialogData (FILE          * fptr,
                               const String    fsID,
                               Boolean       * dialogAvailable,
                               dtFSData      * fsDialogData);
static int     readLine        (FILE  * fptr, String line);
static int     stricmp         (const String  s1,  const String  s2);
static String  strip           (String  s);
static Boolean configFileOK(void);


/*----------------------
 *   fsDialogAvailable
 *----------------------*/

Boolean
fsDialogAvailable (const String path, dtFSData *fsDialogData)

{
   char     fsType[MAXLINELENGTH], platform[MAXLINELENGTH];
   Boolean  dialogAvailable;

   DPRINTF(("fsDialogAvailble: checking path \"%s\"\n",path));
   if ( ! configFileOK())
      return FALSE;

   getFSType(path, fsType, platform);
   DPRINTF(("   fsType=\"%s\"\n   platform=\"%s\"\n",fsType,platform));

   if (strlen(fsType) == 0)
      return FALSE;
   else
   {
      readConfigFile(fsType, platform, fsDialogData, &dialogAvailable);
      if (dialogAvailable)
      {
         strcpy (fsDialogData->path,path);
      }
      return dialogAvailable;
   }

}


/*--------------------------------------------------------------------------------
 *
 *   configFileOK
 *
 *   is the configuration file accessible?
 *
 *------------------------------------------------------------------------------*/

static Boolean
configFileOK(void)

{
   struct stat  buf;
   int         	err;
   String       fname;
   String       msg1, msg2;

   fname = configFileName();
   err   = stat(fname, &buf);
   DPRINTF(("   config file \"%s\"  stat ret=%i\n",fname,err));

   if (err == 0)
      return TRUE;
   else
   {
      msg1 = GETMESSAGE(21, 22, "Cannot open file manager configuration file: ");
      msg2 = strerror(errno);
      sprintf(g_errorMessage,"%s%s\n   %s\n",msg1,fname,msg2);
      _DtSimpleError (application_name, DtError, NULL, g_errorMessage, NULL);
      return FALSE;
   }

}  /* end configFileOK */


/*--------------------------------------------------------------------------------
 *
 *   getFSType
 *
 *   Do the platform-specific work required to determine what type of file system
 *   the file defined by path is on. Return strings defining the file system type
 *   and the platform. Any errors result in a fsType set to the null string.
 *
 *   Both fsType and platform are allocated by the caller
 *
 *   The strings used in platform and fsType must match those used in
 *      the configuration file.
 *
 *--------------------------------------------------------------------------------*/

static void getFSType(const String  path,
                            String  fsType,
                            String  platform)

{
#ifdef __aix
#define GETFSTYPE
   struct stat  buf;

   strncpy(platform,"aix",MAXLINELENGTH);
   if (lstat(path, &buf) == 0)
      sprintf(fsType,"%i",buf.st_vfstype);
   else
      strncpy(fsType,"",MAXLINELENGTH);
#endif /* __aix */

#ifdef sun
#define GETFSTYPE
   struct statvfs buf;

   strncpy(platform,"sunos",MAXLINELENGTH);
   if (statvfs(path, &buf) == 0)
      strncpy(fsType,buf.f_basetype,MAXLINELENGTH);
   else
      strncpy(fsType,"",MAXLINELENGTH);
#endif /* sun */

#ifdef __hpux
#define GETFSTYPE
   struct statfs buf;

   strncpy(platform,"hpux",MAXLINELENGTH);
   if (statfs(path, &buf) == 0)
      sprintf(fsType,"%li",buf.f_fsid[1]);
   else
      strncpy(fsType,"",MAXLINELENGTH);
#endif /* __hpux */

#ifndef GETFSTYPE
   strncpy(platform,"unknown",MAXLINELENGTH);
   strncpy(fsType,  "",MAXLINELENGTH);
#endif  /* unknown platform */

   return;

}    /* end getFSType */


/*--------------------------------------------------------------------------------
 *
 *   readConfigFile
 *
 *   Given the platform and type of file system, read the configuration file to
 *   determine if there is a file-system specific dialog. If there is, fill
 *   fsDialogData. If there is no file or if the file has no entry for
 *   platform:fsType, dialogAvailable is set to FALSE.
 *
 *   The configuration file consists of two parts. The first is a list of entries
 *   (ending with "end") of the form
 *
 *   platform:file-system-type=file-system-id
 *
 *   where platform is one of the strings from getFSType(), file-system-type is
 *   a (platform-dependent) string identifying the type of file system, and
 *   file-system-id is a string used in the configuration file to identify the
 *   file system. The second part of the file consists of lists of attributes
 *   (in the form name = value, one per line) for each of the file system id's.
 *   Blank lines and lines beginning with * are ignored as is case. A example
 *   is given below.
 *
 *   -------------------------------------------------
 *   * sample dtfs configuration file
 *
 *   aix:4=afs
 *   hpux:8=afs
 *   sunos:nfs=nfs
 *   end
 *
 *   afs:     buttonlabel = Change AFS ACL ...
 *            fsDialog    = modAttrAFS
 *            warning     = File system access may be further restriced by AFS ACLs
 *
 *   nfs:     buttonLabel = Change NFS attributes ...
 *            fsDialog    = <configuration-location>/bin/modNFSAttr
 *   -------------------------------------------------
 *
 *--------------------------------------------------------------------------------*/

static void
readConfigFile(const String      fsType,
               const String      platform,
                     dtFSData  * fsDialogData,
                     Boolean   * dialogAvailable)
{
   char      fsID[MAXLINELENGTH];
   FILE    * fptr;
   String    fname, msg1, msg2;


   *dialogAvailable = FALSE;

   fname = configFileName();
   fptr = fopen(fname,"r");
   if (fptr == NULL)
   {
      msg1 = GETMESSAGE(21, 22, "Cannot open file manager configuration file: ");
      msg2 = strerror(errno);
      sprintf(g_errorMessage,"%s%s\n   %s\n",msg1,fname,msg2);
      _DtSimpleError (application_name, DtError, NULL, g_errorMessage, NULL);
      return;
   }

   readFSID(fptr, fsType, platform, fsID);
   DPRINTF(("   fsID=\"%s\"\n",fsID));

   readDialogData(fptr, fsID, dialogAvailable, fsDialogData);

   /* make sure that a dialog program has been specified or execl will do unfortunate things */
   if (strlen(fsDialogData->fsDialogProgram) == 0)
   {
      *dialogAvailable = FALSE;
      if (strlen(fsDialogData->buttonLabel) != 0)
      {
         msg1 = XtNewString(GETMESSAGE(21, 29, "No value was provided for the fsDialog field in dtfile's configuration file"));
         _DtSimpleError (application_name, DtError, NULL, msg1, NULL);
         XtFree(msg1);
      }
   }

   fclose(fptr);
   return;

}  /* end readConfigFile */


/*--------------------------------------------------------------------------------
 *
 *   configFileName
 *
 *   return the name of the configuration file which defines file-system
 *   specific dialogs ... a hierarchy of names is searched until a file
 *   is found ... no checking is done to see if it can be read
 *
 *------------------------------------------------------------------------------*/

static String
configFileName(void)

{
          int          i, err;
          struct stat  buf;
          String       s;
   static char         fn[MAX_PATH]="";   /* remember name once its been determined */


   if (strlen(fn) != 0) return fn;   /* has filename been determined already? */

   for (i=0; i < 4; i++)
   {
      switch (i)
      {
         case 0: /* file name $DTFSCONFIG */
            if ( (s=getenv("DTFSCONFIG")) != NULL)  	  /* a convenience for debugging */
               strncpy(fn,s,MAX_PATH);
            else  /* DTFSCONFIG not defined */
               continue;
            break;

         case 1: /* $HOME/dtfile.config */
            if ( (s=getenv("HOME")) != NULL)
            {
               strncpy(fn,s,MAX_PATH);
               strcat (fn,"/");
               strncat(fn,CONFIGFILENAME,MAX_PATH-strlen(fn)-1);
            }
            else  /* $HOME not defined */
               continue;
            break;

         case 2: /* SYSCONFIGFILEDIR, e.g. /etc/dt/config/dtfile.config */
            strncpy(fn,SYSCONFIGFILEDIR,MAX_PATH);
            strcat (fn,"/");
            strncat(fn,CONFIGFILENAME,MAX_PATH-strlen(fn)-1);
            break;

         case 3: /* DEFCONFIGFILEDIR, e.g. /usr/dt/config/C/dtfile.config */
            strncpy(fn,DEFCONFIGFILEDIR,MAX_PATH);
            strcat (fn,"/");
            if ( (s=getenv("LANG")) != NULL)
            {
               strncat(fn,s,MAX_PATH-strlen(fn)-1);
               strcat (fn,"/");
            }
            else
            {
               strncat(fn,"C",MAX_PATH-strlen(fn)-1);
               strcat (fn,"/");
            }
            strncat(fn,CONFIGFILENAME,MAX_PATH-strlen(fn)-1);
            break;
      }  /* end switch */

      err   = stat(fn, &buf);
      DPRINTF(("   config file \"%s\"  stat ret=%i\n",fn,err));

      if (err == 0)  /* file is found */
         return fn;

   }  /* end loop over possible config files */

   /* didn't find any of the files (this should never, ever happen) */
   /* return the name of the factory default (case 3 above) which should always be there */

   strncpy(fn,DEFCONFIGFILENAME,MAX_PATH);
   return fn;


}  /* end configFileName */


/*--------------------------------------------------------------------------------
 *
 *   readFSID
 *
 *   get the file-system identifier from the configuration file
 *
 *   fptr refers to an open configuration file
 *   fsType and platform identify the platfrom (aix, hpux, sunos, etc)
 *      and file-system type (afs, nfs, dfs, etc.)
 *   fsID is filled by matching platform:fsType with with information in the
 *      file and reading the id; the string comparisons are case insensitive
 *      if there is more than one match, the last fsID found will be returned;
 *      if no match is found, fsID is returned as ""
 *   upon conclusion, the file is positioned past the "end" record
 *
 *------------------------------------------------------------------------------*/

static void
readFSID (      FILE    * fptr,
          const String    fsType,
          const String    platform,
                String    fsID)

{
   char    line[MAXLINELENGTH];
   String  pform, type, id;
   int     lineLength;
   String  lineIn, lineOut;


   fsID[0] = '\0';

   while ( (lineLength=readLine(fptr,line)) != EOF &&
            stricmp(line,"end")             != 0 )
   {
      /* readLine has changed all white space to blanks ... now remove
         the blanks as they are not significant here */
      for (lineIn = lineOut = line; *lineIn != '\0'; lineIn++)
         if (*lineIn != ' ')
         {
            *lineOut = *lineIn;
            lineOut++;
         }
      *lineOut = '\0';

      pform = strtok(line,":");
      type  = strtok(NULL,"=");
      id    = strtok(NULL," ");
      if (stricmp(pform,platform)==0 && stricmp(type,fsType)==0)
         strncpy(fsID,id,MAXLINELENGTH);
   }

   return;

}  /* end readFSID */


/*--------------------------------------------------------------------------------
 *
 *   readDialogData
 *
 *   fill dtFSData with information from the configuration file; set
 *      dialogAvailable
 *
 *   fsID is a file-system identifier defined, in the configuration file,
 *      by a platform:file-system-type pair. The configuration file is
 *      searched from its current position until a definition section
 *      for fsID is found. The string comparison is case insensitive; if more
 *      than one match is found, the last in the file is used. The fields in
 *      dtFSData are initialized (e.g. string fields are set to "") if no
 *      matches are found or if no match is found for a particular field.
 *   Upon conclusion, the file is positioned at EOF.
 *
 *------------------------------------------------------------------------------*/

static void
readDialogData(      FILE      * fptr,
               const String      fsID,
                     Boolean   * dialogAvailable,
                     dtFSData  * fsDialogData)


{
   int     lineLength;
   String  s, id, token1, token2;
   String  msg1;
   char    line[MAXLINELENGTH];


   *dialogAvailable = FALSE;

   /* initialize fields in fsDialogData */
   strcpy(fsDialogData->buttonLabel,"");
   strcpy(fsDialogData->fsDialogProgram,"");
   strcpy(fsDialogData->warningMessage,"");
   fsDialogData->dismissStdPermissionDialog = FALSE;

   if (strlen(fsID) == 0)
      return;

   /* loop over lines in the file; note that readLine is called
      from within the loop except for the first pass */
   for (lineLength=readLine(fptr,line); lineLength != EOF; )
   {
      id = strtok(line,":");
      if (stricmp(id,fsID) == 0)
      {
         /* a section matching the input fsID has been found */
         *dialogAvailable = TRUE;
         token1 = strtok(NULL," =");
         s = strchr(token1,'\0') + 1;   /* first character after token1 */
         token2 = &s[strspn(s," =")];   /* first non-blank, non-= after token1 */
         /* loop to get data for fields in fsDialogData */
         while(strchr(token1,':') == NULL  && lineLength != EOF)
         {
            if (stricmp(token1,"buttonlabel") == 0)
               strncpy(fsDialogData->buttonLabel,token2,MAXLINELENGTH);
            else if (stricmp(token1,"warning") == 0)
            {
               strncpy(fsDialogData->warningMessage,token2,MAXLINELENGTH);
            }
            else if (stricmp(token1,"fsdialog") == 0)
               strncpy(fsDialogData->fsDialogProgram,token2,MAXLINELENGTH);
            else if (stricmp(token1,"dismiss") == 0)
               fsDialogData->dismissStdPermissionDialog = (stricmp(token2,"yes") == 0);
            else
            {
               msg1 = GETMESSAGE(21, 24, "Unknown field label in file manager configuration file: ");
               sprintf(g_errorMessage,"%s\"%s\"\n",msg1,token1);
               _DtSimpleError (application_name, DtWarning, NULL, g_errorMessage, NULL);
            }
            lineLength = readLine(fptr,line);
            if (lineLength != EOF)
            {
               token1 = strtok(line," =");
               s = strchr(token1,'\0') + 1;
               token2 = &s[strspn(s," =")];
            }
         } /* end while */
      }  /* end if */
      else
      {
         /* the current line does not match fsID ... get next line */
         lineLength = readLine(fptr,line);
      }
   }  /* end for */

   if ( ! *dialogAvailable )
   {
      msg1 = GETMESSAGE(21, 25, "No information found in file manager configuration file for file-system identifier");
      sprintf(g_errorMessage,"%s \"%s\"\n",msg1,fsID);
      _DtSimpleError (application_name, DtError, NULL, g_errorMessage, NULL);
   }

}  /* end readDialogData */


/*--------------------------------------------------------------------------------
 *
 *   readLine
 *
 *   read a line from the stream pointed to by fptr and return it in line
 *   line is allocated by the caller
 *
 *   blank lines and lines starting with * are skipped
 *   leading and trailing white space is removed
 *   imbedded white space characters are changed to blanks
 *   imbedded "\n" is changed to space, newline
 *   the length of the line or EOF (on end-of-file or error) is returned
 *
 *------------------------------------------------------------------------------*/

static int
readLine(FILE  * fptr, String  line)

{
   const char    commentChar = '#';
         char    myLine[MAXLINELENGTH];
         String  s, t;
         int     i, len;

   while (TRUE)
   {
      if ( fgets(myLine,MAXLINELENGTH,fptr) == NULL)
         return EOF;
      else
      {
         s = strip(myLine);  /* remove leading & trailing whitespace */
         if ((len=strlen(s)) != 0  &&  s[0] != commentChar)
         {
            /* change imbedded white space characters to spaces */
            for (i=0; i<len; i++)
               if (isspace(s[i]))
                  s[i] = ' ';

            /* change any imbedded "\n" to space followed by newline */
            while ( (t=strstr(s,"\\n")) != NULL )
            {
               *t     = ' ';      /* space */
               *(t+1) = '\n';     /* new line */
            }

            strncpy(line,s,MAXLINELENGTH);
            return len;
         }
      }
   }

}  /* end readLine */


/*--------------------------------------------------------------------------------
 *
 *  stricmp
 *
 *  compare strings ignoring case, return value as in strcmp
 *
 *------------------------------------------------------------------------------*/

static int
stricmp(const String  s1, const String  s2)

{
   int i;

   for (i=0; tolower(s1[i]) == tolower(s2[i]); i++)
      if (s1[i] == '\0')
         return 0;

   return (tolower(s1[i]) > tolower(s2[i])) ? +1 : -1;

}  /* end stricmp */


/*--------------------------------------------------------------------------------
 *
 *   strip
 *
 *   remove trailing white space by inserting \0
 *   return a pointer to the first non-white space character
 *------------------------------------------------------------------------------*/

static String
strip(String  s)

{
   int i;

   for (i=(strlen(s)-1); isspace(s[i]) && i>=0; i--)
      ;
   s[i+1] = '\0';

   for (i=0; isspace(s[i]); i++)
      ;

   return &s[i];

}  /* end strip */


/*----------------
 *   for testing
 *----------------*/

 /*
main(int argc, char **argv)
{
    char        pwd[100];
    int         err = 0;
    Boolean     test;
    dtFSData    data;

    if(argc > 1)
    {
        strcpy(pwd, argv[1]);
    }
    else
    {
        getcwd(pwd,sizeof(pwd));

    }
    printf("testing file \"%s\"\n",pwd);
    test = fsDialogAvailable(pwd, &data);
    if (test)
    {
        printf("dialog is available\n");
        printf("   buttonLabel=\"%s\"\n",data.buttonLabel);
        printf("   fsDialogProgram=\"%s\"\n",data.fsDialogProgram);
    }
    else
    {
        printf("dialog not available\n");
    }
}
*/

