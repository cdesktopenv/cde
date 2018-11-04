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
/*
 *  Copyright 1993 Open Software Foundation, Inc., Cambridge, Massachusetts.
 *  All rights reserved.
 */
/*
 * Copyright (c) 1994  
 * Open Software Foundation, Inc. 
 *  
 * Permission is hereby granted to use, copy, modify and freely distribute 
 * the software in this file and its documentation for any purpose without 
 * fee, provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation.  Further, provided that the name of Open 
 * Software Foundation, Inc. ("OSF") not be used in advertising or 
 * publicity pertaining to distribution of the software without prior 
 * written permission from OSF.  OSF makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is" 
 * without express or implied warranty. 
 */
/* ________________________________________________________________________
 *
 *  Common definitions for "instant" program.
 * ________________________________________________________________________
 */

#ifdef STORAGE
#ifndef lint
static char *gen_h_RCSid =
  "$XConsortium: general.h /main/11 1996/08/08 14:41:50 cde-hp $";
#endif
#endif

/* get Tcl header so the variables make sense */
#include <tcl.h>

/* Compatibility with pre 8.6 versions */
#if (TCL_MAJOR_VERSION < 8) || (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION < 6)
# define Tcl_GetErrorLine(x) ((x)->errorLine)
#endif

/* instant variable delimiter (can't use '$', collides with Tcl) */
#define VDELIM		'@'

/* string/numeric/character definitions */

#define EOS		'\0'
#define NL		'\n'
#define TAB		'\t'
#define CR		'\r'
#define ANCHOR		'^'

/* bigmask/flags for the Split() function */
#define S_STRDUP	0x01
#define S_ALVEC		0x02

/*  Command codes (1st char of esis lines) from sgmls.  See its manpage. */
#define CMD_DATA	'-'
#define CMD_OPEN	'('
#define CMD_CLOSE	')'
#define CMD_ATT		'A'
#define CMD_D_ATT	'D'
#define CMD_NOTATION	'N'
#define CMD_EXT_ENT	'E'
#define CMD_INT_ENT	'I'
#define CMD_SYSID	's'
#define CMD_PUBID	'p'
#define CMD_FILENAME	'f'
#define CMD_LINE	'L'
#define CMD_PI		'?'
#define CMD_SUBDOC	'S'
#define CMD_SUBDOC_S	'{'
#define CMD_SUBDOC_E	'}'
#define CMD_EXT_REF	'&'
#define CMD_APPINFO	'#'
#define CMD_CONFORM	'C'

/*  Some sizes */
#define MAX_DEPTH	40
#define LINESIZE	60000

/*  Name of library env variable, and default value. */
#ifndef TPT_LIB
#define TPT_LIB	"TPT_LIB"
#endif
#ifndef DEF_TPT_LIB
#define DEF_TPT_LIB	"/project/tools/libdata/tpt"
#endif

/* for buffered output (buffer gets passed to interpreter) */
typedef struct _OutputBuffer {
    char *base;     /* pointer to start of buffer */
    char *current;  /* pointer to current location in buffer */
    int   size;     /* total size of buffer */
} OutputBuffer_t;

#define OBUF_INCR 4096 /* enlarge buffer by this when full */
#ifndef MIN
#define MIN(i,j) (((i) < (j)) ? (i) : (j))
#endif

/*  Relationships - for querying */
typedef enum {
    REL_None, REL_Parent, REL_Child, REL_Ancestor, REL_Descendant,
    REL_Sibling, REL_Preceding, REL_ImmPreceding, REL_Following,
    REL_ImmFollowing, REL_Cousin, REL_Is1stContent, REL_HasOnlyContent,
    REL_Unknown
} Relation_t;

/* Initial map sizes (IMS) */
#define IMS_relations		3
#define IMS_setvar		3
#define IMS_incvar		3
#define IMS_sdata		50
#define IMS_sdatacache		30
#define IMS_variables		20
#define IMS_attnames		50
#define IMS_elemnames		50

/* ----- structure definitions ----- */

/*  We use this for variables, attributes, etc., so the caller only needs an
 *  opaque handle to the thing below, not worrying about array management.  */
typedef struct {
    char	*name;			/* name of the thing */
    char	*sval;			/* string value */
} Mapping_t;

typedef struct {
    int		n_alloc;		/* number of elements allocated */
    int		n_used;			/* number of elements used */
    int		slot_incr;		/* increment for allocating slots */
    int		flags;			/* info about this set of mappings */
    Mapping_t	*maps;			/* array of mappings */
} Map_t;

/* ______________________________________________________________________ */

/*  Information about an entity reference.  Not all fields will be used
 *  at once.  */
typedef struct _ent {
    char	*type;			/* entity type */
    char	*ename;			/* entity name */
    char	*nname;			/* notation name */
    char	*sysid;			/* sys id */
    char	*pubid;			/* pub id */
    char	*fname;			/* filename */
    struct _ent	*next;			/* next in linked list */
} Entity_t;

/*  Content (child nodes) of an element (node in the tree) -- both data
 *  and other elements.  */
typedef struct {
    char		type;		/* element, data, or pi? */
    union {
	struct _elem	*elem;		/* direct children of this elem */
	char		*data;		/* character data of this elem */
    } ch;
} Content_t;

/*  An element (node in the tree) */
typedef struct _elem {
    char	*gi;			/* element GI */
    Content_t	*cont;			/* content - element & data children */
    int		ncont;			/* # of content/children */
    struct _elem **econt;		/* element children */
    int		necont;			/* # of element children */
    char	**dcont;		/* character data children */
    int		ndcont;			/* # of data children */
    Mapping_t	*atts;			/* array of attributes */
    int		natts;			/* # of attributes */
    Entity_t	*entity;		/* ext entity & notation info */
    char	*id;			/* for linking */
    int		index;			/* an internal bookkeeping mechanism */
    int		depth;			/* how deep in tree */
    int		lineno;			/* line number */
    char	*infile;		/* input filename */
    int		my_eorder;		/* order of this elem of its parent */
    struct _elem *parent;		/* this elem's direct parent */
    struct _elem *next;			/* kept in linked list */
    void	*trans;			/* pointer to translation spec */
    /* I'm not crazy about this, but it works */
    int		gen_trans[2];		/* refs to generated trans specs */
    int		processed;		/* was this node processed? */
} Element_t;

/*  For mapping of element IDs to elements themselves.  */
typedef struct id_s {
    char	*id;			/* ID of the element */
    Element_t	*elem;			/* pointer to it */
    struct id_s	*next;
} ID_t;

/* ----- global variable declarations ----- */

#ifdef STORAGE
# define def
#else
# define def	extern
#endif

def Element_t	*DocTree;		/* root of document tree */
def char	**UsedElem;		/* a unique list of used elem names */
def int		nUsedElem;		/* number of used elem names */
def char	**UsedAtt;		/* a unique list of used attrib names */
def int		nUsedAtt;		/* number of used attrib names */
def ID_t	*IDList;		/* list of IDs used in the doc */
def Map_t	*Variables;		/* general, global variables */
def Map_t	*SDATAmap;		/* SDATA mappings */
def Map_t	*PImap;			/* Processing Instruction mappings */
def Entity_t	*Entities;		/* list of entities */

def FILE	*outfp;			/* where output is written */
def char	*tpt_lib;		/* TPT library directory */
def int		verbose;		/* flag - verbose output? */
def int		warnings;		/* flag - show warnings? */
def int		interactive;		/* flag - interactive browsing? */
def int		slave;			/* are we slave to another process? */
def int		fold_case;		/* flag - fold case of GIs? */
def Tcl_Interp	*interpreter;		/* interp for start_ and end_code */
def Element_t   *tclE;                  /* active element in Tcl call */


/* ----- some macros for convenience and ease of code reading ----- */

/* #define stripNL(s)	{ char *_cp; if ((_cp=strchr(s, NL))) *_cp = EOS; } */
#define stripNL(s)	{ char *_cp = s; while (*_cp) _cp++; \
			  if ((_cp > s) && (*--_cp == NL)) *_cp = EOS; }

#define I18N_TRIGGER 0xFF

/*  Similar to calloc(), malloc(), and realloc(), but check for success.
 *  Args to all:
 *	(1) number of 'elements' to allocate
 *	(2) variable to point at allocated space
 *	(3) type of 'element'
 *  Eg:	Calloc(5, e, Element_t) replaces
 *	if (!(e = (Element_t *)calloc(5, sizeof(Element_t))) {
 *		... handle error ... ;
 *	}
 */
#define Calloc(N,V,T)	\
    { if (!((V) = (T *)calloc((size_t)N, sizeof(T)))) { \
	perror("Calloc failed -- out of memory.  Bailing out.");  exit(1); \
    } }
#define Malloc(N,V,T)	\
    { if (!((V) = (T *)malloc((size_t)N*sizeof(T)))) { \
	perror("Malloc failed -- out of memory.  Bailing out.");  exit(1); \
    } }
#define Realloc(N,V,T)	\
    { if (!((V) = (T *)realloc(V,(size_t)N*sizeof(T)))) { \
	perror("Realloc failed -- out of memory.  Bailing out.");  exit(1); \
    } }

/*  similar to strcmp(), but check first chars first, for efficiency */
#define StrEq(s1,s2)	(s1[0] == s2[0] && !strcmp(s1,s2))

/*  similar to isspace(), but check for blank or tab - without overhead
 *  of procedure call */
#define IsWhite(c)	(c == ' ' || c == TAB)

#define ContType(e,i)	(e->cont[i].type)
#define ContData(e,i)	(e->cont[i].ch.data)
#define ContElem(e,i)	(e->cont[i].ch.elem)
#define IsContData(e,i)	(e->cont[i].type == CMD_DATA)
#define IsContElem(e,i)	(e->cont[i].type == CMD_OPEN)
#define IsContPI(e,i)	(e->cont[i].type == CMD_PI)

/* ----- function prototypes ----- */

/* things defined in util.c */
Element_t	*QRelation(Element_t *, char *, Relation_t);
Relation_t	FindRelByName(char *);
char		*FindAttValByName(Element_t *, char *);
char		*FindContext(Element_t *, int, char *);
char		*AddElemName(char *);
char		*AddAttName(char *);
void		 OutputString(char *, FILE *, int);
int		Putc(int, FILE *);
int		FPuts(const char *, FILE *);
int		FFlush(FILE *);
FILE		*OpenFile(char *);
char		*FindElementPath(Element_t *, char *);
char		*NearestOlderElem(Element_t *, char *);
void		PrintLocation(Element_t *, FILE *);
char		**Split(char *, int *, int);
void		DescendTree(Element_t *, void(*)(), void(*)(), void(*)(), void *);
Map_t		*NewMap(int);
Mapping_t	*FindMapping(Map_t *, char *);
char		*FindMappingVal(Map_t *, char *);
void		SetMapping(Map_t *, char *);
void		SetMappingNV(Map_t *, char *, char *);
void		AddID(Element_t *, char *);
Element_t	*FindElemByID(char *);

/* things defined in translate.c */
void		DoTranslate(Element_t*, char *, FILE *);

/* things defined in traninit.c */
void		ReadTransSpec(char *);
void		ReadSDATA(char *);
void		ReadCharMap(char *);

/* things defined in info.c */
void		PrintContext(Element_t *e);
void		PrintElemSummary(Element_t *);
void		PrintElemTree(Element_t *);
void		PrintStats(Element_t *);
void		PrintIDList();

