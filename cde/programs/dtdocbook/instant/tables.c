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
 *  This module is for handling OSF table markup, printing TeX or tbl
 *  (tbl) markup to the output stream.  Also, table markup checking is
 *  done here.  Yes, this depends on the DTD, but it makes translation
 *  specs much cleaner (and makes some things possible.
 *
 *  Incomplete / not implemented / limitations / notes:
 *	vertical alignment (valign attr)
 *	vertical spanning
 *	'wrap hint' attribute
 *	row separators are for the whole line, not per cell (the prog looks
 *		at rowsep for the 1st cell and applies it to the whole row)
 *	trusts that units if colwidths are acceptable to LaTeX and tbl
 *	"s" is an acceptable shorthand for "span" in model attributes
 *
 *  A note on use of OutputString():  Strings with backslashes (\) need lots
 *  of backslashes.  You have to escape them for the C compiler, and escape
 *  them again for OutputString() itself.
 * ________________________________________________________________________
 */

#ifndef lint
static char *RCSid =
  "$XConsortium: tables.c /main/3 1996/06/19 17:13:17 drk $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <errno.h>

#include <tptregexp.h>
#include "general.h"
#include "translate.h"

/* text width of page, in inches */
#define TEXTWIDTH	6.0
#define MAXCOLS		100
#define SPAN_NOT	0
#define SPAN_START	1
#define SPAN_CONT	2

/* these cover the attributes on the table element */
typedef struct {
    char	*ncols;
    char	*halign,    **halign_v;
    char	*model,     **model_v;
    char	*colwidth,  **colwidth_v;
    char	*colsep,    **colsep_v;
    char	*colweight, **colweight_v;
    char	*frame;
    int		n_halign, n_model, n_colwidth, n_colsep, n_colweight;
    int		repeathead;	
    int		nc;
} TableInfo;


/* some flags, set when the table tag is processed, used later */
static int	rowsep, siderules;
static int	frametop, framebot, frameall;
static char	basemodel[128];	/* model for table (in formatting language) */
static int	spaninfo[MAXCOLS];	/* 100 columns, max */
static TableInfo	TheTab;

/* forward references */
void	SetTabAtts(Element_t *, TableInfo *, int);
void	FreeTabAtts(TableInfo	*);
void	CheckTable(Element_t *);
void	TblTable(Element_t *, FILE *);
void	TblTableCellStart(Element_t *, FILE *);
void	TblTableCellEnd(Element_t *, FILE *);
void	TblTableRowStart(Element_t *, FILE *);
void	TblTableRowEnd(Element_t *, FILE *);
void	TblTableTop(Element_t *, FILE *);
void	TblTableBottom(Element_t *, FILE *);
void	TexTable(Element_t *, FILE *);
void	TexTableCellStart(Element_t *, FILE *);
void	TexTableCellEnd(Element_t *, FILE *);
void	TexTableRowStart(Element_t *, FILE *);
void	TexTableRowEnd(Element_t *, FILE *);
void	TexTableTop(Element_t *, FILE *);
void	TexTableBottom(Element_t *, FILE *);

/* ______________________________________________________________________ */
/*  Hard-coded stuff for OSF DTD tables.
 *  Here are the TABLE attributes (for handy reference):
 *	ncols      NUMBER	num of cells/row should match 
 *	model      CDATA	column prototypes for this table 
 *	colwidth   NUTOKENS	absolute widths of cols 
 *	colweight  NUMBERS	column weights 
 *	halign     CDATA	horiz alignment for columns 
 *	valign     CDATA	vertical alignment for columns 
 *	colsep     NUMBERS	use col separators (lines)? 
 *	rowsep     NUMBERS	use row separators (lines)? 
 *	wrap       NUMBERS	wrap hints for columns
 *	repeathead NUMBER	carry title rows to other pages
 *	frame      (top|bottom|topbot|all|sides|none)	frame style
 *
 *  The 'wrap' attribute is never used.
 *
 *  Usage in transpec: _osftable [tex|tbl|check] ['aspect']
 *  where 'aspect' is:
 *	rowstart	stuff to do at start of a row (tests for spanning)
 *	rowend		stuff to do at end of a row (eg, rules, etc.)
 *	cellstart	stuff to do at start of a cell (eg, handle actual
 *			spanning instructions, etc.)
 *	cellend		stuff to do at end of a cell  (eg, cell separator)
 *	top		stuff to do at top of the table
 *			(like whether or not it needs a starting horiz rule)
 *	bottom		stuff to do at bottom of the table
 *			(like whether or not it needs an ending horiz rule)
 *	(nothing)	the 'cols' param to LaTeX's \begin{tabular}[pos]{cols}
 *			or 'options' and 'formats' part in tbl
 */

/*  Procedure to
 *  Arguments:
 *	Pointer to element under consideration.
 *	FILE pointer to where to write output.
 *	Vector of args to _osftable
 *	Count of args to _osftable
 */
void
OSFtable(
    Element_t	*e,
    FILE	*fp,
    char	**av,
    int		ac
)
{
    /* Check params and dispatch to appropriate routine */

    if (ac > 1 && !strcmp(av[1], "check")) CheckTable(e);

    else if (!strcmp(av[1], "tbl")) {
	if (ac > 2) {
	    if (!strcmp(av[2], "cellstart"))	TblTableCellStart(e, fp);
	    else if (!strcmp(av[2], "cellend"))	TblTableCellEnd(e, fp);
	    else if (!strcmp(av[2], "rowstart")) TblTableRowStart(e, fp);
	    else if (!strcmp(av[2], "rowend"))	TblTableRowEnd(e, fp);
	    else if (!strcmp(av[2], "top"))	TblTableTop(e, fp);
	    else if (!strcmp(av[2], "bottom"))	TblTableBottom(e, fp);
	    else fprintf(stderr, "Unknown %s table instruction: %s\n",
		av[1], av[2]);
	}
	else TblTable(e, fp);
    }

    else if (!strcmp(av[1], "tex")) {
	if (ac > 2) {
	    if (!strcmp(av[2], "cellstart"))	TexTableCellStart(e, fp);
	    else if (!strcmp(av[2], "cellend"))	TexTableCellEnd(e, fp);
	    else if (!strcmp(av[2], "rowstart")) TexTableRowStart(e, fp);
	    else if (!strcmp(av[2], "rowend"))	TexTableRowEnd(e, fp);
	    else if (!strcmp(av[2], "top"))	TexTableTop(e, fp);
	    else if (!strcmp(av[2], "bottom"))	TexTableBottom(e, fp);
	    else fprintf(stderr, "Unknown %s table instruction: %s\n",
		av[1], av[2]);
	}
	else TexTable(e, fp);
    }

    else fprintf(stderr, "Unknown table type: %s\n", av[1]);

}

/* ______________________________________________________________________ */
/*  Set values of the our internal table structure based on the table's
 *  attributes.  (This is also called for rows, since tables and rows
 *  share many of the same attributes.)
 *  Arguments:
 *	Pointer to element under consideration.
 *	Pointer table info structure which will be filled in.
 *	Flag saying whether or not to set global variables based on attrs.
 */
void
SetTabAtts(
    Element_t	*e,
    TableInfo	*t,
    int		set_globals
)
{
    char	*at;

    memset(t, 0, sizeof(TableInfo));

    /* remember values of attributes */
    if ((at = FindAttValByName(e, "HALIGN")))	  t->halign     = at;
    if ((at = FindAttValByName(e, "MODEL")))	  t->model      = at;
    if ((at = FindAttValByName(e, "COLWIDTH")))	  t->colwidth   = at;
    if ((at = FindAttValByName(e, "COLSEP")))	  t->colsep     = at;
    if ((at = FindAttValByName(e, "COLWEIGHT")))  t->colweight  = at;
    if ((at = FindAttValByName(e, "FRAME")))	  t->frame      = at;
    if ((at = FindAttValByName(e, "REPEATHEAD"))) t->repeathead = atoi(at);
    if ((at = FindAttValByName(e, "NCOLS")))	  t->ncols	= at;

    /* Set some things for later when processing this table */
    if (set_globals) {

	rowsep = 1;
	frametop = framebot = 1;		/* default style */

	/* For now we look at the first number of rowsep - it controls the
	 * horiz rule for then entire row.  (not easy to specify lines that
	 * span only some columns in tex or tbl. */
	if ((at = FindAttValByName(e, "ROWSEP")))	rowsep = atoi(at);
    }

    if (t->frame) {
	/* top|bottom|topbot|all|sides|none */
	if (!strcmp(t->frame, "NONE") || !strcmp(t->frame, "SIDES"))
	    frametop = framebot = 0;
	else if (!strcmp(t->frame, "TOP"))    framebot = 0;
	else if (!strcmp(t->frame, "BOTTOM")) frametop = 0;
    }

    /* tbl and tex like lower case for units. convert. */
    if (t->colwidth) {
	char *cp;
	for (cp=t->colwidth; *cp; cp++)
	    if (isupper(*cp)) *cp = tolower(*cp);
    }

    /* Now, split (space-separated) strings into vectors.  Hopefully, the
     * number of elements in each vector matches the number of columns.
     */
    t->halign_v    = Split(t->halign, &t->n_halign, S_STRDUP|S_ALVEC);
    t->model_v     = Split(t->model, &t->n_model, S_STRDUP|S_ALVEC);
    t->colwidth_v  = Split(t->colwidth, &t->n_colwidth, S_STRDUP|S_ALVEC);
    t->colweight_v = Split(t->colweight, &t->n_colweight, S_STRDUP|S_ALVEC);
    t->colsep_v    = Split(t->colsep, &t->n_colsep, S_STRDUP|S_ALVEC);

    /* Determin the _numeric_ number of columns, "nc".  The order in which we
     * check things to set nc is: NCOLS attribute, # of child element of 1st
     * row, number of tokens in the various attr lists.
     */
    if (t->ncols) t->nc = atoi(t->ncols);

    /* If ncols attribute not set, see how many children first child has.
     * I can't see how this can be non-zero (unless there are no rows, or
     * no rows have any cells).
     */
    if (!t->nc && e->necont) t->nc = e->econt[0]->necont;

    /* If ncols still not set, guess it from other attrs. Last resort. */
    if (!t->nc) {
	if (t->n_halign)   t->nc = t->n_halign;
	else if (t->n_model)     t->nc = t->n_model;
	else if (t->n_colwidth)  t->nc = t->n_colwidth;
	else if (t->n_colweight) t->nc = t->n_colweight;
	else if (t->n_colsep)    t->nc = t->n_colsep;
    }
}

/* ______________________________________________________________________ */

/*  Free the storage of info use by the table info structure.  (not the
 *  structure itself, but the strings its elements point to)
 *  Arguments:
 *	Pointer table info structure to be freed.
 */
void
FreeTabAtts(
    TableInfo	*t
)
{
    if (!t) return;
    if (t->halign_v)    free(*t->halign_v);
    if (t->model_v)     free(*t->model_v);
    if (t->colwidth_v)  free(*t->colwidth_v);
    if (t->colweight_v) free(*t->colweight_v);
    if (t->colsep_v)    free(*t->colsep_v);
}

/* ______________________________________________________________________ */
/*  Check the attributes and children of the table pointed to by e.
 *  Report problems and inconsistencies to stderr.
 *  Arguments:
 *	Pointer to element (table) under consideration.
 */

void
CheckTable(
    Element_t	*e
)
{
    int		pr_loc=0;	/* flag to say if we printed location */
    int		i, r, c;
    float	wt;
    char	*tpref = "Table Check";		/* prefix for err messages */
    char	*ncolchk =
	"Table Check: %s ('%s') has wrong number of tokens.  Expecting %d.\n";

    if (strcmp(e->gi, "TABLE")) {
	fprintf(stderr, "%s: Not pointing to a table!\n", tpref);
	return;
    }

    FreeTabAtts(&TheTab);	/* free storage, if allocated earlier */
    SetTabAtts(e, &TheTab, 1);	/* look at attributes */

    /* NCOLS attribute set? */
    if (!TheTab.ncols) {
	pr_loc++;
	fprintf(stderr, "%s: NCOLS attribute missing. Inferred as %d.\n",
		tpref, TheTab.nc);
    }

    /* HALIGN attribute set? */
    if (!TheTab.halign) {
	pr_loc++;
	fprintf(stderr, "%s: HALIGN attribute missing.\n", tpref);
    }

    /* See if the number of cells in each row matches */
    for (r=0; r<e->necont; r++) {
	if (e->econt[r]->necont != TheTab.nc) {
	    pr_loc++;
	    fprintf(stderr, "%s: NCOLS (%d) differs from actual number of cells (%d) in row %d.\n",
		tpref, TheTab.nc, e->econt[r]->necont, r);
	}
    }

    /* Check HALIGN */
    if (TheTab.halign) {
	if (TheTab.nc != TheTab.n_halign) {	/* number of tokens OK? */
	    pr_loc++;
	    fprintf(stderr, ncolchk, "HALIGN", TheTab.halign, TheTab.nc);
	}
	else {				/* values OK? */
	    for (i=0; i<TheTab.nc; i++) {
		if (*TheTab.halign_v[i] != 'c' && *TheTab.halign_v[i] != 'l' &&
			*TheTab.halign_v[i] != 'r') {
		    pr_loc++;
		    fprintf(stderr, "%s: HALIGN (%d) value wrong: %s\n",
			tpref, i, TheTab.halign_v[i]);
		}
	    }
	}
    }

    /* check COLWIDTH */
    if (TheTab.colwidth) {
	if (TheTab.nc != TheTab.n_colwidth) {	/* number of tokens OK? */
	    pr_loc++;
	    fprintf(stderr, ncolchk, "COLWIDTH", TheTab.colwidth, TheTab.nc);
	}
	else {				/* values OK? */
	    for (i=0; i<TheTab.nc; i++) {

		/* check that the units after the numbers are OK
		    we want "in", "cm".
		 */
	    }
	}
    }

    /* check COLWEIGHT */
    if (TheTab.colweight) {
	if (TheTab.nc != TheTab.n_colweight) {	/* number of tokens OK? */
	    pr_loc++;
	    fprintf(stderr, ncolchk, "COLWEIGHT", TheTab.colweight, TheTab.nc);
	}
	else {				/* values OK? */
	    for (i=0; i<TheTab.nc; i++) {	/* check that magitude is reasonable */
		wt = atof(TheTab.colweight_v[i]);
		if (wt > 50.0) {
		    pr_loc++;
		    fprintf(stderr, "%s: unreasonable COLWEIGHT value: %f.\n",
				tpref, wt);
		}
	    }
	}
    }

    /* check COLSEP */
    if (TheTab.colsep) {
	if (TheTab.nc != TheTab.n_colsep) {	/* number of tokens OK? */
	    pr_loc++;
	    fprintf(stderr, ncolchk, "COLSEP", TheTab.colsep, TheTab.nc);
	}
	else {				/* values OK? */
	    for (i=0; i<TheTab.nc; i++) {
	    }
	}
    }

    /* See if MODEL has the same number of tokens as NCOLS.  Then do model. */
    if (TheTab.model) {
	if (TheTab.nc != TheTab.n_model) {
	    pr_loc++;
	    fprintf(stderr, ncolchk, "MODEL", TheTab.model, TheTab.nc);
	}

	for (r=0; r<e->necont; r++) {
	    /* only check normal rows */
	    if (strcmp(e->econt[r]->gi, "ROW")) continue;
	    for (c=0; c<e->econt[r]->necont; c++) {
		if (!strcmp(TheTab.model_v[c], "text") ||
			!strcmp(TheTab.model_v[c], "-")) continue;
		if (e->econt[r]->econt[c]->necont &&
		    strcmp(e->econt[r]->econt[c]->econt[0]->gi, TheTab.model_v[c])) {
		    fprintf(stderr, "%s: MODEL wants %s, but cell contains %s: row %d, cell %d.\n",
			tpref, TheTab.model_v[c],
			e->econt[r]->econt[c]->econt[0]->gi, r, c);
		    pr_loc++;
		}
	    }
	}
    }

    if (pr_loc) {
	fprintf(stderr, "%s: Above problem in table located at:\n", tpref);
	PrintLocation(e, stderr);
    }
}

/* ______________________________________________________________________ */
/*  Do the "right thing" for the table spec for tbl (troff) tables.  This will
 *  generate the "center,box,tab(@)..." and the column justification stuff.
 *  Arguments:
 *	Pointer to element (table) under consideration.
 *	FILE pointer to where to write output.
 */
void
TblTable(
    Element_t	*e,
    FILE	*fp
)
{
    int		i, n;
    char	*fr;
    float	tot;
    char	*cp, wbuf[1500], **widths=0, **widths_v=0, *mp;

    FreeTabAtts(&TheTab);	/* free storage, if allocated earlier */
    SetTabAtts(e, &TheTab, 1);	/* look at attributes */

    fr = "box";				/* default framing */
    frameall = 1;
    siderules = 0;
    if (TheTab.frame) {
	if (!strcmp(TheTab.frame, "ALL")) {
	    fr = "box";
	    frametop = framebot = 0;
	}
	else {
	    fr = "";
	    frameall = 0;
	}
	if (!strcmp(TheTab.frame, "SIDES")) siderules = 1;
    }
    else frametop = framebot = 0;	/* because 'box' is default */
    fprintf(fp, "center, %s%s tab(@);\n", fr, ((*fr)?",":""));

    /* Figure out the widths, based either on "colwidth" or "colweight".
     * (we pick width over weight if both are specified). */
    if (TheTab.colwidth && TheTab.nc == TheTab.n_colwidth) {
	widths = TheTab.colwidth_v;
    }
    else if (TheTab.colweight && TheTab.nc == TheTab.n_colweight) {
	for (n=0,i=0; i<TheTab.nc; i++) n += atoi(TheTab.colweight_v[i]);
	tot = (float)n;
	cp = wbuf;
	for (i=0; i<TheTab.nc; i++) {
	    sprintf(cp, "%5.3fin", atof(TheTab.colweight_v[i])*(TEXTWIDTH/tot));
	    while (*cp) cp++;
	    *cp++ = ' ';
	}
	*cp = EOS;
	widths_v = Split(wbuf, 0, S_ALVEC);
	widths = widths_v;
    }

    /* Remember the base model in case we do spans later.  We write it
     * into a static buffer, then output it at once. */
    mp = basemodel;
    if (siderules) *mp++ = '|';
    for (i=0; i<TheTab.nc; i++) {
	/* If width specified, use it; else if halign set, use it; else left. */
	if (widths && widths[i][0] != '0' && widths[i][1] != EOS) {
	    if (i) *mp++ = ' ';
	    strcpy(mp, TheTab.halign_v[i]);
	    while (*mp) mp++;
	    *mp++ = 'w';
	    *mp++ = '(';
	    strcpy(mp, widths[i]);
	    while (*mp) mp++;
	    *mp++ = ')';
	}
	else if (TheTab.halign && TheTab.nc == TheTab.n_halign) {
	    if (i) *mp++ = ' ';
	    strcpy(mp, TheTab.halign_v[i]);
	    while (*mp) mp++;
	}
	else {
	    if (i) *mp++ = ' ';
	    *mp++ = 'l';
	}
	/* See if we want column separators. */

	if (TheTab.colsep) {
	    if ( (i+1) < TheTab.nc ) {
		if ( *TheTab.colsep_v[i] == '1' )
		    *mp++ = '|';
		if ( *TheTab.colsep_v[i] == '2') {
		    *mp++ = '|';
		    *mp++ = '|';
		}
	    }
	}
    }
    if (siderules) *mp++ = '|';
/*    *mp++ = '.';*/
/*    *mp++ = '^';*/
    *mp = EOS;
    OutputString(basemodel, fp, 1);
    OutputString(".^", fp, 1);

    if (widths_v) free(widths_v);
}

/*
 *  Arguments:
 *	Pointer to element (cell) under consideration.
 *	FILE pointer to where to write output.
 */
void
TblTableCellStart(
    Element_t	*e,
    FILE	*fp
)
{
    /* nothing to do at start of cell */
}

/*
 *  Arguments:
 *	Pointer to element (cell) under consideration.
 *	FILE pointer to where to write output.
 */
void
TblTableCellEnd(
    Element_t	*e,
    FILE	*fp
)
{
    /* do cell/col separators */
    if (e->my_eorder < (TheTab.nc-1)) {
	if (spaninfo[e->my_eorder] == SPAN_NOT ||
			spaninfo[e->my_eorder+1] != SPAN_CONT)
	    OutputString("@", fp, 1);
    }
}

/*  Look at model attribute for spanning.  If set, remember info for when
 *  doing the cells.  Called by TblTableRowStart() and TexTableRowStart().
 *  Arguments:
 *	Pointer to element (row) under consideration.
 */
int
check_for_spans(
    Element_t	*e
)
{
    char	*at;
    char	**spans;
    int		n, i, inspan;

    /* See if MODEL attr is set */
    if ((at = FindAttValByName(e, "MODEL"))) {

	/* Split into tokens, then look at each for the word "span" */
	n = TheTab.nc;
	spans = Split(at, &n, S_STRDUP|S_ALVEC);

	/* Mark columns as start-of-span, in-span, or not spanned.  Remember
	 * in at list, "spaningo".  (Span does not make sense in 1st column.)
	 */
	for (i=1,inspan=0; i<n; i++) {
	    if (StrEq(spans[i], "span") || StrEq(spans[i], "s")) {
		if (inspan == 0) spaninfo[i-1] = SPAN_START;
		spaninfo[i] = SPAN_CONT;
		inspan = 1;
	    }
	    else {
		spaninfo[i] = SPAN_NOT;
		inspan = 0;
	    }
	}
	free(*spans);				/* free string */
	free(spans);				/* free vector */
	spaninfo[TheTab.nc] = SPAN_NOT;		/* after last cell */
	return 1;
    }
    /* if model not set, mark all as not spanning */
    else
	for (i=0; i<MAXCOLS; i++) spaninfo[i] = SPAN_NOT;
    return 0;
}

/*  Output format for cell.  Called from TblTableRowStart().
 *  Arguments:
 *	Pointer to table info structure (for this row)
 *	Which cell/column we're considering
 *	Flag saying whether we're on last column
 *	Default format of col, if none is set for this row or table
 *	FILE pointer to where to write output.
 */

void
tbl_cell_fmt(
    TableInfo	*t,
    int 	i,
    int		lastcol,
    char	*def_fmt,
    FILE	*fp
)
{
    if (t->halign) OutputString(t->halign_v[i], fp, 1);
    else if (TheTab.halign) OutputString(TheTab.halign_v[i], fp, 1);
    else OutputString(def_fmt, fp, 1);

    if (!lastcol && spaninfo[i+1] != SPAN_CONT) {
	if (t->colsep) {
	    if (*t->colsep_v[i] == '1')
		OutputString("|", fp, 1);
	    if (*t->colsep_v[i] == '2')
		OutputString("||", fp, 1);
	}
	else if (TheTab.colsep) {
	    if (*TheTab.colsep_v[i] == '1')
		OutputString("|", fp, 1);
	    if (*TheTab.colsep_v[i] == '2')
		OutputString("||", fp, 1);
	}
	else OutputString("|", fp, 1);
    }
    OutputString(" ", fp, 1);
}

/*  
 *  Arguments:
 *	Pointer to element (row) under consideration.
 *	FILE pointer to where to write output.
 */

void
TblTableRowStart(
    Element_t	*e,
    FILE	*fp
)
{
    int		i, lastcol, stayhere;
    char	**basev, *cp;
    TableInfo	RowInfo;

    /* check if we're spanning, or if HALIGN set */
    stayhere = 0;
    if (check_for_spans(e)) stayhere = 1;
    SetTabAtts(e, &RowInfo, 0);
    if (RowInfo.halign) stayhere = 1;

    if (!stayhere) return;

    /* Change table layout because we have a span, or the row has HALIGN. */
    OutputString("^.T&^", fp, 1);
    basev = Split(basemodel, 0, S_ALVEC|S_STRDUP);

    for (i=0; i<TheTab.nc; i++) {

	lastcol = !(i < TheTab.nc-1);
	if (spaninfo[i] == SPAN_START) {
	    tbl_cell_fmt(&RowInfo, i, lastcol, "c ", fp);
	}
	else if (spaninfo[i] == SPAN_CONT) {
	    /* See if next col is NOT spanned, and we're not in last col */
	    OutputString("s", fp, 1);
	    if (!lastcol && spaninfo[i+1] != SPAN_CONT) {
		if (RowInfo.colsep) cp = RowInfo.colsep_v[i];
		else if (TheTab.colsep) cp = TheTab.colsep_v[i];
		else cp = "1";

		if (*cp == '1')
		    OutputString("|", fp, 1);
		if (*cp == '2')
		    OutputString("||", fp, 1);
	    }
	    OutputString(" ", fp, 1);
	}
	else
	    tbl_cell_fmt(&RowInfo, i, lastcol, "l ", fp);
    }
    OutputString("^", fp, 1);
    OutputString(basemodel, fp, 1);
    OutputString(".^", fp, 1);
    free(*basev);
    free(basev);
    FreeTabAtts(&RowInfo);
}

/*
 *  Arguments:
 *	Pointer to element (row) under consideration.
 *	FILE pointer to where to write output.
 */
void
TblTableRowEnd(
    Element_t	*e,
    FILE	*fp
)
{
    char	*at;

    /* See if we're on the last row, then if we're putting a frame
     * around the whole table.  If so, we need no bottom separator. */
    if ((e->parent->necont-1) == e->my_eorder) {
	if (frameall || framebot) return;
    }
    /* check this row's attributes */
    if ((at = FindAttValByName(e, "ROWSEP"))) {
	if (at[0] == '1') fprintf(fp, "_\n");
    }
    else if (rowsep) /* fprintf(fp, "_\n") */ ;
}

/*
 *  Arguments:
 *	Pointer to element (table) under consideration.
 *	FILE pointer to where to write output.
 */
void
TblTableTop(Element_t *e, FILE *fp)
{
    if (frametop) OutputString("^_^", fp, 1);
}

void
TblTableBottom(Element_t *e, FILE *fp)
{
    if (framebot) OutputString("^_^", fp, 1);
}

/* ______________________________________________________________________ */
/* Do the "right thing" for the table spec for TeX tables.  This will
 * generate the arg to \begin{tabular}[xxx].
 *  Arguments:
 *	Pointer to element (table) under consideration.
 *	FILE pointer to where to write output.
 */
void
TexTable(
    Element_t	*e,
    FILE	*fp
)
{
    int		i, n;
    float	tot;
    char	*cp, wbuf[1500], **widths=0, **widths_v=0;

    FreeTabAtts(&TheTab);	/* free storage, if allocated earlier */
    SetTabAtts(e, &TheTab, 1);	/* look at attributes */

    /* Figure out the widths, based either on "colwidth" or "colweight".
     * (we pick width over weight if both are specified). */
    if (TheTab.colwidth && TheTab.nc == TheTab.n_colwidth) {
	widths = TheTab.colwidth_v;
    }
    else if (TheTab.colweight && TheTab.nc == TheTab.n_colweight) {
	for (n=0,i=0; i<TheTab.nc; i++) n += atoi(TheTab.colweight_v[i]);
	tot = (float)n;
	cp = wbuf;
	for (i=0; i<TheTab.nc; i++) {
	    sprintf(cp, "%5.3fin", atof(TheTab.colweight_v[i])*(TEXTWIDTH/tot));
	    while (*cp) cp++;
	    *cp++ = ' ';
	}
	*cp = EOS;
	widths_v = Split(wbuf, 0, S_ALVEC);
	widths = widths_v;
    }
    siderules = 1;
    if (TheTab.frame)
	if (strcmp(TheTab.frame, "ALL") && strcmp(TheTab.frame, "SIDES"))
	    siderules = 0;

    if (siderules) OutputString("|", fp, 1);
    for (i=0; i<TheTab.nc; i++) {
	/* If width specified, use it; else if halign set, use it; else left. */
	if (widths && widths[i][0] != '0' && widths[i][1] != EOS) {
	    fprintf(fp, "%sp{%s}", (i?" ":""), widths[i]);
	}
	else if (TheTab.halign && TheTab.nc == TheTab.n_halign) {
	    fprintf(fp, "%s%s", (i?" ":""), TheTab.halign_v[i]);
	}
	else
	    fprintf(fp, "%sl", (i?" ":""));
	/* See if we want column separators. */
	if (TheTab.colsep) {

	    if ( (i+1) < TheTab.nc ) {
		if ( *TheTab.colsep_v[i] == '1' ) {
		    fprintf(fp, " |");
		}
		if ( *TheTab.colsep_v[i] == '2' ) {
		    fprintf(fp, " ||");
		}
	    }

	}
    }
    if (siderules) OutputString("|", fp, 1);

    if (widths_v) free(widths_v);
}

/*
 *  Arguments:
 *	Pointer to element (cell) under consideration.
 *	FILE pointer to where to write output.
 */
void
TexTableCellStart(
    Element_t	*e,
    FILE	*fp
)
{
    int		n, i;
    char	buf[50], *at;

    if (spaninfo[e->my_eorder] == SPAN_START) {
	for (i=e->my_eorder+1,n=1; ; i++) {
	    if (spaninfo[i] == SPAN_CONT) n++;
	    else break;
	}
	sprintf(buf, "\\\\multicolumn{%d}{%sc%s}", n,
		(siderules?"|":""), (siderules?"|":""));
	OutputString(buf, fp, 1);
    }
#ifdef New
    if ((at = FindAttValByName(e->parent, "HALIGN"))) {
	/* no span, but user wants to change the alignment */
	h_v = Split(wbuf, 0, S_ALVEC|S_STRDUP);
	OutputString("\\\\multicolumn{1}{%sc%s}", n,
		fp, 1);
    }
#endif

    if (spaninfo[e->my_eorder] != SPAN_CONT) OutputString("{", fp, 1);
}

/*
 *  Arguments:
 *	Pointer to element (cell) under consideration.
 *	FILE pointer to where to write output.
 */
void
TexTableCellEnd(
    Element_t	*e,
    FILE	*fp
)
{
    if (spaninfo[e->my_eorder] != SPAN_CONT) OutputString("} ", fp, 1);

    /* do cell/col separators */
    if (e->my_eorder < (TheTab.nc-1)) {
	if (spaninfo[e->my_eorder] == SPAN_NOT ||
			spaninfo[e->my_eorder+1] != SPAN_CONT)
	    OutputString("& ", fp, 1);
    }
}

/*  Look at model for spanning.  If set, remember it for when doing the cells.
 *  Arguments:
 *	Pointer to element (row) under consideration.
 *	FILE pointer to where to write output.
 */
void
TexTableRowStart(
    Element_t	*e,
    FILE	*fp
)
{
    check_for_spans(e);
}

/*
 *  Arguments:
 *	Pointer to element (row) under consideration.
 *	FILE pointer to where to write output.
 */
void
TexTableRowEnd(
    Element_t	*e,
    FILE	*fp
)
{
    char	*at;

    /* check this row's attributes */
    if ((at = FindAttValByName(e, "ROWSEP"))) {
	if (at[0] == '1') OutputString("\\\\\\\\[2mm] \\\\hline ", fp, 1);
    }
    else if (rowsep) OutputString("\\\\\\\\ ", fp, 1);
    else 
        OutputString("\\\\\\\\ ", fp, 1);

}

/*
 *  Arguments:
 *	Pointer to element (table) under consideration.
 *	FILE pointer to where to write output.
 */
void
TexTableTop(Element_t *e, FILE *fp)
{
    if (frametop) OutputString("\\\\hline", fp, 1);
}

void
TexTableBottom(Element_t *e, FILE *fp)
{
    if (framebot) OutputString("\\\\hline", fp, 1);
}

/* ______________________________________________________________________ */

