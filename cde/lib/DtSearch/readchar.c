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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: discard_to_ETX
 *		readchar_ftext
 *		readchar_string
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************ READCHAR.C *******************************
 * $XConsortium: readchar.c /main/3 1996/05/07 13:47:58 drk $
 * January 1996.
 * Character reading cofunctions for language parsers.
 *
 * $Log$
 * Revision 1.5  1996/03/25  17:01:19  miker
 * Clean up compiler warning.
 *
 * Revision 1.4  1996/03/13  22:59:39  miker
 * Added prolog.  Changed char to UCHAR several places.
 *
 * Revision 1.3  1996/03/05  18:39:34  miker
 * Make *all* char ptrs unsigned.
 *
 * Revision 1.2  1996/03/05  18:08:03  miker
 * Readchar functions return unsigned chars for compatibility
 * with compilers whose default char type is signed.
 *
 * Revision 1.1  1996/02/01  19:20:39  miker
 * Initial revision
 */
#include "SearchP.h"
#include <stdlib.h>
#include <string.h>

#define PROGNAME	"READCHAR"

/************************************************/
/*						*/
/*               readchar_string		*/
/*						*/
/************************************************/
/* Generic readchar cofunction for parsers when the
 * text block is a string.  Used for example when
 * parsing queries.
 */
UCHAR	readchar_string (UCHAR *the_string)
{
    static UCHAR	*strp = (UCHAR *) "";
    if (the_string)
	strp = the_string;
    return ((*strp)? *strp++ : 0);
}


/************************************************/
/*						*/
/*		 discard_to_ETX			*/
/*						*/
/************************************************/
/* Called when dtsrload or dtsrindex wants to skip to next
 * .fzk record by reading and discarding all text to either
 * end of record marker or end of file.
 * Usually called after some error condition in the .fzk file,
 * such as recid not found in database, or when blobs not
 * used in dtsrload so they can be discarded.
 * Wraps around readchar_ftext(), which does the actual read
 * and checks for ETX with a read-ahead buffer.
 */
void	discard_to_ETX (PARG *parg)
{
    if (!readchar_ftext (parg))
	return;
    while (readchar_ftext (NULL))
	;
    return;
} /* discard_to_ETX() */


/************************************************/
/*						*/
/*                readchar_ftext		*/
/*						*/
/************************************************/
/* Returns next char in a text file.  Called in 2 different situations:
 * It's a character reader cofunction called from
 * linguistic parser functions for supported languages.
 * It's also called directly from discard_to_ETX() in offline
 * build tools for *all* languages when for some reason the
 * current record being indexed must be discarded all the way
 * to end of text block (ETX).
 * ETX is when etxdelim string detected, or at end-of-file.
 *
 * The first call, which passes parg, is a reset trigger
 * to clear ETX.  The globals are then set and used in
 * subsequent calls (passing NULL).  This technique is
 * used because it will be called many times in a time
 * critical loop while indexing.
 *
 * Returns '\0' at ETX, and keeps returning '\0'
 * without further reads until the ETX flag is reset.
 * Returns '\0' forever at end-of-file.
 */
UCHAR	readchar_ftext (PARG *parg)
{
    static FILE		*ftext =	NULL;
    static UCHAR	*etxdelim =	NULL;
    static UCHAR	*rabuf =	NULL;
    static int		ETX_flag =	TRUE;
    static int		delimsz =	0;
    static int		bufcount =	0;
    static int		i;
    static UCHAR	*head, *tail, *cptr, *rabufend;

    /* I'm always going to read ahead just enough chars
     * to test the delim string.  The string is expected
     * to be small, typically just a few chars.
     * (A single \0 char indicates there is no record
     * delimiter--record ends only at end of file.)
     * I use a circular read ahead buffer with head and tail ptrs.
     * Bufcount contains current number of chars in buf.
     * Head is next file read point, ahead of youngest char in buf.
     * Tail is next char to return, ie oldest char in buf.
     */
    if (parg) {
	ftext =		parg->ftext;
	if (feof(ftext)) {
	    ETX_flag =	TRUE;
	    return 0;
	}
	if (etxdelim) {
	    free (etxdelim);
	    etxdelim = NULL;
	}
	if (parg->etxdelim)
	    etxdelim =	(UCHAR *) strdup (parg->etxdelim);
	ETX_flag =	FALSE;
	delimsz =	(etxdelim)? strlen((char*) etxdelim) : 0;
	if (!rabuf) {
	    rabuf = austext_malloc (MAX_ETXDELIM + 2, PROGNAME"479", NULL);
	    rabufend = rabuf + MAX_ETXDELIM;
	}
	if (delimsz >= MAX_ETXDELIM) {
	    fprintf (aa_stderr, PROGNAME"505 Record delimiter too long.\n");
	    DtSearchExit (2);
	}
	head = tail =	rabuf;
	bufcount =	0;
    }

    if (ETX_flag)
	return 0;

    /* Read chars into read ahead buf until we
     * have enough to compare for etxdelim.
     * If possible, always read in at least one char.
     */
    while (bufcount == 0 || bufcount < delimsz) {
	if (feof(ftext))
	    break;
	if ((i = fgetc (ftext)) == EOF)
	    break;
	*head++ = i;
	bufcount++;
	if (head >= rabufend)
	    head = rabuf;
    }

    /* There are now 3 possibilities.
     * (1) If bufcount == 0 we got EOF and there
     *     are no chars remaining in buffer, quit now.
     * (2) Most likely bufcount is nonzero and equals delimsz.
     *     Do a wrap-around strcmp looking for delim string.
     * (3) If bufcount is positive but less than delimsz,
     *     we got EOF before the last record ended.
     *     We'll fall through to the code that returns the next
     *     char in the buffer, returning the remaining chars one
     *     at a time until exhausted.
     * Note this sequence also handles the case where delimsz == 0.
     */
    if (bufcount <= 0) {
	ETX_flag = TRUE;
	return 0;
    }

    /* Compare chars in read ahead buf for delim string.
     * (Note that if the compare succeeds, both bufcount
     * and delimsz must be > 0).
     */
    if (bufcount == delimsz) {
	cptr = tail;
	for (i = 0;  i < delimsz;  i++) {
	    if (etxdelim[i] != *cptr)
		break;
	    cptr++;
	    if (cptr >= rabufend)
		cptr = rabuf;
	}
	if (i == delimsz) {
	    ETX_flag = TRUE;
	    return 0;
	}
    }

    /* No ETX yet.  Return the oldest char in read ahead buffer. */
    cptr = tail++;
    if (tail >= rabufend)
	tail = rabuf;
    bufcount--;
    return *cptr;
} /* readchar_ftext */

/*************************** READCHAR.C ****************************/

