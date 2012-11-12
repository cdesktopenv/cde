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
 *   FUNCTIONS: hilite_cleartext
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************************* HILITE.C ********************************
 * $XConsortium: hilite.c /main/5 1996/05/07 13:36:46 drk $
 * January 1992.
 * Opera Engine (OE) functions that create the usrblk.hitwords 
 * array in response to several user requests.
 * The hitwords array identifies the offset and
 * length of words or substrings in cleartext to be hilited
 * by the User Interface (UI).
 * Formerly the hilite.c module was named oehitw.c
 *
 * $Log$
 * Revision 2.4  1996/03/20  19:33:36  miker
 * Changed hilite_cleartext() usage to pass in stems array
 * so code could be reused for collocations searches.
 *
 * Revision 2.3  1996/02/01  19:06:53  miker
 * AusText 2.1.11, DtSearch 0.3:  Major rewrite for new parsers.
 * Replaced oe_stems_to_hitwords with hilite_cleartext.
 *
 * Revision 2.2  1995/10/24  22:03:53  miker
 * Add prolog.
 *
 * Revision 2.1  1995/09/22  20:53:52  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.1  1995/08/31  21:02:32  miker
 * Initial revision
 */
#include "SearchE.h"
#include <stdlib.h>
#include <string.h>

#define PROGNAME	"HILITE"



/****************************************/
/*					*/
/*	     hilite_cleartext		*/
/*					*/
/****************************************/
/* Builds usrblk.hitwords array from usrblk.cleartext
 * and passed stems array.  Arg parse_type must
 * contain 'W' or 'S' to correctly indicate whether
 * the stems array is loaded with parsed words or stems.
 * Stems array must be same format at usrblk.stems (double
 * indexed array with max DtSrMAXWIDTH_HWORD columns per row).
 * (Formerly this function was named 'build_hitwords',
 * then 'oe_stems_to_hitwords').
 * Returns new value of usrblk.hitwords.
 * If zero hitwords, returns NULL in usrblk.hitwords.
 */
long	hilite_cleartext (int parse_type, char *stems, int stemcount)
{
    int			i;
    char		*textp, *stemp;
    DBLK		*dblk =		usrblk.dblk;
    PARG		parg;
    char *		(*parser)() =	usrblk.dblk->parser;
    char *		(*stemmer)() =	usrblk.dblk->stemmer;
    DtSrHitword		*hitwords;
    size_t		hitwords_size =	0;
    long		hitwcount =	0;
    long		offset;
    int			wordlen;
    int			debugging_hilite = (usrblk.debug & USRDBG_HILITE);

    /* Ensure current usrblk.hitwords values are clear */
    clear_hitwords ();

    memset (&parg, 0, sizeof(PARG));
    parg.dblk =		dblk;
    parg.string =	usrblk.cleartext;
    parg.offsetp =	&offset;
    parg.flags |=	PA_HILITING;

    /* OUTER LOOP: Parse/stem each word in cleartext */
    for (	textp = parser (&parg);
		textp;
		textp = parser (NULL)) {
	wordlen = strlen (textp);
	if (parse_type == 'S')
	    textp = stemmer (textp, dblk);
	if (debugging_hilite) {
	    fprintf (aa_stderr,
		PROGNAME"127 offs:%5ld '%s' %n",
		offset, textp, &i);
	    while (i++ < 35)
		fputc (' ', aa_stderr);
	}

	/* INNER LOOP: Search for word in stems array */
	stemp =	stems;
	for (i=stemcount; i;  i--) {
	    if (strcmp (textp, stemp) == 0)
		break;
	    stemp += DtSrMAXWIDTH_HWORD;
	}

	/* Miss.  If text word is not in
	 * stems array, go parse next word.
	 */
	if (i == 0) {
	    if (debugging_hilite)
		fprintf (aa_stderr, "miss.\n");
	    continue;
	}

	/* HIT!  Add to hitwords table. */
	if (hitwcount >= hitwords_size) {
	    if (hitwords_size == 0) {
		hitwords_size = 200;
		hitwords = malloc (
		    hitwords_size * sizeof(DtSrHitword) + 16);
	    }
	    else {
		hitwords_size += hitwords_size >> 1;	/* 1.5 times */
		if (debugging_hilite || (usrblk.debug & USRDBG_RETRVL))
		    fprintf (aa_stderr,
			PROGNAME"098 realloc for %d hitwords.\n",
			hitwords_size);
		hitwords = realloc (hitwords,
		    hitwords_size * sizeof(DtSrHitword) + 16);
	    }
	    if (!hitwords) {
		fputs (PROGNAME"091 Out of Memory!\n", aa_stderr);
		DtSearchExit (91);
	    }
	}
	hitwords[hitwcount].offset =	offset;
	hitwords[hitwcount].length =	wordlen;
	hitwcount++;
	if (debugging_hilite)
	    fprintf (aa_stderr, "HIT!  hwct=%ld.\n", hitwcount);
    } /* end OUTER parse loop */

    usrblk.hitwcount = hitwcount;
    usrblk.hitwords = (hitwcount)? hitwords : NULL;
    if (debugging_hilite || (usrblk.debug & USRDBG_RETRVL))
	fprintf (aa_stderr,
	    PROGNAME"138 parstyp='%c' stemct=%d hitwcount=%ld\n",
	    parse_type, stemcount, usrblk.hitwcount);
    return hitwcount;
}  /* hilite_cleartext() */

/******************************* HILITE.C ********************************/
