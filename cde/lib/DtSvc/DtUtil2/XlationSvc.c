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
/* $TOG: XlationSvc.c /main/13 1999/10/14 15:59:35 mgreess $ */
/****************************************************************************
$FILEBEG$:    XlationSvc.c
$PROJECT$:    Cde 1.0
$COMPONENT$:  DtXlate service
$1LINER$:     Implements a translation service using tables and regex search
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
#include <limits.h>           /* INT_MAX */
#include <pwd.h>              /* for getpw... */
#include <sys/utsname.h>      /* for uname */
#include <sys/param.h>        /* MAXPATHLEN */

/* for RADIXCHAR and nl_langinfo */
#if defined(linux)
# define RADIXCHAR MON_DECIMAL_POINT
#endif
#include <langinfo.h>       

#if defined(sun) || defined(USL) || defined(__uxp__)
#include <regexpr.h>          /* for compile,advance */
#else
#include <regex.h>            /* for regcomp,regexec */
#endif

/* for Xrm */
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>

/*=================================================================
$SHAREDBEG$:  This header appears in all appropriate DtXlate topics
=======================================================$SKIP$======*/
/*$INCLUDE$*/ 
#include "XlationSvc.h"
#include "LocaleXlate.h"
/*$END$*/

/**** For extensive Introductory info, go to the end of this file ****/

/*========================================================*/
/*====================== Constants =======================*/
/*========================================================*/

/*=============== private =================*/
/*#define DBG_MATCHING  ** if defined, debugging matching stages are compiled */

#define DEBUG_XLATE_NAME	"dtXlate.debugDtXlate"
#define DEBUG_XLATE_CLASS	"DtXlate.DebugDtXlate"

#define EOS                '\0'

/* A "random" number used to ensure that the Db has been initalized */
#define INIT_OCCURRED      2329479
#define PATH_SEPARATOR     ':'
#define DIR_SLASH          '/'

#define LESSTHAN_STR     "<"
#define EQUALS_STR       "="
#define CONTAINS_STR     "~"
#define MORETHAN_STR     ">"
#define INHIBIT_STR      "0"

#define MATCHALL_CHAR      '?'
#define ESCAPE_CHAR        '\\'
#define COMMENT_CHAR       '!'
#define QUOTE_CHAR         '\"'
#define OPER_SEPARATOR     ','
#define STDVALUE_SEPARATOR '.'
#define MATCHALL_VER       0

#define PLATFORM_QUARK   0      /* index offsets into quarklist */
#define VERSION_QUARK    1
#define OPERS_QUARK      2
#define DIRECTION_QUARK  3
#define FIRSTSTD_QUARK   4

#define MAXSPECQUARKS    43     /* std + platform + version + operation */
#define MAXSTDQUARKS     40
#define MAXRHSSIZE       100    /* max supported RHS size */
#define MAXLHSSIZE       200    /* max supported LHS size */
#define MAXINTSTRSIZE    15     /* handle any long int -> string */

typedef enum {
   __DtXLATE_TYPE_NONE = 0,
   __DtXLATE_TYPE_INHIBIT = 1,
   __DtXLATE_TYPE_REGEX = 2,
   __DtXLATE_TYPE_PURE = 3,
   __DtXLATE_TYPE_CONTAINS = 4
} __DtXlateType;

#if defined(sun) || defined(USL) || defined(__uxp__)
/* Sun doesn't support regcomp() and regexec() yet, so
   define this here and fill it will the results of advance() */
typedef struct regmatch_t {
    int rm_so;        /* start offset */
    int rm_eo;        /* end offset */
} regmatch_t;
#endif

/*========================================================*/
/*====================== Typedefs ========================*/
/*========================================================*/

#if DOC
/*========================================================*/
$PTYPEBEG$: __DtXlateDbRec
$1LINER$:  A private object used to represent translation dbs
$SUMMARY$:
__DtXlateDbRec is the type of the contents of a translation database
object.  The database object must be opened before use and closed
after use.  The definition of the object is opaque to users.
$ARGS$:
xrmDb:         Xrm database used to hold specs
initGuard:     used to test whether Db initialized
/*================================================$SKIP$==*/
#endif

/*$DEF$*/
typedef struct __DtXlateDbRec
{
  XrmDatabase  xrmDb;
  int          initGuard;
  Boolean      debugMode;
} __DtXlateDbRec;
/*$END$*/

#if DOC
/*========================================================*/
$PTYPEBEG$: __DtXlateSrchData
$1LINER$:  A private object used to collect search-related data
$SUMMARY$:
__DtXlateSrchData stores all the data pertaining to a search
and the search results.  The search routines utilize this
to maintain status info over multiple calls by the
enumeration routines and to return info to the routine
that initiated the search.

$ARGS$:
filterQuarks:  quark list for the platform, version, and op filter
stdValueQuarks:   quark list for the std value
opValue:       ptr to operation-specific value string
SpecRef:       indices 1 to MAXSPECQUARKS point to matching Xrm strings
SubEx:         indices 1 to MAXSPECQUARKS index sub exprs in opValue
/*================================================$SKIP$==*/
#endif
/*$DEF$*/
typedef struct __DtXlateSrchData
{
   /* db info */
   _DtXlateDb      db;

   /* query info */
   const char *    platformStr;
   XrmQuark        platformQuark;
   int             version;
   char            verStr[MAXINTSTRSIZE];   /* handle any long int */
   const char *    operStr;
   int             operStrLen;
   XrmQuark        operQuark;
   XrmQuark        lessThanQuark;
   XrmQuark        equalsToQuark;
   XrmQuark        containsQuark;
   XrmQuark        moreThanQuark;
   XrmQuark        inhibitQuark;

   /* query or search info */
   XrmQuark        stdValueQuarks[MAXSTDQUARKS];
   const char *    opValue;

   /* search info */
   __DtXlateType   curTransType;
   __DtXlateType   bestTransType;
   int             curScore;
   int             bestScore;
   /* MAXSPECQUARKS is depended upon elsewhere to be the size of these */
   const char *    curSpecRefs[MAXSPECQUARKS];
   const char *    bestSpecRefs[MAXSPECQUARKS];  
   regmatch_t      curSubEx[MAXSPECQUARKS];   /* pattern match data */
   regmatch_t      bestSubEx[MAXSPECQUARKS];  /* pattern match data */
} __DtXlateSrchData;
/*$END$*/

/*========================================================*/
/*================== Private routines ====================*/
/*========================================================*/

#if DOC
/*========================================================*/
$PFUNBEG$:  ExpandPath()
$1LINER$: adds current working directory to front of path if its relative
$SUMMARY$: 
If path is absolute, returns a malloced copy.
If path is relative, inserts the CWD in front of the relative path
and returns a mallocedd memory.

The caller must free the memory when no longer needed.
$ARGS$:
filespec:  the pathname
$RETURNS$:
ptr to mallocedd memory or NULL
/*================================================$SKIP$==*/
#endif

char * ExpandPath (
       const char * filespec)
{  /* $CODE$ */
    char tmpPath[MAXPATHLEN + 2];
    char * pathName;
    char * eos = NULL;
    const char * slash = NULL;

    /* test args */
    if (NULL == filespec) return NULL;

    /*** is the file absolute ***/
    /* if filespec begins with / then it is absolute */
    if (   (   MB_CUR_MAX == 1 
            || mblen(filespec, MB_CUR_MAX) == 1)/* 1st char is 1 byte */
         && *filespec == DIR_SLASH)  /* and its a / */
    {
       return strdup(filespec);        /* RETURN */
    }

    /*** otherwise, make it relative to the current directory ***/

    /* get user's current working directory */
    if (getcwd(tmpPath, MAXPATHLEN) == 0) return NULL; /* RETURN: error */

    /*** add a slash to end of path component, if needed ***/
    /* get end of the string */
    eos = tmpPath + strlen(tmpPath);
    /* get last slash */
    _DtMBStrrchr(tmpPath,DIR_SLASH,-1,&slash);
    if (slash != (eos-1)) /* is slash last char of path? */
    {
       *eos++ = DIR_SLASH; 
       *eos = EOS; 
    }

    /* make a malloc'd copy of the path with room to grow */
    pathName = malloc(sizeof(char) * 
                 (strlen(filespec) + (eos-tmpPath) + 5) ); /* 5: arbitrary */
    if (NULL == pathName) return NULL;		/* RETURN: error */

    /* build the absolute path */
    strcpy(pathName,tmpPath);
    strcat(pathName,filespec);

    return pathName;                             /* RETURN: found */
}  /* $END$ */

#if DOC
/*========================================================*/
$PFUNBEG$:  DeleteDbMem()
$1LINER$: Zeros out the db mem & frees it
$SUMMARY$: 
Zeros out the db mem & frees it
The xrmDb should have already been destroyed before calling
this function.
$ARGS$:
io_db:  db to delete
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
void DeleteDbMem(
      _DtXlateDb * io_db)
{ /*$CODE$*/
       /* zero out object mem and free it */
       (*io_db)->xrmDb = NULL;
       (*io_db)->initGuard = 0;
       free(*io_db);
       *io_db= NULL;
} /*$END$*/

#if DOC
/*========================================================*/
$IFUNBEG$:  _DtMBStrchr()
$1LINER$: Searches for a character in a multibyte string
$SUMMARY$: 
Returns in 'ret_ptr' the address of the first occurence of 'value'
in string s1.  Value may also be the end of string marker '\0'.
$ARGS$:
$RETURNS$:
-1  If found an invalid character.
 0  If found value in string s2
 1  If found the null byte character without finding 'value'.
    'ret_ptr' will also be null in this case.
/*================================================$SKIP$==*/
#endif

int _DtMBStrchr (
    const char * s1,
    int          value,
    int          max_len,
    const char * * ret_ptr )
{       /*$CODE$*/
    int          len;
    const char * p1;
    wchar_t      wcs;

    *ret_ptr = NULL;

    if (!s1 || *s1 == '\0')
        return 1;

    if (max_len == 1)
      {
        *ret_ptr = strchr (s1, value);
        if (*ret_ptr)
            return 0;
        return 1;
      }

    p1 = s1;
    while (*p1 != '\0')
      {
        len = mblen (p1, max_len);
        if (len == -1)
            return -1;
        if (len == 1)
          {
            if (*p1 == value)
              {
                *ret_ptr = p1;
                return 0;
              }
            p1++;
          }
        else
          {
            if (mbstowcs (&wcs, p1, 1) == value)
              {
                *ret_ptr = p1;
                return 0;
              }
            p1 += len;
          }
      }

    /* check for match on EOS */
    if (*p1 == value) *ret_ptr = p1;

    return ((*ret_ptr) ? 0 : 1);
}       /*$CODE$*/


#if DOC
/*========================================================*/
$IFUNBEG$:  _DtMBStrrchr()
$1LINER$: Searches for a character in a multibyte string
$SUMMARY$: 
Returns in 'ret_ptr' the address of the last occurence of 'value'
in string s1.  Value may also be the end of string marker '\0'.
$ARGS$:
$RETURNS$:
-1  If found an invalid character.
 0  If found value in string s2
 1  If found the null byte character without finding 'value'.
    'ret_ptr' will also be null in this case.
/*================================================$SKIP$==*/
#endif

int _DtMBStrrchr (
    const char *   s1,
    int            value,
    int            max_len,
    const char * * ret_ptr )
{        /*$CODE$*/
    int          len;
    const char * p1;
    wchar_t      wcs;

    *ret_ptr = NULL;

    if (!s1 || *s1 == '\0')
        return 1;

    if (max_len == 1)
      {
        *ret_ptr = strrchr (s1, value);
        if (*ret_ptr)
            return 0;
        return 1;
      }

    p1 = s1;
    while (*p1 != '\0')
      {
        len = mblen (p1, max_len);
        if (len == -1)
            return -1;
        if (len == 1)
          {
            if (*p1 == value)   *ret_ptr = p1;
            p1++;
          }
        else
          {
            if (mbstowcs (&wcs, p1, 1) == value)   *ret_ptr = p1;
            p1 += len;
          }
      }

    /* check for match on EOS */
    if (*p1 == value) *ret_ptr = p1;

    return ((*ret_ptr) ? 0 : 1);
}


#if DOC
/*========================================================*/
$PFUNBEG$: SetDebugModeState()
$1LINER$:  Checks db for debug mode and sets flag
$SUMMARY$: 
$WARNING$:
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
void SetDebugModeState(
       _DtXlateDb    dbRec)
{   /*$CODE$*/
    XrmValue value;
    char * str_type;
    if (XrmGetResource(dbRec->xrmDb,
           DEBUG_XLATE_NAME,DEBUG_XLATE_CLASS,&str_type,&value) == True)
       dbRec->debugMode = True;
}


#if DOC
/*========================================================*/
$PFUNBEG$: ReplaceMatchallSubex()
$1LINER$:  Replace matchall subexpression refs (e.g. ?1) with values
$SUMMARY$: 
If subexpressions are specified and referenced, the
routine assumes that the string pointed to by pStr was
allocated using malloc() and can be resized using realloc().
The value and size of pStr may be different after the call.
$WARNING$:
This routine assumes it is working on a stdvalue expression
(e.g. from the LHS of a spec), that uses only stdvalue strings
or matchall-style subexpression replacement specs, e.g. ?1.
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
void ReplaceMatchallSubex(
         char * *       pStr,
         regmatch_t *   subex,
         const char *   matchedStr)
{        /*$CODE$*/
    char * nxt = *pStr;
    char * old = *pStr;

    /* strip escapes out */
    for ( *nxt = *old;        /* xfer but don't advance */
          *old != EOS;
          *nxt = *old )       /* xfer but don't advance */
    {
        if ( *old == MATCHALL_CHAR )    /* if an escaped char follows */
        {
           /* if MATCHALL_CHAR is not followed by a digit, e.g. \1
              or no replacement values exist, ignore it */
           if (   NULL == subex 
               || isdigit(*(old+1)) == 0 ) 
           {
              old++;                 /* go past the escape char */
              *nxt++ = *old++;       /* keep just the char that was escaped
                                        and assign here to avoid tranlating
                                        that character, then move on to the 
                                        next one */
              continue;                      /* CONTINUE */
           }
           else  /* a value reference is being made */
           {  /* get the refNum and advance the ptr */
              int refNum, numLen;
              int newOff, oldOff;

              sscanf(++old,"%d%n", &refNum, &numLen);
              old += numLen;   /* move old ptr past the ref number */

/* printf("%d=%s\n", refNum, &matchedStr[subex[refNum].rm_so]); ** DBG */

              /* test for valid replacement */
              if (   refNum >= 0 
                  && refNum < MAXSPECQUARKS 
                  && subex[refNum].rm_so != -1)
              {
                 int    repLen;
                 int    strLen;
                 char * oldTmp;
                 char * newTmp;

                 newOff = nxt - *pStr;
                 oldOff = old - *pStr;
                 repLen = subex[refNum].rm_eo - subex[refNum].rm_so;
                 strLen = strlen(*pStr);

                 /* expand memory and reset pointers */
                 *pStr = realloc(*pStr,strLen+repLen+1);
                 if (NULL == *pStr) return;           /* RETURN */
                 nxt = *pStr + newOff;
                 old = *pStr + oldOff;

                 /* move rest back to leave room for the replacement value */
                 oldTmp = *pStr+strLen;      /* pts to old EOS */
                 newTmp = *pStr+strLen-(oldOff-newOff)+repLen;/*pts to new EOS*/
                 while (oldTmp >= old) *newTmp-- = *oldTmp--; 

                 /* replace the ref with a value but don't append EOS */
                 strncpy(nxt,&matchedStr[subex[refNum].rm_so],repLen);
                 nxt += repLen;  /* move new to end of replace string */
                 old += repLen - (oldOff-newOff);  
                                 /* move old to end of expanded old string */
              }  /* if valid replacement */
           }  /* if a replacement requested */
           continue;                              /* CONTINUE */
        }  /* if an escaped character */
        /* if survived all the checks, can advance to next char */
        nxt++;
        old++;
    }
}        /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: StripMetaAndReplaceEscSubex()
$1LINER$:  Strips off meta chars and replaces escaped subex (e.g. \1) values
$SUMMARY$:
Strip is performed in place if replaceValues is NULL.

If replaceValues are specified and referenced, the
routine assumes that the string pointed to by pStr was
allocated using malloc() and can be resized using realloc().
The value and size of pStr may be different after the call.

$WARNING$:
This routine assumes it is working on a value expression
(e.g. from the RHS of a spec), that uses meta chars and
regex(5)-style subexpression replacement specs.
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
void StripMetaAndReplaceEscSubex(
         char * *       pStr,
         const Boolean  keepEscChar,
         const char * * replaceValues)
{        /*$CODE$*/
    char * nxt = *pStr;
    char * old = *pStr;
    Boolean inQuote = False;

    /* strip escapes out */
    for ( *nxt = *old;        /* xfer but don't advance */
          *old != EOS;
          *nxt = *old )       /* xfer but don't advance */
    {
        if ( *old == ESCAPE_CHAR )    /* if an escaped char follows */
        {
           /* if ESCAPE_CHAR is not followed by a digit, e.g. \1
              or no replacement values exist, ignore it */
           if (   NULL == replaceValues 
               || keepEscChar
               || isdigit(*(old+1)) == 0 )
           {
              if (!keepEscChar) old++;       /* go past the escape char */
              else *nxt++ = *old++;          /* copy esc char over */
              *nxt++ = *old++;       /* keep just the char that was escaped
                                        and assign here to avoid tranlating
                                        that character, then move on to the
                                        next one */
              continue;                      /* CONTINUE */
           }
           else  /* a value reference is being made */
           {  /* get the refNum and advance the ptr */
              int refNum, numLen;
              int newOff, oldOff;

              sscanf(++old,"%d%n", &refNum, &numLen);
              old += numLen;   /* move old ptr past the ref number */

/*  printf("%x=%s\n", replaceValues[refNum], replaceValues[refNum]); ** DBG */

              /* test for valid replacement */
              if (   refNum >= 0
                  && refNum < MAXSPECQUARKS
                  && replaceValues[refNum] != NULL)
              {
                 int    repLen;
                 int    strLen;
                 char * oldTmp;
                 char * newTmp;

                 newOff = nxt - *pStr;
                 oldOff = old - *pStr;
                 repLen = strlen(replaceValues[refNum]);
                 strLen = strlen(*pStr);
                 /* expand memory and reset pointers */
                 *pStr = realloc(*pStr,strlen(*pStr)+repLen+1);
                 if (NULL == *pStr) return;           /* RETURN */
                 nxt = *pStr + newOff;
                 old = *pStr + oldOff;

                 /* move rest back to leave room for the replacement value */
                 oldTmp = *pStr+strLen;      /* pts to old EOS */
                 newTmp = *pStr+strLen-(oldOff-newOff)+repLen;/*pts to new EOS*/
                 while (oldTmp >= old) *newTmp-- = *oldTmp--;

                 /* replace the ref with a value but don't append EOS */
                 strncpy(nxt,replaceValues[refNum],repLen);
                 nxt += repLen;  /* move new to end of replace string */
                 old += repLen - (oldOff-newOff);
                                 /* move old to end of expanded old string */
              }  /* if valid replacement */
           }  /* if a replacement requested */
           continue;                              /* CONTINUE */
        }  /* if an escaped character */
        else
        {   /* a non-escaped char; make further checks */
            if ( *old == COMMENT_CHAR )
            {
               *old = EOS;  /* end the string */
               continue;                          /* CONTINUE */
            }
            else if ( *old == QUOTE_CHAR )
            {
               if ( !inQuote) inQuote = True;  /* start quote */
               else inQuote = False;           /* end quote */
               old++;     /* go to next char */
               continue;                          /* CONTINUE */
            }
            else if ( !inQuote && isspace(*old) )
            {
               old++;     /* skip the space */
               continue;                          /* CONTINUE */
            }
        }   /* else non-escaped char */

        /* if survived all the checks, can advance to next char */
        nxt++;
        old++;
    }
}        /*$END$*/

/*========================================================*/
/*============== Private Xlate routines ==================*/
/*========================================================*/


#if DOC
/*========================================================*/
$PFUNBEG$: PrintDbEntry()
$1LINER$:  Prints a db entry to stdout
$SUMMARY$: 
$ARGS$:
quarks:  NULLQUARK-terminated list of quarks for the LHS of the entry
value:   value of the RHS of the entry
$RETURNS$:
/*================================================$SKIP$==*/
#endif
static
void PrintDbEntry(
       XrmQuarkList quarks,
       XrmValue *   value)
{        /*$CODE$*/
   char *   str;
   XrmQuark quark;

   /* print the entry */
   quark=*quarks;
   while ( quark != NULLQUARK )
   {
      str = XrmQuarkToString(quark);
      fprintf(stderr,"%s", str);
      quark = *(++quarks);
      if (quark != NULLQUARK) fprintf(stderr,".");
   }
   fprintf(stderr,":%s\n",value->addr);
}        /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: strCaseiCmp
$1LINER$:  case insensitive string comparison
$SUMMARY$: 
Rolled my own because strcasecmp() not available on
all platforms.
$ARGS$:
$RETURNS$:
True:  strings match
False: they dont
/*================================================$SKIP$==*/
#endif

static
Boolean strCaseiCmp(
       const char * str1,
       const char * str2)
{        /*$CODE$*/
   /* if ( !str1 || !str2 ) return False; *//* unneeded performance hit */
   while ( *str1 && *str2 )
     if ( tolower(*str1++) != tolower(*str2++) )  return False;
   return (*str1 == *str2);
}        /*$END$*/

#if DOC
/*========================================================*/
$PFUNBEG$: CheckForMatchall()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckForMatchall(
           __DtXlateSrchData * srchData, 
           const char *      matchallString,
           const char *      matchingString)
{        /*$CODE$*/
   int refNum = -1;
   int score = 0;

   /* test for a match all */
   if (! (   matchallString[0] == MATCHALL_CHAR 
          && (   matchallString[1] == EOS
              || sscanf(matchallString+1,"%d", &refNum) == 1) ) )
      return False;        /* RETURN: syntax error or non-matchall */

   /* matchall occurred; save the matching string if valid ref num */
   if (   refNum != -1 
       && refNum >= 0 
       && refNum < XtNumber(srchData->curSpecRefs) )
   {
      /* Don't store the string if it is just a matchall */
      /* This allows a replacement ref to be deleted on a match 
         for which there is no replacement value. */

      if (   matchingString[0] == MATCHALL_CHAR
          && matchingString[1] == EOS )
      {
         srchData->curSpecRefs[refNum] = NULL;
         /* don't add to score for a matchall with no replacement value */
      }
      else
      {
         /* recall: string not owned by curSpecRefs */
         srchData->curSpecRefs[refNum] = matchingString;
         score = 1;
      }
   } 
   else  /* not a value reference; just determine if a plain match */
   {
      if (   matchingString[0] == MATCHALL_CHAR
          && matchingString[1] == EOS )
          score = 1;      /* a plain matchall matches a plain matchall */
   }

   /* if a perfect matchall match, bump the score */
   srchData->curScore += score;

   return True;        /* RETURN: matchall */
}        /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchPlatformMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchPlatformMatchesSpec(
           __DtXlateSrchData * srchData, 
           XrmQuark          specPlatformQuark)
{        /*$CODE$*/
   /* CheckForMatchall stores away the matching string if of form ?n */
   if (srchData->platformQuark != NULLQUARK)
   {
      char * specStr = XrmQuarkToString(specPlatformQuark);
      /* CheckForMatchall incs score if appropriate */
      if (CheckForMatchall(srchData,specStr,srchData->platformStr) == True)
         return True;            /* RETURN: platform matches */
   }
   if (srchData->platformQuark == specPlatformQuark)
   {
      srchData->curScore += 2;   /* perfect match better than matchall match */
      return True;               /* RETURN: platform matches */
   }
   return False;                 /* RETURN: platform doesnt match */
}        /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchVerMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchVerMatchesSpec(
           __DtXlateSrchData * srchData, 
           XrmQuark          specVersionQuark)
{        /*$CODE$*/
   const char * numStr = XrmQuarkToString(specVersionQuark);
   int          lowerBnd = 0;
   int          upperBnd = INT_MAX;
   int          score = 0;

   if (   srchData->version == MATCHALL_VER
       || CheckForMatchall(srchData,numStr,srchData->verStr) == True )
      return True;          /* RETURN; matchall specified */

   /*** sscanf()-based parsing ***/
   /* note that the (score=x) is an assignment, not a compare */
   if (   (score=1) && sscanf(numStr,"%d-%d", &lowerBnd, &upperBnd) != 2
       && (score=1) && sscanf(numStr,"%d+", &lowerBnd) != 1
       && (score=2) && sscanf(numStr,"%d", &lowerBnd) != 1 )
       return False;        /* RETURN: syntax error */

   if ( lowerBnd > srchData->version || upperBnd < srchData->version )
       return False;        /* RETURN: version doesnt match */

   srchData->curScore += score;
   return True;             /* RETURN: version matches */
}        /*$END$*/



#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchOperMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
srchData:            state of the search
specOperationQuark:  quark for the operation specification string
$RETURNS$:
True:  if srchData->operation is found in specOperation string
       or the specOperation string is a match all
False: if not
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchOperMatchesSpec(
           __DtXlateSrchData * srchData, 
           XrmQuark          specOperationQuark)
{        /*$CODE$*/
   const char * opStr = XrmQuarkToString(specOperationQuark);
   int          hitLen;
   const char * hit;
   const char * remainingOps;

   if (   srchData->operStr == NULL
       || CheckForMatchall(srchData,opStr,srchData->operStr) == True
       || CheckForMatchall(srchData,srchData->operStr,opStr) == True )
      return True;          /* RETURN; matchall specified */

   /* quark compare search */
   if ( specOperationQuark == srchData->operQuark )
       goto matched;

   /*** strstr-based search ***/
   hitLen = srchData->operStrLen;
   remainingOps = opStr;
   do 
   {
      /* look for operation in remainingOps */
      hit = strstr(remainingOps,srchData->operStr);
      /* see if the hit is on a complete token */
      if (   NULL != hit
          && (hit == remainingOps || *(hit-1) == OPER_SEPARATOR)
          && (hit[hitLen] == EOS || hit[hitLen] == OPER_SEPARATOR) )
      {
matched:
         srchData->curScore += 2; /*perfect match better than a matchall match*/
         return True;             /* RETURN: operation matches */
      }
   } while(hit != NULL && *(remainingOps = hit+1) != EOS);

   return False;        /* RETURN: no match on operation */
}        /*$END$*/



#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchDirOpToStdMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchDirOpToStdMatchesSpec(
           __DtXlateSrchData * srchData, 
           XrmQuark          specDirectionQuark)
{        /*$CODE$*/
   int          score = 0;
   __DtXlateType type = __DtXLATE_TYPE_NONE;

   if (specDirectionQuark == srchData->inhibitQuark) 
       return False;  /* RETURN: no match */

   /* Note that the type and score expressions are assignments */
   if (! (   (   (type=__DtXLATE_TYPE_REGEX) 
              && (specDirectionQuark == srchData->lessThanQuark))
          || (   (type=__DtXLATE_TYPE_PURE)
              && (score=1) 
              && (specDirectionQuark == srchData->equalsToQuark)) 
          || (   (type=__DtXLATE_TYPE_CONTAINS)
              && (score=1) 
              && (specDirectionQuark == srchData->containsQuark)) ) )
       return False;        /* RETURN: no match */

   srchData->curScore += score;
   srchData->curTransType = type;
   return True;             /* RETURN: direction matches */
}        /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchDirStdToOpMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchDirStdToOpMatchesSpec(
           __DtXlateSrchData * srchData, 
           XrmQuark          specDirectionQuark)
{        /*$CODE$*/
   int          score = 0;
   __DtXlateType type = __DtXLATE_TYPE_NONE;

   if (specDirectionQuark == srchData->inhibitQuark) 
       return False;  /* RETURN: no match */

   /* Note that the type and score expressions are assignments */
   if (! (   (   (type=__DtXLATE_TYPE_REGEX) 
              && (specDirectionQuark == srchData->moreThanQuark))
          || (   (type=__DtXLATE_TYPE_PURE)
              && (score=1) 
              && (specDirectionQuark == srchData->equalsToQuark))
          || (   (type=__DtXLATE_TYPE_CONTAINS)
              && (score=1) 
              && (specDirectionQuark == srchData->containsQuark)) ) )
       return False;        /* RETURN: no match */

   srchData->curScore += score;
   srchData->curTransType = type;
   return True;             /* RETURN: direction matches */
}        /*$END$*/



#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchStdValueMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchStdValueMatchesSpec(
           __DtXlateSrchData * srchData, 
           XrmQuark *        specStdValueQuarks)
{        /*$CODE$*/
   int        score = 0;
   int        unmatched = 0;
   XrmQuark * patQuarks;

   /* walk through all available quarks */
   for ( patQuarks = srchData->stdValueQuarks;
         *specStdValueQuarks != NULLQUARK && *patQuarks != NULLQUARK;
         specStdValueQuarks++, patQuarks++ )
   { 
      char * specStr = XrmQuarkToString(*specStdValueQuarks);
      char * patStr = XrmQuarkToString(*patQuarks);
      if (    CheckForMatchall(srchData,specStr,patStr) == True 
           || CheckForMatchall(srchData,patStr,specStr) == True )
         continue;     /* no score for a matchall */

      /* is not exact match, match fails */
      /* be case insensitive when comparing standard values */
      if (    *patQuarks != *specStdValueQuarks
           && strCaseiCmp(specStr,patStr) == False )
          return False;              /* RETURN: no match */

      /* one more match--increase score, go to next */
      score++;
   }

   /* find out how many stdValue fields were left unmatched */
   for ( unmatched = 0;
         *specStdValueQuarks != NULLQUARK;
         specStdValueQuarks++ ) 
       { unmatched++; }

   /* Score is combo of the number matched - the number unmatched 
      and not counting the number matchalls that coincided with
      the search pattern.   This technique allows the spec for
      val1 to be at "better" match than the one for val2, and
      val2 to be a better match than val3, and val3 to be a better
      match than val4.
         .a.std     : val1          querypattern = a.std
         .?.std     : val2          querypattern = a.std
         .?1.std    : \\1val3       querypattern = a.std
         .a.std     : val4          querypattern = ?.std
         .?.std     : val5          querypattern = ?.std
         .?1.std    : \\1val6       querypattern = ?.std

         .a.std.?   : val4          querypattern = a.std
         .?.std.?   : val5          querypattern = a.std
         .?.std.?1  : \\1val5       querypattern = a.std
         .?1.std.?  : \\1val5       querypattern = a.std
         .?1.std.?2 : \\1\\2val5    querypattern = a.std
         .a.std.?   : val4          querypattern = a.std.b
         .?.std.?   : val5          querypattern = a.std.b
         .?.std.?1  : \\1val5       querypattern = a.std.b
         .?1.std.?  : \\1val5       querypattern = a.std.b
         .?1.std.?2 : \\1\\2val5    querypattern = a.std.b
   */
   srchData->curScore += score + MAXSTDQUARKS - unmatched;

   return True;             /* RETURN: direction matches */
}        /*$END$*/



#if DOC
/*========================================================*/
$PFUNBEG$: CheckSearchOpValueMatchesSpec()
$1LINER$:  Matches search pattern to spec data
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Boolean CheckSearchOpValueMatchesSpec(
           __DtXlateSrchData * srchData, 
           const char *        specOpValue)
{        /*$CODE$*/
    char       opValBuf[MAXRHSSIZE]; /* max supported RHS size */
    char *     pOpValBuf = opValBuf;  /* need this for StripMeta... call */
    size_t     opValLen;
    Boolean    matches = False;

    /* copy value to mutable memory */
    strncpy(opValBuf,specOpValue,sizeof(opValBuf));
    opValBuf[sizeof(opValBuf)-1] = EOS;
    opValLen = strlen(opValBuf);

    /* depending on the translation type of the spec, do a 
       regexex match of the spec value pattern to the search 
       value or do a pure match */
    if (srchData->curTransType == __DtXLATE_TYPE_REGEX)
    {
#if defined(sun) || defined(USL) || defined(__uxp__)
        char *     ex = NULL;

        /* True: leave escape char in place */
        StripMetaAndReplaceEscSubex(&pOpValBuf,True,NULL);

        /* we need to use regexex to pattern match */
        /* and we need to save of the reference value matches */
        if (   (ex = compile(opValBuf,NULL,NULL)) != NULL
            && advance(srchData->opValue,ex) != 0)
        {
            int matchSize;
            int subExCnt = nbra;  /* Sun global for advance() */
            matches = True;       /* if got this far */

            /* need due to bug in advance()--operation doesn't meet documentation */
            if (NULL == loc1) loc1=(char *)srchData->opValue;

            /* inc score by the size of the match after 
               scaling for the maximum possible match size */
            matchSize = loc2 - loc1; /*loc[12] are Sun globals for advance()*/
            if (matchSize < 0 || matchSize >= sizeof(opValBuf)) matchSize = 0;

            /* NOTE: this scoring code should be identical in the 
               Sun-specific and non-Sun code blocks */
            if (matchSize == strlen(srchData->opValue))
            {
               /* if the matchSize is the length of srchData->opValue,
                  then we have a complete match.  In this case, use the
                  specificity of the pattern to pick the best match */
               /* NOTE: opValLen is a crude measure of specificity.  
                  A better measure would be to count the number of
                  literals/ranges that matched exactly.  When doing this,
                  a perfect match without regex syntax should rank higher
                  than a perfect match with regex syntax.  This is one
                  area where the current algorithm breaks.  For example:
                     opValue=23, pat1=23, pat2=[0-9]3.
                  Both patterns match and pat1 is a better match,
                  but not with the current length-based algorithm. */
               /* NOTE: this formula does not advance the score
                  to sizeof(opValBuf) for a perfect match.  Other match 
                  formulas use sizeof(opValBuf) as the max value
                  to indicate a perfect match. */
               srchData->curScore += matchSize + opValLen;
            }
            else
            {
               /* if its not a complete match, inc score by match size */
               srchData->curScore += matchSize;
            }

            /* put sub expression matching stuff in srchData->curSubEx */
            for( ; nbra > 0; nbra-- )
            {
                srchData->curSubEx[nbra].rm_so = braslist[nbra-1] - loc1;
                srchData->curSubEx[nbra].rm_eo = braelist[nbra-1] - loc1;
            }
        }
        if (ex) free(ex);
#else
        regex_t    re;

        /* True: leave escape char in place */
        StripMetaAndReplaceEscSubex(&pOpValBuf,True,NULL);

        /* we need to use regexex to pattern match */
        /* and we need to save of the reference value matches */
        if (   regcomp(&re,opValBuf,0) == 0
            && regexec(&re,srchData->opValue,
                   XtNumber(srchData->curSubEx),srchData->curSubEx,0) == 0)
        {
            int matchSize;
            matches = True;       /* if got this far */

            /* inc score by the size of the match after 
               scaling for the maximum possible match size */
            matchSize = srchData->curSubEx[0].rm_eo - 
                                               srchData->curSubEx[0].rm_so;

            /* NOTE: this scoring code should be identical in the 
               Sun-specific and non-Sun code blocks */
            if (matchSize == strlen(srchData->opValue))
            {
               /* if the matchSize is the length of srchData->opValue,
                  then we have a complete match.  In this case, use the
                  specificity of the pattern to pick the best match */
               /* NOTE: opValLen is a crude measure of specificity.  
                  A better measure would be to count the number of
                  literals/ranges that matched exactly.  When doing this,
                  a perfect match without regex syntax should rank higher
                  than a perfect match with regex syntax.  This is one
                  area where the current algorithm breaks.  For example:
                     opValue=23, pat1=23, pat2=[0-9]3.
                  Both patterns match and pat1 is a better match,
                  but not with the current length-based algorithm. */
               /* NOTE: this formula does not advance the score
                  to sizeof(opValBuf) for a perfect match.  Other match 
                  formulas use sizeof(opValBuf) as the max value
                  to indicate a perfect match. */
               srchData->curScore += matchSize + opValLen;
            }
            else
            {
               /* if its not a complete match, inc score by match size */
               srchData->curScore += matchSize;
            }

            /* sub expression matching stuff already in srchData->curSubEx */
        }
        regfree(&re);
#endif
    }
    else /* (srchData->curTransType == __DtXLATE_TYPE_PURE || __DtXLATE_TYPE_CONTAINS */
    {
        char * opValueInBuf;

        /* False: strip escape char as well */
        StripMetaAndReplaceEscSubex(&pOpValBuf,False,NULL);
        matches = (strcmp(srchData->opValue,opValBuf) == 0);

        /* if matches, inc score to show a perfect match (max poss value) */
        if (matches) srchData->curScore += sizeof(opValBuf);

        /* don't test for contains if a perfect match or a pure match spec */
        if (matches || srchData->curTransType == __DtXLATE_TYPE_PURE)
           return matches;                            /* RETURN */

        /*  (srchData->curTransType == __DtXLATE_TYPE_CONTAINS) */
        /* is opValue contained in opValBuf? */
        /* is opValBuf contained in opValue? */
        opValueInBuf = NULL;
        matches =    (opValBuf[0] != EOS && srchData->opValue[0] != EOS) 
                  && ((opValueInBuf=strstr(opValBuf,srchData->opValue)) != NULL
                      || strstr(srchData->opValue,opValBuf) != NULL);

        /* if matches, inc score to show a contains match */
        if (matches) 
        {
           if (opValueInBuf) srchData->curScore += strlen(srchData->opValue);
           else srchData->curScore += strlen(opValBuf);
        }
    }
    return matches;
}        /*$END$*/




#if DOC
/*========================================================*/
$PFUNBEG$: FindStdToOpMatchCB()
$1LINER$:  Matches std value of entry to search pattern; gets op value
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Bool FindStdToOpMatchCB(
            XrmDatabase *       database,
            XrmBindingList      bindings,
            XrmQuarkList        quarks,
            XrmRepresentation * type,
            XrmValue *          value,
            XPointer            client_data)
{       /*$CODE$*/
   __DtXlateSrchData * srchData = (__DtXlateSrchData *) client_data;

   /* always begin scoring from 0 and replacement values at NULL */
   srchData->curScore = 0;
   memset(srchData->curSpecRefs,0, sizeof(srchData->curSpecRefs));

#ifdef DBG_MATCHING
   fprintf(stderr,"FindStdToOpMatch: "); PrintDbEntry(quarks,value);
#endif

   /* check for a match */
   if ( CheckSearchPlatformMatchesSpec(srchData, 
                      quarks[PLATFORM_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"platform matches\n");
#endif

   if ( CheckSearchVerMatchesSpec(srchData, 
                      quarks[VERSION_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"ver matches\n");
#endif

   if ( CheckSearchOperMatchesSpec(srchData, 
                      quarks[OPERS_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"oper matches\n");
#endif

   if ( CheckSearchDirStdToOpMatchesSpec(srchData, 
                      quarks[DIRECTION_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"kind matches\n");
#endif

   /* now check for std value match and, if it is
      the best match so far, record the value */
   if ( CheckSearchStdValueMatchesSpec(srchData, 
                       &quarks[FIRSTSTD_QUARK]) == False )
   {
      if (srchData->db->debugMode)
      {
         fprintf(stderr,"mismatch ");
         PrintDbEntry(quarks,value);
      }
   
      return False;      /* continue enumeration */
   }

#ifdef DBG_MATCHING
   fprintf(stderr,"std value matches\n");
#endif

   if (srchData->db->debugMode)
   {
      fprintf(stderr,"match (%d) ",srchData->curScore);
      PrintDbEntry(quarks,value);
   }
   
   /* we have a match! (we made it through all match checks) */
   /* is it better than or same as any earlier match? */
   if ( srchData->curScore >= srchData->bestScore )
   {
      /* recall that all strings are owned by Xrm==>no need to free them */
      srchData->bestScore = srchData->curScore;
      srchData->bestTransType = srchData->curTransType;
      memcpy(srchData->bestSpecRefs,srchData->curSpecRefs,
                sizeof(srchData->bestSpecRefs)); /* no array assignment in C */
      srchData->opValue = value->addr;
   }

   return False;    /* continue enumeration */
}       /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: FindOpToStdMatchCB()
$1LINER$:  Matches op value of entry to search pattern; gets std value
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif

static
Bool FindOpToStdMatchCB(
            XrmDatabase *       database,
            XrmBindingList      bindings,
            XrmQuarkList        quarks,
            XrmRepresentation * type,
            XrmValue *          value,
            XPointer            client_data)
{       /*$CODE$*/
   __DtXlateSrchData * srchData = (__DtXlateSrchData *) client_data;

   /* always begin scoring from 0 and subexpression indices at -1 */
   srchData->curScore = 0;
   memset(srchData->curSubEx,-1, sizeof(srchData->curSubEx));

#ifdef DBG_MATCHING
   fprintf(stderr,"FindOpToStdMatch: "); PrintDbEntry(quarks,value);
#endif

   /* check for a match */
   if ( CheckSearchPlatformMatchesSpec(srchData, 
                      quarks[PLATFORM_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"platform matches\n");
#endif

   if ( CheckSearchVerMatchesSpec(srchData, 
                      quarks[VERSION_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"ver matches\n");
#endif

   if ( CheckSearchOperMatchesSpec(srchData, 
                      quarks[OPERS_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"oper matches\n");
#endif

   if ( CheckSearchDirOpToStdMatchesSpec(srchData, 
                      quarks[DIRECTION_QUARK]) == False) 
      return False;      /* continue enumeration */

#ifdef DBG_MATCHING
   fprintf(stderr,"kind matches\n");
#endif

   /* now check for op value match and, if it is
      the best match so far, record the std value */
   if ( CheckSearchOpValueMatchesSpec(srchData, 
                                   value->addr) == False )
   {
      if (srchData->db->debugMode)
      {
         fprintf(stderr,"mismatch ");
         PrintDbEntry(quarks,value);
      }
   
      return False;      /* continue enumeration */
   }

#ifdef DBG_MATCHING
   fprintf(stderr,"op value matches\n");
#endif

   if (srchData->db->debugMode)
   {
      fprintf(stderr,"match (%d) ",srchData->curScore);
      PrintDbEntry(quarks,value);
   }

   /* we have a match! (we made it through all match checks) */
   /* is it better than or same as any earlier match? */
   if ( srchData->curScore >= srchData->bestScore )
   {
      XrmQuarkList stdQ;
      XrmQuarkList curQ;
      /* recall that all strings are owned by Xrm==>no need to free them */
      srchData->bestScore = srchData->curScore;
      srchData->bestTransType = srchData->curTransType;
      memcpy(srchData->bestSubEx,srchData->curSubEx,
                sizeof(srchData->bestSubEx)); /* no array assignment in C */
      /* store off the std value of the best match */
      stdQ = srchData->stdValueQuarks;
      curQ = &quarks[FIRSTSTD_QUARK];
      while ( (*stdQ = *curQ) != NULLQUARK ) stdQ++, curQ++;
   }

   return False;    /* continue enumeration */
}       /*$END$*/


#if DOC
/*========================================================*/
$PFUNBEG$: DoCommonSrchDataPrep
$1LINER$:  Prep srchData to search for a pattern
$SUMMARY$: 
$ARGS$:
$RETURNS$:
/*================================================$SKIP$==*/
#endif
static
void DoCommonSrchDataPrep(
      __DtXlateSrchData * srchData,
      _DtXlateDb          db,
      const char *        platform,
      const int           version,
      const char *        operation)
{       /*$CODE$*/
   int       verNum = version;       /* for lint */

   /* zero the search data */
   memset(srchData,0,sizeof(__DtXlateSrchData));

   /* set the db */
   srchData->db = db;
   
   /* build filter list for enumerating the db */
   if (verNum < MATCHALL_VER) verNum = MATCHALL_VER;
   srchData->platformStr = platform;
   srchData->platformQuark = (platform?XrmStringToQuark(platform):NULLQUARK);
   srchData->version = verNum;
   sprintf(srchData->verStr,"%d",verNum);
   srchData->operStr = operation;
   srchData->operStrLen = strlen(operation);
   srchData->operQuark = (operation ? XrmStringToQuark(operation) : NULLQUARK);
   srchData->lessThanQuark = XrmStringToQuark(LESSTHAN_STR);
   srchData->equalsToQuark = XrmStringToQuark(EQUALS_STR);
   srchData->containsQuark = XrmStringToQuark(CONTAINS_STR);
   srchData->moreThanQuark = XrmStringToQuark(MORETHAN_STR);
   srchData->inhibitQuark = XrmStringToQuark(INHIBIT_STR);
}       /*$END$*/

/*========================================================*/
/*=============== Public Xlate routines ==================*/
/*========================================================*/



#if DOC
/*========================================================*/
$FUNBEG$:  _DtXlateOpenDb()
$1LINER$:   Open a translation database
$SUMMARY$: 
Opens a translation resource database and returns a
reference to it in ret_db.

Initializes the _DtXlateDb object to ready for use.
If an error occurs, _DtXlateDb is set to NULL.
$ARGS$:
$RETURNS$:
0:  no error occurred
-1: if ret_db is NULL
    if XrmGetFileDatabase() failed on databaseName
    if malloc fails to alloc a db structure
/*================================================$SKIP$==*/
#endif

int  _DtXlateOpenDb(
       const char *  databaseName,
       _DtXlateDb *  ret_db)
{       /*$CODE$*/
    XrmDatabase xrmDb;
    __DtXlateDbRec * dbRec = NULL;
    char * path = NULL;

    if(NULL == ret_db) return -1;        /* RETURN */
    *ret_db = NULL;

    /* Do NOT check for whether *ret_db is already
       a valid db; this is none of our affair. */

    /* get an absolute path for the file */
    path = ExpandPath(databaseName);

    if (NULL == path) return -1;        /* RETURN */

    xrmDb = XrmGetFileDatabase(path);
    if (NULL == xrmDb) { free(path); return -1; } /* RETURN */

    /* alloc a db ref */
    dbRec = (__DtXlateDbRec *) calloc(1,sizeof(__DtXlateDbRec));
    if(NULL == dbRec) 
    {
       XrmDestroyDatabase(xrmDb); 
       free(path);
       return -1;                        /* RETURN */
    }

    /* and populate it */
    dbRec->xrmDb = xrmDb;
    dbRec->initGuard = INIT_OCCURRED;

    /* check for debug mode */
    SetDebugModeState(dbRec);

    if (dbRec->debugMode) 
       fprintf(stderr,"_DtXlateOpenDb: opened: %s; new db: %p\n",path, (void *)dbRec);

    *ret_db = dbRec;
    free(path);
    return 0;       /* RETURN */
}       /*$END$*/

#if DOC
/*========================================================*/
$FUNBEG$: _DtXlateOpenAndMergeDbs()
$1LINER$: Opens a translation database and merges with earlier dbs
$SUMMARY$: 
Opens a translation resource database and returns a
reference to it in ret_db.

Initializes the _DtXlateDb object to ready for use.
$ARGS$:
$RETURNS$:
0:  no error occurred
-1: if io_db is NULL
    if XrmGetFileDatabase() failed on databaseName
/*================================================$SKIP$==*/
#endif

int  _DtXlateOpenAndMergeDbs(
       const char *   databaseName,
       _DtXlateDb *   io_db)
{       /*$CODE$*/
    XrmDatabase xrmDb;
    char * path;
    int  ret;

    if(NULL == io_db) return -1;                /* RETURN */

    /* if a db has not yet been opened */
    if(   NULL == *io_db
       || (*io_db)->initGuard != INIT_OCCURRED 
       || (*io_db)->xrmDb == NULL)
    {
       ret = _DtXlateOpenDb(databaseName,io_db); /* RETURN */
       if ( (*io_db) && (*io_db)->debugMode) 
          fprintf(stderr,"_DtXlateOpenAndMergeDb: "
                      "used _DtXlateOpenDb to open first file\n");
       return ret;      /* RETURN */
    }
       
    if ( (*io_db) && (*io_db)->debugMode) 
       fprintf(stderr,"_DtXlateOpenAndMergeDb: "
                      "target file: %s; existing db: %p\n",databaseName, (void *) *io_db);

    /* a db has been opened, let's merge with it */

    /* get an absolute path for the file */
    path = ExpandPath(databaseName);

    if (NULL == path) goto Failed;     /* RETURN */

    xrmDb = XrmGetFileDatabase(path);
    if (NULL == xrmDb) goto Failed;    /* RETURN */

    /* merge and destroy xrmDb for me */
    XrmMergeDatabases(xrmDb,&(*io_db)->xrmDb);

    /* check for debug mode */
    SetDebugModeState(*io_db);

    if ((*io_db)->debugMode) 
       fprintf(stderr,"_DtXlateOpenAndMergeDb: "
                      "opened: %s; merged db: %p\n",path, (void *) *io_db);

    free(path);
    return 0;      /* RETURN */

Failed:
    if ( (*io_db) && (*io_db)->debugMode) 
       fprintf(stderr,"_DtXlateOpenAndMergeDb: open failed; file: %s\n", 
          (path ? path : (databaseName ? databaseName : "NULL") ) );
    if (path) free(path);
    return -1;      /* RETURN */
}       /*$END$*/

#if DOC
/*========================================================*/
$FUNBEG$: _DtXlateMergeDbs()
$1LINER$: Merges two open dbs into one and closes the merged-in db.
$SUMMARY$: 
Merges two databases into one and closes the merged db.

The io_dbToMerge database must be a valid translation database.  
The io_dbToMerge database is merged into the io_mergeIntoDb.  
The io_mergeIntoDb may either be invalid or valid.  If invalid,
the io_dbToMerge database is simply moved over to io_mergeIntoDb.
If io_mergeIntoDb is valid, the entries in the io_dbToMerge
database are merged into it and take precedence over entries in the
io_mergeIntoDb, and the io_dbToMerge database is closed.
$ARGS$:
io_dbToMerge:    database to merge into io_mergeIntoDb
io_mergeIntoDb:  database to hold merged result
$RETURNS$:
0:  no error occurred
-1: if io_dbToMerge or io_mergeIntoDb is NULL
    if *io_dbToMerge is NULL or uninitialized
/*================================================$SKIP$==*/
#endif

int  _DtXlateMergeDbs(
       _DtXlateDb *  io_dbToMerge,
       _DtXlateDb *  io_mergeIntoDb)
{       /*$CODE$*/

    /* check args */
    if(   NULL == io_mergeIntoDb 
       || NULL == io_dbToMerge
       || NULL == *io_dbToMerge
       || (*io_dbToMerge)->initGuard != INIT_OCCURRED 
       || (*io_dbToMerge)->xrmDb == NULL)
       return -1;                             /* RETURN */

    /* check for debug mode */
    if (   ((*io_mergeIntoDb) && (*io_mergeIntoDb)->debugMode) 
        || (*io_dbToMerge)->debugMode) 
       fprintf(stderr,"_DtXlateMergeDbs: "
                  "mergeIntoDb: %p; dbToMerge: %p\n", (void *) *io_mergeIntoDb, (void *) *io_dbToMerge);

    /* if db_mergeIntoDb has not yet been opened */
    if(   NULL == *io_mergeIntoDb
       || (*io_mergeIntoDb)->initGuard != INIT_OCCURRED 
       || (*io_mergeIntoDb)->xrmDb == NULL)
    {
       /* just move dbToMerge into mergeIntoDb */
       *io_mergeIntoDb = *io_dbToMerge;
       DeleteDbMem(io_dbToMerge);

       return 0;				/* RETURN */
    }
       
    /* merge and destroy io_dbToMerge->xrmDb for me */
    XrmMergeDatabases((*io_dbToMerge)->xrmDb,&(*io_mergeIntoDb)->xrmDb);
    DeleteDbMem(io_dbToMerge);

    /* check for debug mode */
    SetDebugModeState(*io_mergeIntoDb);

    if ((*io_mergeIntoDb)->debugMode) 
       fprintf(stderr,"merged db: %p\n", (void *) *io_mergeIntoDb);

    return 0;
}       /*$END$*/

#if DOC
/*========================================================*/
$FUNBEG$: _DtXlateOpenAllDbs()
$1LINER$: Open and merge all locale translation databases that can be found
$SUMMARY$:
DtXlateOpenAllDbs() locates all translation databases
present in the search paths directories.
$ARGS$:
searchPaths:  ':' separated list of directories
databaseName: name of the database file in those directories
ret_db:       the reference to the open database is stored here
$RETURNS$:
 0: at least one database was opened
-1: no database was opened
/*================================================$SKIP$==*/
#endif

int  _DtXlateOpenAllDbs(
         const char * searchPaths,
         const char * databaseName,
         _DtXlateDb * ret_db)
{       /*$CODE$*/
    const char * workStart = searchPaths;
    const char * separator = NULL;
    char         dbFile[MAXPATHLEN+1];
    int          ret = ~0;    /* all bits set */

    /* cycle through the paths, opening each one */
    do
    {
       int  workLen = 0;
       const char * slash = NULL;

       dbFile[0] = EOS;

       /* isolate the next part of the path */
       _DtMBStrchr (workStart, PATH_SEPARATOR, -1, &separator);
       if (NULL == separator) _DtMBStrchr (workStart, EOS, -1, &separator);
       if (NULL == separator) break;           /* BREAK */
       workLen = separator - workStart;  /* dont include +1 for EOS */

       /* copy over the path component */
       strncpy(dbFile,workStart,workLen);
       workStart = separator + 1;

       /* add a slash to end of path component, if needed */
       *(dbFile+workLen) = EOS;  /* add an EOS for _DtMBStrrchr to find */
       _DtMBStrrchr(dbFile,DIR_SLASH,-1,&slash);
       if (slash != dbFile+workLen-1) /* is slash last char of path? */
       {
         *(dbFile+workLen) = DIR_SLASH;
         workLen++;
       }

       /* append the filename and EOS */
       strcpy(dbFile+workLen,databaseName);

/*printf("Working on: %s\n", dbFile);  **DBG*/

       /* open and merge the database with previously opened dbs */
       /* by ANDing, we determine whether at least one call returned 0 */
       ret &= _DtXlateOpenAndMergeDbs(dbFile,ret_db);

    } while ( *separator != EOS );

    if (*ret_db && (*ret_db)->debugMode) 
       fprintf(stderr,"_DtXlateOpenAllDbs: completed\n"
                      "  srchpaths: %s; db file: %s\n",searchPaths,databaseName);

    return (ret == 0 ? 0 : -1);  /* ret != 0 ==> no db was opened */
}       /*$END$*/


#if DOC
/*========================================================*/
$FUNBEG$: _DtXlateCloseDb()
$1LINER$: Close an open translation database
$SUMMARY$:
_DtXlafteCloseDb() releases all memory associated with
the translation database.  Further use of the database
object is an error.
$ARGS$:
$RETURNS$:
 0:  database was valid and has been closed
-1:  invalid database pointer
/*================================================$SKIP$==*/
#endif

int _DtXlateCloseDb(
       _DtXlateDb * io_db)
{       /*$CODE$*/
     __DtXlateDbRec * dbRec;
     
     if(   NULL == io_db 
        || NULL == (dbRec = *io_db)        /* dbRec assigned */
        || dbRec->initGuard != INIT_OCCURRED) 
        return -1;                     /* RETURN */
 
     XrmDestroyDatabase(dbRec->xrmDb);

     if (dbRec->debugMode) fprintf(stderr,"_DtXlateCloseDb: %p\n", (void *) dbRec);

     /* zero out object mem and free it */
     DeleteDbMem(io_db);

     return 0;
}       /*$END$*/


#if DOC
/*========================================================*/
$FUNBEG$: _DbXlateStdToOpValue()
$1LINER$:  Translates a standardized spec to an operation-specific value
$SUMMARY$: 
Looks up the best translation of the standard value for an
operation and places a pointer to the translation string
at the location pointed to by ret_opValue.

The translated string was allocated using malloc() and
must be freed when no longer needed.

If ret_opValue is NULL, the function merely verifies that
a valid translation exists.
$ARGS$:
db:		a translation database
platform:	the platform string (see _DtXlateGetXlateEnv())
version:	the version number (see _DtXlateGetXlateEnv())
operation:	the operation of interest, e.g. "setlocale"
stdValue:	the standard value pattern
ret_opValue:	location where ptr to translated string is stored
ret_reserved:	reserved for future use
$RETURNS$:
 0: translation found
-1: invalid database (NULL ptr, not opened)
    no operation was specified
    query failed to find a match
/*================================================$SKIP$==*/
#endif

int _DtXlateStdToOpValue(
       _DtXlateDb        db,
       const char *      platform,
       const int         version,
       const char *      operation,
       const char *      stdValue,
       char * *          ret_opValue,
       void *            ret_reserved)
{       /*$CODE$*/
   __DtXlateSrchData srchData;
   XrmQuark  empty = NULLQUARK;

   if (   NULL == db 
       || db->initGuard != INIT_OCCURRED
       || NULL == operation
       || operation[0] == EOS)
       return -1;                                   /* RETURN error */

   /* prep srch data for search */
   DoCommonSrchDataPrep(&srchData,db,platform,version,operation);

   /* handle a rare case */
   if (NULL == stdValue)
   {
       if (db->debugMode) fprintf(stderr,"_DtXlateStdToOpValue: NULL std value\n");
       if (ret_opValue) *ret_opValue = NULL;
       return -1;                                  /* RETURN error */
   }

   /* build std value list for use during comparison */
   srchData.stdValueQuarks[0] = NULLQUARK;
   if (NULL != stdValue && stdValue[0] != EOS)
      XrmStringToQuarkList(stdValue,srchData.stdValueQuarks);

   if (db->debugMode)
      fprintf(stderr,"_DtXlateStdToOpValue: %s.%d.%s.%s: <op>\n",
          platform,version,operation,stdValue);

   /* scan through this Db looking for matches and put in search */
   XrmEnumerateDatabase(db->xrmDb,&empty,&empty,
              XrmEnumAllLevels, FindStdToOpMatchCB, (XPointer) &srchData);

   if (   srchData.opValue != NULL
       && srchData.bestTransType != __DtXLATE_TYPE_INHIBIT )
   {
       char * opValue;
       
       if (ret_opValue == NULL)
       {
          if (db->debugMode) fprintf(stderr,"translation exists\n");
          return 0;  /* RETURN: translation exists */
       }
       
       /* alloc the string to return */
       opValue = strdup(srchData.opValue);

       if (db->debugMode) fprintf(stderr,"raw opval:%s\n",opValue);

       /* do quote and escape removal and ref replacement in the opValue */
       if ( srchData.bestTransType == __DtXLATE_TYPE_REGEX )
          StripMetaAndReplaceEscSubex(&opValue,False,srchData.bestSpecRefs);
       else
          StripMetaAndReplaceEscSubex(&opValue,False,NULL);

       if (db->debugMode) fprintf(stderr,"op value:%s\n",opValue);

       *ret_opValue = opValue;
       return 0;                              /* RETURN: search successful */
   }
   return -1;                                /* RETURN: search failed */
}       /*$END$*/


#if DOC
/*========================================================*/
$FUNBEG$: _DbXlateOpToStdValue()
$1LINER$:  Translates an operation-specific value to a standardized one
$SUMMARY$: 
Looks up the best translation of the operation value for an
operation and places a pointer to the standard string
at the location pointed to by ret_stdValue.

The standard string was allocated using malloc() and
must be freed when no longer needed.

If ret_stdValue is NULL, the function merely verifies that
a valid translation exists.
$ARGS$:
db:		a translation database
platform:	the platform string (see _DtXlateGetXlateEnv())
version:	the version number (see _DtXlateGetXlateEnv())
operation:	the operation of interest, e.g. "setlocale"
opValue:	the operation-specific value pattern
ret_stdValue:	location where ptr to standard string is stored
ret_reserved:	reserved for future use
$RETURNS$:
 0: translation found
-1: invalid database (NULL ptr, not opened)
    no operation was specified
    query failed to find a match
/*================================================$SKIP$==*/
#endif

int _DtXlateOpToStdValue(
       _DtXlateDb        db,
       const char *      platform,
       const int         version,
       const char *      operation,
       const char *      opValue,
       char * *          ret_stdValue,
       void *            ret_reserved)
{       /*$CODE$*/
   __DtXlateSrchData srchData;
   XrmQuark  empty = NULLQUARK;
   char      lhs[MAXLHSSIZE];

   if (   NULL == db 
       || db->initGuard != INIT_OCCURRED
       || NULL == operation
       || operation[0] == EOS)
       return -1;                                   /* RETURN error */

   /* prep srch data for search */
   DoCommonSrchDataPrep(&srchData,db,platform,version,operation);

   /* after check on value, store op value for use during comparison */
   /* not meaningful to check for a NULL value */
   if (NULL == opValue)
   {
      if (db->debugMode) fprintf(stderr,"_DtXlateOpToStdValue: NULL op value\n");
      if (ret_stdValue) *ret_stdValue = NULL;
      return -1;                                   /* RETURN error */
   }
   srchData.opValue = opValue;

   if (db->debugMode)
      fprintf(stderr,"_DtXlateOpToStdValue: %s.%d.%s.<std>: %s\n",
          platform,version,operation,opValue);

   /* scan through this Db looking for matches and put in search */
   XrmEnumerateDatabase(db->xrmDb,&empty,&empty,
              XrmEnumAllLevels, FindOpToStdMatchCB, (XPointer) &srchData);

   if (   srchData.stdValueQuarks[0] != NULLQUARK
       && srchData.bestTransType != __DtXLATE_TYPE_INHIBIT )
   {
      XrmQuarkList stdQ;
      char * stdValue = lhs;
      int    stdValueLen = 0;

      if (ret_stdValue == NULL)
      {
         if (db->debugMode) fprintf(stderr,"translation exists\n");
         return 0;  /* RETURN: translation exists */
      }

      /* make a STDVALUE_SEPARATOR separated string out of the std data */
      for ( stdQ = srchData.stdValueQuarks;
            *stdQ != NULLQUARK;
            stdQ++ )
      {
         const char * str = XrmQuarkToString(*stdQ);
         int strLen;
         if (NULL == str || str[0] == EOS) continue;
         
         strLen = strlen(str);
         if (stdValue != lhs) 
             {*stdValue++ = STDVALUE_SEPARATOR; stdValueLen++; }
         if ((stdValueLen + strLen) > sizeof(lhs) )
             strLen = sizeof(lhs)-stdValueLen;
         strncpy(stdValue,str,strLen);
         stdValueLen += strLen;
         stdValue += strLen;
      }
      *stdValue = EOS;

      if (db->debugMode) fprintf(stderr,"raw stdval:%s\n",lhs);

      /* do quote and escape removal and ref replacement in the stdValue */
      stdValue = strdup(lhs);        /* reset stdValue ptr */
      if ( srchData.bestTransType == __DtXLATE_TYPE_REGEX )
          ReplaceMatchallSubex(&stdValue,srchData.bestSubEx,srchData.opValue);

      if (db->debugMode) fprintf(stderr,"std value:%s\n",stdValue);

      *ret_stdValue = stdValue;
      return 0;                              /* RETURN: search successful */
   }
   return -1;                                /* RETURN: search failed */
}       /*$END$*/



#if DOC
/*========================================================*/
$FUNBEG$: _DtXlateGetXlateEnv()
$1LINER$: Get the DtXlate compilation and execution environments.
$SUMMARY$:
_DtXlateGetXlateEnv() recovers the identity of the application
current platform, the version value of the application
execution environment, and the version value of the operating
system version for which DtXlateGetXlateEnv() was
compiled. These values can be used in formulating queries,
especially for the _DtXlateStdToOpValue() query.

The technique used by this routine is as follows.  Using
uname(2), the routine retrieves the sysname, release, and version
strings.  The sysname is used as the platform name.  The
release and version strings are concatenated in that order
and as treated below as the <op-rel-ver>.  An OpToStd translation
looks for a match to
    <sysname>.?.version.<.<std-version>: <op-rel-ver>
 or
    <sysname>.?.version.=.<std-version>: <op-rel-ver>

If no match is found, the next fallback position is to
get the specification with the same sysname and the highest
std-version integer value.  If no specifications exist for
that sysname, then sysname is set to the empty string and
std-version is -1.

ret_AppExecEnvPlatform should point to a character array at least 
_DtPLATFORM_MAX_LEN characters long.  The sysname is copied to it.

ret_AppExecEnvVersion is given the integer value of the
std-version recovered from the translation.

ret_XlateCompiledForOSVersion is given the integer value
determined by using the OSMajorVersion and OSMinorVersion
constants defined by the build environment of _DtXlate as follows:

#define _STR(V) #V
#define STR(V) _STR(V)
      sprintf(buf,"%s%s%s", STR(OSMAJORVERSION), 
                     nl_langinfo(RADIXCHAR), STR(OSMAJORVERSION));
      verNum = (int) (100.0 * atof(buf));

For example:
OSMAJORVERSION & OSMINORVERSION are compile-time constants
that must be defined as part of the build environment.
It is assumed that these constants are of the form:

   e.g. Sun 5.3
   #define OSMAJORVERSION        5
   #define OSMINORVERSION        3
   
   530 = (int) 100.0 * atof("5.3");
   
   e.g. HP-UX 8.09
   #define OSMAJORVERSION         8
   #define OSMINORVERSION         09
   
   809 = (int) 100.0 * atof("8.09");
   
Note that it may be necessary for the application to determine
the version number of an operation in some platform-specific
and operation-specific manner, for example using a library
version value.  In many cases, however, the O.S. version value 
for which _DtXlate was compiled will be sufficient
when identifying version numbers for standard development
environment libraries, such as libc.

$EXAMPLE$:
char * 
xlateStd2Op(_DtXlateDb db,Boolean runtimeOp, char * operation,char * stdVal)
{
   char platform[_DtPLATFORM_MAX_LEN];
   int  version;
   char * opVal = NULL;
   int    ret;

   if (runtimeOp)
      ret=_DtXlateGetXlateEnv(db,platform,&version,NULL);
   else
      ret=_DtXlateGetXlateEnv(db,platform,NULL,&version);
   if (ret == 0)
      _DtXlateStdToOpValue(db,platform,version,operation,stdVal,&opVal,NULL);
   return opVal;       /* will be NULL if error occurred */
}

$ARGS$:
ret_AppExecEnvPlatform:  pts to a string at least _DtPLATFORM_MAX_LEN long
                         that will hold the string uname(2) returns for sysname
ret_AppExecEnvVersion:   pts to an integer that will receive the platform
                         standardized version number, as determined by
                         a translation on uname(2) release+version.
ret_XlateCompiledForOSVersion:   pts to an integer that will receive the
                         operating system version for which _DtXlate was
                         compiled using OSMajorVersion * 100 + OSMinorVersion
Any of the arguments may be NULL.
$RETURNS$:
 0: if no error occurred
-1: if no translation was possible to get the AppExecEnvVersion
/*================================================$SKIP$==*/
#endif

int _DtXlateGetXlateEnv(
         _DtXlateDb db,
         char *     ret_AppExecEnvPlatform,
         int *      ret_AppExecEnvVersion,
         int *      ret_XlateCompiledForOSVersion)
{       /*$CODE$*/
   struct utsname names;
   int    ret = 0;
   char * platform = "NULL";
   int    execver = -1;
   int    compver = -1;

   /* get host specifics */
   uname(&names);

   /* first get execution host name */
   if (ret_AppExecEnvPlatform) 
   {
      strncpy(ret_AppExecEnvPlatform,names.sysname,_DtPLATFORM_MAX_LEN-1);
      ret_AppExecEnvPlatform[_DtPLATFORM_MAX_LEN-1] = EOS;
      platform=ret_AppExecEnvPlatform;
   }

   /* then look up version number of execution host */
   if (ret_AppExecEnvVersion) 
   {
#if defined(sun) || defined(_AIX) || defined(USL) || defined(__uxp__) || defined(__osf__) || defined(linux) || defined(CSRG_BASED)
      char version[SYS_NMLN+SYS_NMLN+2];
#else
      char version[UTSLEN+UTSLEN+2];
#endif
      char * stdVer = NULL;
      int  verNum = MATCHALL_VER;

      /* cat release version and do a translation on it to a std value */
      /* then convert the std value to a integer */
      strcpy(version,names.release);
      strcat(version,names.version);
      ret = _DtXlateOpToStdValue(db,names.sysname,0,
                   _DtXLATE_OPER_VERSION,version,&stdVer,NULL);
      if (ret == 0)
      {
         if (sscanf(stdVer,"%d",&verNum) != 1) verNum = MATCHALL_VER;;
         free(stdVer);
      }
      *ret_AppExecEnvVersion = verNum;
      execver = verNum;
   }
  
   /* then look up version number of execution host */
   if (ret_XlateCompiledForOSVersion) 
   {
      char buf[MAXINTSTRSIZE];

#define _STR(V) #V
#define STR(V) _STR(V)

      /*=========================== 
         OSMAJORVERSION & OSMINORVERSION are compile-time constants
         that must be defined as part of the build environment. 
         It is assumed that these constants are of the form:

          e.g. Sun 5.3
          #define OSMAJORVERSION        5
          #define OSMINORVERSION        3
          
             530 = (int) (100.0 * atof("5.3"));
          
          e.g. HP-UX 8.09
          #define OSMAJORVERSION         8
          #define OSMINORVERSION         09
          
             809 = (int) (100.0 * atof("8.09"));
       ===========================*/

#if !defined(OSMAJORVERSION) || !defined(OSMINORVERSION) || OSMAJORVERSION == 0
#error OSMAJORVERSION and/or OSMINORVERSION not defined
#endif

#if defined(linux) || defined(CSRG_BASED)
      sprintf(buf,"%s%s%s", STR(OSMAJORVERSION), 
                     nl_langinfo('.'), STR(OSMINORVERSION));
#else
      sprintf(buf,"%s%s%s", STR(OSMAJORVERSION), 
                     nl_langinfo(RADIXCHAR), STR(OSMINORVERSION));
#endif

      *ret_XlateCompiledForOSVersion = (int) (100.0 * atof(buf));
      compver = *ret_XlateCompiledForOSVersion;
   }

   if (db->debugMode) 
      fprintf(stderr,"_DtXlateGetXlateEnv: "
                     "Platform: %s; Execution Ver: %d; Compiled Ver: %d\n",
                      platform,execver,compver);

   return ret;
}       /*$END$*/


/*========================================================*/
/*================ Introductory Info =====================*/
/*========================================================*/

#if DOC
/*========================================================*/
$INTROBEG$: _DtXlate family
$1LINER$: API to translate any value to a standard value and back again
$SUMMARY$:
_DtXlate is a collection of routines that allow translation
between platform, version, and operation specific values
into standard values and back again.  Translation is
based on the contents of database files specified using
Xrm-style resources.  The semantics of the translation
are a combination of standard semantics for specifying
platform, version, operation, and translation type, and
caller-specific semantics for the standard value format
and operation values.

The API contains routines to process translation databases
in a useful manner.

_DtXlateOpenDb() opens a particular translation database
_DtXlateOpenAndMergeDb() opens a particular database and
  merges it with an already opened one, overriding any
  repeated specifications.
_DtXlateOpenAllDbs() opens all occurances of a translation
   database found in a search path and cumulatively merges 
   the contents, allowing for the override of existing 
   specifications as well as the addition of new ones.
_DtXlateCloseDb() closes an open database
_DtXlateOpToStdValue() translates an platform, version,
   and operation specific value into a standard value.
_DtXlateStdToOpValue() translates a standard value into
   a platform, version, and operation specific value.
/*=$END$================================================*/
#endif

#if DOC
/*========================================================*/
$INTROBEG$: Design and Implementation Considerations
$1LINER$: Factors influencing design and implementation
$SUMMARY$:
The syntax of the translation specification table is designed
to be compatible with Xrm resource specifications.  This
allows Xrm to be used to load, parse, and merge the databases
for processing.  This also causes the specifications to be
case-sensitive, as case is preserved by Xrm and is used when
processing queries.

However, due to the semantics of translation specifications
and limitations of Xrm, XrmGetResource() queries generally
will not be useful.  Rather, a pattern matching API is provided
that implements the query capability used for the translations.
The initial implementation will use XrmEnumerateDatabase().

Using Xrm leads to an in-memory approach to translation, meaning
that all the resources files are parsed and loaded into memory
as the first step of a translation.  A line-at-a-time approach,
that process one line at a time directly from the file in a
grep- or awk-like manner, is likely more memory-efficient.

Note that the line-at-a-time approach does not avoid parsing
all the specification files, as the API supports inheritence
and override of specifications.  Hence, to ensure the correct
value, the entire file set must be processed.  Note also that
in a situation where translations will be repeatedly performed,
the Xrm in-memory approach may be more time-efficient, as the
files need only be parsed once, and then are utilized repeatedly.

Because of time constraints, a line-at-a-time approach will not
be used for the first implementation, as Xrm provides all the
necessary parsing, filtering, and symbol hashing routines
for free.  Given the likely large size of the tables and their
infrequent use, a line-at-a-time approach is likely the better
choice.

/*=$END$================================================*/
#endif


#if DOC
/*========================================================*/
$INTROBEG$: translation BNF syntax and semantics
$1LINER$: _DtXlate translation specification syntax and semantics
$SUMMARY$:
   BNF Syntax of Translation Specification
   =======================================

<specfile>   ::= (<xlatespec> | <comment> | <cr>)*
<xlatespec>  ::= <platform>.<version>.<operations>.<transtype>
                      .<stdvalue>:<opvalue> (<comment>|<cr>)
<platform>   ::= <identifier> | <matchall>
<version>     ::= <number> [+ | (- <number>)] | <matchall>
<operations> ::= <identifier> [',' <identifier>]* | <matchall>
<transtype>  ::= '<' | '=' | '~' | '>' | '0'
<stdvalue>   ::= <identifier> [.<identifier>]*
<langterr>   ::= <identifier> | <matchall>
<codeset>    ::= <identifier> | <matchall>
<modifier>   ::= <identifier> | <matchall>
<opvalue>    ::= (<vischars>|<metachar>)+ | '"'(<anychar>|<metachar>)+'"'

<matchall>   ::= '?'[<number>]
<cr>         ::= '\n'
<comment>    ::= '!' [^<cr>]* <cr>
<number>     ::= [0-9]+
<identifier> ::= [-_a-ZA-Z0-9]
<vischar>    ::= any non-whitespace character; meta and regular
                 expression chars must be escaped with "\\"
<anychar>    ::= any printable character; meta and regular
                 expression chars must be escaped with "\\"
<metachar>   ::= '!' | '"' | <backslash> | <valex> | <regex>
<valex>      ::= <backslash><number>
<regex>      ::= see regexp(5)
<backslash>  ::= the backslash character (\).

   Semantics of the Translation Specification
   ==========================================

<specfile> : a file containing zero or more translation specifications
or comments

<xlatespec> : a translation specification defines a fully qualified
string value which may be recovered by query pattern matching the
specification.  The qualifiers identify the semantics of the usage
of the value, allowing queries to be meaningful.  The entire xlatespec
must be on one line of the file.

<platform> : a CDE-standardized identifier for an operating system
platform.  The platform need not be supported by CDE, but CDE must
have standardized an identifier for the platform in order for it to
be used.  For example, candidate platform identifiers are "HP-UX",
"AIX", "SunOS", "Solaris", "SCO", etc.  These identifiers are the
values returned by uname(2):utsname.sysname and uname(1) -s, and
I propose CDE standardize on using these values.

The identifier string and matching constant value must be added to
the source code of the translation routine in order for it to be
recognized.  This allows control over the platform registry
and enables automatic generation of the platform on the execution
host through the use of #ifdef <PLATFORM_CONST> in the source code.

<version> : a platform-specific numeric value or range of numberic
values that allows the value to be qualified to a particular version
and release (version + release = version) or range of versions and
releases of the platform.  The version numbers must be integral and
suitable for numeric comparison.  The '+' can be used to specify an
open upwards bound, as in 900+.  The '- <number>' can be used to
specify an upper bound, as in 900-999.  If no range is specified,
the version number must match exactly to the query pattern and/or
execution environment.

Platforms may not directly provide version numbers in a numeric
format suitable for use when comparing with a translation
specification.  As part of the translation API source code,
conversion routines must be supplied that translate the platform-
specific values, such as those provided by uname(2), into a
translation specification-compliant format suitable for comparison
to <version>.  Understanding the conversion routines operation
will be necessary to ensure correct translation specifications.

<operations> : a CDE-standardized identifier for the operation(s) to
which the value applies.  The operation(s) need not be supported by every
platform, but CDE must have standardized an identifier for the operation
in order for it to be used.  More than one identifer may be included by
concatenating them using the ',' separator, eg. "iconv1,iconv3".

<transtype> : The <transtype> field records the direction of
translation that the specification supports:  '<', '=', '~', '>', '0'.
Use '<' if the <opvalue> contains a regular expression; the
specification may only be used to match an operation value in
a _DtLcxXlateOpToStd() translation.  Use '>' if the <opvalue>
contains value replacement expressions using values from
the <stdvalue> strings; the specification may only be used by a
_DtLcxXlateStdToOp() translation.  Use '=' if the <opvalue>
is a pure string that may be used for either OpToStd() or
StdToOp() translations, and the  specification op-value must exactly
match the op-value of the translation request.  use '~' if the
<opvalue> is a pure string that may be used for either OpToStd() or
StdToOp() translations, and the specifation op-value must be
a string within the op-value of the translation request.
Use the '0' translation to represent that this particular 
translation should result in an error, that is should not result 
in a translation.

Translations are two-way operations: from a host- and
operation-specific value to standardized value, and back again.
In many cases, a straight string value for string value translation
is sufficient.

But often, more sophisticated pattern matching and value generation
mechanisms are useful to keep the table size small and as general
as possible.  In these cases, wildcard and regular expression
operators are useful.  Unfortunately, these specifications are not
bi-direction.  That is, the pattern matching string can be used
to match against a pattern, but not to generate a result string.
For example, the vi expression  s/he*lo/hello/ matches "hello"
as well as "hey look out below", but the reverse direction of
"hello"->"he*lo" will not work.

<stdvalue> : a sequence of one or more '.'-separated CDE-standardizd
identifiers or matchall characters.  This represents the canonical
string used as a standard representation of a semantic value that
may vary in different situations.

<opvalue> : a string that has different uses depending on the
value of <transtype>.  If transtype is '<', the string is used
as a regular expression to match the opValue in _DtLcxXlateOpToStd()
requests.  If the transtype is '>', the string is used as a
replacement expression to generate a opValue in _DtLcxXlateStdToOp().
If transtype is '=' or '~', the string is used as a straight string to
both match for the OpToStd translation and as a value for
the StdToOp translation.
%
If the opvalue contains whitespace, the opvalue must be enclosed in
double quote characters.  If the opvalue contains any meta characters
that should not be treated as meta characters, they must be escaped by
two preceding backslashs.Lack of an opvalue is not an error, but will
be ignored during matches and return an empty string in a StdToOp
translation; to specify that a given translation does not exist, the
'0' translation type should be used or the specification should not
occur.

<comment> : a comment begins on an empty line with an unescaped !
and continues to the end of the line.

<matchall> : The matchall character is a universal quantifier, meaning
that it symbolizes all possible values of the field where it is used.
The matchall character may occur only in the qualifers side of
a translation specification.  Traditionally, the matchall character
has been '*', but because of the semantics of Xrm, if the '*' is used
as the matchall, Xrm does not restrict enumeration as needed.

The matchall character may be followed by a number when the <transtype>
is '>'.  When this occurs, the string that matches the matchall
character may be referenced in the <opvalue> by using the sequence
\<number>, as in "\1".  The occurance is replaced with the matched
string.
/*=$END$================================================*/
#endif

#if DOC
/*========================================================*/
$INTROBEG$: translation specification examples
$1LINER$: examples of _DtXlate translation specifications
$EXAMPLE$:

!! These are examples of bi-directional straight translations
HP-UX.900-999.setlocale.=.en_US.hp-roman8:        american
HP-UX.900-999.setlocale.=.en_US.iso88591:         american.iso88591
HP-UX.900-999.setlocale.=.nl_BE.hp-roman8.fold:   dutch@fold
HP-UX.900-999.setlocale.=.nl_BE.hp-roman8.nofold: dutch@nofold

!! These are examples of OpToStd translations utilizing regular 
!! expression patterns to match the search value.
HP-UX.?.version.<.900:   "A\\.09\\..*"     !! any HPUX 9.x version
HP-UX.?.version.<.807:   "A\\.08\\.07.*"
HP-UX.?.version.<.800:   "A\\.08\\..*"
AIX.?.version.<.320:     "2 3"
AIX.?.version.<.300:     "[0-9] 3"
SunOS.?.version.<.530:   "5\\.3.*"
SunOS.?.version.<.500:   "5\\..*"

!! These are examples of StdToOp translations utilizing matchall
!! specifiers in the std value to match the search value.
HP-UX.900+.iconv1.>.?.iso88596:     arabic8
HP-UX.900+.iconv1.>.?.iso88597:     greek8
HP-UX.900+.iconv1.>.?.hp-kana8:     kana8
HP-UX.900+.iconv1.>.?.hp-roman8:    roman8

!! The following examples use value replacement as part of their
!! specifications.  Using this can lead to much sparser tables, but
!! it depends on op-specific and std values sharing the same strings.
HP-UX.900-999.setlocale.<.nl_BE.hp-roman8.?1:     [dD][uU][tT][cC][hH]@\\(.*\\)
!HP-UX.900-999.setlocale.<.nl_BE.hp-roman8.?1:     dutch@\\(.*\\)
HP-UX.1000+.setlocale.>.?1.hp-roman8:    \\1.roman8     !! all non-modif cases
HP-UX.1000+.setlocale.>.?1.hp-roman8.?2: \\1.roman8@\\2 !! all modif cases

/*=$END$================================================*/
#endif


#if DOC
/*========================================================*/
$INTROBEG$: _DtXlate example usage
$1LINER$: Examples of how to _DtXlate
$EXAMPLE$:
#include <XlationSvc.h>
main()
{
   _DtXlateDb db = NULL;
   int  ret;
   char plat[_DtPLATFORM_MAX_LEN];
   int  execver;
   int  compver;
   char * val = NULL;
   char * str = NULL;
   char empty = 0;
#define OPER_YOUROP "myop"

   env = getenv("MYPATH");
   if (env == NULL) env = &empty;

   ret = _DtXlateOpenAllDbs(env,"myfile.xlt",&db);

   ret = _DtXlateGetXlateEnv(db,plat,&execver,&compver);
   printf("Platform: %s\nExec Ver: %d\nComp Ver: %d\n",
                    plat,execver,compver);

   ret = _DtXlateStdToOpValue(db,plat,compver,OPER_YOUROP,
                     str="en_US.hp-roman8",&val,NULL);
   if (ret==0) printf("setlocale(%s) xlation=%s\n", str, val);
   else printf("no xlation\n", val);

   ret = _DtXlateOpToStdValue(db,plat,execver,OPER_YOUROP,
                     str="american",&val,NULL);
   if (ret==0) printf("setlocale(%s) xlation=%s\n", str, val);
   else printf("no xlation\n", val);

   ret = _DtXlateCloseDb(&db);
}
/*=$END$================================================*/
#endif
