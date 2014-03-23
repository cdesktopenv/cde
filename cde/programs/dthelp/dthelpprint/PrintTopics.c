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
#if DOC
/*===================================================================
$FILEBEG$:   PrintTopics.c
$COMPONENT$: dthelpprint
$PROJECT$:   Cde1
$SYSTEM$:    HPUX 9.0; AIX 3.2; SunOS 5.3
$REVISION$:  $TOG: PrintTopics.c /main/12 1998/04/01 17:26:51 mgreess $
$COPYRIGHT$:
   (c) Copyright 1996 Digital Equipment Corporation.
   (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
   (c) Copyright 1993,1994,1996 International Business Machines Corp.
   (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
   (c) Copyright 1993,1994,1996 Novell, Inc. 
   (c) Copyright 1996 FUJITSU LIMITED.
   (c) Copyright 1996 Hitachi.
==$END$==============================================================*/
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>    /* for stat() */
#if defined(sun) || defined(USL) || defined(__uxp__)
#include <widec.h>
#else
#include <wchar.h>
#endif
#include <locale.h>
#if !defined(sun)
#include <langinfo.h>
#endif

/* Iconv not defined for linux.  Use the EUSCompat stubs instead. */
#if !defined(linux)
#  include <iconv.h>
#else
#  include <EUSCompat.h>
#endif
#include <errno.h>

#include <time.h>

#include "HelpPrintI.h"  /* helpprint */

#include "HelpTermP.h"	/* from libDtHelp */
#include "CanvasP.h"	/* from libDtHelp */
#include "StringFuncsI.h" /* from libDtHelp */
#include "LocaleXlate.h" /* from libDtHelp */
#include "bufioI.h" /* from libDtHelp; required for AccessI.h */
#include "FileUtilsI.h" /* from libDtHelp */
#include "GenUtilsP.h" /* from libDtHelp */

/*#include "AccessI.h" ** from libDtHelp */
/* I can't include AccessI.h because it redefines the Boolean type,
which is also defined in Xt/Intrisincs.h.  I'm just including the
prototypes from AccessI.h here. */
extern char * _DtHelpCeGetVolumeLocale(VolumeHandle helpVolumeHandle);
extern const char * _DtHelpCeGetVolumeCharSet(VolumeHandle helpVolumeHandle);

/*======== platform adjustments ==============*/
#ifdef sun
#define wcswidth(s,n) wscol(s)
#define wcslen(s) wslen(s)
#endif

/*======== boundary values ==============*/
#define MAXSECTS		50	   /* max number of section nesting */

#define MAXVOLTITLEWIDTH 50
#define MAXTOPICTITLEWIDTH 50
#define MAXPAGENUMWIDTH 3
#define MAXSECTNUMWIDTH 8

/*======== helper values ===============*/
#define EOS           '\0'
#define EMPTY_STR     s_EmptyStr

#define PTSET       3               /* message set */


/*======== helper variables ===============*/
static char s_EmptyStr[1] = { EOS };

/* To do:
	* check roman 8/Latin 1
	* check PAGER env variable
	* do character wrap
*/

/*======== data structs ==============*/

/* These data structs manage the table of contents (Toc)
   of a help volume.  The Toc uses two organizational mechanisms:
   a linear table of entries, which gives sequential order
   and is used to generate the TOC; and a binary tree of entries
   that is sorted alphabetically by location id, and is used to
   speed lookup of the page number of a locationId for use in
   generating the index. */

#if DOC
===================================================================
$PTYPEBEG$:  TocEntry
$1LINER$:  All data for a single toc entry
$DESCRIPT$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/
/*$DEF$*/
typedef struct TocEntry
{
   struct TocEntry * nextEntry;		/* next entry in the linear list */
   struct TocEntry * leftLoc;		/* left location of sorted binary tree */
   struct TocEntry * rightLoc;		/* right location of sorted binary tree */
   char * locationId;			/* locationId of the topic */
   int    pageNumber;			/* page the topic located on */
   int    level;			/* nesting level */
   int    sectNums[MAXSECTS];		/* section numbers */
   char * sectStr;                      /* alternate section string */
} TocEntry;
/*$END$*/

#if DOC
===================================================================
$PTYPEBEG$:  Toc
$1LINER$:  Manages the toc entries in a volume
$DESCRIPT$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/
/*$DEF$*/
typedef struct Toc
{
   TocEntry * linearEntries;		/* ptr to head of linear list of entries */
   TocEntry * lastLinearEntry;		/* ptr to tail of linear list of entries */
   TocEntry * sortedEntries;		/* ptr to top of sorted btree of entries */
} Toc;
/*$END$*/

#if DOC
===================================================================
$PTYPEBEG$:  HeadFootFormat
$1LINER$:  All data related to the header/footer formatting
$DESCRIPT$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/
/*$DEF$*/
typedef struct HeadFootFormat
{
   char *  formattedEvenHeader; /* formatted even-page header when printing volume */
   char *  formattedOddHeader;  /* formatted odd-page header when printing volume */
   char *  formattedEvenFooter; /* formatted even-page footer when printing volume */
   char *  formattedOddFooter;  /* formatted odd-page footer when printing volume */
   int     evenHeaderLineCnt;   /* num lines in even-page header when printing volume */
   int     oddHeaderLineCnt;    /* num lines in odd-page header when printing volume */
   int     evenFooterLineCnt;   /* num lines in even-page footer when printing volume */
   int     oddFooterLineCnt;    /* num lines in odd-page footer when printing volume */
} HeadFootFormat;
/*$END$*/

#if DOC
===================================================================
$PTYPEBEG$:  HeadFootFormatArgs
$1LINER$:  Arguments used in head/foot formatting
$DESCRIPT$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/
/*$DEF$*/
typedef struct HeadFootFormatArgs
{
   char *   volumeTitle;
   int      volumeTitleColsWidth;
   char *   topicTitle;
   int      topicTitleColsWidth;
   char *   volumeDate;
   int      volumeDateColsWidth;
   char *   todaysDate;
   int      todaysDateColsWidth;
   int      sectNumColsWidth;
   int      pageNumColsWidth;
}
HeadFootFormatArgs;
/*$END$*/

#if DOC
===================================================================
$PTYPEBEG$:  PrintState
$1LINER$:  All data related to the on-going printing operation
$DESCRIPT$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/
/*$DEF$*/
typedef struct PrintState
{
   char *   currentLocId;		/* current loc Id in volume */
   char *   outputFromLocId;		/* start output at this location */
   HeadFootFormatArgs hffArgs;          /* arguments used in head/foot formatting */
   HeadFootFormat tocHFF;		/* HF formatting info for TOC */
   HeadFootFormat bodyHFF;		/* HF formatting info for body */
   HeadFootFormat indexHFF;		/* HF formatting info for index */
   Boolean  inhibitOutput;		/* inhibit output */
   int      curPageNumber;		/* page number of current output */
   int      curLineNumber;		/* line number of current page of output */
   VolumeHandle volHandle;		/* volume handle of volume being printed */
   CanvasHandle canvasHandle;		/*canvas handle of terminal format canvas*/
   int      sectNums[MAXSECTS];		/* current section number */
   char *   sectStr;			/* current section string value */
   int      level;			/* level of section nesting */
} PrintState;
/*$END$*/

/*======== static variables ===============*/

#if DOC
===================================================================
$PTYPEBEG$:  SymValue
$1LINER$:  Maps a symbolic string to a printf-format string.
$DESCRIPT$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/
/*$DEF$*/
typedef struct SymValue
{
   char *  symbol;
   char *  argref;
} SymValue;

/* List of symbols that can be used in header/footer strings
   and the printf args that are used to access the data. */
/* NOTE: ***IT IS CRITICAL THAT THE SYMBOL STRING HAVE THE 
   SAME OR LONGER LENGTH THAN THE ARGUMENT STRING. ***/
/* NOTE: ***ALWAYS UPDATE THE UNUSED_ARGREF AND THE sprintf() CALL 
            IN PrintHeadFootStr() WHEN ADDING NEW SYMBOLS***/
/* NOTE: The replacement values use a trick to get around the limitations
   of printf(), which only allows args numbered 1 to 9.  The strategy
   is to do two printfs, and to use the %% construct in the first string
   to generate a % construct in the second string. */
SymValue g_HeadFootSymsList[] = 
{
  { "$VOLUMEFILL", "%%2$*1$c" },  /* filler for fixed sized volume name */
  { "$TOPICFILL", "%%4$*3$c" },   /* filler for fixed sized current topic title */
  { "$PAGENUMFILL", "%%6$*5$c" }, /* filler for fixed sized page number */
  { "$SECTNUMFILL", "%%8$*7$c" }, /* filler for fixed sized section number or name */
  { "$TODAY", "%1$s" },       /* today's date */
  { "$VOLUME", "%2$s" },      /* volume name */
  { "$TOPIC", "%3$s" },       /* current topic title */
  { "$PAGENUM", "%4$d" },     /* page number */
  { "$VOLDATE", "%5$s" },     /* date on the help volume file */
  { "$SECTNUM", "%6$s" },     /* section number or name */
  { "$LMARGIN", "%8$*7$c" },      /* left margin blanks; args 7 & 8 */
  { NULL, NULL }
};
#define  UNUSED_ARGREF   "%%9$n"    /* arg to hold the num of unused args */
/*$END$*/


/*======== functions ==============*/

#if DOC
===================================================================
$PFUNBEG$:  TocNextEntry()
$1LINER$:  Get next entry in the toc
$DESCRIPT$:
$ARGS$:
$RETURNS$:
!= NULL:   next entry
NULL:      no more entires
========================================================$SKIP$=====*/
#endif /*DOC*/

static
TocEntry * TocNextEntry(
  Toc *      toc,
  TocEntry * entry)
{       /*$CODE$*/
  if (NULL == entry) return toc->linearEntries;
  return entry->nextEntry;
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  TocFindSortedEntry()
$1LINER$:  Finds an entry using the sorted btree of entries
$DESCRIPT$:
$RETURNS$:
$ARGS$:
0:   ok
-1:  entry not found
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int TocFindSortedEntry(
  Toc *        toc,
  char *       locationId,
  TocEntry * * ret_parent,
  TocEntry * * ret_entry)
{       /*$CODE$*/
   TocEntry * prev;
   TocEntry * cur;
   TocEntry * tmp;

   if (NULL == ret_parent) ret_parent = &tmp;
   if (NULL == ret_entry) ret_entry = &tmp;

   *ret_parent = NULL;
   *ret_entry = NULL;

   prev = toc->sortedEntries;
   cur = prev;
   while ( NULL != cur )
   {
      int cmp;
      cmp = _DtHelpCeStrCaseCmp(locationId,cur->locationId);
      if ( cmp < 0 )
         prev = cur, cur = cur->leftLoc;
      else if ( cmp > 0 )
         prev = cur, cur = cur->rightLoc;
      else
      {
         *ret_parent = prev, *ret_entry = cur;
         return 0;                            /* RETURN: found */
      }
   }
   *ret_parent = prev;
   return -1;                                 /* RETURN: not found */
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  TocNewEntry()
$1LINER$:  Add a Toc entry to the Toc
$DESCRIPT$:
$RETURNS$:
$ARGS$:
0:   ok
-1:  memory allocation error
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int TocNewEntry(
  Toc *      toc,
  char *     locationId,
  int        pageNumber,
  int        level,
  int *      sectNums,
  char *     sectStr)
{       /*$CODE$*/
   TocEntry * new;

   new = calloc(1,sizeof(TocEntry));
   if (NULL == new) return -1;        /* RETURN: mem alloc err */

   /* init contents */
   new->locationId = strdup(locationId);
   new->pageNumber = pageNumber;
   new->level = level;
   new->sectStr = sectStr;
   memcpy(new->sectNums, sectNums, sizeof(new->sectNums));

   if (NULL == new->locationId)
      { free(new); return -1; }       /* RETURN: mem alloc err */

   /*** insert into toc ***/

   /* if first in list */
   if (NULL == toc->linearEntries)
   {
      toc->linearEntries = new;
      toc->lastLinearEntry = new;
      toc->sortedEntries = new;
   }
   else /* not first in list */
   {
      TocEntry * parent, * entry;

      if ( TocFindSortedEntry(toc,locationId,&parent,&entry) == 0 )
         return -2;                        /* RETURN: duplicate entry */
      /* insert into the btree (not balanced) */
      if( _DtHelpCeStrCaseCmp(locationId,parent->locationId) < 0 )
         parent->leftLoc = new;
      else
         parent->rightLoc = new;
      /* insert into the linear list */
      toc->lastLinearEntry->nextEntry = new;
      toc->lastLinearEntry = new;
   }
   return 0;        /* RETURN: ok */
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  IconvFile()
$1LINER$:  Compares cur env and volume and iconv file if necessary 
$DESCRIPT$:
$RETURNS$:
0:  no conversion needed or not possible to determine if needed
1:  conversion needed & successful
-1: conversion needed and failed
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int IconvFile(
      _DtHPrOptions * options,
      VolumeHandle helpVolumeHandle,
      char * * srcFile)
{       /*$CODE$*/
#define CUR_LOCALE    0
#define CUR_CODESET   1
#define VOL_LOCALE    2
#define VOL_CODESET   3
#define FROM_CODESET  4
#define TO_CODESET    5
#define NUMSTRS       6

   int    ret;
   int    i;
   char * loc[NUMSTRS];
   char * destFile = NULL;
   char * codeset;
   char   buf[1000];

   for (i=0; i<NUMSTRS; i++) loc[i] = NULL;
   
   /* get the normalized current codeset */
   _DtHelpCeXlateOpToStdLocale (
                     DtLCX_OPER_SETLOCALE, setlocale(LC_CTYPE,NULL),
                     &loc[CUR_LOCALE], NULL, &loc[CUR_CODESET]);

   /* get the normalized volume codeset */
   loc[VOL_LOCALE] = _DtHelpCeGetVolumeLocale(helpVolumeHandle);

   /* codeset begins after the '.'; find it */
   codeset = NULL;
   if (    loc[VOL_LOCALE]
        && _DtHelpCeStrchr(loc[VOL_LOCALE], ".", 1, &codeset) == 0)
   {
       codeset++;
   }
   loc[VOL_CODESET] = (NULL != codeset ? strdup(codeset) : NULL);

   /* if either locale is NULL or if they are the same string
      then don't iconv the file */
   if (   NULL == loc[CUR_CODESET]
       || NULL == loc[VOL_CODESET]
       || strcmp(loc[CUR_CODESET],loc[VOL_CODESET]) == 0 )
   {
       ret = 0;                   /* RETURN:  no iconv needed/possible */
       goto cleanup;
   }

   /* get the source codeset */
   _DtHelpCeXlateStdToOpLocale (
                     DtLCX_OPER_ICONV1, loc[VOL_LOCALE],
                     "iso8859_1", &loc[FROM_CODESET]);

   /* get the target codeset */
   _DtHelpCeXlateStdToOpLocale (
                     DtLCX_OPER_ICONV1, loc[CUR_LOCALE],
                     "iso8859_1", &loc[TO_CODESET]);

   /* construct the command line */
   destFile = _DtHPrCreateTmpFile(TMPFILE_PREFIX,TMPFILE_SUFFIX);
   if (NULL == destFile)
   {
       ret = -1;                   /* error */
       goto cleanup;
   }

   sprintf(buf,options->iconvCmdAndArgs,
                 loc[FROM_CODESET],loc[TO_CODESET],*srcFile, destFile);

   /* do the conversion */
   if(options->debugHelpPrint) printf("%s\n",buf);
   ret = system(buf);
   ret = (ret == 0 ? 1 : -1);    /* 1: success; -1: failure */

   /* if successful conversion, change the src file */
   if (ret >= 0)
   {
      unlink(*srcFile);
      free(*srcFile);
      *srcFile = destFile;
   }
   else
   {
      unlink(destFile);
   }

cleanup:
   /* free memory */
   for (i=0; i<NUMSTRS; i++) if (loc[i]) free(loc[i]);

   return ret;
} /* count lines */

#ifdef ICONV_INBUF_CONST
# define ICONV_INBUF_TYPE	const char **
#else
# define ICONV_INBUF_TYPE	char **
#endif

#define WORKSIZE 1024*10	/* 10k */

/*
 * _i18nwork1[] is used to convert the passed string with CD iconv.
 * in _converter_().
 *
 */
static void           *_i18nwork1 = NULL;
static unsigned long  _i18nsize1 = 0;
static int            shouldAlloc1 = ~0;


static void _converter_( iconv_t CD,
			void *from, unsigned long from_len,
			void **to )
{
    char          *InBuf;
    size_t        InBytesLeft;
    char          *OutBuf = NULL;
    size_t        OutBytesLeft = 0;
    size_t        _OutBytesLeft = 0;
    size_t        iconv_ret;
    size_t        converted_num = 0;
    unsigned long to_len;


    *to = NULL;
    to_len = 0;

    if ( shouldAlloc1 ) {
        /* Obtain work area */
        _i18nwork1 = (size_t *)malloc( WORKSIZE );
        if ( !_i18nwork1 ) {
            _i18nwork1 = NULL;
            return;
        }
        _i18nsize1 = WORKSIZE; 
        shouldAlloc1 = 0;
    }

    InBuf        = (char *)from;
    InBytesLeft  = from_len;
    OutBytesLeft = _i18nsize1;
    OutBuf = (char *)_i18nwork1;

    /*
     * Need to place iconv state to the initial one by
     * setting inbuf to NULL of iconv().
     */
    iconv( CD, (ICONV_INBUF_TYPE)NULL, 0, NULL, 0 );
    while( 1 ) {
	/*
	 * InBuf
	 *  v
	 * +----------------------------+
	 * | |                        | |
	 * +----------------------------+
	 *  <-------------------------->
	 *          InBytesLeft
	 *
	 *             |
	 *             | iconv()
	 *             V
	 * (_i18nwork1)
	 * OutBuf
	 *  v
	 * +----------------------------+
	 * | |                        | |
	 * +----------------------------+
	 *  <-------------------------->
	 *          InBytesLeft
	 */

	iconv_ret = iconv( CD, (ICONV_INBUF_TYPE)&InBuf, &InBytesLeft,
                               &OutBuf, &OutBytesLeft );
	if ( iconv_ret == 0 ) {
	    /* iconv done
	     *                             InBuf
	     *                               v
	     * +----------------------------+
	     * |XXXXXXXXXXXXXXXXXXXXXXXXXXXX|
	     * +----------------------------+
	     *                               
	     *                               InBytesLeft=0
	     *
	     * (_i18nwork1)
	     *  |               OutBuf
	     *  V                 v
	     * +----------------------------+
	     * |XXXXXXXXXXXXXXXXX| |      | |
	     * +----------------------------+
	     *  <---------------> <-------->
	     *   converted_num    OutBytesLeft
	     */
	    converted_num = (unsigned long)((char *)OutBuf-(char *)_i18nwork1);
	    *to = (void *)_i18nwork1;
	    to_len = (unsigned long)converted_num;
	    break;
	} else {
	    if ( errno == E2BIG ) {
		/* Overflow. still data is left.
		 *               InBuf
		 *                 v
		 * +----------------------------+
		 * |XXXXXXXXXXXXXX| |         | |
		 * +----------------------------+
		 *                 <----------->
		 *                  InBytesLeft
		 *
		 * (_i18nwork1)
		 *  |                         OutBuf
		 *  V                          v
		 * +----------------------------+
		 * |XXXXXXXXXXXXXXXXXXXXXXXXXXX |
		 * +----------------------------+
		 *  <-------------------------> 
		 *          converted_num      OutBytesLeft=?
		 */
		void *_p;

		/* Check how many converted already. */
		converted_num =
			(unsigned long)((char *)OutBuf - (char *)_i18nwork1);
		_i18nsize1 += WORKSIZE;
		_p = realloc( _i18nwork1, _i18nsize1 );
		if ( !_p ) {
		    *to = NULL;
		    to_len = 0;
		    free( _i18nwork1 );
		    _i18nwork1 = NULL;
		    _i18nsize1 = 0;
		    shouldAlloc1 = ~0;
		    break;
		} else {
		    _i18nwork1 = _p;
		    OutBuf = (char *)((char*)_i18nwork1 + converted_num);
		    OutBytesLeft += WORKSIZE;
		}  
	    } else {
		*to = NULL;
		to_len = 0;
		break;
	    }
	}
    }

    /*
     * Null terminate
     */

    if ( *to != NULL ) {
        if ( _i18nsize1 >= to_len + 1 ) {
            ((char *)_i18nwork1)[to_len] = '\0';
        } else {
	    void *_p;

            _i18nsize1++;
            _p = realloc( _i18nwork1, _i18nsize1 );
	    if ( !_p ) {
	        *to = NULL;
	        to_len = 0;
	        free( _i18nwork1 );
	        _i18nwork1 = NULL;
	        _i18nsize1 = 0;
	        shouldAlloc1 = ~0;
	    } else {
	        _i18nwork1 = _p;
	        ((char *)_i18nwork1)[to_len] = '\0';
	    }  
       }
    }
}

/*
 * Iconv for buffer
 */
static
int IconvBuffer(
      VolumeHandle helpVolumeHandle,
      char *src,
      char **dest )
{       /*$CODE$*/
#define CUR_LOCALE    0
#define CUR_CODESET   1
#define VOL_LOCALE    2
#define VOL_CODESET   3
#define FROM_CODESET  4
#define TO_CODESET    5
#define NUMSTRS       6

   int    ret;
   int    i;
   char * loc[NUMSTRS];
   char * codeset;
   char   buf[1000];
   static int isFirst = ~0;
   static iconv_t CD = (iconv_t)-1;

   for (i=0; i<NUMSTRS; i++) loc[i] = NULL;
   
   /* get the normalized current codeset */
   _DtHelpCeXlateOpToStdLocale (
                     DtLCX_OPER_SETLOCALE, setlocale(LC_CTYPE,NULL),
                     &loc[CUR_LOCALE], NULL, &loc[CUR_CODESET]);

   /* get the normalized volume codeset */
   loc[VOL_LOCALE] = _DtHelpCeGetVolumeLocale(helpVolumeHandle);

   /* codeset begins after the '.'; find it */
   codeset = NULL;
   if (    loc[VOL_LOCALE]
        && _DtHelpCeStrchr(loc[VOL_LOCALE], ".", 1, &codeset) == 0)
   {
       codeset++;
   }
   loc[VOL_CODESET] = (NULL != codeset ? strdup(codeset) : NULL);

   /* if either locale is NULL or if they are the same string
      then don't iconv the file */
   if (   NULL == loc[CUR_CODESET]
       || NULL == loc[VOL_CODESET]
       || strcmp(loc[CUR_CODESET],loc[VOL_CODESET]) == 0 )
   {
       ret = 0;                   /* RETURN:  no iconv needed/possible */
       goto cleanup;
   }

   /* get the source codeset */
   _DtHelpCeXlateStdToOpLocale (
                     DtLCX_OPER_ICONV1, loc[VOL_LOCALE],
                     "iso8859_1", &loc[FROM_CODESET]);

   /* get the target codeset */
   _DtHelpCeXlateStdToOpLocale (
                     DtLCX_OPER_ICONV1, loc[CUR_LOCALE],
                     "iso8859_1", &loc[TO_CODESET]);

   if ( isFirst ) {
      CD = iconv_open( loc[TO_CODESET], loc[FROM_CODESET] );
      isFirst = 0;
   }
   if ( CD == (iconv_t)-1 ) {
      ret = 0;
   } else {
      ret = 1;
       _converter_( CD, (void *)src, (unsigned long)strlen( src ),
		(void **)dest );
       if ( *dest == NULL )
           ret = 0;
   }
cleanup:
   /* free memory */
   for (i=0; i<NUMSTRS; i++) if (loc[i]) free(loc[i]);

   return ret;
} /* count lines */



#if DOC
===================================================================
$PFUNBEG$:  OpenTmpFile()
$1LINER$:  Opens temporary file
$DESCRIPT$:
opens a temporary file
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
FILE * OpenTmpFile(
      _DtHPrOptions * options,
      char * * ret_tmpFile)
{       /*$CODE$*/
   FILE * fp;

   /* Put in $HOME/.dt/tmp so that if the printer operation is running
      of a remote system, it can get to the (local) temp file. 
      This would not be possible if the file were put in /tmp */
   *ret_tmpFile = _DtHPrCreateTmpFile(TMPFILE_PREFIX,TMPFILE_SUFFIX);
   if (NULL == *ret_tmpFile) return NULL;     /* RETURN: error */

   fp = fopen(*ret_tmpFile,"w");
   if (NULL == fp)
   {
      fprintf(stderr,_DTGETMESSAGE(PTSET,6,
                           "%s Error: unable to open temporary file %s\n"),
                           options->programName, *ret_tmpFile);
   }

   return fp;
} /* open tmp file */


#if DOC
===================================================================
$PFUNBEG$:  CountLines()
$1LINER$:  counters number of CRs in a string
$DESCRIPT$:
counters number of CRs in a string
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int CountLines(
      char * str)
{       /*$CODE$*/
   char * substr;
   int    lineCount = 0;
   static char * newLine = "\n";

   substr = str;
   while( _DtHelpCeStrchr(substr,newLine,MB_CUR_MAX,&substr) == 0 )
   {
      lineCount++;
      substr++;
   }
   /* return line count to caller */
   return lineCount;
} /* count lines */



#if DOC
===================================================================
$PFUNBEG$:  AvailContentLines()
$1LINER$:  count number of lines available between top of page & footer
$DESCRIPT$:
Count number of lines available between top of page & footer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int AvailContentLines(
  _DtHPrOptions * options,
  PrintState *    state,
  HeadFootFormat * hff)
{      /*$DEF$*/
    return options->rowsTextHeight - 
                 ( (state->curPageNumber % 2) == 0 
                     ? hff->evenFooterLineCnt 
                     : hff->oddFooterLineCnt);  
}      /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  SectNumStr()
$1LINER$:  generates string ver of section number
$DESCRIPT$:
 generates string ver of section number
$RETURNS$:
$ARGS$:
buf:       must be at least 4 * MAXSECTS chars long
========================================================$SKIP$=====*/
#endif /*DOC*/

static
char * SectNumStr(
  int *    sectNums,
  char *   sectStr,
  char *          buf)
{       /*$CODE$*/
    char partial[5];

     /* recall: if sectNums[0] == 0, then no section number is defined
        and none should be printed; try sectStr as alternate. */

    /* generate the section number */
    buf[0] = EOS;
    if (sectNums && sectNums[0] != 0 )
    {
       int sect;
       for ( sect = 1; sectNums[sect] != 0; sect++ )
       {
          sprintf(partial, "%d.", sectNums[sect]);
          strcat(buf,partial);
       } 
    }
    else  /* if no section number, take the section string, if avail */
    {
       if (sectStr) strcpy(buf,sectStr);
       else buf[0] = EOS;
    }
    return buf;
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  OutputBlankSpaces()
$1LINER$:  Outputs blank Spaces
$DESCRIPT$:
Outputs blank Spaces
$RETURNS$:
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, do nothing
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void OutputBlankSpaces(
  FILE *  topicsFP,
  int     spaceCount)
{       /*$CODE$*/
     if (NULL == topicsFP || spaceCount < 0) return;
     while (spaceCount--) fprintf(topicsFP," "); 
} /* $END$ */


#if DOC
===================================================================
$PFUNBEG$:  OutputBlankLines()
$1LINER$:  Outputs blank lines
$DESCRIPT$:
Outputs blank lines
$RETURNS$:
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, do nothing
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void OutputBlankLines(
  FILE *  topicsFP,
  PrintState * state,
  int     lineCount)
{       /*$CODE$*/
     if (lineCount < 0) return;			/* RETURN */
     state->curLineNumber += lineCount;
     if (NULL == topicsFP) return;		/* RETURN */
     while (lineCount--) fprintf(topicsFP,"\n"); 
} /* $END$ */


#if DOC
===================================================================
$PFUNBEG$:  DoStrColsWidth()
$1LINER$:  Calculates number of cols used by a string; truncate if needed
$DESCRIPT$:
Calculates the number of cols used by the string and truncates
the string to the specified maxWidth if this is exceeded.
$NOTE$:
$RETURNS$:
The number of columns this string requires for printing.
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int DoStrColsWidth(
      char *   str,
      int      maxWidth,
      Boolean  truncateStr)
{       /*$CODE$*/
   int len;
   wchar_t * wcstr;
   int wclen;
   int width;

   /* alloc memory for the wchar_t string */
   len = strlen(str);
   wcstr = malloc(sizeof(wchar_t) * (len+1));

   /* convert str to wchar_t and get width in chars */
   mbstowcs(wcstr,str,len+1);
   wclen = wcslen(wcstr);

   /* get col width of the string and truncate if necessary */
   while (    (width = wcswidth(wcstr,wclen+1)) > maxWidth 
           && truncateStr == True)
      wcstr[--wclen] = EOS;
   wcstombs(str,wcstr,len+1);

   return wclen;
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  GenHeadFootFormatArgs()
$1LINER$:  Generates args used by a header/footer format string
$DESCRIPT$:
Generates args used by a header/footer format string
$NOTE$:
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void GenHeadFootFormatArgs(
  _DtHPrOptions * options,
  PrintState *    state,
  Boolean         updateTopicTitle,
  char *          sectNumStr,
  Boolean         updatePageNum)
{       /*$CODE$*/
   char * str;
   int    width;
   time_t date = 0;
   struct tm * pTm;
   char   buf[100];

    /* get the volume title and its size */
    if (NULL == state->hffArgs.volumeTitle)
    {
       /* get the volume title */
       str = NULL;
       _DtHelpCeGetVolumeTitle(state->canvasHandle,state->volHandle,&str);
       if (NULL == str) str = strdup("");

       width = DoStrColsWidth(str,MAXVOLTITLEWIDTH,True);

       /* put into state data */
       state->hffArgs.volumeTitle = str;
       state->hffArgs.volumeTitleColsWidth = width;
    }

    /* get the volume's date */
    if (NULL == state->hffArgs.volumeDate)
    {
       char * locDocId = NULL;
       char * locDateStamp = NULL;
       long long_date = 0;

       /* locDocId & locDateStamp will point to private memory; do not modify */
       _DtHelpCeGetDocStamp(state->volHandle,&locDocId, &locDateStamp);
       if (    NULL != locDateStamp
            && sscanf(locDateStamp, "%ld", &long_date) != 1 )
          locDateStamp = NULL;     /* make invalid */
       else
	 date = (time_t)long_date;

       /* if no vol date, try getting from the help volume file */
       if ( NULL == locDateStamp )
       {
          struct stat stats;
          date = 0;
          if ( stat(options->helpVolume,&stats) == 0 )
             date = stats.st_mtime;
       }

       /* convert the int into a string */
       pTm = localtime(&date);
       strftime(buf,sizeof(buf),"%x",pTm);

       width = DoStrColsWidth(buf,0,False);

       /* put into state data */
       state->hffArgs.volumeDate = strdup(buf);
       state->hffArgs.volumeDateColsWidth = width;
    }

    /* get today's date */
    if (NULL == state->hffArgs.todaysDate)
    {
       /* convert the int into a string */
       date = time(NULL);
       pTm = localtime(&date);
       strftime(buf,sizeof(buf),"%x",pTm);

       width = DoStrColsWidth(buf,0,False);

       /* put into state data */
       state->hffArgs.todaysDate = strdup(buf);
       state->hffArgs.todaysDateColsWidth = width;
    }

    /* get the topic title and its size */
    if (updateTopicTitle)
    {
       /* get the topic title */
       str = NULL;
       _DtHelpCeGetTopicTitle(state->canvasHandle, state->volHandle,
                              state->currentLocId, &str);
       if (NULL == str) str = strdup("");

       width = DoStrColsWidth(str,MAXTOPICTITLEWIDTH,True);

       /* put into state data */
       if (state->hffArgs.topicTitle) free(state->hffArgs.topicTitle);
       state->hffArgs.topicTitle = str;
       state->hffArgs.topicTitleColsWidth = width;
    }

    /* get the size of the section number */
    if (sectNumStr)
    {
       width = DoStrColsWidth(sectNumStr,0,False);
       state->hffArgs.sectNumColsWidth = width;
    }

    /* get the size of the page number */
    if (updatePageNum)
    {
       int num;
       for ( width = 1, num = state->curPageNumber;
             (num >= 0) && ((num / 10) > 0);
             num /= 10, width++ )
       { /* do nothing */ }
       state->hffArgs.pageNumColsWidth = width;
    }
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  GenHeadFootFormatStr()
$1LINER$:  Generates a printf-ready format string for the header/footer
$DESCRIPT$:
Generates a printf-ready format string for the header/footer
$NOTE$:
This function generates a string that uses the "%n$" prefix 
supported by printf(3S) to allow a footer/header string to 
ref any argument out of order.  This functionality requires 
that a ref to *every* argument be part of the format string.  
Because not all args may be part of the user-specified format 
string, this function appends the unused strings to the end 
of the format string for purposes of sprintf(), then truncates 
the unused portion before output.
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void GenHeadFootFormatStr(
  char *          specStr,
  char * *        io_formattedStr,
  int *           io_lineCnt)
{       /*$CODE$*/
    char * lowLevelFormatStr;
    SymValue * sym;
    char * substr;
    char   unusedSyms[60];

    if (NULL == specStr) 
    {
       *io_formattedStr = NULL;
       *io_lineCnt = 0;
       return;                        /* RETURN */
    }

    /*** create the formatted string ***/

    /* Make a working copy of the string; I assume that the
       values of 'argref' string always be shorter than 'symbol'. */
    lowLevelFormatStr = malloc(strlen(specStr) + sizeof(unusedSyms) + 10);
    if (NULL == lowLevelFormatStr) return;
    strcpy(lowLevelFormatStr,specStr);

    /* replace the symbolic names with printf argument refs */
    strcpy(unusedSyms,UNUSED_ARGREF);
    for ( sym = g_HeadFootSymsList; sym->symbol != NULL; sym++ )
    {
       Boolean unused = True;

       /* look for the symbol string */
       while ( (substr = strstr(lowLevelFormatStr,sym->symbol)) != NULL )
       {  /* and replace it with the argref */
          unused = False;
          strcpy(substr, sym->argref);
          strcpy(substr + strlen(sym->argref), substr + strlen(sym->symbol));
       }
       /* if unused, add to unused list */
       if (unused) strcat(unusedSyms,sym->argref);
    }

    /* append unused syms to end of format str */
    strcat(lowLevelFormatStr,unusedSyms);

    /* store in caller's location */
    *io_formattedStr = lowLevelFormatStr;
    *io_lineCnt = CountLines(lowLevelFormatStr);
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  GenAllHeadFootFormatStrs()
$1LINER$:  Generates printf-ready format strings for all headers/footers
$DESCRIPT$:
Generates printf-ready format strings for all headers/footers
$NOTE$:
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void GenAllHeadFootFormatStrs(
  _DtHPrOptions * options,
  PrintState *    state)
{       /*$CODE$*/
   /* Toc */
   GenHeadFootFormatStr(options->tocHF.evenHeader,
            &state->tocHFF.formattedEvenHeader, &state->tocHFF.evenHeaderLineCnt);
   GenHeadFootFormatStr(options->tocHF.oddHeader,
            &state->tocHFF.formattedOddHeader, &state->tocHFF.oddHeaderLineCnt);
   GenHeadFootFormatStr(options->tocHF.evenFooter,
            &state->tocHFF.formattedEvenFooter, &state->tocHFF.evenFooterLineCnt);
   GenHeadFootFormatStr(options->tocHF.oddFooter,
            &state->tocHFF.formattedOddFooter, &state->tocHFF.oddFooterLineCnt);
   /* Body */
   GenHeadFootFormatStr(options->bodyHF.evenHeader,
            &state->bodyHFF.formattedEvenHeader, &state->bodyHFF.evenHeaderLineCnt);
   GenHeadFootFormatStr(options->bodyHF.oddHeader,
            &state->bodyHFF.formattedOddHeader, &state->bodyHFF.oddHeaderLineCnt);
   GenHeadFootFormatStr(options->bodyHF.evenFooter,
            &state->bodyHFF.formattedEvenFooter, &state->bodyHFF.evenFooterLineCnt);
   GenHeadFootFormatStr(options->bodyHF.oddFooter,
            &state->bodyHFF.formattedOddFooter, &state->bodyHFF.oddFooterLineCnt);
   /* Index */
   GenHeadFootFormatStr(options->indexHF.evenHeader,
            &state->indexHFF.formattedEvenHeader, &state->indexHFF.evenHeaderLineCnt);
   GenHeadFootFormatStr(options->indexHF.oddHeader,
            &state->indexHFF.formattedOddHeader, &state->indexHFF.oddHeaderLineCnt);
   GenHeadFootFormatStr(options->indexHF.evenFooter,
            &state->indexHFF.formattedEvenFooter, &state->indexHFF.evenFooterLineCnt);
   GenHeadFootFormatStr(options->indexHF.oddFooter,
            &state->indexHFF.formattedOddFooter, &state->indexHFF.oddFooterLineCnt);
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  PrintHeadFootStr()
$1LINER$:  Formats and prints the header/footer string
$DESCRIPT$:
Takes a format string, replaces the place holders with actual values,
and prints the output to topicsFP.
$NOTE$:
This function uses the "%n$" prefix supported by printf(3S)
to allow a footer/header string to ref any argument out of
order.  This functionality requires that a ref to *every*
argument be part of the format string.  Because not all args
may be part of the user-specified format string, this function
appends the unused strings to the end of the format string
for purposes of sprintf(), then truncates the unused portion
before output.
$RETURNS$:
number of lines in the header or footer
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, increment
           page and reset line number), but do not output;
           If not NULL, request new page as well
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int PrintHeadFootStr(
  _DtHPrOptions * options,
  FILE *          topicsFP,
  PrintState *    state,
  char *          formattedStr,
  int             lineCnt)
{       /*$CODE$*/
    char * newLine = "\n";
    int    lastValid = 0;
    char   sectNumStr[MAXSECTS * 4 + 5];	/* $SECTNUM */
    char   buf[3000];

    if (NULL == formattedStr) return 0;        /* RETURN */
    if (NULL == topicsFP) return lineCnt;      /* RETURN */

    /*** generate dynamic data ***/

    /* get the section number */
    SectNumStr(state->sectNums,state->sectStr,sectNumStr);

    /* update args; FIX: impove efficiency by processing topic title only when needed */
    GenHeadFootFormatArgs(options,state,True,sectNumStr,True);

    /* guidelines on string size and construction: 
       The objective is to allow one set of headers & footers to apply
       to many different volumes and topics.  This is made possible by
       allowing for fixed length strings.  To get fixed length strings,
       the header/footer spec should include not only the string but
       also the fill for that string.

       The fill size is calculated based on the following widths:
         volTitle   : 50 printing chars    e.g. "title        "
         topicTitle : 50 printing chars    e.g. "title        "
         sectNumStr :  8 printing chars    e.g. "  3.10.4"
         pageNum    :  3 printing chars    e.g. "  3"
         volDate    : constant by locale--no fill needed e.g. "Mon, Jul 4, 1988"
         dateStr    : constant by locale--no fill needed e.g. "Mon, Jul 4, 1988"
    */

    /*** generate the str ***/
    /* IMPT: the order of these arguments MUST match the argument numbers
       given in the definition of the g_HeadFootSymsList variable. */
    /* print the first set */
    sprintf(buf, formattedStr,
               state->hffArgs.todaysDate,state->hffArgs.volumeTitle,
               state->hffArgs.topicTitle, (int) state->curPageNumber,
               state->hffArgs.volumeDate, sectNumStr,
               (int) options->colsAdjLeftMargin,(int) ' ' );

    /* move the format string for the second set into new memory */
    formattedStr = strdup(buf);
    if(NULL == formattedStr) return 0;            /* RETURN */

    /* print the second set */
    sprintf(buf, formattedStr,
        MAXVOLTITLEWIDTH - state->hffArgs.volumeTitleColsWidth, (int) ' ',
        MAXTOPICTITLEWIDTH - state->hffArgs.topicTitleColsWidth, (int) ' ',
        MAXPAGENUMWIDTH - state->hffArgs.pageNumColsWidth, (int) ' ',
        MAXSECTNUMWIDTH - state->hffArgs.sectNumColsWidth, (int) ' ',
        &lastValid);
    buf[lastValid] = EOS;	/* truncate unused args */
    free(formattedStr);

    /*** output the str ***/
    fprintf(topicsFP, "%s", buf);

    return lineCnt;
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  PrintFooter()
$1LINER$:  Print footer and use right form for odd/even pages
$DESCRIPT$:
Print footer and use right form for odd/even pages
$RETURNS$:
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, increment
           lines), but do not output;
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void PrintFooter(
  _DtHPrOptions * options,
  FILE *          topicsFP,
  PrintState *    state,
  _DtHPrHeadFoot * headFootInfo,
  HeadFootFormat * headFootFormatting)
{       /*$CODE$*/
   /* fill to bottom of page */
   OutputBlankLines(topicsFP,state,
        (options->rowsTextHeight - state->curLineNumber) - 
              ( (state->curPageNumber % 2) == 0 
                  ? headFootFormatting->evenFooterLineCnt 
                  : headFootFormatting->oddFooterLineCnt));  

   if ( (state->curPageNumber % 2) == 0)  /* Even page */
       state->curLineNumber += PrintHeadFootStr(options,topicsFP,state,
                                 headFootFormatting->formattedEvenFooter,
                                 headFootFormatting->evenFooterLineCnt);
   else  /* odd page */
       state->curLineNumber += PrintHeadFootStr(options,topicsFP,state,
                                 headFootFormatting->formattedOddFooter,
                                 headFootFormatting->oddFooterLineCnt);
}  /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  PrintHeader()
$1LINER$:  Print footer and use right form for odd/even pages
$DESCRIPT$:
Print footer and use right form for odd/even pages
$RETURNS$:
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, increment
           lines), but do not output;
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void PrintHeader(
  _DtHPrOptions * options,
  FILE *          topicsFP,
  PrintState *    state,
  _DtHPrHeadFoot * headFootInfo,
  HeadFootFormat * headFootFormatting)
{       /*$CODE$*/

   if ( (state->curPageNumber % 2) == 0)  /* Even page */
       state->curLineNumber += PrintHeadFootStr(options,topicsFP,state,
                                 headFootFormatting->formattedEvenHeader,
                                 headFootFormatting->evenHeaderLineCnt);
   else
       state->curLineNumber += PrintHeadFootStr(options,topicsFP,state,
                                 headFootFormatting->formattedOddHeader,
                                 headFootFormatting->oddHeaderLineCnt);
}  /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  NewPage()
$1LINER$:  Outputs paper form feed, increments page count, resets line cnt
$DESCRIPT$:
Outputs paper form feed, increments page count, resets line cnt
$RETURNS$:
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, increment
           page and reset line number), but do not output;
           If not NULL, request new page as well
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void NewPage(
  _DtHPrOptions * options,
  FILE *          topicsFP,
  PrintState *    state,
  Boolean         advancePage)
{       /*$CODE$*/
    /* start new page: form feed */
    if (topicsFP) 
    {
       if (advancePage) fprintf(topicsFP,"\n");

       /* print top margin but don't add it to line count because the rowsTextHeight
          value is calculated post-topMargin */
       OutputBlankLines(topicsFP,state,options->rowsAdjTopMargin);
    }

    /* adjust page and line numbers */
    if (advancePage) state->curPageNumber++;
    state->curLineNumber = 1;
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  ProcessOneTopic()
$1LINER$:  Recovers and formats help text for one topic
$DESCRIPT$:
Recovers and formats help text for one topic
$RETURNS$:
 0:  The number of lines output.
-2:  could not get topic information
$ARGS$:
topicsFP:  pointer to output stream.  If NULL, process topic
           (e.g. count lines and inc page numbers), but do not output;
           If not NULL, output lines and page headers
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int ProcessOneTopic(
  _DtHPrOptions * options,
  FILE *          topicsFP,
  PrintState *    state,
  Boolean         printHeaderFooter)
{       /*$CODE$*/
   char * * helpList = NULL;
   char * * ptrToLst;
   int      lineCount;
   int      availLines;
   char sectNumStr[MAXSECTS * 4 + 5];

   /* retrieve the text (but not the hyperlinks) from the volume */
   if ( _DtHelpTermGetTopicData(state->canvasHandle, state->volHandle,
                       state->currentLocId, &helpList, NULL) != 0 )
   {
      fprintf(stderr,_DTGETMESSAGE(PTSET,5,
                           "%s Error: unable to get topic information:\n"
                           "volume %s, locationId %s\n"),
                     options->programName, options->helpVolume, options->locationId);
      return -2;          		/* RETURN error */
   }

   /* output topic section number */
   /* this operates on the assumption that topic title is first line */
   ptrToLst = helpList;
   SectNumStr(state->sectNums,state->sectStr,sectNumStr);
   if (sectNumStr[0] != EOS)  
   {
      /* NOTE: if allow the sect num string to be resource-defined,
         then count the number of \n chars in it; don't assume how many. */
      if (topicsFP) fprintf(topicsFP,"\n");
      OutputBlankSpaces(topicsFP,options->colsAdjLeftMargin);
      if (topicsFP) fprintf(topicsFP,"%s ", sectNumStr);
      state->curLineNumber++;

      /* and put the title (must be on first two lines) on the same line */
      if (   (*helpList && (*helpList)[0] != EOS)
          || (*(++helpList) && (*helpList)[0] != EOS) )
      {
        if (topicsFP) {
	   char *_p;
           int ret;

           ret = IconvBuffer( state->volHandle, *helpList, &_p );
	   if ( ret ) {
               fprintf(topicsFP,"%s\n", _p); /* output title */
           } else {
               fprintf(topicsFP,"%s\n", *helpList); /* output title */
           }
	}
         state->curLineNumber += CountLines(*helpList) + 1; /* 1=the known \n */
         helpList++;
      }
   }

   /* calc number of available lines from top of page to footer */
   availLines = AvailContentLines(options,state,&state->bodyHFF);

   /* cycle through the lines; add new pages where necessary */
   for ( ; *helpList != NULL; helpList++ )
   {
      /* NOTE: it's impt to calc the final line before outputting it,
         as the line may contain embedded newlines */
      int linesCnt = CountLines(*helpList) + 1;     /* 1=the known \n */ 

      /* calc what line that will leave us on */
      state->curLineNumber += linesCnt;
 
      /* if at the end of a page, print footer, eject, and print header */
      if (state->curLineNumber >= availLines)
      {
         /* output any filler blank lines */
         OutputBlankLines(topicsFP,state,
                   availLines - (state->curLineNumber - linesCnt) );
         if (printHeaderFooter) 
            PrintFooter(options,topicsFP,state,&options->bodyHF,&state->bodyHFF);
         NewPage(options,topicsFP,state,True);
         if (printHeaderFooter) 
            PrintHeader(options,topicsFP,state,&options->bodyHF,&state->bodyHFF);

         /* recalc the line we're on */
         state->curLineNumber += linesCnt;

         /* calc number of available lines from top of page to footer */
         availLines = AvailContentLines(options,state,&state->bodyHFF);
      }

      /* output the lines */
      if (topicsFP) 
      {
         OutputBlankSpaces(topicsFP,options->colsAdjLeftMargin);
	{
	    char *_p;
	    int ret;

	    ret = IconvBuffer( state->volHandle, *helpList, &_p );
	    if ( ret ) {
               fprintf(topicsFP,"%s\n", _p );
	    } else {
               fprintf(topicsFP,"%s\n",*helpList);
	    }
	}
      }
   }

   /* free the memory of helpList */
   _DtHelpFreeTopicData(ptrToLst,NULL);

   return 0;
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  ProcessSubTopics()
$1LINER$:  Recovers and formats help text for current & sub topics
$DESCRIPT$:
     ond none should be printed. */

Subsections are numbered according to the legal method (e.g. 1.5.4.3)

Recovers and formats help text for current & sub topics
$RETURNS$:
 0:  success
-1:  could not create or open a temp file
-2:  could not get help topic info
INHIBIT_OUTPUT:  stop outputting children; we printed all we need to
$ARGS$:
level:   may range from 1 to (MAXSECTS-1)
========================================================$SKIP$=====*/
#endif /*DOC*/
static
int ProcessSubTopics(
  _DtHPrOptions * options,
  FILE *          topicsFP,
  Toc *           toc,
  int             level,
  PrintState *    state)
{       /*$CODE$*/
   char * * children = NULL;
   int      ret = 0;
   int      firstSubSectNum;
   int      subSectNumIndex;
   FILE *   curFP;

#define INHIBIT_OUTPUT (-3)
   /* remember, don't turn inhibitOutput on if there's no match */
   if ( _DtHelpCeStrCaseCmp(state->outputFromLocId,state->currentLocId) == 0 )
   {
      int curPos;

      /* toggle the flag and set ret value */
      state->inhibitOutput = False;
      ret = INHIBIT_OUTPUT;     /* stop printing after this topic & its children */

      curPos = state->curLineNumber;  /* save off the current line position */

      /* start new page but don't eject; output header;
         output right number of blank lines to get to where topic starts */
      NewPage(options,topicsFP,state,False);
      PrintHeader(options,topicsFP,state,&options->bodyHF,&state->bodyHFF);
      OutputBlankLines(topicsFP,state,curPos - state->curLineNumber);
   }

   /* init file ptr */
   curFP = (state->inhibitOutput ? (FILE *) NULL : topicsFP);

   /* init according to level */
   if(level == 0)   /* the top topic? */
   {  /* put top topic at same level as children */
      firstSubSectNum = 2;
      subSectNumIndex = 1;
      state->level = 1;
      /* no prior instance of ProcessSubTopics() set this */
      state->sectNums[subSectNumIndex] = 1;  
   }
   else /* not the top topic */
   {
      firstSubSectNum = 1;
      subSectNumIndex = level;
      state->level = level;
   }

   /* add this topic to the toc */
   TocNewEntry(toc,state->currentLocId,state->curPageNumber,
                           state->level,state->sectNums,state->sectStr);

   /* retrieve and possibly output the current topic */
   ProcessOneTopic(options,curFP,state,True);

   /* output the sub topics */
   if (_DtHelpCeGetTopicChildren(state->volHandle,
                            state->currentLocId,&children) > 0)
   {
      char * * topic;
      Boolean  initSubSect = True;

      /* cycle through the topics and output each one */
      for (topic = children; *topic != NULL && (*topic)[0]; topic++ )
      {
         /* start level 0 & 1 topics on a new page */
         if ( subSectNumIndex < 2 )
         {
            PrintFooter(options,curFP,state,&options->bodyHF,&state->bodyHFF);
            NewPage(options,curFP,state,True);
            PrintHeader(options,curFP,state,&options->bodyHF,&state->bodyHFF);
         }

         /* avoid orphans (i.e. require more than 4 lines left on page) */
         if ( (AvailContentLines(options,state,&state->bodyHFF) 
                                               - state->curLineNumber) < 4 )
         {
             PrintFooter(options,curFP,state,&options->bodyHF,&state->bodyHFF);
             NewPage(options,curFP,state,True);
             PrintHeader(options,curFP,state,&options->bodyHF,&state->bodyHFF);
         }

         /* init new sub sect, if haven't yet done so */
         if (initSubSect)
         {
            /* start new subsection; -1: make up for ++ later */
            state->sectNums[subSectNumIndex] = firstSubSectNum - 1;
            initSubSect = False;
         }

         /* make this the current topic */
         state->currentLocId = *topic;
         /* use subSectNumIndex, not level, so that the top topic and its
            immediate children are at the same level */
         /* inc the sect num before the call so that the
            sect used below in the INHIBIT_OUTPUT wrapup is correct. */
         /* pass in topicsFP, not curFP */
         state->sectNums[subSectNumIndex]++;     /* next subsection */
         /* even though ProcessSubTopics() can return 'INHIBIT_OUTPUT',
            don't stop processing, as this would cause the Toc to
            be incomplete. */
         ProcessSubTopics(options,topicsFP,toc, subSectNumIndex+1,state);
         /* ProcessSubTopics(options,topicsFP,toc, state->level+1,state); */
      }
   }

   /* if this is the last topic to be output, then finish up */
   if (ret == INHIBIT_OUTPUT)
   {
      /* save off line num */
      int curLine = state->curLineNumber;

      /* output footer */
      PrintFooter(options,curFP,state,&options->bodyHF,&state->bodyHFF);
      state->inhibitOutput = True;	/* inhibit again */

      /* restore line num */
      state->curLineNumber = curLine;
   }

   /* if processing subtopics, reset subsection number */
   if(subSectNumIndex > 1) state->sectNums[subSectNumIndex] = 0;
   state->level = level;	/* state->level was modified by the FOR loop */
   return ret;
}


#if DOC
===================================================================
$PFUNBEG$:  ProcessFrontMatter()
$1LINER$:  Process the help volume to generate a file of front matter
$DESCRIPT$:
Process the help volume to generate a file of front matter
$RETURNS$:
 0:  success
-1:  could not create or open a temp file
-2:  could not get help topic info
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int ProcessFrontMatter(
  _DtHPrOptions * options,
  char * *        ret_resultsFile,
   PrintState *   state)
{       /*$CODE$*/
   FILE *  fp;
   int     ret;

   /* open file */
   fp = OpenTmpFile(options,ret_resultsFile);
   if (NULL == fp) return -1;               /* RETURN: error */

   /*** process text ***/
   /* setup the state */
   state->inhibitOutput = False;
   state->curPageNumber = 1;
   state->curLineNumber = 1;
   state->sectNums[0] = 0;      /* inhibit section num printing */
   state->sectStr = NULL;       /* inhibit section string printing */

   OutputBlankLines(fp,state, options->rowsTextHeight / 3);

   /* NOTE: the code below causes the memory allocated for currentLocId
      to be lost.  I didn't fix this to save a few bytes of code space. */

   /* generate volume title */
   state->currentLocId = strdup("_TITLE");
   state->outputFromLocId = state->currentLocId;
   ret = ProcessOneTopic(options,fp,state,False);
   /*free(state->currentLocId);*/

   NewPage(options,fp,state,True);

   /* generate abstract */
   state->currentLocId = strdup("_ABSTRACT");
   state->outputFromLocId = state->currentLocId;
   ret = ProcessOneTopic(options,fp,state,False);
   /*free(state->currentLocId);*/ 

   /* Make a space between abstract and copyright */
   if ( AvailContentLines(options,state,&state->indexHFF) > 3 )
      OutputBlankLines(fp,state, 3);
   else
      NewPage(options,fp,state,True);

   /* generate copyright */
   state->currentLocId = strdup("_COPYRIGHT");
   state->outputFromLocId = state->currentLocId;
   ret = ProcessOneTopic(options,fp,state,False);
   /*free(state->currentLocId);*/
   /*state->currentLocId = NULL;*/

   if (fp) fclose(fp);

   return ret;
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  ProcessIndex()
$1LINER$:  Process the TOC data to generate an index file
$DESCRIPT$:
Process the TOC data to generate an index file
$WARNING$:
This function uses state->pageNumber+1 as the first page
of the index.
$RETURNS$:
 0:  success
-1:  could not create or open a temp file
-2:  could not get help topic info
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int ProcessIndex(
  _DtHPrOptions * options,
  char * *        ret_resultsFile,
   PrintState *   state,
   Toc *          toc)
{       /*$CODE$*/
   FILE *   fp;
   int      ret;
   int      availLines;
   char * * indexEntriesList;
   char *   entryStr;
   int      entriesCnt;

   *ret_resultsFile = NULL;

   /* Get the index.  Recall that the array and strings pointed to by
      the indexEntriesList is owned by the open volume */
   entriesCnt = _DtHelpCeGetKeywordList(state->volHandle,&indexEntriesList);
   if (entriesCnt <= 0) return 0;

   /* open file */
   fp = OpenTmpFile(options,ret_resultsFile);
   if (NULL == fp) return -1;               /* RETURN: error */

   /*** process index ***/

   /* setup the state */
   state->curPageNumber++;         /* inc page number from prev. value */
   state->inhibitOutput = False;
   state->curLineNumber = 1;
   state->sectNums[0] = 0;      /* inhibit section num printing */
   state->sectStr = _DTGETMESSAGE(PTSET,10,"Index");  /* $SECTNUM name */

   /* add an index entry to the TOC */
   TocNewEntry(toc,_DTGETMESSAGE(PTSET,30,"__GENERATED-INDEX"),
                 state->curPageNumber, 1,state->sectNums,state->sectStr);

   /* start new page (but don't eject) and output header */
   NewPage(options,fp,state,False);
   PrintHeader(options,fp,state,&options->indexHF,&state->indexHFF);

   /* calc number of available lines from top of page to footer */
   availLines = AvailContentLines(options,state,&state->indexHFF);

   /*** loop through the entries ***/
   for ( entryStr = *indexEntriesList++;
         NULL != entryStr;
         entryStr = *indexEntriesList++ )
   {
      char * * topicIdsList;
      int      topicCnt;
      int      i;
      TocEntry * tocEntry;

      /* get the topics associated with this entry */
      /* topicIdsList is set but not allocated & need not be freed */
      topicCnt = _DtHelpCeFindKeyword(state->volHandle,
                               entryStr,&topicIdsList);
      if (topicCnt <= 0) continue;             /* CONTINUE */

      /*** if only one topic, just put page number after the entry ***/
      if (topicCnt == 1)
      {
         /* find the toc entry of the topic */
         ret = TocFindSortedEntry(toc, *topicIdsList, NULL, &tocEntry);
         if (ret < 0) continue;                 /* CONTINUE */

         /* output the entry */
         OutputBlankSpaces(fp,options->colsAdjLeftMargin);
         {
             char *_p;
             int ret;

             ret = IconvBuffer( state->volHandle, entryStr, &_p );
	     if ( ret ) {
                 fprintf(fp, _DTGETMESSAGE(PTSET,20,"%s, %d\n"), 
                         _p, tocEntry->pageNumber); 
             } else {
                 fprintf(fp, _DTGETMESSAGE(PTSET,20,"%s, %d\n"), 
                         entryStr, tocEntry->pageNumber); 
             }
         }
         state->curLineNumber++;

         /* if at the end of a page, print footer, eject, and print header */
         if (state->curLineNumber >= availLines)
         {
             PrintFooter(options,fp,state,&options->indexHF,&state->indexHFF);
             NewPage(options,fp,state,True);
             PrintHeader(options,fp,state,&options->indexHF,&state->indexHFF);
             /* calc number of available lines from top of page to footer */
             availLines = AvailContentLines(options,state,&state->indexHFF);
         }

         continue;                              /* CONTINUE */
      }

      /*** if more than one topic, list topic title & page number under the entry ***/

      /* do we need a new page to start this entry? */
      if ( (availLines - state->curLineNumber) < 3 )
      {
          PrintFooter(options,fp,state,&options->indexHF,&state->indexHFF);
          NewPage(options,fp,state,True);
          PrintHeader(options,fp,state,&options->indexHF,&state->indexHFF);
          /* calc number of available lines from top of page to footer */
          availLines = AvailContentLines(options,state,&state->indexHFF);
      }

      OutputBlankSpaces(fp,options->colsAdjLeftMargin);
      {
          char *_p;
          int  ret;

          ret = IconvBuffer( state->volHandle, entryStr, &_p );
          if ( ret ) {
              fprintf(fp,"%s\n", _p ); 
          } else {
              fprintf(fp,"%s\n", entryStr); 
          }
      }
      state->curLineNumber++;

      /* for all topics in an index entry */
      for ( i=0; 
            i<topicCnt; 
            i++, topicIdsList++ )
      {
         char * topicTitle;

         if (NULL == *topicIdsList) continue;   /* CONTINUE */

         /* find the toc entry of the topic */
         ret = TocFindSortedEntry(toc, *topicIdsList, NULL, &tocEntry);
         if (ret < 0) continue;                 /* CONTINUE */

         /* get the topic title */
         _DtHelpCeGetTopicTitle(state->canvasHandle, state->volHandle,
                              tocEntry->locationId, &topicTitle);
         if (NULL == topicTitle) 
            topicTitle = _DTGETMESSAGE(PTSET,31,EMPTY_STR);

         /* output the entry */
         OutputBlankSpaces(fp,options->colsAdjLeftMargin);
         {
             char *_p;
             int ret;

             ret = IconvBuffer( state->volHandle, topicTitle, &_p );
             if ( ret ) {
                 fprintf(fp,_DTGETMESSAGE(PTSET,21,"     %s, %d\n"), 
                      _p, tocEntry->pageNumber); 
             } else {
                 fprintf(fp,_DTGETMESSAGE(PTSET,21,"     %s, %d\n"), 
                      topicTitle, tocEntry->pageNumber); 
             }
         }
         state->curLineNumber++;

         /* if at the end of a page, print footer, eject, and print header */
         if (state->curLineNumber >= availLines)
         {
             PrintFooter(options,fp,state,&options->indexHF,&state->indexHFF);
             NewPage(options,fp,state,True);
             PrintHeader(options,fp,state,&options->indexHF,&state->indexHFF);
             /* calc number of available lines from top of page to footer */
             availLines = AvailContentLines(options,state,&state->indexHFF);
         }
      }  /* for all topics in an index entry */

   }  /* for all index entries */

   /* output footer */
   PrintFooter(options,fp,state,&options->indexHF,&state->indexHFF);

   if (fp) fclose(fp);

   return 0;
}       /*$END$*/



#if DOC
===================================================================
$PFUNBEG$:  ProcessToc()
$1LINER$:  Process the TOC data to generate a toc file
$DESCRIPT$:
Process the TOC data to generate a toc file
$RETURNS$:
 0:  success
-1:  could not create or open a temp file
-2:  could not get help topic info
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int ProcessToc(
  _DtHPrOptions * options,
  char * *        ret_resultsFile,
   PrintState *   state,
   Toc *          toc)
{       /*$CODE$*/
   FILE *  fp;
   int     availLines;
   TocEntry * entry;

   /* open file */
   fp = OpenTmpFile(options,ret_resultsFile);
   if (NULL == fp) return -1;               /* RETURN: error */

   /*** process toc ***/

   /* setup the state */
   state->inhibitOutput = False;
   state->curPageNumber = 1;
   state->curLineNumber = 1;
   state->sectNums[0] = 0;      /* inhibit section num printing */
   state->sectStr = _DTGETMESSAGE(PTSET,11,"Table of Contents"); /* $SECTNUM name */

   /* start new page and output header */
   NewPage(options,fp,state,False);
   PrintHeader(options,fp,state,&options->tocHF,&state->tocHFF);

   /* calc number of available lines from top of page to footer */
   availLines = AvailContentLines(options,state,&state->tocHFF);

   /* walk through the toc; output the label, title string, and page num */
   for ( entry = TocNextEntry(toc,NULL);
         NULL != entry;
         entry = TocNextEntry(toc,entry) )
   {
       char   sectNumStr[MAXSECTS * 4 + 5];	/* $SECTNUM */
       char * title = NULL;
       wchar_t * wctitle;
       int    lhsWidth;
       int    titlelen;
       int    fillerChar;
       int    blanksCnt;

       /* get the data to print */
       SectNumStr(entry->sectNums,entry->sectStr,sectNumStr);
       _DtHelpCeGetTopicTitle(state->canvasHandle, state->volHandle,
                              entry->locationId, &title);
       if (NULL == title) title = EMPTY_STR;

       /* set max length of title and calc num blanks needed */
       lhsWidth = strlen(sectNumStr) + 1;   /* 1=blank after sect num */
       titlelen = strlen(title);
       wctitle = malloc(sizeof(wchar_t) * (titlelen+1));
       /* truncate the title using wchar_t */
       if(wctitle)
       {
          /* convert title to wchar_t and get num chars */
          mbstowcs(wctitle,title,titlelen+1);
          titlelen = wcslen(wctitle);
          /* truncate the title to fit on the line */
          if (titlelen > options->colsTextWidth - lhsWidth)
          {
             wctitle[options->colsTextWidth - lhsWidth - 1] = EOS;
             titlelen = wcslen(wctitle);
          }
          wcstombs(title,wctitle,titlelen+1);
          free(wctitle);
       }
       /* truncate the title assuming single-byte */
       else if (titlelen > options->colsTextWidth - lhsWidth)
       { 
          title[options->colsTextWidth - lhsWidth - 1] = EOS;
          titlelen = strlen(title);
       }
       lhsWidth += titlelen;

       /* add a blank line before a major section */
       if ( 0 == entry->sectNums[2] )
       {
          fprintf(fp,"\n");
          state->curLineNumber++;

          /* do we need a new page to start this section? */
          if ( (availLines - state->curLineNumber) < 3 )
          {
              PrintFooter(options,fp,state,&options->tocHF,&state->tocHFF);
              NewPage(options,fp,state,True);
              PrintHeader(options,fp,state,&options->tocHF,&state->tocHFF);
              /* calc number of available lines from top of page to footer */
              availLines = AvailContentLines(options,state,&state->tocHFF);
          }
       }

       /* output the beginning of the line */
       if ( sectNumStr[0] == EOS )      /* no section number */
       {
          /* space between title and page num */
          /* 1: no sect num so no space betw sect num & title */
          /* -3: reserve 3 spaces for the page number */
          blanksCnt = ((options->colsTextWidth - lhsWidth) - 3) + 1;

          {
              char *_p;
              int ret;

              ret = IconvBuffer( state->volHandle, title, &_p );
              if ( ret ) {
                  fprintf(fp,"%*c%s", options->colsAdjLeftMargin, ' ', _p );
              } else {
                  fprintf(fp,"%*c%s", options->colsAdjLeftMargin, ' ', title);
              }
          }
       }

       else     /* valid section number */
       {
          /* space between title and page num */
          /* -3: reserve 3 spaces for the page number */
          blanksCnt = (options->colsTextWidth - lhsWidth) - 3;

          {
              char *_p;
              int ret;

              ret = IconvBuffer( state->volHandle, title, &_p );
              if ( ret ) {
                fprintf(fp,"%*c%s %s",
                      options->colsAdjLeftMargin, ' ', sectNumStr, _p );
              } else {
                fprintf(fp,"%*c%s %s",
                      options->colsAdjLeftMargin, ' ', sectNumStr, title);
              }
          }
       }

       /* output the filler and page number */
       if ( (blanksCnt % 2) != 0) { fputc(' ',fp); blanksCnt--; }
       for( ;blanksCnt > 0; blanksCnt -= 2) fprintf(fp," .");
       fprintf(fp,"%3d\n", entry->pageNumber);

       state->curLineNumber++;
 
       /* if at the end of a page, print footer, eject, and print header */
       if (state->curLineNumber >= availLines)
       {
           PrintFooter(options,fp,state,&options->tocHF,&state->tocHFF);
           NewPage(options,fp,state,True);
           PrintHeader(options,fp,state,&options->tocHF,&state->tocHFF);
           /* calc number of available lines from top of page to footer */
           availLines = AvailContentLines(options,state,&state->tocHFF);
       }
   }  /* for all entries */

   /* output footer */
   PrintFooter(options,fp,state,&options->tocHF,&state->tocHFF);

   if (fp) fclose(fp);

   return 0;
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  ProcessTopics()
$1LINER$:  Process the help volume to generate a topics file
$DESCRIPT$:
Process the TOC data to generate a toc file
$RETURNS$:
 0:  success
-1:  could not create or open a temp file
-2:  could not get help topic info
$ARGS$:
outputFromLocId:  output of topics occurs for this topic and all its
                  subtopics.  This topic Id is strcasecmp()ed against the
                  current Id to determine a match.  If NULL, the volume
                  top topic is used.
inhibitTopicsOutput:    if true, output only starts when outputFromLocId is found

outputFromLocId  inhibitOutput   action
---------------  -------------   -------------------------------------
NULL              True            no output is generated
NULL              False           output begins with the top topic
!= NULL           True            output begins with outputFromLocId
                                    and stops after last subtopic of
                                    outputFromLocId
!= NULL           False           output begins with the top topic
                                    and stops after last subtopic of
                                    outputFromLocId
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int ProcessTopics(
  _DtHPrOptions * options,
  char * *        ret_resultsFile,
   PrintState *   state,
   Toc *          toc,
  Boolean         processSubTopics,
  char *          outputFromLocId,
  Boolean         inhibitTopicsOutput)
{       /*$CODE$*/
   FILE *  fp;
   int     ret;

   /* don't print a single topic with no locId */
   if (    processSubTopics == False
        && (outputFromLocId == NULL || *outputFromLocId == EOS) )
       return 0;                                /* RETURN: ok */

   /* if no output is desired, set ptr to NULL */
   if ( NULL == outputFromLocId && inhibitTopicsOutput )
       fp = NULL;
   else
   {  /* output is desired...open file */
      fp = OpenTmpFile(options,ret_resultsFile);
      if (NULL == fp) return -1;               /* RETURN: error */
   }

   /*** process text ***/

   /* if processing subtopics, start processing at the top */
   if ( processSubTopics )
   {
      char * name = NULL;
      int    offset;

      /* get the top topic of the volume */
      ret = _DtHelpCeGetTopTopicId(state->volHandle, &state->currentLocId);
      if (ret != True) state->currentLocId = strdup("_HOMETOPIC");
      if(name) free(name);
   }
   else
   {   /* otherwise, process only where needed */
       state->currentLocId = strdup(outputFromLocId);
   }

   /* set the other state values */
   if (   NULL != outputFromLocId
       && (ret = _DtHelpCeIsTopTopic(state->volHandle,outputFromLocId)) != 0 )
      state->outputFromLocId = outputFromLocId;
   else
      state->outputFromLocId = state->currentLocId;
   state->inhibitOutput = inhibitTopicsOutput;
   state->curPageNumber = 1;
   state->curLineNumber = 1;

   /* output topics but dont inhibit */
   if ( processSubTopics )
   {
      state->sectNums[0] = 1;      /* support section num printing */
      /* 0: level of the top topic; note tha this is diff from the
         level of the top topic in an SDL volume, which is 1.  This
         is merely for convenience, to make addressing into the
         sectNums array directly rather than subtracting 1 or 2 to
         get the array index from the level. */
      ret = ProcessSubTopics(options,fp,toc,0,state);
   }
   else
   {
      /* start new page but don't eject; output header;
         output right number of blank lines to get to where topic starts */

      state->sectNums[0] = 0;      /* inhibit section num printing */
      NewPage(options,fp,state,False);        /* for top margin */
      PrintHeader(options,fp,state,&options->bodyHF,&state->bodyHFF);

      ret = ProcessOneTopic(options,fp,state,True);

      /* output footer */
      PrintFooter(options,fp,state,&options->bodyHF,&state->bodyHFF);
   }

   /* if the file was opened, close & check if anything written to it */
   /* if empty, pretend the file wasn't created. */
   if (fp)
   {
      Boolean empty = False;
      long int pos;
      pos = ftell(fp);
      if (pos <= 0) empty = True;
      fclose(fp);
      if (empty)
      {
         unlink(*ret_resultsFile);
         free(*ret_resultsFile);
         *ret_resultsFile = NULL;
      }
   }

   return ret;
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  MakePageBreakFile()
$1LINER$:  Creates a file with only a page break char in it
$DESCRIPT$:
Creates a file with only a page break char in it.
If fails, file ptr is NULL
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void MakePageBreakFile(
  _DtHPrOptions * options,
  char * *        ret_resultsFile)
{       /*$CODE$*/
   FILE * fp;

   fp = OpenTmpFile(options,ret_resultsFile);
   if (NULL == fp) 
   { 
      free(*ret_resultsFile); 
      *ret_resultsFile = NULL; 
      return; 
   }
   fprintf(fp,"\n");
   fclose(fp);
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  DoHelpTopicsProcessing()
$1LINER$:  Passes through entire volume and generates toc and topic output
$DESCRIPT$:
Passes through entire volume and generates toc and topic output
$RETURNS$:
 0:  success
-1:  could not create or open a temp file
-2:  could not get help topic info
-3:  could not alloc memory
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int DoHelpTopicsProcessing(
  _DtHPrOptions * options,
  char * *        ret_resultsFile, 
  Boolean         processToc,
  Boolean         processIndex,
  Boolean         processFrontMatter,
  Boolean         processSubTopics,
  char *          outputFromLocId,
  Boolean         inhibitTopicsOutput)
{       /*$CODE$*/
#define FM     0
#define TOC    1
#define TOPICS 2
#define INDEX  3
#define NUMPARTS  4
   
   PrintState  state;
   Toc         toc;
   int         ret = 0;
   int         i;
   char *      buf;
   char *      start;
   char *      next;
   char *      pgbrkFile;
   char *      partFiles[NUMPARTS];
   Boolean     validFile = False;

   /* init larger vars to 0 */
   memset(&toc,0,sizeof(Toc));
   memset(&state,0,sizeof(state));
   memset(partFiles,0,sizeof(partFiles));

   /* create terminal-based canvas for text retrieval */
   ret = _DtHelpTermCreateCanvas(options->colsTextWidth,&state.canvasHandle);
   if ( ret != 0 ) return ret;             /* RETURN: error */

   /* open volume and get state.volHandle */
   ret = _DtHelpCeOpenVolume(state.canvasHandle, 
                             options->helpVolume, &state.volHandle);
   if ( ret != 0 ) return ret;             /* RETURN: error */

   /* do some prepatory format string generation */
   GenAllHeadFootFormatStrs(options,&state);

   /* after this point, if we get an error, we don't return--we do the best we can */
   /* so don't monitor the return values. */

   /*** process a help volume; maybe create a topics file ***/
   /* always do this, as all other processing (except FM) needs the data */
   ret=ProcessTopics(options,&partFiles[TOPICS],&state,&toc,
               processSubTopics,outputFromLocId,inhibitTopicsOutput);

   /*** process toc to create an index and add an index entry to toc ***/
   /* Do this right after processing topics so that page number info is right */
   if (processIndex)
      ret=ProcessIndex(options,&partFiles[INDEX],&state,&toc);

   /*** process volume to create front matter ***/
   if (processFrontMatter)
      ret=ProcessFrontMatter(options,&partFiles[FM],&state);

   /*** process toc to create a toc; do this last to get all entries ***/
   if (processToc)
      ret=ProcessToc(options,&partFiles[TOC],&state,&toc);

   /*** create the final output file ***/

   MakePageBreakFile(options,&pgbrkFile);

   /* create a temporary file in $HOME/.dt/tmp to hold everything */
   /* Put in $HOME/.dt/tmp so that if the printer operation is running
      of a remote system, it can get to the (local) temp file. 
      This would not be possible if the file were put in /tmp */
   *ret_resultsFile = _DtHPrCreateTmpFile(TMPFILE_PREFIX,TMPFILE_SUFFIX);
   if (NULL == *ret_resultsFile) { ret = -1; goto cleanup; } /* RETURN: error */

   buf = malloc(3000 * sizeof(char));
   if (NULL == buf) { ret = -3; goto cleanup; }      /* RETURN: error */

   /* cat together the part files that got created in the array order */
   /* separate them with a file containing only a page break */
   strcpy(buf,"cat ");
   start = buf + strlen(buf);
   next = start;
   for (i=0; i<NUMPARTS; i++)
   {
     if (partFiles[i]) 
     { 
        /* if at least one file in cat list, separate from it with a pgbrk */
        if (start != next && pgbrkFile)
        {
           sprintf(next,"%s ", pgbrkFile);
           next += strlen(next);
        }
        sprintf(next,"%s ", partFiles[i]);
        next += strlen(next);
        validFile = True;
     }
   }
   /* only do the operation if there are valid files */
   if (validFile)
   {
      int rv;
      sprintf(next,"> %s", *ret_resultsFile);
      if(options->debugHelpPrint) printf("%s\n",buf);
      rv = system(buf);
   }
   free(buf);
   ret = 0;

   /* if needed, iconv file and change filenames */

   /* close the volume when done */
   _DtHelpCeCloseVolume(state.canvasHandle,state.volHandle);

   /*** cleanup ***/
cleanup:
   /* delete all part files */
   for (i=0; i<NUMPARTS; i++)
     if (partFiles[i]) 
     { 
        unlink(partFiles[i]); 
        free(partFiles[i]);
     }
   unlink(pgbrkFile);

   /* NOTE: should free Toc here if interested in no leaks */

   return ret;
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  GenerateTopicsFile()
$1LINER$:  Recovers and formats help topic text and then sends to printer
$DESCRIPT$:
Recovers and formats help topic text and then sends to printer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
int GenerateTopicsFile(
  Display *       dpy,
  _DtHPrOptions * options,
  char * *        ret_resultsFile)
{       /*$CODE$*/
   int ret = 0;
   Boolean allTopics = (options->allTopics != NULL);
   Boolean subTopics = (options->subTopics != NULL);
   Boolean toc = (options->toc != NULL);
   Boolean index = (options->index != NULL);
   Boolean frontMatter = (options->frontMatter != NULL);
   Boolean oneTopic = (options->oneTopic != NULL);
   Boolean dfltOneTopic;
   char *  locationId = options->locationId;

   /* one topic is selected if other options aren't selected */
   dfltOneTopic = !(allTopics || subTopics || toc || index);

   /* This is sort of a kludge, but since we don't require the -One flag,
      this keeps the _Hometopic from being printed along with 
      the front matter, when only front matter was specified */
   if (frontMatter && !oneTopic && dfltOneTopic)
      locationId = NULL;   /* prevent printing the hometopic */

   /* to generate a toc or index, we need to process all topics; if
      processing all or subtopics wasn't requested, request it;
      and set a locationId that will never match (an empty string) */
   if ( (toc || index) && !allTopics ) 
   { 
      if (!subTopics) locationId = ""; 
      subTopics = True;
   }

   /* check the flags and do right thing */
   if (oneTopic || dfltOneTopic)
      /* False: no subtopics--output only locationId; False: don't inhibit output */
      ret = DoHelpTopicsProcessing(
                   options, ret_resultsFile, 
                   False, False, frontMatter,
                   False, locationId, False);
   else if (allTopics)
      /* start at root and don't inhibit output */
      ret = DoHelpTopicsProcessing(
                   options, ret_resultsFile, 
                   True, True, True,
                   True, NULL, False);
   else
      ret = DoHelpTopicsProcessing(
                   options, ret_resultsFile, 
                   toc, index, frontMatter,
                   subTopics, locationId,True);

   return ret;
}       /*$END$*/



#if DOC
===================================================================
$FUNBEG$:  _DtHPrPrintHelpTopic()
$1LINER$:  Recovers and formats help topic text and then sends to printer
$DESCRIPT$:
Recovers and formats help topic text and then sends to printer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int _DtHPrPrintHelpTopic(
   Display *      dpy,
  _DtHPrOptions * options)
{       /*$CODE$*/
   char * printCommand;
   char   cmdFormat[100];
   char   prOffsetArg[30];
   char * tmpFile;
   int    status;
   char * path;


    if ( NULL == options->helpVolume )
    {
         fprintf(stderr, _DTGETMESSAGE(PTSET,1,
                             "%s: Error: helpType is topic, "
                             "but no helpVolume specified.\n"), 
                             options->programName);

         return 1;                      /* RETURN error */
    }

   /* try to locate the volume */
   path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, options->helpVolume,
                               _DtHelpFileSuffixList,False,R_OK);
   if (path == NULL)
   {
         fprintf(stderr, _DTGETMESSAGE(PTSET,2,
                             "%s Error: unable to locate help volume '%s'\n"),
                             options->programName, options->helpVolume);

         return 1;                      /* RETURN error */
   }
   options->helpVolume = path;  /* don't free old helpVolume: owned by Xrm */

   /* generate a file containing the help topic */
   status = GenerateTopicsFile(dpy,options,&tmpFile);
   if (status != 0)
   {
         fprintf(stderr, _DTGETMESSAGE(PTSET,3,
                             "%s Error: problem processing help volume '%s'\n"),
                             options->programName, options->helpVolume);

         return 1;                      /* RETURN error */
   }

   /* Alloc max shell command line len */
   printCommand = malloc(MAX_COMMAND_LENGTH*sizeof(char));
   if (printCommand == NULL)
   {
         fprintf(stderr, _DTGETMESSAGE(PTSET,4,
                             "%s Error: memory allocation failed\n"),
                             options->programName);

         return 1;                      /* RETURN error */
   }
   

   /** generate the file **/
   sprintf(printCommand,"cat %s",tmpFile);

   status = _DtHPrGenFileOrPrint(options,"Help Topic(s)",printCommand);

   unlink(tmpFile);

   return(status);
} /*$END$*/



#if 0  /* This is left-over code that used to deal with printing
          raster images.  We don't need it for printing text-only.
	  I left it here for future reference. */
#if DOC
===================================================================
$FUNBEG$:  _DtHPrPrintHelpTopicWithXvp()
$1LINER$:  Recovers and formats help topic text and then sends to printer
$DESCRIPT$:
Recovers and formats help topic text and then sends to printer
$STATUS$:  
Code left over from VUE 3.0 help print
Has not been updated.
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int _DtHPrPrintHelpTopicWithXvp(
   Display *    dpy,
   VolumeHandle volHandle,
   char *       locationId)
{       /*$CODE$*/
  extern _DtHelpDisplayAreaStruct *Spoof();
  GC             gc;
  GC             pageNumberGC;
  int            screen;
  Window         main_window;
  Display       *dpy;
  XFontStruct   *defaultFont;
  XFontStruct   *pageNumberFont;
  _DtHelpDisplayAreaStruct *pDAS;
  char *lpDocName;

#ifdef RASTER_PRINT
   char         *displayNameX;
   Display      *dpyX;
   Window       main_windowX;
   GC           gcX;
   GC           invert_gcX;
   int          widthX;
   int          heightX;
   XFontStruct  *defaultFontX;
#endif

#define MY_WIDTH        400
#define MY_HEIGHT       400
#define DEFAULT_FONT "-agfa-courier-normal-r-normal-93950-0-120-300-300-m-0-hp-roman8"

#ifdef RASTER_PRINT
   /* set color to bitonal */
   value.addr = RN_bitonal;
   value.size = strlen(RN_BITONAL) + 1;
   XrmPutResource(&XvpDB, STAR_RN_helpColorUse, *str_type, &value);
#endif

#ifdef RASTER_PRINT
   /********************/
   /* Get display name */
   /********************/

   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_display);
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_display);
   if (XrmGetResource(appDB, resource_name,
                      resource_class,
                      str_type, &value) == True)
      displayNameX = value.addr;
   else displayNameX = EMPTY_STR;
#endif

   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_jobTitle);
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_jobTitle);
   if (XrmGetResource(appDB, resource_name, 
		      resource_class, 
		      str_type, &value) == True)
      lpDocName = (char *)value.addr;
   else
      lpDocName = DFLT_JOB_TITLE_STR;
   
   status = XvpEscape(dpy, STARTDOC, strlen(lpDocName), lpDocName, NULL);
   screen = DefaultScreen(dpy);
   
  /*	Calculate application page size		*/

   _DtHPrCalculatePageSize(dpy, &x, &y, &width, &height);

   /* create a window; x, y, width, and height are recorded */
   main_window = XCreateSimpleWindow(dpy, XRootWindow(dpy, screen),
		x, y,
		width,
		height,
		10, 0, XWhitePixel (dpy, screen));
  		/* 0, 0, MY_WIDTH, MY_HEIGHT, 10, 0, XWhitePixel (dpy, screen)); */

  XSelectInput (dpy, main_window, ExposureMask);
  XMapWindow (dpy, main_window);
  XvpEscape(dpy, SET_IMAGE_RESOLUTION, 4, 100, NULL);

  defaultFont = XLoadQueryFont (dpy, DEFAULT_FONT);
  if (defaultFont == NULL)
    {
	printf ("Unable to get a font\n");
	exit (1);
    }

  gc = DefaultGC(dpy, screen);
  XSetForeground (dpy, gc, XBlackPixel (dpy, screen));
  XSetBackground (dpy, gc, XWhitePixel (dpy, screen));
  XSetFont(dpy, gc, defaultFont->fid);
  XSetLineAttributes(dpy, gc, 2, LineSolid, CapButt, JoinMiter);
 
#ifndef RASTER_PRINT
  pageNumberFont = 
     XLoadQueryFont (dpy, 
	     "-agfa-univers-normal-r-normal-94021-0-100-300-300-p-0-iso8859-1");
  if (pageNumberFont == NULL)
    {
	printf ("Unable to get a font\n");
	exit (1);
    }

  pageNumberGC = XCreateGC(dpy, RootWindow(dpy, screen), NULL, NULL);
  XSetForeground (dpy, pageNumberGC, XBlackPixel (dpy, screen));
  XSetBackground (dpy, pageNumberGC, XWhitePixel (dpy, screen));
  XSetFont(dpy, pageNumberGC, pageNumberFont->fid);
  XSetLineAttributes(dpy, pageNumberGC, 2, LineSolid, CapButt, JoinMiter);
#endif

#ifdef RASTER_PRINT
  widthX = width/3; /* hard coded for 300 dpi printers */
  heightX = height/3;
  status = CreateRealXObjects(displayNameX, widthX, heightX, 
			      &dpyX, &main_windowX, &gcX,
			      &invert_gcX, &defaultFontX);
  dpyX->db = dpy->db;	/* carry over data base */
  
  pDAS = Spoof (dpyX, gcX, invert_gcX, defaultFontX, widthX, heightX, lineLeading);
#else
  pDAS = Spoof (dpy, gc, gc, defaultFont, width, height, lineLeading);
#endif
  if (pDAS == NULL)
  {
     fprintf(stderr, "%s: Internal error.\n");
     exit(1);
  }

  if (helpType == DtHELP_TYPE_TOPIC)
#ifdef RASTER_PRINT
     status = printTopicAndChildrenX(helpVolume, locationId, allTopics, pDAS, 
				    dpy, main_window, gc, 
				    dpyX, main_windowX, gcX,
				    widthX, heightX);
#else
     status = printTopicAndChildren(helpVolume, locationId, allTopics, pDAS, 
				    dpy, main_window, gc, pageNumberGC );
#endif
     /*
     pDAS = (_DtHelpDisplayAreaStruct *) SetUp (pDAS, dpy, main_window, gc, defaultFont,
     width,
     height,
     helpVolume,
     locationId);
     */
     else if (helpType == DtHELP_TYPE_DYNAMIC_STRING)
     {
#ifdef RASTER_PRINT
     XImage	*image;
     
     pDAS = (_DtHelpDisplayAreaStruct *) SetUpDynamicString (pDAS, dpy, gc, 
							 defaultFont,
							 width,
							 height,
							 stringData);
     RenderLinesX (dpyX, main_windowX, pDAS, widthX, heightX, &image);
     XvpPutImage(dpy, main_window, gcX, image, 0, 0, 0, 0, 
		 (image->width)*3, (image->height*3) );
     status = XvpEscape(dpy, NEWFRAME, NULL, NULL, NULL);
     if (status > 0) ;
  else if (status < 0)
     fprintf (stdout, "XvpEscape(NEWFRAME) not implemented.\n");
  else fprintf(stdout, "XvpEscape(NEWFRAME) not successful.\n");
#else
	pDAS = (_DtHelpDisplayAreaStruct *) SetUpDynamicString (pDAS, dpy, gc, 
							    defaultFont,
							    width,
							    height,
							    stringData);
	RenderLines (dpy, main_window, pDAS);
#endif
     }
     else 
     {
	fprintf(stderr, "%s Error: Illegal helpType %d.\n",
		argv[0], helpType);
	exit(1);
     }
  
  if (status == -1)
  {
     fprintf (stdout, "Error occurred in XvpEscape(STARTDOC)\n");
     exit(1);
  }

  status = XvpEscape(dpy, ENDDOC, NULL, NULL, NULL);
  if (status == -1)
  {
     fprintf (stdout, "Error occurred in XvpEscape(ENDDOC)\n");
     exit(1);
  }
  
  exit(0);
} /*$END$*/
#endif  /* #if 0 at function start */
