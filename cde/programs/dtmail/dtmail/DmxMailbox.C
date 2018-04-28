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
/* $XConsortium: DmxMailbox.C /main/3 1996/04/21 19:55:45 drk $ */

/*
 *+SNOTICE
 *
 *	$:$
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
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
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
#include "Dmx.h"
#include "MemUtils.hh"
#include "RoamApp.h"

const int	DmxMailbox::_firstIndex = 1;

DmxMailbox::DmxMailbox (char *filename)
{
	_mbox = NULL;
	_messageCount = 0;
	_fileName = strdup_n(filename);
        _message = NULL;
}

DmxMailbox::~DmxMailbox (void)
{
    delete(_fileName);
    if (_mbox)
      delete (_mbox);
}

void
DmxMailbox::loadMessages (void)
{
	DtMailEnv	env;
	DtMailBoolean	moreMessages = DTM_TRUE;
	DtMail::Session	*session = theRoamApp.session()->session();

	// try to construct _mbox
        _mbox = session->mailBoxConstruct (
                                env,
                                DtMailFileObject,
                                _fileName,
                                NULL,
                                NULL,
                                NULL);

        if (handleError (env, "new DtMail::MailBox") == DTM_TRUE)
                exit (1);
        
	// open the mailbox
	// (O_RDONLY for now)
	_mbox->open (env,	/* DtMailEnv */
		    DTM_FALSE,	/* auto_create */
		    O_RDONLY, 	/* open(2) flag */ 
		    (mode_t) 0,	/* create(2) mode */
		    DTM_FALSE,	/* lock_flag */
		    DTM_TRUE	/* auto_parse */
	  );

	if (handleError (env, "open mailbox") == DTM_TRUE)
		exit (1);

	// get the first message handle
	DtMailMessageHandle	first = NULL, next = NULL, prev = NULL;
	DtMailHeaderRequest	request;
	DtMailHeaderLine 	hdrline;
	int			i = _firstIndex;
	DtMail::Message		*m = NULL;	// temporary

	createHeaderRequest (request);

	first = _mbox->getFirstMessageSummary (env, request, hdrline);

	if (handleError (env, "get first msg summary") == DTM_TRUE)
		exit (1);

	if (first == NULL)
	{
		fprintf (stderr,
			"loadMessages: error w/1st message...exiting.\n");
		exit (1);
	}

	// this makes hash of the caching strategy, but oh well....
	m = _mbox->getMessage (env, first);
	if (handleError (env, "get first msg") == DTM_TRUE)
		exit (1);
	_messages [i].setMessage (m);
	
	prev = first;

	_messages [i].setHandle (first);
	_messages [i].setHeader (hdrline);

	while (moreMessages == DTM_TRUE)
	{
		next = _mbox->getNextMessageSummary (env, prev,
						request, hdrline);

		if (next == NULL)
		{
			moreMessages = DTM_FALSE;
			if (handleError (env, "msgLoop") == DTM_TRUE)
				exit (1);
			break; // break out if error
		} else {
			i++;	
			_messages [i].setHandle (next);
			_messages [i].setHeader (hdrline);
		}
		
		prev = next;
	}	

	_messageCount = i;


	// fill in the remaining message structures .. temporary, honest!
	for (int j = _firstIndex; j <= _messageCount; j++)
	{
		m = _mbox->getMessage (env, _messages [j].msgHandle);
		if (handleError (env, "getMessage loop") == DTM_TRUE)
		{
			printf ("fatal error...bailing...\n");
			exit (1);
		}	
		_messages [j].setMessage (m);

		char	tmp [100];
		sprintf (tmp, "%d", j);		// ick
		_messages [j].setInfo (tmp);
	}

	// free header request structure
	int k;
	// this assumes that you have allocated all of the possible headers
	// in the DmxHeaders -- a bad assumption, but currently true
	for (k = 0; k < DMXNUMHDRS; k++)
	{
		free (request.header_name [k]);
	}
	delete request.header_name;
	
	return;
}

void
DmxMailbox::createHeaderRequest (DtMailHeaderRequest &request)
{
	// set up default headers
	request.number_of_names = DMXNUMHDRS;
	request.header_name = new (char* [DMXNUMHDRS]);
	request.header_name[DMXCC] = strdup(DtMailMessageCc);
	request.header_name[DMXFROM] = strdup(DtMailMessageSender);
	request.header_name[DMXSUBJ] = strdup(DtMailMessageSubject);
	request.header_name[DMXDATE] = strdup(DtMailMessageReceivedTime);
	request.header_name[DMXCLENGTH] = strdup(DtMailMessageContentLength);
	request.header_name[DMXSTATUS] = strdup(DtMailMessageStatus);
	request.header_name[DMXTO] = strdup(DtMailMessageTo);
	request.header_name[DMXV3CHARSET] = strdup(DtMailMessageV3charset);
	request.header_name[DMXCONTENTTYPE] = strdup(DtMailMessageContentType);

	return;
}

void
DmxMailbox::printMailboxInfo (void)
{
    	printf ("\"%s\": %d messages\n", _fileName, _messageCount);
	return;
}
