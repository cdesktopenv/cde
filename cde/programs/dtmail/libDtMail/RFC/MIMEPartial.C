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
 *
 *	$TOG: MIMEPartial.C /main/6 1998/04/06 13:27:21 mgreess $
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
#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <DtMail/DtMail.hh>
#include "RFCImpl.hh"
#include "str_utils.h"

static const char	* contentType = "content-type";
static const char	* partial = "message/partial";

//
// Get the named header out of out of the message.
//
static char	*
getNamedHeader(DtMailEnv		& error,
	       const char	* headerName,
	       RFCMessage	* message)
{
  DtMailValueSeq	  value;
  char			* results = NULL;
  RFCEnvelope		* env = (RFCEnvelope *)message->getEnvelope(error);

  if (error.isNotSet()) {
    env->getHeader(error, headerName, DTM_FALSE, value);

    if (error.isNotSet()) {
      results = strdup(*(value[0]));
    }
  }

  return(results);
}

//
// Return the value to the right of the '=' in the named string.
// As an INT.
//
static unsigned int
getNamedValueInt(const char *string, const char *name)
{
  int		  	stringLen = strlen(string);
  int		  	nameLen = strlen(name);
  int		  	results = 0;
  register unsigned int	offset;

  for (offset = 0; offset < stringLen - nameLen; offset++) {
    if (strncasecmp(&string[offset], name, nameLen) == 0) {
      if (string[offset + nameLen] == '=') {
	results = atoi(&string[offset + nameLen + 1]);
	break;
      }
    }
  }
  return(results);
}

//
// Return the value to the right of the '=' in the named string.
// As a const char *.
//
static const char	*
getNamedValueString(const char *string, const char *name)
{
  int		  	  stringLen = strlen(string);
  int		  	  nameLen = strlen(name);
  const char		* results;
  char			* stringEnd;
  register unsigned int	  offset;

  for (offset = 0; offset < stringLen - nameLen; offset++) {
    if (strncasecmp(&string[offset], name, nameLen) == 0) {
      if (string[offset + nameLen] == '=') {

	//
	// We only want what is inside the '"' quotes.
	//
	results = strdup(&string[offset + nameLen + 1]);
	if (*results == '"') {
	  results++;
	  stringEnd = const_cast <char *> (strchr(results, '"'));
	  if (stringEnd != NULL) {
	    *stringEnd = '\0';
	  }
	}

	break;
      }
    }
  }
  return(results);
}

DtMailBoolean
RFCMailBox::_isPartial(DtMailEnv	& error,
		       RFCMessage	* message)
{
  DtMailBoolean		  results = DTM_FALSE;
  RFCMessage		* messageArray = NULL;

  char		* type;

  if (message != NULL
      && (type = getNamedHeader(error, contentType, message)) != NULL) {
    
    if (error.isNotSet()) {
      if (strncasecmp(type, partial, 15) == 0) {

	//
	// Add ourselves to the array.
	//
	_partialData	* newData = new _partialData;

	newData->id = getNamedValueString(type, "id");
	newData->number = getNamedValueInt(type, "number");
	newData->total =  getNamedValueInt(type, "total");
	newData->msg = message;
	//
	// It has to have a number and an ID or it is not a valid
	// message/partial and there would be no way to put it
	// back together. Total is optional except for the last part
	// where it is required.
	//
	if (newData->number > 0 && newData->id != NULL) {
	  _partialList = (_partialData **)realloc(_partialList,
						  sizeof(_partialData *)
						   *(_partialListCount+1));
	  if (_partialList != NULL) {
	    _partialList[_partialListCount] = newData;
	    _partialListCount++;
	    results = DTM_TRUE;
	  }
	} else {
	  delete newData;	// Not a valid message/partial.
	}
      }
      free(type);
    }
    else {
	error.clear();
    }
  }
  return(results);
}

RFCMessage	*
RFCMailBox::_assemblePartial(DtMailEnv	& error,
			     RFCMessage	* message)
{
  register unsigned int	  offset;
  unsigned int	  	  totalParts = 0;

  RFCMessage		* msg = message;

  _partialData		* data = NULL;

  if (message != NULL) {
    char		* type = getNamedHeader(error, contentType, message);

    //
    // Find ourselfs in the list.
    //
    for (offset = 0 ; offset < _partialListCount ; offset++) {
      if (_partialList[offset]->msg == message) {
	data = _partialList[offset];
	break;
      }
    }

    //
    // Now look for the total for this message-id.
    //
    if (data != NULL) {
      for (offset = 0 ; offset < _partialListCount ; offset++) {
	if (strcasecmp(_partialList[offset]->id, data->id) == 0) {
	  totalParts = 	_partialList[offset]->total;
	  if (totalParts > 0) {
	    break;
	  }
	}
      }
    }

    //
    // If we know how many parts we are to process - start looking.
    //
    // If we do not know the total, then we know that not all of the
    // parts have arrived. (the last partial message must have
    // the total in it).
    //
    if (totalParts > 0) {

      char		* dummy = (char *) calloc(1, totalParts * sizeof(RFCMessage *));
      RFCMessage	** messages = (RFCMessage **)dummy;

      //
      // Now we go thru the list again, this time looking for
      // all of the parts. Place them in the correct order
      // in the list.
      //
      for (offset = 0 ; offset < _partialListCount ; offset++) {
	if (strcasecmp(_partialList[offset]->id, data->id) == 0) {
	  messages[_partialList[offset]->number - 1]
	    = _partialList[offset]->msg;
	}
      }

      //
      // If all of the parts are IN, then assemble them into
      // a new message.
      //
      int		totalSize = 0;	// Total size of all of the parts.
      char		*messageLength;

      for (offset = 0 ; offset < totalParts ; offset++) {
	if (messages[offset] == NULL) {
	  break;	// Found a missing part, do not assemble.
	}
	messageLength = getNamedHeader(error, "Content-Length",
				       messages[offset]);
	if (messageLength == NULL || error.isSet()) {
	  break;	// No way to assemble if we do not know the size.
	}
	totalSize += atoi(messageLength);	// Bump the total size.
	free(messageLength);
      }
      if (offset == totalParts) {	// Did we find all of the parts ?

	//
	// From RFC 1521, section 7.3.2:
	//
	// When generating and reassembling the parts of a message/partial
	// message, the headers of the encapsulated message must be merged with
	// the headers of the enclosing entities.  In this process the
	// following rules must be observed:
	//
	// (1) All of the header fields from the initial enclosing entity
	// (part one), except those that start with "Content-" and the
	// specific header fields "Message-ID", "Encrypted", and
	// "MIME-Version", must be copied, in order, to the new message.
	//
	// (2) Only those header fields in the enclosed message which start
	// with "Content-" and "Message-ID", "Encrypted", and "MIME-Version"
	// must be appended, in order, to the header fields of the new
	// message.  Any header fields in the enclosed message which do not
	// start with "Content-" (except for "Message-ID", "Encrypted", and
	// "MIME-Version") will be ignored.
	//
	// (3) All of the header fields from the second and any subsequent
	// messages will be ignored.

	//
	// Pick 10% more space for the headers, or a random size for
	// the new message.
	//
	totalSize = (totalSize > 10*1024) ? (int) (totalSize*1.1) : (20*1024);

	char 		* newMessage = (char *)malloc(totalSize);

	newMessage[0] = '\0';

	//
	// Copy over all of the headers from the outer 1st message
	// except the ones that we are not suppost to copy.
	//

	RFCEnvelope		* env
	  = (RFCEnvelope *)messages[0]->getEnvelope(error);

	DtMailHeaderHandle	  headerHandle;
	DtMailValueSeq		  headerValue;
	char			* headerName;
	const char		* unixFromLine = NULL;

	unsigned int	  	  duplicateCount;
	int			  fromLen;
	int			  headerNumber = 0;
	register unsigned int	  dupOffset;

	if (error.isNotSet()) {
	  //
	  // Get the first header.
	  //
	  headerHandle = env->getFirstHeader(error, &headerName, headerValue);

	  unixFromLine = env->unixFrom(error, fromLen);
	  if (unixFromLine != NULL) {
	    strncat(newMessage, unixFromLine, fromLen);
	    newMessage[fromLen] = '\0';
	  }
	  if (error.isNotSet()) {

	    //
	    // Keep geting more (all) of the headers.
	    //
	    do {
	      if ((strncasecmp(headerName, "Content-", 8) != 0)
		  && (strcasecmp(headerName, "Message-ID") != 0)
		  && (strcasecmp(headerName, "Encrypted") != 0)
		  && (strcasecmp(headerName, "MIME-Version") != 0)
		  && (strcasecmp(headerName, "Status") != 0)) {
		    

		//
		// If there is more than one value for each type,
		// then get all of the values.
		//
		duplicateCount = headerValue.length();
		for (dupOffset=0; dupOffset < duplicateCount; dupOffset++ ) {

		  //
		  // Skip if the first line is a from.
		  //
		  if (unixFromLine != NULL && headerNumber == 0) {
		    continue;
		  }
		  strcat(newMessage, headerName);
		  strcat(newMessage, ": ");
		  
		  strcat(newMessage, *(headerValue[dupOffset]));
		  strcat(newMessage, "\n");
		}
	      }

	      //
	      // Clear out the current one, so that getNextHeader
	      // does not append.
	      //
	      headerValue.clear();

	      headerHandle = env->getNextHeader(error,
						headerHandle,
						&headerName,
						headerValue);

	      headerNumber++;
	      //
	      // getNextHeader returns NULL where there are no more.
	      //
	    } while(error.isNotSet() && headerHandle != NULL);

	    //
	    headerValue.clear();

	    //
	    // Now add in the headers from the imbeded message in part 1.
	    // To do this we need to convert the message to an envelope
	    // and then get the headers from that.
	    //
	    // We then delete the envelope as it is not longer needed.
	    //
	    RFCBodyPart		* bp
	      = (RFCBodyPart *)messages[0]->getFirstBodyPart(error);

	    unsigned long	  length;

	    int			  embHeader1StLen;

	    const void		* contents;
	    const char		* embHeader1St;
	    char		* endHeader;

	    //
	    // Get the body part of the first message. It contains
	    // the headers that we need to merge with.
	    //
	    if (error.isNotSet()) {
	      bp->getContents(error, &contents, &length,
			      NULL, NULL, NULL, NULL);

	      if (error.isNotSet()) {
		//
		// Turn the 1st messages body into an envelope.
		//
		// Headers end with two new lines.
		//
		embHeader1StLen = (int) length;
		embHeader1St = (const char *)contents;
		endHeader = const_cast <char *> (strstr((const char *)contents, "\n\n"));

		if (endHeader != NULL) {
		  RFCEnvelope	embEnv(error,
				       (DtMail::Message *)NULL,
				       (const char *)contents,
				       (int)((unsigned long)endHeader
					     - (unsigned long)contents));

		  if (error.isNotSet()) {
		    
		    //
		    // Get the first header from the embeded message.
		    //
		    headerHandle = embEnv.getFirstHeader(error,
							 &headerName,
							 headerValue);

		    if (error.isNotSet()) {

		      //
		      // Keep geting more (all) of the headers.
		      //
		      do {
			if ((strncasecmp(headerName, "Content-", 8) == 0)
			    || (strcasecmp(headerName, "Message-ID") == 0)
			    || (strcasecmp(headerName, "Encrypted") == 0)
			    || (strcasecmp(headerName, "MIME-Version") == 0)) {
			  
			  //
			  // If there is more than one value for each type,
			  // then get all of the values.
			  //
			  duplicateCount = headerValue.length();
			  for (dupOffset=0
			       ; dupOffset < duplicateCount
			       ; dupOffset++ ) {

			    strcat(newMessage, headerName);
			    strcat(newMessage, ": ");
			    strcat(newMessage, *(headerValue[dupOffset]));
			    strcat(newMessage, "\n");
			  }
			}

			//
			// Clear out the current one, so that getNextHeader
			// does not append.
			//
			headerValue.clear();
			headerHandle = embEnv.getNextHeader(error,
							    headerHandle,
							    &headerName,
							    headerValue);
			//
			// getNextHeader returns NULL where there are no more.
			//
		      } while(error.isNotSet() && headerHandle != NULL);
		      //
		      headerValue.clear();
		      strcat(newMessage, "\n");	// End of headers.
		    }
		  }
		}
	      }
	      if (error.isNotSet()) {
		//
		// Now add the data parts into the one part.
		//
		size_t		messageSize = strlen(newMessage);

		length = embHeader1StLen - (&endHeader[2] - embHeader1St);

		strncat(newMessage,
			(const char *)&endHeader[2],	// After \n\n.
			(size_t) length);

		messageSize += (size_t) length;
		newMessage[messageSize] = '\0';

		//
		// Add in the 2nd -> Nth body parts.
		//
		for (offset = 1; offset < totalParts ; offset++) {
		  bp =(RFCBodyPart *)messages[offset]->getFirstBodyPart(error);
		  if (error.isSet()) {
		    break;
		  }

		  bp->getContents(error, &contents, &length,
				  NULL, NULL, NULL, NULL);
		  if (error.isSet()) {
		    break;
		  }

		  strncat(newMessage, (const char *)contents, (size_t) length);
		  messageSize += (size_t) length;
		  newMessage[messageSize] = '\0';
		}
		//
		// Make the new Message, we strdup() so we only use
		// as much space as needed (and free() old).
		//
		const char	*msgResults = strdup(newMessage);
		free(newMessage);
		newMessage = (char *)msgResults;

		msg = new RFCMessage(error, this,
				     (const char **)&msgResults,
				     (const char *)newMessage + messageSize);

		for (offset = 0; offset < totalParts ; offset++) {
		  //
		  // Delete the old parts.
		  //
		  messages[offset]->setFlag(error, DtMailMessageDeletePending);

		  //
		  // TODO - CALL EXPUNGE to get rid of the message that
		  // we just marked for delete !!!!!
		  //

		  messages[offset] = NULL;
		}
	      }
	    }
	  }
	}
      }
      if (messages != NULL) {
	free(messages);
      }
    }
    free(type);
  }
  return(msg);	// This could be the new or the original message.
}

