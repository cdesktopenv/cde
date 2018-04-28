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
 *   FUNCTIONS: append_ext
 *		replace_ext
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1990,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** APNDEXT.C *************************
 * $XConsortium: apndext.c /main/5 1996/05/07 13:24:29 drk $
 * append_ext:
 * 1. Concatenates a file extension to a passed filename,
 * 	unless an extension is already provided in the name.
 * 	Moves the concatenated string to the passed buffer.
 * 2. Checks for filename buffer overflow.
 * 
 * replace_ext:
 * Same thing, except that if a dotted extension is already
 * provided, it REPLACES the last extension with the passed one. 
 *
 * $Log$
 * Revision 2.2  1995/10/25  22:25:29  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  18:08:26  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 */
#include "SearchP.h"
#include <string.h>

/********************************************************/
/*							*/
/*	               append_ext			*/
/*							*/
/********************************************************/
void append_ext(char *buffer,		/* output assy area */
		int  buflen,		/* length of buffer */
		char *fname,		/* input prefix (file name) */
		char *fext)		/* input suffix (file ext .XXX) */
{
    char	*endptr, *slashptr, *dotptr;

    strncpy (buffer, fname, buflen);
    *(buffer + buflen - 5) = '\0';	/* room for ".xxx" at end */

    /* Look for FINAL dot and FINAL slash (directory delim) */
    dotptr = slashptr = NULL;
    for (endptr = buffer;  *endptr != 0;  endptr++) {
	if (*endptr == LOCAL_SLASH)
	    slashptr = endptr;
	else if (*endptr == '.')
	    dotptr = endptr;
    }

    /* If extension already exists (ie there's a dot in the name),
     * and its past any dir delim, if any,
     * return immediately:  the dot is a valid extension marker.
     */
    if (dotptr) {	/* extension already exists */
	if (slashptr == NULL)
	    return;
	else if (slashptr < dotptr)
	    return;
    }

    /* Extension does not exist or its in earlier dir name */
    strcpy (endptr, fext);
    return;
} /* append_ext() */


/********************************************************/
/*							*/
/*	              replace_ext			*/
/*							*/
/********************************************************/
void	replace_ext (char *buffer,	/* output assy area */
		int  buflen,		/* length of buffer */
		char *fname,		/* input prefix (file name) */
		char *fext)		/* input suffix (file ext .XXX) */
{
    int		fnamelen;
    char	*targ;

    strncpy(buffer, fname, buflen - 1);
    *(buffer + buflen - 1) = '\0';		/* just in case */
    fnamelen = strlen(buffer);
    if ((targ = strrchr(buffer, '.')) == NULL) {
	strncpy (buffer + fnamelen, fext, buflen - fnamelen - 1);
	*(buffer + buflen - 1) = '\0';	/* again just in case */
    }
    else {
	while (*targ != 0 && *fext != 0) *targ++ = *fext++;
	if (*fext == 0) *targ = 0;
    }
    return;
} /* replace_ext() */

/*************************** APNDEXT.C ****************************/
