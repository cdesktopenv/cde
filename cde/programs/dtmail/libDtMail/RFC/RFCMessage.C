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
 *	$TOG: RFCMessage.C /main/9 1998/04/06 13:29:56 mgreess $
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <DtMail/DtMail.hh>
#include "RFCImpl.hh"
#include <DtMail/Threads.hh>
#include "str_utils.h"

const char * RFCDeleteHeader = "X-Dt-Delete-Time";
static unsigned long RFCMessageSignature = 0xff83421e;

// RFCMessage constructor for alternative message parts
// Arguments:
//  error	--
//  alt_start	-- beginning of alternative message (at interpart boundary)
//  alt_end	-- last byte of alternative message
// Outputs:
//  RFCMessage * (with _alternativeMessage == DTM_TRUE) that has a completely
//  parsed and setup message with body parts and minimal envelope.
// Description:
//  This constructor is used to create a message that is one of possibly
//  many alternative messages that are part of a single MIME multipart
//  alternative message.
//
RFCMessage::RFCMessage(DtMailEnv & error,
		       const char * alt_start,
		       const char * alt_end)
: DtMail::Message(error, NULL), _bp_cache(8), _alt_msg_cache(0)
{
  error.clear();

  // Initialize objects within RFCMessage structure
  //
  _object_signature = RFCMessageSignature;
  _dirty = 0;				// can never be dirty
  _alternativeMultipart = DTM_FALSE;	// msg does not contain alternatives
  _alternativeMessage = DTM_TRUE;	// msg is an alternative message
  _alternativeValid = DTM_FALSE;	// msg is not valid (maybe later)
  _msg_start = alt_start;
  _msg_end = alt_end;
  _msg_buf = NULL;
  _body_start = NULL;

  // parse the message, creating an envelope to encompass the first headers
  // found and setting up the body part boundaries
  //
  (void) parseMsg(error, (char *) alt_end);
  if (error.isSet()) {
    return;
  }

  // parse the message bodies - this constructor is currently only called
  // for a MIME multipart alternative message part. If other mail protocols
  // are added that have similar functionality, this constructor will have
  // to be augmented to take this into consideration
  //
  parseMIMEBodies(error);
  if (error.isSet()) {
    return;
  }

  // it worked!
  return;
}

RFCMessage::RFCMessage(DtMailEnv & error, DtMail::MailBox * parent, 
		       const char ** start,
		       const char * end_of_file)
: DtMail::Message(error, parent), _bp_cache(8), _alt_msg_cache(8)
{
    _object_signature = RFCMessageSignature;
    error.clear();

    if (_parent) {
	_session = _parent->session();
    }

    _dirty = 0;
    _alternativeMultipart = DTM_FALSE;
    _alternativeMessage = DTM_FALSE;
    _alternativeValid = DTM_FALSE;

    _msg_start = *start;

    *start = parseMsg(error, end_of_file);

    // We want to be lazy about parsing the body. Most messages are
    // never viewed in a folder. We have already hit the pages once,
    // but with MADV_SEQUENTIAL on, they are marked for immediate
    // disposal so they may be gone. Of course we are going to slow
    // the first display of the message, but odds are that will never
    // happen anyway.
    //
    // There is another reason for delaying the parsing on an MT hot
    // platform. We are parsing on a separate thread. If we wait until
    // we are asked for information about the bodies to parse them,
    // we can get the main thread to do this work. So, we can actually
    // get some concurrent parsing to occur.
    //
    // Conclusion: Lazy is better!
    //
    return;
}

RFCMessage::RFCMessage(DtMailEnv & error,
		       DtMail::Session * session,
		       DtMailObjectSpace space,
		       void * arg,
		       DtMailCallback,
		       void *)
: DtMail::Message(error, NULL), _bp_cache(8), _alt_msg_cache(8)
{
    // We do different processing, depending on the space for the
    // object.
    //
    _session = session;

    switch (space) {
      case DtMailBufferObject:
      {
	  _object_signature = RFCMessageSignature;
	  error.clear();
	  
	  _dirty = 0;
	  _alternativeMultipart = DTM_FALSE;
	  _alternativeMessage = DTM_FALSE;
	  _alternativeValid = DTM_FALSE;
	  
	  _msg_buf = (DtMailBuffer *)arg;

	  // If the buffer is not null, then we need to parse the
	  // buffer.
	  //
	  if (_msg_buf->size > 0) {
	      if (hasHeaders((const char *)_msg_buf->buffer, _msg_buf->size) == DTM_FALSE) {
		  error.setError(DTME_UnknownFormat);
		  return;
	      }

	      _msg_start = (char *)_msg_buf->buffer;
	      (void) parseMsg(error, (char *)_msg_buf->buffer + _msg_buf->size);
	      if (error.isSet()) {
		  return;
	      }
	  }
	  else {
	      // We need to construct an empty message.
	      //
	      _msg_start = _msg_end = _body_start = NULL;
    	      _envelope = new RFCEnvelope(error, this, NULL, 0);
	  }
      }
      break;

      case DtMailFileObject:
      default:
        error.setError(DTME_NotSupported);
    }
}

RFCMessage::~RFCMessage(void)
{
  if (_object_signature == RFCMessageSignature) {
    MutexLock lock_scope(_obj_mutex);
    _object_signature = 0;

    // spin through the body part cache deleting all parts cached.
    // If _alternativeMultipart is set, this is the "main message" and
    // as such all of the body parts are merely copied from one of the
    // underlying alternative messages, so in this case we only delete the
    // body part cache and not the contents of the cache, as that will be
    // taken care of when the alternative message cache is purged.
    //
    while (_bp_cache.length()) {
      BodyPartCache * bpc = _bp_cache[0];
      if (!_alternativeMultipart)
	delete bpc->body;
      delete bpc;
      _bp_cache.remove(0);
    }

    // spin through the alternative message cache destroying any
    // alternative messages which have been built to support a
    // multipart alternative message
    //
    if (_alternativeMultipart == DTM_TRUE)
      while (_alt_msg_cache.length()) {
	AlternativeMessageCache * amc = _alt_msg_cache[0];
	assert(amc != NULL);
	assert(amc->amc_msg != NULL);
	delete amc->amc_msg;
	assert(amc->amc_body != NULL);
	delete amc->amc_body;
	delete amc;
	_alt_msg_cache.remove(0);
      }
  }
}

void
RFCMessage::toBuffer(DtMailEnv & error, DtMailBuffer & buf)
{
    error.clear();

    buf.size = _msg_end - _msg_start + 1;
    buf.buffer = new char[buf.size];
    if (buf.buffer == NULL) {
	error.setError(DTME_NoMemory);
	return;
    }

    memcpy(buf.buffer, _msg_start, (size_t) buf.size);
}

const char *
RFCMessage::impl(DtMailEnv & error)
{
    // By definition, we are contained within some RFC container. We
    // want to make sure we return the exact same string, so just return
    // what our containing parent uses for its impl ID string.
    //
    if (_parent) {
	return(_parent->impl(error));
    }

    return("Internet MIME");
}

DtMail::Envelope *
RFCMessage::getEnvelope(DtMailEnv & error)
{
    error.clear();
    return(_envelope);
}

int
RFCMessage::getBodyCount(DtMailEnv & error)
{
  error.clear();

    // We may need to parse the body!
    //
    if (_bp_cache.length() == 0) {
	parseBodies(error);
    }

    return(_bp_cache.length());
}

DtMail::BodyPart *
RFCMessage::getFirstBodyPart(DtMailEnv & error)
{
    error.clear();
    
    // We may need to parse the body!
    //
    if (_bp_cache.length() == 0 && _msg_start) {
	parseBodies(error);
    }
    
    if (_bp_cache.length() == 0) {
	return(NULL);
    }

    return(bodyPart(error, 0));
}

DtMail::BodyPart *
RFCMessage::getNextBodyPart(DtMailEnv & error, DtMail::BodyPart * last)
{
    error.clear();
    
    int slot = lookupByBody(last);
    if (slot < 0 || (slot + 1) >= _bp_cache.length()) {
	return(NULL);
    }
    
    slot += 1;
    return(bodyPart(error, slot));
}

DtMail::BodyPart *
RFCMessage::newBodyPart(DtMailEnv & error, DtMail::BodyPart * after)
{
    error.clear();

    int slot = -1;

    if (after) {
	slot = lookupByBody(after);
    }

    RFCBodyPart * bp = new MIMEBodyPart(error,
					this,
					NULL,
					0,
					NULL);

    BodyPartCache * bpc = new BodyPartCache;
    bpc->body = bp;
    bpc->body_start = NULL;

    if (slot < 0) {
	_bp_cache.append(bpc);
    }
    else {
	_bp_cache.insert(bpc, slot + 1);
    }

    return(bp);
}

void
RFCMessage::setFlag(DtMailEnv & error, const DtMailMessageState flag)
{	
  error.clear();

    time_t now;
    char	str_time[20];

    switch (flag) {
      case DtMailMessageNew:
	_envelope->setHeader(error, "Status", DTM_TRUE, "NR");
	break;

      case DtMailMessageDeletePending:
	// Start the delete time out by setting the X-Delete-Time header.
	//
	now = time(NULL);
	sprintf(str_time, "%08lX", (long)now);
	_envelope->setHeader(error, RFCDeleteHeader, DTM_TRUE, str_time);
	break;

      case DtMailMessagePartial:
	//
	// There is nothing to do for partial. It is already
	// marked as partial as it has the Content-Type: message/partial
	// header. This case statment exists so that SetFlag will not
	// drop into the default 'error' below.
	break;

      default:
	error.setError(DTME_OperationInvalid);
	break;
    }

    return;
}

void
RFCMessage::resetFlag(DtMailEnv & error, const DtMailMessageState flag)
{
  error.clear();

    switch (flag) {
      case DtMailMessageNew:
	_envelope->setHeader(error, "Status", DTM_TRUE, "RO");
	break;

      case DtMailMessageDeletePending:
	_envelope->removeHeader(error, RFCDeleteHeader);
	break;

      case DtMailMessagePartial:
	//
	// There is nothing to do for partial. It is already
	// marked as partial as it has the Content-Type: message/partial
	// header. This case statment exists so that resetFlag will not
	// drop into the default 'error' below.
	//
	// You can not 'reset' message partial, you can only
	// delete the header.
	break;

      default:
	error.setError(DTME_OperationInvalid);
    }

    return;
}

DtMailBoolean
RFCMessage::flagIsSet(DtMailEnv & error, const DtMailMessageState flag)
{
    DtMailValueSeq	 value;
    DtMailBoolean	answer = DTM_FALSE;

    switch (flag) {
      case DtMailMessageNew:
	_envelope->getHeader(error, "Status", DTM_FALSE, value);
	if (error.isNotSet()) {	
	    const char * status = *(value[0]);
	    if (strcasecmp(status, "ro")) {
		answer = DTM_TRUE;
	    }
	}
	else {
	    // No Status: means a new message.
	    error.clear();
	    answer = DTM_TRUE;
	}

	break;

      case DtMailMessageDeletePending:
	_envelope->getHeader(error, RFCDeleteHeader, DTM_FALSE, value);
	if (error.isNotSet()) {
	    answer = DTM_TRUE;
	}
	else {
	    error.clear();
	}
	break;

      case DtMailMessageMultipart:
	_envelope->getHeader(error, "Content-Type", DTM_FALSE, value);
	if (error.isNotSet()) {
	    const char * type = *(value[0]);
	    if (strcasecmp(type, "X-Sun-Attachment") == 0 ||
		strncasecmp(type, "multipart", 9) == 0) {
		answer = DTM_TRUE;
	    }
	}
	else {
	    error.clear();
	}
	break;

      case DtMailMessagePartial:
	_envelope->getHeader(error, "Content-Type", DTM_FALSE, value);
	if (error.isNotSet()) {
	    const char * type = *(value[0]);
	    if (strncasecmp(type, "message/partial", 15) == 0) {
		answer = DTM_TRUE;
	    }
	} else {
	    error.clear();
	}
	break;

      default:
	break;
    }

    error.clear();

    return(answer);
}

time_t
RFCMessage::getDeleteTime(DtMailEnv & error)
{
    time_t	delete_time = 0;

    DtMailValueSeq value;
    _envelope->getHeader(error, RFCDeleteHeader, DTM_FALSE, value);
    if (error.isNotSet()) {
	delete_time = (time_t) strtol(*(value[0]), NULL, 16);
    }

    error.clear();

    return(delete_time);
}

void
RFCMessage::markDirty(const int delta)
{
    if (_parent) {
	((RFCMailBox *)_parent)->markDirty(delta);
    }

    _dirty += delta;
    if (!_dirty) {
	_dirty += 1;
    }
}

// Function: RFCMessage::fixMessageLocation - return address/length of message
// Description:
//  This function is used to obtain an address and length in bytes of a
//  message.
// Method:
//  If the message is not dirty (e.g. already mapped into memory as one
//  contiguous stream of bytes) then simply return its address and length;
//  otherwise, must construct the message into a temporary area, the address
//  and length of which is then returned.
// Arguments:
//  long & msgLen	-- (output) length of message in bytes
//  int & msgTemporary	-- (output) 0 if message resides in previously
//				allocated memory (e.g. mapped already),
//				!= 0 if temporary space allocated to hold
//				message, requiring later deallocation
// Returns:
//  const char * -> first byte of message
//
void
RFCMessage::fixMessageLocation(char ** msgHeaderStart, long & msgHeaderLen,
			       char ** msgBodyStart, long &msgBodyLen,
			       int & msgTemporary, long & msgBodyOffset)
{
  if (!_dirty) {
    // This is easy as the message is complete as is, simply return
    // its current memory location and size in bytes. The message is
    // "permanent" in that it does not currently reside in a temporary
    // memory address which later has to be deallocated.
    //
    *msgHeaderStart = (char *)_msg_start;
    msgHeaderLen = _body_start - _msg_start;
    *msgBodyStart = (char *)_body_start;
    msgBodyLen = (_msg_end - _body_start)+1;
    msgTemporary = 0;
    msgBodyOffset = 0;	// no header offset fudge factor
    return;
  }

  // Well, now it gets tricky. The message is not assembled in one
  // place in memory, which prevents fixing an address and length
  // for it, so we have to force the message to be constructed. This
  // is done in a "temporary" memory area which has to be deallocated
  // when the message is mapped in from a real file.
  //
  pinMessageDown(msgHeaderStart, msgHeaderLen, msgBodyStart, msgBodyLen);
  msgBodyOffset = msgHeaderLen-(_body_start-_msg_start);
  msgTemporary = 1;
  return;
}

// Function: RFCMessage::pinMessageDown - fix complete message in memory
// Description:
//  This function is used on a "dirty" message to cause a complete
//  in memory copy of the message to be created so that it can be
//  written out (probably as a result of a mailbox save operation).
// Method:
// Arguments:
//  long & msgLen	-- (output) length of message in bytes
//  long & msgBodyOffset -- (output) +- adjustment (between original message
//			and new message) of the first byte at which the
//			body of the message begins -- this lets us determine
//			the body start in cases where the headers are changed
//			without reparsing the message
// Returns:
//  const char * -> first byte of message pinned in memory
//
void
RFCMessage::pinMessageDown(char ** msgHeaderStart, long & msgHeaderLen,
			   char ** msgBodyStart, long &msgBodyLen)
{
  // First, determine which write method to use. We have
  // 2 choices, Sun V3, and MIME. We will need to compute
  // the size of the message, then make another pass to
  // actually store the body.
  //
  DtMailEnv error;
  int content_length;
  DtMailValueSeq	value;

  // Compute content length of message
  //
  _envelope->getHeader(error, "Mime-Version", DTM_FALSE, value);
  if (error.isNotSet()) {
      content_length = sizeMIMEBodies(error);
  }
  else {
      error.clear();
      content_length = sizeV3Bodies(error);
  }

  content_length = content_length < 0 ? 0 : content_length;

  char len_buf[20];
  sprintf(len_buf, "%d", content_length);
  _envelope->setHeader(error, "Content-Length", DTM_TRUE, len_buf);

  // Allocate storage for the headers and write headers into it
  //
  const size_t maxHeaderLength =
			(size_t) ((RFCEnvelope *)_envelope)->headerLength();
  const size_t fudgeAtEnd = 102;	// two extra \n's at end of msg + slop
  size_t msgNewHeaderSize = (maxHeaderLength+fudgeAtEnd);
  *msgHeaderStart = (char *)malloc(msgNewHeaderSize);
  assert(*msgHeaderStart != NULL);

  char * end = ((RFCEnvelope *)_envelope)->writeHeaders(*msgHeaderStart);
  end += 1;
  *end++ = '\n';
  msgHeaderLen = end-*msgHeaderStart;

  // Make sure we did not overflow the allocated area
  //
  assert(msgHeaderLen < msgNewHeaderSize);

  // As of this implementation, body parts can never change, so we do not have to
  // worry about the body part needing reconstruction
  //
  *msgBodyStart = (char *)_body_start;
  msgBodyLen = (_msg_end - _body_start)+1;
  
  return;
}

void
RFCMessage::adjustMessageLocation(char * oldStart, char * newStart, long newLength, int msgTemporary, long newBodyOffset)
{
  MutexLock lock_scope(_obj_mutex);
  
  assert((_dirty && msgTemporary) || (!_dirty && !msgTemporary));

  // At this point the message has been mapped into a new location in memory
  // Must adjust all pointers to data contained within this message
  //
  int len = (int) ((_msg_end - _msg_start) + 1 + newBodyOffset);
  assert(len == newLength);
  _body_start = (_body_start - _msg_start) + newStart + newBodyOffset;
  _msg_end = newStart + len - 1;

  ((RFCEnvelope *)_envelope)->adjustHeaderLocation(newStart, (int)(_body_start-newStart));
  int bpMaxSlot = _bp_cache.length();
  int bp;
  for (bp = 0; bp < bpMaxSlot; bp++) {
    char * new_loc = newStart + (_bp_cache[bp]->body_start - _msg_start) + newBodyOffset;
      _bp_cache[bp]->body->adjustBodyPartsLocation(new_loc);
      _bp_cache[bp]->body_start = new_loc;
  }

  // If this is the main portion of a multipart alternative message,
  // we must handle each alternative and all of the body parts that
  // comprise each alternative
  //
  if (_alternativeMultipart == DTM_TRUE) {
    // Multipart Alternative message parent body
    // Adjust all of the body parts cached for the alternatives
    //
    int altCacheMaxSlot = _alt_msg_cache.length();
    for (int altCacheSlot = 0; altCacheSlot < altCacheMaxSlot; altCacheSlot++) {
      AlternativeMessageCache *amc = _alt_msg_cache[altCacheSlot];
      char *altNewStart = (((RFCEnvelope *) amc->amc_msg->_envelope)->headerLocation() - _msg_start) + newStart + newBodyOffset;
      int altHeaderLength =
		(int) ((RFCEnvelope *) amc->amc_msg->_envelope)->headerLength();
      ((RFCEnvelope *) amc->amc_msg->_envelope)->adjustHeaderLocation(altNewStart, altHeaderLength);
      bpMaxSlot = amc->amc_msg->_bp_cache.length();
      for (bp = 0; bp < bpMaxSlot; bp++) {
        char *new_loc = newStart + (amc->amc_msg->_bp_cache[bp]->body_start - _msg_start) + newBodyOffset;
        amc->amc_msg->_bp_cache[bp]->body->adjustBodyPartsLocation(new_loc);
	amc->amc_msg->_bp_cache[bp]->body_start = new_loc;
      }
    }
  }

  _msg_start = newStart;

  // If this message is "temporary", it was dirty at the time its location
  // was fixed, and therefore was placed into an area allocated on the fly.
  // Since the message and all associated pointers no longer refer to this
  // area, it must be free'd
  //
  if (msgTemporary)
    free(oldStart);
  
  _dirty = 0;
  
}

void
RFCMessage::unfixMessageLocation(char * msgStart, int msgTemporary)
{
  MutexLock lock_scope(_obj_mutex);

  // If this message is "temporary", it was dirty at the time its location
  // was fixed, and therefore was placed into an area allocated on the fly.
  // Unfix is only called if an error has occurred which requires the
  // new message to be tossed, so we must free any storage allocated to
  // this message by a previous "fixMessageLocation".
  //
  if (msgTemporary)
    free(msgStart);
}

DtMail::BodyPart *
RFCMessage::bodyPart(DtMailEnv & error, const int slot)
{
  error.clear();

    MutexLock lock_scope(_obj_mutex);

    return(_bp_cache[slot]->body);
}

int
RFCMessage::lookupByBody(DtMail::BodyPart * bp)
{
    for (int slot = 0; slot < _bp_cache.length(); slot++) {
	if (_bp_cache[slot]->body == bp) {
	    return(slot);
	}
    }

    return(-1);
}

inline const char *
backcrlf(const char * pos)
{
    const char * back = pos;
    if (*back == '\n') {
	back -= 1;
    }

    if (*back == '\r') {
	back -= 1;
    }
    return(back);
}

const char *
RFCMessage::parseMsg(DtMailEnv & error,
		     const char * end_of_file)
{
    // We need to find the end of the envelope. This is signified by a
    // single blank line. This should simply be "\n\n" in the message
    // but RFC says parsers should be forgiving so we will tolerate
    // "\n<any-white-space>\n"
    //
    const char *hdr_end;
    for (hdr_end = _msg_start; hdr_end <= end_of_file; hdr_end++) {
	if (*hdr_end == '\n') {
	    int blanks_only = 1;
	    for (const char * blanks = hdr_end + 1; 
		 *blanks != '\n' && blanks <= end_of_file; blanks++) {
		if (!isspace((unsigned char)*blanks)) {
		    blanks_only = 0;
		    break;
		}
	    }

	    if (blanks_only) {
		// Found the end of the headers.
		break;
	    }
	}
    }

    if (hdr_end > end_of_file) { // Ran off the end.
	error.setError(DTME_NotMailBox);
	return(end_of_file + 1);
    }

    // We need to parse the headers now, because they will give us the
    // content length, type, and a message id.
    //
    _envelope = new RFCEnvelope(error, this, _msg_start, hdr_end - _msg_start + 1);
    if (error.isSet()) {
	// Oops! We need to find the next "From " line if possible to at least
	// let the rest of the parsing proceed.
	//
	const char *next_from;
	for (next_from = hdr_end + 1;
	     next_from <= (end_of_file - 6); next_from++) {
	    if (strncmp(next_from, "\nFrom ", 6) == 0) {
		break;
	    }
	}
	const char * new_end;
	if (next_from > (end_of_file - 6)) {
	    new_end = end_of_file + 1;
	}
	else {
	    new_end = next_from + 1;
	}
	return(new_end);
    }

    // The hdr_end now points at the last newline of the envelope.
    // We want the body to start after the next newline, but we must
    // be prepared to stop if the end of the file is seen before the
    // body start is found, which can happen if the last message in
    // the inbox has no contents at all.
    //
    for (_body_start = hdr_end + 1; _body_start <= end_of_file && *_body_start != '\n'; _body_start++) {
	continue;
    }
    _body_start += 1;
    if (_body_start > end_of_file)	// past the end??
      _body_start = end_of_file;	// yes: body starts on last byte

    // Need to know where this message ends.
    //
    const char *messageEnd = findMsgEnd(error, end_of_file);

    // Need to guard against the case where the current body begins
    // past the "end" of the current message - this can happen if a
    // message consists of [this_msg_headers\n][\n][next_msg_headers]
    // where there is a zero length (no) body.
    //
    if (_body_start > _msg_end)		// Does body start after last byte in message?
      _body_start = _msg_end;		// Yes: force body to start at last byte of message

    return(messageEnd);
}

// returns -> last byte of current message
//
const char *
RFCMessage::findMsgEnd(DtMailEnv & error, const char * eof)
{
    // This function sets "_msg_end" to ( "_body_start" - 1 + length_of_message ).
    // This function returns a pointer to the beginning of the next envelope "From " or (eof+1).
    // See if we have a content length. If so, then will try it first.
    //
    long content_length;

    error.clear();

    _msg_end = _body_start;

    DtMailValueSeq	value;
    _envelope->getHeader(error, "content-length", DTM_FALSE, value);
    if (error.isNotSet()) {
	content_length = atol(*(value[0]));

	// Look forward content_length amount and see if we are at
	// the end (but not beyond) of file, or have a "\nFrom ".
	// We must be careful here when using content_length as it *may*
	// be a really large (bogus) number and the addition of that number
	// to the start of the body *may* wrap around the top of the address
	// space, thus resulting in a memory access violation.
	//
	const char * next_msg = _body_start + content_length;
	const int contentLengthFudgeFactor = 30;	// "From" header min length

	if (next_msg < _body_start)			// message wrapped address space?
	  next_msg = eof+contentLengthFudgeFactor+1;	// yes: downsize it
	

	// A special case for content-length = 0. Some folders only
	// have one new line in this case which will confuse us
	// unless we do something special.
	//
	if (content_length == 0) {
	    next_msg -= 1;
	    if (next_msg <= eof && strncmp(next_msg, "\nFrom ", 6) == 0) {
		_msg_end = next_msg;
		return(next_msg + 1);
	    }
	}

	if (next_msg <= (eof - 6) && strncmp(next_msg, "\nFrom ", 6) == 0) {
	    // It worked!
	    _msg_end = backcrlf(next_msg);
	    return(next_msg + 1);
	}

	// Content length extends beyond the eof!
	// Here it becomes tricky (see bug 1204026). We do not want to
	// cause the partial receipt of a very large mail message to
	// cause other valid mail messages to be "gobbled up" in this
	// message; therefore, we apply a small fudge factor rule here.
	// If the current end of message is within X bytes (about the
	// size of a reasonable "From" header) then we take the current
	// end to be fore this message; otherwise, err on the side of
	// bursting this message into multiple smaller ones by doing
	// the "From" scan dance.
	//
	if ( (next_msg > eof)
	  && (next_msg < eof+contentLengthFudgeFactor) ) {
	    _msg_end = eof;
	    return(next_msg);
	}

	// We need to deal with extraneous white space at the end of
	// the file.
	//
	if (next_msg <= eof) {
	    const char *white;
	    for (white = next_msg; white <= eof && isspace((unsigned char)*white);
		 white++) {
		continue;
	    }

	    if (white > eof) {
		_msg_end = backcrlf(next_msg);
		return(eof + 1);
	    }

	    // See if we are at a "\nFrom ". If so, consider it a
	    // case of off by one and just accept it. This will save
	    // us from scanning the entire message when the content-length
	    // simply didnt get trailing white space right.
	    //
	    if (strncmp(white - 1, "\nFrom ", 6) == 0) {
		_msg_end = white - 2;
		return(white);
	    }
	}
    }
    else {
	error.clear();
    }

    // If the message was sent with X-Lines, we can use it to help
    // us find the end of the message.
    //
    value.clear();
    _envelope->getHeader(error, "x-lines", DTM_FALSE, value);
    if (error.isNotSet()) {
	int xlines = (int) atol(*(value[0]));

	int lcnt = 0;
	for (_msg_end = _body_start; _msg_end <= eof; _msg_end++) {
	    if (*_msg_end == '\n') {
		lcnt += 1;
		if (lcnt == xlines) {
		    break;
		}
	    }
	}
    }

    if (strncmp(_msg_end, "\nFrom ", 6) != 0) {
	// Well, looks like we have to do it the costly way. Scan the
	// folder until we hit the end of file, or we hit a "From " at
	// the start of a line.
	//
	for (_msg_end = _body_start - 1; _msg_end <= (eof - 6); _msg_end++) {
	    if (strncmp(_msg_end, "\nFrom ", 6) == 0) {
		break;
	    }
	}
    }

    // The end is either the eof, or the from line.
    //
    const char * real_end = _msg_end + 1;
    if (_msg_end > (eof - 6)) {
	real_end = eof + 1;
	_msg_end = eof;
    }
    else {
        // Again, protect against NULL mesages with 1 blank line
        // before next message.  Size was < 0 and crashed in "memcpy".
        if ( _msg_end >= _body_start )
	    _msg_end = backcrlf(_msg_end);
    }

    // Let's put a content length on this thing so we won't have to go
    // through this silliness again!
    //
    content_length = _msg_end - _body_start + 1;
    content_length = content_length < 0 ? 0 : content_length;

    char buf[20];
    sprintf(buf, "%lu", content_length);
    _envelope->setHeader(error, "Content-Length", DTM_TRUE, buf);

    return(real_end);
}

// This is a list of all of the Sun V3 specific headers
//
static const char *SCANLIST[] = {
    "x-sun-charset",
    "x-sun-content-length",
    "x-sun-content-lines",
    "x-sun-data-description",
    "x-sun-data-file",
    "x-sun-data-name",
    "x-sun-data-type",
    "x-sun-encoding-info",
    "x-sun-reference-file",
    "x-sun-text-type",
    (const char *)0};
    
void
RFCMessage::parseBodies(DtMailEnv & error)
{
    error.clear();
    
    // First we need to determine which type of message bodies we
    // have. There are actually 3 possibilities: single body part,
    // Sun V3, or MIME. The main difference in the last 2 is the
    // headers and delimiters used.
    //
    DtMailValueSeq	value;
    _envelope->getHeader(error, "Mime-Version", DTM_FALSE, value);
    if (error.isNotSet()) {
	parseMIMEBodies(error);
    }
    else {
	error.clear();
	
	// We need to try a common MIME header to see if maybe the
	// message is just poorly formed. We will examine the content
	// type to see if smells like a MIME type.
	//
	value.clear();
	_envelope->getHeader(error, "Content-Type", DTM_FALSE, value);
	if (error.isSet()) {
	    // No content-type or Mime-Version header: treat as V3
	    //
	    error.clear();
	    parseV3Bodies(error);
	}
	else {
	    const char * content_type = *(value[0]);
            // Handle "Content-Type: text" problem with /usr/lib/mail.local:
	    // Apparently /usr/lib/mail.local will insert a "Content-Type: text" 
	    // header into received mail if no other Content-Type field is present.
	    // The format of this header is consistent with RFC 1049 (March 1988)
	    // which has since been superceeded by the MIME RFC 1521.
            //
	    if (strpbrk(content_type, "/;")) {
		// sub-types identify this as MIME formatted e-mail
		//
		parseMIMEBodies(error);
	    }
	    else if (strcasecmp(content_type, "text")==0) {
		// Problem time - no Mime-Version is present yet there is a
		// content-type: text which means it could either be a Sun V3 OR
		// a very poorly formatted MIME message: see if any V3 message
		// headers are found and if so treat as V3 else treat as MIME
		//
		const char **cp;
		for (cp = SCANLIST; *cp; cp++) {
		    _envelope->getHeader(error, *cp, DTM_FALSE, value);
		    if (error.isNotSet())
			break;
		    error.clear();
		}
		if (*cp) {
		    // Found v3 header - treat as such
		    //
		    parseV3Bodies(error);
		}
		else {
		    // No v3 headers found - treat as MIME
		    //
		    parseMIMEBodies(error);
		}
	    }
	    else {
		parseV3Bodies(error);
	    }
	}
    }
    
    return;
}

void
RFCMessage::parseMIMEBodies(DtMailEnv & error)
{
  error.clear();
  
  // MIME messages have many different forms, all of which we have to
  // take into account when parsing the bodies. If there is *no* MIME
  // Content-Type field, then we treat the entire body of the message
  // as a single body part of Content-Type: text/plain.
  //
  DtMailValueSeq	value;
  _envelope->getHeader(error, "Content-Type", DTM_FALSE, value);
  if (error.isSet()) {
    parseMIMETextPlain(error);
    return;
  }

  // We have a MIME Content-Type field - handle the field appropriately
  const char * content_type = *(value[0]);
  if (strncasecmp(content_type, "multipart/", 10) == 0)
    parseMIMEMultipartSubtype(error, content_type+9);
  else if (strncasecmp(content_type, "message/", 8) == 0)
    parseMIMEMessageSubtype(error, content_type+7);
  else 	  // Unknown MIME Content-Type field - handle as text/plain.
    parseMIMETextPlain(error);

  return;
}

// parseMIMEMultipartSubtype -- parse a MIME Multipart content-type
// Arguments:
//  error --
//  subtype	-- -> Multipart subtype string (begining with "/")
// Outputs:
//  Handle the parsing of the MIME Multipart content-type message according
//  to the Multipart sub-type.
// Returns:
//  <<none>>
// Description:
//  Call this function to handle the parsing of a MIME Multipart subtype.
//
void
RFCMessage::parseMIMEMultipartSubtype(DtMailEnv & error,
				      const char * subtype)
{
  // Have a multipart message to digest - determine type of multipart message
  //
  const char * boundary = (const char *) extractBoundary(subtype);
  if (!boundary) {
    // odd--there is no boundary in the multipart/ specification?
    // This is specifically required as per RFC 1521:
    //   The only mandatory parameter for the multipart Content-Type is
    //   the boundary parameter.
    // Treat as one big unbounded message message.
    //
    BodyPartCache * bpc = new BodyPartCache;
    bpc->body_start = _body_start;
    bpc->body = new MIMEBodyPart(error, this, _body_start, 
				 _msg_end - _body_start + 1,
				 (RFCEnvelope *)_envelope);
    
    _bp_cache.append(bpc);
    return;
  }
  
  if (strncasecmp(subtype, "/alternative", 12)==0)
    parseMIMEMultipartAlternative(error, boundary);
  else if (strncasecmp(subtype, "/mixed", 6)==0)
    parseMIMEMultipartMixed(error, boundary);
  else		// unknown types handled as Multipart/mixed
    parseMIMEMultipartMixed(error, boundary);

  assert(boundary != NULL);
  free((void *)boundary);
  return;
}

// parseMIMEMessageExternalBody -- parse a MIME Message/External-body message
// Arguments:
//  error --
// Outputs:
//  Process a MIME Message/External-body message
// Returns:
//  <<none>>
// Description:
//  Call this function to handle a MIME Message/External-body message
//
void
RFCMessage::parseMIMEMessageExternalBody(DtMailEnv & error)
{
  // Have a Message/External-Body message to digest - real dumb here,
  // include entire message including the headers as a single text/plain
  //
  
  BodyPartCache * bpc = new BodyPartCache;
  bpc->body_start = _msg_start;
  bpc->body = new MIMEBodyPart(error, this, _msg_start,
			       _msg_end - _msg_start + 1,
			       (RFCEnvelope *)_envelope);
  
  _bp_cache.append(bpc);

  return;
}
  
// parseMIMEMessageSubtype -- parse a MIME Message content-type
// Arguments:
//  error --
//  subtype	-- -> Message subtype string (begining with "/")
// Outputs:
//  Handle the parsing of the MIME Message content-type message according
//  to the Message sub-type.
// Returns:
//  <<none>>
// Description:
//  Call this function to handle the parsing of a MIME Message subtype.
//
void
RFCMessage::parseMIMEMessageSubtype(DtMailEnv & error,
				      const char * subtype)
{
  // Have a Message message to digest - determine type of Message message
  //

  if (strncasecmp(subtype, "/external-body", 14) == 0)
    parseMIMEMessageExternalBody(error);
  else
    parseMIMETextPlain(error);  
  return;
}

// parseMIMETextPlain -- parse a MIME Text/Plain message
// Arguments:
//  error --
// Outputs:
//  Construct single body part cache component encompassing the entire body
//  of the message as defined by _body_start for (_msg_end-_body_start)+1 bytes
// Returns:
//  <<none>>
// Description:
//  Call this function to take the current message body and treat it as
//  a single message of type text/plain.
//
void
RFCMessage::parseMIMETextPlain(DtMailEnv & error)
{
  error.clear();
  
  // This is a single body part of text/plain.
  //
  BodyPartCache * bpc = new BodyPartCache;
  bpc->body_start = _body_start;
  bpc->body = new MIMEBodyPart(error, this, _body_start, 
			       _msg_end - _body_start + 1,
			       (RFCEnvelope *)_envelope);
  
  _bp_cache.append(bpc);
  
  return;
}

// parseMIMEMultipartAlternative -- parse a MIME multipart/alternative message
// Arguments:
//  error --
//  boundary -- interpart boundary 
// Outputs:
//  updated message pointers, constructed body part cache, and
//  constructed alternative message cache
// Returns:
//  <<none>>
// Description:
//  Given a multipart alternative MIME message, parse the message, creating
//  a body part cache with an entry containing each body part, and ???GMG???
//
// Here is how a multipart alternative message will be parsed into memory,
// where alternatives 1 and 2 can be displayed / processed on this system,
// but alternative 3 can not.
//
//  +--------------------+ -> A
//  | headers            |
//  +--------------------+ -> B
//  | interpart boundary |
//  +--------------------+ -> C
//  | alternative 1      |
//  +--------------------+ -> D
//  | interpart boundary |
//  +--------------------+ -> E
//  | alternative 2      |
//  +--------------------+ -> F
//  | interpart boundary |
//  +--------------------+ -> G
//  | alternative 3      |
//  +--------------------+ -> H
//  _alternativeMultipart == DTM_TRUE
//  _alternativeMessage == DTM_FALSE
//  _alternativeValid == DTM_FALSE
//  _bp_cache = contents of alternative 2's bp_cache (because 1-2 valid, 3 not)
//  _alt_msg_cache = entries for alternative 1, alternative 2, alternative 3
//
//  +----------------------+ -> C
//  | headers              |
//  +----------------------+
//  | multipart boundary   |
//  +----------------------+
//  | alternative 1 part a |
//  +----------------------+
//  | multipart boundary   |
//  +----------------------+
//  | alternative 1 part b |
//  +----------------------+ -> D
//  _alternativeMultipart == DTM_FALSE
//  _alternativeMessage == DTM_TRUE
//  _alternativeValid == DTM_TRUE [this sample case can display this alt.]
//  _bp_cache = entries for part a and part b
//  _alt_msg_cache = empty
//
//  +----------------------+ -> E
//  | headers              |
//  +----------------------+
//  | multipart boundary   |
//  +----------------------+
//  | alternative 2 part a |
//  +----------------------+
//  | multipart boundary   |
//  +----------------------+
//  | alternative 2 part b |
//  +----------------------+ -> F
//  _alternativeMultipart == DTM_FALSE
//  _alternativeMessage == DTM_TRUE
//  _alternativeValid == DTM_TRUE [this sample case can display this alt.]
//  _bp_cache = entries for part a and part b
//  _alt_msg_cache = empty
//
//  +----------------------+ -> G
//  | headers              |
//  +----------------------+
//  | multipart boundary   |
//  +----------------------+
//  | alternative 3 part a |
//  +----------------------+
//  | multipart boundary   |
//  +----------------------+
//  | alternative 3 part b |
//  +----------------------+ -> H
//  _alternativeMultipart == DTM_FALSE
//  _alternativeMessage == DTM_TRUE
//  _alternativeValid == DTM_FALSE [this sample case can NOT display this alt.]
//  _bp_cache = entries for part a and part b
//  _alt_msg_cache = empty
//
void
RFCMessage::parseMIMEMultipartAlternative(DtMailEnv & error, const char * boundary)
{
  assert(_alternativeMessage == DTM_FALSE);
  assert(boundary != NULL);

  // Chew through anything that appears before the first boundary.
  //
  const char * body = _body_start;
  const char * body_end = _msg_end;
  
  int bndry_len = strlen(boundary);

  for (; body <= _msg_end; body++) {
    if (*body == '-' &&
	*(body + 1) == '-' &&
	strncmp(body + 2, boundary, bndry_len) == 0) {
      break;
    }
  }

  if (body > _msg_end ||
      *(body + bndry_len + 2) == '-') {
    // No boundaries. Treat as one big message.
    //
    BodyPartCache * bpc = new BodyPartCache;
    bpc->body_start = _body_start;
    bpc->body = new MIMEBodyPart(error, this, _body_start, 
				 _msg_end - _body_start + 1,
				 (RFCEnvelope *)_envelope);
    
    _bp_cache.append(bpc);
    return;
  }

  // We are sitting at the beginning of the first boundary
  // Construct an alternative message cache for the various alternative bodies
  // On input to this loop,
  //   body -> first byte of body part including boundary
  //   body_end -> end of message (_msg_end)
  // On each iteration through the loop,
  //   - MIMEBodyPart is called with a body boundary of [body]...[body_end]
  //   - MIMEBodyPart alters body_end to be the last byte of the message,
  //		containing the boundary *only if* it is the last body part
  //
  do {
    AlternativeMessageCache * amc = new AlternativeMessageCache;
    amc->amc_body_start = body;
    amc->amc_body_end = body_end;
    amc->amc_body = new MIMEBodyPart(error, this, body, &amc->amc_body_end, boundary);
    body = amc->amc_body_end;
    if ( (amc->amc_body_end == (body_end+1))
	 && (strncmp(amc->amc_body_end-(bndry_len+3), boundary, bndry_len) == 0) ){
      amc->amc_body_end -= bndry_len+5;
    }
    amc->amc_msg = new RFCMessage(error, amc->amc_body_start, amc->amc_body_end);
    _alt_msg_cache.append(amc);
  } while (body <= _msg_end);

  // Now we *should* go through the entire alternative message cache validating
  // each message to see if it can be displayed on this system. This is a task
  // left for a future exercise. For now, use the first alternative always.
  //
  AlternativeMessageCache *amc = _alt_msg_cache[_alt_msg_cache.length()-1];
  int slotMax = amc->amc_msg->_bp_cache.length();
  for (int slot = 0; slot < slotMax; slot++) {
    BodyPartCache * p_bpc = new BodyPartCache;
    BodyPartCache * c_bpc = amc->amc_msg->_bp_cache[slot];
    p_bpc->body = c_bpc->body;
    p_bpc->body_start = c_bpc->body_start;
    _bp_cache.append(p_bpc);
  }

  _alternativeMultipart = DTM_TRUE;	// main msg of multipart alternative
}

// parseMIMEMultipartMixed -- parse a MIME multipart/mixed message
// Arguments:
//  error --
//  boundary -- bodypart boundary
// Outputs:
//  updated message pointers and constructed body part cache
// Returns:
//  <<none>>
// Description:
//  Given a multipart mixed MIME message, parse the message, creating a body
//  part cache with an entry containing each body part.
//
void
RFCMessage::parseMIMEMultipartMixed(DtMailEnv & error, const char * boundary)
{
  assert(boundary != NULL);
  
  // Chew through anything that appears before the first boundary.
  //
  const char * body = _body_start;
  const char * body_end = _msg_end;
  
  int bndry_len = strlen(boundary);

  for (; body <= _msg_end; body++) {
    if (*body == '-' &&
	*(body + 1) == '-' &&
	strncmp(body + 2, boundary, bndry_len) == 0) {
      break;
    }
  }

  if (body > _msg_end ||
      *(body + bndry_len + 2) == '-') {
    // No boundaries. Treat as one big message.
    //
    BodyPartCache * bpc = new BodyPartCache;
    bpc->body_start = _body_start;
    bpc->body = new MIMEBodyPart(error, this, _body_start, 
				 _msg_end - _body_start + 1,
				 (RFCEnvelope *)_envelope);
    
    _bp_cache.append(bpc);
    return;
  }

  // We are sitting at the beginning of the first boundary
  // Construct a body part cache from the various body parts
  // On input to this loop,
  //   body -> first byte of body part (including boundary)
  //   body_end -> end of message (_msg_end)
  // On each iteration through the loop,
  //   - MIMEBodyPart is called with a body boundary of [body]...[body_end]
  //   - MIMEBodyPart alters body_end to be the last byte of the message,
  //		containing the boundary *only if* it is the last body part
  //
  do {
    BodyPartCache * bpc = new BodyPartCache;
    bpc->body_start = body;
    bpc->body = new MIMEBodyPart(error, this, body, &body_end, boundary);
    _bp_cache.append(bpc);
    
    body = body_end;
    body_end = _msg_end;
    
  } while (body <= _msg_end);
  return;
}

void
RFCMessage::parseV3Bodies(DtMailEnv & error)
{
  error.clear();

    // We have 3 choices here. We may have no content-type field,
    // in which case it is a simple RFC822 message. We handle those
    // in the V3 body code because this is really the legacy branch.
    //
    // We can have a content-type = text which is a single body part
    // of either 7 bit text, or 8 bit unencoded text.
    //
    // Finally we could have a Sun V3 multipart document.
    //
    DtMailValueSeq	value;
    _envelope->getHeader(error, "Content-Type", DTM_FALSE, value);
    if (error.isSet()) {
	// Pretty simple. Pass the entire body and the envelope to
	// the V3 body constructor.
	//
	error.clear();
	BodyPartCache * bpc = new BodyPartCache;
	bpc->body_start = _body_start;
	bpc->body = new V3BodyPart(error, this, _body_start, 
			       _msg_end - _body_start + 1,
			       (RFCEnvelope *)_envelope);

	_bp_cache.append(bpc);

	return;
    }

    const char * content_type = *(value[0]);

    // If the type is text, or, if it isn't an attachment type we
    // understand treat the body as a single part.
    //
    if (strcasecmp(content_type, "text") == 0 ||
	strcasecmp(content_type, "x-sun-attachment") != 0) {
	BodyPartCache * bpc = new BodyPartCache;
	bpc->body_start = _body_start;
	bpc->body = new V3BodyPart(error, this, _body_start,
			       _msg_end - _body_start + 1,
			       (RFCEnvelope *)_envelope);

	_bp_cache.append(bpc);
    }
    else {
	// We need to scan for each of the message boundaries and
	// let the body part object parse the important stuff.
	//
	// V3 bodies start with a sequence of 10 dashes. After that,
	// there *should* be a newline.  After such a sequence, 
	// the body part constructor will give us the body end.
	//
	const char *body;
	for (body = _body_start - 1; body <= _msg_end; body++) {
	    if (*body == '\n' && 
		strncmp(body + 1, "----------", 10) == 0 &&
		(*(body + 11) == '\n' || 
		 *(body + 11) == '\r' && *(body + 12) == '\n')) {
		break;
	    }
	}
	if (body > _msg_end) {
	    // Well, we have some kind of inconsistency here. Let's
	    // treat it as one big body part so we can display something.
	    //
	    BodyPartCache * bpc = new BodyPartCache;
	    bpc->body_start = _body_start;
	    bpc->body = new V3BodyPart(error, this, _body_start,
				   _msg_end - _body_start + 1,
				   (RFCEnvelope *)_envelope);
	    
	    _bp_cache.append(bpc);
	    return;
	}

	body += 1;

	const char * body_end = _msg_end;

	do {
	    BodyPartCache * bpc = new BodyPartCache;
	    bpc->body_start = body;
	    bpc->body = new V3BodyPart(error, this, body, &body_end);
	    _bp_cache.append(bpc);
	    body = body_end;
	    body_end = _msg_end;
	} while (body <= _msg_end);
    }

    return;
}

int
RFCMessage::sizeMIMEBodies(DtMailEnv &)
{
  // We will use _msg_end - _body_start because this is
  // both the correct body length, and the one we updated.
  //
  return(_msg_end - _body_start + 1);
}

int
RFCMessage::sizeV3Bodies(DtMailEnv &)
{
  // We will use _msg_end - _body_start because this is
  // both the correct body length, and the one we updated.
  //
  return(_msg_end - _body_start + 1);
}

char *
RFCMessage::extractBoundary(const char * content_type)
{
    const char * boundary;

    if (!content_type) {
	char * new_bdry = (char *)malloc(78);
	sprintf(new_bdry, "%p-%08lx-%p", new_bdry, (long)time(NULL), &new_bdry);
    }

    // We will need the boundary to find the message boundaries.
    //
    for (boundary = content_type; *boundary; boundary++) {
	if (strncasecmp(boundary, "boundary=", 9) == 0) {
	    break;
	}
    }
    if (!*boundary) {
	return(NULL);
    }
    
    // Get past uninteresting cruft.
    //
    boundary += 9;
    int quoted = 0;
    if (*boundary == '"') {
	boundary += 1;
	quoted = 1;
    }
    
    // Find the end of the boundary. This will be either the end of
    // the line, a quote, or a semi-colon.
    //
    const char *b_end;
    for (b_end = boundary; *b_end; b_end++) {
	if (quoted) {
	    if (*b_end == '"') {
		break;
	    }
	}
	else {
	    if (*b_end == ';' || *b_end == '\r' || *b_end == '\n') {
		break;
	    }
	}
    }

    int len = b_end - boundary + 1;
    char * result = (char *)malloc(len);
    strncpy(result, boundary, len - 1);
    result[len - 1] = 0;

    return(result);
}

DtMailBoolean
RFCMessage::hasHeaders(const char * buf, const unsigned long len)
{
    const char * start = buf;

    if (len > 5 && strncmp(buf, "From ", 5) == 0) {
	// Get past this line. Unix from line. It doesnt count as
	// a header for our purposes.
	//
	while (start < (buf + len) && *start != '\n') {
	    start++;
	}

	start += 1;

	if (start >= (buf + len)) {
	    return(DTM_FALSE);
	}
    }

    // At this point we should have something that looks like a header.
    // This will be a string with no spaces, terminated with a colon,
    // followed by some text.
    //
    const char *colon;
    for (colon = start; colon < (buf + len) && *colon != ':'; colon++) {
	continue;
    }

    if (colon >= (buf + len) || *colon != ':') {
	return(DTM_FALSE);
    }

    // RFC822 states:
    // Once a field has been unfolded, it may be viewed as being com-
    // posed of a field-name followed by a colon (":"), followed by a
    // field-body, and  terminated  by  a  carriage-return/line-feed.
    // The  field-name must be composed of printable ASCII characters
    // (i.e., characters that  have  values  between  33.  and  126.,
    // decimal, except colon).
    //
    // We should only find characters between 33 and 126 from the start
    // to the colon. Any exception means this is not a valid field-name,
    // and therefore, not a valid RFC822 header.
    //
    for (const char * check = start; check < colon; check++) {
	if (*check < 33 || *check > 126) {
	    return(DTM_FALSE);
	}
    }

    // We passed the criteria, so this must be a header.
    //
    return(DTM_TRUE);
}
