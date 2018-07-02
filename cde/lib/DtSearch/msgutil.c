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
 *   FUNCTIONS: austext_malloc
 *		clean_wrap
 *		cutnode_llist
 *		free_llist
 *		join_llists
 *		merge_llist
 *		nowstring
 *		pop_llist
 *		sort_llist
 *		split_llist
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1991,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** MSGUTIL.C *************************
 * $XConsortium: msgutil.c /main/9 1996/11/25 18:47:48 drk $
 * August 1991.
 * Utilites for generic manipulation of linked lists and binary trees.
 * All utilities require that the link fields be the first fields
 * in each structure.  Also includes an error message mechanism
 * that is absolutely independent of user interface (UI)--
 * not even stdio is used.  The trick is to append all
 * error/information messages to the end of a linked list that
 * eventually will be returned to the UI to be displayed
 * in whatever manner is appropriate.  (However a generic stdio
 * print facility for the linked msglists is also provided 
 * for dumping the list before crashing or when stdio is ok).
 * 
 * With 2 exceptions, all messages should contain only
 * printable ascii characters and the space character.
 * Control characters and extended ascii graphics chars are verboten.
 * The exceptions are \n and \r, which are always permitted.
 * 
 * The most common fatal error in opera and other text analysis
 * systems occurs when a memory allocation fails.  Therefore
 * this module contains a generic 'safe malloc' function
 * which tests for failure and prints all outstanding messages
 * before exiting if malloc fails.  It also uses DtSearchExit()
 * for the actual abort so other system dependent stuff can be
 * performed before going down.
 *
 * $Log$
 * Revision 2.4  1996/03/05  17:58:29  miker
 * Replace isspace() with ref to locale independent ascii_charmap[].
 *
 * Revision 2.3  1995/10/25  16:46:00  miker
 * Added prolog.
 *
 * Revision 2.2  1995/10/19  20:58:05  miker
 * Fix segfault in cleanwrap() if text contains single word > wraplen.
 *
 * Revision 2.1  1995/09/22  21:20:35  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.8  1995/09/05  18:21:23  miker
 * For DtSearch, rename and move universal msglist functions to msgs.c.
 */
#include "SearchP.h"
/****#include <ctype.h>****/
#include <string.h>
#include <stdlib.h>
#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#include <X11/Xos_r.h>

#define PROGNAME	"MSGUTIL"
#define MAX_LINELEN	77
#define MS_misc		1

CMPLL compare_llist = NULL;	/* global function pointer */

/************************************************/
/*						*/
/*		   nowstring			*/
/*						*/
/************************************************/
/* Returns ptr to static string where the current time
 * is formatted in human readable form.  Used in audit
 * records, debugging logs, and error messages.
 */
char           *nowstring (time_t * now)
{
    static char     buf[128];
    time_t          mynow;
    struct tm *	    time_ptr;
    _Xltimeparams   localtime_buf;

    if (now == NULL) {
	now = &mynow;
	time (now);
    }
    time_ptr = _XLocaltime(now, localtime_buf);
    strftime (buf, sizeof (buf),
	catgets (dtsearch_catd, MS_misc, 2, "%Y/%m/%d,%H:%M:%S"),
	time_ptr);
    return buf;
}  /* nowstring() */


/************************************************/
/*						*/
/*		   free_llist			*/
/*						*/
/************************************************/
/* Frees storage for all items in an LLIST and
 * sets it top-of-list pointer to NULL.  This works only for lists
 * created by append_msglist, append_textblobs, and other LLIST
 * structures where the data and the node itself
 * are allocated in one call to malloc().
 */
void            free_llist (LLIST ** llhead)
{
    LLIST          *next;
    LLIST          *ll = *llhead;
    while (ll != NULL) {
	next = ll->link;
	free (ll);
	ll = next;
    }
    *llhead = NULL;
    return;
}  /* free_llist() */

/************************************************/
/*						*/
/*		   join_llists			*/
/*						*/
/************************************************/
/* Merges two list by appending sublist to end of mainlist,
 * then setting sublist to NULL.  Originally either list can be NULL.
 * Mainlist is represented by ptr to ptr so it can be modified if NULL.
 * Sublist is ptr to ptr so it can be SET to NULL after join.
 * Method:
 * Init pp = ptr to first 'link' field in list,
 * i.e. the ptr to a ptr passed by the caller.
 * Usually this will be the addr of the top of the list pointer.
 * Then advance it to point to the last link in the list.
 * 
 * Note that this function works for any LLIST including those
 * whose 'data' pointer is not allocated concurrently with the node.
 */
void            join_llists (LLIST ** mainlist, LLIST ** sublist)
{
    LLIST         **pp;
    for (pp = mainlist; *pp != NULL; pp = &((*pp)->link));
    *pp = *sublist;
    *sublist = NULL;
    return;
}  /* join_llists() */


/************************************************/
/*						*/
/*		  pop_llist			*/
/*						*/
/************************************************/
/* Detaches first node in an llist and returns it.
 * If *llistp is empty return NULL, else set *llistp to the link
 * cell of the first LLIST node on *llistp and return a pointer to
 * the first LLIST node on *llistp.  Used mainly by merge_llist(),
 * which is itself called by sort_llist(), but can be called by
 * anyone needing to remove the first element of an llist.
 */
LLIST          *pop_llist (LLIST ** llistp)
{
    LLIST          *first_node = *llistp;
    if (first_node != NULL)
	*llistp = first_node->link;
    return first_node;
}  /* pop_llist() */


/************************************************/
/*						*/
/*		  cutnode_llist			*/
/*						*/
/************************************************/
/* Detaches any specified node in an llist and rejoins the
 * loose ends of the llist.  *llistp may become NULL.
 * Returns NULL if *llistp is initially NULL or if node is not on llist,
 * else returns the detached node.
 */
LLIST          *cutnode_llist (LLIST * node, LLIST ** llistp)
{
    LLIST         **pp;	/* link addr pointing to current node */
    for (pp = llistp; *pp != NULL; pp = &((*pp)->link)) {
	if (*pp == node)
	    break;
    }
    if (*pp == NULL)
	return NULL;
    *pp = node->link;	/* join the loose ends */
    return node;
}  /* cutnode_llist() */


/************************************************/
/*						*/
/*		   split_llist			*/
/*						*/
/************************************************/
/* Subroutine of sort_llist().
 * Find the middle node in lst. Set its 'link' pointer to NULL.
 * Return the remainder of lst, i.e. a pointer to the
 * next node after the middle node.
 */
static LLIST   *split_llist (LLIST * lst)
{
    LLIST          *tail;
    if (lst == NULL || lst->link)
	return lst;

    tail = lst->link;

    /* advance 'tail' to end of list, and advance 'lst' only half as often */
    while ((tail != NULL) && ((tail = tail->link) != NULL)) {
	lst = lst->link;
	tail = tail->link;
    }
    tail = lst->link;
    lst->link = NULL;
    return tail;
}  /* split_llist() */


/************************************************/
/*						*/
/*		 merge_llist			*/
/*						*/
/************************************************/
/* Subroutine of sort_llist().  Merges two sorted LLISTs together. */
static LLIST   *merge_llist (LLIST * l1, LLIST * l2)
{
    LLIST          *myqueue = NULL;
    LLIST          *myend = NULL;
    LLIST          *mynext;

    while ((l1 != NULL) && (l2 != NULL)) {
	/*
	 * Perform ENQUEUE function. Next item popped off a list is
	 * the next one in sorted order. It is added to END of
	 * myqueue to maintain order. THIS IS WHERE THE ACTUAL SORT
	 * COMPARE FUNCTION IS PERFORMED. 
	 */
	mynext = (compare_llist (l1, l2) < 0) ?
	    pop_llist (&l1) : pop_llist (&l2);
	mynext->link = NULL;
	if (myqueue == NULL)
	    myqueue = mynext;
	else
	    myend->link = mynext;
	myend = mynext;
    }

    /* attach the remainder of whichever list is left to the end of queue */
    if (l1 != NULL)
	myend->link = l1;
    if (l2 != NULL)
	myend->link = l2;
    return myqueue;
}  /* merge_llist() */


/************************************************/
/*						*/
/*		  sort_llist			*/
/*						*/
/************************************************/
/* Sorts a list of LLIST structures and returns ptr to sorted list.
 * The basic idea is to sort by recursively splitting a list
 * into two equal halves and sorting each of those.  The recursion
 * ends when there are only two small lists which are either
 * already sorted or are swapped.  This sort rarely runs out
 * of stack space because each recursion cuts the list length in
 * half so there are at most 1 + log-N-to-the-base-2 items on the stack.
 * (e.g. 64,000 nodes = max stack depth of 16:  2**16 = 64K).
 *
 * The compare function accepts pointers to two LLIST structures.
 * It returns <0, =0, or >0 based on whether the first structure (left)
 * is <, =, or > the second (right) structure in sort order.
 * For efficiency's sake, a pointer to the compare function is placed
 * in the global variable 'compare_llist' before calling
 * sort_llist the first time, rather than continually
 * passing it to all these nested functions.
 */
LLIST          *sort_llist (LLIST * lst)
{
    LLIST          *lst2;
    if ((lst == NULL) || (lst->link == NULL))
	return lst;
    lst2 = split_llist (lst);
    return merge_llist (sort_llist (lst), sort_llist (lst2));
}  /* sort_llist() */


/************************************************/
/*						*/
/*		  austext_malloc		*/
/*						*/
/************************************************/
/* 'location' may be NULL.  Last arg (formerly msglist) is isgnored.
 * Renamed from safe_malloc() to force compile errors if args not changed.
 */
void           *austext_malloc (size_t size, char *location, void *ignore)
{
    static void    *ptr;
    char           *outofmem_msg;

    if ((ptr = malloc (size)) != NULL)
	return ptr;

    ptr = ((aa_argv0) ? aa_argv0 : "");
    if (location == NULL)
	location = catgets (dtsearch_catd, MS_misc, 1, "<null>");
    outofmem_msg = catgets (dtsearch_catd, MS_misc, 3,
	"*** %sOut of Memory at %s asking for %lu bytes! ***\n");
    fprintf (aa_stderr, outofmem_msg, ptr, location, size);
    fflush (aa_stderr);
    if (ausapi_msglist)
	fprintf (aa_stderr, "%s\n", DtSearchGetMessages ());
    fflush (aa_stderr);
    DtSearchExit (43);
    return NULL;
}  /* austext_malloc */

/************************************************/
/*						*/
/*		   clean_wrap			*/
/*						*/
/************************************************/
/* Utility which provides a way of breaking up long
 * messages which contain no control characters into lines
 * whose linefeed breaks occur at even word boundaries,
 * and whose lengths are controlled by the caller.
 * It should only be called when text itself may be
 * modified and may safely contain a linefeed (ascii 0x0A).
 * Converts a long text string into several lines by overlaying
 * the whitespace char nearest to the end of every line with \n.
 * Restarts length count if \n is found already within string.
 * Does nothing if passed wrap length = 0.
 * Does not append \n to end of string.  Does not alter string length.
 * Returns total number of lines (\n's + trailing piece of last line),
 * or returns 0 if wraplen == 0.
 */
int             clean_wrap (char *string, int wraplen)
{
    char           *nlptr, *breakptr;
    int             linecount = 0;

    if (wraplen <= 0)
	return 0;
    while (strlen (string) > wraplen) {
	breakptr = string + wraplen;

	/* Look for \n within the next wraplen */
	for (nlptr = string; nlptr < breakptr; nlptr++)
	    if (*nlptr == '\n')
		break;
	if (nlptr < breakptr) {
	    string = ++nlptr;
	    goto LINE_DONE;
	}

	/* Otherwise back up to the first whitespace before last word */
	for (nlptr = breakptr - 1;  nlptr > string;  nlptr--)
	    if (ascii_charmap[*nlptr] & WHITESPACE) {
		*nlptr = '\n';
		string = ++nlptr;
		goto LINE_DONE;
	    }

	/* No whitespace at all in "text" before wraplen!
	 * No choice but to overlay the last char.
	 */
	*(--breakptr) = '\n';
	string = ++breakptr;

LINE_DONE:
	linecount++;
    }

    /* Done wrapping.  now just count remaining lines in string. */
    while (*string != 0)
	if (*string++ == '\n')
	    linecount++;
    if (*(string - 1) != '\n')
	linecount++;
    return linecount;
}  /* clean_wrap() */

/********************** MSGUTIL.C *************************/
