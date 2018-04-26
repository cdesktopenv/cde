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
 *	$TOG: IMAPServer.C /main/7 1998/11/10 17:08:32 mgreess $
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
#include  <DtMail/IO.hh>

#define dtmasTAGGET()	(_transtag)

IMAPServer::IMAPServer(
		char			*folder,
		DtMail::Session		*session,
		DtMail::MailBox		*mailbox,
		DtMailAppendCallback	append_mailbox_cb,
		void			*append_mailbox_cb_data)
: DtMailServer(folder, session, mailbox,
	       append_mailbox_cb, append_mailbox_cb_data)
{
    _append_mailbox_cb = append_mailbox_cb;
    _append_mailbox_cb_data = append_mailbox_cb_data;
    _count = 0;
    _imap4 = 0;
    _recent = 0;
    _seen = 0;
    _unseen = 0;
}

IMAPServer::~IMAPServer()
{
}


//
// Set delete flag for given message.
//
DTMailError_t
IMAPServer::ptrans_delete(int msg)
{
    // Use SILENT if possible as a minor throughput optimization.
    if (_imap4)
      return do_transaction("STORE %d +FLAGS.SILENT (\\Deleted)", msg);
    else
      return do_transaction("STORE %d +FLAGS (\\Deleted)", msg);
}

//
// Apply for connection authorization
//
DTMailError_t
IMAPServer::ptrans_authorize(char*)
{
    /* try to get authorized */
    DTMailError_t	ok;
    
    ok = do_transaction("LOGIN %s \"%s\"", _username, _password);
    if (DTME_NoError != ok) return DTME_MailServerAccess_AuthorizationFailed;

    // probe to see if we're running IMAP4 and can use RFC822.PEEK 
    _imap4 = ((do_transaction("CAPABILITY")) == 0);

    return DTME_NoError;
}

//
// Get range of messages to be fetched
//
DTMailError_t
IMAPServer::ptrans_fldstate_read(int *countp, int *newp)
{
    DTMailError_t	ok;

    /* find out how many messages are waiting */
    _recent = _unseen = 0;
    ok = do_transaction("SELECT %s", _folder);
    if (DTME_NoError != ok) return ok;

    *countp = _count;

    if (_unseen)	// Optional response, but better if we see it.
      *newp = _unseen;
    else if (_recent)
      *newp = _recent;	// Mandatory
    else
      *newp = -1;	// Should never happen, RECENT is mandatory.

    return DTME_NoError;
}

//
// Capture the sizes of all messages.
//
DTMailError_t
IMAPServer::ptrans_msgsizes(int count, int *sizes)
{
    char 		buf[DTMAS_POPBUFSIZE+1];
    DTMailError_t	ok = DTME_NoError;

    ok = do_send("FETCH 1:%d RFC822.SIZE", count);
    if (DTME_NoError != ok) return ok;

    while (SockGets(buf, sizeof(buf), _sockfp))
    {
	int num, size;

	if (buf[strlen(buf)-1] == '\n')
	  buf[strlen(buf)-1] = '\0';
	if (buf[strlen(buf)-1] == '\r')
	  buf[strlen(buf)-1] = '\r';

	if (_protologging)
	  _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);

	if (strstr(buf, "OK"))
	  break;
	else if (sscanf(buf, "* %d FETCH (RFC822.SIZE %d)", &num, &size) == 2)
	  sizes[num - 1] = size;
	else
	  sizes[num - 1] = -1;
    }

    return DTME_NoError;
}

//
// Is the given message old?
//
int
IMAPServer::ptrans_msgisold(int num)
{
    DTMailError_t	ok;

    ok = do_transaction("FETCH %d FLAGS", num);
    if (DTME_NoError != ok) return 0;

    return _seen;
}

//
// request nth message
//
DTMailError_t
IMAPServer::ptrans_retrieve_start(int msg, int *lenp)
{
    char		buf[DTMAS_POPBUFSIZE+1];
    DTMailError_t	ok = DTME_NoError;
    int			num;

    //
    // If we're using IMAP4, we can fetch the message without setting its
    // seen flag.  This is good!  It means that if the protocol exchange
    // craps out during the message, it will still be marked `unseen' on
    // the server.
    //
    // However...*don't* do this if we're using keep to suppress deletion!
    // In that case, marking the seen flag is the only way to prevent the
    // message from being re-fetched on subsequent runs.
    //
    if (_imap4 && _removeafterdelivery)
      ok = do_send("FETCH %d RFC822.PEEK", msg);
    else
      ok = do_send("FETCH %d RFC822", msg);
    if (DTME_NoError != ok) return ok;

    // looking for FETCH response
    do
    {
        if (! SockGets(buf, sizeof(buf), _sockfp))
	{
	    _logger.logError(DTM_FALSE, "Socket Error fetching message");
	    return DTME_MailServerAccess_SocketIOError;
	}

	if (_protologging)
	  _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);
    } while (sscanf(buf+2, "%d FETCH (RFC822 {%d}", &num, lenp) != 2);

    if (num != msg)
    {
	_logger.logError(DTM_FALSE, "Protocol Error fetching message");
        return DTME_MailServerAccess_Error;
    }
    else
    {
        return DTME_NoError;
    }
}

//
// Parse command response
//
DTMailError_t
IMAPServer::ptrans_parse_response(char *argbuf)
{
    char buf[DTMAS_POPBUFSIZE+1];

    _seen = 0;
    do
    {
	if (! SockGets(buf, sizeof(buf), _sockfp))
	{
	    _logger.logError(DTM_FALSE, "Socket Error reading response");
	    return DTME_MailServerAccess_SocketIOError;
	}

	if (buf[strlen(buf)-1] == '\n')
	  buf[strlen(buf)-1] = '\0';
	if (buf[strlen(buf)-1] == '\r')
	  buf[strlen(buf)-1] = '\r';

	if (_protologging)
	  _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);

	/* interpret untagged status responses */
	if (strstr(buf, "EXISTS"))
	  _count = atoi(buf+2);
	if (strstr(buf, "RECENT"))
	  _recent = atoi(buf+2);
	if (strstr(buf, "UNSEEN"))
	  _unseen = atoi(buf+2);
	if (strstr(buf, "FLAGS"))
	  _seen = (strstr(buf, "Seen") != (char *)NULL);

    } while (strlen(dtmasTAGGET()) &&
	     strncmp(buf, dtmasTAGGET(), strlen(dtmasTAGGET())));

    if (! strlen(dtmasTAGGET()))
    {
	strcpy(argbuf, buf);
	return DTME_NoError; 
    }
    else
    {
	char	*cp;

	/* skip the tag */
	for (cp = buf; !isspace(*cp); cp++)
	  continue;

	while (isspace(*cp))
	  cp++;

	if (strncmp(cp, "OK", 2) == 0)
	{
	    strcpy(argbuf, cp);
	    return DTME_NoError;
	}
	else if (strncmp(cp, "BAD", 2) == 0)
	{
	    _logger.logError(DTM_FALSE, "Protocol Error reading response");
	    return DTME_MailServerAccess_Error;
	}
	else
	{
	    _logger.logError(DTM_FALSE, "Protocol Violation reading response");
	    return DTME_MailServerAccess_ProtocolViolation;
	}
    }
}

//
// Retrieve messages using IMAP Version 2bis or Version 4.
//
void
IMAPServer::retrieve_messages(DtMailEnv &error)
{
    DtMailServer::retrieve_messages(error);
}

//
// Discard tail of FETCH response after reading message text.
//
DTMailError_t
IMAPServer::ptrans_retrieve_end(int)
{
    char buf [DTMAS_POPBUFSIZE+1];

    if (! SockGets(buf, sizeof(buf), _sockfp))
    {
	_logger.logError(DTM_FALSE, "Socket Error reading trail");
        return DTME_MailServerAccess_SocketIOError;
    }

    return DTME_NoError;
}
