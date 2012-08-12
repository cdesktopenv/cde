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
 *   FUNCTIONS: clear_hitwords
 *		clear_usrblk_record
 *		get_hitlist_text
 *		print_dittolist
 *		print_usrblk_record
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
/*********************** DTSRUTIL.C ***********************
 * $XConsortium: dtsrutil.c /main/5 1996/08/12 13:18:30 cde-ibm $
 * October 1991.
 * Set of utility functions for opera User Interfaces (UIs),
 * although may also be used by Opera Engine (OE) itself.
 * Function free_dittolist() replaced by macro call to free_llist().
 * Ausapi wrapped aa_free_dittolist around free_llist().
 * DtSearch renamed aa_free_dittolist to DtSearchFreeResults().
 *
 * $Log$
 * Revision 2.4  1996/03/13  22:54:21  miker
 * Changed char to UCHAR several places.
 *
 * Revision 2.3  1996/03/05  16:05:45  miker
 * Added print_stems().
 *
 * Revision 2.2  1995/10/25  18:36:09  miker
 * Renamed from uiutil.c.  Added prolog.
 *
 * Log: uiutil.c,v
 * Revision 2.1  1995/09/22  22:19:47  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.13  1995/09/05  19:17:28  miker
 * Minor name and function changes for DtSearch.  Made usrblk global.
 */
#include "SearchE.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#define MS_misc		1

/********#define DEBUG_CLEARREC*********/
/**********#define DEBUG_FAX***********/
#define PROGNAME	"DTSRUTIL"


/************************************************/
/*						*/
/*		  print_stems			*/
/*						*/
/************************************************/
/* For debugging.  Prints passed stems array to aa_stderr. */
void	print_stems (int stemcount, void *stems, char *prefix)
{
    int		i;
    UCHAR	*cptr;

    if (stemcount <= 0) {
	fprintf (aa_stderr, "%s Stems array is empty.\n", prefix);
	fflush (aa_stderr);
	return;
    }
    fprintf (aa_stderr, "%s stemct = %d:\n", prefix, stemcount);
    for (i=0;  i<stemcount;  i++) {
	cptr = (UCHAR *) stems + (i * DtSrMAXWIDTH_HWORD);
	if (i == 3 || i == 6)
	    fputc ('\n', aa_stderr);
	fprintf (aa_stderr, "   #%d:'%c%s'",
	    i,  (*cptr < 32) ? '~' : *cptr,  cptr + 1);
    }
    fputc ('\n', aa_stderr);
    fflush (aa_stderr);
    return;
} /* print_stems() */


/************************************************/
/*                                              */
/*		print_dittolist			*/
/*                                              */
/************************************************/
/* Only for debugging in oe.  Dumps first 10 list nodes. */
void            print_dittolist (DtSrResult * dittolist, char *prefix)
{
    DtSrResult     *dit;
    char            datebuf[48];
    int             maxcount = 10;

    if (prefix == NULL)
	prefix = "HITLIST";
    if (dittolist == NULL) {
	fprintf (aa_stderr, "%s Dittolist is empty.\n", prefix);
	return;
    }
    fprintf (aa_stderr, "%s First %d hits on dittolist at %p:\n",
	prefix, maxcount, dittolist);
    for (dit = dittolist; dit != NULL && maxcount-- > 0; dit = dit->link) {
	if (dit->objdate == 0)
	    strcpy (datebuf, "0");
	else
	    strftime (datebuf, sizeof (datebuf), "%y/%m/%d",
		objdate2tm (dit->objdate));
	fprintf (aa_stderr,
	    " dbn=%d dba=%d:%ld prox=%d sz=%ld date=%s key='%s'\n",
	    dit->dbn, dit->dba >> 24, (long)dit->dba & 0xffffffL,
	    dit->proximity, (long)dit->objsize,
	    datebuf, dit->reckey);
	if (dit->abstractp)
	    if (dit->abstractp[0] != 0)
		fprintf (aa_stderr, "    abstract='%.50s'\n", dit->abstractp);
    }
    return;
}  /* print_dittolist() */

/************************************************/
/*                                              */
/*              get_hitlist_text                */
/*                                              */
/************************************************/
/* Converts dittolist into a single block of clean ascii text
 * for use as a hitlist, or for printing out to hardcopy.
 * Wraps lines intelligently to ensure no line is greater than maxlen.
 * If maxlen == 0, no wrapping is performed.
 * Returns static pointer to dynamically allocated buffer--
 * if permanent copy is desired, caller must copy text
 * to his own buffer before next call.
 */
char           *get_hitlist_text (int maxlen)
{
    static char    *text = NULL;
    int             sofar;
    char           *src, *targ, *eol;
    DtSrResult     *dit;
    char            sprintbuf[80];
    size_t          mallocsz;

    if (usrblk.dittocount <= 0L) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_misc, 96,
		"%s Hitlist is empty."), PROGNAME"96");
	DtSearchAddMessage (sprintbuf);
	return NULL;
    }
    if (maxlen > 0 && maxlen < DtSrMAX_DB_KEYSIZE + 7) {
	sprintf (sprintbuf, PROGNAME "97 maxlen = %d is too small.", maxlen);
	DtSearchAddMessage (sprintbuf);
	return NULL;
    }

/* Allocate enough memory for each item in ditto list */
    mallocsz = usrblk.dittocount *
	(usrblk.abstrbufsz + DtSrMAX_DB_KEYSIZE + 80L);
    if (text != NULL)
	free (text);
    text = austext_malloc (mallocsz, PROGNAME "103", NULL);

/* Loop thru ditto list, creating text out of hitlist */
    targ = text;
    for (dit = usrblk.dittolist; dit != NULL; dit = dit->link) {
	if (maxlen == 0) {
	    sprintf (targ, "%5d %-*s %s\n%n",
		dit->proximity,
		DtSrMAX_DB_KEYSIZE,
		dit->reckey,
		dit->abstractp,
		&sofar);
	    targ += sofar;
	}
	else	/* (maxlen > 0) */
	    /*
	     *  We may have more text than can fit on one line.
	     * Wrap lines to fit within maxlen. 
	     */
	{
	    eol = targ + maxlen;
	    sprintf (targ, "%5d %-*s %n",
		dit->proximity,
		DtSrMAX_DB_KEYSIZE,
		dit->reckey,
		&sofar);
	    targ += sofar;
	    src = dit->abstractp;
	    for (;;) {
		while (targ < eol && *src != 0)
		    *targ++ = *src++;
		*targ++ = '\n';
		if (*src == 0)
		    break;
		eol = targ + maxlen;
		strcpy (targ, "         ");
		targ += 9;
	    }
	}	/* end else (maxlen > 0) */
    }	/* end loop on dittolist */

    *targ = 0;	/* ...I don't know about you, but I always forget
		 * this */

    if (usrblk.debug & USRDBG_UTIL)
	fprintf (aa_stderr, PROGNAME "160 "
	    "get_hitlist_text(): mallocsz=%ld textlen=%ld\n",
	    mallocsz, strlen (text));
    return text;
}  /* get_hitlist_text() */


/****************************************/
/*                                      */
/*         print_usrblk_record          */
/*                                      */
/****************************************/
/* dumps out usrblk 'record' fields for debugging */
void	print_usrblk_record (char *prefix)
{
    fprintf (aa_stderr, "%s usrblk_record(): dba=%ld:%ld objkey='%s'\n"
	"  notesp=%p clearlen=%d cleartxt='%.30s'\n"
	"  #hitw=%d hitw=%p abstr='%.24s'\n",
	prefix,
	(long)usrblk.dba >> 24, (long)usrblk.dba & 0xffffffL,
	usrblk.objrec.or_objkey,
	usrblk.notes,
	(int)usrblk.clearlen,
	NULLORSTR (usrblk.cleartext),
	(int)usrblk.hitwcount,
	usrblk.hitwords,
	NULLORSTR (usrblk.abstrbuf));
    return;
}  /* print_usrblk_record() */


/****************************************/
/*                                      */
/*         clear_usrblk_record          */
/*                                      */
/****************************************/
/* Clears and initializes all USRBLK fields that
 * represent an opera record from vista, specifically
 * objrec, notes, textblobs, clearlen, and cleartext.
 * This function MUST NOT ALTER dba, hitwords, and hitwcount,
 * because many callers require these things to remain untouched.
 * If usrblk.objrec.or_objkey[0] == '\0',
 * UI must presume that all record fields are invalid
 * including usrblk.objfzkey and usrblk.abstract.
 */
void            clear_usrblk_record (void)
{
    usrblk.clearlen = 0;
    if (usrblk.cleartext != NULL) {
	free (usrblk.cleartext);
	usrblk.cleartext = NULL;
    }
    free_llist (&usrblk.notes);
    usrblk.objrec.or_objkey[0] = '\0';
    if (usrblk.abstrbufsz > 0)
	usrblk.abstrbuf[0] = 0;
    if (usrblk.debug & USRDBG_RETRVL)
	print_usrblk_record (PROGNAME "600 clear: ");
    return;
}  /* clear_usrblk_record() */


/************************************************/
/*                                              */
/*               clear_hitwords                 */
/*                                              */
/************************************************/
/* clears hitwcount and array in usrblk */
void            clear_hitwords (void)
{
    usrblk.hitwcount = 0;
    if (usrblk.hitwords != NULL) {
	free (usrblk.hitwords);
	usrblk.hitwords = NULL;
    }
    return;
}  /* clear_hitwords() */


/*********************** DTSRUTIL.C ***********************/
