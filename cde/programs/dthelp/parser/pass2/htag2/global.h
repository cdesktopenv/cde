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
/* $TOG: global.h /main/4 1998/04/06 13:19:19 mgreess $ */
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

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__) || defined(linux) || defined(CSRG_BASED)
#include <unistd.h>
#include <locale.h>
#include <fcntl.h>
#endif

#ifndef MB_MAX_LEN
#define MB_MAX_LEN 4
#endif

#ifndef WCOREDUMP
#define WCOREDUMP(_X) ((int)(_X)&0200)
#endif

/*????????????????????????????????????????????????????*/
EXTERN LOGICAL stoponerror INIT(TRUE) ;

EXTERN char dirsep INIT('/') ;
EXTERN char *indir INIT(NULL) ;
EXTERN char *nodirbase ;
EXTERN char *install INIT(NULL) ;

typedef struct search SEARCH ;
struct search {
  char *directory ;
  SEARCH *next ;
  } ;
EXTERN SEARCH *path INIT(NULL) ;
EXTERN SEARCH **endpath INIT(&path) ;

/*====================================================*/
#define xxx 1

/* Must be in same order as values in optkey, adjust NUMOPTIONS if changed*/
#define ONERROR      1

#define NUMOPTIONS 1

EXTERN int optval INIT(M_NULLVAL) ;

EXTERN char *optkey[NUMOPTIONS]
#if defined(DEFINE)
  = {
      "onerror",
      }
#endif
  ;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define RS       '\036'
#define LINEMAX  (BUFSIZ * 8)

EXTERN LOGICAL haveIndex INIT(FALSE);

#define TMPDIR  "/tmp"
#define VERSION "Version"

#define SDLNAMESIZ sizeof("SDL-RESERVED")
EXTERN char  sdlReservedName[] INIT("SDL-RESERVED");

#define DOTSDLSIZ sizeof(".sdl")
EXTERN char  dotSDL[]          INIT(".sdl");
#define DOTERRSIZ sizeof(".err")
EXTERN char  dotERR[]          INIT(".err");
#define DOTIDXSIZ sizeof(".idx")
EXTERN char  dotIDX[]          INIT(".idx");
#define DOTSNBSIZ sizeof(".snb")
EXTERN char  dotSNB[]          INIT(".snb");
#define DOTTSSSIZ sizeof(".tss")
EXTERN char  dotTSS[]          INIT(".tss");

/* set to TRUE if the user asks for compression */
EXTERN LOGICAL compressFlag    INIT(FALSE);
/* set to TRUE if the user asks for optimization */
EXTERN LOGICAL optimizeFlag    INIT(FALSE);
/* set to TRUE if the user asks for <toss> to be necessary and sufficient */
EXTERN LOGICAL minimalTossFlag INIT(FALSE);
/* set to TRUE if the user asks for decompression */
EXTERN LOGICAL decompressFlag  INIT(FALSE);

/* we change outFile from vstructFile to tempFile upon first virpage */
EXTERN LOGICAL firstVirpage    INIT(TRUE);
EXTERN long int vstructSize    INIT(0);

/* the base name of this program - used in error messages */
EXTERN char *progName          INIT(NULL);

/*
 * Names for all the input, output and intermediate files we'll need.
*/
char *inFileName;
char *outFileName;
char *errFileName;
char *idxFileName;
char *snbFileName;
char *sortedIdxFileName;
char *tossFileName;
char *vstructFileName;
char *tempFileName;
char *compFileName;
char *compZFileName;

FILE *inFile;
FILE *outFile;

typedef struct _loidsRec *LoidsPtr;
typedef struct _loidsRec {
    char     *type;
    char     *rid;
    char     *rssi;
    char     *rlevel;
    long int  offset;
    LoidsPtr  next;
} LoidsRec;

EXTERN LoidsRec loids; /* first node is a dummy to make the logic easier */
EXTERN LoidsPtr pLoidsEnd INIT(&loids);

EXTERN int startOfLOIDS     INIT(0);
EXTERN int startOfIndex     INIT(0);
EXTERN int startOfLOPhrases INIT(0);
EXTERN int startOfTOSS      INIT(0);
EXTERN int endOfLOIDS       INIT(0);
EXTERN int endOfIndex       INIT(0);
EXTERN int endOfLOPhrases   INIT(0);
EXTERN int endOfTOSS        INIT(0);

typedef enum _element_types {
  e_sdldoc,
  e_vstruct,
  e_virpage,
  e_snb,
  e_block,
  e_form,
  e_fstyle,
  e_frowvec,
  e_fdata,
  e_p,
  e_cp,
  e_head,
  e_subhead,
  e_anchor,
  e_link,
  e_snref,
  e_refitem,
  e_alttext,
  e_rev,
  e_key,
  e_sphrase,
  e_if,
  e_cond,
  e_then,
  e_else,
  e_spc,
  e_graphic,
  e_text,
  e_audio,
  e_video,
  e_animate,
  e_script,
  e_crossdoc,
  e_man_page,
  e_textfile,
  e_sys_cmd,
  e_callback,
  e_switch,
  e_rel_docs,
  e_rel_file,
  e_notes,
  e_loids,
  e_id,
  e_index,
  e_entry,
  e_lophrases,
  e_phrase,
  e_toss,
  e_keystyle,
  e_headstyle,
  e_formstyle,
  e_frmtstyle,
  e_grphstyle,
  e_cdata
} ElementTypes;

typedef struct _element *ElementPtr;

typedef struct _sdldoc {
  ElementPtr pVstruct;
  ElementPtr pHead;
  ElementPtr pSnb;
  ElementPtr pVirpage;
  M_WCHAR *pub_id;
  M_WCHAR *doc_id;
  M_WCHAR *timestmp;
  M_WCHAR *first_page;
  M_WCHAR *product;
  M_WCHAR *prodpn;
  M_WCHAR *prodver;
  M_WCHAR *license;
  M_WCHAR *language;
  M_WCHAR *charset;
  M_WCHAR *author;
  M_WCHAR *version;
  M_WCHAR *sdldtd;
  M_WCHAR *srcdtd;
} Sdldoc_el;

typedef struct _vstruct {
  ElementPtr pLoids;
  ElementPtr pToss;
  ElementPtr pLophrases;
  ElementPtr pIndex;
  ElementPtr pRel_docs;
  ElementPtr pRel_file;
  ElementPtr pNotes;
  M_WCHAR *version;
  M_WCHAR *doc_id;
} Vstruct_el;

typedef struct _virpage {
  ElementPtr pHead;
  ElementPtr pSnb;
  ElementPtr pBlockOrForm;
  M_WCHAR *id;
  M_WCHAR *level;
  M_WCHAR *version;
  M_WCHAR *language;
  M_WCHAR *charset;
  M_WCHAR *doc_id;
  M_WCHAR *ssi;
} Virpage_el;

typedef struct _snb {
  ElementPtr pHead;
  ElementPtr pSystemNotations;
  M_WCHAR *version;
} Snb_el;

typedef struct _block {
  ElementPtr pHead;
  ElementPtr pCpOrP;
  M_WCHAR *id;
  M_WCHAR *level;
  M_WCHAR *version;
  M_WCHAR *class;
  M_WCHAR *language;
  M_WCHAR *charset;
  M_WCHAR *length;
  M_WCHAR *app;
  M_WCHAR *timing;
  M_WCHAR *ssi;
} Block_el;

typedef struct _form {
  ElementPtr pHead;
  ElementPtr pFdata;
  ElementPtr pFstyle;
  M_WCHAR *id;
  M_WCHAR *level;
  M_WCHAR *version;
  M_WCHAR *class;
  M_WCHAR *language;
  M_WCHAR *charset;
  M_WCHAR *length;
  M_WCHAR *app;
  M_WCHAR *ssi;
} Form_el;

typedef struct _fstyle {
  ElementPtr pFrowvec;
  M_WCHAR *ncols;
} Fstyle_el;

typedef struct _frowvec {
  M_WCHAR *hdr;
  M_WCHAR *cells;
} Frowvec_el;

typedef struct _fdata {
  ElementPtr pBlockOrForm;
} Fdata_el;

typedef struct _p {
  ElementPtr pHead;
  ElementPtr pAtomicOrPCDATA;
  M_WCHAR *id;
  M_WCHAR *version;
  M_WCHAR *type;
  M_WCHAR *ssi;
} P_el;

typedef struct _cp {
  ElementPtr pCDATA;
  M_WCHAR *id;
  M_WCHAR *version;
  M_WCHAR *type;
  M_WCHAR *ssi;
} Cp_el;

typedef struct _head {
  ElementPtr pAtomicOrPCDATA;
  ElementPtr pSubhead;
  M_WCHAR *id;
  M_WCHAR *version;
  M_WCHAR *class;
  M_WCHAR *language;
  M_WCHAR *charset;
  M_WCHAR *type;
  M_WCHAR *abbrev;
  M_WCHAR *ssi;
} Head_el;

typedef struct _subhead {
  ElementPtr pAtomicOrPCDATA;
  M_WCHAR *id;
  M_WCHAR *version;
  M_WCHAR *class;
  M_WCHAR *language;
  M_WCHAR *charset;
  M_WCHAR *type;
  M_WCHAR *abbrev;
  M_WCHAR *ssi;
} Subhead_el;

typedef struct _anchor {
  M_WCHAR *id;
} Anchor_el;

typedef struct _link {
  ElementPtr pAtomicOrPCDATA;
  M_WCHAR *rid;
  M_WCHAR *button;
  M_WCHAR *linkinfo;
  M_WCHAR *descript;
  M_WCHAR *window;
  M_WCHAR *traversal;
} Link_el;

typedef struct _snref {
  ElementPtr pRefitem;
  ElementPtr     pAlttext;
  M_WCHAR *id;
} Snref_el;

typedef struct _refitem {
  ElementPtr pHead;
  ElementPtr pReferencedItem;
  M_WCHAR *rid;
  M_WCHAR *class;
  M_WCHAR *button;
  M_WCHAR *ssi;
} Refitem_el;

typedef struct _alttext {
  ElementPtr pCDATA;
} Alttext_el;

typedef struct _rev {
  ElementPtr pAtomicOrPCDATA;
} Rev_el;

typedef struct _key {
  ElementPtr pAtomicOrPCDATA;
  M_WCHAR *charset;
  M_WCHAR *class;
  M_WCHAR *ssi;
} Key_el;

typedef struct _sphrase {
  ElementPtr pSpcOrPCDATA;
  M_WCHAR *class;
  M_WCHAR *ssi;
} Sphrase_el;

typedef struct _if {
  ElementPtr pCond;
  ElementPtr pThen;
  ElementPtr pElse;
} If_el;

typedef struct _cond {
  ElementPtr pCDATA;
  M_WCHAR *interp;
} Cond_el;

typedef struct _then {
  ElementPtr pAtomicOrPCDATA;
} Then_el;

typedef struct _else {
  ElementPtr pAtomicOrPCDATA;
} Else_el;

typedef struct _spc {
  M_WCHAR *name;
} Spc_el;

typedef struct _graphic {
  M_WCHAR *id;
  M_WCHAR *format;
  M_WCHAR *method;
  M_WCHAR *xid;
} Graphic_el;

typedef struct _text {
  ElementPtr pCpOrP;
  M_WCHAR *id;
  M_WCHAR *language;
  M_WCHAR *charset;
} Text_el;

typedef struct _audio {
  M_WCHAR *id;
  M_WCHAR *format;
  M_WCHAR *method;
  M_WCHAR *xid;
} Audio_el;

typedef struct _video {
  M_WCHAR *id;
  M_WCHAR *format;
  M_WCHAR *method;
  M_WCHAR *xid;
} Video_el;

typedef struct _animate {
  M_WCHAR *id;
  M_WCHAR *format;
  M_WCHAR *method;
  M_WCHAR *xid;
} Animate_el;

typedef struct _script {
  ElementPtr pCDATA;
  M_WCHAR *id;
  M_WCHAR *interp;
} Script_el;

typedef struct _crossdoc {
  M_WCHAR *id;
  M_WCHAR *xid;
} Crossdoc_el;

typedef struct _man_page {
  M_WCHAR *id;
  M_WCHAR *xid;
} Man_page_el;

typedef struct _textfile {
  M_WCHAR *id;
  M_WCHAR *xid;
  M_WCHAR *offset;
  M_WCHAR *format;
} Textfile_el;

typedef struct _sys_cmd {
  M_WCHAR *id;
  M_WCHAR *command;
} Sys_cmd_el;

typedef struct _callback {
  M_WCHAR *id;
  M_WCHAR *data;
} Callback_el;

typedef struct _switch {
  ElementPtr pCDATA;
  M_WCHAR *id;
  M_WCHAR *interp;
  M_WCHAR *branches;
} Switch_el;

typedef struct _rel_docs {
  void *no_attrs;
} Rel_docs_el;

typedef struct _rel_file {
  void *no_attrs;
} Rel_file_el;

typedef struct _notes {
  void *no_attrs;
} Notes_el;

typedef struct _loids {
  ElementPtr pId;
} Loids_el;

typedef struct _id {
  M_WCHAR *type;
  M_WCHAR *rid;
  M_WCHAR *rssi;
  M_WCHAR *rlevel;
  M_WCHAR *offset;
} Id_el;

typedef struct _index {
  ElementPtr pEntry;
  M_WCHAR *head;
} Index_el;

typedef struct _entry {
  ElementPtr pSimpleOrPCDATA;
  ElementPtr pEntry;
  M_WCHAR *id;
  M_WCHAR *main;
  M_WCHAR *locs;
  M_WCHAR *syns;
  M_WCHAR *sort;
} Entry_el;

typedef struct _lophrases {
  ElementPtr pPhrase;
} Lophrases_el;

typedef struct _phrase {
  M_WCHAR *text;
} Phrase_el;

typedef struct _toss {
  ElementPtr pKeyStyle;
  ElementPtr pHeadStyle;
  ElementPtr pFormStyle;
  ElementPtr pFrmtStyle;
  ElementPtr pGrphStyle;
} Toss_el;

typedef struct _keystyle {
  LOGICAL  beenUsed;
  M_WCHAR *class;
  M_WCHAR *ssi;
  M_WCHAR *rlevel;
  M_WCHAR *phrase;
  M_WCHAR *srch_wt;
  M_WCHAR *enter;
  M_WCHAR *exit;
  M_WCHAR *pointsz;
  M_WCHAR *set_width;
  M_WCHAR *color;
  M_WCHAR *xlfd;
  M_WCHAR *xlfdi;
  M_WCHAR *xlfdb;
  M_WCHAR *xlfdib;
  M_WCHAR *typenam;
  M_WCHAR *typenami;
  M_WCHAR *typenamb;
  M_WCHAR *typenamib;
  M_WCHAR *style;
  M_WCHAR *spacing;
  M_WCHAR *weight;
  M_WCHAR *slant;
  M_WCHAR *special;
} Keystyle_el;

typedef struct _headstyle {
  LOGICAL  beenUsed;
  M_WCHAR *class;
  M_WCHAR *ssi;
  M_WCHAR *rlevel;
  M_WCHAR *phrase;
  M_WCHAR *srch_wt;
  M_WCHAR *orient;
  M_WCHAR *vorient;
  M_WCHAR *placement;
  M_WCHAR *headw;
  M_WCHAR *stack;
  M_WCHAR *flow;
  M_WCHAR *pointsz;
  M_WCHAR *set_width;
  M_WCHAR *color;
  M_WCHAR *xlfd;
  M_WCHAR *xlfdi;
  M_WCHAR *xlfdb;
  M_WCHAR *xlfdib;
  M_WCHAR *typenam;
  M_WCHAR *typenami;
  M_WCHAR *typenamb;
  M_WCHAR *typenamib;
  M_WCHAR *style;
  M_WCHAR *spacing;
  M_WCHAR *weight;
  M_WCHAR *slant;
  M_WCHAR *special;
  M_WCHAR *l_margin;
  M_WCHAR *r_margin;
  M_WCHAR *t_margin;
  M_WCHAR *b_margin;
  M_WCHAR *border;
  M_WCHAR *vjust;
  M_WCHAR *justify;
} Headstyle_el;

typedef struct _formstyle {
  LOGICAL  beenUsed;
  M_WCHAR *class;
  M_WCHAR *ssi;
  M_WCHAR *rlevel;
  M_WCHAR *phrase;
  M_WCHAR *srch_wt;
  M_WCHAR *colw;
  M_WCHAR *colj;
  M_WCHAR *pointsz;
  M_WCHAR *set_width;
  M_WCHAR *color;
  M_WCHAR *xlfd;
  M_WCHAR *xlfdi;
  M_WCHAR *xlfdb;
  M_WCHAR *xlfdib;
  M_WCHAR *typenam;
  M_WCHAR *typenami;
  M_WCHAR *typenamb;
  M_WCHAR *typenamib;
  M_WCHAR *style;
  M_WCHAR *spacing;
  M_WCHAR *weight;
  M_WCHAR *slant;
  M_WCHAR *special;
  M_WCHAR *l_margin;
  M_WCHAR *r_margin;
  M_WCHAR *t_margin;
  M_WCHAR *b_margin;
  M_WCHAR *border;
  M_WCHAR *vjust;
} Formstyle_el;

typedef struct _frmtstyle {
  LOGICAL  beenUsed;
  M_WCHAR *class;
  M_WCHAR *ssi;
  M_WCHAR *rlevel;
  M_WCHAR *phrase;
  M_WCHAR *f_margin;
  M_WCHAR *srch_wt;
  M_WCHAR *pointsz;
  M_WCHAR *set_width;
  M_WCHAR *color;
  M_WCHAR *xlfd;
  M_WCHAR *xlfdi;
  M_WCHAR *xlfdb;
  M_WCHAR *xlfdib;
  M_WCHAR *typenam;
  M_WCHAR *typenami;
  M_WCHAR *typenamb;
  M_WCHAR *typenamib;
  M_WCHAR *style;
  M_WCHAR *spacing;
  M_WCHAR *weight;
  M_WCHAR *slant;
  M_WCHAR *special;
  M_WCHAR *l_margin;
  M_WCHAR *r_margin;
  M_WCHAR *t_margin;
  M_WCHAR *b_margin;
  M_WCHAR *border;
  M_WCHAR *vjust;
  M_WCHAR *justify;
} Frmtstyle_el;

typedef struct _grphstyle {
  LOGICAL  beenUsed;
  M_WCHAR *class;
  M_WCHAR *ssi;
  M_WCHAR *rlevel;
  M_WCHAR *phrase;
  M_WCHAR *l_margin;
  M_WCHAR *r_margin;
  M_WCHAR *t_margin;
  M_WCHAR *b_margin;
  M_WCHAR *border;
  M_WCHAR *vjust;
  M_WCHAR *justify;
} Grphstyle_el;

typedef struct _cdata {
  M_WCHAR *buffer;
  int      buffPos;
  int      buffSize;
} CDATA_el;

typedef struct _element {
  ElementTypes  type;
  ElementPtr    pNext;
  LOGICAL       beenEmitted;
  LOGICAL       tossChecked;
  M_WCHAR      *inheritedLevel;
  M_WCHAR      *inheritedClass;
  union {
    Sdldoc_el    u_sdldoc;
    Vstruct_el   u_vstruct;
    Virpage_el   u_virpage;
    Snb_el       u_snb;
    Block_el     u_block;
    Form_el      u_form;
    Fstyle_el    u_fstyle;
    Frowvec_el   u_frowvec;
    Fdata_el     u_fdata;
    P_el         u_p;
    Cp_el        u_cp;
    Head_el      u_head;
    Subhead_el   u_subhead;
    Anchor_el    u_anchor;
    Link_el      u_link;
    Snref_el     u_snref;
    Refitem_el   u_refitem;
    Alttext_el   u_alttext;
    Rev_el       u_rev;
    Key_el       u_key;
    Sphrase_el   u_sphrase;
    If_el        u_if;
    Cond_el      u_cond;
    Then_el      u_then;
    Else_el      u_else;
    Spc_el       u_spc;
    Graphic_el   u_graphic;
    Text_el      u_text;
    Audio_el     u_audio;
    Video_el     u_video;
    Animate_el   u_animate;
    Script_el    u_script;
    Crossdoc_el  u_crossdoc;
    Man_page_el  u_man_page;
    Textfile_el  u_textfile;
    Sys_cmd_el   u_sys_cmd;
    Callback_el  u_callback;
    Switch_el    u_switch;
    Rel_docs_el  u_rel_docs;
    Rel_file_el  u_rel_file;
    Notes_el     u_notes;
    Loids_el     u_loids;
    Id_el        u_id;
    Index_el     u_index;
    Entry_el     u_entry;
    Lophrases_el u_lophrases;
    Phrase_el    u_phrase;
    Toss_el      u_toss;
    Keystyle_el  u_keystyle;
    Headstyle_el u_headstyle;
    Formstyle_el u_formstyle;
    Frmtstyle_el u_frmtstyle;
    Grphstyle_el u_grphstyle;
    CDATA_el     u_cdata;
  } u;
} Element;

EXTERN ElementPtr  pCurrentElement INIT(NULL);

/* variables for counting entry, phrase and style elements */
EXTERN ElementPtr  pElementChain   INIT(NULL);
EXTERN int         nElements;

/* save the toss for later emission */
EXTERN ElementPtr pTossChain  INIT(NULL);
EXTERN int        nStyles;

EXTERN int        saveSysECnt INIT(-1);

/* Function prototypes */
#include "fproto.h"
