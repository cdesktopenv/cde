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
/* $TOG: global.h /main/4 1998/04/06 13:18:34 mgreess $ */
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
#include <time.h>
#if defined(MSDOS)
#include <io.h>
#include <process.h>
#include <direct.h>
#endif
#include <ctype.h>

#include <unistd.h>
#include <locale.h>
#include <fcntl.h>

#define MAXHEADLINES 3
#define FNAMELEN 513
#define BIGBUF 1024

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

/* Flag for filelist vs. Tag to SDL translator */
EXTERN LOGICAL filelist INIT(FALSE) ;

EXTERN M_WCHAR *helpcharset INIT(NULL);
EXTERN M_WCHAR *helplang    INIT(NULL);
EXTERN char    *docId INIT("SDL-RESERVED0");
EXTERN char    sdlReservedName[] INIT("SDL-RESERVED");
#define SDLNAMESIZ sizeof("SDL-RESERVED")
EXTERN char    *pubId INIT("CDE 1.0");

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
EXTERN char dirsep INIT('/') ;

#define CSEP '/'
#define SSEP "/"

/* True within a sequence of rsect's */
EXTERN LOGICAL rsectseq INIT(FALSE) ;

/* True if SAMEPAGE for this rsect sequence */
EXTERN LOGICAL rsectsame ;

/* True if this manual has a glossary */
EXTERN LOGICAL glossary INIT(FALSE) ;

/* True while processing a note if a head was encountered */
EXTERN LOGICAL notehead ;

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

LOGICAL inParText INIT(FALSE);
LOGICAL inSdlP    INIT(FALSE);

/* Save these ids and reuse them on the current virpage.
 * We need two of each (except for the first) so we can alternate to
 * avoid <form> thinking it is supposed to span them.  The bullet id
 * indexes will flip back and forth between 0 and 1 to pick an id.
*/
int bulletId[2], looseBulletId[2], firstBulletId, firstLooseBulletId;
int bulletIdIndex INIT(1);
int looseBulletIdIndex INIT(1);

/* save the textsize to emit it on the <p> tag; this allows us to have
 * a single ssi= for the "ex" <block> and modify the text size individually 
 * in the "ex" <p>
*/
EXTERN int exTextSize;

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
/* Node level variables */
EXTERN int thisnodelevel INIT(0);
EXTERN M_WCHAR *savehelpfilename INIT(NULL);
/* flag for omitting first newline in topic map file */
EXTERN LOGICAL nohometopic INIT(TRUE);
EXTERN int     parTextId   INIT(0);
EXTERN char *helpbase INIT(NULL);
EXTERN char *helpext;

/* Should we use these BASENAME_LIMIT on the size of file names? */
LOGICAL usingshortnames INIT(FALSE);
/* Limit on how long the basename can be for a .ht file */
#define BASENAME_LIMIT 6
/* limit on how many additional character are allowed before the ext */
/* (How many chapter numbers    foo``12''.ht */
#define PRE_EXTENSION_LIMIT 2

EXTERN FILE *snbfp;
EXTERN FILE *indexfp;

/* Whether have processed anything in chapter or appendix */
EXTERN LOGICAL chapst INIT(FALSE) ; 

/* TRUE for chapter, FALSE for appendix */
EXTERN LOGICAL inchapter INIT(TRUE) ;

/* Figure number within this chapter or appendix */
EXTERN int figno ;

/* Options on a particular figure */
EXTERN int ftype, fborder, ftonumber, fvideo, fstrip, fsnap, fautoscale,
           fmaxlistrows ;

/* Place to hold <snb> entries until after a <virpage><head>, if any,
 * is seen.  Also, an integer value of the offset into the file to the
 * point immediately after either <virpage> or <virpage><head> - the
 * place to put the <snb>.
*/
EXTERN char *savesnb  INIT(NULL);
EXTERN int   snbstart INIT(0);

/* Table number within this chapter or appendix */
EXTERN int tableno ;

/* footnote number */
EXTERN int footnoteno INIT(0);

/* True while processing abbrev head for page footer */
EXTERN LOGICAL fabbrev INIT(FALSE) ;

EXTERN FILE *outfile ;

/* True if a figure or table has a caption */
EXTERN LOGICAL figcaption ;
EXTERN LOGICAL tabcaption ;
EXTERN char   *capposition; /* left/center/right position of the caption */

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

/* Error _CoeMessage section */
EXTERN LOGICAL emsghead INIT(FALSE) ;
/* emsghead has three possible values:
     DEFHEAD  : use default error message title
     USERHEAD : user specified
     FALSE    : do not process head (assume define is not equal to
			       DEFHEAD or USERHEAD)
*/
#define DEFHEAD  2
#define USERHEAD 3

/* declarations for the <form> stack */
typedef struct forminfo FORMINFO;
struct forminfo {
  char *rowVec;
  int   vecLen;
  };
EXTERN FORMINFO *formStackBase INIT(NULL);
EXTERN FORMINFO *formStackTop  INIT(NULL);
EXTERN FORMINFO *formStackMax  INIT(NULL);

/* flag that we have output an SDL <block> for <partext> */
EXTERN LOGICAL inBlock INIT(FALSE);

/* flag that we need to output an <fdata> for a <form> */
EXTERN LOGICAL needFData INIT(FALSE);

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
EXTERN int     svheadlen ;
EXTERN int     svtclen ;
EXTERN LOGICAL savhd ;
EXTERN LOGICAL savtc ;
EXTERN LOGICAL hderr ;
EXTERN LOGICAL echo ;
EXTERN LOGICAL tooutput INIT(TRUE) ;

/* Save example until we see an annotation (which will behave like a head */
EXTERN char    *saveex;
EXTERN int      svexlen;
EXTERN char    *saveexseg;
EXTERN int      svexseglen;
EXTERN LOGICAL  savex;
EXTERN LOGICAL  stackex;
EXTERN LOGICAL  firstAnnot;
EXTERN LOGICAL  tonumexlines;
EXTERN int      exLineNum;
EXTERN int      oldExLineNum;

/* Glossary Declarations */
EXTERN M_TRIE gtree ;
#define MAXTERM 200
EXTERN M_WCHAR term[MAXTERM + 1] ;
EXTERN M_WCHAR *termp ;
EXTERN LOGICAL wsterm ;
EXTERN int     lastTermId;

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
EXTERN LOGICAL firstPInBlock; /* give the first para a different ssi= */

EXTERN LOGICAL lablisttight[MAXLISTLEV];
EXTERN int labhid[MAXLISTLEV];
EXTERN int labhtextid[MAXLISTLEV];
EXTERN int labelid[MAXLISTLEV];
EXTERN int labeltextid[MAXLISTLEV];
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

typedef struct rowvec ROWVEC;
struct rowvec {
  unsigned char firstitem;
  unsigned char longlabel;
  };
EXTERN ROWVEC listitems[MAXLISTLEV+1];

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

/* Function prototypes */
#include "fproto.h"

/* Declarations for handling special characters */
#include "roman8.h"

/* Declarations for handling special characters for calculators*/
EXTERN char ccoutbuf[128] ;

/* Character conversion tables */
#include "charconv.h"
