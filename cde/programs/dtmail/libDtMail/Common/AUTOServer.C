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
 *	$TOG: AUTOServer.C /main/6 1998/11/10 17:07:17 mgreess $
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

#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>

#include  <DtMail/DtMailServer.hh>

AUTOServer::AUTOServer(
		char			*folder,
		DtMail::Session		*session,
		DtMail::MailBox		*mailbox,
		DtMailAppendCallback	append_mailbox_cb,
		void			*append_mailbox_cb_data)
: DtMailServer(folder, session, mailbox,
	       append_mailbox_cb, append_mailbox_cb_data)
{
    _server = NULL;
}

AUTOServer::~AUTOServer()
{
}


//
// Set delete flag for given message.
//
DTMailError_t
AUTOServer::ptrans_delete(int number)
{
    static char	*pname = "AUTOServer::ptrans_delete";

    if (_server) return _server->ptrans_delete(number);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Apply for connection authorization
//
DTMailError_t
AUTOServer::ptrans_authorize(char *buf)
{
    static char	*pname = "AUTOServer::ptrans_authorize";

    if (_server) return _server->ptrans_authorize(buf);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Expunge deleted messages
//
DTMailError_t
AUTOServer::ptrans_fldstate_expunge()
{
    static char	*pname = "AUTOServer::ptrans_fldstate_expunge";

    if (_server) return _server->ptrans_fldstate_expunge();

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Get range of messages to be fetched
//
DTMailError_t
AUTOServer::ptrans_fldstate_read(int *countp, int *newp)
{
    static char	*pname = "AUTOServer::ptrans_fldstate_read";

    if (_server) return _server->ptrans_fldstate_read(countp, newp);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Capture the sizes of all messages.
//
DTMailError_t
AUTOServer::ptrans_msgsizes(int count, int *sizes)
{
    static char	*pname = "AUTOServer::ptrans_msgsizes";

    if (_server) return _server->ptrans_msgsizes(count, sizes);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Is the given message old?
//
int
AUTOServer::ptrans_msgisold(int num)
{
    static char	*pname = "AUTOServer::ptrans_msgisold";

    if (_server) return _server->ptrans_msgisold(num);

    return 0;
}

//
// Quit the server
//
DTMailError_t
AUTOServer::ptrans_quit()
{
    static char	*pname = "AUTOServer::ptrans_quit";

    if (_server) return _server->ptrans_quit();

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// request nth message
//
DTMailError_t
AUTOServer::ptrans_retrieve_start(int number, int *lenp)
{
    static char	*pname = "AUTOServer::ptrans_retrieve_start";

    if (_server) return _server->ptrans_retrieve_start(number, lenp);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Parse command response
//
DTMailError_t
AUTOServer::ptrans_parse_response(char *argbuf)
{
    static char	*pname = "AUTOServer::ptrans_parse_response";

    if (_server) return _server->ptrans_parse_response(argbuf);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}

//
// Retrieve messages using IMAP Version 2bis or Version 4.
//
void
AUTOServer::retrieve_messages(DtMailEnv &error)
{
    if (NULL != _server)
    {
        _server->retrieve_messages(error);
	return;
    }

    //
    // First time through
    //
    for (int i=0; i<AUTO_NPROTOCOLS; i++)
    {
	if (NULL != _server)
	  delete _server;
	
	switch (i)
	{
	  case AUTO_POP3:
	      _server = new POP3Server(
					_folder,
					_session,
					_mailbox,
					_append_mailbox_cb,
					_append_mailbox_cb_data);
	      break;
	  case AUTO_APOP:
	      _server = new APOPServer(
					_folder,
					_session,
					_mailbox,
					_append_mailbox_cb,
					_append_mailbox_cb_data);
	      break;
	  case AUTO_IMAP:
	      _server = new IMAPServer(
					_folder,
					_session,
					_mailbox,
					_append_mailbox_cb,
					_append_mailbox_cb_data);
	      break;
	  case AUTO_POP2:
	      _server = new POP2Server(
					_folder,
					_session,
					_mailbox,
					_append_mailbox_cb,
					_append_mailbox_cb_data);
	      break;
	}

        if (NULL != _server)
        {
            if (NULL != _password)
              _server->set_password(_password);

            _server->retrieve_messages(error);
	    if (DTME_MailServerAccess_MissingPassword == (DTMailError_t) error)
	    {
		delete _server;
		_server = NULL;
		return;
	    }
	    else if (error.isNotSet())
	      return;
        }
    }
}

//
// Discard tail of FETCH response after reading message text.
//
DTMailError_t
AUTOServer::ptrans_retrieve_end(int number)
{
    static char	*pname = "AUTOServer::ptrans_retrieve_end";

    if (_server)
      return _server->ptrans_retrieve_end(number);

    _logger.logError(DTM_FALSE, "%s:  NULL server\n", pname);
    return DTME_MailServerAccess_Error;
}
