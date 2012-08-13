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
 *	$TOG: RFCFormat.C /main/11 1998/07/23 18:03:02 mgreess $
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

#include <ctype.h>
#include <EUSCompat.h>
#include <DtMail/IO.hh>
#include "RFCFormat.hh"
#include "str_utils.h"

#include <time.h>

RFCFormat::RFCFormat(DtMail::Session * session)
{
    _session = session;
    _is_write_bcc = DTM_FALSE;
}

RFCFormat::~RFCFormat(void)
{
}

void
RFCFormat::msgToBuffer(DtMailEnv & error,
		       DtMail::Message & msg,
		       DtMailBoolean include_content_length,
		       DtMailBoolean include_unix_from,
		       DtMailBoolean,
		       Buffer & headers,
		       Buffer & body)
{
    error.clear();

    _use_cr = DTM_TRUE;

    if (include_content_length || include_unix_from) {
	_use_cr = DTM_FALSE;
    }

    // We will format the bodies first, then the headers. The
    // reason we do this is that the result of formatting the body
    // is required for the headers. The content-type, content-length,
    // and other content based headers need to be computed before
    // being written.
    //
    char * extra_headers = NULL;
    formatBodies(error, msg, include_content_length, &extra_headers, body);
    if (error.isSet()) {
	return;
    }

    formatHeaders(error, msg, include_unix_from, extra_headers, headers);
    free(extra_headers);
    return;
}

void
RFCFormat::writeHeaders(DtMailEnv & error,
			DtMail::Message & msg,
			DtMailBoolean include_unix_from,
			const char * extra_headers,
			const char ** suppress_headers,
			Buffer & buf)
{
    error.clear();

    // First we copy each header from the message to the
    // buffer. The headers may need encoding to put them away, so
    // we will apply RFC1522 if necessary.
    //
    DtMailHeaderHandle hnd;
    DtMail::Envelope * env = msg.getEnvelope(error);
    if (error.isSet()) {
	return;
    }

    char * name = NULL;
    DtMailValueSeq value;

    hnd = env->getFirstHeader(error, &name, value);
    if (!hnd || error.isSet()) {
	return;
    }

    if (include_unix_from &&
	(error.isSet() || strcmp(name, "From") != 0)) {
	// We require a Unix from line, and we don't have one.
	// we will make one up using the sender, and the current
	// date.
	//
	char *unix_from = new char[100];
	strcpy(unix_from, "From ");
	
	DtMailValueSeq sender;
	env->getHeader(error, DtMailMessageSender, DTM_TRUE, sender);
	if (error.isSet()) {
	    // We no longer know who this is really from.
	    //
	    strcat(unix_from, "nobody@nowhere");
	}
	else {
	    DtMailAddressSeq * addrSeq = sender[0]->toAddress();
	    
	    strcat(unix_from, (*addrSeq)[0]->dtm_address);
	    delete addrSeq;
	}
	error.clear();

	time_t now = time(NULL);
	char time_format[30];
	
	SafeCtime(&now, time_format, sizeof(time_format));
	
	strcat(unix_from, " ");
	strcat(unix_from, time_format);
	buf.appendData(unix_from, strlen(unix_from));
	delete [] unix_from;
    }
    else {
	// Put out any header, except Unix From line
	//
	if (strcmp(name, "From") == 0) {
	    value.clear();
	    free(name);
	    hnd = env->getNextHeader(error, hnd, &name, value);
	}
    }

    for (; // First time is determined above.
	 hnd && !error.isSet();
	 value.clear(), hnd = env->getNextHeader(error, hnd, &name, value)) {

	const char **hdr;
	for (hdr = suppress_headers; *hdr; hdr++) {
	    if (strcasecmp(name, *hdr) == 0)
	      break;
	}

        //add _is_write_bcc for fixing aix defect 177096
	if (*hdr || strcasecmp(name, "bcc") == 0 && !_is_write_bcc ) {
	    free(name);
	    continue; // We will generate these headers.
	}

	int name_len = strlen(name);

	for (int val = 0; val < value.length(); val++) {
	    //
	    // If the value is null or empty do not emit this field
	    const char *valPtr;
	    for (valPtr = *(value[val]);
		 *valPtr && (isspace((unsigned char)*valPtr));
		 valPtr++)
	    {}
	    if (!*valPtr)
	      continue;
	    
	    buf.appendData(name, name_len);
	    buf.appendData(": ", 2);
	    rfc1522cpy(buf, *(value[val]));
	}

	free(name);
    }
    error.clear();

    buf.appendData(extra_headers, strlen(extra_headers));

    // Add new line that terminates the headers.
    //
    crlf(buf);
}

void
RFCFormat::rfc1522cpy(Buffer & buf, const char * value)
{
    buf.appendData(value, strlen(value));
    crlf(buf);
}

void
RFCFormat::getCharSet(char * charset)
{
	char *mimeCS = NULL;

        mimeCS = _session->targetTagName();

    if (mimeCS) {
        strcpy(charset, mimeCS);
        free(mimeCS);
    } else {
	strcpy(charset, "us-ascii");   /* default MIME codeset */
    }
}

void
RFCFormat::getCharSet(char * charset, char *special)
{
	char *mimeCS = NULL;

        mimeCS = _session->targetTagName(special);

    if (mimeCS) {
       strcpy(charset, mimeCS);
       free(mimeCS);
    } else {
	strcpy(charset, "us-ascii");   /* default MIME codeset */
    }
}
