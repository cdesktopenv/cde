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
 *   FUNCTIONS: findstr_workproc
 *		kwic_workproc
 *		make_one_kwic
 *		oe_ditto2kwic
 *		oe_findstr_hitl
 *		restore_findstr_hitl
 *
 *   ORIGINS: 27
 *
 *   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 *   combined with the aggregated modules for this product)
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************************* OEKWIC.C ********************************
 * $XConsortium: oekwic.c /main/4 1996/05/07 13:42:36 drk $
 * April 1992.
 * Opera Engine (OE) functions that create KeyWord In Context (KWIC)
 * abstracts to replace database abstract when requested by user.
 * KWIC abstract is a string extracted
 * from the cleartext where the first hitword appears.
 * Also includes find-string functions which use similar logic.
 * Also includes a few generic OE utilities.
 *
 * $Log$
 * Revision 1.6  1995/09/05  19:02:02  miker
 * Made usrblk universal global.  Deleted refs to socblk.
 * Other minor name and function changes for DtSearch.
 *
 */
#include "SearchE.h"
#include <ctype.h>

/******#define DEBUG_KWIC*********/
/********#define DEBUG_FINDSTR_ITER***********/
/******#define DEBUG_FINDSTR********/

#define PROGNAME			"OEKWIC"
#define START_KWIC_ITERATIONS		10	
#define START_FINDSTR_ITERATIONS	10	
#define MIN_KWIC_ITERATIONS		2
#define MIN_FINDSTR_ITERATIONS		2
#define MIN_KWIC_BUFSZ			20

static int	found_one_substring = FALSE;

/************************************************/
/*						*/
/*		  make_one_kwic			*/
/*						*/
/************************************************/
/* Builds abstract for record in usrblk.cleartext to
 * KWIC string where first word/substring in
 * usrblk.hitwords array is in center of abstract string.
 * Returns new abstract in passed buffer (in a ditto list).
 * General format of new abstract is:  "...text <<word>> text...".
 */
static void	make_one_kwic (char *abstract)
{
char	c;
int	i;
long	from;	/* offset from beginning of cleartext */
int	abstrsz;
char	*to, *lastto;

abstrsz = usrblk.dblk->dbrec.or_abstrsz;
if (usrblk.kwiclen > 0 && usrblk.kwiclen < abstrsz)
    abstrsz = usrblk.kwiclen;
to = abstract;
lastto = to + abstrsz - 2;

/* No hitwords to abstract */
if (usrblk.hitwcount <= 0L)
    return;

/* Find beginning of 'from' string */
from = usrblk.hitwords[0].offset - 
    ((abstrsz - usrblk.hitwords[0].length - 14) >> 1L);
if (from < 0L) from = 0L;

/* If abstract doesnt begin at start of cleartext, print ellipsis */
if (from != 0L)  for (i=3; i>0; i--)
    *to++ = '.';

/* Move text up to the start of the word.
 * Replace cntrl chars with single space.
 */
while (from < usrblk.hitwords[0].offset)  
    {
    c = usrblk.cleartext [from++];
    *to++ = (iscntrl(c))? ' ' : c;
    }

/* Move the word itself, hilited with angle brackets */
for (i=2; i>0; i--)
    *to++ = '<';
for (i=0; i<usrblk.hitwords[0].length; i++)  
    *to++ = usrblk.cleartext [from++];
for (i=2; i>0; i--)
    *to++ = '>';

/* Move text beyond the word until end of input or end of abstract buffer */
while (from < usrblk.clearlen && to < lastto - 4)
    {
    c = usrblk.cleartext [from++];
    *to++ = (iscntrl(c))? ' ' : c;
    }

/* If not end of input, print another ellipsis */
if (from < usrblk.clearlen)  for (i=3; i>0; i--)  *to++ = '.';

*to = '\0';

#ifdef DEBUG_KWIC
printf("%s\n", abstract);
#endif

return;
} /* make_one_kwic() */


/************************************************/
/*						*/
/*		  oe_ditto2kwic			*/
/*						*/
/************************************************/
/* Converts all abstracts in dittolist to KWIC strings */
void	oe_ditto2kwic (void)
{
void	kwic_workproc (void);
int	search_type = usrblk.search_type;

/* First validate the input fields in usrblk */
if  (usrblk.dittocount <= 0L || usrblk.stemcount <= 0)
    {
    usrblk.retncode = OE_BAD_QUERY;
    return;
    }

/* Set up various global variables for calls to oe_stems_to_hitwords().
 * Only an exact words search (search_type == 'W') does not require
 * stemming.  Statistical, semantic and exact stems searches
 * do require stemming.
 */
OE_kind_of_stems = (search_type == 'W') ? WORD_KIND : STEM_KIND;

/* Initialize where user loop status maintained between workproc calls */
saveusr.dittolist = usrblk.dittolist;
saveusr.iterations = START_KWIC_ITERATIONS;

/* Call the work procedure that traverses the entire dittolist.
 * If NO_ITERATE is specified, it will run to completion.
 * Otherwise it will run just its first set of iterations,
 * move its own address into usrblk.workproc, then return.
 * Until workproc/mainloop is done, the static variable 'saveusr.dittolist'
 * will always indicate where the last execution of the loop ended.
 */
usrblk.flags &= ~USR_STOPSRCH;		/* init stop button to OFF */
kwic_workproc();			/* work proc will set retncode */
return;
} /* oe_ditto2kwic() */


/************************************************/
/*						*/
/*		  kwic_workproc			*/
/*						*/
/************************************************/
/* called repeatedly to convert abstracts in dittolist to KWIC strings */
void	kwic_workproc (void)
{
int	iter;
int	entire_list_done;
time_t	start_time;
double	time_dif;
DtSrResult	*dit;
LLIST	*bloblist;
char	sprintbuf[256];

/* test whether user has pushed STOP button since last call */
if (usrblk.flags & USR_STOPSRCH)
    {
    usrblk.retncode = OE_USER_STOP;
    return;
    }

#ifdef DEBUG_KWIC
printf ("\nSTART ITERATIONS = %d\n", saveusr.iterations);
#endif

/* initialize the loop */
dit = saveusr.dittolist;
entire_list_done = FALSE;
time (&start_time);

/* Traverse entire dittolist.  Unblob each record,
 * create a hitword list, then use it to convert abstract.
 */
for (iter = saveusr.iterations;  iter > 0; )
    {
    /* Set usrblk.dblk ptr to correct database for curr ditto node */
    if (dbn_to_dblk (dit->dbn))
	saveusr.vistano = usrblk.dblk->vistano;
    else
	{
	usrblk.retncode = OE_NOTAVAIL;
	return;
	}

    /* If this record's database has too small an
     * abstract buffer, just skip the conversion.
     */
    if (usrblk.dblk->dbrec.or_abstrsz < MIN_KWIC_BUFSZ)
	goto NEXT_DIT;

    /* Skip any records which do not have blobs. */
    if (usrblk.dblk->dbrec.or_dbaccess != ORA_BLOB)
	goto NEXT_DIT;

    if ((bloblist = ve_getblobs (dit->dba, saveusr.vistano)) == NULL)
	{
	sprintf (sprintbuf, PROGNAME"149 "
	    "Corrupted database address on hitlist. "
	    "Erroneous record: '%s' in database '%s'.",
	    dit->reckey, usrblk.dblk->label);
	DtSearchAddMessage (sprintbuf);
	usrblk.retncode = OE_NOTAVAIL;
	return;
	}
    usrblk.retncode = oe_unblob (bloblist, FALSE);
    if (usrblk.retncode != OE_OK)
	{
	sprintf (sprintbuf, PROGNAME"213 "
	    "Erroneous record: '%s' in database '%s'.",
	    dit->reckey, usrblk.dblk->label);
	DtSearchAddMessage (sprintbuf);
	return;
	}
    if (!oe_stems_to_hitwords (1))
	return;
    make_one_kwic (dit->abstractp);

    /* Increment ditto pointer. Check for end of dittolist. */
NEXT_DIT:
    if ((dit = dit->link) == NULL)
	{
	entire_list_done = TRUE;
	break;
	}

    /* Decrement iteration counter unless user said not to */
    if (!(usrblk.flags & USR_NO_ITERATE))
	iter--;
    } /* end iteration loop */

if (usrblk.debug & (USRDBG_SRCHCMPL | USRDBG_HITLIST))
    print_dittolist (saveusr.dittolist, PROGNAME"888");

/* End of current set of iterations.
 * If main loop is not completed,
 * adjust number of iterations to about 1 second,
 * save current status, and return to caller.
 */
if (!entire_list_done)
    {
    time_dif = difftime (time(NULL), start_time);
    if (time_dif < 1.)
	saveusr.iterations = (float) saveusr.iterations * 1.5;
    else if (time_dif > 1.)
	saveusr.iterations = (double) saveusr.iterations / time_dif;
    if (saveusr.iterations < MIN_KWIC_ITERATIONS) 
	saveusr.iterations = MIN_KWIC_ITERATIONS;

    #ifdef DEBUG_KWIC
    printf ("\nEND ITERATIONS = %d, time_dif = %lf\n", 
	saveusr.iterations, time_dif);
    #endif

    saveusr.dittolist = dit;	/* temp save curr loc in usrblk.dittolist */
    usrblk.workproc = kwic_workproc;
    usrblk.retncode = OE_SEARCHING;
    return;
    } /* endif where we are still searching */

/* Unless user specified no_iterate, calling program should always 
 * reset workproc to NULL.  Dummy_workproc just appends an
 * error message to notify programmer of his problem.
 */
if (!(usrblk.flags & USR_NO_ITERATE)) 
    usrblk.workproc = dummy_workproc;

usrblk.retncode = OE_OK;
return;
} /* kwic_workproc() */


/************************************************/
/*						*/
/*		 oe_findstr_hitl		*/
/*						*/
/************************************************/
/* Reduces dittolist to the subset of records that
 * contain string in query, and converts their
 * abstracts to a KWIC-like string.
 */
void	oe_findstr_hitl (void)
{
void	findstr_workproc (void);
char	*ptr, *ptr2;
char	msgbuf [256];

/* Validate input fields */
if (usrblk.query == NULL || usrblk.query[0] == '\0')
    {
    usrblk.retncode = OE_BAD_QUERY;
    return;
    }
if (strlen(usrblk.query) >= DtSrMAXWIDTH_HWORD)
    {
    sprintf (msgbuf, PROGNAME"516 "
	"No more than %d characters are allowed in the search string.",
	DtSrMAXWIDTH_HWORD - 1);
    DtSearchAddMessage (msgbuf);
    usrblk.retncode = OE_BAD_QUERY;
    return;
    }
if (usrblk.dittocount <= 0 || usrblk.dittolist == NULL)
    {
    DtSearchAddMessage (PROGNAME"317 Hitlist empty.  Nothing to search.");
    usrblk.retncode = OE_BAD_HITLIST;
    return;
    }

/* Save stems array so we can restore it later,
 * then copy the query to it for use by oe_stems_to_hitwords call.
 * Convert to uppercase as we copy.
 */
saveusr.stemcount = usrblk.stemcount;
memcpy (saveusr.stems, usrblk.stems,
    (size_t) (usrblk.stemcount * DtSrMAXWIDTH_HWORD));
usrblk.stemcount = 1;
ptr = usrblk.stems[0];		/* target */
ptr2 = usrblk.query;		/* source */
while (*ptr2 != 0)
    *ptr++ = toupper(*ptr2++);
*ptr = 0;

OE_kind_of_stems = STRING_KIND;

/* initialize saveusr loop status stuff maintained between workproc calls */
saveusr.vistano = usrblk.dblk->vistano;
saveusr.iterations = START_FINDSTR_ITERATIONS;
saveusr.dittolist = usrblk.dittolist;	/* curr start of each iteration */
saveusr.dittocount = 0;		/* number of records containing substring */

/* Call the work procedure that traverses the entire dittolist.
 * If NO_ITERATE is specified, it will run to completion.
 * Otherwise it will run just its first set of iterations,
 * move its own address into usrblk.workproc, then return.
 * Until workproc/mainloop is done, the static variable 'saveusr.dittolist'
 * will always indicate where the last execution of the loop ended.
 * The ditto node for each record which is found to contain
 * the string will be removed from dittolist and added to newditlist.
 */
usrblk.flags &= ~USR_STOPSRCH;		/* init stop button to OFF */
found_one_substring = FALSE;		/* init HIT indicator */
findstr_workproc();			/* work proc will set retncode */
return;
} /* oe_findstr_hitl() */


/************************************************/
/*						*/
/*	       restore_findstr_hitl		*/
/*						*/
/************************************************/
/* Each time the find-string workproc detected the search
 * string in a ditto node, it converted its abstract.
 * Other nodes were just marked for deletion.
 * When the entire dittolist has been traversed,
 * or if the user pushes the stop button to stop traversal,
 * this cleanup function is called to delete those nodes that
 * were marked for deletion.  However if NO records were
 * ever found containing the string, nothing is deleted and
 * the hitlist is restored as it was prior to the beginning of the search.
 * The hitwords array is always blown away but the stems
 * array is always restored no matter what.
 */
static void	restore_findstr_hitl (void)
{
DtSrResult	*dit, *nextdit;
DtSrResult	**lastlink;
long	newdittocount = 0L;

/* If the string was ever found in any record,
 * delete all nonhits up to the last successful find.
 */
#ifdef DEBUG_FINDSTR
printf ("\nRESTORE FINDSTR: totnumhits = %ld\n", saveusr.dittocount);
#endif
if (saveusr.dittocount > 0)
    {
    dit = usrblk.dittolist;
    lastlink = &usrblk.dittolist;
    while (dit != NULL)
	{
	/* On a hit, advance the pointers, advance new dittocount.
	 * Break the loop on the last hit.
	 */
	if (dit->flags & DIT_FINDSTR)
	    {
	    #ifdef DEBUG_FINDSTR
	    printf ("#%ld HIT %s, \"%s\"\n", newdittocount+1,
		dit->reckey, dit->abstract);
	    #endif
	    lastlink = &dit->link;
	    dit = dit->link;
	    if (++newdittocount >= saveusr.dittocount)
		break;
	    }
	/* If this was NOT a hit, delete node,
	 * and link up loose ends.
	 */
	else
	    {
	    #ifdef DEBUG_FINDSTR
	    printf ("RESTORE DELETING %s\n", dit->reckey);
	    #endif
	    nextdit = dit->link;
	    free (dit);
	    *lastlink = nextdit;
	    dit = nextdit;
	    }
	}

    /* At this point, we've cleaned up the list
     * down to the last hit, or to its end.
     * Keep deleting until end of list, or user's stop point.
     */
    while (dit != NULL)
	{
	if (!(usrblk.flags & USR_STOPSRCH)  /* never stopped */
	    || !(dit->flags & DIT_STOP))   /* stopped somewhere ahead */
	    {
	    #ifdef DEBUG_FINDSTR
	    printf ("PAST LAST HIT, DELETING %s\n", dit->reckey);
	    #endif
	    nextdit = dit->link;
	    free (dit);
	    *lastlink = nextdit;
	    dit = nextdit;
	    }
	else
	    {
	    break;	/* user stopped at this exact node */
	    }
	}

    /* If there's anything left on the list,
     * its because user stopped the search here.
     * Just count the remaining records for the final tally.
     */
    while (dit != NULL)
	{
	newdittocount++;
	#ifdef DEBUG_FINDSTR
	printf ("#%ld SAVING AFTER STOP %s\n", newdittocount, dit->reckey);
	#endif
	dit = dit->link;
	}
    usrblk.dittocount = newdittocount;
    } /* endif where at least one record had a string hit */

/* restore the original stems array */
usrblk.stemcount = saveusr.stemcount;
memcpy (usrblk.stems, saveusr.stems,
    (size_t) (saveusr.stemcount * DtSrMAXWIDTH_HWORD));
#ifdef DEBUG_FINDSTR
printf ("LEAVING RESTORE now real dittocount = %ld, stemcount = %d\n"
    "   first stem = '%s'\n", usrblk.dittocount,
    usrblk.stemcount, usrblk.stems[0]);
#endif
return;
} /* restore_findstr_hitl() */


/************************************************/
/*						*/
/*		 findstr_workproc		*/
/*						*/
/************************************************/
/* Called repeatedly to search for character substrings in records
 * on the hitlist, and convert their abstracts to KWIC strings.
 */
void	findstr_workproc (void)
{
int	iter;
int	entire_list_done;
time_t	start_time;
double	time_dif;
DtSrResult	*dit, *cutdit;
LLIST	*bloblist;

/* Test whether user has pushed STOP button since last call.
 * The DIT_STOP marks where the search ended for restore_findstr_hitl().
 */
if (usrblk.flags & USR_STOPSRCH)
    {
    saveusr.dittolist->flags |= DIT_STOP;
    usrblk.retncode = (found_one_substring)? OE_OK : OE_USER_STOP;
    restore_findstr_hitl();
    return;
    }

#ifdef DEBUG_FINDSTR_ITER
printf ("\nSTART ITERATIONS = %d\n", saveusr.iterations);
#endif

/* initialize the loop */
dit = saveusr.dittolist;
entire_list_done = FALSE;
time (&start_time);

/* Traverse entire dittolist, starting where we last left off.
 * Unblob each record and search for the query string. 
 * If found, mark it and convert its abstract.
 * If not found, make sure it's not marked and then continue.
 */
for (iter = saveusr.iterations;  iter > 0; )
    {
    /*****usrblk.dba = dit->dba;**** unnecessary?******/
    if ((bloblist = ve_getblobs (dit->dba, saveusr.vistano)) == NULL)
	{
	DtSearchAddMessage (
	    PROGNAME"390 Corrupted database address on hitlist.");
	restore_findstr_hitl();
	usrblk.retncode = OE_BAD_HITLIST;
	return;
	}
    usrblk.retncode = oe_unblob (bloblist, FALSE);
    if (usrblk.retncode != OE_OK)
	return;
    if (!oe_stems_to_hitwords (1))
	return;

    if (usrblk.hitwcount > 0)		/* string FOUND */
	{
	found_one_substring = TRUE;
	saveusr.dittocount++;
	#ifdef DEBUG_FINDSTR_ITER
	printf ("<<--->> HIT numhits=%ld, reckey = %s\n",
	    saveusr.dittocount, dit->reckey);
	#endif
	make_one_kwic (dit->abstractp);
	dit->flags |= DIT_FINDSTR;
	}
    else dit->flags = 0;			/* string NOT found */

    /* advance to next ditto node */
    if ((dit = dit->link) == NULL)
	{
	entire_list_done = TRUE;
	break;
	}

    /* decrement iteration counter unless user said not to */
    if (!(usrblk.flags & USR_NO_ITERATE)) iter--;
    } /* end iteration loop */

/* End of current set of iterations.
 * If main loop is not completed,
 * adjust number of iterations to about 1 second,
 * save current status, and return to caller.
 */
if (!entire_list_done)
    {
    time_dif = difftime (time(NULL), start_time);
    if (time_dif < 1.)
	saveusr.iterations = (float) saveusr.iterations * 1.5;
    else if (time_dif > 1.)
	saveusr.iterations = (double) saveusr.iterations / time_dif;
    if (saveusr.iterations < MIN_FINDSTR_ITERATIONS) 
	saveusr.iterations = MIN_FINDSTR_ITERATIONS;

    #ifdef DEBUG_FINDSTR_ITER
    printf ("\nEND ITERATIONS = %d, time_dif = %lf\n", 
	saveusr.iterations, time_dif);
    #endif

    saveusr.dittolist = dit;	/* temp save curr loc in usrblk.dittolist */
    usrblk.workproc = findstr_workproc;
    usrblk.retncode = OE_SEARCHING;
    return;
    } /* endif where we are still searching */

/* Completely done!  Unless user specified no_iterate,
 * calling program should always reset workproc to NULL. 
 * In case he forgets, dummy_workproc just appends an
 * error message to notify programmer of his problem.
 */
if (!(usrblk.flags & USR_NO_ITERATE)) 
    usrblk.workproc = dummy_workproc;

if (saveusr.dittocount > 0)
    usrblk.retncode = OE_OK;
else
    usrblk.retncode = OE_NOTAVAIL;
restore_findstr_hitl();
return;
} /* findstr_workproc() */
    
/******************************* OEKWIC.C ********************************/
