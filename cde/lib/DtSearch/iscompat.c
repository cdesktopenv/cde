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
 *   FUNCTIONS: false
 *		is_compatible_version
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
/*********************** ISCOMPAT.C ************************
 * $XConsortium: iscompat.c /main/7 1996/11/21 19:50:44 drk $
 * November 1993.
 * Verifies version number compatibility between caller and this program.
 *
 * $Log$
 * Revision 2.2  1995/10/25  17:46:54  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  20:55:37  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.4  1995/09/05  18:10:55  miker
 * Name changes for DtSearch.
 */
#include "SearchP.h"
#include <stdlib.h>

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

/****#define DEBUG_ISCOMPAT****/

#ifdef DEBUG_ISCOMPAT
static int false (int n) { printf("iscompat=%d\n",n);  return FALSE; }
#else
#define false(n)	FALSE
#endif

/************************************************/
/*						*/
/*	       is_compatible_version		*/
/*						*/
/************************************************/
/* Returns TRUE if the caller's 'version' and 'revision' numbers,
 * ie the first two numbers of a standard "v.r.m" or "v.r"
 * version string, are between VERSCONST and the version
 * compiled for this program, ie the constant AUSAPI_VERSION.
 * VERSCONST is also expected to be a #define constant,
 * either SCHEMA_VERSION, when the last schema change occurred,
 * or PROTOCOL_VERSION, when the protocol between the ui and
 * the engine changed.
 * Returns FALSE if caller's version is not within that range.
 * Uses strtok()!
 */
int             is_compatible_version (char *callers_version, char *VERSCONST)
{
char            safebuf [24];
char           *ptr;
int             fuzzy_v, fuzzy_r, his_v, his_r, my_v, my_r;
_Xstrtokparams	strtok_buf;

    if (callers_version == NULL)
	return false(1);
    if (*callers_version == '\0')
	return false(2);
    strncpy (safebuf, callers_version, sizeof(safebuf));
    safebuf[sizeof(safebuf) - 1] = 0;
    if ((ptr = _XStrtok (safebuf, ".", strtok_buf)) == NULL)
	return false(3);
    if ((his_v = atoi (ptr)) == 0)
	if (*ptr != '0')
	    return false(4);
    if ((ptr = _XStrtok (NULL, ".", strtok_buf)) == NULL)
	return false(5);
    if ((his_r = atoi (ptr)) == 0)
	if (*ptr != '0')
	    return false(6);

    strncpy (safebuf, VERSCONST, sizeof(safebuf));
    safebuf[sizeof(safebuf) - 1] = 0;
    if ((ptr = _XStrtok (safebuf, ".", strtok_buf)) == NULL)
	return false(7);
    if ((my_v = atoi (ptr)) == 0)
	if (*ptr != '0')
	    return false(8);
    if ((ptr = _XStrtok (NULL, ".", strtok_buf)) == NULL)
	return false(9);
    if ((my_r = atoi (ptr)) == 0)
	if (*ptr != '0')
	    return false(10);

    strncpy (safebuf, AUSAPI_VERSION, sizeof(safebuf));
    safebuf[sizeof(safebuf) - 1] = 0;
    if ((ptr = _XStrtok (safebuf, ".", strtok_buf)) == NULL)
	return false(11);
    if ((fuzzy_v = atoi (ptr)) == 0)
	if (*ptr != '0')
	    return false(12);
    if ((ptr = _XStrtok (NULL, ".", strtok_buf)) == NULL)
	return false(13);
    if ((fuzzy_r = atoi (ptr)) == 0)
	if (*ptr != '0')
	    return false(14);

    if (his_v < my_v || fuzzy_v < his_v)
	return false(15);
    if (his_v == my_v && his_r < my_r)
	return false(16);
    if (fuzzy_v == his_v && fuzzy_r < his_r)
	return false(17);
    return TRUE;
}  /* is_compatible_version() */

/*********************** ISCOMPAT.C ************************/
