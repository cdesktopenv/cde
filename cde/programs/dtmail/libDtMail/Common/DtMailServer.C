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
/*
 *+SNOTICE
 *
 *	$TOG: DtMailServer.C /main/23 1999/01/29 14:46:18 mgreess $
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

#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <DtMail/DtMailError.hh>
#include <DtMail/DtMailServer.hh>
#include <DtMail/IO.hh>

#if defined(USE_ITIMER_REAL)
#define SIGNAL_TYPE	SIGALRM
#define ITIMER_TYPE	ITIMER_REAL
#else
#define SIGNAL_TYPE	SIGVTALRM
#define ITIMER_TYPE	ITIMER_VIRTUAL
#endif

#define dtmasTAGCLR()	*_transtag = '\0'; _transnum=0;
#define dtmasTAGGEN()	(sprintf(_transtag, "a%04d", ++_transnum), _transtag)
#define dtmasTAGGET()	(_transtag)

static jmp_buf	restart;

DtMailServer::DtMailServer(
		char			*folder,
		DtMail::Session		*session,
		DtMail::MailBox		*mailbox,
		DtMailAppendCallback	append_mailbox_cb,
		void			*append_mailbox_cb_data)
{
    struct passwd	pw;

    GetPasswordEntry(pw);

    if (folder)
      _folder		= strdup(folder);
    else
      _folder		= "INBOX";
    _session 		= session;
    _mailbox 		= mailbox;

    _append_mailbox_cb 	= append_mailbox_cb;
    _append_mailbox_cb_data
			= append_mailbox_cb_data;
    _protologging	= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_PROTOLOGGING,
					(Boolean) False);
    _protologgingplus	= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_PROTOLOGGINGPLUS,
					(Boolean) False);
    _errorstring	= NULL;
    _password		= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_PASSWORD,
					NULL, DTM_TRUE);
    _removeafterdelivery
			= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_REMOVEAFTERDELIVERY,
					True);
    _retrieveold	= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_RETRIEVEOLD,
					True);
    _retrieveerrors	= 0;
    _servername		= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_SERVERNAME,
					DTMAS_PROPDFLT_SERVERNAME);
    _shroud		= NULL;
    _sizelimit		= 0;
    _sockfp		= NULL;
    dtmasTAGCLR();
    _transnum		= 0;
    _timeout		= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_TIMEOUT,
					DTMAS_TIMEOUT);
    _username		= get_mailrc_value(
					_session, _folder,
					DTMAS_PROPKEY_USERNAME,
					pw.pw_name);
}

DtMailServer::~DtMailServer()
{
    if (_errorstring) free(_errorstring);
    if (_folder) free(_folder);
    if (_password) free(_password);
    if (_servername) free(_servername);
    if (_shroud) free(_shroud);
    if (_username) free(_username);
}

void
DtMailServer::set_password(char *password)
{
    if (NULL == password) return;

    if (NULL != _password) free(_password);
    _password = NULL;
    if (NULL != password) _password = strdup(password);
}

//
// Read message content and append to mailbox
//
//	len	- Length of message.
//
#if defined(reallyoldsun) || defined(USL)
#define SA_HANDLER_TYPE void (*)(void)
#else
#define SA_HANDLER_TYPE void (*)(int)
#endif

DTMailError_t
DtMailServer::ptrans_retrieve_readandappend(
				DtMailEnv	&error,
				long		len)
{
    static char	*pname = "DtMailServer::ptrans_retrieve_readandappend";
    static char *from = "From ";
    struct sigaction action, o_action;
    int from_done = FALSE;
    int done = FALSE;
    size_t nread = 0;
    char *s, *t;

    memset((char*) &action, 0, sizeof(struct sigaction));
    memset((char*) &o_action, 0, sizeof(struct sigaction));
    action.sa_handler = (SA_HANDLER_TYPE) SIG_IGN;
    sigaction(SIGINT, (const struct sigaction *) &action, &o_action);

    while (! done)
    {
        size_t	nbytes;

	if (nread < len)
	{
	    if (DTMAS_MSGBUFSIZE - 1 > len - nread)
	      nbytes = (size_t) len - nread;
	    else
	      nbytes = DTMAS_MSGBUFSIZE - 1;

            if (0 == (nbytes=SockRead(_msgbuf, 1, nbytes, _sockfp)))
	    {
	        _logger.logError(
			DTM_FALSE,
			"Failed to retrieve %d bytes.",
			len);
            	return DTME_MailServerAccess_SocketIOError;
	    }
            _msgbuf[nbytes] = '\0';
	}
	else
	{
	    if (NULL == SockGets(_msgbuf, DTMAS_MSGBUFSIZE, _sockfp))
	    {
	        _logger.logError(DTM_FALSE, "Failed to retrieve %d bytes", len);
                return DTME_MailServerAccess_SocketIOError;
	    }
	    nbytes = strlen(_msgbuf);
	}

	nread += nbytes;

        // Delete all carriage returns
        for (s = t = _msgbuf; *s; s++)
          if (*s != '\r') *t++ = *s;
        *t = '\0';

        if (_protologgingplus)
	  _logger.logError(
			DTM_FALSE,
			"INFO: read %d of %d octets %d remaining\n%s< %s\n",
			nbytes, len, len-nread, proto_name(), _msgbuf);

	// Determine if we are done with this message.
        if (proto_is_delimited())
	{
	  char *s = const_cast<char *> (strrchr((const char *) _msgbuf, (int) '.'));
	    
	    if (s &&
		(s == _msgbuf || *(s-1) == '\n') &&
		(*s == '.') &&
		(*(s+1) == '\n'))
	    {
		*s = '\0';
	        done = TRUE;
	    }
	}
	else if (nread >= len)
	  done = TRUE;
    
	if (0 < (nbytes = strlen(_msgbuf)))
	{
	    if (! from_done && strncmp(_msgbuf, from, strlen(from)))
	    {
		time_t clock;
	        char buffer[BUFSIZ];

		clock = time(&clock);
		sprintf(buffer, "%s %s %s",
			from, _servername, ctime((const time_t *) &clock));
                _append_mailbox_cb(
				error, buffer, strlen(buffer),
				_append_mailbox_cb_data);
	    }

	    from_done = TRUE;
            _append_mailbox_cb(error, _msgbuf, nbytes, _append_mailbox_cb_data);
	}

	if (error.isSet())
        {
	    error.logError(
			DTM_TRUE,
			"%s: Failed to append mailbox %s: %s.\n",
			pname, _folder, error.errnoMessage());
	    return DTME_AppendMailboxFile_Error;
        }
    }
  
    // Message separation.
    _append_mailbox_cb(error, "\n", 1, _append_mailbox_cb_data);

    // Sink the file pointer.
    sigaction(SIGINT, (const struct sigaction *) &o_action, NULL);
    return DTME_NoError;
}

//
// Assemble command in printf(3) style and send to the server.
//

DTMailError_t
DtMailServer::do_send(char *fmt, ... )
{
    static char	*pname = "DtMailServer::do_send";
    char	*buf = new char[DTMAS_POPBUFSIZE+1];
    int		nbytes;
    va_list	ap;
    DtMailEnv	error;

    if (proto_is_tagged())
    {
	dtmasTAGGEN();
	(void) sprintf(buf, "%s ", dtmasTAGGET());
    }
    else
    {
	dtmasTAGCLR();
	buf[0] = '\0';
    }

    va_start(ap, fmt);
    vsprintf(buf + strlen(buf), fmt, ap);
    va_end(ap);

    strcat(buf, "\r\n");
    nbytes = SockWrite(buf, 1, strlen(buf), _sockfp);
    if (nbytes != strlen(buf))
    {
	_logger.logError(DTM_FALSE, "Socket Error:  writing '%s'", buf);
	delete [] buf;
        return DTME_MailServerAccess_SocketIOError;
    }

    if (_protologging)
    {
	char *cp;
	if (_shroud && (cp = strstr(buf, _shroud)))
	  memset(cp, '*', strlen(_shroud));
	buf[strlen(buf)-1] = '\0';

	_logger.logError(DTM_FALSE, "%s> %s", proto_name(), buf);
    }
    delete [] buf;
    return DTME_NoError;
}

//
// Assemble command in printf(3) style, send to server, accept a response.
//
#if defined(__hpux)
#define DTMAS_COMMAND_TERMINATOR	"\n"
#else
#define DTMAS_COMMAND_TERMINATOR	"\r\n"
#endif
DTMailError_t
DtMailServer::do_transaction(char *fmt, ... )
{
    static char		*pname = "DtMailServer::do_transaction";
    DTMailError_t	ok;
    char		*buf = new char[DTMAS_POPBUFSIZE+1];
    int			nbytes;
    va_list		ap;

    if (proto_is_tagged())
    {
	dtmasTAGGEN();
	(void) sprintf(buf, "%s ", dtmasTAGGET());
    }
    else
    {
	dtmasTAGCLR();
	buf[0] = '\0';
    }

    va_start(ap, fmt);
    vsprintf(buf + strlen(buf), fmt, ap);
    va_end(ap);

    strcat(buf, DTMAS_COMMAND_TERMINATOR);
    nbytes = SockWrite(buf, 1, strlen(buf), _sockfp);
    if (nbytes != strlen(buf))
    {
	_logger.logError(DTM_FALSE, "Socket Error:  writing '%s'", buf);
	delete [] buf;
        return DTME_MailServerAccess_SocketIOError;
    }

    if (_protologging)
    {
	char *cp;
	if (_shroud && (cp = strstr(buf, _shroud)))
	  memset(cp, '*', strlen(_shroud));
	buf[strlen(buf)-1] = '\0';

	_logger.logError(DTM_FALSE, "%s> %s", proto_name(), buf);
    }

    /* we presume this does its own response echoing */
    ok = ptrans_parse_response(buf);
    if (ok != DTME_NoError)
    {
      if (NULL == _errorstring)
        _errorstring = strdup(buf);
    }
    delete [] buf;
    return ok;
}

//
// Get the specified mailrc value interpreted as an integer
//
int
DtMailServer::get_mailrc_value(
			DtMail::Session *ssn,
			char *pfx,
			char *id,
			int dflt)
{
    char	*string = NULL;
    int		value = 0;

    string = get_mailrc_value(ssn, pfx, id, (char*) NULL);
    if (NULL == string)
      return dflt;

    value = atoi(string);
    return value;
}

//
// Get the specified mailrc value interpreted as an string
//
char *
DtMailServer::get_mailrc_value(
			DtMail::Session *ssn,
			char *pfx,
			char *id,
			char *dflt,
			DtMailBoolean decrypt)
{
    DtMailEnv		error;
    DtMail::MailRc	*mailrc = ssn->mailRc(error);
    const char		*value = NULL;
    static char		idbuf[DTMAS_IDSIZE+1];
    char		*charval = NULL;

    DTMAS_CONCAT_MAILRC_KEY(idbuf, pfx, id);
    mailrc->getValue(error, idbuf, &value, decrypt);
    if (error.isSet())
    {
        if (NULL != dflt)
          charval = strdup(dflt);
    }
    else
      charval = strdup(value);

    if (NULL != value) free((void*) value);
    return charval;
}

//
// Get the specified mailrc value interpreted as an string
//
Boolean
DtMailServer::get_mailrc_value(
			DtMail::Session *ssn,
			char *pfx,
			char *id,
			Boolean dflt)
{
    DtMailEnv		error;
    DtMail::MailRc	*mailrc = ssn->mailRc(error);
    const char		*value = NULL;
    static char		idbuf[DTMAS_IDSIZE+1];
    Boolean		boolval = FALSE;

    DTMAS_CONCAT_MAILRC_KEY(idbuf, pfx, id);
    mailrc->getValue(error, idbuf, &value);
    if (error.isSet())
      boolval = dflt;
    else
      boolval = TRUE;

    if (NULL != value) free((void*) value);
    return boolval;
}

//
// Returns TRUE if the folder being accessed is the inbox.
//
Boolean
DtMailServer::is_inbox()
{
    if (NULL==_folder || 0==strncmp(_folder, DTMAS_INBOX, strlen(DTMAS_INBOX)))
      return TRUE;
    
    return FALSE;
}

//
// Send message to the front end for display in the status line.
//
void
DtMailServer::send_info_message(DtMailCallbackOp op)
{
    const char *errmsg = (const char *) _info;

#if defined(SEPARATE_FRONT_AND_BACK_END)
    DtMailEnv error;
    DtMailEventPacket event;

    event.key = _mailbox->getObjectKey();
    event.target = DTM_TARGET_MAILBOX;
    event.target_object = _mailbox;
    event.operation = (void*) op;
    if (NULL != errmsg)
      event.argument = strdup(errmsg);
    else
      event.argument = NULL;
    event.event_time = time(NULL);

    _session->writeEventData(error, &event, sizeof(event));
#else
    _mailbox->callCallback(op, strdup(errmsg));
#endif
}

//
// Retrieve messages from server using given protocol method table
//
void
DtMailServer::retrieve_messages(DtMailEnv &error)
{
    DTMailError_t	ok = DTME_NoError;
    int			js;
    struct sigaction	action, o_action;
    int			*msgsizes = (int*) NULL;
    int			*msgisold = (int*) NULL;

    _transnum = 0;

    /* set up the server-nonresponse timeout */
    memset((char*) &action, 0, sizeof(struct sigaction));
    memset((char*) &o_action, 0, sizeof(struct sigaction));
    action.sa_handler = (SA_HANDLER_TYPE) vtalarm_handler;

    sigaction(SIGNAL_TYPE, (const struct sigaction *) &action, &o_action);

    if ((js = setjmp(restart)) == 1)
    {
	_logger.logError(
		DTM_TRUE,
		"Timeout after %d seconds waiting for %s.\n",
		_timeout, _servername);
	ok = DTME_MailServerAccess_Error;
    }
    else if (js == 2)
    {
	/* error message printed at point of longjmp */
	ok = DTME_MailServerAccess_Error;
    }
    else
    {
	char	buf[DTMAS_POPBUFSIZE+1];
	int	len, num, count, numnew;
	int	deletions = 0;
	int	sockfd = -1;

        if (proto_requires_password() && NULL == _password)
        {
	    ok = DTME_MailServerAccess_MissingPassword;
	    goto restoreSignal;
        }

        vtalarm_setitimer(_timeout);
        _info.vSetError(DTME_MailServerAccessInfo_SocketOpen,
			DTM_FALSE, NULL, _username, _servername);
	send_info_message(DTMC_SERVERACCESSINFO);
	if (NULL != _errorstring)
	{
	    free(_errorstring);
	    _errorstring = NULL;
	}
	_sockfp = SockOpen(_servername, proto_port(), &_errorstring);
	if (NULL == _sockfp)
	{
	    if (NULL == _errorstring)
	      _errorstring = strdup(_logger.errnoMessage());
	    ok = DTME_MailServerAccess_SocketIOError;
	    goto restoreSignal;
	}
	else if (_protologging)
	{
	    _logger.logError(DTM_FALSE, "%s> %s", proto_name(), "SockOpen");
	}

	/* accept greeting message from mail server */
	vtalarm_setitimer(_timeout);
	ok = ptrans_parse_response(buf);
	if (ok != DTME_NoError) goto closeServer;

	/* try to get authorized to fetch mail */
	vtalarm_setitimer(_timeout);
	_shroud = _password;
	ok = ptrans_authorize(buf);
	_shroud = (char*) NULL;
	if (ok != DTME_NoError) goto closeServer;

	/* compute number of messages and number of new messages waiting */
	vtalarm_setitimer(_timeout);
	ok = ptrans_fldstate_read(&count, &numnew);
	if (ok != DTME_NoError) goto closeServer;

	/* show user how many messages we downloaded */
	if (_protologging)
	{
	    if (count == 0)
	      _logger.logError(
				DTM_FALSE,
				"INFO: No mail from %s@%s",
				_username, _servername);
	    else
	    {
		if (numnew != -1 && (count - numnew) > 0)
		  _logger.logError(
				DTM_FALSE,
				"INFO: %d message%s (%d seen) from %s@%s.",
		    		count, count > 1 ? "s" : "", count-numnew,
				_username, _servername);
		else
		    _logger.logError(
				DTM_FALSE,
				"INFO: %d message%s from %s@%s.",
				count, count > 1 ? "s" : "",
				_username, _servername);
	    }
	}

	if (count > 0)
	{
	    int nmsgtofetch = 0;
	    int fetched = 0;

	    /* we may need to get sizes in order to check message limits */
	    if (_sizelimit)
	    {
	        msgsizes = (int *)malloc(sizeof(int) * count);

	        vtalarm_setitimer(_timeout);
	        ok = ptrans_msgsizes(count, msgsizes);
	        if (ok != DTME_NoError) goto closeServer;
	    }

	    if (! _retrieveold)
	    {
	        msgisold = (int*) malloc(sizeof(int) * count);

	        for (num = 1; num <= count; num++)
	        {
	            vtalarm_setitimer(_timeout);
		    msgisold[num-1] = ptrans_msgisold(num);
	        }
	    }

	    for (num = 1; num <= count; num++)
	    {
		int toolarge =  msgsizes && (msgsizes[num-1] > _sizelimit);
		int ignoreold = msgisold && msgisold[num-1];

		if (! (toolarge || ignoreold)) nmsgtofetch++;
	    }

	    if (! nmsgtofetch)
	    {
                _info.vSetError(DTME_MailServerAccessInfo_NoMessages,
			        DTM_FALSE, NULL, _username, _servername);
	        send_info_message(DTMC_SERVERACCESSINFO);
	    }

	    /*
	     * In IMAP4 you can retrieve a message without marking it as
	     * having been seen while in POP3 and IMAP2BIS this is NOT
	     * possible.  The proto_is_peek_capable encapsulates this
	     * capability.
	     *
	     * The problem is when there is any kind of transient error
	     * (DNS lookup failure, or sendmail refusing delivery due to
	     * process-table limits) during retrieval, the message will
	     * be marked "seen" on the server without having been retrieved
	     * by dtmail.
	     *
	     * We need to keep track of any errors which take place in a
	     * given retrieval pass (_retrieveerrors).  If there were any
	     * errors during the previous pass, all messages which were new
	     * at that time will be delivered.
	     */

	    /* read, forward, and delete messages */
	    _retrieveerrors = 0;
	    for (num = 1, fetched = 0; nmsgtofetch && num <= count; num++)
	    {
		int	toolarge =  msgsizes && (msgsizes[num-1] > _sizelimit);
		int	ignoreold = msgisold && msgisold[num-1];

		/*
		 * We may want to reject this message if it is
		 * too large or old
		 */
		if (toolarge || ignoreold)
		{
		    if (_protologging)
		    {
			_logger.logError(DTM_FALSE,"skipping message %d",num);
			if (toolarge)
			  _logger.logError(
					DTM_FALSE,
					" (oversized, %d bytes)",
					msgsizes[num-1]);
		    }
		    if (toolarge)
		    {
        	        _info.vSetError(
				DTME_MailServerAccessInfo_MessageTooLarge,
				DTM_FALSE, NULL, msgsizes[num-1]);
		        send_info_message(DTMC_SERVERACCESSINFOERROR);
		    }
		}
		else
		{
	            vtalarm_setitimer(_timeout);

		    /*
		     * Fetch a message from the server.
		     */
		    fetched++;
                    _info.vSetError(DTME_MailServerAccessInfo_RetrievingMessage,
				    DTM_FALSE, NULL,
				    fetched, nmsgtofetch,
				    _username, _servername);
	            send_info_message(DTMC_SERVERACCESSINFO);

		    ok = ptrans_retrieve_start(num, &len);
		    if (ok != DTME_NoError)
		    {
			_retrieveerrors++;
			goto closeServer;
		    }

		    if (_protologging)
		      _logger.logError(
					DTM_FALSE,
					"INFO: reading message %d (%d bytes)",
					num, len);

		    /* Read the message and append it to the mailbox. */
		    vtalarm_setitimer(_timeout);
		    ok = ptrans_retrieve_readandappend(error, len);
		    if (ok != DTME_NoError)
		    {
			_retrieveerrors++;
			goto closeServer;
		    }

		    /* Tell the server we got it OK and resynchronize. */
		    vtalarm_setitimer(_timeout);
		    ok = ptrans_retrieve_end(num);
		    if (ok != DTME_NoError)
		    {
			_retrieveerrors++;
			goto closeServer;
		    }

		    /* Mark the message seen and remove it from the server. */
		    if (_removeafterdelivery)
		    {
		        deletions++;
		        if (_protologging) 
		          _logger.logError(DTM_FALSE, " deleted\n");
		        vtalarm_setitimer(_timeout);
		        ok = ptrans_delete(num);
		        if (ok != DTME_NoError) goto closeServer;
		    }
		    else if (_protologging) 
		      _logger.logError(DTM_FALSE, " not deleted\n");
		}
	    }

	    /* Remove all messages flagged for deletion. */
	    if (deletions > 0)
	    {
		vtalarm_setitimer(_timeout);
		ok = ptrans_fldstate_expunge();
		if (ok != DTME_NoError) goto closeServer;
	    }
	}
	else
	{
            _info.vSetError(DTME_MailServerAccessInfo_NoMessages,
			    DTM_FALSE, NULL, _username, _servername);
	    send_info_message(DTMC_SERVERACCESSINFO);
	}

    closeServer:
	vtalarm_setitimer(_timeout);
	if (ok != DTME_MailServerAccess_SocketIOError)
	  if (ok == DTME_NoError)
	    ok = ptrans_quit();
	  else
	    (void) ptrans_quit();
	vtalarm_setitimer(0);
	SockClose(_sockfp);
	_sockfp = NULL;
	dtmasTAGCLR();
    }

restoreSignal:
    _sockfp = NULL;
    if (ok != DTME_NoError)
    {
	if (NULL == _errorstring)
	  _errorstring = strdup("");
        error.vSetError(
			ok, DTM_TRUE, NULL,
			_username, _servername, proto_name(), _errorstring);
        _logger.logError(
			DTM_TRUE,
			"Error while fetching from '%s':  \n%s\n",
			_servername, (const char*) error);

    }
    else
      error.clear();

    if (_errorstring)
    {
	free(_errorstring);
	_errorstring = NULL;
    }
    if (msgsizes != NULL) free(msgsizes);
    if (msgisold != NULL) free(msgisold);
    sigaction(SIGNAL_TYPE, (const struct sigaction *) &o_action, NULL);
}

//
// Reset the nonresponse-timeout
//
#if defined(__hpux)
#define TV_USEC_TYPE	long
#else
#define TV_USEC_TYPE	int
#endif
void
DtMailServer::vtalarm_setitimer(int timeout_seconds)
{
    struct itimerval ntimeout;

    ntimeout.it_interval.tv_sec = (time_t) 0;
    ntimeout.it_interval.tv_usec = (TV_USEC_TYPE) 0;
    ntimeout.it_value.tv_sec  = (time_t) timeout_seconds;
    ntimeout.it_value.tv_usec = (TV_USEC_TYPE) 0;
    setitimer(ITIMER_TYPE, &ntimeout, (struct itimerval*) NULL);
}

//
// Handle server-timeout ALARM signal
//
void
DtMailServer::vtalarm_handler(int)
{
    longjmp(restart, 1);
}
