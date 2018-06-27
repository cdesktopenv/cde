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
 *   FUNCTIONS: bmhcore
 *		bmhtable_build
 *		bmstrstr
 *		main
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************* BMSTRSTR.C ****************************
 * $TOG: bmstrstr.c /main/6 1998/04/17 11:25:23 mgreess $
 * Original module named fastsearch.c
 * and included colocation string search functions.
 * Modification of Boyer-Moore-Horspool algorithm,
 * Sec 10.5.2 of Information Retrieval, Frakes and Baeza-Yates, editors.
 * Provides a generalized boyer-moore
 * strstr() function.  The table used in the BMH algorithm is
 * generated in a separate function to improve efficiency when 
 * looking for the same substring pattern in multiple text strings.
 * The 'length' arguments can be passed if known, or passed as
 * strlen(xxx) if not known.  HOWEVER the string arrays MUST be at
 * least 1 char larger then strlen() says ('cause we insert a \0).
 * This whole thing has been coded for SPEED!
 *
 * $Log$
 * Revision 2.2  1995/10/26  15:37:42  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:10:39  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.2  1995/08/31  22:12:05  miker
 * Minor changes for DtSearch.
 */
#include "SearchP.h"
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#ifndef _AIX
#define __strcmp	strcmp
#endif

/*********#define TEST_BMSTRSTR**************/

/****************************************/
/*					*/
/*	      bmhtable_build		*/
/*					*/
/****************************************/
/* Builds the table for search substring 'pattern'
 * used by the BMH core search algorithm.
 * 'patlen' is the string length of 'pattern'.
 * The caller defines and passes 'bmhtable',
 * an array of long integers (size_t) of size MAX_BMHTAB.
 * This function initializes bmhtable for later search call.
 */
void            bmhtable_build (
                    unsigned char *pattern,
                    size_t patlen,
                    size_t * bmhtable)
{
    int             k;

    for (k = 0; k < MAX_BMHTAB; k++)
	bmhtable[k] = patlen;
    patlen--;
    for (k = 0; k < patlen; k++)
	bmhtable[pattern[k]] = (patlen - k);
    return;
}  /* bmhtable_build() */


/****************************************/
/*					*/
/*		 bmhcore		*/
/*					*/
/****************************************/
/* Performs 'core' BMH search after bmhtable is built.
 * Returns ptr to first occurrence of pattern in text or NULL.
 * WARNING! IF EITHER txtlen OR patlen <= 0, THIS FUNCTION WILL CRASH!!!
 * Pattern and patlen MUST BE identical with those used to build
 * bmhtable.
 */

char           *bmhcore (
                    unsigned char	*text,
                    size_t		txtlen,
                    unsigned char	*pattern,
                    size_t		patlen,
                    size_t		*bmhtable)
{
    unsigned char
		lastchar = pattern[patlen - 1];
    unsigned char
		textchar;
    unsigned char
		*cp;
    unsigned char
		*last;
    int		savechar;
    int		savechar2;
    unsigned char
		*result = NULL;

    /* Terminate pattern with a char we KNOW is not in text.
     * Note that this requires string to have room for \0 at end.
     */
    savechar = pattern[patlen];
    pattern[patlen] = '\0';

    last = text + txtlen;
    for (cp = text + patlen - 1; cp < last; cp += bmhtable[textchar]) {
	/*
	 * Check if last character matches. If it doesn't, no need
	 * to check any further. 
	 */
	if ((textchar = *cp) != lastchar)
	    continue;
	savechar2 = cp[1];
	cp[1] = '\0';
	if (!__strcmp ((char *) (cp + 1 - patlen), (char *) pattern))
	    result = cp + 1 - patlen;
	cp[1] = savechar2;
	if (result)
	    break;
    }
    pattern[patlen] = savechar;	/* restore last char */
    return (char *) result;
}  /* bmhcore() */


/****************************************/
/*					*/
/*		 bmstrstr		*/
/*					*/
/****************************************/
/* Search in text [1..txtlen] for pattern [1..patlen].
 * Returns ptr to first occurrence of pattern, or NULL.
 */
char           *bmstrstr (
                    unsigned char *text,
                    size_t txtlen,
                    unsigned char *pattern,
                    size_t patlen)
{
    size_t          bmhtable[MAX_BMHTAB];

    bmhtable_build (pattern, patlen, bmhtable);
    return bmhcore (text, txtlen, pattern, patlen, bmhtable);
}  /* bmstrstr() */


#ifdef TEST_BMSTRSTR	/* for test only */
#include <sys/stat.h>
/****************************************/
/*					*/
/*		   main			*/
/*					*/
/****************************************/
/* tests bmstrstr() against standard strstr() on a specified file */
main ()
{
    FILE           *f;
    struct stat     statbuf;
    size_t          fsize = 0L;
    char            fname[BUFSIZ+1];
    char            pattern[MAXPATHLEN+1];
    char           *readbuf = NULL;
    char           *ptr;

MAIN_LOOP:
    printf ("\nEnter a filename (Ctrl-C quits) > ");

    *fname = '\0';
    fgets (fname, sizeof(fname), stdin);
    if (strlen(fname) && fname[strlen(fname)-1] == '\n')
      fname[strlen(fname)-1] = '\0';

    if ((f = fopen (fname, "r")) == NULL) {
	printf ("Can't open '%s': %s\n", fname, strerror (errno));
	goto MAIN_LOOP;
    }

    fstat (fileno (f), &statbuf);
    if (fsize > statbuf.st_size) {
	free (readbuf);
	readbuf = NULL;
    }
    fsize = statbuf.st_size;
    if (readbuf == NULL)
	readbuf = malloc (fsize + 64L);

    fread (readbuf, fsize, 1L, f);
    fclose (f);

    printf ("Enter a search pattern > ");

    *pattern = '\0';
    fgets (pattern, sizeof(pattern), stdin);
    if (strlen(pattern) && pattern[strlen(pattern)-1] == '\n')
      pattern[strlen(pattern)-1] = '\0';

    ptr = bmstrstr (readbuf, fsize, pattern, strlen (pattern));
    if (ptr == NULL)
	puts ("bmstrstr: Pattern not found.");
    else
	printf ("bmstrstr: Pattern found at offset %ld.\n", ptr - readbuf);

    ptr = strstr (readbuf, pattern);
    if (ptr == NULL)
	puts ("strstr:   Pattern not found.");
    else
	printf ("strstr:   Pattern found at offset %ld.\n", ptr - readbuf);

    goto MAIN_LOOP;
}  /* main() test program */

#endif

/************************* BMSTRSTR.C ****************************/
