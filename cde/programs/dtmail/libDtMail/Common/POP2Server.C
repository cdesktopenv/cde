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
 *	$TOG: POP2Server.C /main/6 1998/11/10 17:08:56 mgreess $
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
#include <stdlib.h>

#include <DtMail/DtMailServer.hh>
#include <DtMail/IO.hh>

POP2Server::POP2Server(
		char			*folder,
		DtMail::Session		*session,
		DtMail::MailBox		*mailbox,
		DtMailAppendCallback	append_mailbox_cb,
		void			*append_mailbox_cb_data)
: DtMailServer(folder, session, mailbox,
	       append_mailbox_cb, append_mailbox_cb_data)
{
    _pound_arg = 0;
    _equal_arg = 0;
}

POP2Server::~POP2Server()
{
}


//
// Request nth message.
//
DTMailError_t
POP2Server::ptrans_retrieve_start(int msg, int *lenp)
{
    DTMailError_t	ok;

    *lenp = 0;
    ok = do_transaction("READ %d", msg);
    if (DTME_NoError != ok) return ok;

    *lenp = _equal_arg;
    ok = do_send("RETR");
    return ok;
}

//
// Apply for connection authorization.
//
DTMailError_t
POP2Server::ptrans_authorize(char*)
{
    DTMailError_t	ok;
    ok = do_transaction("HELO %s %s", _username, _password);
    if (DTME_NoError != ok) return DTME_MailServerAccess_AuthorizationFailed;
    return DTME_NoError;
}

//
// Get range of messages to be fetched.
//
DTMailError_t
POP2Server::ptrans_fldstate_read(int *countp, int *newp)
{
    //
    // We should have picked up a count of messages in the user's
    // default inbox from the pop2_getauth() response.
    //
    if (_pound_arg == -1)
	return DTME_MailServerAccess_Error;

    // Maybe the user wanted a non-default folder.
    if (! is_inbox())
    {
	DTMailError_t	ok;
	
	ok = do_transaction("FOLD %s", _folder);
	if (DTME_NoError != ok) return ok;
	if (_pound_arg == -1) return DTME_MailServerAccess_Error;
    }

    *countp = _pound_arg;
    *newp = -1;

    return DTME_NoError;
}

//
// Capture the sizes of all messages.
//
DTMailError_t
POP2Server::ptrans_msgsizes(int countp, int *sizes)
{
    memset((char*) sizes, 0, countp * sizeof(int));
    return DTME_NoError;
}

//
// Is the given message old?
//
int
POP2Server::ptrans_msgisold(int)
{
    return 0;
}

//
// Parse POP2 command response.
//
DTMailError_t
POP2Server::ptrans_parse_response (char *argbuf)
{
    DTMailError_t	ok;
    char		buf[DTMAS_POPBUFSIZE+1];

    _pound_arg = _equal_arg = -1;
    if (SockGets(buf, sizeof(buf), _sockfp))
    {
	if (buf[strlen(buf)-1] == '\n')
	  buf[strlen(buf)-1] = '\0';
	if (buf[strlen(buf)-1] == '\r')
	  buf[strlen(buf)-1] = '\r';

	if (_protologging)
	  _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);

	if (buf[0] == '+')
	  ok = DTME_NoError;
	else if (buf[0] == '#')
	{
	    _pound_arg = atoi(buf+1);
	    ok = DTME_NoError;
	}
	else if (buf[0] == '=')
	{
	    _equal_arg = atoi(buf+1);
	    ok = DTME_NoError;
	}
	else if (buf[0] == '-')
	{
	    _logger.logError(DTM_FALSE, "Protocol Error reading response");
	    ok = DTME_MailServerAccess_Error;
	}
	else
	{
	    _logger.logError(DTM_FALSE, "Protocol Violation reading response");
	    ok = DTME_MailServerAccess_ProtocolViolation;
	}

	if (argbuf != NULL)
	  strcpy(argbuf,buf);
    }
    else 
    {
	_logger.logError(DTM_FALSE, "Socket Error reading response");
        ok = DTME_MailServerAccess_SocketIOError;
    }

    return ok;
}

//
// retrieve messages using POP2.
//
void
POP2Server::retrieve_messages(DtMailEnv &error)
{
    DtMailServer::retrieve_messages(error);
}

//
// Send acknowledgement for message data.
//
DTMailError_t
POP2Server::ptrans_retrieve_end(int)
{
    if (_removeafterdelivery)
      return do_transaction("ACKD");
    else
      return do_transaction("ACKS");
}
