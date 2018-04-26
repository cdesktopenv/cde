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
 *   FUNCTIONS: delete_whitespace
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
/***************************** DELSPACE.C *****************************
 * $XConsortium: delspace.c /main/5 1996/05/07 13:28:17 drk $
 * Removes trailing whitespace from a character buffer 
 * (such as a line read in with fgets).
 * If the last char in the original buffer was \n, 
 * then the last char in the resulting string will be \n.
 * Replaces all internal control and extended graphics
 * characters (except the last \n) with a blank.
 * Does nothing to empty strings ("\0") and strings
 * beginning with a form feed ("\f....").
 * All Conversions are done inplace, i.e. within 
 * the same buffer passed to the function.
 *
 * $Log$
 * Revision 2.3  1995/12/01  16:05:37  miker
 * No longer overlays tabs with a space char.
 *
 * Revision 2.2  1995/10/25  22:06:36  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:46:16  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.2  1995/08/31  22:27:03  miker
 * Minor changes for DtSearch.
 */
#include "SearchP.h"
#include <ctype.h>

void            delete_whitespace (char *linebuf)
{
    static char    *ptr, *endptr;
    static int      has_linefeed;

    /* do nothing if empty or ff string */
    if (*linebuf == '\0' || *linebuf == '\f')
	return;

    ptr = linebuf + strlen (linebuf) - 1;
    has_linefeed = (*ptr == '\n');
    while (!isgraph (*ptr) && ptr >= linebuf)
	ptr--;

    endptr = ptr + 1;	/* endptr = just past last char */
    for (ptr = linebuf; ptr < endptr; ptr++)
	if (!isprint(*ptr) && *ptr != '\t')
	    *ptr = ' ';

    if (has_linefeed)
	*endptr++ = '\n';
    *endptr = 0;
    return;
}

/***************************** DELSPACE.C *****************************/
