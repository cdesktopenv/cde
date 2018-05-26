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
 *  Program to manipulate SGML instances.
 *
 *  These are data definitions for the "translating" portion of the program.
 *
 * ________________________________________________________________________
 */

#ifdef STORAGE
#ifndef lint
static char *tr_h_RCSid =
  "$XConsortium: translate.h /main/3 1996/06/19 17:13:31 drk $";
#endif
#endif

#define L_CURLY		'{'
#define R_CURLY		'}'

/* things to ignore when processing an element */
#define IGN_NONE	0
#define IGN_ALL		1
#define IGN_DATA	2
#define IGN_CHILDREN	3

/* for CheckRelation() */
typedef enum { RA_Current, RA_Related } RelAction_t;

/* for ExpandSpecialVar() */
typedef enum { CONT_CONTINUE, CONT_BREAK } ContParse_t;

typedef struct {
    char	*name;		/* attribute name string */
    char	*val;		/* attribute value string */
    regexp	*rex;		/* attribute value reg expr (compiled) */
} AttPair_t;

typedef struct _Trans {
    /* criteria */
    char	*gi;		/* element name of tag under consideration */
    char	**gilist;	/* list of element names (multiple gi's) */
    char	*context;	/* context in tree - looking depth levels up */
    regexp	*context_re;	/* tree hierarchy looking depth levels up */
    int		depth;		/* number of levels to look up the tree */
    AttPair_t	*attpair;	/* attr name-value pairs */
    int		nattpairs;	/* number of name-value pairs */
    char	*parent;	/* GI has this element as parent */
    int		nth_child;	/* GI is Nth child of this of parent element */
    char	*content;	/* element has this string in content */
    regexp	*content_re;	/* content reg expr (compiled) */
    char	*pattrset;	/* is this attr set (any value) in parent? */
    char	*var_name;	/* variable name */
    char	*var_value;	/* variable value */
    Map_t	*relations;	/* various relations to check */

    /* actions */
    char	*starttext;	/* string to output at the start tag */
    char	*startcode;	/* Tcl code to execute at the start tag */
    char	*endtext;	/* string to output at the end tag */
    char	*endcode;	/* Tcl code to execute at the end tag */
    char	*replace;	/* string to replace this subtree with */
    char	*message;	/* message for stderr, if element encountered */
    int		ignore;		/* flag - ignore content or data of element? */
    char	*var_reset;
    char	*increment;	/* increment these variables */
    Map_t	*set_var;	/* set these variables */
    Map_t	*incr_var;	/* increment these variables */
    char	*quit;		/* print message and exit */

    /* pointers and bookkeeping */
    int		my_id;		/* unique (hopefully) ID of this transpec */
    int		use_id;		/* use transpec whose ID is this */
    struct _Trans *use_trans;	/* pointer to other transpec */
    struct _Trans *next;	/* linked list */
    int		lineno;		/* line number of end of transpec */
} Trans_t;

#ifdef def
#undef def
#endif
#ifdef STORAGE
# define def
#else
# define def    extern
#endif

def Trans_t	*TrSpecs;
def Mapping_t	*CharMap;
def int		nCharMap;

/* prototypes for things defined in translate.c */
int	CheckRelation(Element_t *, char *, char *, char *, FILE*, RelAction_t);
Trans_t	*FindTrans(Element_t *);
Trans_t	*FindTransByName(char *);
Trans_t	*FindTransByID(int);
void	PrepTranspecs(Element_t *);
void	ProcessOneSpec(char *, Element_t *, FILE *, int);
void	TransElement(Element_t *, FILE *, Trans_t *);
void	TranByAction(Element_t *, int, FILE *);

/* prototypes for things defined in tranvar.c */
ContParse_t	ExpandSpecialVar(char *, Element_t *, FILE *, int);

/* prototypes for things defined in tables.c */
void	OSFtable(Element_t *, FILE *, char **, int);

/* prototypes for things defines in util.c */
void	ClearOutputBuffer();
char	*GetOutputBuffer();
int 	OutputBufferActive();
/* ______________________________________________________________________ */

