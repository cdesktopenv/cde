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
