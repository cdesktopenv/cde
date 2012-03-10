/*
 *+SNOTICE
 *
 *
 *	$XConsortium: ImplDriver.hh /main/4 1996/04/21 19:45:15 drk $
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

#ifndef _IMPLDRIVER_HH
#define _IMPLDRIVER_HH

#include <DtMail/DtMail.hh>
#include <stdarg.h>

static const char *	QueryImplEntryOp = "QueryImplEntry";
typedef void (*QueryImplEntry)(DtMail::Session &,
			       DtMailEnv &,
			       const char * capability,
			       va_list args);

static const char *	QueryOpenEntryOp = "QueryOpenEntry";
typedef DtMailBoolean (*QueryOpenEntry)(DtMail::Session &,
					DtMailEnv &,
					DtMailObjectSpace space,
					void * arg);

static const char *	MailBoxConstructEntryOp = "MailBoxConstructEntryOp";
typedef DtMail::MailBox * (*MailBoxConstructEntry)(DtMail::Session &,
						   DtMailEnv &,
						   DtMailObjectSpace space,
						   void * arg,
						   DtMailCallback cb,
						   void * client_data);

static const char *	QueryMessageEntryOp = "QueryMessageEntry";
typedef DtMailBoolean (*QueryMessageEntry)(DtMail::Session &,
					   DtMailEnv &,
					   DtMailObjectSpace space,
					   void * arg);

static const char *	MessageConstructEntryOp = "MessageConstructEntryOp";
typedef DtMail::Message * (*MessageConstructEntry)(DtMail::Session &,
						   DtMailEnv &,
						   DtMailObjectSpace space,
						   void * arg,
						   DtMailCallback cb,
						   void * client_data);

static const char *	TransportConstructEntryOp = "TransportConstructEntryOp";
typedef DtMail::Transport * (*TransportConstructEntry)(DtMail::Session &,
						       DtMailEnv &,
						       DtMailStatusCallback,
						       void *);
#endif
