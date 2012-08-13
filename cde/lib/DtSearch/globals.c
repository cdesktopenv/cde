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
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/**************************** GLOBALS.C *************************
 * $XConsortium: globals.c /main/5 1996/05/07 13:34:49 drk $
 * March 1995.
 * Contains only the most global of DtSearch/AusText globals.
 * Used by everybody, including offlines, so must be kept
 * in a very small module, and reference as few headers as possible.
 *
 * $Log$
 * Revision 2.3  1995/10/26  15:19:28  miker
 * Added prolog.
 *
 * Revision 2.2  1995/10/02  20:32:00  miker
 * Comments cleanup only.
 *
 * Revision 2.1  1995/09/22  20:11:33  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.2  1995/09/05  17:53:52  miker
 * Added dtsearch_catd and _XOPEN_SOURCE for DtSearch.
 */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <stdio.h>
#include <nl_types.h>

FILE	*aa_stderr =		NULL;
char	*aa_argv0 =		"<argv0>";
nl_catd	dtsearch_catd =		(nl_catd) -1;
nl_catd	austools_catd =		(nl_catd) -1;
void	*ausapi_msglist =	NULL;	/* really (LLIST *) */
