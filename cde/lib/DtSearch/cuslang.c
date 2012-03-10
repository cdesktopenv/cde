/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: load_custom_language
 *		unload_custom_language
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
/************ CUSLANG.C **********
 * $XConsortium: cuslang.c /main/4 1996/05/07 13:26:29 drk $
 * August 1995.
 * Dummy load_custom_language() and unload_custom_language().
 * Can be overridden by user's own custom language functions
 * by linking in a module with these functions prior to
 * linking in this module.
 * Both are referenced in loadlang.c.
 *
 * $Log$
 * Revision 2.2  1995/10/26  15:33:30  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:13:59  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 */
#include "SearchP.h"
int	load_custom_language (DBLK *dblk, DBLK *dblist)
{
    char	msgbuf [1024];
    sprintf (msgbuf,
	"CUSLANG01 Database '%s': Language number %d\n"
	"  is unsupported and user has not provided a\n"
	"  load_custom_language() function to handle it.",
	dblk->name, dblk->dbrec.or_language);
    DtSearchAddMessage (msgbuf);
    return FALSE;
}

void	unload_custom_language (DBLK *dblk)
/* If user provides load() but not unload(),
 * then this dummy will almost certainly cause
 * memory leaks at REINIT.
 */
{ return; }

