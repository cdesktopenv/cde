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
 *	$TOG: RFCEnvelope.C /main/7 1998/04/06 13:27:59 mgreess $
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

#include <EUSCompat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/utsname.h>

#include <DtMail/DtMail.hh>
#include "RFCImpl.hh"
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>
#include "str_utils.h"

unsigned long RFCEnvelopeSignature = 0x55fd23ef;

// This constant defines how big the parsed header structure starts.
// it will grow dynamically over time if need be. The guess here is
// that most messages will have about 32 headers so we won't have
// to grow very often, but we also aren't wasting a lot of space.
//
static const int INITIAL_HEADER_COUNT = 32;

static const int NAME_MASK = 0x1;
static const int VALUE_MASK = 0x2;

RFCEnvelope::RFCEnvelope(DtMailEnv & error,
			 DtMail::Message * parent,
			 const char * start,
			 const int len)
: DtMail::Envelope(error, parent), _parsed_headers(INITIAL_HEADER_COUNT)
{
    error.clear();

    _header_text = start;
    _header_len = len;
    _dirty = 0;
    _header_lock = MutexInit();

    if (_header_text) {
	parseHeaders();
    }

    _object_signature = RFCEnvelopeSignature;
    //fix for the defect 177527
    _use_reply_to=DTM_TRUE;
}

RFCEnvelope::~RFCEnvelope(void)
{
    if (_object_signature == RFCEnvelopeSignature) {
	MutexLock lock_scope(_obj_mutex);
	if (_object_signature == RFCEnvelopeSignature) {
	    for (int hdr = 0; hdr < _parsed_headers.length(); hdr++) {
		ParsedHeader * hdrp = _parsed_headers[hdr];
		delete _parsed_headers[hdr];
	    }
	    _object_signature = 0;
	}
    }
}

DtMailHeaderHandle
RFCEnvelope::getFirstHeader(DtMailEnv & error,
			     char ** name,
			     DtMailValueSeq & value)
{
    MutexLock lock_header(_header_lock);

    error.clear();

    if (_parsed_headers.length() == 0) {
	return(NULL); // No headers.
    }

    DtMailHeaderHandle handle = _parsed_headers[0];

    *name = (char *)malloc(_parsed_headers[0]->name_len + 1);
    strncpy(*name, _parsed_headers[0]->name_start, _parsed_headers[0]->name_len);
    (*name)[_parsed_headers[0]->name_len] = 0;

    makeValue(error, *_parsed_headers[0], value);

    return(handle);
}

DtMailHeaderHandle
RFCEnvelope::getNextHeader(DtMailEnv & error,
			    DtMailHeaderHandle last,
			    char ** name,
			    DtMailValueSeq & value)
{
    MutexLock lock_header(_header_lock);

    error.clear();

    if (!last) {
	error.setError(DTME_BadArg);
	return(NULL);
    }

    int slot = _parsed_headers.indexof((ParsedHeader *)last);
    if (slot < 0) {
	return(NULL);
    }

    slot += 1;
    if (slot >= _parsed_headers.length()) {
	return(NULL);
    }

    ParsedHeader * hdr = _parsed_headers[slot];

    *name = (char *)malloc(hdr->name_len + 1);
    strncpy(*name, hdr->name_start, hdr->name_len);
    (*name)[hdr->name_len] = 0;

    makeValue(error, *hdr, value);

    return(hdr);
}

struct AbstractMap {
    const char *	abstract;
    const char **	transports;
};

static const char * DtMailMessageToMap[] = {
    "To", "Apparently-To", "Resent-To", NULL
};

static const char * DtMailMessageSenderMap[] = {
    "Reply-To", "From", "Return-Path", "Resent-From", NULL
};

static const char * DtMailMessageCcMap[] = {
    "Cc", NULL
};

static const char * DtMailMessageBccMap[] = {
    "Bcc", NULL
};

static const char * DtMailMessageReceivedTimeMap[] = {
    NULL
};

static const char * DtMailMessageSentTimeMap[] = {
"Date", NULL
};

static const char * DtMailMessageIdMap[] = {
    "Message-Id", NULL
};

static const char * DtMailMessageSubjectMap[] = {
    "Subject", NULL
};

static const char * DtMailMessageContentLengthMap[] = {
    "Content-Length", NULL
};

static const char * DtMailMessageStatusMap[] = {
    "Status", "X-Status", NULL
};

static const char * DtMailMessageV3charsetMap[] = {
    "X-Sun-Charset", NULL
};

static const char * DtMailMessageContentTypeMap[] = {
    "Content-Type", NULL
};

static const AbstractMap abstract_map[] = {
{ DtMailMessageTo, 		DtMailMessageToMap },
{ DtMailMessageSender,		DtMailMessageSenderMap },
{ DtMailMessageCc,		DtMailMessageCcMap },
{ DtMailMessageBcc,		DtMailMessageBccMap },
{ DtMailMessageReceivedTime,	DtMailMessageReceivedTimeMap },
{ DtMailMessageSentTime,	DtMailMessageSentTimeMap },
{ DtMailMessageMessageId,	DtMailMessageIdMap },
{ DtMailMessageSubject,		DtMailMessageSubjectMap },
{ DtMailMessageContentLength,	DtMailMessageContentLengthMap },
{ DtMailMessageStatus,		DtMailMessageStatusMap },
{ DtMailMessageV3charset,		DtMailMessageV3charsetMap },
{ DtMailMessageContentType,		DtMailMessageContentTypeMap },
{ NULL,				NULL }
};

void
RFCEnvelope::getHeader(DtMailEnv & error,
			const char * name,
			const DtMailBoolean abstract,
			DtMailValueSeq & value)
{
    MutexLock lock_header(_header_lock);

    error.clear();

    // If we are not in the abstract space, then simply get the
    // transport header and return.
    //
    if (abstract == DTM_FALSE) {
	getTransportHeader(error, name, value);
    }
    else {
	// We need to handle reply to's with special care. We may
	// need to strip the sender from the list.
	//
	if (strcmp(name, DtMailMessageToReply) == 0) {
	    makeReply(error, DtMailMessageTo, value);
	    return;
	}

	if (strcmp(name, DtMailMessageCcReply) == 0) {
	    makeReply(error, DtMailMessageCc, value);
	    return;
	}

	// Okay, we need to work out what the transport name
	// might be. We do this by first finding the abstract
	// name in the abstract->transport mapping table.
	//
	int abs = 0;
	for (abs = 0; abstract_map[abs].abstract; abs++) {
	    if (strcmp(abstract_map[abs].abstract, name) == 0) {
		break;
	    }
	}

	// If we didn't find the abstract name, then feed it through
	// as a transport name. This is useful behavior so intermediates
	// like the RFCMailBox::getMessageSummary method can always
	// assume abstract names, but allow its client to specify transport
	// names.
	//
	if (!abstract_map[abs].abstract) {
	    getTransportHeader(error, name, value);
	    return;
	}

	// Now we need to go through the list of transport names until
	// we find a match. The first match is taken because the map
	// should be ordered based on preference.
	//
	for (int trans = 0; abstract_map[abs].transports[trans]; trans++) {
	    getTransportHeader(error,
			       abstract_map[abs].transports[trans],
			       value);
        // If the abstract is  DtMailMessageSender we need do something
        // special. If the request is for displaying the sender's
        // name or e-mail address in the msg list scrolled window in the
        // RMW, we should return the "From" value instead of the 
        // "Reply-To" value. Otherwise, we should return the Reply-To
        // value (such as "Reply to sender")
        //  
	    if (error.isNotSet()) { // Found one!
               if(!getUseReplyTo() && 
                   strcmp(abstract_map[abs].transports[trans],"Reply-To") == 0)
                 {
                  value.clear();
                  continue;
                 }
                else
		  return;
	    }
	    error.clear();
	}

	// If this isn't a request for the sender ("From"), then
	// we don't have a known value for the header. Bail!
	//
	if (strcmp(DtMailMessageSender, name) && 
	    strcmp(DtMailMessageReceivedTime, name)) {
	    error.setError(DTME_NoObjectValue);
	    return;
	}

	if (strncmp(_parsed_headers[0]->name_start, "From", 4) == 0) {
	    if (strcmp(DtMailMessageSender, name) == 0) {
		parseUnixFrom(error, *_parsed_headers[0], value);
	    }
	    else if (strcmp(DtMailMessageReceivedTime, name) == 0) {
		parseUnixDate(error, *_parsed_headers[0], value);
	    }
	}
	else {
	    error.setError(DTME_NoObjectValue);
	    return;
	}
	return;
    }

    return;
}

void
RFCEnvelope::setHeader(DtMailEnv & error,
		       const char * name, 
		       const DtMailBoolean replace,
		       const char * val)
{
    MutexLock lock_header(_header_lock);

    RFCMessage * msg = (RFCMessage *)_parent;

    error.clear();

    // First we need to see if we have this header. We are
    // only interested in the first occurrence.
    //
    ParsedHeader * hdr;
    const char * real_name;

    // Find the header if it currently exists
    //
    int slot = lookupHeader(name);

    // Determine if the value is really empty (just blanks)
    //
    DtMailBoolean valueIsEmpty = DTM_TRUE;
    for (const char *cv = val; *cv; cv++) {
      if (!isspace((unsigned char)*cv)) {
	valueIsEmpty = DTM_FALSE;
	break;
      }
    }
    
    // If the value to be set is empty (only spaces), then treat this
    // set header request specially - either toss it if requesting to
    // append an empty header, or remove an existing header if requesting
    // to set an empty header
    //
    if (valueIsEmpty == DTM_TRUE) {
      if (slot < 0) {
	// The header was not found and the value is empty -
	// Just return as there was no header to set anyway
	return;
      }
      if (replace == DTM_TRUE) {
	// The header was found and replacement was requested
	// but the value is empty - request to replace existing
	// header with empty header - treat as a remove header request
	//
	removeHeader(error, name);
	return;
      }
      // The header was found, replacement is not requested, and the
      // value is empty - request to append empty header - toss
      //
      return;
    }
    
    if (slot < 0 || replace == DTM_FALSE) {
	// Need to create a new header entry for this one.
	//
	hdr = new ParsedHeader;
	slot = _parsed_headers.append(hdr);
	real_name = mapName(name);
	_dirty = 1;				// new entry: header dirty
    }
    else {
	hdr = _parsed_headers[slot];
	real_name = name;
    }

    // First, see if we need to do something about the name.
    //
    if (!hdr->name_start) {
	hdr->alloc_mask |= NAME_MASK;
	hdr->name_start = strdup(real_name);
	hdr->name_len = strlen(real_name);
	_header_len += hdr->name_len;
	_dirty = 1;				// new name: header dirty
    }

    // Clean up the existing value if need be.
    //
    if (hdr->value_start) {
      if ( (strlen(val) != hdr->value_len)
	|| (strncmp(hdr->value_start, val, hdr->value_len)!=0) )	// has value changed??
	  _dirty = 1;				// yes: header dirty
	if (hdr->alloc_mask & VALUE_MASK) {
	    free((char *)hdr->value_start);
	}
	hdr->value_start = NULL;
	_header_len -= hdr->value_len;
	hdr->value_len = 0;
    }
    else
      _dirty = 1;				// new value: header dirty

    hdr->value_start = strdup(val);
    hdr->value_len = strlen(hdr->value_start);
    _header_len += hdr->value_len + 1;
    hdr->alloc_mask |= VALUE_MASK;

    if (msg) {
      msg->markDirty(_dirty);
    }
}

void
RFCEnvelope::removeHeader(DtMailEnv & error, const char * name)
{
    MutexLock lock_header(_header_lock);

    error.clear();

    // Remove all versions of this header.
    //
    int slot = lookupHeader(name);
    while (slot >= 0) {
	_parsed_headers.remove(slot);
	slot = lookupHeader(name);
    }

    RFCMessage * msg = (RFCMessage *)_parent;
    if (msg) {
	msg->markDirty(1);
    }

    if (!_dirty) {
	_dirty += 1;
    }

    return;
}

void
RFCEnvelope::adjustHeaderLocation(char * headerStart, int headerLength)
{
    MutexLock lock_header(_header_lock);

    if (_dirty) {
      // reparse headers in their new location
      // destroy current headers
      //
      for (int hdr = 0; hdr < _parsed_headers.length(); hdr++) {
	ParsedHeader * hdrp = _parsed_headers[hdr];
	delete _parsed_headers[hdr];
	_parsed_headers.remove(hdr);
	hdr -=1;
      }
      
      // parse headers from scratch
      _header_text = headerStart;
      _header_len = headerLength;
      parseHeaders();
    }
    else {
      // We must adjust the offset of every header in the parsed header
      // structure. For those headers values that have been malloc()ed
      // (e.g. NAME_MASK or VALUE_MASK are set), don't have to do anything
      // as they are deallocated only when the header is destroyed.
      //
      for (int hdr = 0; hdr < _parsed_headers.length(); hdr++) {
	ParsedHeader * h = _parsed_headers[hdr];
	if (!(h->alloc_mask & NAME_MASK)) {
	  h->name_start = (h->name_start - _header_text) + headerStart;
	}
	if (!(h->alloc_mask & VALUE_MASK)) {
	  h->value_start = (h->value_start - _header_text) + headerStart;
	}
      }
      _header_text = headerStart;
    }
    _dirty = 0;
    return;
}

char *
RFCEnvelope::writeHeaders(char * new_loc)
{
  MutexLock lock_header(_header_lock);

  // Copy the headers to the new region
  //
  char * cur_loc = new_loc;
  int first = 1;
  for (int hdr = 0; hdr < _parsed_headers.length(); hdr++) {
    ParsedHeader * h = _parsed_headers[hdr];
    
    const char * new_name = cur_loc;
    memcpy(cur_loc, h->name_start, h->name_len);
    cur_loc += h->name_len;
    if (!first || strncmp(h->name_start, "From", h->name_len) != 0) {
      *cur_loc++ = ':';
    }
    *cur_loc++ = ' ';
    first = 0;
    
    // Copy the value
    //
    const char * new_value = cur_loc;
    memcpy(cur_loc, h->value_start, h->value_len);
    cur_loc += h->value_len;
    
    // Insert a trailing crlf if necessary.  We need this so that when
    // we write into the file, the header lines are as in RFC822 format.
    if (*(cur_loc - 1) != '\n') {
      *cur_loc++ = '\n';
    }
  }

  return(cur_loc - 1);
}


const char *
RFCEnvelope::unixFrom(DtMailEnv & error, int & length)
{
    ParsedHeader * hdr = _parsed_headers[0];
    const char * ufrom = NULL;

    length = 0;
    error.clear();

    if (strncmp(hdr->name_start, "From ", 5) == 0) {
	ufrom = hdr->name_start;
	length = (hdr->value_start + hdr->value_len) - hdr->name_start + 1;
    }
    else {
	error.setError(DTME_NoObjectValue);
    }

    return(ufrom);
}

void
RFCEnvelope::parseHeaders(void)
{
    // Now we actually parse the headers. Each header either ends with
    // a new line, or is continued if the next line begins with white
    // space.
    //
    ParsedHeader * hdr = new ParsedHeader;
    for (const char * scan = _header_text; scan < (_header_text + _header_len);) {
	if ((scan == _header_text) && (strncmp(scan, "From ", 5) == 0)) {
	    // Unix "From" line. This header has a different structure.
	    // It is "From user@host <date>". It does not have a colon
	    // like all other RFC headers so we have to parse it specially.
	    //
	    hdr->name_start = scan;
	    hdr->name_len = 4;

	    // Look for the first non-blank after the "From ".
	    //
	    for (scan += 4; *scan && isspace((unsigned char)*scan); scan++) {
		continue;
	    }

	    hdr->value_start = scan;

	    // Find the new line.
	    for (; *scan && *scan != '\n'; scan++) {
		continue;
	    }

	    hdr->value_len = scan - hdr->value_start;
	    if (*(scan - 1) == '\r') {
		hdr->value_len -= 1;
	    }

	    scan += 1;
	    _parsed_headers.append(hdr);
	    hdr = new ParsedHeader;
	    continue;
	}

	// We should be at the start of a header. Let's look for a ":". If
	// we find any white space first, then we have a problem.
	//
	hdr->name_start = scan;
	for (;*scan && *scan != ':'; scan++) {
	    if (isspace((unsigned char)*scan)) {
		break;
	    }
	}

	if (*scan != ':') {
	    // Find the next new line, and try again.
	    //
	    for (;*scan && *scan != '\n'; scan++) {
		continue;
	    }
	    scan += 1;
	    continue;
	}

	hdr->name_len = scan - hdr->name_start;

	// Look for the first non-blank after the colon.
	//
	for (scan += 1; scan < (_header_text + _header_len) 
	     && *scan != '\n' && isspace((unsigned char)*scan); scan++) {
	    continue;
	}

	if (*scan == '\n') {
	    // Null value!
	    hdr->value_start = scan;
	    hdr->value_len = 0;
	    _parsed_headers.append(hdr);
	    hdr = new ParsedHeader;
	    scan += 1;
	    continue;
	}

	// Okay, now we want scan looking for a new line that is
	// not followed immediately by white space. That will give
	// us the end of the header.
	//
	hdr->value_start = scan;
	for (;scan < (_header_text + _header_len); scan++) {
	    if (*scan == '\n' && !isspace((unsigned char)*(scan + 1))) {
		break;
	    }
	}

	hdr->value_len = scan - hdr->value_start;
	if (*(scan - 1) == '\r') {
	    hdr->value_len -= 1;
	}

	scan += 1;
	_parsed_headers.append(hdr);
	hdr = new ParsedHeader;
    }

    // Made one to many.
    //
    delete hdr;
}

void
RFCEnvelope::getTransportHeader(DtMailEnv & error,
				 const char * name,
				 DtMailValueSeq & value)
{
    error.clear();

    // First, let's try to find out how many times the header
    // appears. It may appear 1, many, or not at all.
    //
    int appears = 0;
    for (int hdr = 0; hdr < _parsed_headers.length(); hdr++) {
	// We need to lock the object until we are done.
	//
	MutexLock lock_header(_parsed_headers[hdr]->mutex);

	// Make sure we have a header!
	//
	if (!_parsed_headers[hdr]->name_start ||
	    !_parsed_headers[hdr]->value_start) {
	    continue;
	}

	// Unix From doesn't count. We only use it as a fall back.
	// It will always appear as the first header, if it appears
	// at all.
	//
	if (hdr == 0 &&	
	    strncmp(_parsed_headers[hdr]->name_start, "From ", 5) == 0) {
	    continue;
	}

	if (matchName(*_parsed_headers[hdr], name) == DTM_TRUE) {
	    // If the header exists, make sure it has a value.
	    if (_parsed_headers[hdr]->value_len > 0)
		appears += 1;
	}
    }

    if (appears == 0) { // Not here!
	error.setError(DTME_NoObjectValue);
	return;
    }

    // Second pass, find the headers and convert the values to the
    // appropriate type.
    //
    int ent = 0;
    for (int val = 0; val < _parsed_headers.length(); val++) {
	if (val == 0 &&	
	    strncmp(_parsed_headers[val]->name_start, "From ", 5) == 0) {
	    continue;
	}

	if (matchName(*_parsed_headers[val], name) == DTM_TRUE) {
	    RFCValue * new_value = new RFCValue(_parsed_headers[val]->value_start,
						_parsed_headers[val]->value_len, _parent->session());
	    value.append(new_value);
	}
    }

    return;
}

void
RFCEnvelope::parseUnixFrom(DtMailEnv & error,
			    const ParsedHeader & hdr,
			    DtMailValueSeq & value)
{
    error.clear();

    // The value_start will point to the beginning of the address.
    // The Unix From header doesn't maintain all of the strange
    // quoting behavior so spaces don't appear.
    //
    const char *end;
    for (end = hdr.value_start; *end && !isspace((unsigned char)*end); end++) {
	continue;
    }

    int size = end - hdr.value_start;

    RFCValue * new_value = new RFCValue(hdr.value_start, size, _parent->session());
    value.append(new_value);

    return;
}

void
RFCEnvelope::parseUnixDate(DtMailEnv & error,
			   const ParsedHeader & hdr,
			   DtMailValueSeq & value)
{
    error.clear();

    // The value_start will point to the beginning of the address.
    // The Unix From header doesn't maintain all of the strange
    // quoting behavior so spaces don't appear.
    //
    const char * end;
    for (end = hdr.value_start; end < (hdr.value_start + hdr.value_len) 
	 && !isspace((unsigned char)*end); end++) {
	continue;
    }

    // Now end points at the white space between the sender and
    // the date it arrived.
    //
    for (; end < (hdr.value_start + hdr.value_len) &&
	 isspace((unsigned char)*end); end++) {
	continue;
    }

    // Now we are at the start of the date.
    //
    int size = (hdr.value_start + hdr.value_len) - end;

    RFCValue * new_value = new RFCValue(end, size, _parent->session());
    value.append(new_value);

    return;
}

void
RFCEnvelope::makeValue(DtMailEnv & error,
			const ParsedHeader & hdr,
			DtMailValueSeq & value)
{
    error.clear();

    RFCValue * new_value = new RFCValue(hdr.value_start, hdr.value_len, _parent->session());
    value.append(new_value);
}

DtMailBoolean
RFCEnvelope::matchName(const ParsedHeader & hdr, const char * name)
{
    if (hdr.name_len != strlen(name)) {
	return(DTM_FALSE);
    }

    if (strncasecmp(hdr.name_start, name, hdr.name_len) == 0) {
	return(DTM_TRUE);
    }

    return(DTM_FALSE);
}

int
RFCEnvelope::lookupHeader(const char * name, DtMailBoolean real_only)
{
    // Look for the name in real space.
    //
    int hdr;
    int len = strlen(name);
    for (hdr = 0; hdr < _parsed_headers.length(); hdr++) {
	if (_parsed_headers[hdr]->name_start &&
	    strncasecmp(_parsed_headers[hdr]->name_start, name, len) == 0) {
	    return(hdr);
	}
    }

    if (real_only == DTM_TRUE) {
	return(-1);
    }

    for (const AbstractMap * abs = abstract_map; abs->abstract; abs++) {
	if (strcmp(abs->abstract, name) == 0) {
	    for (int trans = 0; abs->transports[trans]; trans++) {
		int slot = lookupHeader(abs->transports[trans], DTM_TRUE);
		if (slot >= 0) {
		    return(slot);
		}
	    }
	}
    }

    return(-1);
}

void
RFCEnvelope::makeReply(DtMailEnv & error,
		       const char * name,
		       DtMailValueSeq & value)
{
    error.clear();

    // We have an abstract name. We need to loop through the
    // transport headers, gathering up the values.
    //
    int abs = 0;
    for (abs = 0; abstract_map[abs].abstract; abs++) {
	if (strcmp(abstract_map[abs].abstract, name) == 0) {
	    break;
	}
    }

    if (!abstract_map[abs].abstract) {
	error.setError(DTME_NoObjectValue);
    }

    // Now we need to go through the list of transport names until
    // we find a match. The first match is taken because the map
    // should be ordered based on preference.
    //
    DtMailBoolean found = DTM_FALSE;
    DtMailValueSeq lvalue;
    for (int trans = 0; abstract_map[abs].transports[trans]; trans++) {
	getTransportHeader(error,
			   abstract_map[abs].transports[trans],
			   lvalue);
	if (error.isNotSet()) { // Found one!
	    found = DTM_TRUE;
	    break;
	}
	error.clear();
    }

    if (found == DTM_FALSE) {
	error.setError(DTME_NoObjectValue);
	return;
    }

    if (_parent == NULL) {
	// We're done. Copy the values from one to the other.
	//
	for (int nc = 0; nc < lvalue.length(); nc++) {
	    RFCValue * new_value = new RFCValue(*(lvalue[nc]), strlen(*(lvalue[nc])), _parent->session());
	    value.append(new_value);
	}
	return;
    }

    DtMail::MailRc * mailrc = _parent->session()->mailRc(error);

    // If metoo is set, then we are also done.
    //
    const char * mval;
    DtMailEnv lerror;
    mailrc->getValue(lerror, "metoo", &mval);
    if (lerror.isNotSet()) {
	for (int nc = 0; nc < lvalue.length(); nc++) {
	    RFCValue * new_value = new RFCValue(*(lvalue[nc]), strlen(*(lvalue[nc])), _parent->session());
	    value.append(new_value);
	}
	return;
    }

    // Finally, the point of being here! We need to build a new
    // value that has the user stripped from the reply list. This
    // includes any alternates the user has specified.
    //
    passwd pw;
    GetPasswordEntry(pw);
    struct utsname uname_val;
    uname(&uname_val);

    char * my_addr = new char[strlen(pw.pw_name) + strlen(uname_val.nodename) + 5];
    strcpy(my_addr, pw.pw_name);
    strcat(my_addr, "@");
    strcat(my_addr, uname_val.nodename);

    DtMailAddressSeq alts;
    DtMailValueAddress * me = new DtMailValueAddress(my_addr,
						     pw.pw_gecos,
						     DtMailAddressDefault);
    alts.append(me);

    // Fetch the alternates, if any and add them to the list to strip.
    //
    const char * others = mailrc->getAlternates(lerror);
    if (others) {
	RFCTransport::arpaPhrase(others, alts);
    }

    // Finally, we need to see if the user wants us to ignore the host
    // component of the address when stripping.
    //
    DtMailBoolean allnet = DTM_FALSE;
    mailrc->getValue(lerror, "allnet", &mval);
    if (lerror.isNotSet()) {
	allnet = DTM_TRUE;
    }

    DtMailAddressSeq keepers;

    for (int nc = 0; nc < lvalue.length(); nc++) {
	DtMailAddressSeq * cur_val = lvalue[nc]->toAddress();
	for (int naddr = 0; naddr < cur_val->length(); naddr++) {
	    if (metooAddr(*(*cur_val)[naddr], alts, allnet) == DTM_FALSE) {
		DtMailValueAddress * kaddr = new DtMailValueAddress(*(*cur_val)[naddr]);
		keepers.append(kaddr);
	    }
	}
    }

    // Finally, let's build a single string from the remaining list and
    // set the value to that string.
    //
    int max_len = 0;
    for (int cstr = 0; cstr < keepers.length(); cstr++) {
	DtMailValueAddress * addr = keepers[cstr];
	max_len += strlen(addr->dtm_address) + 3;
    }

    if (max_len > 0) {
	char * str_addr = new char[max_len];
	str_addr[0] = 0;
	for (int copy = 0; copy < keepers.length(); copy++) {
	    DtMailValueAddress * addr = keepers[copy];
	    strcat(str_addr, addr->dtm_address);
	    if (copy != (keepers.length() - 1)) {
		strcat(str_addr, ", ");
	    }
	}

	RFCValue * new_val = new RFCValue(str_addr, strlen(str_addr), _parent->session());
	value.append(new_val);
	delete str_addr;
    }
}

char *
striphosts(char * addr)
{
    char *cp, *cp2;
    
    if ((cp = strrchr(addr,'!')) != NULL)
	cp++;
    else
	cp = addr;
    /*
     * Now strip off all Internet-type
     * hosts.
     */
    if ((cp2 = strchr(cp, '%')) == NULL)
	cp2 = strchr(cp, '@');
    if (cp2 != NULL)
	*cp2 = '\0';
    return(cp);
}

DtMailBoolean
RFCEnvelope::metooAddr(DtMailValueAddress & addr,
		       DtMailAddressSeq & alts,
		       DtMailBoolean allnet)
{
    char * str_addr_buf = strdup(addr.dtm_address);
    char * str_addr = str_addr_buf;

    if (allnet) {
	str_addr = striphosts(str_addr_buf);
    }

    for (int nalt = 0; nalt < alts.length(); nalt++) {
	char * cmp_addr_buf = strdup(alts[nalt]->dtm_address);
	char * cmp_addr = cmp_addr_buf;

	if (allnet) {
	    cmp_addr = striphosts(cmp_addr_buf);
	}

	if (strcasecmp(str_addr, cmp_addr) == 0) {
	    free(str_addr_buf);
	    free(cmp_addr_buf);
	    return(DTM_TRUE);
	}

	free(cmp_addr_buf);
    }

    free(str_addr_buf);
    return(DTM_FALSE);
}


// This method attempts to map an abstract name to a transport name.
// When multiple transport names exist, the first is always used.
//
const char *
RFCEnvelope::mapName(const char * name)
{
    for (const AbstractMap * abs = abstract_map; abs->abstract; abs++) {
	if (strcmp(name, abs->abstract) == 0) {
	    return(abs->transports[0]);
	}
    }

    // Must be a real name.
    //
    return(name);
}

RFCEnvelope::ParsedHeader::ParsedHeader(void)
{
    mutex = MutexInit();
    alloc_mask = 0;
    name_start = NULL;
    name_len = 0;
    value_start = NULL;
    value_len = 0;
}

RFCEnvelope::ParsedHeader::~ParsedHeader(void)
{
    MutexLock lock_scope(mutex);

    if (alloc_mask & NAME_MASK) {
	free((char *)name_start);
    }

    if (alloc_mask & VALUE_MASK) {
	free((char *)value_start);
    }
    
    lock_scope.unlock_and_destroy();
}
