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
 *	$TOG: POP3Server.C /main/8 1998/11/10 17:09:21 mgreess $
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <unistd.h>
 
#include  <DtMail/DtMail.hh>
#include  <DtMail/DtMailError.hh>
#include  <DtMail/DtMailServer.hh>
#include  <DtMail/DtVirtArray.hh>
#include  <DtMail/IO.hh>

POP3Server::POP3Server(
		char			*folder,
		DtMail::Session		*session,
		DtMail::MailBox		*mailbox,
		DtMailAppendCallback	append_mailbox_cb,
		void			*append_mailbox_cb_data)
: DtMailServer(folder, session, mailbox,
	       append_mailbox_cb, append_mailbox_cb_data)
{
    _lastretrieved = 0;
    _uidlist_old = NULL;
    _uidlist_current = NULL;
    _uidlist_file = NULL;
}

POP3Server::~POP3Server()
{
    if (_uidlist_old)
      uidlist_destroy(_uidlist_old);
    _uidlist_old = NULL;

    if (_uidlist_current)
      uidlist_destroy(_uidlist_current);
    _uidlist_current = NULL;

    if (_uidlist_file)
      free(_uidlist_file);
}


//
// Delete a given message.
//
DTMailError_t
POP3Server::ptrans_delete(int msg)
{
    if (_uidlist_current)
    {
	char	*uidliststr;
        uidliststr = uidlist_find(_uidlist_current, msg, NULL);
	_uidlist_current->remove(uidliststr);
	free(uidliststr);
    }
    return do_transaction("DELE %d", msg);
}

//
// Request nth message.
//
DTMailError_t
POP3Server::ptrans_retrieve_start(int msg, int *lenp)
{
    DTMailError_t	ok;
    char		buf[DTMAS_POPBUFSIZE+1];
    char		*cp;

    ok = do_send("RETR %d", msg);
    if (DTME_NoError != ok) return ok;
    ok = ptrans_parse_response(buf);
    if (DTME_NoError != ok) return ok;

    // Look for "nnn octets" -- there may or may not be preceding cruft.
    if ((cp = strstr(buf, " octets")) == (char *)NULL)
      *lenp = 0;
    else
    {
	while (--cp > buf && isdigit(*cp))
	    continue;
	*lenp = atoi(cp);
    }
    return DTME_NoError;
}

//
// Apply for connection authorization.
//
DTMailError_t
POP3Server::ptrans_authorize(char*)
{
    static char		*pname = "POP3Server::ptrans_authorize";
    DTMailError_t	ok;

    ok = do_transaction("USER %s", _username);
    if (DTME_NoError != ok) return DTME_MailServerAccess_AuthorizationFailed;

    ok = do_transaction("PASS %s", _password);
    if (DTME_NoError != ok) return DTME_MailServerAccess_AuthorizationFailed;

    // We're approved.
    return DTME_NoError;
}

//
// Get range of messages to be fetched.
//
DTMailError_t
POP3Server::ptrans_fldstate_read(int *countp, int *newp)
{
    static char		*pname = "POP3Server::ptrans_fldstate_read";
    DTMailError_t	ok;
    char		buf[DTMAS_POPBUFSIZE+1];

    /* get the total message count */
    ok = do_send("STAT");
    if (DTME_NoError != ok) return ok;
    ok = ptrans_parse_response(buf);
    if (DTME_NoError != ok) return ok;
    
    sscanf(buf, "%d %*d", countp);

    /*
     * Newer, RFC-1725-conformant POP servers may not have the LAST command.
     * We work as hard as possible to hide this ugliness, but it makes 
     * counting new messages intrinsically quadratic in the worst case.
     */
    *newp = -1;
    if (*countp > 0)
    {
	ok = do_send("LAST");
        if (DTME_NoError != ok) return ok;
	ok = ptrans_parse_response(buf);
	if (DTME_NoError == ok)
	{
	    int	last = 0;

	    if (sscanf(buf, "%d", &last) == 0)
	      _logger.logError(DTM_FALSE, "Protocol Error getting range");

	    if (0 == _retrieveerrors) _lastretrieved = last;

	    *newp = (*countp - _lastretrieved);
	}
 	else
 	{
	    if (NULL == _uidlist_old)
	    {
                _uidlist_old = uidlist_create();
                uidlist_read(_uidlist_old);
	    }

	    if (NULL == _uidlist_current)
              _uidlist_current = uidlist_create();

 	    /* grab the mailbox's UID list */
 	    ok = do_transaction("UIDL");
            if (DTME_NoError != ok)
	      return ok;
	    else
	    {
		int	curmsg;
		char	curuidstr[DTMAS_IDSIZE+1];


		*newp = 0;
 		while (SockGets(buf, sizeof(buf), _sockfp))
		{
		    if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
		    if (buf[strlen(buf)-1] == '\r')
			buf[strlen(buf)-1] = '\r';

		    if (_protologging)
	  	      _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);

 		    if (buf[0] == '.')
 			break;
 		    else if (sscanf(buf, "%d %s", &curmsg, curuidstr) == 2)
		    {
			char	*uidliststr = NULL;

 			_uidlist_current->append(strdup(buf));

	  		uidliststr = uidlist_find(_uidlist_old, -1, curuidstr);
			if (uidliststr)
			  (*newp)++;
		    }
 		}
 	    }
 	}
    }

    return DTME_NoError;
}

//
// Capture the sizes of all messages.
//
DTMailError_t
POP3Server::ptrans_msgsizes(int, int *sizes)
{
    static char		*pname = "POP3Server::ptrans_msgsizes";
    char		buf[DTMAS_POPBUFSIZE+1];
    DTMailError_t	ok;

    ok = do_transaction("LIST");
    if (DTME_NoError != ok) return ok;
    
    while (SockGets(buf, sizeof(buf), _sockfp))
    {
	int curmsg, cursize;

	if (buf[strlen(buf)-1] == '\n')
	  buf[strlen(buf)-1] = '\0';
	if (buf[strlen(buf)-1] == '\r')
	  buf[strlen(buf)-1] = '\r';

	if (_protologging)
	  _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);

	if (buf[0] == '.')
	  break;
	else if (sscanf(buf, "%d %d", &curmsg, &cursize) == 2)
	  sizes[curmsg-1] = cursize;
	else
	  sizes[curmsg-1] = -1;
    }

    return DTME_NoError;
}

//
// Is the given message old?
//
int
POP3Server::ptrans_msgisold(int msg)
{
    if (NULL == _uidlist_old)
      return (msg <= _lastretrieved);
    else
    {
	char	curuidstr[DTMAS_IDSIZE+1];
	int	curmsg;
	char	*uidliststr = NULL;

	uidliststr = uidlist_find(_uidlist_current, msg, NULL);
 	if (sscanf(uidliststr, "%d %s", &curmsg, curuidstr) == 2)
	  uidliststr = uidlist_find(_uidlist_old, -1, curuidstr);
	else
	  uidliststr = NULL;

	if (NULL != uidliststr)
	  return 1;
	else
	  return 0;
    }
}

//
// parse command response.
//
DTMailError_t
POP3Server::ptrans_parse_response (char *argbuf)
{
    DTMailError_t	ok;
    char		buf[DTMAS_POPBUFSIZE+1];
    char		*bufp;

    if (SockGets(buf, sizeof(buf), _sockfp))
    {
	if (buf[strlen(buf)-1] == '\n')
	  buf[strlen(buf)-1] = '\0';
	if (buf[strlen(buf)-1] == '\r')
	  buf[strlen(buf)-1] = '\r';

	if (_protologging)
	  _logger.logError(DTM_FALSE, "%s< %s", proto_name(), buf);

	bufp = buf;
	if (*bufp == '+' || *bufp == '-')
	  bufp++;
	else
	  return DTME_MailServerAccess_ProtocolViolation;

	while (isalpha(*bufp)) bufp++;
	*(bufp++) = '\0';

	if (strcmp(buf,"+OK") == 0)
	{
	    ok = DTME_NoError;
	}
	else if (strcmp(buf,"-ERR") == 0)
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
	  strcpy(argbuf,bufp);
    }
    else 
    {
	_logger.logError(DTM_FALSE, "Socket Error reading response");
	ok = DTME_MailServerAccess_SocketIOError;
    }

    return ok;
}

//
// Retrieve messages using POP3.
//
void
POP3Server::retrieve_messages(DtMailEnv &error)
{
    DtMailServer::retrieve_messages(error);

    uidlist_destroy(_uidlist_old);
    _uidlist_old = _uidlist_current;
    uidlist_write(_uidlist_old);
    _uidlist_current = NULL;
}

DtVirtArray<char*>*
POP3Server::uidlist_create()
{
    DtVirtArray<char*>	*uidlist = new DtVirtArray<char*>(25);
    return uidlist;
}

void
POP3Server::uidlist_destroy(DtVirtArray<char*> *uidlist)
{
    int		i, nuidliststr;
    char	*uidliststr;

    if (NULL == uidlist) return;

    for (i=0, nuidliststr=uidlist->length(); i<nuidliststr; i++)
    {
	uidliststr = (*uidlist)[i];
	if (NULL != uidliststr)
	  free(uidliststr);
    }
    delete uidlist;
}

char*
POP3Server::uidlist_find(
				DtVirtArray<char*> *uidlist,
				int msg,
				char *uidstr)
{
    char	*uidliststr;
    int		i, nuidliststr;

    if (msg < 0 && NULL == uidstr) return NULL;

    for (i=0, nuidliststr=uidlist->length(); i<nuidliststr; i++)
    {
	int	curmsg;
	char	curuidstr[DTMAS_IDSIZE+1];

	uidliststr = (*uidlist)[i];
 	if (sscanf(uidliststr, "%d %s", &curmsg, curuidstr) == 2)
	{
	    if ((msg < 0 || msg == curmsg) &&
		(uidstr == NULL || 0 == strcasecmp(uidstr, curuidstr)))
	      return uidliststr;
	}
    }
    return NULL;
}

void
POP3Server::uidlist_read(DtVirtArray<char*> *uidlist)
{
    static char	*pname = "POP3Server::uidlist_read";
    DtMailEnv	error;
    FILE	*fp;

    if (NULL == uidlist) return;

    if (NULL == _uidlist_file)
    {
	char	*path = new char[MAXPATHLEN+1];
	sprintf(path, "+/.%s@%s.uidlist", _folder, _servername);

	_uidlist_file = _session->expandPath(error, (const char*) path);
	if (NULL == _uidlist_file && error.isSet())
	{
	    _logger.logError(
			DTM_FALSE,
			"%s:  Failed to find uidlist file %s\n",
			pname, path);
	    delete [] path;
	    return;
	}
	delete [] path;
    }

    if (NULL != (fp = fopen(_uidlist_file, "r")))
    {
	char	uidliststr[DTMAS_POPBUFSIZE+1];
	char	curuidstr[DTMAS_IDSIZE+1];
	int	curmsg;

	while (NULL != fgets(uidliststr, DTMAS_POPBUFSIZE, fp))
 	  if (sscanf(uidliststr, "%d %s", &curmsg, curuidstr) == 2)
	    uidlist->append(strdup(uidliststr));

	fclose(fp);
    }
}

void
POP3Server::uidlist_write(DtVirtArray<char*> *uidlist)
{
    FILE	*fp;
    char	*uidliststr;
    int		i, nuidliststr;

    if (NULL == uidlist || NULL == _uidlist_file) return;

    if (NULL != (fp = fopen(_uidlist_file, "w")))
    {
        for (i=0, nuidliststr=uidlist->length(); i<nuidliststr; i++)
        {
	    uidliststr = (*uidlist)[i];
	    fprintf(fp, "%s\n", uidliststr);
        }
        fclose(fp);
    }
}
