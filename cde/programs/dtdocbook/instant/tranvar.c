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
 *  instant - a program to manipulate SGML instances.
 *
 *  This module is for handling "special variables".  These act a lot like
 *  procedure calls
 * ________________________________________________________________________
 */

#ifndef lint
static char *RCSid =
  "$XConsortium: tranvar.c /main/7 1996/08/08 14:42:09 cde-hp $";
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

static char	**idrefs;		/* list of IDREF att names to follow */
static char	*def_idrefs[] = { "LINKEND", "LINKENDS", "IDREF", 0 };

/* forward references */
void	ChaseIDRefs(Element_t *, char *, int, FILE *);
void	Find(Element_t *, int, char **, FILE *);
void	GetIDREFnames();

static	void	OutputCDATA(Content_t *cp, void *client_data);
typedef struct _cdata_info {
    int   track_pos;
    FILE *fp;
} OutputCDATA_info_t;


/* ______________________________________________________________________ */
/*  Handle "special" variable - read file, run command, do action, etc.
 *  Arguments:
 *	Name of special variable to expand.
 *	Pointer to element under consideration.
 *	FILE pointer to where to write output.
 *	Flag saying whether to track the character position we're on
 *	  (passed to OutputString).
 */

ContParse_t
ExpandSpecialVar(
    char	*name,
    Element_t	*e,
    FILE	*fp,
    int		track_pos
)
{
    FILE	*infile;
    char	buf[LINESIZE], tempbuf[LINESIZE], *cp, *atval, letter;
    char	**tok;
    int		ntok, n, i, action, action1, number;
    Element_t	*ep;
    Trans_t	*t, *tt;
    static char	*s_A, *s_C;

    /* Run a command.
     * Format: _! command args ... */
    if (*name == '!') {
	name++;
	if ((infile = popen(name, "r"))) {
	    while (fgets(buf, LINESIZE, infile)) FPuts(buf, fp);
	    pclose(infile);
	    FFlush(fp);
	}
	else {
	    fprintf(stderr, "Could not start program '%s': %s",
		name, strerror(errno));
	}
	return CONT_CONTINUE;
    }

    /* See if caller wants one of the tokens from _eachatt or _eachcon.
     * If so, output it and return.  (Yes, I admit that this is a hack.)
     */
    if (*name == 'A' && name[1] == EOS && s_A) {
	OutputString(s_A, fp, track_pos);
	return CONT_CONTINUE;
    }
    if (*name == 'C' && name[1] == EOS && s_C) {
	OutputString(s_C, fp, track_pos);
	return CONT_CONTINUE;
    }

    ntok = 0;
    tok = Split(name, &ntok, 0);

    /* Include another file.
     * Format: _include filename */
    if (StrEq(tok[0], "include")) {
	name = tok[1];
	if (ntok > 1 ) {
	    if ((infile=OpenFile(name)) == NULL) {
		sprintf(buf, "Can not open included file '%s'", name);
		perror(buf);
		return CONT_CONTINUE;
	    }
	    while (fgets(buf, LINESIZE, infile)) FPuts(buf, fp);
	    fclose(infile);
	}
	else fprintf(stderr, "No file name specified for include\n");
	return CONT_CONTINUE;
    }

    /* Print location (nearest title, line no, path).
     * Format: _location */
    else if (StrEq(tok[0], "location")) {
	PrintLocation(e, fp);
    }

    /* Print path to this element.
     * Format: _path */
    else if (StrEq(tok[0], "path")) {
	(void)FindElementPath(e, buf);
	OutputString(buf, fp, track_pos);
    }

    /* Print name of this element (gi).
     * Format: _gi [M|L|U] */
    else if (StrEq(tok[0], "gi")) {
	strcpy(buf, e->gi);
	if (ntok >= 2) {
	    if (*tok[1] == 'L' || *tok[1] == 'l' ||
		*tok[1] == 'M' || *tok[1] == 'm') {
		for (cp=buf; *cp; cp++)
		    if (isupper(*cp)) *cp = tolower(*cp);
	    }
	    if (*tok[1] == 'M' || *tok[1] == 'm')
		if (islower(buf[0])) buf[0] = toupper(buf[0]);
	}
	OutputString(buf, fp, track_pos);
    }

    /* Print name of the parent of this element (parent).
     * If a parent number is given, go that far up the parent tree
     * (e.g., "_parent 1 U" returns the parent in upper case
     *        "_parent 2 L" returns the grandparent in lower case
     *        "_parent 0" is equivalent to "_gi"
     *        "_parent" is equivalent to "_parent 1")
     * Format: _parent [<number>] [M|L|U] */
    else if (StrEq(tok[0], "parent")) {
	number = 1;
	letter = 'U';
	if (ntok >= 2) {
	    if (isdigit(*tok[1])) {
		number = atoi(tok[1]);
		if (ntok >= 3) {
		    letter = *tok[2];
		}
	    } else {
		letter = *tok[1];
	    }
	}

	ep = e;
	while (--number >= 0) {
	    if (ep) ep = ep->parent;
	}
	if (ep) {
	    strcpy(buf, ep->gi);
	} else {
	    *buf = 0;
	}

	if (letter == 'L' || letter == 'l' ||
	    letter == 'M' || letter == 'm') {
	    for (cp=buf; *cp; cp++)
		if (isupper(*cp)) *cp = tolower(*cp);
	}
	if (letter == 'M' || letter == 'm')
	    if (islower(buf[0])) buf[0] = toupper(buf[0]);

	OutputString(buf, fp, track_pos);
    }

    /* Print filename of this element's associated external entity.
     * Format: _filename */
    else if (StrEq(tok[0], "filename")) {
	if (!e->entity) {
	    fprintf(stderr, "Expected ext entity (internal error? bug?):\n");
	    PrintLocation(e, stderr);
	    return CONT_CONTINUE;
	}
	if (!e->entity->fname) {
	    fprintf(stderr, "Expected filename ");
	    if (e->entity->sysid) {
		fprintf(stderr,
			"(could not find \"%s\"):\n",
			e->entity->sysid);
	    } else if (e->entity->pubid) {
		fprintf(stderr,
			"(could not resolve \"%s\"):\n",
			e->entity->pubid);
	    } else {
		fprintf(stderr, "(internal error? bug?):\n");
	    }
	    PrintLocation(e, stderr);
	    return CONT_CONTINUE;
	}
	OutputString(e->entity->fname, fp, track_pos);
    }

    /* Value of parent's attribute, by attr name.
     * Format: _pattr attname */
    else if (StrEq(tok[0], "pattr")) {
	ep = e->parent;
	if (!ep) {
	    fprintf(stderr, "Element does not have a parent:\n");
	    PrintLocation(ep, stderr);
	    return CONT_CONTINUE;
	}
	if ((atval = FindAttValByName(ep, tok[1]))) {
	    OutputString(atval, fp, track_pos);
	}
    }

    /* Use an action, given transpec's SID.
     * Format: _action action */
    else if (StrEq(tok[0], "action")) {
	action = atoi(tok[1]);
	if (action) TranByAction(e, action, fp);
    }

    /* Number of child elements of this element.
     * Format: _nchild */
    else if (StrEq(tok[0], "nchild")) {
	if (ntok > 1) {
	    for (n=0,i=0; i<e->necont; i++)
		if (StrEq(e->econt[i]->gi, tok[1])) n++;
	}
	else n = e->necont;
	sprintf(buf, "%d", n);
	OutputString(buf, fp, track_pos);
    }

    /* number of 1st child's child elements (grandchildren from first child).
     * Format: _n1gchild */
    else if (StrEq(tok[0], "n1gchild")) {
	if (e->necont) {
	    sprintf(buf, "%d", e->econt[0]->necont);
	    OutputString(buf, fp, track_pos);
	}
    }

    /* Chase this element's pointers until we hit the named GI.
     * Do the action if it matches.
     * Format: _chasetogi gi action */
    else if (StrEq(tok[0], "chasetogi")) {
	if (ntok < 3) {
	    fprintf(stderr, "Error: Not enough args for _chasetogi.\n");
	    return CONT_CONTINUE;
	}
	action = atoi(tok[2]);
	if (action) ChaseIDRefs(e, tok[1], action, fp);
    }

    /* Follow link to element pointed to, then do action.
     * Format: _followlink [attname] action. */
    else if (StrEq(tok[0], "followlink")) {
	char **s;
	if (ntok > 2) {
	    action = atoi(tok[2]);
	    if ((atval = FindAttValByName(e, tok[1]))) {
		if ((ep = FindElemByID(atval))) {
		    TranByAction(ep, action, fp);
		    return CONT_CONTINUE;
		}
	    }
	    else fprintf(stderr, "Error: Did not find attr: %s.\n", tok[1]);
	    return CONT_CONTINUE;
	}
	else action = atoi(tok[1]);
	GetIDREFnames();
	for (s=idrefs; *s; s++) {
	    /* is this IDREF attr set? */
	    if ((atval = FindAttValByName(e, *s))) {
		ntok = 0;
		tok = Split(atval, &ntok, S_STRDUP);
		/* we'll follow the first one... */
		if ((ep = FindElemByID(tok[0]))) {
		    TranByAction(ep, action, fp);
		    return CONT_CONTINUE;
		}
		else fprintf(stderr, "Error: Can not find elem for ID: %s.\n",
			tok[0]);
	    }
	}
	fprintf(stderr, "Error: Element does not have IDREF attribute set:\n");
	PrintLocation(e, stderr);
	return CONT_CONTINUE;
    }

    /* Starting at this element, decend tree (in-order), finding GI.
     * Do the action if it matches.
     * Format: _find args ... */
    else if (StrEq(tok[0], "find")) {
	Find(e, ntok, tok, fp);
    }

    /* Starting at this element's parent, decend tree (in-order), finding GI.
     * Do the action if it matches.
     * Format: _pfind args ... */
    else if (StrEq(tok[0], "pfind")) {
	Find(e->parent ? e->parent : e, ntok, tok, fp);
    }

    /* Content is supposed to be a list of IDREFs.  Follow each, doing action.
     * If 2 actions are specified, use 1st for the 1st ID, 2nd for the rest.
     * Format: _namelist action [action2] */
    else if (StrEq(tok[0], "namelist")) {
	int id;
	action1 = atoi(tok[1]);
	if (ntok > 2) action = atoi(tok[2]);
	else action = action1;
	for (i=0; i<e->ndcont; i++) {
	    n = 0;
	    tok = Split(e->dcont[i], &n, S_STRDUP);
	    for (id=0; id<n; id++) {
		if (fold_case)
		    for (cp=tok[id]; *cp; cp++)
			if (islower(*cp)) *cp = toupper(*cp);
		if ((e = FindElemByID(tok[id]))) {
		    if (id) TranByAction(e, action, fp);
		    else TranByAction(e, action1, fp);	/* first one */
		}
		else fprintf(stderr, "Error: Can not find ID: %s.\n", tok[id]);
	    }
	}
    }

    /* For each word in the element's content, do action.
     * Format: _eachcon action [action] */
    else if (StrEq(tok[0], "eachcon")) {
	int id;
	action1 = atoi(tok[1]);
	if (ntok > 3) action = atoi(tok[2]);
	else action = action1;
	for (i=0; i<e->ndcont; i++) {
	    n = 0;
	    tok = Split(e->dcont[i], &n, S_STRDUP|S_ALVEC);
	    for (id=0; id<n; id++) {
		s_C = tok[id];
		TranByAction(e, action, fp);
	    }
	    free(*tok);
	}
    }
    /* For each word in the given attribute's value, do action.
     * Format: _eachatt attname action [action] */
    else if (StrEq(tok[0], "eachatt")) {
	int id;
	action1 = atoi(tok[2]);
	if (ntok > 3) action = atoi(tok[3]);
	else action = action1;
	if ((atval = FindAttValByName(e, tok[1]))) {
	    n = 0;
	    tok = Split(atval, &n, S_STRDUP|S_ALVEC);
	    for (id=0; id<n; id++) {
		s_A = tok[id];
		if (id) TranByAction(e, action, fp);
		else TranByAction(e, action1, fp);	/* first one */
	    }
	    free(*tok);
	}
    }

    /* Do action on this element if element has [relationship] with gi.
     * Format: _relation relationship gi action [action] */
    else if (StrEq(tok[0], "relation")) {
	if (ntok >= 4) {
	    if (!CheckRelation(e, tok[1], tok[2], tok[3], fp, RA_Current)) {
		/* action not done, see if alt action specified */
		if (ntok >= 5)
		    TranByAction(e, atoi(tok[4]), fp);
	    }
	}
    }

    /* Do action on followed element if element has [relationship] with gi.
     * Format: _followrel relationship gi action */
    else if (StrEq(tok[0], "followrel")) {
	if (ntok >= 4)
	    (void)CheckRelation(e, tok[1], tok[2], tok[3], fp, RA_Related);
    }

    /* Find element with matching ID and do action.  If action not specified,
     * choose the right one appropriate for its context.
     * Format: _id id [action] */
    else if (StrEq(tok[0], "id")) {
	if (ntok > 2) action = atoi(tok[2]);
	else action = 0;
	if ((ep = FindElemByID(tok[1]))) {
	    if (action) TranByAction(ep, action, fp);
	    else {
		t = FindTrans(ep);
		TransElement(ep, fp, t);
	    }
	}
    }

    /* Set variable to value.
     * Format: _set name value */
    else if (StrEq(tok[0], "set")) {
	SetMappingNV(Variables, tok[1], tok[2]);
    }

    /* Do action if variable is set, optionally to value.
     * If not set, do nothing.
     * Format: _isset varname [value] action */
    else if (StrEq(tok[0], "isset")) {
	if ((cp = FindMappingVal(Variables, tok[1]))) {
	    if (ntok == 3) TranByAction(e, atoi(tok[2]), fp);
	    else if (ntok > 3 && !strcmp(cp, tok[2]))
		TranByAction(e, atoi(tok[3]), fp);
	}
    }

    /* If variable is unset or not set to optional value, return an
     * indication that the parsing of this specification should
     * continue; otherwise, return an indication that the parse should
     * quit. */
    else if (StrEq(tok[0], "break")) {
	if ((cp = FindMappingVal(Variables, tok[1]))) {
	    if ((ntok <= 2) || (strcmp(cp, tok[2]) == 0)) return CONT_BREAK;
	}
	return CONT_CONTINUE;
    }

    /* Insert a node into the tree at start/end, pointing to action to perform.
     * Format: _insertnode S|E action */
    else if (StrEq(tok[0], "insertnode")) {
	action = atoi(tok[2]);
	if (*tok[1] == 'S') e->gen_trans[0] = action;
	else if (*tok[1] == 'E') e->gen_trans[1] = action;
    }

    /* Do an OSF DTD table spec for TeX or troff.  Looks through attributes
     * and determines what to output. "check" means to check consistency,
     * and print error messages.
     * This is (hopefully) the only hard-coded part of the program.
     * Format: _osftable [tex|roff|check] [cell|top|bottom|rowend] */
    else if (StrEq(tok[0], "osftable")) {
	OSFtable(e, fp, tok, ntok);
    }

    /* Do action if element's attr is set, optionally to value.
     * If not set, do nothing.
     * Format: _attval att [value] action */
    else if (StrEq(tok[0], "attval")) {
	if ((atval = FindAttValByName(e, tok[1]))) {
	    if (ntok == 3) TranByAction(e, atoi(tok[2]), fp);
	    else if (ntok > 3 && !strcmp(atval, tok[2]))
		TranByAction(e, atoi(tok[3]), fp);
	}
    }
    /* Same thing, but look at parent */
    else if (StrEq(tok[0], "pattval")) {
	if ((atval = FindAttValByName(e->parent, tok[1]))) {
	    if (ntok == 3) {
		TranByAction(e, atoi(tok[2]), fp);
	    }
	    if (ntok > 3 && !strcmp(atval, tok[2]))
		TranByAction(e, atoi(tok[3]), fp);
	}
    }

    /* Print each attribute and value for the current element, hopefully
     * in a legal sgml form: <elem-name att1="value1" att2="value2:> .
     * Format: _allatts */
    else if (StrEq(tok[0], "allatts")) {
	for (i=0; i<e->natts; i++) {
	    if (i != 0) Putc(' ', fp);
	    FPuts(e->atts[i].name, fp);
	    FPuts("=\"", fp);
	    FPuts(e->atts[i].sval, fp);
	    Putc('"', fp);
	}
    }

    /* Print the element's input filename, and optionally, the line number.
     * Format: _infile [line] */
    else if (StrEq(tok[0], "infile")) {
	if (e->infile) {
	    if (ntok > 1 && !strcmp(tok[1], "root")) {
		strcpy(buf, e->infile);
		if ((cp = strrchr(buf, '.'))) *cp = EOS;
		FPuts(buf, fp);
	    }
	    else {
		FPuts(e->infile, fp);
		if (ntok > 1 && !strcmp(tok[1], "line"))
		    {
		    sprintf(tempbuf, " %d", e->lineno);
		    FPuts(tempbuf, fp);
		    }
	    }
	    return CONT_CONTINUE;
	}
	else FPuts("input-file??", fp);
    }

    /* Get value of an environement variable */
    else if (StrEq(tok[0], "env")) {
	if (ntok > 1 && (cp = getenv(tok[1]))) {
	    OutputString(cp, fp, track_pos);
	}
    }

    /* Get the cdata content of the node (and descendents) */
    else if (StrEq(tok[0], "cdata")) {
	OutputCDATA_info_t client_data;

	client_data.track_pos = track_pos;
	client_data.fp        = fp;
	DescendTree(e, 0, 0, OutputCDATA, (void *) &client_data);
    }

    /* Something unknown */
    else {
	fprintf(stderr, "Unknown special variable: %s\n", tok[0]);
	tt = e->trans;
	if (tt && tt->lineno)
	    fprintf(stderr, "Used in transpec, line %d\n", tt->lineno);
    }
    return CONT_CONTINUE;
}

/* ______________________________________________________________________ */
/*  A routine to pass to DescendTree().  This routine will be called
 *  on each data node in the tree from the current element (e) down -
 *  putting any cdata on the output stream.
 *  Arguments:
 *	Pointer to content of the node
 *      Client data - holds fp and track_pos from ExpandSpecialVariable()
 */
static void
OutputCDATA(Content_t *cp, void *client_data)
{
OutputCDATA_info_t *pInfo = (OutputCDATA_info_t *) client_data;

if (cp->type == CMD_DATA)
    OutputString(cp->ch.data, pInfo->fp, pInfo->track_pos);
}

/* ______________________________________________________________________ */
/*  Chase IDs until we find an element whose GI matches.  We also check
 *  child element names, not just the names of elements directly pointed
 *  at (by IDREF attributes).
 */

void
GetIDREFnames(void)
{
    char	*cp;

    if (!idrefs) {
	/* did user or transpec set the variable */
	if ((cp = FindMappingVal(Variables, "link_atts")))
	    idrefs = Split(cp, 0, S_STRDUP|S_ALVEC);
	else
	    idrefs = def_idrefs;
    }
}

/* ______________________________________________________________________ */
/*  Chase ID references - follow IDREF(s) attributes until we find
 *  a GI named 'gi', then perform given action on that GI.
 *  Arguments:
 *	Pointer to element under consideration.
 *	Name of GI we're looking for.
 *	Spec ID of action to take.
 *	FILE pointer to where to write output.
 */
void
ChaseIDRefs(
    Element_t	*e,
    char	*gi,
    int		action,
    FILE	*fp
)
{
    int		ntok, i, ei;
    char	**tok, **s, *atval;

    /* First, see if we got what we came for with this element */
    if (StrEq(e->gi, gi)) {
	TranByAction(e, action, fp);
	return;
    }
    GetIDREFnames();

    /* loop for each attribute of type IDREF(s) */
    for (s=idrefs; *s; s++) {
	/* is this IDREF attr set? */
	if ((atval = FindAttValByName(e, *s))) {
	    ntok = 0;
	    tok = Split(atval, &ntok, 0);
	    for (i=0; i<ntok; i++) {
		/* get element pointed to */
		if ((e = FindElemByID(tok[i]))) {
		    /* OK, we found a matching GI name */
		    if (StrEq(e->gi, gi)) {
			/* process using named action */
			TranByAction(e, action, fp);
			return;
		    }
		    else {
			/* this elem itself did not match, try its children */
			for (ei=0; ei<e->necont; ei++) {
			    if (StrEq(e->econt[ei]->gi, gi)) {
				TranByAction(e->econt[ei], action, fp);
				return;
			    }
			}
			/* try this elem's IDREF attributes */
			ChaseIDRefs(e, gi, action, fp);
			return;
		    }
		}
		else {
		    /* should not happen, since parser checks ID/IDREFs */
		    fprintf(stderr, "Error: Could not find ID %s\n", atval);
		    return;
		}
	    }
	}
    }
    /* if the pointers didn't lead to the GI, give error */
    if (!s)
	fprintf(stderr, "Error: Could not find '%s'\n", gi);
}

/* ______________________________________________________________________ */

/* state to pass to recursive routines - so we don't have to use
 * global variables. */
typedef struct {
    char	*gi;
    char	*gi2;
    int		action;
    Element_t	*elem;
    FILE	*fp;
} Descent_t;

static void
tr_find_gi(
    Element_t	*e,
    Descent_t	*ds
)
{
    if (StrEq(ds->gi, e->gi))
	if (ds->action) TranByAction(e, ds->action, ds->fp);
}

static void
tr_find_gipar(
    Element_t	*e,
    Descent_t	*ds
)
{
    if (StrEq(ds->gi, e->gi) && e->parent &&
		StrEq(ds->gi2, e->parent->gi))
	if (ds->action) TranByAction(e, ds->action, ds->fp);
}

static void
tr_find_attr(
    Element_t	*e,
    Descent_t	*ds
)
{
    char	*atval;
    if ((atval = FindAttValByName(e, ds->gi)) && StrEq(ds->gi2, atval))
	TranByAction(e, ds->action, ds->fp);
}

static void
tr_find_parent(
    Element_t	*e,
    Descent_t	*ds
)
{
    if (QRelation(e, ds->gi, REL_Parent)) {
	if (ds->action) TranByAction(e, ds->action, ds->fp);
    }
}

/* ______________________________________________________________________ */
/*  Descend tree, finding elements that match criteria, then perform
 *  given action.
 *  Arguments:
 *	Pointer to element under consideration.
 *	Number of tokens in special variable.
 *	Vector of tokens in special variable (eg, "find" "gi" "TITLE")
 *	FILE pointer to where to write output.
 */
void
Find(
    Element_t	*e,
    int		ac,
    char	**av,
    FILE	*fp
)
{
    Descent_t	DS;		/* state passed to recursive routine */

    memset(&DS, 0, sizeof(Descent_t));
    DS.elem = e;
    DS.fp   = fp;

    /* see if we should start at the top of instance tree */
    if (StrEq(av[1], "top")) {
	av++;
	ac--;
	e = DocTree;
    }
    if (ac < 4) {
	fprintf(stderr, "Bad '_find' specification - missing args.\n");
	return;
    }
    /* Find elem whose GI is av[2] */
    if (StrEq(av[1], "gi")) {
	DS.gi     = av[2];
	DS.action = atoi(av[3]);
	DescendTree(e, tr_find_gi, 0, 0, &DS);
    }
    /* Find elem whose GI is av[2] and whose parent GI is av[3] */
    else if (StrEq(av[1], "gi-parent")) {
	DS.gi     = av[2];
	DS.gi2    = av[3];
	DS.action = atoi(av[4]);
	DescendTree(e, tr_find_gipar, 0, 0, &DS);
    }
    /* Find elem whose parent GI is av[2] */
    else if (StrEq(av[0], "parent")) {
	DS.gi     = av[2];
	DS.action = atoi(av[3]);
	DescendTree(e, tr_find_parent, 0, 0, &DS);
    }
    /* Find elem whose attribute av[2] has value av[3] */
    else if (StrEq(av[0], "attr")) {
	DS.gi     = av[2];
	DS.gi2    = av[3];
	DS.action = atoi(av[4]);
	DescendTree(e, tr_find_attr, 0, 0, &DS);
    }
}

/* ______________________________________________________________________ */

