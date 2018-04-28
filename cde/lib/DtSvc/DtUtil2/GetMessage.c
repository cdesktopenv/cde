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
/* $XConsortium: GetMessage.c /main/5 1996/06/21 17:22:13 ageorge $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         GetMessage.c
 **
 **   Project:      DT
 **
 **   Description:  This file contains the library source code to get
 **                 a localized message.
 **
 **
 **   (c) Copyright 1992 by Hewlett-Packard Company
 **
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <nl_types.h>
#include "DtSvcLock.h"

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif

/*****************************************************************************
 *
 * Function: _DtGetMessage
 *
 * Parameters:
 *
 *   char 	*filename -	Filename to open.
 *
 *   int	set -		The message catalog set number.
 *
 *   int	n - 		The message number.
 *
 *   char	*s -		The default message if the message is not
 *				retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 *
 *****************************************************************************/

char * 
_DtGetMessage(
        char *filename,
        int set,
        int n,
        char *s )
{
        char *msg;
	char *lang;
	nl_catd catopen();
	char *catgets();
	static int first = 1;
	static nl_catd nlmsg_fd;

	_DtSvcProcessLock();
	if ( first ) 
        {
		first = 0;
		nlmsg_fd = catopen(filename, NL_CAT_LOCALE);
	}
	msg=catgets(nlmsg_fd,set,n,s);
	_DtSvcProcessUnlock();
	return (msg);
}
