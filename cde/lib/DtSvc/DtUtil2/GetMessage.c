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
