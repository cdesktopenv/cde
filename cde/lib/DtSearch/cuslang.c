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

