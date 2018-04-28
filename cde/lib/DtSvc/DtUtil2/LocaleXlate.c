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
/* $TOG: LocaleXlate.c /main/14 1999/10/14 15:58:52 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
$FILEBEG$:    LocaleXlate.c
$PROJECT$:    CDE 1.0
$COMPONENT$:  DtLcx service
$1LINER$:     Locale translation routines
$COPYRIGHT$:
    (c) Copyright 1993, 1994 Hewlett-Packard Company
    (c) Copyright 1993, 1994 International Business Machines Corp.
    (c) Copyright 1993, 1994 Sun Microsystems, Inc.
    (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
$END$
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(sun)
#include <sys/utsname.h>
#endif
#include <limits.h>
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>		/* for getpw... */
#include <sys/param.h>		/* MAXPATHLEN */
#include <sys/utsname.h>	/* for uname */
#include <unistd.h>

/* for Xrm */
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>

#include "XlationSvc.h"
/*=================================================================
$SHAREDBEG$:  This is the header file that should appear in all 
DtLcx topics
=======================================================$SKIP$======*/
/*$INCLUDE$*/ 
#include "LocaleXlate.h"
/*$END$*/

/*========================================================*/
/*================ Introductory Info =====================*/
/*========================================================*/

#if DOC
/*========================================================*/
$INTROBEG$: _DtLcx family
$1LINER$: Translate locale and codeset strings to/from any platform
$SUMMARY$:
The _DtLcx family of routines enables the translation of a 
locale string from any platform into a standard locale string, 
and from standard locale string to the locale string of any platform.

This is useful because each platform, and in fact within
version ranges of some platforms, different strings are used
that have the same meaning.  This becomes a problem as soon as
any of these strings is stored in a file.  If the file is
opened on a platform for which the string has no meaning,
that functionality supported by using the string is lost.
With the _DtLcx service, the string can be translated into
a string appropriate to the current platform.

Standard locale strings are based on the XoJIG proposed strings,
but with a slightly altered syntax to adhere to the constraints
of the translation database.

The following are the current set of standard strings:

_DtLcx uses the _DtXlate translation database services,
but the _DtLcx routines provide a high-level interface to
the _DtXlate routines tuned to locales.  They allow any
component of a locale to be translated individually.

The _DtLcxOpenAllDbs() routine opens all the predefined
locale translation databases that can be found.  These
may are the following:
   /usr/dt/config/svc/<platform>.lcx
   /etc/dt/config/svc/<platform>.lcx
   $HOME/.dt/config/svc/<platform>.lcx

Alternatively, the DTLCXSEARCHPATH directory may be used
to specify a colon separated search path.  Relative paths
in the search path are relative to the current working
directory.

One of the nice features of the _DtLcx service is that
the translation database is designed for extension.  So
if new locales need to be supported, a system admin
or user can add the specifications to the appropriate
file and it will be immediately accessible.

/*=$END$================================================*/
#endif

#if DOC
/*========================================================*/
$INTROBEG$: _DtLcx translation table syntax
$1LINER$: translation table syntax for _DtLcx
$SUMMARY$:

  BNF Syntax of Locale Translation Specification
  ==============================================

For the full syntax of translation specifications,
see the _DtXlate documentation.

<stdvalue>   ::= <langterr>[.<codeset>[.<modifier>]]
<langterr>   ::= <identifier> | <matchall>
<codeset>    ::= <identifier> | <matchall>
<modifier>   ::= <identifier> | <matchall>
<opvalue>    ::= (<vischars>|<metachar>)+ | '"'(<anychar>|<metachar>)+'"'

  Semantics of the Translation Specification
  ==========================================

For a full description of translation table semantics,
refer to the _DtXlatedocumentation.

<operations> : a CDE-standardized identifier for the operation(s) to
which the value applies.  The operation(s) need not be supported by every
platform, but CDE must have standardized an identifier for the operation
in order for it to be used.  More than one identifier may be included by
concatenating them using the ',' separator, eg. "iconv1,iconv3".

_DtLcx defines a number of standard operation strings that use
locales.  Users of _DtLcx should use the constants beginning
_DtLCX_OPER... to specify a particular operation.

<stdvalue> : a sequence of one or more '.'-separated CDE-standardizd
identifiers or matchall characters.  This represents the canonical
string used as a standard representation of a semantic value that
may vary in different situations.

<langterr> : a CDE-standardized identifier for a language and territory
for which the operation is valid.  The language and territory must be
supported or irrelvant for the operations, as qualified for platform
and version, e.g. en_US.  The identifier need not be the identifier
used on any actual platform to specify language and territory.

<codeset> : a CDE-standardized identifier for a codeset for which the
operation is valid.  The codeset must be supported or irrelvant for
the operations, as qualified for platform and version, e.g. iso88591.
The identifier need not be the identifier used on any actual platform
to specify codeset.

<modifier> : a CDE-standardized identifier for a locale modifier.
The modifier must be supported or irrelvant for the operations,
as qualified for platform and version.  The identifier need not be
the identifier used on any actual platform to specify modifiers.

<opvalue> : can be used in three ways.  When a straight translation,
it is string that is matched against a query locale value to determine
the standard value of the locale.  When a OpToStd translation,
the string is a regular expression that is matched against the
query string.  When a StdToOp translation, the string is a
replacement value for the std value and may contain subexpression
replacement specifiers.

$EXAMPLE$:
These are some example specs:

HP-UX.900-999.setlocale.=.en_US.HP-ROMAN8:        american
HP-UX.900-999.setlocale.=.en_US.ISO-8859-1:       american.iso88591
HP-UX.900-999.setlocale.=.nl_NL.HP-ROMAN8:        dutch
HP-UX.900-999.setlocale.=.nl_NL.ISO-8859-1:       dutch.iso88591
/*=$END$================================================*/
#endif

#if DOC
/*========================================================*/
$INTROBEG$: _DtLcx example usage
$1LINER$: Examples of how to _DtLcx
$EXAMPLE$:
#include <LocaleXlate.h>
main()
{
   _DtXlateDb db = NULL;
   int  ret;
   char plat[_DtPLATFORM_MAX_LEN];
   int  execver;
   int  compver;
   char * val = NULL;
   char * str = NULL;
   char * val1 = NULL;
   char * val2 = NULL;
   char * val3 = NULL;

   ret = _DtLcxOpenAllDbs(&db);

   ret = _DtXlateGetXlateEnv(db,plat,&execver,&compver);
   printf("Platform: %s\nExec Ver: %d\nComp Ver: %d\n",
                    plat,execver,compver);
   ret = _DtLcxXlateStdToOp(db,plat,compver,DtLCX_OPER_SETLOCALE,
                     str="en_US.hp-roman8",NULL,NULL,NULL,&val);
   if (ret==0) printf("setlocale(%s) xlation=%s\n", str, val);
   else printf("no xlation\n", val);

   ret = _DtLcxXlateStdToOp(db,plat,compver,DtLCX_OPER_SETLOCALE,
                     str="en_US.?",NULL,NULL,NULL,&val);
   if (ret==0) printf("setlocale(%s) xlation=%s\n", str, val);
   else printf("no xlation\n", val);

   ret = _DtLcxXlateOpToStd(db,plat,execver,DtLCX_OPER_SETLOCALE,
                     str="american",&val,&val1,&val2,&val3);
   if (ret==0) printf("setlocale(%s) xlation=%s; %s; %s; %s\n",
                       str, val,val1,val2,val3);
   else printf("no xlation\n", val,val1,val2,val3);

   ret = _DtLcxXlateOpToStd(db,plat,execver,DtLCX_OPER_SETLOCALE,
                     str="dutch@fold",&val,&val1,&val2,&val3);
   if (ret==0) printf("setlocale(%s) xlation=%s; %s; %s; %s\n",
                       str, val,val1,val2,val3);

   ret = _DtLcxCloseDb(&db);
}
/*=$END$================================================*/
#endif


/*========================================================*/
/*====================== Constants =======================*/
/*========================================================*/

/*=============== private =================*/
/* A "random" number used to ensure that the Db has been initalized */
#define PATH_SEPARATOR ':'
#define EOS            '\0'
#define DIR_SLASH      '/'
#define DIR_SLASH_STR  "/"

#define MATCHALL_STR   "?"
#define DOT_STR        "."

/*=============== internal =================*/
#define  DTLCXSEARCHPATH  "DTLCXSEARCHPATH"

#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP "/etc/dt"
#endif

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP  "/usr/dt"
#endif

#ifndef CDE_USER_TOP
#define CDE_USER_TOP          ".dt"
#endif

#define DtLCX_USER_PATH     		s_LcxUserPath
#define DtLCX_INSTALL_AND_CONFIG_PATHS	s_LcxInstallAndConfigPaths

#define _DtLCX_INSTALL_DB_DIR     CDE_INSTALLATION_TOP "/config/svc/"

/* This is the file type of a _DtLcx file */
#define _DtLCX_DATABASE_TYPE      ".lcx"
/* This is the name of the fallback _DtLcx file */
#define _DtLCX_DATABASE_FALLBACK  ("dtcomplete" _DtLCX_DATABASE_TYPE)
/* This is the name of the CDE standard _DtLcx file */
#define _DtLCX_DATABASE_CDE       ("CDE" _DtLCX_DATABASE_TYPE)

/*========================================================*/
/*====================== Variables =======================*/
/*========================================================*/

static char s_LcxUserPath[] = CDE_USER_TOP "/config/svc";
static char s_LcxInstallAndConfigPaths[] = 
                 CDE_INSTALLATION_TOP  "/config/svc:"
                 CDE_CONFIGURATION_TOP "/config/svc:";

/*========================================================*/
/*================== Private routines ====================*/
/*========================================================*/

#if DOC
/*========================================================*/
/*
$PFUNBEG$: GetHomeDirPath()
$1LINER$:  Retrieves path to current user's home directory
$SUMMARY$: 
Looks for first the HOME and then USER environment
variables.  If these are not set, uses the password
info to get the user's home directory.
$ARGS$:
outptr: pts to string allocated by caller to hold the home dir path 
        Generally, the string should be at least MAXPATHLEN+1 in size.
max:    maximum number of bytes allowed (including ending bytes).
$RETURNS$:
 */
/*================================================$SKIP$==*/
#endif
static
void GetHomeDirPath(
         char * outptr,
	 unsigned int    max)
{        /*$CODE$*/
    int    uid;
    char * ptr = NULL;
    _Xgetpwparams pwd_buf;
    struct passwd * pwd_ret;

    if((ptr = (char *)getenv("HOME")) == NULL) 
    {
        if((ptr = (char *)getenv("USER")) != NULL)
            pwd_ret = _XGetpwnam(ptr, pwd_buf);
        else 
        {
            uid = getuid();
            pwd_ret = _XGetpwuid(uid, pwd_buf);
        }
        if (pwd_ret != NULL)
            ptr = pwd_ret->pw_dir;
        else
            ptr = NULL;
    }

    if (ptr && strlen(ptr))
    { 
	strncpy(outptr, ptr, max-1);
	outptr[max-1] = '\0';
    }
    else outptr[0] = '\0' ;
}        /*$END$*/


/*========================================================*/
/*================ Public DtLcx routines =================*/
/*========================================================*/


#if DOC
/*========================================================*/
$FUNBEG$: _DtLcxOpenAllDbs()
$1LINER$: Open and merge all locale translation databases that can be found
$SUMMARY$:
DtLcxOpenAllDbs() locates all translation databases
named "<platform>.lcx" present in the DTLCXSEARCHPATH 
directories.  If none exist, the file "dtcomplete.lcx"
is tried in those directories.  Finally, the database "CDE.lcx"
is merged into those databases already loaded.

The <platform> string is taken from uname(2), which is the
same string returned by the command 'uname -s'.  For example,
on HP-UX platforms, the string is "HPUX", so the translation
databases to be loaded must be named "HPUX.lcx".

By default, the search paths are:
DTLCXSEARCHPATH = "/usr/dt/config/svc:" \
                  "/etc/dt/config/svc:" \
                  "$HOME/.dt/config/svc"

Alternatively, the DTLCXSEARCHPATH directory may be used
to specify a colon separated search path.  Relative paths
in the search path are relative to the current working
directory.
$ARGS$:
$RETURNS$:
Returns the return value of _DtXlateOpenAllDbs()
/*================================================$SKIP$==*/
#endif

int  _DtLcxOpenAllDbs(
         _DtXlateDb *   ret_db)
{       /*$CODE$*/
   char * paths;
   char * dbPaths;
   struct utsname names;
   int    ret = 0;
   int    globRet = -1;
   int    len;
   char   lcxfile[100];
   char   homePath[MAXPATHLEN];
   _DtXlateDb   cde_db = NULL;

#define MAXSHORTFNAMELEN 14

   /* get host specifics and generate platform-specific lcx file name */
   uname(&names);
   len = MAXSHORTFNAMELEN - strlen(_DtLCX_DATABASE_TYPE);
   strncpy(lcxfile,names.sysname,len);
   lcxfile[len-1] = EOS;
   strcat(lcxfile,_DtLCX_DATABASE_TYPE);       /* e.g. HP-UX.lcx */

   /* get paths for LCX */
   paths = getenv(DTLCXSEARCHPATH);
   if (NULL != paths && paths[0] != EOS)
   {
      dbPaths = strdup(paths);
   }
   else
   {
      const char *slash;
      char *end;

      /* no DTLCXSEARCHPATH; build a default path */
      GetHomeDirPath(homePath, MAXPATHLEN);
      end = homePath + strlen(homePath);
      _DtMBStrrchr(homePath,DIR_SLASH,-1,&slash);
      if ((end - 1) != slash && end < homePath + MAXPATHLEN - 2 )
	{
	  *end++ = DIR_SLASH;
	  *end   = EOS;
	}
      if (end < homePath + MAXPATHLEN - strlen(DtLCX_USER_PATH) - 1)
          strcat(homePath,DtLCX_USER_PATH);

      dbPaths = malloc(sizeof(char) * 
                (strlen(homePath)+strlen(DtLCX_INSTALL_AND_CONFIG_PATHS)+5));
      if (dbPaths) 
         sprintf(dbPaths,"%s:%s",DtLCX_INSTALL_AND_CONFIG_PATHS,homePath);
   }

   /* open all dbs of filename found in paths */
   globRet = _DtXlateOpenAllDbs(dbPaths,lcxfile, ret_db);
   if (globRet != 0)
   {  /* on failure */
      /* open all dbs of the fallback filename found in paths */
      globRet = _DtXlateOpenAllDbs(dbPaths,_DtLCX_DATABASE_FALLBACK, ret_db);
   }

   /* merge in the CDE standard translations database */
   ret = _DtXlateOpenAllDbs(dbPaths,_DtLCX_DATABASE_CDE, &cde_db);

   if (ret == 0)
   {
      globRet = 0;
      _DtXlateMergeDbs(&cde_db,ret_db);  /* cde_db get's closed by the merge */
   }
   else
      _DtXlateCloseDb(&cde_db);

   if(dbPaths) free(dbPaths);
   return globRet;
}       /*$END$*/

#if DOC
/*========================================================*/
$FUNBEG$: _DtLcxCloseDb()
$1LINER$: Close an open locale translation database
$SUMMARY$:
_DtLcxCloseDb() releases all memory associated with
the translation database.  Further use of the database
object is an error.
$ARGS$:
$RETURNS$:
 0:  database was valid and has been closed
-1:  invalid database pointer

$DEF$:
int _DtLcxCloseDb(
       _DtXlateDb * io_db)
$NOTE$: the current implementation is a macro call to
        _DtXlateCloseDb().
/*================================================$SKIP$==*/
#endif


#if DOC
/*========================================================*/
/*
$FUNBEG$: _DtLcxXlateOpToStd()
$1LINER$: Translate an operation-specific locale to a standard locale
$SUMMARY$:
_DtLcxXlateOpToStd() is designed to allow a locale string
that is specific to some platform, version, and operation to be
translated to a CDE standard locale.  For example, a locale
string stored in a file on systemA and recovered by an application
on systemB can be translated into a CDE standard locale by the
application on system B.

The xlationDb is the database to use for the translation.
It must have been opened with a call to _DtLcxOpenAllDbs().

The platform and version arguments can be used to specify
systemA if the information is known.  The platform value
should be one of _DtPLATFORM_xxx.  If platform is unknown,
use _DtPLATFORM_UNKNOWN, and if version is unknown, use -1.

The operation is an optional argument.  If the operation is known
from which the opValue originated or for which it was intended
for use on systemA, the operation may be specified using the
appropriate DtLCX_OPER_xxx constant. If the operation is unknown,
it should be NULL.

The opValue is the systemA locale string recovered by the
application on systemB and which should be translated.  It is
an error if it is NULL.

Zero or more of the ret_xxx arguments may be NULL values.
If they are NULL, that value is not determined.  For
non-NULL ret_xxx arguments, the string value of each
is allocated using malloc() and the pointer is assigned
at the location pointed to by the argument.  The caller
function should free the memory wth free() when it is
no longer needed.  The ret_xxx arguments have the following
values.

The ret_stdLocale points to a caller-owned string with
the CDE standard equivalent to opValue.  This value is the '.'-
separated concatenation of the <langterr> and <codeset> fields
of the specification.  If <langterr> or <codeset> is the matchall
character, that value and the '.' separator are dropped.

The ret_stdLangTerr points to a caller-owned string with
the CDE standard equivalent of the opValue's language and
territory.  If <langterr> or <codeset> in the translation
specification is the matchall character, the value is the
empty string.

The ret_stdCodeset points to a caller-owned string with
the CDE standard equivalent of the opValue's codeset.
If <codeset> in the translation specification is the matchall
character, the value is the empty string.

The ret_stdModifier points to a caller-owned string with
the CDE standard equivalent of the opValue's modifier.
If <modifier> in the translation specification is the matchall
character or not present, the value is the empty string.
$ARGS$:
xlation_db:	a translation database
platform:	the platform string (see _DtXlateGetXlateEnv())
version:	the version number (see _DtXlateGetXlateEnv())
operation:	the operation of interest, e.g. "setlocale"
opValue:	the operation-specific value pattern
ret_stdLocale:	location where ptr to standard locale string is stored
ret_stdLangTerr:location where ptr to standard lang+terr string is stored
ret_stdCodeset:	location where ptr to standard codeset string is stored
ret_stdModifier:location where ptr to standard modifier string is stored
$RETURNS$:
Return values are those of _DtXlateOpToStd()
 */
/*================================================$SKIP$==*/
#endif

int _DtLcxXlateOpToStd(
       const _DtXlateDb   xlationDb,
       const char *       platform,
       const int          version,
       const char *       operation,
       const char *       opValue,
       char * *           ret_stdLocale,
       char * *           ret_stdLangTerr,
       char * *           ret_stdCodeset,
       char * *           ret_stdModifier)
{       /*$CODE$*/
    int     ret;
    char *  stdValue = NULL;
    Boolean freeStdValue = True;
    int     scanned = 0;
    char    langterr[50];
    char    codeset[50];
    char    mod[50];

    /* do the translation */
    ret = _DtXlateOpToStdValue(xlationDb,platform,version,operation,opValue,
                                      &stdValue,NULL);

    /* std locale string syntax: langterr.codeset.modifier */
    /* parse into the desire chunks */
    if (ret == 0 && stdValue) 
       scanned = sscanf(stdValue,"%[^.].%[^.].%s",langterr,codeset,mod);

    /* locale string is just the std value */

    if ( ret_stdLocale ) 
        { *ret_stdLocale = stdValue; freeStdValue = False; }
    if ( ret_stdLangTerr)
        { *ret_stdLangTerr = ( scanned >= 1 ? strdup(langterr) : NULL); }
    if ( ret_stdCodeset ) 
        { *ret_stdCodeset = ( scanned >= 2 ? strdup(codeset) : NULL); }
    if ( ret_stdModifier ) 
        { *ret_stdModifier = ( scanned >= 3 ? strdup(mod) : NULL); }

    if (freeStdValue && NULL != stdValue) free(stdValue);
    return ret;
}       /*$END$*/


#if DOC
/*========================================================*/
$FUNBEG$: _DtLcxXlateStdToOp()
$1LINER$: Translate a standard locale to an operation-specific one
$SUMMARY$:
_DtLcxXlateStdToOp() is designed to allow a locale string
that is the CDE standard locale to be translated to the appropriate
string for some platform, version, and operation.  For example, a
CDE locale string stored in a file on systemA and recovered by an
application on systemB can be translated into a platform- and
operation-specific locale by the application on system B.

_DtLcxXlateStdToOp() takes a number of query qualifiers
and determines the best translation that matches them.  The
routine allocates memory for the resulting operation-specific
locale using malloc() and stores the pointer to it at the
location pointed to by ret_opValue.  If the query qualifiers
do not uniquely identify a translation specification, the
specification used is chosen at random from those that match.
[Random selection is an artifact of using XrmEnumerateDatabase()
to process the contents of the table--entries are not presented
to the processing routine in a pre-specified order.]

The xlationDb is the database to use for the translation.
It must have been opened with a call to _DtLcxOpenAllDbs().

The platform and version arguments are optional.  If the desired
platform is the current one for the executing application,
use _DtPLATFORM_CURRENT, and if version is the current one
for the application, use -1.

The operation is a required argument, and should be selected
from the DtLCX_OPER_xxx constants.

The stdLocale is the CDE standard locale string, such as the
one recovered using _DtLcxXlateOpToStd().  The value is
the '.'-separated concatenation of the <langterr>, <codeset>,
and <modifier> fields, or it may be NULL.  The stdLocale string
alone *or* any combination of stdLangTerr, stdCodeset, and
stdModifier strings may be specified for the translation, but
not both.

The stdLangTerr, stdCodeset, and stdModifier are the CDE strings,
such as the one recovered using _DtLcxXlateOpToStd().  The
strings are valid across CDE platforms and across operations.
The stdLangTerr, stdCodeset, or stdModifier string may be used
alone *or* in any combination with each other, but not with the
stdLocale string.  The values may also be NULL.
$ARGS$:
xlationDb:	a translation database
platform:	the platform string (see _DtXlateGetXlateEnv())
version:	the version number (see _DtXlateGetXlateEnv())
operation:	the operation of interest, e.g. "setlocale"
stdValue:	the standard value pattern
stdLocale:	standard locale strin
stdLangTerr:	standard lang+terr string
stdCodeset:	standard codeset string
stdModifier:	standard modifier string
ret_opValue:	location where ptr to translated string is stored
$RETURNS$:
-1: if all stdXxx strings have NULL values
plus the return values are those of _DtXlateStdToOp()
/*================================================$SKIP$==*/
#endif

int _DtLcxXlateStdToOp(
       const _DtXlateDb   xlationDb,
       const char *       platform,
       const int          version,
       const char *       operation,
       const char *       stdLocale,
       const char *       stdLangTerr,
       const char *       stdCodeset,
       const char *       stdModifier,
       char * *           ret_opValue)
{       /*$CODE$*/
#define DTLCXXLATE_STDTOOP_BUFSIZE	256
    char   stdValueBuf[DTLCXXLATE_STDTOOP_BUFSIZE];
    char   *stdValue = stdValueBuf;
    char   empty = EOS;
    char * matchall = MATCHALL_STR;
    char * dot = DOT_STR;
    char * sepLC = &empty;
    char * sepCM = &empty;
    int retval;
 
    if (stdLocale)
    {
	if (strlen(stdLocale) >= DTLCXXLATE_STDTOOP_BUFSIZE)
	  stdValue = malloc(strlen(stdLocale) + 1);
	else
	  stdValue = stdValueBuf;

        stdValue[0] = EOS;
        strcpy(stdValue, stdLocale);
    }
    else 
    {  
       int need = 0;
       int bytes_needed = 0;

#define NEED_LANGTERR 0x01
#define NEED_CODESET  0x02
#define NEED_MODIFIER 0x04

       if (stdLangTerr)  need = NEED_LANGTERR;
       if (stdCodeset)   need = NEED_LANGTERR | NEED_CODESET;
       if (stdModifier)  need = NEED_LANGTERR | NEED_CODESET | NEED_MODIFIER;

       if (need == 0) return -1;    /* RETURN: need a pattern */

       /* only include matchalls for those that are needed */
       /* This is needed because of the manner of scoring matches.
          If unnecessary matchall's are present, that can detract
          from the score. */
       if (need & NEED_LANGTERR) 
          { stdLangTerr = (stdLangTerr ? stdLangTerr : matchall); }
       if (need & (NEED_LANGTERR | NEED_CODESET))
          { stdCodeset = (stdCodeset ? stdCodeset : matchall); sepLC = dot; }
       if (need & (NEED_LANGTERR | NEED_CODESET | NEED_MODIFIER))
          { stdModifier = (stdModifier ? stdModifier : matchall); sepCM = dot; }

	bytes_needed =
	  strlen(stdLangTerr) + strlen(sepLC) + strlen(stdCodeset) +
	  strlen(sepCM) + strlen(stdModifier) + 1;

	if (bytes_needed > DTLCXXLATE_STDTOOP_BUFSIZE)
	  stdValue = malloc(bytes_needed);
	else
	  stdValue = stdValueBuf;

       /* generate the std value string */
       stdValue[0] = EOS;
       sprintf(stdValue,
		"%s%s%s%s%s", 
                stdLangTerr, sepLC, stdCodeset, sepCM, stdModifier);
    }

    retval = _DtXlateStdToOpValue(
				xlationDb,platform,version,operation,
				stdValue, ret_opValue,NULL);
    if (stdValue && stdValue != stdValueBuf) free(stdValue);
    return retval;
}       /*$END$*/
