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
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: DtSearchFormatObjdate
 *		DtSearchFreeResults
 *		DtSearchGetMaxResults
 *		DtSearchMergeResults
 *		DtSearchSetMaxResults
 *		DtSearchSortResults
 *		DtSearchValidDateString
 *		aa_check_initialization
 *		aa_envars
 *		aa_get_passwd
 *		ditto_pop
 *		ditto_sort
 *		ditto_split
 *		merge_by_date
 *		merge_by_prox
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/**************************** DTSRJOINT.C *************************
 * $XConsortium: dtsrjoint.c /main/5 1996/06/23 16:47:42 cde-ibm $
 * February 1995.
 * Split off of ausapi.c when I converted to client/server model.
 * Ausclt.c is virtual copy of ausapi functions in RPC client stub.
 * This module contains ausapi utilities that are common to both
 * sides (ie do not result in RPC call to server or require
 * access to real usrblk).  My original version of this program
 * was called "aacom" for "ausapi common" but I renamed it to avoid
 * conflict with aacomm, the rpc/sockets communications module.
 *
 * $Log$
 * Revision 2.5  1996/04/10  19:52:16  miker
 * Added locale independent ISDIGIT macro.  Changed function name
 * from aa_merge_dittolists to DtSearchMergeResults.
 *
 * Revision 2.4  1995/12/27  16:11:54  miker
 * Remove save_init_dbxxx globals for DtSearchReinit().
 *
 * Revision 2.3  1995/10/25  22:16:09  miker
 * Renamed from aajoint.c.  Added prolog.
 *
 * Log: aajoint.c,v
 * Revision 2.2  1995/10/02  20:14:39  miker
 * DtSearch function name changes.
 * Added const to strdup() prototype for greater portability.
 * Changed to more uniform output format in DtSearchFormatObjdate().
 *
 * Revision 2.1  1995/09/22  15:18:08  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/08/31  21:31:48  miker
 * Renames and minor changes for DtSearch.
 *
 * Revision 1.2  1995/06/22  18:01:09  miker
 * 2.1.6: RPC version no longer user specifiable.  Removed aa_versnum etc.
 * Added aa_sort_dittolist, date sorting of hitlists by any client.
 */
#include "SearchE.h"
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifndef strdup
extern char    *strdup (const char *s);
#endif

#define PROGNAME	"DTSRJOINT"
#define ISDIGIT(c) ((ascii_charmap [(unsigned int) (c)] & NUMERAL) != 0)

/*------------- GLOBALS VISIBLE TO CALLER -------------------*/
/* see also globals.c */
char          **ausapi_dbnamesv = NULL;	/* array of database names */
int             ausapi_dbnamesc = 0;	/* size of dbnames array */
int             aa_maxhits = DtSrMAXHITS;	/* num hits retnd after
						 * srch */

/*------------------- PRIVATE GLOBALS ---------------------*/
int             aa_is_initialized = FALSE;
int             aa_getnews_flag = 1;
long            save_init_switches = 0L;
int             ditsort_type = 0;


/************************************************/
/*						*/
/*	       aa_check_initialization		*/
/*						*/
/************************************************/
/* Confirms ausapi_init() was first function called. */
void            aa_check_initialization (void)
{
    if (aa_is_initialized)
	return;
    fprintf (aa_stderr,
	catgets (dtsearch_catd, 2, 37,
	    "%s First API function call must be DtSearchInit().\n"),
	PROGNAME"37");
    DtSearchExit (37);
}  /* aa_check_initialization() */


/************************************************/
/*						*/
/*	     DtSearchValidDateString		*/
/*						*/
/************************************************/
/* Subroutine of aa_both_valid_dates(), called once for each date string,
 * or can be called from user interface to validate a date string.
 * Converts passed date string into valid AusText DtSrObjdate.
 * Date string format:	"[yy]yy [mm [dd]]",
 * 1, 2, or 3 numeric tokens separated by one
 * or more nonnumeric chars (whitespace, slashes, etc).
 * The first token is a complete year number integer yyyy
 * in the range 1900 <= yyyy <= 5995.  If the first token contains
 * less than 4 digits it is presumed to be the number of years
 * since 1900.  The month number is the second token mm,
 * in the range 1 - 12, and the third token dd is the day, 1 - 31.
 * If only two tokens are in the string they are presumed to
 * be year and month; the day is presumed to be to 1.
 * If only one token is in the string it is presumed to
 * be the year; the month is presumed to be 1 and the day
 * is presumed to be 1.  NULL and empty strings are always
 * valid.  They mean no date exclusion in the search
 * and this function returns objdate 0 for them.
 * Returns objdate on successful parse and conversion.
 * Returns -1 and err msg if date string is invalid.
 */
DtSrObjdate     DtSearchValidDateString (char *datestr)
{
    DtSrObjdate     objdate = 0L;
    char            parsebuf[64];
    char           *startp, *endp;
    int             yy, mm, dd;
    int             stop_parse = FALSE;
    char            msgbuf[256];

    aa_check_initialization();
    if (datestr == NULL)	/* null string is valid */
	return 0L;
    if (datestr[0] == 0)	/* empty string is valid */
	return 0L;

    strncpy (parsebuf, datestr, sizeof (parsebuf));
    parsebuf[sizeof (parsebuf) - 1] = 0;

    /* yyyy */
    for (startp = parsebuf; *startp != 0 && !ISDIGIT(*startp); startp++);
    if (*startp == 0) {	/* no numeric digits in string */
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"269 No numeric digits in string\n");
#endif
INVALID_DATESTR:
	sprintf (msgbuf,
	    catgets (dtsearch_catd, 2, 115,
		"%s '%s' is invalid or incomplete date string.\n"
		"The correct format is '[yy]yy [mm [dd]]'."),
	    PROGNAME"115", datestr);
	DtSearchAddMessage (msgbuf);
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"278 Returning objdate -1\n");
	fflush (aa_stderr);
#endif
	return -1L;
    }
    for (endp = startp; ISDIGIT(*endp); endp++);
    if (*endp == 0) {	/* mm and dd both missing */
	mm = 1;
	dd = 1;
	stop_parse = TRUE;
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"286 mm and dd both missing\n");
#endif
    }
    *endp = 0;
    yy = atoi (startp);
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"293 yystr='%s' yy=%d\n", startp, yy);
#endif
    if (strlen (startp) < 4)
	yy += 1900;
    if (yy < 1900 || yy > 5995)
	goto INVALID_DATESTR;
    if (stop_parse)
	goto DATESTR_OK;

    /* mm */
    for (startp = ++endp; *startp != 0 && !ISDIGIT(*startp); startp++);
    if (*startp == 0) {	/* no mm in string */
	mm = 1;
	dd = 1;
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"309 No mm in string\n");
#endif
	goto DATESTR_OK;
    }
    for (endp = startp; ISDIGIT(*endp); endp++);
    if (*endp == 0) {	/* no dd in string */
	dd = 1;
	stop_parse = TRUE;
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"318 No dd in string\n");
#endif
    }
    *endp = 0;
    mm = atoi (startp);
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"293 mmstr='%s' mm=%d\n", startp, mm);
#endif
    if (mm < 1 || mm > 12)
	goto INVALID_DATESTR;
    if (stop_parse)
	goto DATESTR_OK;

    /* dd */
    for (startp = ++endp; *startp != 0 && !ISDIGIT(*startp); startp++);
    if (*startp == 0) {	/* no dd in string */
	dd = 1;
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"336 No dd in string\n");
#endif
	goto DATESTR_OK;
    }
    for (endp = startp; ISDIGIT(*endp); endp++);
    *endp = 0;
    dd = atoi (startp);
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr, PROGNAME"344 ddstr='%s' dd=%d\n", startp, dd);
#endif
    if (dd < 1 || dd > 31)
	goto INVALID_DATESTR;

DATESTR_OK:
    objdate = (yy - 1900) << 20 | (mm-1) << 16 | dd << 11;
#ifdef DEBUG_VALDATE
	fprintf (aa_stderr,
	    PROGNAME"352 Returning objdate %08lx, %d/%d/%d, %s\n",
	    objdate, yy, mm, dd, objdate2fzkstr(objdate));
	fflush (aa_stderr);
#endif
    return objdate;
}  /* DtSearchValidDateString() */


/************************************************/
/*						*/
/*	      DtSearchFormatObjdate		*/
/*						*/
/************************************************/
/* Converts objdate in hitlist to displayable string */
char	*DtSearchFormatObjdate (DtSrObjdate objdate)
{
    static char     datestr_buf[24];
    if (objdate == 0L)
	return "(undated)";
    strftime (datestr_buf, sizeof(datestr_buf), "%Y.%m.%d",
	objdate2tm (objdate));
    return datestr_buf;
}  /* DtSearchFormatObjdate() */


/************************************************/
/*						*/
/*		DtSearchGetMaxResults		*/
/*		DtSearchSetMaxResults		*/
/*						*/
/************************************************/
int	DtSearchGetMaxResults (void)
{ return aa_maxhits; }

void	DtSearchSetMaxResults (int newmax)
{ aa_maxhits = newmax; }


/************************************************/
/*						*/
/*		DtSearchFreeResults		*/
/*						*/
/************************************************/
/* Frees storage allocated for the dittolist created
 * by DtSearchQuery(), and sets dittolist pointer to NULL.
 * Always returns DtSrOK.
 */
int             DtSearchFreeResults (DtSrResult ** dittolist)
{
    free_llist ((LLIST **) dittolist);
    return DtSrOK;
}  /* DtSearchFreeResults() */


/************************************************/
/*						*/
/*		DtSearchMergeResults		*/
/*						*/
/************************************************/
/* Merges one dittolist into another using proximity
 * for sort order.  Sets 'src' dittolist pointer to NULL.
 * Does not change dittocount of either list, user must do that.
 * Returns DtSrOK on successful merge,
 * else returns DtSrERROR for programming error.
 * Formerly named aa_merge_dittolists.
 */
int	DtSearchMergeResults (DtSrResult **targ_list, DtSrResult **src_list)
{
    DtSrResult     *targ, *src, *nextsrc, **prevtargl;

    if (src_list == NULL || targ_list == NULL)
	return DtSrERROR;

    /* If there's no 'src' list, return with no changes.
     * In other words, merge was successful before we started.
     */
    if (*src_list == NULL)
	return DtSrOK;

    /* At this point we know there is a src_list.
     * If there's no targ_list, just swap the pointers.
     */
    if (*targ_list == NULL) {
	*targ_list = *src_list;
	*src_list = NULL;
	return DtSrOK;
    }

    /* We have two nonempty lists.  Now do a real merge.
     * Insert src node into targ list only if it's
     * proximity is smaller.  Otherwise just
     * advance to the next targ node.
     */
    src = *src_list;	/* curr src_list node to compare */
    nextsrc = src->link;	/* next src_list node to compare */
    targ = *targ_list;	/* curr targ_list node to compare */
    prevtargl = targ_list;	/* prev targ_list node link pointer */
    while (src != NULL && targ != NULL) {
	if (src->proximity < targ->proximity) {
	    /* Insert src node into targ list */
	    *prevtargl = src;
	    src->link = targ;
	    prevtargl = &src->link;
	    src = nextsrc;
	    if (src)
		nextsrc = src->link;
	}
	else {
	    /* just advance to next targ node */
	    prevtargl = &targ->link;
	    targ = targ->link;
	}
    }

    /* If there's any src_list left,
     *  just tack it onto the end of the targ_list.
     */
    if (src) {
	/* advance to end of targ_list */
	while (targ) {
	    prevtargl = &targ->link;
	    targ = targ->link;
	}
	*prevtargl = src;
    }

    *src_list = NULL;
    return DtSrOK;
}  /* DtSearchMergeResults() */


/************************************************************
 * Aa_sort_dittolist(): basically a copy of ditto_sort() from engine,
 * but will sort on several different ditto fields
 * (currently only date fields implemented),
 * and is only used as convenience function on client/gui side.
 * The proximity field is always the minor sort key.
 * All functions with similar names to engine functions are static
 * in this module so they won't collide with same function in engine.
 * Performs a recursive split-merge sort on ditto lists.
 ************************************************************/

/************************************************/
/*						*/
/*		   ditto_pop			*/
/*						*/
/************************************************/
/* Subroutine of DtSearchSortResults().
 * Detaches first node in a list and returns it...
 * If *lst is empty return NULL, else set *lst to the link
 * cell of the first DtSrResult node on *lst and return a pointer to
 * the first DtSrResult node on *lst.
 */
static DtSrResult *ditto_pop (DtSrResult ** lst)
{
    DtSrResult     *first_node;

    first_node = *lst;
    if (first_node != NULL)
	*lst = first_node->link;
    return first_node;
}  /* ditto_pop() */


/************************************************/
/*						*/
/*		   ditto_split			*/
/*						*/
/************************************************/
/* Subroutine of DtSearchSortResults().
 * Find the middle node in lst. Set its 'next' pointer to NULL.
 * Return the remainder of lst, i.e. a pointer to the
 * next node after the middle node.
 */
static DtSrResult *ditto_split (DtSrResult * lst)
{
    DtSrResult     *tail = lst->link;

    if (lst == NULL || tail == NULL)
	return lst;

    /*
     * Advance 'tail' to end of list, and advance 'lst' only half
     * as often 
     */
    while ((tail != NULL) && ((tail = tail->link) != NULL)) {
	lst = lst->link;
	tail = tail->link;
    }
    tail = lst->link;
    lst->link = NULL;
    return tail;
}  /* ditto_split() */


/************************************************/
/*						*/
/*		 merge_by_prox			*/
/*						*/
/************************************************/
/* Subroutine of DtSearchSortResults().
 * Merges two sorted DtSrResult lists together in proximity order.
 */
static DtSrResult *merge_by_prox (DtSrResult * l1, DtSrResult * l2)
{
    DtSrResult     *myqueue = NULL;
    DtSrResult     *myend = NULL;
    DtSrResult     *mynext;

    while ((l1 != NULL) && (l2 != NULL)) {
	/*
	 * Perform ENQUEUE function. Next item popped off a list is
	 * the next one in sorted order. It is added to END of
	 * myqueue to maintain order. THIS IS WHERE THE ACTUAL SORT
	 * COMPARE FUNCTION IS PERFORMED. 
	 */
	mynext = (l1->proximity < l2->proximity) ?
	    ditto_pop (&l1) : ditto_pop (&l2);

	mynext->link = NULL;
	if (myqueue == NULL)
	    myqueue = mynext;
	else
	    myend->link = mynext;
	myend = mynext;
    }

    /*
     * Perform JOIN QUEUE function. Append entire list to end of
     * queue. 
     */
    if (l1 != NULL)
	myend->link = l1;
    if (l2 != NULL)
	myend->link = l2;
    return myqueue;
}  /* merge_by_prox() */


/************************************************/
/*						*/
/*		 merge_by_date			*/
/*						*/
/************************************************/
/* Subroutine of DtSearchSortResults().
 * Merges two sorted DtSrResult lists together in objdate order.
 */
static DtSrResult *merge_by_date (DtSrResult * l1, DtSrResult * l2)
{
    DtSrResult     *myqueue = NULL;
    DtSrResult     *myend = NULL;
    DtSrResult     *mynext;

    while ((l1 != NULL) && (l2 != NULL)) {
	/*
	 * Perform ENQUEUE function. Next item popped off a list is
	 * the next one in sorted order. It is added to END of
	 * myqueue to maintain order. THIS IS WHERE THE ACTUAL SORT
	 * COMPARE FUNCTION IS PERFORMED. 
	 */
	if (l1->objdate == l2->objdate)
	    mynext = (l1->proximity < l2->proximity) ?
		ditto_pop (&l1) : ditto_pop (&l2);
	else
	    mynext = (l1->objdate > l2->objdate) ?
		ditto_pop (&l1) : ditto_pop (&l2);

	mynext->link = NULL;
	if (myqueue == NULL)
	    myqueue = mynext;
	else
	    myend->link = mynext;
	myend = mynext;
    }

    /*
     * Perform JOIN QUEUE function. Append entire list to end of
     * queue. 
     */
    if (l1 != NULL)
	myend->link = l1;
    if (l2 != NULL)
	myend->link = l2;
    return myqueue;
}  /* merge_by_date() */


/************************************************/
/*						*/
/*		   ditto_sort			*/
/*						*/
/************************************************/
/* Subroutine of DtSearchSortResults().
 * Sorts a list of DtSrResult structures and returns ptr to sorted list.
 * The basic idea is to sort by recursively splitting a list
 * into two equal halves and sorting each of those.  The recursion
 * ends when there are only two small lists which are either
 * already sorted or are swapped.  This sort rarely runs out
 * of stack space because each recursion cuts the list length in
 * half so there are at most 1 + log-N-to-the-base-2 items on the stack.
 * (e.g. 64,000 nodes = max stack depth of 16:  2**16 = 64K).
 */
static DtSrResult *ditto_sort (DtSrResult * lst)
{
    DtSrResult     *lst2;

    if ((lst == NULL) || (lst->link == NULL))
	return lst;
    lst2 = ditto_split (lst);
    switch (ditsort_type) {
	case DtSrSORT_PROX:
	    return merge_by_prox (ditto_sort (lst), ditto_sort (lst2));
	case DtSrSORT_DATE:
	    return merge_by_date (ditto_sort (lst), ditto_sort (lst2));
	default:
	    fprintf (aa_stderr, PROGNAME "525 Invalid Sort Type %d.\n",
		ditsort_type);
	    DtSearchExit (32);
    }
}  /* ditto_sort() */


/************************************************/
/*						*/
/*		 DtSearchSortResults		*/
/*						*/
/************************************************/
/* Only publicly visible sort function.
 * DtSearchSortResults() was formerly named aa_sort_dittolist.
 */
int             DtSearchSortResults (DtSrResult ** dittolist, int sort_type)
{
    switch (sort_type) {
	case DtSrSORT_PROX:
	case DtSrSORT_DATE:
	    ditsort_type = sort_type;
	    *dittolist = ditto_sort (*dittolist);	/* recursive call */
	    return DtSrOK;
	default:
	    DtSearchAddMessage (PROGNAME "140 "
		"Program Error: Invalid sort type.");
	    return DtSrERROR;
    }
}  /* DtSearchSortResults() */

/**************************** DTSRJOINT.C *************************/
