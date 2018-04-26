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
 *+SNOTICE
 *
 *	$TOG: APOPServer.C /main/5 1998/11/10 17:06:52 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993, 1995, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
 
#include  <DtMail/DtMailServer.hh>
#include  "md5.h"

APOPServer::APOPServer(
		char			*folder,
		DtMail::Session		*session,
		DtMail::MailBox		*mailbox,
		DtMailAppendCallback	append_mailbox_cb,
		void			*append_mailbox_cb_data)
: POP3Server(folder, session, mailbox,
	     append_mailbox_cb, append_mailbox_cb_data)
{
}

APOPServer::~APOPServer()
{
}

//
// Apply for connection authorization.
//
DTMailError_t
APOPServer::ptrans_authorize(char *greeting)
{
    static const char
		*pname = "APOPServer::ptrans_authorize";
    static char	ascii_digest [33];
    char	*start,*end;
    char	*msg;
    DTMailError_t ok;

    // Build MD5 digest from greeting timestamp + password.
    // Find start of timestamp.
    for (start = greeting;  *start != 0 && *start != '<';  start++)
      continue;

    if (*start == 0)
    {
        _logger.logError(
			DTM_FALSE,
			"%s: APOP timestamp not found in greeting",
			pname);
        return DTME_MailServerAccess_AuthorizationFailed;
    }

    // Find end of timestamp.
    for (end = start;  *end != 0  && *end != '>';  end++)
      continue;
    if (*end == 0 || end == start + 1)
    {
        _logger.logError(
			DTM_FALSE,
			"%s: APOP timestamp not found in greeting",
			pname);
        return DTME_MailServerAccess_AuthorizationFailed;
    }
    else
      *++end = '\0';

    {
        int		i;
        MD5_CTX		context;
        unsigned char	digest[16];

        // Copy timestamp and password into digestion buffer.
        msg = (char*) malloc((end-start+1) + strlen(_password) + 1);
        strcpy(msg, start);
        strcat(msg, _password);

        MD5Init(&context);
        MD5Update(&context, (unsigned char*) msg, strlen(msg));
        
        for (i = 0;  i < 16;  i++) 
          sprintf(ascii_digest+2*i, "%02x", digest[i]);

        free(msg);
    }

    ok = do_transaction("APOP %s %s", _username, ascii_digest);
    if (DTME_NoError != ok) return DTME_MailServerAccess_AuthorizationFailed;
    return DTME_NoError;
}
