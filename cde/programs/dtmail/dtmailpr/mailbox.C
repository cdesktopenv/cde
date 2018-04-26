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
/* $XConsortium: mailbox.C /main/4 1996/04/21 19:44:02 drk $ */

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

#include <stdio.h>
#include "dmx.hh"


DmxMailbox::DmxMailbox (void)
{
	mbox = NULL;
	newMessages = 0;
	messageCount = 0;
	firstNew = 0;
	name = NULL;
}

void
DmxMailbox::loadMessages (void)
{
	DtMailEnv		env;
	boolean_t	moreMessages = B_TRUE;

	// open the mailbox
	// (O_RDONLY for now)
	mbox->open (env,	/* DtMailEnv */
		    DTM_FALSE,	/* auto_create */
		    O_RDONLY, 	/* open(2) flag */ 
		    (mode_t) 0,	/* create(2) mode */
		    DTM_FALSE,	/* lock_flag */
		    DTM_TRUE	/* auto_parse */
	  );

	if (handleError (env, "open mailbox") == B_TRUE)
		exit (1);

	// get the first message handle
	DtMailMessageHandle	first = NULL, next = NULL, prev = NULL;
	DtMailHeaderRequest	request;
	DtMailHeaderLine 	hdrline;
	int			i = 1;
	DtMail::Message		*m = NULL;	// temporary

	createHeaderRequest (request);

	first = mbox->getFirstMessageSummary (env, request, hdrline);

	if (handleError (env, "get first msg summary") == B_TRUE)
		exit (1);

	if (first == NULL)
	{
		fprintf (stderr,
			"loadMessages: error w/1st message...exiting.\n");
		exit (1);
	}

	// this makes hash of the caching strategy, but oh well....
	m = mbox->getMessage (env, first);
	if (handleError (env, "get first msg") == B_TRUE)
		exit (1);
	msg [i].setMessage (m);
	
	prev = first;

	msg [i].setHandle (first);
	msg [i].setHeader (hdrline);

	while (moreMessages == B_TRUE)
	{
		next = mbox->getNextMessageSummary (env, prev,
						request, hdrline);

		if (next == NULL)
		{
			moreMessages = B_FALSE;
			if (handleError (env, "msgLoop") == B_TRUE)
				exit (1);
			break; // break out if error
		} else {
			i++;	
			msg [i].setHandle (next);
			msg [i].setHeader (hdrline);
		}
		
		prev = next;
	}	

	messageCount = i;


	// fill in the remaining message structures .. temporary, honest!
	for (int j = 1; j <= messageCount; j++)
	{
		m = mbox->getMessage (env, msg [j].msgHandle);
		if (handleError (env, "getMessage loop") == B_TRUE)
		{
			printf ("fatal error...bailing...\n");
			exit (1);
		}	
		msg [j].setMessage (m);

		char	tmp [100];
		sprintf (tmp, "%d", j);		// ick
		msg [j].setInfo (tmp);

		msg[j].getFlags ();
		if (msg[j].isNew == B_TRUE)
		{
			newMessages++;
			if (firstNew == 0)
			{
				firstNew = j;
			}
		}

	}

	// no new messages -- start with first message in mailbox
	if (firstNew == 0)
		firstNew = 1;
		
	// free header request structure
	int k;
	// this assumes that you have allocated all of the possible headers
	// in the enum DmxHeaders -- a bad assumption, but currently true
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
	if (newMessages > 0)
	{
		printf ("\"%s\": %d messages %d new\n",
		name, messageCount, newMessages);
	} else {
		printf ("\"%s\": %d messages\n",
		name, messageCount);
	}

	return;
}
