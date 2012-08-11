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
 *	$TOG: Sort.C /main/10 1998/10/22 11:25:16 mgreess $
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
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdlib.h>
#include <ctype.h>
#include <EUSCompat.h>
#include <DtMail/IO.hh>
#include "Sort.hh"
#include "str_utils.h"

//
// Sort the mailbox according to 'howToSort'.
//
// RETURNS:
//	The location of the displayed message after the sort.
//
int
Sort::sortMessages(MsgScrollingList	*displayList,
		   DtMail::MailBox	*mbox,
		   sortBy		howToSort)
{
  // TODO - CHECK ERROR !!!
  DtMailEnv		 error;
  char			*primary_key_str;
  int			 primary_key_int;
  int			 secondary_key_int;
  DtMailValueSeq	value;
  int			numberMessages;
  MsgHndArray		*msgHandles;
  MsgHndArray		*deletedMsgHandles;
  int			displayed;
  MsgStruct		*displayed_ms;
  DtMailBoolean		useToHeaderWhenMailIsFromMe = DTM_FALSE;
  passwd		pw;

  //
  // Remember the displayed_ms message.
  //
  displayed = displayList->get_displayed_item();
  displayed_ms = displayList->get_message_struct(displayed);

  msgHandles = displayList->get_messages();
  deletedMsgHandles = displayList->get_deleted_messages();
  if (howToSort == SortSender)
  {
      useToHeaderWhenMailIsFromMe =
	displayList->senderIsToHeaderWhenMailFromMe();
      GetPasswordEntry(pw);
  }

  if (msgHandles != NULL && mbox != NULL)
  {
    //
    // Add in the deleted messages for the purpose of sorting.
    //
    if (NULL != deletedMsgHandles)
    {
        numberMessages = deletedMsgHandles->length();
        for (int i = 0; i < numberMessages; i++)
        {
	    MsgStruct *ms = deletedMsgHandles->at(i);
	    msgHandles->insert(ms);
        }
    }

    numberMessages = msgHandles->length();
    if (numberMessages > 0)
    {
      //
      // Need list for all of the messages.
      // +2 for 2 artificial records R(0) and R(n+1).  See Knuth (5.2.4)
      // Algorithm L
      // In addition, the data must be placed in the array with the
      // dummy records at the beginning and end of the array.

      messageRecord	* messages = new messageRecord[numberMessages +2];

      register unsigned int	offset;
      register unsigned int	msgno;
      DtMail::Message		* msg = NULL;
      DtMail::Envelope	* envelope = NULL;

      //
      // Get the messages from the list.
      //
      for(msgno=0 ; msgno<numberMessages; msgno++)
      {
	offset = msgno + 1;

	//
	// Get the handle and envelope and header.
	//
	messages[offset].msg_struct = msgHandles->at(msgno);

	if (howToSort != SortMsgNum)
	{
	    // Don't need envelope to sort by MsgNum since that is
	    // a front end concept
	    msg = mbox->getMessage(error,
				messages[offset].msg_struct->message_handle);

	    if (error.isSet())
	    {
		fprintf(stderr,
			"dtmail: getMessage: Could not get message # %d: %s\n",
			msgno, (const char *)error);
	    }

	    if (msg != NULL)
	    {
		envelope = msg->getEnvelope(error);
		if (error.isSet())
		{
		    fprintf(stderr,
		"dtmail: getEnvelope: Could not get envelope for # %d: %s\n",
			    msgno, (const char *)error);
		}
	    }
	    if (msg == NULL || envelope == NULL) continue;
	}

	primary_key_str = NULL;
	primary_key_int = 0;

	// Set up the secondary sort key using the received timestamp.
	envelope->getHeader(error, DtMailMessageReceivedTime, DTM_TRUE, value);
	if (error.isSet())
	  secondary_key_int = 0;
	else
	{
	    DtMailValueDate ds;
	    ds = (*(value[0])).toDate();
	    secondary_key_int = (int)ds.dtm_date;
	}
	value.clear();

	//
	// The header that we will sort on depends on how we were
	// told to sort.
	//
	switch (howToSort)
	{

	case SortSender:
	  envelope->getHeader(error, DtMailMessageSender, DTM_TRUE, value);
	  if (error.isSet())
	    primary_key_str = strdup("");
	  else
	  {
	      // Stole from MsgScrollingList
	      DtMailAddressSeq	*addr_seq = (value[0])->toAddress();
	      DtMailValueAddress *addr = (*addr_seq)[0];

	      //
	      // If we are displaying the To: header when mail is from me,
	      // check to see if I sent this mail and use to contents of the
	      // To: header in place of the Sender (aka From: header).
	      //
	      if (DTM_TRUE == useToHeaderWhenMailIsFromMe)
	      {
		  const char		*ptr;
		  int			len;
  		  DtMailValueSeq	tovalue;

		  if (NULL != addr)
		  {
		      ptr = strchr(addr->dtm_address, '@');
		      if (NULL != ptr)
			len = ptr - addr->dtm_address;
		      else
			len = strlen(addr->dtm_address);

		      if (strncmp(pw.pw_name, addr->dtm_address, len) == 0)
		      {
	  		  envelope->getHeader(
					error, DtMailMessageTo,
					DTM_TRUE, tovalue);
	   		  if (error.isNotSet())
			  {
			      addr_seq = (tovalue[0])->toAddress();
			      addr = (*addr_seq)[0];
			  }
		      }
		  }
	      }

	      if (!addr)
		primary_key_str = strdup("");
	      else if (addr->dtm_person)
		primary_key_str = strdup(addr->dtm_person);
	      else
	      {
		char *str;
		if (NULL != addr->dtm_address)
		  str = strdup(addr->dtm_address);
		else
		  str = strdup("");
		primary_key_str = strdup(str);
	      }
	  }
	  break;

	case SortSubject:
	  envelope->getHeader(error, DtMailMessageSubject, DTM_TRUE, value);
	  if (error.isSet())
	    primary_key_str = strdup("");
	  else
	  {
	    // Skip over "Re:, Re[n]:"
	    const char *p;

	    p = *(value[0]);
	    if (strncasecmp(p, "Re", 2) == 0)
	    {
		p += 2;
		while (isspace(*p)) p++;
		if (*p == '[') 
		{
		    p++;
		    while (isspace(*p)) p++;
		    while (isalnum(*p)) p++;
		    while (isspace(*p)) p++;
		    if (*p == ']') p++;
		    while (isspace(*p)) p++;
		}
		if (*p == ':') p++;
	    }

	    primary_key_str = strdup(p);
	  }
	  break;

	case SortSize:
	  envelope->getHeader(
			error,
			DtMailMessageContentLength,
			DTM_TRUE,
			value);
	  if (error.isNotSet())
	    primary_key_int = (int) strtol(*(value[0]), NULL, 10);
	  break;

	case SortStatus:
	  envelope->getHeader(error, DtMailMessageStatus, DTM_TRUE, value);

	  // Want sort order to be Read, Unread, New
	  if (error.isSet())
	  {
		// No Status means New
		primary_key_int = 2;
	  }
	  else
	  {
		const char *s;
		s = *(value[0]);

		if (s == NULL) {
			// New
			primary_key_int = 2;
		} else if (strcmp(s, "RO") == 0) {
			// Read
			primary_key_int = 0;
		} else {
			// Unread
			primary_key_int = 1;
		}
	  }
	  break;

	case SortMsgNum:
	  primary_key_int = messages[offset].msg_struct->sessionNumber;
	  break;

	case SortTimeDate:
	  // FALLTHRU
	default:
	  // Default is to use the time received timestamp setup above.
	  primary_key_int = secondary_key_int;
	}

	messages[offset].primary_key_str = primary_key_str;
	messages[offset].primary_key_int = primary_key_int;
	messages[offset].secondary_key_int = secondary_key_int;

	value.clear();
      }

      //
      // Sort them.
      //
      _msort((char *)messages,
	     numberMessages,
	     sizeof(messageRecord),
	     0,				// Link (offset) is at ZERO.
	     _sortCmp);

      //
      // Rearrange the pointers to msg_structs in the original MsgHndArray.
      //
      int i;
      
      i = messages[0].link;
      for (offset = 0; offset < numberMessages ; offset++)
      {
	msgHandles->replace(offset, messages[i].msg_struct);
        if (messages[i].primary_key_str != NULL)
	  free(messages[i].primary_key_str);
	i = messages[i].link;
      }

      // Renumber the session numbers.
      for(msgno=0 ; msgno<numberMessages; msgno++)
      {
	MsgStruct *ms = msgHandles->at(msgno);
	ms->sessionNumber = msgno;
      }

      // Now cleanup.
      delete messages;
    }

    //
    // Remove the deleted messages.
    //
    if (NULL != deletedMsgHandles)
    {
        numberMessages = deletedMsgHandles->length();
        for (int i = 0; i < numberMessages; i++)
        {
	    MsgStruct *ms = deletedMsgHandles->at(i);
	    msgHandles->remove_entry(ms);
        }
    }

    //
    // Figure out the new offset for displayed_ms message
    //
    numberMessages = msgHandles->length();
    for (int i = 0; i < numberMessages; i++)
    {
        MsgStruct *ms = msgHandles->at(i);
        if (ms == displayed_ms) displayed = i + 1;
    }
  }

  return displayed;
}

//
// msort() is a list-merge sort routine generalized from Knuth (5.2.4)
// Algorithm L.  This routine requires 2 artificial records: R0 and
// Rn+1 where n = number of elements "nel".  "offset" is the byte-offset
// of the "link" field.  "width" is the size of each record.  "base" is
// the base address of the starting record (i.e. R0.)
//
// (Code lifted from msort.c in the original mailtool).
//

#define	Record(i)	(base + (width * (i)))
#define	Link(i)		(*((int *) (Record(i) + offset)))

int
Sort::_msort (char	* base,
	      int	  nel,
	      int	  width,
	      int	  offset,
	      int	(*compar)(char	**one,
				  char	**two))
{
  register int	i;
  register int	t;
  register int	s;
  register int	p;
  register int	q;
  char	*k1;
  char 	*k2;

  if (nel < 2) {
    Link(0) = 1;
    return (0);
  }

  /* Prepare two lists. */
  Link(0) = 1;
  Link(nel+1) = 2;
  for (i = nel - 2; i >= 1; i--) {
    Link(i) = -(i+2);
  }
  Link(nel-1) = 0;
  Link(nel) = 0;

  while (1) {
    /* Begin new pass */
    s = 0;
    t = nel + 1;
    p = Link(s);
    q = Link(t);
    if (q == 0) {
      return (0);
    }

	int	loopCount = 0;
    while (1) {
	loopCount++;	
      /* Compare Kp: Kq */
      k1 = Record(p);
      k2 = Record(q);
      if ((*compar)(&k1, &k2) <= 0) {
	/* Advance p */
	i = abs(p);
	Link(s) = (Link(s) < 0) ? -i : i;
	s = p;
	p = Link(p);
	if (p > 0) {
	  continue;
	}

	/* Complete the sublist */
	Link(s) = q;
	s = t;
	do {
	  t = q;
	  q = Link(q);
	} while (q > 0);
      } else {
	/* Advance q */
	i = abs(q);
	Link(s) = (Link(s) < 0) ? -i : i;
	s = q;
	q = Link(q);
	if (q > 0) {
	  continue;
	}

	/* Complete the sublist */
	Link(s) = p;
	s = t;
	do {
	  t = p;
	  p = Link(p);
	} while (p > 0);
      }

      /* End of pass? */
      p = -p;
      q = -q;
      if (q == 0) {
	i = abs(p);
	Link(s) = (Link(s) < 0) ? -i : i;
	Link(t) = 0;
	break;
      }
    }
  }
}

//
// These were used in msort() only. They are #undef'ed as a precaution only.
//
#undef Link
#undef Record

//
// Sort the two records.
//
int
Sort::_sortCmp(char ** one, char ** two)
{
  //
  // Cast the pointers to the known type.
  //
  register messageRecord	* first = (messageRecord *) *one;
  register messageRecord	* second = (messageRecord *) *two;

  if (first->primary_key_str == NULL)
  {
    if (first->primary_key_int < second->primary_key_int)
      return -1;
    else if (first->primary_key_int > second->primary_key_int)
      return 2;
    else if (first->secondary_key_int < second->secondary_key_int)
      return -1;
    else if (first->secondary_key_int > second->secondary_key_int)
      return 2;
    else 
      return 0;
  }
  else
  {
    int retval = strcmp(first->primary_key_str, second->primary_key_str);
    if (retval)
      return retval;
    else if (first->secondary_key_int < second->secondary_key_int)
      return -1;
    else if (first->secondary_key_int > second->secondary_key_int)
      return 2;
    else 
      return 0;
  }
}
