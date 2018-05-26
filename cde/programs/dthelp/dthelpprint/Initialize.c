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
#ifdef DOC
/*===================================================================
$FILEBEG$:   Initialize.c
$COMPONENT$: dthelpprint
$PROJECT$:   Cde1
$SYSTEM$:    HPUX 9.0; AIX 3.2; SunOS 5.3
$REVISION$:  $XConsortium: Initialize.c /main/5 1996/10/30 11:35:21 drk $
$COPYRIGHT$:
   (c) Copyright 1993, 1994 Hewlett-Packard Company
   (c) Copyright 1993, 1994 International Business Machines Corp.
   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
==$END$==============================================================*/
#endif /*DOC*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/param.h> /* MAXPATHLEN */
#include <sys/stat.h>  /* mkdir */
#include <sys/types.h>


#include <X11/Xlibint.h>  /* _XGetHostname() */
#include <X11/Xos.h>     /* FIX: remove? */
#include <X11/Intrinsic.h>   /* for Xt macros */
/* don't use X11/StringDefs.h because it brings in the _XtStrings global */

#include "HelpPrintI.h"

/*#define DEBUG	*/

/*=========== file/dir constants ===========*/
#define DTAPPDEFAULTS_DIR	CDE_INSTALLATION_TOP "/app-defaults/C/"
#define XAPPDEFAULTS_DIR	"/usr/lib/X11/app-defaults/"
#define XDEFAULTS_FILE		"/.Xdefaults"
#define XDEFAULTS_HOST_FILE	"/.Xdefaults-"
#define TMP_DIR_STR		"/.dt/tmp"
#define DTHELPPRINTSH_FILE	"/usr/dt/bin/dthelpprint.sh"

/*=========== helper constants ===========*/
#define EOS           '\0'
#define DOT_STR       "."
#define DIR_SLASH     '/'
#define EMPTY_STR     s_EmptyStr
#define DIR_SLASH_STR s_DirSlashStr

#define RSRCSPECLEN   100

#define INSET    5       /* message catalog set */

/*=========== helper variables ===========*/
static char s_EmptyStr[1] = { EOS };
static char s_DirSlashStr[] = "/";

/*======== helper macro ==========*/
#define _STR(s)   #s
#define STR(s)    _STR(s)

/*======== default values ==============*/
#define DFLT_DISPLAY 		""
#define DFLT_PRINTER 		0
#define DFLT_COPY_CNT 		"1"
#define DFLT_OUTPUT_FILE 	""
#define DFLT_PAPER_SIZE 	RV_letter
#define DFLT_RSRCNAME 		"dthelpprint"
#define DFLT_RSRCCLASS 		"Dthelpprint"
#define DFLT_HELPTYPE           STR(DtHELP_TYPE_TOPIC)
#define DFLT_LOCATIONID 	"_hometopic"
#define DFLT_ALLTOPICS 		0
#define DFLT_SUBTOPICS 		0
#define DFLT_ONETOPIC 		0
#define DFLT_TOC 		0
#define DFLT_INDEX 		0
#define DFLT_FRONTMATTER 	0
/* The default Header / Footer values are designed for 
   English labels and a 70 column content. */
#define DFLT_TOC_HEADER 	"$LMARGIN$VOLUME\n\n"
#define DFLT_TOC_FOOTER 	"\n$LMARGIN                                                      $SECTNUM\n"
#define DFLT_BODY_HEADER 	"$LMARGIN$VOLUME$VOLUMEFILL             $PAGENUMFILLPage $PAGENUM\n\n"
#define DFLT_BODY_FOOTER 	"\n$LMARGIN$TOPIC$TOPICFILL     $SECTNUMFILLSection $SECTNUM\n"
#define DFLT_INDEX_HEADER 	"$LMARGIN$VOLUME$VOLUMEFILL             $PAGENUMFILLPage $PAGENUM\n\n"
#define DFLT_INDEX_FOOTER 	"\n$LMARGIN                                                                  $SECTNUM\n"
#define DFLT_TOPICTITLE 	"Help"
#define DFLT_SH_CMD		DTHELPPRINTSH_FILE
#define DFLT_ICONV_CMD_AND_ARGS	"iconv -f %s -t %s %s > %s"
#define DFLT_LP_CMD             0
#define DFLT_PR_CMD             "pr"
/* pr arg values order:  STRING job title, INT page height, STRING filename */
#define DFLT_PR_ARGS            "-h \"%s\" -f -l%d %s"  
/* pr offset arg value order:  INT col offset */
#define DFLT_PR_OFFSET_ARG      "-o%d"
#define DFLT_ECHO_CMD           "echo"
/* echo arg values order:  STRING string */
#define DFLT_ECHO_ARGS          "\"%s\""
#define DFLT_FOLD_CMD           "fold"
/* fold arg values order:  INT column width, STRING filename */
#define DFLT_FOLD_ARGS          "-w %d %s"
#define DFLT_MAN_CMD            "man"
/* man arg values order:  STRING man page */
#define DFLT_MAN_ARGS           "%s"
/* redirect arg value order: STRING filename */
#define DFLT_REDIR_CMD_AND_ARGS " > %s"
#define DFLT_DEBUG_HELPPRINT 	0

/*======== resource type constants ==============*/
/*** N.b.: These are currently not supported. ***/
#define XtRBool		"Bool"
#define XtRBoolean	"Boolean"
#define XtRDimension	"Dimension"
#define XtRDisplay	"Display"
#define XtRFile		"File"
#define XtRFloat	"Float"
#define XtRFont		"Font"
#define XtRImmediate	"Immediate"
#define XtRInt		"Int"
#define XtROrientation	"Orientation"
#define XtRScreen	"Screen"
#define XtRShort	"Short"
#define XtRString	"String"

#if 0
/*======== helper structures ==============*/
typedef struct _XtResource {
   String       resource_name;
   String       resource_class;
   String       resource_type;
   Cardinal     resource_size;
   Cardinal     resource_offset;
   String       default_type;
   XtPointer    default_addr;
} XtResource, * XtResourceList;

#endif

/*======== variables ==============*/

/* if resource_name or resource_class are NULL, use function arg values */
static XtResource s_OptionResources[] = {
  { RN_display,
    RC_display,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, display),
    XtRImmediate,
    (XtPointer) DFLT_DISPLAY },
   
  { RN_printer,
    RC_printer,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, printer),
    XtRImmediate,
    (XtPointer) DFLT_PRINTER },
   
  { RN_copies,
    RC_copies,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, copies),
    XtRImmediate,
    (XtPointer) DFLT_COPY_CNT },
   
  { RN_outputFile,
    RC_outputFile,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, outputFile),
    XtRImmediate,
    (XtPointer) DFLT_OUTPUT_FILE },
   
  { RN_printer "." RN_paperSize,   /* see CalculatePageSize() to grok this */
    RC_printer "." RC_paperSize,   /* see CalculatePageSize() to grok this */
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, paperSize),
    XtRImmediate,
    (XtPointer) DFLT_PAPER_SIZE },
   
  { RN_rsrcname,
    RC_rsrcname,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, rsrcname),
    XtRImmediate,
    (XtPointer) DFLT_RSRCNAME },
   
  { RN_rsrcclass,
    RC_rsrcclass,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, rsrcclass),
    XtRImmediate,
    (XtPointer) DFLT_RSRCCLASS },
   
  { RN_helpType,
    RC_helpType,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, helpType),
    XtRImmediate,
    (XtPointer) DFLT_HELPTYPE },
   
  { RN_helpVolume,
    RC_helpVolume,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, helpVolume),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_locationId,
    RC_locationId,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, locationId),
    XtRImmediate,
    (XtPointer) DFLT_LOCATIONID },
   
  { RN_allTopics,
    RC_allTopics,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, allTopics),
    XtRImmediate,
    (XtPointer) DFLT_ALLTOPICS },
   
  { RN_subTopics,
    RC_subTopics,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, subTopics),
    XtRImmediate,
    (XtPointer) DFLT_SUBTOPICS },
   
  /* recurse shadows subTopics for legacy reasons.  
     recurse was the VUE 3.0 helpprint option for subTopics */
  { RN_recurse,
    RC_recurse,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, subTopics),
    XtRImmediate,
    (XtPointer) DFLT_SUBTOPICS },
   
  { RN_oneTopic,
    RC_oneTopic,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, oneTopic),
    XtRImmediate,
    (XtPointer) DFLT_ONETOPIC },
   
  { RN_toc,
    RC_toc,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, toc),
    XtRImmediate,
    (XtPointer) DFLT_TOC },
   
  { RN_index,
    RC_index,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, index),
    XtRImmediate,
    (XtPointer) DFLT_INDEX },
   
  { RN_frontMatter,
    RC_frontMatter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, frontMatter),
    XtRImmediate,
    (XtPointer) DFLT_FRONTMATTER },
   
  { RN_evenTocFooter,
    RC_evenTocFooter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, tocHF.evenFooter),
    XtRImmediate,
    (XtPointer) DFLT_TOC_FOOTER },
   
  { RN_oddTocFooter,
    RC_oddTocFooter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, tocHF.oddFooter),
    XtRImmediate,
    (XtPointer) DFLT_TOC_FOOTER },
   
  { RN_evenTocHeader,
    RC_evenTocHeader,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, tocHF.evenHeader),
    XtRImmediate,
    (XtPointer) DFLT_TOC_HEADER },
   
  { RN_oddTocHeader,
    RC_oddTocHeader,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, tocHF.oddHeader),
    XtRImmediate,
    (XtPointer) DFLT_TOC_HEADER },
   
  { RN_evenBodyFooter,
    RC_evenBodyFooter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, bodyHF.evenFooter),
    XtRImmediate,
    (XtPointer) DFLT_BODY_FOOTER },
   
  { RN_oddBodyFooter,
    RC_oddBodyFooter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, bodyHF.oddFooter),
    XtRImmediate,
    (XtPointer) DFLT_BODY_FOOTER },
   
  { RN_evenBodyHeader,
    RC_evenBodyHeader,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, bodyHF.evenHeader),
    XtRImmediate,
    (XtPointer) DFLT_BODY_HEADER },
   
  { RN_oddBodyHeader,
    RC_oddBodyHeader,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, bodyHF.oddHeader),
    XtRImmediate,
    (XtPointer) DFLT_BODY_HEADER },
   
  { RN_evenIndexFooter,
    RC_evenIndexFooter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, indexHF.evenFooter),
    XtRImmediate,
    (XtPointer) DFLT_INDEX_FOOTER },
   
  { RN_oddIndexFooter,
    RC_oddIndexFooter,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, indexHF.oddFooter),
    XtRImmediate,
    (XtPointer) DFLT_INDEX_FOOTER },
   
  { RN_evenIndexHeader,
    RC_evenIndexHeader,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, indexHF.evenHeader),
    XtRImmediate,
    (XtPointer) DFLT_INDEX_HEADER },
   
  { RN_oddIndexHeader,
    RC_oddIndexHeader,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, indexHF.oddHeader),
    XtRImmediate,
    (XtPointer) DFLT_INDEX_HEADER },
   
  { RN_manPage,
    RC_manPage,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, manPage),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_stringData,
    RC_stringData,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, stringData),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_helpFile,
    RC_helpFile,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, helpFile),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_topicTitle,
    RC_topicTitle,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, topicTitle),
    XtRImmediate,
    (XtPointer) DFLT_TOPICTITLE },

  { RN_echoCommand,
    RC_echoCommand,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, echoCommand),
    XtRImmediate,
    (XtPointer) DFLT_ECHO_CMD },
   
  { RN_echoArgs,
    RC_echoArgs,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, echoArgs),
    XtRImmediate,
    (XtPointer) DFLT_ECHO_ARGS },
   
  { RN_foldCommand,
    RC_foldCommand,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, foldCommand),
    XtRImmediate,
    (XtPointer) DFLT_FOLD_CMD },
   
  { RN_foldArgs,
    RC_foldArgs,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, foldArgs),
    XtRImmediate,
    (XtPointer) DFLT_FOLD_ARGS },
   
  { RN_prCommand,
    RC_prCommand,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, prCommand),
    XtRImmediate,
    (XtPointer) DFLT_PR_CMD },
   
  { RN_prArgs,
    RC_prArgs,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, prArgs),
    XtRImmediate,
    (XtPointer) DFLT_PR_ARGS },
   
  { RN_prOffsetArg,
    RC_prOffsetArg,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, prOffsetArg),
    XtRImmediate,
    (XtPointer) DFLT_PR_OFFSET_ARG },
   
  { RN_manCommand,
    RC_manCommand,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, manCommand),
    XtRImmediate,
    (XtPointer) DFLT_MAN_CMD },
   
  { RN_manArgs,
    RC_manArgs,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, manArgs),
    XtRImmediate,
    (XtPointer) DFLT_MAN_ARGS },
   
  { RN_redirectCmdAndArgs,
    RC_redirectCmdAndArgs,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, redirectCmdAndArgs),
    XtRImmediate,
    (XtPointer) DFLT_REDIR_CMD_AND_ARGS },
   
  { RN_lpCommand,
    RC_lpCommand,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, lpCommand),
    XtRImmediate,
    (XtPointer) DFLT_LP_CMD },
   
  { RN_shCommand,
    RC_shCommand,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, shCommand),
    XtRImmediate,
    (XtPointer) DFLT_SH_CMD },
   
  { RN_iconvCmdAndArgs,
    RC_iconvCmdAndArgs,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, iconvCmdAndArgs),
    XtRImmediate,
    (XtPointer) DFLT_ICONV_CMD_AND_ARGS },
   
  { RN_debugHelpPrint,
    RC_debugHelpPrint,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, debugHelpPrint),
    XtRImmediate,
    (XtPointer) DFLT_DEBUG_HELPPRINT },
   
#if 0   /* Because we aren't using Xt and only emulating it,
           we can't handle data type other than String.  These
           are all Integer values, so I process them by hand
           below in CalculatePageSize(). */
  { RN_colsWidth,
    RC_colsWidth,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, colsWidth),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_rowsHeight,
    RC_rowsHeight,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, rowsHeight),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_colsLeftMargin,
    RC_colsLeftMargin,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, colsLeftMargin),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_colsRightMargin,
    RC_colsRightMargin,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, colsRightMargin),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_rowsTopMargin,
    RC_rowsTopMargin,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, rowsTopMargin),
    XtRImmediate,
    (XtPointer) 0 },
   
  { RN_rowsBottomMargin,
    RC_rowsBottomMargin,
    XtRString,
    sizeof(String),
    XtOffset (_DtHPrOptionsPtr, rowsBottomMargin),
    XtRImmediate,
    (XtPointer) 0 },
#endif
   
}; /* resources */




#if DOC
===================================================================
$PFUNBEG$:  GetHomeDir()
$1LINER$:   Gets home directory of user owning current app
$DESCRIPT$:
Gets the home directory by 
   1. using the HOME environment variable
or 2. using the USER env variable and the 
      directory in the password file for that user
or 3. using getuid() and the 
      directory in the password file for that user

The home directory is stored in the string that is passed
in.  No overflow checking is possible, so the string should
be MAXPATHLEN bytes long.
$RETURNS$:
Returns the 'dest' pointer
$ARGS$:
dest: pointer to string to hold home directory
========================================================$SKIP$=====*/
#endif /*DOC*/

static char *GetHomeDir (
	char *dest)
{       /*$CODE$*/
	uid_t uid;
	struct passwd *pw;
	char *ptr;

	if((ptr = getenv("HOME")) != NULL)
        {
            snprintf(dest, MAXPATHLEN, "%s", ptr);
	}
        else
        {
            if((ptr = getenv("USER")) != NULL)
            {
                char user[MAXPATHLEN];
                snprintf(user, MAXPATHLEN, "%s", ptr);
                pw = getpwnam(user);
            }
            else
            {
                uid = getuid();
                pw = getpwuid(uid);
            }
            if (pw)
                snprintf(dest, MAXPATHLEN, "%s", pw->pw_dir);
            else
                *dest = '\0';
	}
	return dest;
}   /*$END$*/





#if DOC
===================================================================
$PFUNBEG$:  Usage()
$1LINER$:  Prints usage message and returns
$DESCRIPT$:
This routine prints a generic help message
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void Usage(void)
{       /*$CODE$*/
   static char * usage[] = 
   {
      "dthelpprint - Print program for Help\n\n",
      "Usage: dthelpprint [options]\n",
      "Options controlling how to print:\n",
      "\t" AN_printer " printername  printer to use\n",
      "\t" AN_copies " number        number of copies to print\n",
      "\t" AN_outputFile " filename  write output to this file\n",
      "\t" AN_paperSize " size       format content to this paper size\n",
      "\t\tsize = {" RV_letter "|" RV_legal "|\n" ,
      "\t\t        " RV_executive "|" RV_a4 "|" RV_b5 "}\n",
      "\t" AN_display " displayname  display from which to get resources\n",
      "\t" AN_rsrcname " name            program name used when getting resources\n",
      "\t" AN_rsrcclass " name           class name used when getting resources\n",
      "\t" AN_xrm " resourcestring   additional resources\n",
      "Options controlling what to print:\n",
      "\t" AN_helpType " type        type of Help data\n",
      "\t\ttype = 0 (help volume), 1 (string), 2 (man page), 3 (help file)\n",
      "\t" AN_helpVolume " volume    full path of help volume file\n",
      "\t" AN_locationId " location  name of Help topic in the volume\n",
      "\t" AN_allTopics "            print all topics, toc, & index in the help volume\n",
      "\t" AN_subTopics "            print topic locationId and all subtopics\n",
      "\t" AN_oneTopic "             print topic locationId\n",
      "\t" AN_toc "                  print help volume table of contents\n",
      "\t" AN_index "                print help volume index\n",
      "\t" AN_frontMatter "          print help volume front matter\n",
      "\t" AN_manPage " manpagename  name of man page\n",
      "\t" AN_stringData " string    Help text to print\n",
      "\t" AN_helpFile " filename    file containing Help text\n",
      "\t" AN_topicTitle " title     title string for Help text\n",
      NULL
   };
    int i;
    for (i=0; usage[i]; i++)
       printf("%s", _DTGETMESSAGE(INSET,i,usage[i]));
}       /*$END$*/


#if DOC
===================================================================
$PFUNBEG$:  CalculatePageSize()
$1LINER$:  calculates the page size based on resource values
$DESCRIPT$:
Reads the resources from the database and calculates
the page dimensions and stores them in the resources structure.
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

static
void CalculatePageSize(
   XrmDatabase     appDB,
   _DtHPrOptions * options,
   char *          appname,
   char *          appclass)
{       /*$CODE$*/

   struct unprintableMargins
   {
      int leftUnprintableMargin;
      int rightUnprintableMargin;
      int topUnprintableMargin;
      int bottomUnprintableMargin;
   };

   static struct unprintableMargins blankMargins = { 6,   2,   2,   1 };

#define LETTER		0
#define LEGAL		1
#define EXECUTIVE	2
#define	A4		3
#define	B5		4
#define	MAXVALIDSIZE	4

   /* page size info */
   struct page 
   {
      int width;
      int height;
      int leftMargin;
      int rightMargin;
      int topMargin;
      int bottomMargin;
   };

   static struct page  pagesize[] = 
   {  /* sizes are in characters of standard HPLJIII courier */
      /* Note that the text margin counts do *NOT* take into account the 
         printless margins, which are the margins on which the printer 
         is unable to print. */
      /* WI: paper size width; HI: paper size height;
         LM: desired text left margin; RM: desired text right margin
         TM: desired text top margin; BM: desired text bottom margin
      */
      /*WI, HI, LM, RM, TM, BM */
      { 91, 69, 10, 10, 6,  6 }, /* letter */
      { 91, 88, 10, 10, 6,  6 }, /* legal */
      { 77, 66, 10, 10, 6,  6 }, /* executive */
      { 88, 73, 10, 10, 6,  6 }, /* A4 */
      { 76, 63, 10, 10, 6,  6 }  /* B5 */
   };
   
#if 0   /* save these dimensions here */
      /* Known Dimensions for standard HPLJIII courier: 
            char size: 2.37 mm width x 4.04 mm height
      US-letter:  8.5x11 in       : 91 col x 69 rows
      US-legal:   8.5x14 in       : 91 col x 88 rows
      executive:  7.25x10.5 in    : 77 col x 66 rows
      A4:         210x297 mm      : 88 col x 73 rows
      B5:         182x257 mm      : 76 col x 63 rows
      */

      /* Known Dimensions in 100ths of a mm */
      { 21590, 27940, 2540, 2540, 2540, 2540 },  /* letter */
      { 21590, 35560, 2540, 2540, 2540, 2540 },  /* legal */
      { 18415, 26670, 2540, 2540, 2540, 2540 },  /* exec */
      { 21000, 29700, 2540, 2540, 2540, 2540 },  /* a4 */
      { 18200, 25700, 2540, 2540, 2540, 2540 }   /* b5 */
static float dphm = 300.*2540.;
#endif

   int  (*stricmp)(const char *,const char *);
   char *name;
   char name_prefix[RSRCSPECLEN];
   char name_prefix_temp[sizeof(name_prefix)];
   char class_prefix[RSRCSPECLEN];
   char class_prefix_temp[sizeof(class_prefix)];
   char resource_name[RSRCSPECLEN];
   char resource_class[RSRCSPECLEN];
   char *str_type[20];
   XrmValue value;
   int papersize = -1;
   char buf[20];
   int width,height;
   int lmargin,rmargin,tmargin,bmargin;
   int adjLmargin,adjRmargin,adjTmargin,adjBmargin;
   int textWidth, textHeight;
   int i;
   
   /* build printer resource name and class */
   snprintf(name_prefix, sizeof(name_prefix), "%s%s", appname, RN_printer);    /* e.g. dthelpprint.printer */
   snprintf(class_prefix, sizeof(class_prefix), "%s%s", appclass, RC_printer); /* e.g. Dthelpprint.Printer */
   
   /********************/
   /* Get printer name */
   /********************/
   
   snprintf(resource_name, sizeof(resource_name), "%s%s", name_prefix, RN_rsrcname);    /* e.g. dthelpprint.printer.name */
   snprintf(resource_class, sizeof(resource_class), "%s%s", class_prefix, RC_rsrcname); /* e.g. Dthelpprint.Printer.Name */
   if (XrmGetResource(appDB, resource_name, resource_class,
                      str_type, &value) == True)
      name = value.addr;
   else name = EMPTY_STR;
   if (name[0] != EOS)
   {
      snprintf(name_prefix_temp, sizeof(name_prefix_temp), "%s%s%s", name_prefix, DOT_STR, name);    /* e.g. dthelpprint.printer.<name> */
      strcpy(name_prefix, name_prefix_temp);
      snprintf(class_prefix_temp, sizeof(class_prefix_temp), "%s%s%s", class_prefix, DOT_STR, name); /* e.g. Dthelpprint.Printer.<name> */
      strcpy(class_prefix, class_prefix_temp);
   }
   
   /**************************/
   /*	Get explicit width   */
   /**************************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_colsWidth); 
                        /* e.g. dthelpprint.printer{.<name>}.colsWidth */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_colsWidth);
                        /* e.g. Dthelpprint.Printer{.<name>}.ColsWidth */
   
   if (XrmGetResource(appDB, resource_name, resource_class,
                      str_type, &value) == True)
   {
      width = atoi (value.addr);
      if (width < 0) width = 0;
   }
   else width = 0;
   
   /**************************/
   /*	Get explicit height  */
   /**************************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_rowsHeight); 
                         /* e.g. dthelpprint.printer{.<name>}.rowsHeight */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_rowsHeight);
                         /* e.g. Dthelpprint.Printer{.<name>}.RowsHeight */
   
   if (XrmGetResource(appDB, resource_name, resource_class, 
		      str_type, &value) == True)
   {
      height = atoi (value.addr);
      if (height < 0) height = 0;
   }
   else height = 0;
   
   /**********************************************************/
   /* get the paper size (will provide default height/width) */
   /**********************************************************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_paperSize);  
                         /* e.g. dthelpprint.printer{.<name>}.paperSize */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_paperSize); 
                         /* e.g. Dthelpprint.Printer{.<name>}.PaperSize */
   
#if defined(_AIX)
   stricmp = strcmp;	/* AIX and USL don't have strcasecmp */
#else
   stricmp = strcasecmp;
#endif
   if (XrmGetResource(appDB, resource_name, resource_class, 
                      str_type, &value) == True)
   {
      if ((*stricmp)(value.addr, RV_letter) == 0) papersize = LETTER;
      else if ((*stricmp)(value.addr, RV_b5) == 0) papersize = B5;
      else if ((*stricmp)(value.addr, RV_a4) == 0) papersize = A4;
      else if ((*stricmp)(value.addr, RV_legal) == 0) papersize = LEGAL;
      else if ((*stricmp)(value.addr, RV_executive) == 0) papersize = EXECUTIVE;
      else 
      {
	 fprintf(stderr, _DTGETMESSAGE(INSET,40,
                           "%s Warning: Illegal paper size '%s'.  " 
                            RV_letter " used.\n"),
                       options->programName, value.addr);
	 papersize = LETTER;
      }
   }  /* if the paper size resource is defined */

   /* if specified only a width or height, but not both */
   if ( papersize < 0 && (width == 0 || height == 0) )
   {
      fprintf(stderr, _DTGETMESSAGE(INSET,41,
                    "%s Warning: Missing paper size, height, or width value.  "
                     RV_letter " used.\n"), 
                    options->programName);
      papersize = LETTER;
   }

   /* if an invaild papersize, dflt to letter */
   if ( papersize < 0 || papersize > MAXVALIDSIZE ) papersize = LETTER;
   
   /* use page size width and height if not specified */
   if (width == 0) width = pagesize[papersize].width;
   if (height == 0) height = pagesize[papersize].height;
   
   /* Adjust margins */
   
   /****************/
   /* Left margins */
   /****************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_colsLeftMargin);     
                         /* e.g. dthelpprint.printer{.<name>}.colsLeftMargin */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_colsLeftMargin);    
                         /* e.g. Dthelpprint.Printer{.<name>}.ColsLeftMargin */
   
   /* get the value */
   if (XrmGetResource(appDB, resource_name, resource_class, 
		      str_type, &value) == True)
      i = atoi (value.addr); /* custom margin */
   else
      i = pagesize[papersize].leftMargin; /* papersize margin */
   lmargin = 0;
   if (i < width) lmargin = i;

   /* available text width, part 1 */
   textWidth = width - lmargin;

   /* all printed text subject to this offset */
   adjLmargin = lmargin - blankMargins.leftUnprintableMargin;
   if (adjLmargin < 0) adjLmargin = 0;
   
   /*****************/
   /* Right margins */
   /*****************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_colsRightMargin);     
                         /* e.g. dthelpprint.printer{.<name>}.colsRightMargin */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_colsRightMargin);    
                         /* e.g. Dthelpprint.Printer{.<name>}.ColsRightMargin */
   
   /* get the value */
   if (XrmGetResource(appDB, resource_name, resource_class, 
		      str_type, &value) == True)
      i = atoi (value.addr); /* custom margin */
   else
      i = pagesize[papersize].rightMargin; /* size margin */
   rmargin = 0;
   if (i < textWidth) rmargin = i;

   /* available text width, part 2 */
   textWidth -= rmargin;

   /* all printed text subject to this offset */
   adjRmargin = rmargin - blankMargins.rightUnprintableMargin;
   if (adjRmargin < 0) adjRmargin = 0;
   
   /***************/
   /* Top Margin */
   /*****************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_rowsTopMargin);     
                         /* e.g. dthelpprint.printer{.<name>}.rowsTopMargin */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_rowsTopMargin);    
                         /* e.g. Dthelpprint.Printer{.<name>}.RowsTopMargin */
   
   /* get the value */
   if (XrmGetResource(appDB, resource_name, resource_class, 
		      str_type, &value) == True)
      i = atoi (value.addr); /* custom margin */
   else
      i = pagesize[papersize].topMargin; /* papersize margin */
   tmargin = 0;
   if (i < height) tmargin = i;

   /* available text height, part 1 */
   textHeight = height - tmargin;
   
   /* all printed text subject to this offset */
   adjTmargin = tmargin - blankMargins.topUnprintableMargin;
   if (adjTmargin < 0) adjTmargin = 0;
   
   /*****************/
   /* Bottom Margin */
   /*****************/
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_rowsBottomMargin);     
                       /* e.g. dthelpprint.printer{.<name>}.rowsBottommargin */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_rowsBottomMargin);    
                       /* e.g. Dthelpprint.Printer{.<name>}.RowsBottommargin */
   
   /* get the value */
   if (XrmGetResource(appDB, resource_name, resource_class,
                      str_type, &value) == True)
      i = atoi (value.addr); /* custom margin */
   else
      i = pagesize[papersize].bottomMargin; 	/* size margin */
   bmargin = 0;
   if ( i < textHeight ) bmargin = i;

   /* available text height, part 2 */
   textHeight -= bmargin;
   
   /* all printed text subject to this offset */
   adjBmargin = tmargin - blankMargins.bottomUnprintableMargin;
   if (adjBmargin < 0) adjBmargin = 0;
   
   /*** set the values ***/
   options->rowsHeight = height;
   options->colsWidth = width;
   options->colsLeftMargin = lmargin;
   options->colsRightMargin = rmargin;
   options->rowsTopMargin = tmargin;
   options->rowsBottomMargin = bmargin;
   options->rowsTextHeight = textHeight;
   options->colsTextWidth = textWidth;
   options->colsAdjLeftMargin = adjLmargin;
   options->colsAdjRightMargin = adjRmargin;
   options->rowsAdjTopMargin = adjTmargin;
   options->rowsAdjBottomMargin = adjBmargin;

   if (options->debugHelpPrint)
   {
      printf("options.%s: %d\n","rowsHeight",options->rowsHeight);
      printf("options.%s: %d\n","colsWidth",options->colsWidth);
      printf("options.%s: %d\n","colsLeftMargin",options->colsLeftMargin);
      printf("options.%s: %d\n","colsRightMargin",options->colsRightMargin);
      printf("options.%s: %d\n","rowsTopMargin",options->rowsTopMargin);
      printf("options.%s: %d\n","rowsBottomMargin",options->rowsBottomMargin);
      printf("options.%s: %d\n","rowsTextHeight",options->rowsTextHeight);
      printf("options.%s: %d\n","colsTextWidth",options->colsTextWidth);
      printf("options.%s: %d\n","colsAdjLeftMargin",options->colsAdjLeftMargin);
      printf("options.%s: %d\n","colsAdjRightMargin",options->colsAdjRightMargin);
      printf("options.%s: %d\n","rowsAdjTopMargin",options->rowsAdjTopMargin);
      printf("options.%s: %d\n","rowsAdjBottomMargin",options->rowsAdjBottomMargin);
   }
}       /*$END$*/
	

#if DOC
===================================================================
$FUNBEG$:  _DtHPrBuildResourceDb()
$1LINER$:  Parses command line and constructs a single db of resources
$DESCRIPT$:
This routine parses the command line and reads various data bases
to create a single data base that copntains all resources associated
with this application.

It generates the resulting resource database by merging the
following databases:
      help-print specific cmdline & environment var->resources database
          $PRINTER -> RN_printer, RC_printer
          $DISPLAY -> RN_display, RC_display
          AN_rsrcname -> RN_appname
          AC_rsrcname -> RC_appname
          AN_rsrcclass -> RN_appclass
          AC_rsrcclass -> RC_appclass
      app-specific class resource file on local host
          /usr/lib/X11/app-defaults/<appclass>
      app-specific user resource file on local host
          $HOME/<appclass>
      display server resources or user preferences file on local host
          XResourceManagerString() or $HOME/.Xdefaults
      screen resources from display server
          XScreenResourcesString()
      user environment resource file on local host
          $XENVIRONMENT or $HOME/.Xdefaults-<host>
      application command line

This function is a replacement for XtAppInitialize/XtInitialize()
and XtDisplayInitialize() to remove dependency on Xt.
$RETURNS$:
argc is modified to be the number of remaining args
argv is modified to point to the remaining args
appDB is filled with the option settings used by the rest of the program
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

void _DtHPrBuildResourceDb(
   int *         argc,
   char * *      argv,
   XrmDatabase * appDB,
   Display * *   pDpy)
{       /*$CODE$*/
   static XrmOptionDescRec optionsTable[] = {
/* how to print */
      {AN_display,	RN_display,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_printer,	RN_printer,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_copies,	RN_copies,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_outputFile,	RN_outputFile,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_paperSize,	RN_printer "." RN_paperSize,
					XrmoptionSepArg, (caddr_t) NULL},
      {AN_xrm,		NULL,		XrmoptionResArg, (caddr_t) NULL},
      {AN_rsrcname,	RN_rsrcname,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_rsrcclass,	RN_rsrcclass,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_evenTocFooter,  RN_evenTocFooter,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_oddTocFooter,   RN_oddTocFooter,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_evenTocHeader,  RN_evenTocHeader,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_oddTocHeader,   RN_oddTocHeader,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_evenBodyFooter, RN_evenBodyFooter,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_oddBodyFooter,  RN_oddBodyFooter,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_evenBodyHeader, RN_evenBodyHeader,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_oddBodyHeader,  RN_oddBodyHeader,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_evenIndexFooter,RN_evenIndexFooter,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_oddIndexFooter, RN_oddIndexFooter,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_evenIndexHeader,RN_evenIndexHeader,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_oddIndexHeader, RN_oddIndexHeader,	XrmoptionSepArg, (caddr_t) NULL},
/* what to print */
      {AN_helpType,	RN_helpType,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_helpVolume,	RN_helpVolume,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_locationId,	RN_locationId,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_allTopics,	RN_allTopics,	XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_subTopics,	RN_subTopics,	XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_recurse,	RN_recurse,	XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_oneTopic,	RN_oneTopic,	XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_toc,		RN_toc,		XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_index,	RN_index,	XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_frontMatter,	RN_frontMatter,	XrmoptionNoArg,	 (caddr_t) "True"},
      {AN_manPage,	RN_manPage,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_stringData,	RN_stringData,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_helpFile,	RN_helpFile,	XrmoptionSepArg, (caddr_t) NULL},
      {AN_topicTitle,	RN_topicTitle,	XrmoptionSepArg, (caddr_t) NULL},
      };
   static int numOptions = XtNumber(optionsTable);  /*number entries in table*/
	 
   XrmDatabase appClassDB = NULL;
   XrmDatabase appUserDB = NULL;
   XrmDatabase dispDB = NULL;
   XrmDatabase homeDB = NULL;
   XrmDatabase scrnDB = NULL;
   XrmDatabase envDB = NULL;
   XrmDatabase commandLineDB = NULL;
   char        filename[MAXPATHLEN+1];
   XrmValue    value;
   char *      str_type[20];
   char *      invocationName=NULL;
   char        name_prefix[RSRCSPECLEN];
   char        class_prefix[RSRCSPECLEN];
   char        resource_name[RSRCSPECLEN];
   char        resource_class[RSRCSPECLEN];
   char *      commandLineName = NULL;
   char *      environment = NULL;
   char *      display = NULL;
   char *      dispDBStr = NULL;
   char *      scrnDBStr = NULL;
   int         i;
   char *      tmp_string;
   
   /** n.b.: If *appDB == NULL, the first XrmPutResource() call
       creates a database and stored its ref in appDB.  We don't
       need to explicitly create the db.  **/

   str_type[0] = NULL;

   if (*argc < 2) 
   {
      Usage();	/* print message if no arguments */
      exit(1);
   }
   
   /** Parse the commandline and put options into a db **/
   /* Do this now so that we can recover options from 
      the db for immediate use. */

   /* search arguments for AN_rsrcname option */
   for (i=1; i<(*argc-1); i++)
   {
      if ( strcmp(argv[i], AN_rsrcname)==0 )  
      {
	 invocationName=argv[i+1];
	 break;
      }
   }
   if (invocationName == NULL)
   {
      /* search for last '/' of string */
      if ( _DtHelpCeStrrchr(argv[0],DIR_SLASH_STR,MB_CUR_MAX,&commandLineName) == 0 )
         invocationName = commandLineName+1;
      else    /* Starts at character after the last slash */
         invocationName = argv[0]; 
   }
   
   XrmParseCommand(&commandLineDB, optionsTable, numOptions, invocationName, 
		   argc, argv);
   
   if (*argc != 1) 
   {
      Usage();	/* print message if any arguments left */
      exit(1);
   }
   
   /** now create a baseline appDB with a few minimal resources in it **/

   /* Set name and class initial values */
   strcpy(name_prefix, invocationName);                  /* e.g. dthelpprint */
   strcpy(class_prefix, HELPPRINT_APPLICATION_CLASS);    /* e.g. Dthelpprint */

   /* get DB values */
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_rsrcclass);   /* e.g. dthelpprint.class */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_rsrcclass);  /* e.g. Dthelpprint.Class */
   if (XrmGetResource(commandLineDB, resource_name, resource_class, 
		      str_type, &value) == True)
   {
      strcpy(class_prefix, value.addr);
   }
   else 
   {
      value.size = strlen(HELPPRINT_APPLICATION_CLASS)+1;
      value.addr = HELPPRINT_APPLICATION_CLASS;
   }
   /* CHK: Note that *str_type may be NULL here.  Is this ok? */
   XrmPutResource(appDB, RN_appclass, NULL, &value);
   XrmPutResource(appDB, RC_appclass, NULL, &value);
   
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_rsrcname);    /* e.g. dthelpprint.name */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_rsrcname);   /* e.g. Dthelpprint.Name */
   if (invocationName != NULL)      /* always true */
   {
      value.size = strlen(invocationName)+1;
      value.addr = invocationName;           /* e.g. dthelpprint */
      /* CHK: state of *str_type unknown here.  Is this ok? */
      XrmPutResource(appDB, RN_appname, NULL, &value);
      XrmPutResource(appDB, RC_appname, NULL, &value);
   }
   /* CHK: currently, this and the next 'elses' are never executed */
   else if( XrmGetResource((XrmDatabase)commandLineDB, resource_name, 
		      resource_class, str_type, &value) == True)
   {
      strcpy(name_prefix, value.addr);
   }
   else 
   {
      value.size = strlen(HELPPRINT_APPLICATION_NAME)+1;
      value.addr = HELPPRINT_APPLICATION_NAME;           /* i.e. dthelpprint */
      XrmPutResource(appDB, RN_appname, NULL, &value);
      XrmPutResource(appDB, RC_appname, NULL, &value);
      strcpy(name_prefix, value.addr);
   }
   
   
   /**	Get environment variables and put values in appDB as resource **/
   
   if ((tmp_string = getenv("DISPLAY")) != NULL) 
   {
      i = strlen(tmp_string);
      value.addr=malloc(i+1);    /* FIX: use tmp_string here directly? */
      value.size = i+1;
      strcpy(value.addr, tmp_string);
      strcpy(resource_name, name_prefix);
      strcat(resource_name, RN_display);   /* e.g. dthelpprint.display */
      strcpy(resource_class, class_prefix);
      strcat(resource_class, RC_display);  /* e.g. Dthelpprint.Display */
      XrmPutResource(appDB, resource_name, NULL, &value);
      XrmPutResource(appDB, resource_class, NULL, &value);
   }

   if ((tmp_string = getenv("PRINTER")) != NULL) 
   {
      i = strlen(tmp_string);
      value.addr=malloc(i+1);     /* FIX: use tmp_string here directly? */
      value.size = i+1;
      strcpy(value.addr, tmp_string);
      strcpy(resource_name, name_prefix);
      strcat(resource_name, RN_printer);    /* e.g. dthelpprint.printer */
      strcpy(resource_class, class_prefix);
      strcat(resource_class, RC_printer);    /* e.g. Dthelpprint.Printer */
      XrmPutResource(appDB, resource_name, NULL, &value);
      XrmPutResource(appDB, resource_class, NULL, &value);
   }
   
   /*** Build the display resource db ***/

   /* Sequence and logic of db overlays taken from the X11R5 description
      of XtDisplayInitialize()  (p. 172, Xt Ref Man) and enhanced to use 
      appDB baseline values.  Note that it is my opinion that the
      description in the manual DOES NOT match the implementation
      of XtScreenDatabase(), which is the actual routine used
      by XtDisplayInitialize().  The following is the order
      of database merges:
          help-print specific environment var->resources database
          DT app-specific class resource file on local host
          X app-specific class resource file on local host
          app-specific user resource file on local host
          display server resources or user
              preferences file on local host
          screen resources from display server
          user environment resource file on local host
          application command line

      N.B. This code does not pay attention to XFILESEARCHPATH and
      does not consider languages other than C in /usr/dt/app-defaults/%L.
   */

   /* Overlay the central Dt app-defaults */
   (void) strcpy(filename, DTAPPDEFAULTS_DIR);
   strcat(filename, class_prefix);/*eg. /usr/dt/app-defaults/C/Dthelpprint*/
   appClassDB = XrmGetFileDatabase(filename);
   if (appClassDB != NULL)
      (void) XrmMergeDatabases(appClassDB, appDB);
   
   /* Overlay the central X app-defaults (if exists) */
   (void) strcpy(filename, XAPPDEFAULTS_DIR);
   strcat(filename, class_prefix);/*eg. /usr/lib/X11/app-defaults/Dthelpprint*/
   appClassDB = XrmGetFileDatabase(filename);
   if (appClassDB != NULL)
      (void) XrmMergeDatabases(appClassDB, appDB);
   
   /* Overlay the user class resources  */
   GetHomeDir(filename);
   strcat(filename, DIR_SLASH_STR);
   strcat(filename, class_prefix);          /* e.g. $HOME/Dthelpprint */
   appUserDB = XrmGetFileDatabase(filename);
   if (appUserDB != NULL)
      (void) XrmMergeDatabases(appUserDB, appDB);

   /** to get the display's resources, we need to open the display first **/
   /* to get the display name, we first look in the commandLineDB,
      and if not found there, in the appDB loaded thus far. */
   strcpy(resource_name, name_prefix);
   strcat(resource_name, RN_display);   /* e.g. dthelpprint.display */
   strcpy(resource_class, class_prefix);
   strcat(resource_class, RC_display);  /* e.g. Dthelpprint.Display */
   if (   XrmGetResource((XrmDatabase)commandLineDB, resource_name, 
		      resource_class, str_type, &value) == True
       || XrmGetResource((XrmDatabase)*appDB, resource_name, 
		      resource_class, str_type, &value) == True)
      display = value.addr;
   else
      display = NULL;
   /* if display == NULL, uses DISPLAY env var */
   *pDpy = XOpenDisplay(display);  /* FIX: should this be .printer ?? */
   if (*pDpy == NULL)
   {  /* Bad display */
                                   /* FIX: chg msg to match above semantics */
      fprintf(stderr, _DTGETMESSAGE(INSET,42,
                  "%s Warning: Unable to open display %s\n"), 
                  argv[0], (NULL == display ? "" : display) );
   } /* if bad display */
   else
   {  /* Good display */
      /* try to overlay the display's resources (i.e. the xrdb-managed ones) */
      dispDBStr = XResourceManagerString(*pDpy);
      if (dispDBStr) dispDB = XrmGetStringDatabase(dispDBStr);
      else  /* if no display resources, overlay $HOME/.Xdefaults file */
      {
         filename[0] = EOS;
         (void) GetHomeDir (filename);
         (void) strcat (filename, XDEFAULTS_FILE);  /* e.g. $HOME/.Xdefaults */
         dispDB = XrmGetFileDatabase (filename);
      }
      if (dispDB != NULL)
         (void) XrmMergeDatabases(dispDB, appDB);
      
      scrnDBStr = XScreenResourceString(DefaultScreenOfDisplay(*pDpy));
      if (scrnDBStr) scrnDB = XrmGetStringDatabase(scrnDBStr);
      if (scrnDB != NULL)
         (void) XrmMergeDatabases(scrnDB, appDB);
   } /* else good display */

   /* Overlay either file from XENVIRONMENT or $HOME/.Xdefaults-<host> */
   if ((environment = getenv ("XENVIRONMENT")) == NULL) 
   {	/* no XENVIRONMENT variable */
      int len;
      filename[0] = EOS;
      (void) GetHomeDir (filename);
      (void) strcat (filename, XDEFAULTS_HOST_FILE);
      len = strlen (filename);
      (void) _XGetHostname (filename+len, 1024-len);
      environment = filename;               /* e.g. $HOME/.Xdefaults-<host> */
   }
   envDB = XrmGetFileDatabase (environment);
   if (envDB != NULL)
      (void) XrmMergeDatabases(envDB, appDB);
 
   /* Overlay command line */
   if (*appDB == NULL) *appDB = commandLineDB;
   else XrmMergeDatabases (commandLineDB, appDB);
   
}       /*$END$*/


 

#if DOC
===================================================================
$FUNBEG$:  _DtHPrGetResources()
$1LINER$:  Retrieves options from a Xrm db and assigns value to a struct
$DESCRIPT$:
Retrieves options from a Xrm db and assigns value to a struct

This is a poor-man emulation to the mechanism Xt provides for
setting values of widgets. 

$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

void _DtHPrGetResources(
   XrmDatabase     db,
   _DtHPrOptions * options)
{       /*$CODE$*/
   XtResource * rsrc;
   XrmValue value;
   char *  str_type[20];
   char    resource_name[256];
   char    resource_class[256];
   char *  name_prefix;
   char *  class_prefix;
   int     debugHelpPrint;
   int     cnt;

   /* Get name and class */
   if (XrmGetResource(db, RN_appname, RC_appname,
                      str_type, &value) == True)
      name_prefix = value.addr;
   else name_prefix = HELPPRINT_APPLICATION_NAME;     /*e.g. dthelpprint*/

   if (XrmGetResource(db, RN_appclass, RC_appclass,
                      str_type, &value) == True)
      class_prefix = value.addr;
   else class_prefix = HELPPRINT_APPLICATION_CLASS;   /*e.g. Dthelpprint*/

   if (XrmGetResource(db, STAR_RN_debugHelpPrint, STAR_RC_debugHelpPrint,
                      str_type, &value) == True)
      debugHelpPrint = 1;
   else debugHelpPrint = 0;

   /* walk thru all resources and get their values */
   for ( rsrc = s_OptionResources, cnt = XtNumber(s_OptionResources); 
         cnt > 0; 
         cnt--, rsrc++ )
   {
      snprintf(resource_name, sizeof(resource_name), "%s%s", name_prefix, rsrc->resource_name);
      snprintf(resource_class, sizeof(resource_class), "%s%s", class_prefix, rsrc->resource_class);
      if (XrmGetResource(db, resource_name,
                      resource_class, str_type, &value) == True)
         *XtRefOffset(options,rsrc->resource_offset) = value.addr;
      else 
         *XtRefOffset(options,rsrc->resource_offset) = rsrc->default_addr;

      if (debugHelpPrint)
      {
         if (*XtRefOffset(options,rsrc->resource_offset))
             printf("options%s: %s\n",  (char *) rsrc->resource_name, 
             		 (char *) *XtRefOffset(options,rsrc->resource_offset));
         else
             printf("options%s: <NULL>\n", (char *) rsrc->resource_name);
      }
   }
   /* and calculate the page sizes */
   CalculatePageSize(db, options, name_prefix, class_prefix);

} /*$END$*/



#if DOC
===================================================================
$FUNBEG$:  _DtHPrCreateTmpFile()
$1LINER$:  Creates a tmp file in $HOME/.dt/tmp
$DESCRIPT$:
The file is put in $HOME/.dt/tmp so that the
file can be referenced from other systems.  This
assumes that the user home directory is mounted
on those systems.
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

char * _DtHPrCreateTmpFile(
        char * prefix,
        char * suffix)
{       /*$CODE$*/
   static int filecnt = 0;
   char   dirname[MAXPATHLEN+1];
   char * tmppath;
   char * newtmpfile;
   int    len;
   char * tmp;

   if (NULL == prefix) prefix = EMPTY_STR;
   if (NULL == suffix) suffix = EMPTY_STR;

   GetHomeDir(dirname);
   tmppath = dirname + strlen(dirname) + 1;
   strcat(dirname, TMP_DIR_STR);

   /* create the directory */
   for ( tmppath = strchr(tmppath,DIR_SLASH);
         tmppath != NULL;
         tmppath = strchr(++tmppath,DIR_SLASH) )
   {
      *tmppath = EOS;
      mkdir(dirname,0777);
      *tmppath = DIR_SLASH;
   }
   mkdir(dirname,0777);

#define FILENAMELEN 25
   /* generate the new tmp file */
   newtmpfile=malloc((strlen(dirname) + FILENAMELEN + 2) * sizeof(char));
   if (NULL == newtmpfile)
   {
      fprintf(stderr, "%s", _DTGETMESSAGE(INSET,45,
                     "Error: Unable to allocate memory for temporary file\n"));
   }
   else
   {
      sprintf(newtmpfile, _DTGETMESSAGE(INSET,50,"%1$s/%2$s%3$d_%4$d%5$s"), 
		dirname, prefix, getpid(), filecnt++, suffix );
   }

   return newtmpfile;
}       /*$END$*/

