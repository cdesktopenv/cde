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
 *   FUNCTIONS: fillnew_wordrec
 *		find_keyword
 *		read_wordstr
 *		write_wordstr
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* VSTFUNCT.C **********************************
 * $XConsortium: vstfunct.c /main/6 1996/08/12 13:20:01 cde-ibm $
 * Performs vista KEYFIND, RECREAD, RECWRITE, and FILLNEW functions
 * on word database files without caller having to know
 * whether word is short, long, or huge.
 * Remember: all word lengths are 1 byte shorter than the field size,
 * plus 1 byte for the terminating \0.
 *
 * $Log$
 * Revision 2.2  1995/10/26  14:12:32  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:23:13  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  19:34:07  miker
 * Minor name changes for DtSearch.
 */
#include "SearchP.h"
#include "vista.h"

#define	PROGNAME	"VSTFUNCT"

static struct or_swordrec Swordbuf;
static struct or_lwordrec Lwordbuf;


/************************/
/*			*/
/*	find_keyword	*/
/*			*/
/************************/
/* Sets vista "curr record" to correct word record.
 * Usually performed prior to vista read or write function.
 * CALLER MUST CHECK DB_STATUS.
 */
void            find_keyword (char *cur_word, int vista_num)
{
    static size_t   len;
    static long     keyfield;

    len = strlen (cur_word);
    if (len < sizeof (Swordbuf.or_swordkey))
	keyfield = OR_SWORDKEY;
    else if (len < sizeof (Lwordbuf.or_lwordkey))
	keyfield = OR_LWORDKEY;
    else
	keyfield = OR_HWORDKEY;
    KEYFIND (PROGNAME "24", keyfield, cur_word, vista_num);
    return;
}  /* find_keyword() */



/********************************/
/*				*/
/*	   read_wordstr		*/
/*				*/
/********************************/
/* Performs vista RECREAD on curr word record.
 * CALLER SHOULD CHECK DB_STATUS.
 */
void            read_wordstr (struct or_hwordrec * glob_word, int vista_num)
{
    static size_t   len;

    len = strlen (glob_word->or_hwordkey);
    if (len < sizeof (Swordbuf.or_swordkey)) {
	RECREAD (PROGNAME "61", &Swordbuf, vista_num);
	if (db_status != S_OKAY)
	    return;
	strncpy (glob_word->or_hwordkey, Swordbuf.or_swordkey,
	    DtSrMAXWIDTH_HWORD);
	glob_word->or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
	glob_word->or_hwoffset =	ntohl (Swordbuf.or_swoffset);
	glob_word->or_hwfree =		ntohl (Swordbuf.or_swfree);
	glob_word->or_hwaddrs =		ntohl (Swordbuf.or_swaddrs);
    }
    else if (len < sizeof (Lwordbuf.or_lwordkey)) {
	RECREAD (PROGNAME "69", &Lwordbuf, vista_num);
	if (db_status != S_OKAY)
	    return;
	strncpy (glob_word->or_hwordkey, Lwordbuf.or_lwordkey,
	    DtSrMAXWIDTH_HWORD);
	glob_word->or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
	glob_word->or_hwoffset =	ntohl (Lwordbuf.or_lwoffset);
	glob_word->or_hwfree =		ntohl (Lwordbuf.or_lwfree);
	glob_word->or_hwaddrs =		ntohl (Lwordbuf.or_lwaddrs);
    }
    else {
	RECREAD (PROGNAME "78", glob_word, vista_num);
	glob_word->or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
	NTOHL (glob_word->or_hwoffset);
	NTOHL (glob_word->or_hwfree);
	NTOHL (glob_word->or_hwaddrs);
    }
    return;
}  /* read_wordstr() */


/********************************/
/*				*/
/*	   write_wordstr	*/
/*				*/
/********************************/
/* performs vista RECWRITE on curr word record.
 * CALLER MUST CHECK DB_STATUS.
 */
void            write_wordstr (struct or_hwordrec * glob_word, int vista_num)
{
    static size_t   len;

    len = strlen (glob_word->or_hwordkey);

    if (len < sizeof (Swordbuf.or_swordkey)) {
	strcpy (Swordbuf.or_swordkey, glob_word->or_hwordkey);
	Swordbuf.or_swoffset =	htonl (glob_word->or_hwoffset);
	Swordbuf.or_swfree =	htonl (glob_word->or_hwfree);
	Swordbuf.or_swaddrs =	htonl (glob_word->or_hwaddrs);
	RECWRITE (PROGNAME "102", &Swordbuf, vista_num);
    }
    else if (len < sizeof (Lwordbuf.or_lwordkey)) {
	strcpy (Lwordbuf.or_lwordkey, glob_word->or_hwordkey);
	Lwordbuf.or_lwoffset =	htonl (glob_word->or_hwoffset);
	Lwordbuf.or_lwfree =	htonl (glob_word->or_hwfree);
	Lwordbuf.or_lwaddrs =	htonl (glob_word->or_hwaddrs);
	RECWRITE (PROGNAME "111", &Lwordbuf,
	    vista_num);
    }
    else {
	if (len >= DtSrMAXWIDTH_HWORD) {
	    printf ("\n" PROGNAME "124 Program Error Abort, "
		"word too long:\n  '%s'\n",
		glob_word->or_hwordkey);
	    DtSearchExit (24);
	}
	HTONL (glob_word->or_hwoffset);
	HTONL (glob_word->or_hwfree);
	HTONL (glob_word->or_hwaddrs);
	RECWRITE (PROGNAME "115", glob_word, vista_num);
	NTOHL (glob_word->or_hwoffset);
	NTOHL (glob_word->or_hwfree);
	NTOHL (glob_word->or_hwaddrs);
    }
    return;
}  /* write_wordstr() */


/****************************************/
/*					*/
/*	     fillnew_wordrec		*/
/*					*/
/****************************************/
/* The input record is always a word in a 'huge' structure.
 * This function performs vista FILLNEW on that word,
 * but into correct sized word rec.
 * Formerly this function was called put_new_word().
 * CALLER MUST CHECK DB_STATUS.
 */
void            fillnew_wordrec (struct or_hwordrec * glob_word, int vista_num)
{
    static size_t   len;

    len = strlen (glob_word->or_hwordkey);
    if (len < sizeof (Swordbuf.or_swordkey)) {
	strcpy (Swordbuf.or_swordkey, glob_word->or_hwordkey);
	Swordbuf.or_swoffset =	htonl (glob_word->or_hwoffset);
	Swordbuf.or_swfree =	htonl (glob_word->or_hwfree);
	Swordbuf.or_swaddrs =	htonl (glob_word->or_hwaddrs);
	FILLNEW (PROGNAME "137", OR_SWORDREC, &Swordbuf, vista_num);
    }
    else if (len < sizeof (Lwordbuf.or_lwordkey)) {
	strcpy (Lwordbuf.or_lwordkey, glob_word->or_hwordkey);
	Lwordbuf.or_lwoffset =	htonl (glob_word->or_hwoffset);
	Lwordbuf.or_lwfree =	htonl (glob_word->or_hwfree);
	Lwordbuf.or_lwaddrs =	htonl (glob_word->or_hwaddrs);
	FILLNEW (PROGNAME "147", OR_LWORDREC, &Lwordbuf, vista_num);
    }
    else {
	if (len >= DtSrMAXWIDTH_HWORD) {
	    printf ("\n" PROGNAME "168 Program Error Abort, "
		"word too long:\n  '%s'\n",
		glob_word->or_hwordkey);
	    DtSearchExit (68);
	}
	HTONL (glob_word->or_hwoffset);
	HTONL (glob_word->or_hwfree);
	HTONL (glob_word->or_hwaddrs);
	FILLNEW (PROGNAME "151", OR_HWORDREC, glob_word, vista_num);
	NTOHL (glob_word->or_hwoffset);
	NTOHL (glob_word->or_hwfree);
	NTOHL (glob_word->or_hwaddrs);
    }
    return;
}  /* fillnew_wordrec() */

/********************* VSTFUNCT.C **********************************/
