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
/* $TOG: global.h /main/4 1998/04/06 13:17:04 mgreess $ */
/* Copyright (c) 1988, 1989 Hewlett-Packard Co. */
/* Global definitions for HP Tag/TeX translator */
/*   When changing this file, check "grphpar.h" for conflicts */

#if defined(DEFINE)
#define EXTERN
#define INIT(a) = a
#else
#define EXTERN extern
#define INIT(a)
#endif

#include <stdlib.h>
#include <string.h>
#if defined(MSDOS)
#include <io.h>
#include <process.h>
#include <direct.h>
#endif
#include <ctype.h>

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__) || defined(linux) || defined(CSRG_BASED)
#include <unistd.h>
#include <locale.h>
#endif

#define MAXHEADLINES 3
#define FNAMELEN 513
#define BIGBUF 1024


/* substitute for m_ctupper; we don't cast x into (int), thereby
   avoiding any problems of high order bit.  This is in response
   to "Entity no allowed in ART" messages by cupertino.  However,
   this more of a loose deduction, and really needs confirmation.  */
#define ctupper(x) (m_ctarray[x].upper)


/* Must be in same order as values in optkey, adjust NUMOPTIONS if changed*/
#define DRAFT         1
#define FINAL         2
#define MEMO          3
#define NOMEMO        4
#define SEARCHKEY     5
#define CLEARSEARCH   6
#define LONGFILES     7
#define LONGFILE      8
#define LONG          9
#define SHORTFILES   10
#define SHORTFILE    11
#define SHORT        12
#define CHARSET      13
#define ONERROR      14

#if defined(FUTURE_FEATURE)
#define GRAPHICS     15
#define NOGRAPHICS   16
#define REV          17
#define NOREV        18
#endif /* FUTURE_FEATURE */

#define NUMOPTIONS 14

EXTERN int optval INIT(M_NULLVAL) ;

EXTERN char *optkey[NUMOPTIONS]
#if defined(DEFINE)
  = {
      "draft",
      "final",
      "memo",
      "nomemo",
      "search",
      "clearsearch",
      "longfiles",
      "longfile",
      "long",
      "shortfiles",
      "shortfile",
      "short",
      "charset",
      "onerror",

#if defined(FUTURE_FEATURE)
      "graphics",
      "nographics",
      "rev",
      "norev",
#endif /* FUTURE_FEATURE */
      }
#endif
  ;

/* Processing options */
EXTERN LOGICAL final INIT(FALSE) ;
EXTERN LOGICAL memo INIT(FALSE) ;
EXTERN int rebuild INIT(FALSE) ;

EXTERN LOGICAL stoponerror INIT(TRUE) ;

#if defined(FUTURE_FEATURE)
EXTERN LOGICAL prntrevs INIT(FALSE) ;
#endif /* FUTURE_FEATURE */

/* variables for index sorting and tagging in the scripts */
       /* we create a child directory for processing the index, so as not
          to clobber any files with the same base name.  However, this will
	  affect search paths that are relative; those that are relative
          need to have parent level prepended.  This is done only when
          "parentsrch" is set to TRUE; done only when we process indexes.  */
EXTERN LOGICAL parentsrch INIT(FALSE) ;
EXTERN char *idxfile INIT(NULL) ; /* index file name only */
EXTERN char *idxpath INIT(NULL) ; /* index file name w/path */
EXTERN char *idxbase INIT(NULL) ; /* index base name only */

/* TeX counter for chapter number */
#define FRONTCOUNT 0
#define APPCOUNT 1000
#define TOCCOUNT 2000
#define MSGCOUNT 3000
#define BIBCOUNT 4000
#define GLOSSCOUNT 5000
#define INDEXCOUNT 6000

/* flag for TeX output while in HelpTag parser */
EXTERN LOGICAL texoutput INIT(FALSE);

/* Flag for filelist vs. Tag to TeX translator */
EXTERN LOGICAL filelist INIT(FALSE) ;

/* NOTE: If driver is redimensioned, a change is needed in texinit() */
EXTERN char driver[20]
#if defined(MSDOS)
  INIT("CDS") 
#else
#if defined(hpux)
  INIT("HPUX")
#endif
#if defined(_AIX)
  INIT("AIX")
#endif
#if defined(linux)
  INIT("LINUX")
#endif
#if defined(sun)
  INIT("SUN")
#endif
#if defined(USL)
  INIT("USL")
#endif
#if defined(__uxp__)
  INIT("UXP")
#endif
#endif
  ;

EXTERN M_WCHAR *helpcharset INIT(NULL);

/* Working directory, input directory, installation directory,
   and search paths */
EXTERN LOGICAL defaultext ;
EXTERN M_WCHAR *inputname;    /* pointer to name of main input file */
EXTERN char *base ;
EXTERN char *baseext ;
EXTERN char *nodirbase ;
EXTERN char *work INIT("") ;
EXTERN char *indir INIT(NULL) ;
EXTERN char *install INIT(NULL) ;
typedef struct search SEARCH ;
struct search {
  char *directory ;
  SEARCH *next ;
  } ;
EXTERN SEARCH *path INIT(NULL) ;
EXTERN SEARCH **endpath INIT(&path) ;
EXTERN char dirsep
#if defined(MSDOS)
INIT('\\')
#else
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__)
INIT('/')
#else
****define directory separator here****
#endif
#endif
  ;

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__)
#define CSEP '/'
#define SSEP "/"
#else
#if defined(MSDOS)
#define CSEP '\\'
#define SSEP "\\"
#else
****define directory separator here****
#endif
#endif

/* Location in .TEX file of two \wlog messages */
EXTERN long wlog ;

/* True if have encountered idx or idxsyn */
EXTERN LOGICAL have_index INIT(FALSE) ; 
EXTERN long idxupdate INIT(0L) ; /* file/seek pointer for make facility */

/* True within a sequence of rsect's */
EXTERN LOGICAL rsectseq INIT(FALSE) ;

/* True if SAMEPAGE for this rsect sequence */
EXTERN LOGICAL rsectsame ;

/* True if have encountered <NEWPAGE> in a SAMEPAGE <RSECT>*/
EXTERN LOGICAL rsectholdnew ;

/* True in an rsect that has an abbreviated head */
EXTERN LOGICAL didabbrev ;

/* True if this manual has a glossary */
EXTERN LOGICAL glossary INIT(FALSE) ;

/* True while processing a note if a head was encountered */
EXTERN LOGICAL notehead ;

/* String emitted at start of note, caution or warning */
EXTERN char ncwStart[] INIT("<PARAGRAPH %s after 0 left 5 first 5 right 0>\n\
<TYPE serif><WEIGHT bold>");

/* String emitted at end of note, caution or warning */
EXTERN char ncwEnd[] INIT("%s</WEIGHT></TYPE></PARAGRAPH>\n\
<PARAGRAPH before 0 after 0 left 5 right 0>\n");

/* String emitted if a graphic is present for note, caution or warning */
EXTERN char ncwGraphic[] INIT("<LABEL><GRAPHIC file %s></LABEL>\n");

/* String emitted to position note, caution or warning in a tight list */
EXTERN char *ncwTightList INIT("");

/* File address for "Generated TeX file complete" message */
EXTERN long texcomplete ;

/* File pointer for cross-reference file */
EXTERN FILE *xrf ;

/* global link type pointer, used in xref to match latest link type */
EXTERN int global_linktype INIT (0);

/* pointers to image parameters for use in imagetext */
EXTERN M_WCHAR *indentp;
EXTERN M_WCHAR *imagegentityp;
EXTERN M_WCHAR *imagegpositionp;
EXTERN M_WCHAR *imageghyperlinkp;
EXTERN M_WCHAR *imageglinktypep;
EXTERN M_WCHAR *imagegdescription;

/* indent for p, image, ex, vex, based on plain list */
#define LISTFIRSTINDENT 2
#define LISTITEMINDENT  2
#define LEFTINDENT (LISTFIRSTINDENT + LISTITEMINDENT)

#define ORDEREDLISTFIRSTINDENT 1
#define ORDEREDLISTITEMINDENT  3
#define ORDEREDLEFTINDENT (ORDEREDLISTFIRSTINDENT + ORDEREDLISTITEMINDENT)

/* File offset after preamble, and before \bye.  Used to determine 
   whether content found in the document */
EXTERN long postpreamble INIT(0L) ;
EXTERN long prebye INIT(0L) ;
EXTERN LOGICAL filefound INIT(FALSE) ;

/* Chapter or appendix number */
EXTERN int part INIT(0) ;
EXTERN int chapter INIT(0) ;
EXTERN int chapinc INIT(0) ;
EXTERN char chapstring[15] INIT("0") ;

#define NODEID_LENGTH 204
/* name of current node */
EXTERN M_WCHAR nodeid[NODEID_LENGTH+1];  
/* Node number for autogenerated node id */
EXTERN int autonodenumber INIT(1);
EXTERN char nodestring[15] INIT("0");
/* Node level variables */
EXTERN int lastnodelevel INIT(0);
EXTERN int thisnodelevel INIT(0);
/* Offset of node from beginning of file */
EXTERN long int nodeoffset INIT(0);
EXTERN M_WCHAR *savehelpfilename INIT(NULL);
/* is this the first node of a .ht file? */
EXTERN LOGICAL firstnode INIT(TRUE);
/* flag for omitting first newline in topic map file */
EXTERN LOGICAL isfirsttopicmap INIT(TRUE);
EXTERN LOGICAL nohometopic INIT(TRUE);
EXTERN long	topicoffset INIT(0L);
EXTERN int	htfileno INIT(1);
/* The first tag in the front section should trigger the node */
EXTERN LOGICAL have_a_frontnode INIT(FALSE);
EXTERN char *helpbase INIT(NULL);
EXTERN char *helpext;
/* small buffer for scratch work */
EXTERN char helptmpbuf[2] INIT("0");

/* Should we use these BASENAME_LIMIT on the size of file names? */
LOGICAL usingshortnames INIT(FALSE);
/* Limit on how long the basename can be for a .ht file */
#define BASENAME_LIMIT 6
/* limit on how many additional character are allowed before the ext */
/* (How many chapter numbers    foo``12''.ht */
#define PRE_EXTENSION_LIMIT 2

EXTERN FILE *indexfp;
EXTERN FILE *docregfp;
EXTERN FILE *idtablefp;
EXTERN FILE *topicmapfp;

/* Whether have processed anything in chapter or appendix */
EXTERN LOGICAL chapst INIT(FALSE) ; 

/* Whether have just started a fourth-level section */
EXTERN LOGICAL st4 INIT(FALSE) ;

/* TRUE for chapter, FALSE for appendix */
EXTERN LOGICAL inchapter INIT(TRUE) ;

/* Figure number within this chapter or appendix */
EXTERN int figno ;

/* Options on a particular figure */
EXTERN int ftype, fborder, ftonumber, fvideo, fstrip, fsnap, fautoscale,
           fmaxlistrows ;

/* Table number within this chapter or appendix */
EXTERN int tableno ;

/* footnote number */
EXTERN int footnoteno INIT(0);

/* True while processing abbrev head for page footer */
EXTERN LOGICAL fabbrev INIT(FALSE) ;
EXTERN LOGICAL abbrev INIT(FALSE) ;

EXTERN FILE *outfile ;
/* re-directing output to nothing, eg. used for unimplemented tags */
EXTERN FILE *nullfile ;
/* save the old outfile when redirecting data to nullfile */
EXTERN FILE *CDROMsaveoutfile ;
/* save the old outfile when redirecting header/footer data to nullfile */
EXTERN FILE *HFsaveoutfile;

/* True if a figure or table has a caption */
EXTERN LOGICAL figcaption ;
EXTERN LOGICAL tabcaption ;

EXTERN LOGICAL had_an_otherfront_head INIT(FALSE);

/* For softkeys */
EXTERN int sklines, skchars ;

/* contains current m_textchar, used to see if we need to issue an italic
   correction.  Issue italic correction everywhere except after commas
   and periods.  */
EXTERN M_WCHAR cur_char INIT(' ') ;

/* Special characters */
#define O_BR  '{'
#define C_BR  '}'
#define ESCAPE 27

/* Error _DtMessage section */
EXTERN LOGICAL emsghead INIT(FALSE) ;
/* emsghead has three possible values:
     DEFHEAD : use default error message title
		 USERHEAD : user specified
		 FALSE : do not process head (assume define is not equal to
			       DEFHEAD or USERHEAD)
*/
#define DEFHEAD  2
#define USERHEAD 3

/* Figure stuff */
#define FIGDEFWH "3in"
#define MAXNORMROWS 55
#define MAXSMALROWS 75
#define MAXTINYROWS 100
#define MAXPENS 8  /* when changing, check prfigpenwidths() ! */
#define MAXPENWIDTH 1864680    /* approx 1cm in scaled points */

EXTERN LOGICAL fignote ;
EXTERN int fignotePtr, in_graphic, nlines, blank_middle ;
EXTERN unsigned long ncolumns, max_columns, res_index, xx, ri ;
EXTERN LOGICAL file_ent ;
EXTERN M_WCHAR *f_content ;
EXTERN M_WCHAR f_contqual[FNAMELEN] ;
EXTERN M_WCHAR *f_file ;

/* callouts -- entfile and lineno */
EXTERN M_WCHAR *figinpath ;
EXTERN int figline INIT(0) ;

/* -- callouts -- */
#define	BYTE       unsigned char
#define	NEWPT      0
#define	NEWGROUP   1
#define	NEWLINE	   2
#define	NEWCALLOUT 3

typedef struct tagFPOINT {
float   x,y;
} FPOINT ;

typedef struct tagPwT {
FPOINT  coord;
char    ew[2],
        ns[2],
        lcr[2];
} PTWTXT ;

#define AN_desc		3
#define AN_nsew		60
#define AN_text		12
#define AN_isbros		16
#define AN_hasbros	32
#define AN_sf1			64
#define AN_sf2			128
#define SH_desc		0
#define SH_nsew		2
#define SH_text		2
#define SH_isbros		4
#define SH_hasbros	5
#define SH_sf1			6
#define SH_sf2			7

#define UNPK(c,an,sh)	((c&an)>>sh)

EXTERN FILE *calfile ;
EXTERN int maxct;
EXTERN LOGICAL okcallout ;
/* -- end callout -- */

EXTERN FILE *pclfile ;

#define VERSION "Version"
EXTERN char *version INIT(NULL) ;

EXTERN char texdone[] INIT("TeX run completed successfully.") ;

/* Code for standard library function "access" */
#define READABLE 04

/* Allowable units on dimensions */
#define SPperPT 65536.0
#define PTperIN 72.27
#define CMperIN 2.54
#define MMperIN 25.4
#define PTperPC 12.0
EXTERN char *dimarray[5] 
#if defined(DEFINE)
 = {  "in",
      "cm",
      "mm",
      "pc",
      "pt" }
#endif
 ;

/* misc */
EXTERN LOGICAL unleaded INIT(FALSE) ;
EXTERN int newhline INIT(0) ;
EXTERN int synelcnt INIT(0);

/* Cross-Reference Declarations */
/* do we have foreward references? */
EXTERN LOGICAL have_forward_xrefs INIT(FALSE);
EXTERN M_WCHAR *xrffile ;    /* Save file and line number for error messages */
EXTERN int xrfline INIT(0) ;  /* --used to store into structure "xref"   */
EXTERN M_TRIE xtree ;
struct xref {
    M_WCHAR *textptr ;       /* Pointer to expansion of xref */
    LOGICAL defined ;     /* Whether the xref has been defined */
    LOGICAL wheredef ;    /* Whether defined in this pass or a previous one */
    LOGICAL retrieved ;   /* Whether the value has been used */
    LOGICAL csensitive ;  /* Whether the value is chapter sensitive */
    LOGICAL xrefable;     /* can this be xref'd to legally? */
    char *chapstring ;    /* Number of chapter or appendix where it occurs */
    LOGICAL inchapter ;   /* Whether in chapter or appendix */
    M_WCHAR *file ;      /* File and line # where first referred to or  */
    int line ;            /*   first defined for error reports   */
    } ;
  /* Value of one particular cross-reference */
EXTERN M_WCHAR xrefstring[400] ;
EXTERN int xstrlen INIT(0) ;
  /* Pointer to the id */
EXTERN M_WCHAR *savid ;
EXTERN LOGICAL  iderr ;
EXTERN char *imageId INIT(NULL); /* "image" id saved for "imagetext" */

/* Save head for page header in rsect and table of contents string*/
EXTERN M_WCHAR savehead[400] ;
EXTERN M_WCHAR savetabcap[400] ;
EXTERN int svheadlen ;
EXTERN int svtclen ;
EXTERN LOGICAL savhd ;
EXTERN LOGICAL savtc ;
EXTERN LOGICAL hderr ;
EXTERN LOGICAL echo ;
EXTERN LOGICAL tooutput INIT(TRUE) ;

/* Glossary Declarations */
EXTERN M_TRIE gtree ;
#define MAXTERM 200
EXTERN M_WCHAR term[MAXTERM + 1] ;
EXTERN M_WCHAR *termp ;
EXTERN LOGICAL wsterm ;
#define ENTERTERM 1
#define DEFINETERM 2

EXTERN int exlineno ;
EXTERN LOGICAL tonumexlines ;

/* Saved document title */
EXTERN M_WCHAR *savedtitle;

/* tell whether .hmi should have generic info put into it */
EXTERN LOGICAL hadtitle INIT(FALSE);
EXTERN LOGICAL hadabstract INIT(FALSE);

/* emit an empty abstract title if no head specified */
EXTERN LOGICAL needabstracthead INIT(FALSE);
     
/* Numeric codes corresponding to keyword parameter values */

#define FIXED 1
#define FLOAT 2

#define SIDE 1
#define STACK 2

#define COMPUTER 1
#define DISPLAY  2

/* Note that the definition of BOX was chosen so that the border
parameter on both figure and table could use it, even though these
parameters do not share all possible values */
#define BOX 1
#define TOP 2
#define BOTTOM 3
#define NOBORDER 4
#define SCREEN 5
#define NOBOX 2

#define ART 1
#define PCL 2
#define TIFF 3
#define LISTING 4
#define TEXT 5
#define HPGL 7

/* IMPORTANT: "FIG..." values should not be          */
/*             changed (eg. see proc "checkmargin")  */
#define FIGTOP    0
#define FIGRIGHT  1
#define FIGBOTTOM 2
#define FIGLEFT   3
#define NFIGSIDES 4

#define WRAP 1
#define NOWRAP 2

/* text size macros */
#define NORMAL 1
#define SMALLER 2
#define SMALLEST 3

#define NUMBER 1
#define NONUMBER 2

#define LEFT 1
#define INDENT 2
#define CENTER 3
#define RIGHT 4

#define TIGHT 1
#define LOOSE 2

#define NORULE 1
#define HEADRULE 2
#define GRID 3

#define ORDER 0
#define BULLET 1
#define PLAIN 2
#define MILSPEC 3
#define LABEL 4
#define CHECK 5

#define UROMAN 0
#define UALPHA 1
#define ARABIC 2
#define LALPHA 3
#define LROMAN 4

#define DOTPUNCT 0
#define PARENPUNCT 1

#define NEWPAGE 0
#define SAMEPAGE 1

#define GLOSS 1
#define NOGLOSS 2

#define INVERSE 1
#define NOINVERSE 2

#define STRIP 1
#define NOSTRIP 2

#define DPENWDVAL 1

#define SNAP 1
#define NOSNAP 2

#define AUTOSCALE 1
#define NOAUTOSCALE 2

/* the values for these elements are significant...they are used to set
 * TeX register values which are later tested by  macros
 */
#define COMMAND 1
#define KEYS2 2

/* the values for these elements are significant...they are used to set
 * TeX register values which are later tested by  macros
 */
#define TWO 2
#define FOUR 4
#define EIGHT 8
#define OTHER 0

/* values for ctable tracking */
EXTERN int ctbltype;
EXTERN int ctbldisplines;
EXTERN int ctblgraphics;

/* Declarations for lists */
#define MAXLISTLEV 8
/* FIRST  => first paragraph in list item.  The one following the label.
   FOLLOW => immediately after a figure, note, caution, warning, ex or vex
   INSIDE => at least one previous paragraph
*/
enum ParagraphListLocation {FIRST, FOLLOW, INSIDE};

EXTERN LOGICAL lablistindented[MAXLISTLEV];
EXTERN int list INIT(0) ;
typedef struct contchain CONTCHAIN ;
struct contchain {
  long where ;
  CONTCHAIN *next ;
  } ;
typedef struct list LIST ;
struct list {
  int type ;
  int order ;
  int count ;
  int space ;
  unsigned char punct ;
  unsigned char started ;
  unsigned char where ; 
  LIST *lastlist ;
  CONTCHAIN *chain ; 
  } ;
EXTERN LIST outlist 
#if defined(DEFINE)
  = {0, 0, 0, 0, DOTPUNCT, FALSE, FIRST, NULL, NULL}
#endif
  ;
EXTERN LIST *lastlist INIT(&outlist) ;
EXTERN int bulcount INIT(0) ;

/* Volume name for index */
EXTERN M_WCHAR volume[M_NAMELEN + 1] ;
/* save area for index processing */
EXTERN M_WCHAR idxsav[400] ;
/* Sort form of an index term */
EXTERN M_WCHAR sort[400] ;
/* Sort form of an index sub term */
EXTERN M_WCHAR subsort[400] ;
/* print form of index term */
EXTERN M_WCHAR print[400] ;
/* print form of index sub term */
EXTERN M_WCHAR subprint[400] ;
EXTERN int idxsavlen ;
EXTERN LOGICAL didsort ;
EXTERN LOGICAL didsubsort ;

/* Strings for constructing error messages */
EXTERN char firstused[] INIT("'%s' first used in line %d") ;
EXTERN char undef[] INIT("Undefined cross-reference %s (") ;
EXTERN char infile[] INIT("file %s, ") ;
EXTERN char online[] INIT("line %d)\n") ;
EXTERN char offile[] INIT(" of file: %s") ;
/* Whitemenu supported only in calculator, issue message only once */
EXTERN LOGICAL wmenu_err INIT(FALSE) ;

/* Repeated code at start of each text block */
#define STARTSTUFF {}

/* Pop the stack of font changes */
#define POPFONT 
/* For index entries */
#define IDXSTRT1 {}
#define IDXSTRT2 {}
#define IDXSTRT IDXSTRT1 IDXSTRT2

#define IDXEND1 {}
#define IDXEND2 {}
#define IDXEND3 {}
#define IDXEND IDXEND1 IDXEND2 IDXEND3

/* Absorb consecutive <CR>'s, because TeX gets confused by them */
EXTERN LOGICAL multi_cr_flag INIT(FALSE);

/* flag for leaders within <LIST PLAIN> only */
EXTERN LOGICAL leaderok INIT(FALSE) ;

/* Direct debugging trace output to stdout or outfile */
EXTERN LOGICAL tracetostd INIT(FALSE) ;

/* show memory available */
EXTERN LOGICAL showmem INIT(FALSE);

/* Function prototypes */
#include "fproto.h"

/* Declarations for handling special characters */
#include "roman8.h"

/* Declarations for handling special characters for calculators*/
EXTERN char ccoutbuf[128] ;

/* Character conversion tables */
#include "charconv.h"
