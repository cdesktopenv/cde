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
 *
 *	$TOG: DtMailServer.hh /main/11 1999/01/29 11:49:00 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef I_HAVE_NO_IDENT
#endif

#ifndef _DTMAIL_SERVER_HH
#define _DTMAIL_SERVER_HH

#include <stdio.h>
#include <stdarg.h>

#include <DtMail/DtMail.hh>
#include <DtMail/DtMailTypes.h>
#include <DtMail/DtVirtArray.hh>

#define		DTMAS_INBOX		"INBOX"

#define		DTMAS_PROTO_NONE	"NONE"
#define		DTMAS_PROTO_APOP	"APOP"
#define		DTMAS_PROTO_AUTO	"AUTO"
#define		DTMAS_PROTO_IMAP	"IMAP"
#define		DTMAS_PROTO_POP2	"POP2"
#define		DTMAS_PROTO_POP3	"POP3"

/* definitions for buffer sizes -- somewhat arbitrary */
#define		DTMAS_TIMEOUT		10	/* timeout with server */
#define		DTMAS_IDSIZE		128	/* length of UIDL message ID */
#define		DTMAS_MSGBUFSIZE	4096   	/* size of msg read buffer */
#define		DTMAS_POPBUFSIZE	512	/* per RFC 937 */
#define		DTMAS_TAGSIZE		6	/* len of tagged proto tag */

#define		DTMAS_PROPKEY_GETMAILCOMMAND	"getmailcommand"
#define		DTMAS_PROPKEY_GETMAILVIACOMMAND	"getmailviacommand"
#define		DTMAS_PROPKEY_GETMAILVIASERVER	"getmailviaserver"
#define		DTMAS_PROPKEY_GETMAILVIASYSTEM	"getmailviasystem"
#define		DTMAS_PROPKEY_INBOXPATH		"inboxpath"
#define		DTMAS_PROPKEY_PASSWORD		"password"
#define		DTMAS_PROPKEY_PROTOCOL		"protocol"
#define		DTMAS_PROPKEY_PROTOLOGGING	"protologging"
#define		DTMAS_PROPKEY_PROTOLOGGINGPLUS	"protologgingplus"
#define		DTMAS_PROPKEY_REMEMBERPASSWORD	"rememberpassword"
#define		DTMAS_PROPKEY_REMOTENAME	"remotename"
#define		DTMAS_PROPKEY_REMOVEAFTERDELIVERY "removeafterdelivery"
#define		DTMAS_PROPKEY_RETRIEVEOLD	"retrieveold"
#define		DTMAS_PROPKEY_SERVERNAME	"servername"
#define		DTMAS_PROPKEY_SHOWCONFIRMATION	"showretrievalconfirmation"
#define		DTMAS_PROPKEY_TIMEOUT		"timeout"
#define		DTMAS_PROPKEY_USERNAME		"username"

#define		DTMAS_PROPDFLT_GETMAILCOMMAND	""
#define		DTMAS_PROPDFLT_GETMAILVIACOMMAND "f"
#define		DTMAS_PROPDFLT_GETMAILVIASERVER	"f"
#define		DTMAS_PROPDFLT_GETMAILVIASYSTEM	""
#define		DTMAS_PROPDFLT_PASSWORD		""
#define		DTMAS_PROPDFLT_PROTOCOL		DTMAS_PROTO_POP3
#define		DTMAS_PROPDFLT_PROTOLOGGING	"f"
#define		DTMAS_PROPDFLT_PROTOLOGGINGPLUS	"f"
#define		DTMAS_PROPDFLT_REMEMBERPASSWORD	"f"
#define		DTMAS_PROPDFLT_REMOVEAFTERDELIVERY ""
#define		DTMAS_PROPDFLT_RETRIEVEOLD	""
#define		DTMAS_PROPDFLT_SERVERNAME	""
#define		DTMAS_PROPDFLT_SHOWCONFIRMATION	"f"
#define		DTMAS_PROPDFLT_TIMEOUT		"30"


#define		DTMAS_PROTOCOL_ERROR(pname) \
{ \
  if (_errorlogging) \
  { \
    DtMailEnv	error; \
    error.logError(DTM_FALSE, (const char*) "%s:  protocol error\n", pname); \
  } \
  return PS_ERROR; \
}

#define		DTMAS_CONCAT_MAILRC_KEY(buf, pfx, key) \
{ if (NULL==(pfx)) (void) sprintf((buf), "%s", (key)); \
  else (void) sprintf((buf), "%s_%s", (pfx), (key)); }

typedef void	(*DtMailAppendCallback)(DtMailEnv&, char*, int, void*);

class DtMailServer : public DtCPlusPlusAllocator
{
public:
    friend class AUTOServer;

    DtMailServer(char*, DtMail::Session*, DtMail::MailBox*,
		 DtMailAppendCallback, void*);
    ~DtMailServer();

    static int		 get_mailrc_value(
    					DtMail::Session*,
					char*, char*,
					int dflt = 0);
    static char		*get_mailrc_value(
    					DtMail::Session*,
					char*, char*,
					char*dflt = NULL,
					DtMailBoolean decrypt = DTM_FALSE);
    static Boolean	 get_mailrc_value(
    					DtMail::Session*,
					char*, char*,
					Boolean dflt = False);
    void		 set_password(char *);
    virtual void	 retrieve_messages(DtMailEnv&);

protected:
    //
    // Protocol specific access methods
    //
    virtual DTMailError_t	ptrans_authorize(char*) = 0;
    virtual DTMailError_t	ptrans_delete(int) = 0;
    virtual int			ptrans_msgisold(int) = 0;
    virtual DTMailError_t	ptrans_fldstate_expunge() = 0;
    virtual DTMailError_t	ptrans_fldstate_read(int*, int*)= 0;
    virtual DTMailError_t	ptrans_msgsizes(int, int*) = 0;
    virtual DTMailError_t	ptrans_parse_response(char*) = 0;
    virtual DTMailError_t	ptrans_quit() = 0;
    virtual DTMailError_t	ptrans_retrieve_start(int, int*) = 0;
    virtual DTMailError_t	ptrans_retrieve_end(int) = 0;

    //
    // Protocol specific characteristics
    //
    virtual int			 proto_is_delimited() = 0;
    virtual int			 proto_is_peek_capable() = 0;
    virtual int			 proto_is_tagged() = 0;
    virtual char		*proto_name() = 0;
    virtual int			 proto_port() = 0;
    virtual int			 proto_requires_password() = 0;

    //
    // Server related Access Protocol data
    //
    DtMailAppendCallback _append_mailbox_cb;
    void		*_append_mailbox_cb_data;
    char		*_errorstring;
    char		*_folder;
    DtMailEnv		 _info;
    DtMailEnv		 _logger;
    char		 _msgbuf[DTMAS_MSGBUFSIZE];
    char		*_password;
    Boolean		 _protologging;
    Boolean		 _protologgingplus;
    Boolean		 _removeafterdelivery;
    Boolean		 _retrieveold;
    int			 _retrieveerrors;
    char		*_servername;
    DtMail::Session	*_session;
    DtMail::MailBox	*_mailbox;
    int			 _sizelimit;
    void		*_sockfp;
    char		*_shroud;	// String to shroud in debug output.
    char		 _transtag[DTMAS_TAGSIZE]; 
					// Tag of the current transaction.
    int			 _transnum;	// Number of the current transaction.
    int			 _timeout;	// Timeout seconds waiting for server.
    char		*_username;

    //
    // Generic Access Protocol methods
    //
    DTMailError_t	do_send(char *fmt, ...);
    DTMailError_t	do_transaction(char *fmt, ...);
    Boolean		is_inbox();
    DTMailError_t	ptrans_retrieve_readandappend(DtMailEnv&, long);
    void		send_info_message(DtMailCallbackOp);
    static void		vtalarm_handler(int);
    void		vtalarm_setitimer(int);
};

class AUTOServer : public DtMailServer
{
public:
    AUTOServer(char*, DtMail::Session*, DtMail::MailBox*,
	       DtMailAppendCallback, void*);
    ~AUTOServer();

    virtual void		retrieve_messages(DtMailEnv&);

protected:
    //
    // Protocol specific methods
    //
    virtual DTMailError_t	ptrans_authorize(char*);
    virtual DTMailError_t	ptrans_delete(int);
    virtual int			ptrans_msgisold(int);
    virtual DTMailError_t	ptrans_fldstate_expunge();
    virtual DTMailError_t	ptrans_fldstate_read(int*, int*);
    virtual DTMailError_t	ptrans_msgsizes(int, int*);
    virtual DTMailError_t	ptrans_parse_response(char*);
    virtual DTMailError_t	ptrans_quit();
    virtual DTMailError_t	ptrans_retrieve_start(int, int*);
    virtual DTMailError_t	ptrans_retrieve_end(int);

    //
    // Protocol characteristics
    //
    virtual int		 proto_is_delimited()
			    { return _server->proto_is_delimited(); }
    virtual int		 proto_is_peek_capable()
			    { return _server->proto_is_peek_capable(); }
    virtual int		 proto_is_tagged()
			    { return _server->proto_is_tagged(); }
    virtual char	*proto_name()
			    { return _server->proto_name(); }
    virtual int		 proto_port()
			    { return _server->proto_port(); }
    virtual int		 proto_requires_password()
			    { return _server->proto_requires_password(); }

#define AUTO_POP3	0
#define AUTO_APOP	1
#define AUTO_IMAP	2
#define AUTO_POP2	3
#define AUTO_NPROTOCOLS 4

    DtMailServer	*_server;
};

class IMAPServer : public DtMailServer
{
public:
    IMAPServer(char*, DtMail::Session*, DtMail::MailBox*,
	       DtMailAppendCallback, void*);
    ~IMAPServer();

    virtual void		retrieve_messages(DtMailEnv&);

protected:
    //
    // Protocol specific methods
    //
    virtual DTMailError_t	ptrans_authorize(char*);
    virtual DTMailError_t	ptrans_delete(int);
    virtual int			ptrans_msgisold(int);
    virtual DTMailError_t	ptrans_fldstate_expunge()
				  { return do_transaction("EXPUNGE"); }
    virtual DTMailError_t	ptrans_fldstate_read(int*, int*);
    virtual DTMailError_t	ptrans_msgsizes(int, int*);
    virtual DTMailError_t	ptrans_parse_response(char*);
    virtual DTMailError_t	ptrans_quit()
				  { return do_transaction("LOGOUT"); }
    virtual DTMailError_t	ptrans_retrieve_start(int, int*);
    virtual DTMailError_t	ptrans_retrieve_end(int);

    //
    // Protocol characteristics
    //
    virtual int		 proto_is_delimited() { return FALSE; }
    virtual int		 proto_is_peek_capable() { return TRUE; }
    virtual int		 proto_is_tagged() { return TRUE; }
    virtual char	*proto_name() { return strdup(DTMAS_PROTO_IMAP); }
    virtual int		 proto_port() { return 143; }
    virtual int		 proto_requires_password() { return TRUE; }

    int			 _count;
    int			 _imap4;
    int			 _recent;
    int			 _seen;
    int			 _unseen;
};

class POP2Server : public DtMailServer
{
public:
    POP2Server(char*, DtMail::Session*, DtMail::MailBox*,
	       DtMailAppendCallback, void*);
    ~POP2Server();

    virtual void		retrieve_messages(DtMailEnv&);

protected:
    //
    // Protocol specific methods
    //
    virtual DTMailError_t	ptrans_authorize(char*);
    virtual DTMailError_t	ptrans_delete(int)
				  { return DTME_NoError; }
    virtual int			ptrans_msgisold(int);
    virtual DTMailError_t	ptrans_fldstate_expunge()
				  { return DTME_NoError; }
    virtual DTMailError_t	ptrans_fldstate_read(int*, int*);
    virtual DTMailError_t	ptrans_msgsizes(int, int*);
    virtual DTMailError_t	ptrans_parse_response(char*);
    virtual DTMailError_t	ptrans_quit()
				  { return do_transaction("QUIT"); }
    virtual DTMailError_t	ptrans_retrieve_start(int, int*);
    virtual DTMailError_t	ptrans_retrieve_end(int);

    //
    // Protocol characteristics
    //
    virtual int		 proto_is_delimited() { return FALSE; }
    virtual int		 proto_is_peek_capable() { return FALSE; }
    virtual int		 proto_is_tagged() { return FALSE; }
    virtual char	*proto_name() { return strdup(DTMAS_PROTO_POP2); }
    virtual int		 proto_port() { return 109; }
    virtual int		 proto_requires_password() { return TRUE; }

    int			 _pound_arg;
    int			 _equal_arg;
};


class POP3Server : public DtMailServer
{
public:
    POP3Server(char*, DtMail::Session*, DtMail::MailBox*,
	       DtMailAppendCallback, void*);
    ~POP3Server();

    virtual void		retrieve_messages(DtMailEnv&);

protected:
    //
    // Protocol specific methods
    //
    virtual DTMailError_t	ptrans_authorize(char*);
    virtual DTMailError_t	ptrans_delete(int);
    virtual int			ptrans_msgisold(int);
    virtual DTMailError_t	ptrans_fldstate_expunge()
				  { return DTME_NoError; }
    virtual DTMailError_t	ptrans_fldstate_read(int*, int*);
    virtual DTMailError_t	ptrans_msgsizes(int, int*);
    virtual DTMailError_t	ptrans_parse_response(char*);
    virtual DTMailError_t	ptrans_quit()
				  { return do_transaction("QUIT"); }
    virtual DTMailError_t	ptrans_retrieve_start(int, int*);
    virtual DTMailError_t	ptrans_retrieve_end(int)
				  { return DTME_NoError; }

    //
    // Protocol characteristics
    //
    virtual int		 proto_is_delimited() { return TRUE; }
    virtual int		 proto_is_peek_capable() { return FALSE; }
    virtual int		 proto_is_tagged() { return FALSE; }
    virtual char	*proto_name() { return strdup(DTMAS_PROTO_POP3); }
    virtual int		 proto_port() { return 110; }
    virtual int		 proto_requires_password() { return TRUE; }

    int			 _lastretrieved;
    DtVirtArray<char*>	*_uidlist_current;
    DtVirtArray<char*>	*_uidlist_old;
    char		*_uidlist_file;

    DtVirtArray<char*>	*uidlist_create();
    void		 uidlist_destroy(DtVirtArray<char*> *uidlist);
    char		*uidlist_find(
				DtVirtArray<char*> *uidlist,
				int		msg,
				char		*uid);
    void		 uidlist_read(DtVirtArray<char*> *uidlist);
    void		 uidlist_write(DtVirtArray<char*> *uidlist);
};

class APOPServer : public POP3Server
{
public:
    APOPServer(char*, DtMail::Session*, DtMail::MailBox*,
	       DtMailAppendCallback, void*);
    ~APOPServer();

protected:
    //
    // Protocol specific methods
    //
    virtual DTMailError_t	ptrans_authorize(char*);

    //
    // Protocol characteristics
    //
    virtual char	*proto_name() { return strdup(DTMAS_PROTO_APOP); }
    virtual int		 proto_requires_password() { return TRUE; }
};

#endif // _DTMAIL_SERVER_HH
