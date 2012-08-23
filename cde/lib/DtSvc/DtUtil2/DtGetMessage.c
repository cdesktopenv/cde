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
/* $TOG: DtGetMessage.c /main/10 1998/07/30 12:12:25 mgreess $ */
/*
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 */
/******************************************************************************
 *
 * File Name: DtGetMessage.c
 *
 *  Contains the function for getting localized strings.
 *
 *****************************************************************************/

#ifndef NO_MESSAGE_CATALOG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nl_types.h>
#include "DtSvcLock.h"

/*****************************************************************************
 *
 * Function: Dt11GetMessage
 *
 * Parameters:
 *
 *   char       *filename -     Filename to open.
 *
 *   int        set -           The message catalog set number.
 *
 *   int        n -             The message number.
 *
 *   char       *s -            The default message if the message is not
 *                              retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 *
 *****************************************************************************/

char *
Dt11GetMessage(
	char *filename,
	int set,
	int n,
	char *s)
{
        char *msg;
        static int first = 1;
        static nl_catd nlmsg_fd;
	static char *nlmsg_filename = NULL;

	_DtSvcProcessLock();
        if ( NULL == nlmsg_filename || 0 != strcmp(nlmsg_filename, filename) )
        {
		nlmsg_fd = catopen(filename, NL_CAT_LOCALE);
		if (nlmsg_filename)
		{
		    free(nlmsg_filename);
		    nlmsg_filename = NULL;
		}
		nlmsg_filename = strdup(filename);
        }
        msg=catgets(nlmsg_fd,set,n,s);
	_DtSvcProcessUnlock();
        return (msg);
}
#endif
