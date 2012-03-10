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

FILE	*aa_stderr =		stderr;
char	*aa_argv0 =		"<argv0>";
nl_catd	dtsearch_catd =		(nl_catd) -1;
nl_catd	austools_catd =		(nl_catd) -1;
void	*ausapi_msglist =	NULL;	/* really (LLIST *) */
