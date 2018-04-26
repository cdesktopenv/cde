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
$FILEBEG$:   HelpPrintI.h
$COMPONENT$: dthelpprint
$PROJECT$:   Cde1
$SYSTEM$:    HPUX 9.0; AIX 3.2; SunOS 5.3
$REVISION$:  $TOG: HelpPrintI.h /main/6 1998/04/06 13:16:19 mgreess $
$CHGLOG$:
$COPYRIGHT$:
   (c) Copyright 1993, 1994 Hewlett-Packard Company
   (c) Copyright 1993, 1994 International Business Machines Corp.
   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
==$END$==============================================================*/
#endif /*DOC*/

#ifndef HELPPRINTI_H
#define HELPPRINTI_H

#define TMPFILE_PREFIX          "phlp"     /* max len of 4 chars */
#define TMPFILE_SUFFIX          ".txt"     /* max len of 4 chars */
 
#define MAX_COMMAND_LENGTH 5120		 /* max system can handle */

/* Message catalog support */
#ifndef NO_MESSAGE_CATALOG
# define _DTGETMESSAGE(set, number, str) _DtHPrGetMessage(set, number, str)
#else
# define _DTGETMESSAGE(set, number, str) str
#endif

/* These are for Xt emulation */
#if 0
typedef char * XtPointer;
typedef char * String;
typedef int    Cardinal;
typedef char   Boolean;

# define XtOffset(p_type,field) ((unsigned int)&(((p_type)NULL)->field))
# define XtNumber(arr)     ((Cardinal) (sizeof(arr) / sizeof(arr[0])))
#else
# include <X11/Intrinsic.h>
#endif

#define XtRefOffset(struct_base,field_offset) \
                    ((XtPointer *)(((unsigned long)struct_base) + (field_offset)))


typedef struct __DtHPrHeadFoot
{
   String  evenHeader;		/* even-page header when printing volume */
   String  oddHeader;		/* odd-page header when printing volume */
   String  evenFooter;		/* even-page footer when printing volume */
   String  oddFooter;		/* odd-page footer when printing volume */
} _DtHPrHeadFoot;

/* This is the list of all options that controls
the operation of dthelprint. */
typedef struct __DtHPrOptions {
   String  display;
   String  printer;
   String  copies;
   String  outputFile;
   String  paperSize;
   String  rsrcname;
   String  rsrcclass;
   String  helpType;
   String  helpVolume;
   String  locationId;
   String  allTopics;
   String  subTopics;
   String  oneTopic;
   String  toc;			/* table of contents of a help volume */
   String  index;		/* index of a help volume */
   String  frontMatter;		/* frontMatter of a help volume */
   _DtHPrHeadFoot tocHF;	/* header / footer of table of contents */
   _DtHPrHeadFoot bodyHF;	/* header / footer of body */
   _DtHPrHeadFoot indexHF;	/* header / footer of index */
   String  manPage;
   String  stringData;
   String  helpFile;
   String  topicTitle;
   String  echoCommand;
   String  echoArgs;
   String  foldCommand;
   String  foldArgs;
   String  prCommand;
   String  prArgs;
   String  prOffsetArg;
   String  manCommand;
   String  manArgs;
   String  redirectCmdAndArgs;
   String  lpCommand;
   String  shCommand;
   String  iconvCmdAndArgs;
   String  debugHelpPrint;
   char *  programName;         /* value from argv[0] */
   int     colsWidth;		/* full width of page */
   int     rowsHeight;		/* full height of page */
   int     colsLeftMargin;	/* blank cols to leave on left */
   int     colsRightMargin;	/* blank cols to leave on right */
   int     rowsTopMargin;	/* blank rows to leave on top */
   int     rowsBottomMargin;	/* blank rows to leave on bottom */
   int     colsTextWidth;	/* width - margins */
   int     rowsTextHeight;	/* height - margins */
   int     colsAdjLeftMargin;	/* minus cols that printer cant print on */
   int     colsAdjRightMargin;	/* minus cols that printer cant print on */
   int     rowsAdjTopMargin;	/* minus rows that printer cant print on */
   int     rowsAdjBottomMargin;	/* minus rows that printer cant print on */
} _DtHPrOptions, * _DtHPrOptionsPtr;


/* application name and class */
#ifdef RASTER_PRINT
#define HELPPRINT_APPLICATION_NAME	"dthelpprintrst"
#define HELPPRINT_APPLICATION_CLASS	"Dthelpprintrst"
#else
#define HELPPRINT_APPLICATION_NAME	"dthelpprint"
#define HELPPRINT_APPLICATION_CLASS	"Dthelpprint"
#endif

/* These constants are used throughout dthelpprint to specify
   command line args, resources, and arg and resource values
   AC: arg class
   AN: arg name 
   RC: rsrc class 
   RN: rsrc name
   RV: rsrc value 
*/
#define RN_appname	"application.name" 
#define RC_appname	"Application.Name" 
#define RN_appclass	"application.class" 
#define RC_appclass	"Application.Class" 

#define AN_display	"-display" 
#define AC_display	"-Display" 
#define RN_display	".display" 
#define RC_display	".Display" 

#define AN_printer	"-printer" 
#define AC_printer	"-Printer" 
#define RN_printer	".printer"
#define RC_printer	".Printer"

#define AN_copies	"-copies" 
#define AC_copies	"-Copies" 
#define RN_copies	".copies" 
#define RC_copies	".Copies" 

#define AN_outputFile	"-outputFile" 
#define AC_outputFile	"-OutputFile" 
#define RN_outputFile	".outputFile" 
#define RC_outputFile	".OutputFile" 

#define AN_paperSize	"-paperSize" 
#define AC_paperSize	"-PaperSize" 
#define RN_paperSize	".paperSize" 
#define RC_paperSize	".PaperSize" 

#define AN_xrm		"-xrm"  

#define AN_rsrcname	"-name" 
#define AC_rsrcname	"-Name" 
#define RN_rsrcname	".name" 
#define RC_rsrcname	".Name" 

#define AN_rsrcclass	"-class" 
#define AC_rsrcclass	"-Class" 
#define RN_rsrcclass	".class" 
#define RC_rsrcclass	".Class" 

#define AN_helpType	"-helpType" 
#define RN_helpType	".helpType" 
#define RC_helpType	".HelpType" 

#define AN_helpVolume	"-helpVolume" 
#define RN_helpVolume	".helpVolume" 
#define RC_helpVolume	".HelpVolume" 

#define AN_locationId	"-locationId" 
#define RN_locationId	".locationId" 
#define RC_locationId	".LocationId" 

#define AN_toc		"-toc"  
#define RN_toc		".toc" 
#define RC_toc		".Toc" 

#define AN_index	"-index"  
#define RN_index	".index" 
#define RC_index	".Index" 

#define AN_frontMatter	"-frontMatter"  
#define RN_frontMatter	".frontMatter" 
#define RC_frontMatter	".FrontMatter" 

#define AN_allTopics	"-allTopics"  
#define RN_allTopics	".allTopics" 
#define RC_allTopics	".AllTopics" 

#define AN_recurse      "-R"        /* VUE 3.0 helpprint name for -subTopics */
#define RN_recurse      "-recurse"  /* VUE 3.0 helpprint name for .subTopics */
#define RC_recurse      "-Recurse"  /* VUE 3.0 helpprint name for .SubTopics */

#define AN_subTopics	"-subTopics"  
#define RN_subTopics	".subTopics" 
#define RC_subTopics	".SubTopics" 

#define AN_oneTopic	"-oneTopic"  
#define RN_oneTopic	".oneTopic" 
#define RC_oneTopic	".OneTopic" 

#define AN_manPage	"-manPage" 
#define RN_manPage	".manPage" 
#define RC_manPage	".ManPage" 

#define AN_stringData	"-stringData" 
#define RN_stringData	".stringData" 
#define RC_stringData	".StringData" 

#define AN_helpFile	"-helpFile" 
#define RN_helpFile	".helpFile" 
#define RC_helpFile	".HelpFile" 

#define AN_topicTitle	"-topicTitle" 
#define AC_topicTitle	"-TopicTitle" 
#define RN_topicTitle	".topicTitle" 
#define RC_topicTitle	".TopicTitle" 

/* page header and footer */
#define AN_evenTocFooter	"-evenTocFooter"  
#define RN_evenTocFooter	".evenTocFooter" 
#define RC_evenTocFooter	".EvenTocFooter" 

#define AN_oddTocFooter		"-oddTocFooter"  
#define RN_oddTocFooter		".oddTocFooter" 
#define RC_oddTocFooter		".OddTocFooter" 

#define AN_evenTocHeader	"-evenTocHeader"  
#define RN_evenTocHeader	".evenTocHeader" 
#define RC_evenTocHeader	".EvenTocHeader" 

#define AN_oddTocHeader		"-oddTocHeader"  
#define RN_oddTocHeader		".oddTocHeader" 
#define RC_oddTocHeader		".OddTocHeader" 

#define AN_evenBodyFooter	"-evenBodyFooter"  
#define RN_evenBodyFooter	".evenBodyFooter" 
#define RC_evenBodyFooter	".EvenBodyFooter" 

#define AN_oddBodyFooter	"-oddBodyFooter"  
#define RN_oddBodyFooter	".oddBodyFooter" 
#define RC_oddBodyFooter	".OddBodyFooter" 

#define AN_evenBodyHeader	"-evenBodyHeader"  
#define RN_evenBodyHeader	".evenBodyHeader" 
#define RC_evenBodyHeader	".EvenBodyHeader" 

#define AN_oddBodyHeader	"-oddBodyHeader"  
#define RN_oddBodyHeader	".oddBodyHeader" 
#define RC_oddBodyHeader	".OddBodyHeader" 

#define AN_evenIndexFooter	"-evenIndexFooter"  
#define RN_evenIndexFooter	".evenIndexFooter" 
#define RC_evenIndexFooter	".EvenIndexFooter" 

#define AN_oddIndexFooter	"-oddIndexFooter"  
#define RN_oddIndexFooter	".oddIndexFooter" 
#define RC_oddIndexFooter	".OddIndexFooter" 

#define AN_evenIndexHeader	"-evenIndexHeader"  
#define RN_evenIndexHeader	".evenIndexHeader" 
#define RC_evenIndexHeader	".EvenIndexHeader" 

#define AN_oddIndexHeader	"-oddIndexHeader"  
#define RN_oddIndexHeader	".oddIndexHeader" 
#define RC_oddIndexHeader	".OddIndexHeader" 

/* star resource matches all comers */ 
#define STAR_RN_topicTitle	"*topicTitle" 
#define STAR_RC_topicTitle	"*TopicTitle" 

/* print activity control */
#define RN_echoCommand		".echoCommand"
#define RC_echoCommand		".EchoCommand"

#define RN_echoArgs		".echoArgs"
#define RC_echoArgs		".EchoArgs"

#define RN_foldCommand		".foldCommand"
#define RC_foldCommand		".FoldCommand"

#define RN_foldArgs		".foldArgs"
#define RC_foldArgs		".FoldArgs"

#define RN_prCommand		".prCommand"
#define RC_prCommand		".PrCommand"

#define RN_prArgs		".prArgs"
#define RC_prArgs		".PrArgs"

#define RN_prOffsetArg		".prOffsetArg"
#define RC_prOffsetArg		".PrOffsetArg"

#define RN_manCommand		".manCommand"
#define RC_manCommand		".ManCommand"

#define RN_manArgs		".manArgs"
#define RC_manArgs		".ManArgs"

#define RN_redirectCmdAndArgs	".redirectCmdAndArgs"
#define RC_redirectCmdAndArgs	".RedirectCmdAndArgs"

#define RN_lpCommand		".lpCommand"
#define RC_lpCommand		".LpCommand"

#define RN_shCommand		".shCommand"
#define RC_shCommand		".ShCommand"

#define RN_iconvCmdAndArgs	".iconvCmdAndArgs"
#define RC_iconvCmdAndArgs	".IconvCmdAndArgs"

/* resources */
#define RN_colsWidth		".colsWidth" 
#define RC_colsWidth		".ColsWidth" 
		
#define RN_rowsHeight		".rowsHeight" 
#define RC_rowsHeight		".RowsHeight" 

#define RN_colsLeftMargin	".colsLeftMargin" 
#define RC_colsLeftMargin	".ColsLeftMargin" 

#define RN_colsRightMargin	".colsRightMargin" 
#define RC_colsRightMargin	".ColsRightMargin" 

#define RN_rowsTopMargin	".rowsTopMargin" 
#define RC_rowsTopMargin	".RowsTopMargin" 

#define RN_rowsBottomMargin	".rowsBottomMargin" 
#define RC_rowsBottomMargin	".RowsBottomMargin" 

/* turn on to debug */
#define RN_debugHelpPrint	".debugHelpPrint" 
#define RC_debugHelpPrint	".DebugHelpPrint" 

/* to determine if debug is on */
#define STAR_RN_debugHelpPrint	"*debugHelpPrint" 
#define STAR_RC_debugHelpPrint	"*DebugHelpPrint" 

/* not currently used */
#define RN_leading 		".leading"
#define RC_leading 		".Leading"

#define STAR_RN_helpColorUse    "*helpColorUse"


/* resource values */
#define RV_letter		"help_papersize_letter" 
#define RV_a4			"help_papersize_a4" 
#define RV_b5			"help_papersize_b5" 
#define RV_legal		"help_papersize_legal" 
#define RV_executive		"help_papersize_executive" 

#define RV_bitonal 		"bitonal"


/*========= Prototypes ===========*/

int _DtHPrPrintStringData(
   Display *dpy,
   _DtHPrOptions * options);
int _DtHPrPrintDynamicStringData(
   Display *dpy,
   _DtHPrOptions * options);
int _DtHPrPrintManPage(
   Display *dpy,
   _DtHPrOptions * options);
int _DtHPrPrintHelpFile(
   Display *dpy,
  _DtHPrOptions * options);
int _DtHPrPrintHelpTopic(
   Display *      dpy,
  _DtHPrOptions * options);
void _DtHPrBuildResourceDb(
   int *         argc,
   char * *      argv,
   XrmDatabase * appDB,
   Display * *   pDpy);
void _DtHPrGetResources(
   XrmDatabase     db,
   _DtHPrOptions * options);
char * _DtHPrCreateTmpFile(
        char * prefix,
        char * suffix);
int _DtHPrGenFileOrPrint(
   _DtHPrOptions * options,
   char *          userfile,
   char *          printCommand);
void _DtHPrGetPrOffsetArg(
   _DtHPrOptions * options,
   char *          argStr);
char * _DtHPrGetMessage(
        int set,
        int n,
        char *s);

#endif /* DTHELPRPINTP_H */

