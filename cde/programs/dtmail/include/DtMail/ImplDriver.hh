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
