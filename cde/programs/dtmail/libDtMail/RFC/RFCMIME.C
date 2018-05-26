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
 *	$TOG: RFCMIME.C /main/13 1998/07/24 16:09:00 mgreess $
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
 *	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef I_HAVE_NO_IDENT
#endif

#include <EUSCompat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <Dt/Dts.h>

#include "md5.h"
#include "RFCMIME.hh"
#include <DtMail/DtMailP.hh>
#include <DtMail/IO.hh>
#include "str_utils.h"

// For CHARSET
#include <limits.h>
static int converted = 0;

#if defined(POSIX_THREADS)
extern "C" int rand_r(unsigned int);
#define brand(a)	rand_r(a)
#else
#define brand(a) 	rand()
#endif

#define DIRECTION_STRING(dir) ((dir==CURRENT_TO_INTERNET) ? \
				"CURRENT_TO_INTERNET" : \
				"INTERNET_TO_CURRENT")

#define ENCODING_STRING(enc) ((enc==MIME_7BIT) ? "MIME_7BIT" : \
			      ((enc==MIME_8BIT) ? "MIME_8BIT" : \
			       ((enc==MIME_QPRINT) ? "MIME_QPRINT" : \
			       "MIME_BASE64")))

#define	NON_MAIL_SAFE(char) (((char >= 0) && (char <= 31) && (char != 9) && \
		(char != 0x0a)) || (char == 127))

inline unsigned int
base64size(const unsigned long len)
{
    unsigned long b_len = len + (len / 3);
    b_len += (b_len / 72 * 2) + 4;

    return((unsigned int) b_len);
}

void
RFCMIME::getMIMEType(DtMail::BodyPart * bp, char * mime_type, DtMailBoolean & is_text)
{
    // Get the Dt type name from the body part.
    //
    char * type = 0;
    DtMailEnv error;

    bp->getContents(error,
		    NULL,
		    NULL,
		    &type,
		    NULL,
		    NULL,
		    NULL);

    // It is possible there is *no* contents associated with this
    // body part - in that case, fake text/plain
    //
    if (error.isSet()) {
      strcpy(mime_type, "text/plain");
      return;
    }
    assert(type != NULL);
    
    // Look it up in the data typing system. Hopefully we will
    // get a db based mime name.
    //
    char * db_type = DtDtsDataTypeToAttributeValue(type,
						   DtDTS_DA_MIME_TYPE,
						   NULL);

    // See if we call this text. If so, then it will be text/plain,
    // if not then application/octet-stream
    //
    char * text_type = DtDtsDataTypeToAttributeValue(type,
						     DtDTS_DA_IS_TEXT,
						     NULL);

    if (db_type) {
	strcpy(mime_type, db_type);
    }
    else {
	if (text_type && strcasecmp(text_type, "true") == 0) {
	    strcpy(mime_type, "text/plain");
	}
	else {
	    strcpy(mime_type, "application/octet-stream");
	}
    }

    is_text = text_type ? DTM_TRUE : DTM_FALSE;

    free(type);
    if (db_type) {
	free(db_type);
    }
    if (text_type) {
	free(text_type);
    }

    return;
}

RFCMIME::RFCMIME(DtMail::Session * session)
: RFCFormat(session)
{
}

RFCMIME::~RFCMIME(void)
{
}


RFCMIME::Encoding
RFCMIME::getEncodingType(const char * body,
			 const unsigned int len,
			 DtMailBoolean strict_mime,
			 int *real8bit)
{
  // Our goal here is to produce the most readable, safe encoding.
  // We have a couple of parameters that will guide our
  // choices:
  //
  // 1) RFC822 allows lines to be a minimum of 1000 characters,
  //    but MIME encourages mailers to keep lines to <76 characters
  //    and use quoted-printable if necessary to achieve this.
  //
  // 2) The base64 encoding will grow the body size by 33%, and
  //    also render it unreadable by humans. We don't want to use
  //    it unless really necessary.
  //
  // Given the above 2 rules, we want to scan the body part and
  // select an encoding. The 3 choices will be decided by:
  //
  // 1) If the text is 7 bit clean, and all lines are <76 chars,
  //    then no encoding will be applied.
  //
  // 2) If the text is not 7 bit clean, or there are lines >76 chars,
  //    and the quoted-printable size is less than the base64 size,
  //    then quoted-printable will be done.
  //
  // 3) If 1 & 2 are not true, then base64 will be applied.
  //
  // If "strict_mime" is false we will only encode if the message:
  //    - is not 7 bit clean
  //    - if any non-printing characters non-spacing characers
  //
  
  if (body == NULL || len == 0) {
    return(MIME_7BIT);
  }
  
  const int base64_growth = base64size(len) - len;
  int qprint_growth = 0;
  DtMailBoolean seven_nonprinting = DTM_FALSE;
  DtMailBoolean eight_bit = DTM_FALSE;
  DtMailBoolean encode = DTM_FALSE;
  
  const char * last_nl = body;
  const char * cur;
  
  if (strncmp(body, "From ", 5) == 0) {
    qprint_growth += 2;
  }
  
  for (cur = body; cur < (body + len); cur++) {
    char curChar = *cur;
    
    if (curChar != (curChar & 0x7f)) {
      eight_bit = DTM_TRUE;
      encode = DTM_TRUE;
      qprint_growth += 2;
	  *real8bit = 1;
    }
    else if (curChar == '=') {
      // These characters don't force encoding, but will be 
      // encoded if we end up encoding.
      qprint_growth += 2;
    }
    else if ( ((curChar < ' ') || (curChar == 0x7f)) 
            && (curChar != 0x09) && (curChar != 0x0A) ) {
      // These characters force encoding
      //
      seven_nonprinting = DTM_TRUE;
      encode = DTM_TRUE;
      qprint_growth += 2;
    }

    if (curChar == '\n') {
#ifdef DEAD_WOOD
      DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
      
      if ((cur - last_nl) > 76) {
	encode = DTM_TRUE;
	qprint_growth += 2;
      }
      
      if ((cur != body && (*(cur - 1) == ' ' || *(cur - 1) == '\t'))) {
	encode = DTM_TRUE;
	qprint_growth += 2;
      }
      
      if ( ((cur + 6) < (body + len) )
	   && (strncmp((cur + 1), "From ", 5) == 0) ) {
	encode = DTM_TRUE;
	qprint_growth += 2;
      }
      
      last_nl = cur + 1;
    }
    
  }
  
  // Deal with buffers that don't end with a new line.
  //
  if ((cur - last_nl) > 76) {
    encode = DTM_TRUE;
    qprint_growth += 2;
  }
  
  Encoding enc = MIME_7BIT;
  
  if (!strict_mime && !eight_bit && !seven_nonprinting) {
    // If strict_mime is off we only encode if we have 8 bit data
    // of most of the non-printing 7-bit characters
    //
    enc = MIME_7BIT;
  }
  else if (encode) {
    // strict_mime is TRUE and we have reason to encode.
    if (qprint_growth > base64_growth) {
      enc = MIME_BASE64;
    }
    else {
      enc = MIME_QPRINT;
    }
  }
  
  return(enc);
}

#ifdef NEVER
// This was the original routine, but it did not handle
// strictmime correctly as it was too eager to encode.  I'm
// leaving it here for now since this is touchy code.
RFCMIME::Encoding
RFCMIME::getEncodingType(const char * body,
			 const unsigned int len,
			 DtMailBoolean strict_mime)
{
    // Our goal here is to produce the most readable, safe encoding.
    // We have a couple of parameters that will guide our
    // choices:
    //
    // 1) RFC822 allows lines to be a minimum of 1000 characters,
    //    but MIME encourages mailers to keep lines to <76 characters
    //    and use quoted-printable if necessary to achieve this.
    //
    // 2) The base64 encoding will grow the body size by 33%, and
    //    also render it unreadable by humans. We don't want to use
    //    it unless really necessary.
    //
    // Given the above 2 rules, we want to scan the body part and
    // select an encoding. The 3 choices will be decided by:
    //
    // 1) If the text is 7 bit clean, and all lines are <76 chars,
    //    then no encoding will be applied.
    //
    // 2) If the text is not 7 bit clean, or there are lines >76 chars,
    //    and the quoted-printable size is less than the base64 size,
    //    then quoted-printable will be done.
    //
    // 3) If 1 & 2 are not true, then base64 will be applied.
    //

    if (body == NULL || len == 0) {
	return(MIME_7BIT);
    }

    const int base64_growth = base64size(len) - len;
    int qprint_growth = 0;
    DtMailBoolean eight_bit = DTM_FALSE;
    DtMailBoolean base64 = DTM_FALSE;

    const char * last_nl = body;

    if (strncmp(body, "From ", 5) == 0) {
	qprint_growth += 2;
    }

    for (const char * cur = body; cur < (body + len); cur++) {
	if (*cur != (*cur & 0x7f) || *cur == '=' || *cur == 0) {
	    eight_bit = DTM_TRUE;
	    qprint_growth += 2;
	}

	if (*cur == '\n') {
#ifdef DEAD_WOOD
	    DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	    
	    if (strict_mime && ((cur - last_nl) > 76)) {
		qprint_growth += 2;
	    }

	    if (strict_mime && 
		(cur != body && (*(cur - 1) == ' ' || *(cur - 1) == '\t'))) {
		qprint_growth += 2;
	    }

	    if ((cur + 6) < (body + len) && strncmp((cur + 1), "From ", 5) == 0) {
		qprint_growth += 2;
	    }

	    last_nl = cur + 1;
	}

	if (qprint_growth > base64_growth) {
	    base64 = DTM_TRUE;
	    break;
	}
    }

    // Deal with buffers that don't end with a new line.
    //
    if (strict_mime && ((cur - last_nl) > 76)) {
	qprint_growth += 2;
    }

    Encoding enc = MIME_7BIT;

    if (base64 == DTM_TRUE) {
	enc = MIME_BASE64;
    }
    else if (eight_bit == DTM_TRUE || qprint_growth > 0) {
	enc = MIME_QPRINT;
    }

    return(enc);
}
#endif

RFCMIME::Encoding
RFCMIME::getClearEncoding(const char * bp, const unsigned int bp_len, int *real8bit)
{
    // Return the appropriate "non-encoding". If we have 8 bit data,
    // then return 8bit. If not, then this is a 7bit encoding.
    //
    for (const char * cur = bp; cur < (bp + bp_len); cur++) {
	if (*cur != (*cur & 0x7f)) {
		*real8bit = 1;
	    return(MIME_8BIT);
	}
    }

    return(MIME_7BIT);
}

void
RFCMIME::writeContentHeaders(Buffer & hdr_buf,
			     const char * type, const char * name,
			     const Encoding enc,
			     const char * digest,
			     DtMailBoolean show_as_attachment,
			     int is2022ASCII )
{
    hdr_buf.appendData("Content-Type: ", 14);
    hdr_buf.appendData(type, strlen(type));

    if (strcasecmp(type, "text/plain") == 0) {
	char default_charset[64];
	if (!is2022ASCII && !converted) {
	  strcpy(default_charset, "us-ascii");
	} else {
	  getCharSet(default_charset);
	}

	int len = strlen(default_charset);

	hdr_buf.appendData("; charset=", 10);

	hdr_buf.appendData(default_charset, len);
    }

    crlf(hdr_buf);

    hdr_buf.appendData("Content-Transfer-Encoding: ", 27);

    // For CHARSET
	// If codeset conversion was done and strict mime mode is off,
	// then transfer encoding should not be done and therefore, should
	// set transfer encoding value to "binary".
	DtMailEnv error;
    DtMail::MailRc * mail_rc = _session->mailRc(error);

    DtMailBoolean strict_mime = DTM_FALSE;
    const char * rc_value;
    mail_rc->getValue(error, "strictmime", &rc_value);
    if (error.isNotSet()) {
	strict_mime = DTM_TRUE;
    }
    error.clear();
	// Read code below to see why and how strict_mime is used.
	// End of For CHARSET

    switch (enc) {
      case MIME_7BIT:
	hdr_buf.appendData("7bit", 4);
	crlf(hdr_buf);
	break;

      case MIME_8BIT:
      default: // Assume the worst.
	hdr_buf.appendData("8bit", 4);
	crlf(hdr_buf);
	break;

      case MIME_QPRINT:
	if (converted && !strict_mime) {
	hdr_buf.appendData("binary", 6);
	} else {
	hdr_buf.appendData("quoted-printable", 16);
	}
	crlf(hdr_buf);
	break;

      case MIME_BASE64:
	if (converted && !strict_mime) {
	hdr_buf.appendData("binary", 6);
	} else {
	hdr_buf.appendData("base64", 6);
	}
	crlf(hdr_buf);
	break;
    }

    hdr_buf.appendData("Content-MD5: ", 13);
    writeBase64(hdr_buf, digest, 16);

    if (name) {
	// Determine real length of the file name for the body part.
	// Make sure we don't have any trailing crlf to confuse things.
	//
	int nlen = strlen(name);
	if (name[nlen - 1] == '\n') {
	    nlen -= 1;
	}

	if (name[nlen - 1] == '\r') {
	    nlen -= 1;
	}

	char * tempName = strdup(name);
	assert(tempName != NULL);

#if 0
	// Now make sure that the name is "unix friendly"; convert
	// any meta-characters into something innocuous
	//
	for (int i = 0; i < nlen; i++)
	  if (tempName[i] < ' ')
	    tempName[i] = '-';		// all control characters are unfriendly
	  else
	    switch(tempName[i]) {
	    case '~':
	      if (i > 0)		// ~ at beginning of a line is special
		break;
	    case '?':
	    case '*':
	    case '/':
	    case ' ':
	    case '{':
	    case '}':
	    case '[':
	    case ']':
	      tempName[i] = '-';	// "-" in unix and dos in innocuous
	      break;
	    default:
	      break;
	    }
#endif
	
	// According to RFC1806, the Content-Disposition header field
	// Should be used to specify filenames.  However, the content
	// disposition field is also used to specify how a body part
	// is presented, inline or attachment.  The CDE mailer always
	// presents the first bp inline and all others as attachments.
	// 
	hdr_buf.appendData("Content-Disposition: ", 21);
	if (show_as_attachment)
	  hdr_buf.appendData("attachment; ", 12);
	else
	  hdr_buf.appendData("inline; ", 8);
	hdr_buf.appendData("filename=", 9);
	rfc1522cpy(hdr_buf, tempName);

	// Output Header: Content-Description: <body-part-name>
	// This is in keeping with the current undefined nature
	// of file names for body parts in RFC 1521 and is
	// compatible with OpenWindows mailtool.
	//
	hdr_buf.appendData("Content-Description: ", 21);
	rfc1522cpy(hdr_buf, tempName);

        // The next crlf() is not needed because it is taken care of in 
        // rfc1522cpy().  This additional call to crlf() causes attachments
        // to be typed incorrectly.  For example, a shell script will be typed
        // as a text file.
//	crlf(hdr_buf);

#if 0
    // THE FOLLOWING IS IFDEF'ED OUT BECAUSE (from MIMEBodyPart.C)
	// CDE DtMail currently sends out and recognizes "X-Content-Name" on MIME
	// compliant entities and uses that information as the "file name" for an
	// attachment. This is essentially a non-standard header field as per RFC
	// 1521:
	//
	//     "X-" fields may be created for experimental or private purposes,
	//     with the recognition that the information they contain may be
	//     lost at some gateways.
	//
	// Output Header: X-Content-Name: <body-part-name>
	// This is only to maintain compatibility with older
	// versions of DtMail that used this field only.
	// WE SHOULD REMOVE THIS AFTER THE OFFICIAL CDE RELEASE.
	//
	hdr_buf.appendData("X-Content-Name: ", 16);
	hdr_buf.appendData(tempName, nlen);
	crlf(hdr_buf);
#endif

	free(tempName);		// done with tempName
    }
}

//
// Base64 Alphabet (65-character subset of US-ASCII as per RFC1521)
//

static const char base64_chars[] = 
{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a',
 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0',
 '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

inline unsigned char
base64inv(const char cur)
{
    switch (cur) {
      case '+':
	return(62);

      case '/':
	return(63);

      default:
	break;
    }

    if ((cur - '0') < 10) {
	return(cur - '0' + 52);
    }

    if ((cur - 'A') < 26) {
	return(cur - 'A');
    }

    if ((cur - 'a') < 26) {
	return(cur - 'a' + 26);
    }

    return(0);
}

//
// RFCMIME::readBase64 -- decode base 64 text into clear text
// Arguments:
//  char * buf		-- buffer into which clear text is to be placed
//  int & off		-- offset into buffer when placing clear text
//			-- incremented as bytes are placed into buffer
//  char *bp		-- buffer from which base 64 text is retrieved
//  const unsigned long len -- number of bytes of base 64 text at bp
// Returns:
//  <void>
// Description:
//  Given a pointer/length to base 64 text, decode the base 64 text into
//  ordinary clear text. The decoding process proceeds from left to right
//  (buffer forward) decoding groups of 4-byte encoded octets into groups
//  of 3-byte clear text triplets. The last octet may contain padding as
//  necessary to represent a data stream that is not a multiple of 24 bits.
//

void
RFCMIME::readBase64(char * buf, int & off, const char * bp, const unsigned long len)
{
    // Make local copy of base 64 text length
    //
    unsigned long main_len = len;

    // Strip trailing white space from base 64 text so we can find the magic '=' 
    // characters at the end which indicate if padding has been used to pad the
    // last octet in the data stream
    //
    while (isspace(bp[main_len - 1])) {
	main_len -= 1;
    }

    // Check to see if the last byte of the final octet in the base 64 text has
    // the magic '=' character - if it does, bump down the base 64 text stream 
    // count by 1 octet and leave decoding of the last octet until after all
    // unpadded octets have been decoded
    //
    if (bp[main_len - 1] == '=') {
	// We have normal base64 padding, which consists of one or two '=' characters
        // at the end of the stream. Ignore it for now and process at the very end.
	//
        main_len -= 4;	// backup over last 4-byte encoding containing padding
    }

    // Main octet decoding loop. We are assured that main_len is mod 4, so
    // we can zip through decoding full octets
    //
    for (const char * cur = bp; cur < (bp + main_len - 3); cur += 4) {
	while (*cur == ' ') {
	    cur += 1;
	}

	if (*cur == '\r') {
	    cur += 1;
	}
	if (*cur == '\n') {
	    cur += 1;
	}
	unsigned char b1 = base64inv(*cur);

	if (*(cur + 1) == '\r') {
	    cur += 1;
	}
	if (*(cur + 1) == '\n') {
	    cur += 1;
	}
	unsigned char b2 = base64inv(*(cur + 1));

	if (*(cur + 2) == '\r') {
	    cur += 1;
	}
	if (*(cur + 2) == '\n') {
	    cur += 1;
	}
	unsigned char b3 = base64inv(*(cur + 2));

	if (*(cur + 3) == '\r') {
	    cur += 1;
	}
	if (*(cur + 3) == '\n') {
	    cur += 1;
	}
	unsigned char b4 = base64inv(*(cur + 3));

	buf[off++] = (char)((unsigned char)(b1 << 2) & 0xfc) | ((unsigned char)(b2 >> 4) & 0x3);
	buf[off++] = (char)((unsigned char)(b2 & 0xf) << 4) | ((unsigned char)(b3 >> 2) & 0xf);
	buf[off++] = (char)((unsigned char)(b3 & 0x3) << 6) | ((unsigned char)(b4 & 0x3f));
    }

    // Throw away any white space left in base 64 text, just in case.
    //
    while (main_len < len && isspace(bp[main_len])) {
	main_len += 1;
    }

    // If end of stream was padded, there will be additional data not yet processed
    // from the stream. Since encoding is three bytes clear to four bytes encoded,
    // padding means there are one or two bytes at most remaining to be decoded from
    // the last octet.
    //
    if (main_len != len) {
	unsigned char b1 = base64inv(bp[main_len]);
	unsigned char b2 = base64inv(bp[main_len + 1]);
	unsigned char b3 = 0;
	if (bp[main_len + 2] != '=') {
	    b3 = base64inv(bp[main_len + 2]);
	}

	buf[off++] = (char)((unsigned char)(b1 << 2) & 0xfc) | ((unsigned char)(b2 >> 4) & 0x3);
	if (bp[main_len + 2] != '=') {
	    buf[off++] = (char)((unsigned char)(b2 & 0xf) << 4) | ((unsigned char)(b3 >> 2) & 0xf);
	}
    }

    return;
}

void
RFCMIME::writeBase64(Buffer & buf, const char * bp, const unsigned long len)
{
    if (bp == NULL || len == 0) {
	crlf(buf);
	return;
    }

    // The length has to be a multiple of 3. We will need to pad
    // any extra. Let's just work on the main body and save the
    // fractional stuff for the end.
    //
    unsigned long main_len = len - (len % 3);
    const unsigned char * ubp = (const unsigned char *)bp;

    char line[80];

    unsigned int enc_char;

    int lf = 0;

    int block;
    for (block = 0; block < main_len; block += 3) {
	enc_char = (ubp[block] >> 2) & 0x3f;
	line[lf++] = base64_chars[enc_char];

	enc_char = ((ubp[block] & 0x3) << 4) | ((ubp[block+1] >> 4) & 0xf);
	line[lf++] = base64_chars[enc_char];

	enc_char = ((ubp[block+1] & 0xf) << 2) | ((ubp[block + 2] >> 6) & 0x3);
	line[lf++] = base64_chars[enc_char];

	enc_char = ubp[block + 2] & 0x3f;
	line[lf++] = base64_chars[enc_char];

	if (lf == 72) {
	    buf.appendData(line, lf);
#ifdef DEAD_WOOD
	    DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	    crlf(buf);
	    lf = 0;
	}
    }

    if (lf > 0) {
	buf.appendData(line, lf);
    }

    if (((lf + 4) % 72) == 0) {
	crlf(buf);
    }

    switch(len % 3) {
      case 1:
	enc_char = (ubp[block] >> 2) & 0x3f ;
	buf.appendData(&base64_chars[enc_char], 1);

	enc_char = ((ubp[block] & 0x3) << 4);
	buf.appendData(&base64_chars[enc_char], 1);

	buf.appendData("==", 2);
	break;

      case 2:
	enc_char = (ubp[block] >> 2) & 0x3f;
	buf.appendData(&base64_chars[enc_char], 1);

	enc_char = ((ubp[block] & 0x3) << 4) | ((ubp[block+1] >> 4) & 0xf);
	buf.appendData(&base64_chars[enc_char], 1);

	enc_char = ((ubp[block + 1] & 0xf) << 2);
	buf.appendData(&base64_chars[enc_char], 1);

	buf.appendData("=", 1);
    }

    crlf(buf);
}

// Function: RFCMIME::readTextEnriched - convert enriched to plain text
// Description:
//  This function converts a buffer containing enriched text into a buffer
//  containing the plain text translation of the enriched text
// Method:
//  This algorithm was adapted from the example simple enriched-to-plain
//  text translator contained in RFC 1563, Appendix A.
//
void
RFCMIME::readTextEnriched(char * buf, int & off, const char * bp, const unsigned long bp_len)
{
  char c, i, paramct=0, newlinect=0, nofill=0;
  char token[256], *p;
  
  const char *ebp = bp+bp_len;
  for (const char * cur = bp; cur < ebp; cur++) {
    c = *cur;
    if (c == '<') {
      if (newlinect == 1)
	buf[off++] = ' ';
      newlinect = 0;
      c = *(++cur);
      if (c == '<') {
	if (paramct <= 0)
	  buf[off++] = c;
      }
      else {
	for (i=0, p=token; (c = *cur) && (cur < ebp) && (c != '>'); i++, cur++) {
	  if (i < sizeof(token)-1)
	    *p++ = isupper(c) ? tolower(c) : c;
	}
	*p = '\0';
	if (cur >= ebp)
	  break;
	if (strcmp(token, "param") == 0)
	  paramct++;
	else if (strcmp(token, "nofill") == 0)
	  nofill++;
	else if (strcmp(token, "/param") == 0)
	  paramct--;
	else if (strcmp(token, "/nofill") == 0)
	  nofill--;
      }
    } else {
      if (paramct > 0)
	; /* ignore params */
      else if ((c == '\n') && (nofill <= 0)) {
	if (++newlinect > 1)
	  buf[off++] = c;
      } else {
	if (newlinect == 1)
	  buf[off++] = ' ';
	newlinect = 0;
	buf[off++] = c;
      }
    }
  }
  buf[off++] = '\n';
  
  return;
}

void
RFCMIME::readQPrint(
		char *buf, int &off,
		const char *bp, const unsigned long bp_len)
{
    for (const char * cur = bp; cur < (bp + bp_len); cur++) {
	if (*cur == '=') {
	    if (*(cur + 1) == '\n') {
		cur += 1;
		continue;
	    }
	    else if (*(cur + 1) == '\r' && *(cur + 2) == '\n') {
		cur += 2;
		continue;
	    }
	    else {
		if (isxdigit(*(cur + 1)) && isxdigit(*(cur + 2))) {
		    char hex[3];
		    hex[0] = *(cur + 1);
		    hex[1] = *(cur + 2);
		    hex[2] = 0;

		    buf[off++] = (char) strtol(hex, NULL, 16);
		    cur += 2;
		    continue;
		}
	    }
	}

	buf[off++] = *cur;
    }

    return;
}

void
RFCMIME::writeQPrint(Buffer & buf, const char * bp, const unsigned long bp_len)
{
  if (bp == NULL || bp_len == 0) {
    crlf(buf);
    return;
  }
  
  int last_nl = 0;
  int off = 0;
  
  // A line buffer for improving formatting performance. Note that
  // QP requires all lines to be < 72 characters plus CRLF. So, a
  // fixed size 80 character buffer is safe.
  //
  char tmp[20];			// temp for constructing "octets"
  char line_buf[80];
  
  // There are probably more elegant ways to deal with a message that
  // begins with "From ", but we will simply due it this more simplistic
  // way.
  //
  const char * start;
  if (strncmp(bp, "From ", 5) == 0) {
    memcpy(&line_buf[off], "=46", 3);
    start = bp + 1;
    off += 3;
  }
  else {
    start = bp;
  }
  
  // This loop will apply the encodings, following the rules identified
  // in RFC1521 (though not necessarily in the order presented.
  //
  const char *cur;
  for (cur = start; cur < (bp + bp_len); cur++) {
    
    // Rule #5: Part 1! We will try to break at white space
    // if possible, but it may not be possible. In any case,
    // we want to force the lines to be less than 76 characters.
    //
    if (off > 72) {
      line_buf[off++] = '=';
      buf.appendData(line_buf, off);
      crlf(buf);
      last_nl = 0;
      off = 0;
    }
    
    // Rule #1: Any octet, except those indicating a line break
    // according to the newline convention mabe represented by
    // an = followed by a two digit hexadecimal representation
    // of the octet's value. We will represent any non-7bit
    // data this way, but let the rest slide. We do wrap "="
    // just to be safe.
    //
    if (*cur != (*cur & 0x7f) || *cur == '=' || NON_MAIL_SAFE(*cur)) {
      sprintf(tmp, "=%02X", (int)(unsigned char)*cur);
      memcpy(&line_buf[off], tmp, 3);
      off += 3;
      continue;
    }
    
    // Rule #2: Octets with decimal values of 33 through 60
    // inclusive and 62 through 126, inclusive, MAY be represented
    // as the ASCII characters which correspond to those octets.
    //
    if ((*cur >= 33 && *cur <= 60) ||
	(*cur >= 62 && *cur <= 126)) {
      line_buf[off++] = *cur;
      continue;
    }
    
    // Rule #5: The q-p encoding REQUIRES that encoded lines be
    // no more than 76 characters long. If longer, an equal sign
    // as the last character n the line indicates a soft line break.
    //
    // This is tricky if you want to leave it reasonably readable
    // (why else do this?). We only want to break on white space.
    // At each white gap, we need to count forward to the next
    // white gap and see if we exceed the 76 character limit.
    // We will cheat a few characters to allow us some room
    // for arithmetic.
    //
    if (*cur == ' ' || *cur == '\t') {
      // Find the end of this clump of white space.
      //
      const char *nw;
      for (nw = cur; nw < (bp + bp_len) && *nw && *nw != '\n'; nw++) {
	if (!isspace((unsigned char)*nw)) {
	  break;
	}
      }
      
      // Find the end of the next non-white region.
      //
      const char *white;
      for (white = nw;
	   white < (bp + bp_len) && *white && !isspace((unsigned char)*white);
	   white++) {
	continue;
      }
      
      int line_len = (off - last_nl) + (white - cur);
      if (line_len > 72) {
	// Need a soft line break. Lets put it after the
	// current clump of white space. We will break
	// at 72 characters, even if we arent at the end
	// of the white space. This prevents buffer overruns.
	//
	for (const char *cp_w = cur; cp_w < nw; cp_w++) {
	  line_buf[off++] = *cp_w;
	  if (off > 72) {
	    line_buf[off++] = '=';
	    buf.appendData(line_buf, off);
#ifdef DEAD_WOOD
	    DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	    crlf(buf);
	    off = 0;
	    last_nl = 0;
	  }
	}
	
	// There is an edge case that we may have written the last
	// white space character in the for loop above. This will
	// prevent us from spitting an extra continuation line.
	//
	if (off) {
	  line_buf[off++] = '=';
	  buf.appendData(line_buf, off);
#ifdef DEAD_WOOD
	  DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	  crlf(buf);
	  last_nl = 0;
	  off = 0;
	}
	
	// If we created a "From " at the front we need to wrap
	// it to protect from parsers.
	//
	if ((nw + 5) < (bp + bp_len) && strncmp(nw, "From ", 5) == 0) {
	  memcpy(&line_buf[off], "=46", 3);
	  off += 3;
	  cur = nw;
	}
	else {
	  cur = nw - 1;
	}
      }
      else {
	line_buf[off++] = *cur;
      }
      
      continue;
    }
    
    // Rule 3: Octets with values of 9 and 32 MAY be represented
    // as ASCII TAB and SPACE but MUST NOT be represented at the
    // end of an encoded line. We solve this be encoding the last
    // white space before a new line (except a new line) using
    // Rule #1.
    //
    if (*cur == '\n') {
      if (cur == start) {
	crlf(buf);
      }
      else {
	last_nl = off + 1;
	
	char prev = *(cur - 1);
	if ((prev == ' ' || prev == '\t') && prev != '\n') {
	  off = off ? off - 1 : off;
	  
	  char tmpbuf[20];
	  sprintf(tmpbuf, "=%02X", *(cur - 1));
	  memcpy(&line_buf[off], tmpbuf, 3);
	  off += 3;
	}
	
	buf.appendData(line_buf, off);
#ifdef DEAD_WOOD
	DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	last_nl = 0;
	off = 0;
	
	if (*(cur - 1) == '\r') {
	  buf.appendData(cur, 1);
	}
	else {
	  crlf(buf);
	}
      }
      // We need to munge a line that starts with "From " to it
      // protect from parsers. The simplest way is to encode the
      // "F" using rule #1.
      //
      if ((cur + 5) < (bp + bp_len) && strncmp((cur + 1), "From ", 5) == 0) {
	memcpy(&line_buf[off], "=46", 3);
	off += 3;
	cur += 1;
      }
      continue;
    }

    // if we have gotten this far, we could not figure out *what* to
    // do with the "octet" at *cur - in this case, apply Rule #1
    // (General 8-bit representation)
    //
    sprintf(tmp, "=%02X", (int)(unsigned char)*cur);
    memcpy(&line_buf[off], tmp, 3);
    off += 3;

  }	// end of big "for" loop
  
  if (off > 0) {
    buf.appendData(line_buf, off);
  }
  
  if (*(cur - 1) != '\n') {
    crlf(buf);
  }
}

void
RFCMIME::writePlainText(Buffer & buf, const char * bp, const unsigned long len)
{
    // It may seem silly, but we do need to make sure every line ends with
    // a CRLF pair. Most buffers will end with only LF.
    //
    if (bp == NULL || len == 0) {
	crlf(buf);
	return;
    }

    const char * line_start = bp;
    const char * cur;
    for (cur = bp; cur < (bp + len); cur++) {
	if (*cur == '\n') {
	    const char * real_end = cur;

	    if (cur != bp && *(cur - 1) == '\r') {
		real_end -= 1;
	    }

	    buf.appendData(line_start, real_end - line_start);
#ifdef DEAD_WOOD
	    DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	    line_start = cur + 1;
	    crlf(buf);
	}
    }

    if (line_start < cur) {
	buf.appendData(line_start, cur - line_start);
    }

    // The body should end with a CRLF.
    //
    if (*(cur - 1) != '\n') {
	crlf(buf);
    }
}

void
RFCMIME::md5PlainText(const char * bp, const unsigned long len, unsigned char * digest)
{
    // We need to compute the md5 signature based on a message that has
    // the CRLF line terminator. Most of our buffers don't so we will need
    // to scan the body and do some magic. The approach will be to sum
    // one line at a time. If the buffer doesn't have CRLF we will do that
    // independently.
    //

    MD5_CTX context;
    MD5Init(&context);
    unsigned char * local_crlf = (unsigned char *)"\r\n";

    const char * last = bp;
    const char * cur;
    for (cur = bp; cur < (bp + len); cur++) {
	if (*cur == '\n') {
#ifdef DEAD_WOOD
	    DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	    if (cur == bp || *(cur - 1) == '\r') {
		MD5Update(&context, (unsigned char *)last,
			  cur - last + 1);
	    }
	    else {
		MD5Update(&context, (unsigned char *)last,
			  cur - last);
		MD5Update(&context, local_crlf, 2);
	    }
	    last = cur + 1;
	}
    }

    if (bp[len - 1] != '\n') {
	// Need to sum the trailing fraction with a CRLF.
	MD5Update(&context, (unsigned char *)last,
		  cur - last);
	MD5Update(&context, local_crlf, 2);
    }

    MD5Final(digest, &context);
}

void
RFCMIME::formatBodies(DtMailEnv & error,
		      DtMail::Message & msg,
		      DtMailBoolean include_content_length,
		      char ** extra_headers,
		      Buffer & buf)
{
// For CHARSET
    char *from_cs = NULL, *to_cs = NULL;
	int eightbit = 0;

	// This is a static global variable that determines charset and whether
	// transfer encoding should be done.  Must (re)set this variable at
	// beginning of formatBodies().
	// When MT safe is implemented, must re-work this.
    converted = 0;

    error.clear();

    BufferMemory hdr_buf(1024);

    // We always put the Mime-Version on MIME messages, regardless
    // of content.
    //
    hdr_buf.appendData("Mime-Version: 1.0", 17);
    crlf(hdr_buf);

    // We need to figure out what the content type will
    // be. If we have multiple message body parts, then
    // it is multipart/mixed. Otherwise we have to retrieve
    // the MIME type for the typing database and also identify
    // the charset and encoding.
    //
    int body_count = msg.getBodyCount(error);
    srand((u_int)time(0));

    char boundary[256];
    sprintf(boundary, "%x_%x-%x_%x-%x_%x", rand(), rand(), rand(),
	    rand(), rand(), rand());

    DtMail::MailRc * mail_rc = _session->mailRc(error);

    DtMailBoolean strict_mime = DTM_FALSE;
    const char * rc_value;
    error.clear();
    mail_rc->getValue(error, "strictmime", &rc_value);
    if (error.isNotSet()) {
	strict_mime = DTM_TRUE;
    }
    error.clear();

    char default_charset[64];
    getCharSet(default_charset);

    if (body_count <= 1) {
	DtMail::BodyPart * bp = msg.getFirstBodyPart(error);
	char mime_type[64];
	DtMailBoolean is_text;
	getMIMEType(bp, mime_type, is_text);

	unsigned long bp_len;
	char * bp_contents=NULL, * name=NULL;
  	const void *tmp_ptr;
	error.clear();
	bp->getContents(error, &tmp_ptr, &bp_len, NULL, &name, NULL, NULL);
	// We don't want to change the contents of the msg.
	if (bp_len > 0) {
        	bp_contents = (char*)malloc((unsigned int)bp_len);
		memcpy(bp_contents, (char*)tmp_ptr, (size_t)bp_len);
	}

	Encoding enc;
	if (strncasecmp(mime_type, "message/", 8) == 0) {
	    enc = getClearEncoding(
				(char *)bp_contents, (unsigned int) bp_len,
				&eightbit);
	}
	else {
	    enc = getEncodingType(
				(char *)bp_contents, (unsigned int) bp_len,
				strict_mime, &eightbit);
	}

// For CHARSET
    	if (bp_contents && (is_text == DTM_TRUE) && eightbit ) {
		from_cs = NULL;
    		from_cs = _session->locToConvName();
		to_cs = NULL;
		to_cs = _session->targetConvName();
		converted = _session->csConvert(
				(char **)&bp_contents, bp_len, 1,
	            		from_cs, to_cs);
		if ( from_cs )
		  free( from_cs );
		if ( to_cs )
		  free( to_cs );
    	}
// End of For CHARSET

	// Do md5 check summing.
	//
	unsigned char digest[16];
	memset(digest, 0, sizeof(digest));

	if (bp_contents && bp_len > 0) {
	    if (is_text == DTM_TRUE) {
		md5PlainText((char *)bp_contents, bp_len, digest);
	    }
	    else {
		MD5_CTX context;
		MD5Init(&context);
		MD5Update(
			&context,
			(unsigned char *)bp_contents,
			(unsigned int) bp_len);
		MD5Final(digest, &context);
#ifdef DEAD_WOOD
		DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
	    }
	}

	writeContentHeaders(hdr_buf, mime_type, NULL, enc,
			    (char *)digest, DTM_FALSE, eightbit);

	if (name)
	    free(name);

	crlf(buf);

	switch (enc) {
	  case MIME_7BIT:
	  case MIME_8BIT:
	  default:
	    // Just copy the bits.
	    writePlainText(buf, (char *)bp_contents, bp_len);
	    break;

	  case MIME_BASE64:
		if (converted && !strict_mime) {
		    	writePlainText(buf, (char *)bp_contents, bp_len);
		    	converted = 0;
		} else {
		   	writeBase64(buf, (char *)bp_contents, bp_len);
		}
	    break;

	  case MIME_QPRINT:
	    if (converted && !strict_mime) {
	    	writePlainText(buf, (char *)bp_contents, bp_len);
		converted = 0;
	    } else {
	   	writeQPrint(buf, (char *)bp_contents, bp_len);
	    }
	    break;
	}
	if (bp_contents)
		free(bp_contents);
    }
    else {
	char *content_type = new char[100];

	sprintf(content_type, "Content-Type: multipart/mixed;boundary=%s",
		boundary);

	int len = strlen(content_type);
	hdr_buf.appendData(content_type, len);
	crlf(hdr_buf);
	delete [] content_type;

	crlf(buf);
	
	int bdry_len = strlen(boundary);
	int cur_body = 0;
	DtMailBoolean show_as_attachment = DTM_FALSE;

	for (DtMail::BodyPart * bp = msg.getFirstBodyPart(error);
	     bp && !error.isSet();
	     bp = msg.getNextBodyPart(error, bp), cur_body += 1) {

	    // Skip this body part if it is deleted.
	    if (bp->flagIsSet(error, DtMailBodyPartDeletePending))
		continue;

	    // Put out the boundary.
	    //
	    buf.appendData("--", 2);
	    buf.appendData(boundary, bdry_len);
	    crlf(buf);

	    char mime_type[64];
	    DtMailBoolean is_text;
	    getMIMEType(bp, mime_type, is_text);
	    
	    unsigned long bp_len;
	    char* bp_contents=NULL, *name=NULL;
	    const void *tmp_ptr;
		char *type = NULL;
	    bp->getContents(error, &tmp_ptr, &bp_len, &type, &name, NULL, NULL);
	    if (bp_len > 0) {
	    	bp_contents = (char*)malloc((unsigned int)bp_len);
	   	memcpy(bp_contents, (char*)tmp_ptr, (size_t)bp_len);
	    }

	    Encoding enc;
	    if (strncasecmp(mime_type, "message/", 8) == 0) {
		enc = getClearEncoding(
				(char *)bp_contents, (unsigned int) bp_len,
				&eightbit);
	    }
	    else {
		enc = getEncodingType(
				(char *)bp_contents, (unsigned int) bp_len,
				strict_mime, &eightbit);
	    }

// For CHARSET
	    if (bp_contents && (is_text == DTM_TRUE) && eightbit ) {
		from_cs = NULL;
       	   	from_cs = _session->locToConvName();
		to_cs = NULL;
	   		to_cs = _session->targetConvName();
	   		converted = _session->csConvert((char **)&bp_contents, 
			bp_len, 1, from_cs, to_cs);
		if ( from_cs )
		  free( from_cs );
		if ( to_cs )
		  free( to_cs );
	    }
// End of For CHARSET

	    unsigned char digest[16];
	    memset(digest, 0, sizeof(digest));

	    if (bp_contents && bp_len > 0) {
		if (is_text == DTM_TRUE) {
		    md5PlainText((char *)bp_contents, bp_len, digest);
		}
		else {
		    MD5_CTX context;
		    MD5Init(&context);
		    MD5Update(
			&context,
			(unsigned char *)bp_contents,
			(unsigned int) bp_len);
		    MD5Final(digest, &context);
#ifdef DEAD_WOOD
		    DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
		}
	    }

	    if (cur_body == 0) {
		show_as_attachment = DTM_FALSE;
		if (name) {
		    free(name);
		    name = NULL;
		}
	    }
	    else
		show_as_attachment = DTM_TRUE;

	    writeContentHeaders(buf, mime_type, name, enc,
				(char *)digest, show_as_attachment, eightbit);

		owCompat(buf, type, name, bp_contents, bp_len);
		if (type) {
		   free(type);   // allocated by getContent above
		}

	    if (name)
		free(name);

	    crlf(buf);
	    
	    switch (enc) {
	      case MIME_7BIT:
	      case MIME_8BIT:
	      default:
		// Just copy the bits.
		writePlainText(buf, (char *)bp_contents, bp_len);
		break;
		
	      case MIME_BASE64:
        	if (converted && !strict_mime) {
			writePlainText(buf, (char *)bp_contents, bp_len);
			converted = 0;
		} else {
			writeBase64(buf, (char *)bp_contents, bp_len);
		}
		break;
		
	      case MIME_QPRINT:
        	if (converted && !strict_mime) {
			writePlainText(buf, (char *)bp_contents, bp_len);
			converted = 0;
		} else {
			writeQPrint(buf, (char *)bp_contents, bp_len);
		}
		break;
	    }
	    if (bp_contents)
		free(bp_contents);
	}

	// Put out the last boundary.
	buf.appendData("--", 2);
	buf.appendData(boundary, bdry_len);
	buf.appendData("--", 2);
	crlf(buf);
    }

    error.clear();

    if (include_content_length) {
	hdr_buf.appendData("Content-Length: ", 16);
	char tmpbuf[20];
	sprintf(tmpbuf, "%d", buf.getSize());
	hdr_buf.appendData(tmpbuf, strlen(tmpbuf));
	crlf(hdr_buf);
    }

    *extra_headers = (char*) malloc((size_t) hdr_buf.getSize() + 1);

    BufReader * rdr = hdr_buf.getReader();

    rdr->getData(*extra_headers, hdr_buf.getSize());
    (*extra_headers)[hdr_buf.getSize()] = 0;

    delete rdr;
    return;
}

static const char * block_headers[] = {
    "Mime-Version",
    "Content-Type",
    "Content-Length",
    "Content-MD5",
    "X-Sun-Charset",
    NULL
};

void
RFCMIME::formatHeaders(DtMailEnv & error,
		       DtMail::Message & msg,
		       DtMailBoolean include_unix_from,
		       const char * extra_headers,
		       Buffer & buf)
{
    error.clear();

    // We can use the parent comment RFC header formatter with our list
    // of headers to suppress.
    //
    writeHeaders(error, msg, include_unix_from, extra_headers, block_headers, buf);
}

// This routine is same as getEncodingType except for certain rules
// not applicable to header fields.
RFCMIME::Encoding
RFCMIME::getHdrEncodingType(const char * body,
			 const unsigned int len,
			 DtMailBoolean strict_mime,
			 const char * charset)
{
  if (body == NULL || len == 0) {
    return(MIME_7BIT);
  }
  
  const int base64_growth = base64size(len) - len;
  int qprint_growth = 0;
  DtMailBoolean seven_nonprinting = DTM_FALSE;
  DtMailBoolean eight_bit = DTM_FALSE;
  DtMailBoolean encode = DTM_FALSE;
  
  const char * last_nl = body;
  const char * cur;
  for (cur = body; cur < (body + len); cur++) {
    char curChar = *cur;
    
    if (curChar != (curChar & 0x7f)) {
      eight_bit = DTM_TRUE;
      encode = DTM_TRUE;
      qprint_growth += 2;
    }
    else if (curChar == '=') {
      // These characters don't force encoding, but will be 
      // encoded if we end up encoding.
      qprint_growth += 2;
    }
    else if ( ((curChar < ' ') || (curChar == 0x7f)) 
            && (curChar != 0x09) && (curChar != 0x0A) ) {
      // These characters force encoding
      //
      seven_nonprinting = DTM_TRUE;
      encode = DTM_TRUE;
      qprint_growth += 2;
    }
    
    if (curChar == '\n') {
#ifdef DEAD_WOOD
      DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
     
      if ((cur - last_nl) > 76) {
	encode = DTM_TRUE;
	qprint_growth += 2;
      }
      
      if ((cur != body && (*(cur - 1) == ' ' || *(cur - 1) == '\t'))) {
	encode = DTM_TRUE;
	qprint_growth += 2;
      }
      
      if ( ((cur + 6) < (body + len) )
	   && (strncmp((cur + 1), "From ", 5) == 0) ) {
	encode = DTM_TRUE;
	qprint_growth += 2;
      }
      
      last_nl = cur + 1;
    }
    
  }
  
  // Deal with buffers that don't end with a new line.
  //
  if ((cur - last_nl) > 76) {
    encode = DTM_TRUE;
    qprint_growth += 2;
  }
  
  Encoding enc = MIME_7BIT;
  
  if (!strict_mime && !eight_bit && !seven_nonprinting) {
    // If strict_mime is off we only encode if we have 8 bit data
    // of most of the non-printing 7-bit characters
    //
    enc = MIME_7BIT;
  }
  else if (encode) {
    //
    // Strict_mime is TRUE and we have reason to encode.
    // Choose base64 if the charset is ISO-2022-JP (as per RFC 1468).
    // Otherwise, choose the most compact encoding.
    //
    if (strncasecmp(charset, "iso-2022-jp", strlen(charset)) == 0)
      enc = MIME_BASE64;
    else if (qprint_growth > base64_growth)
      enc = MIME_BASE64;
    else
      enc = MIME_QPRINT;
  }
  
  return(enc);
}

static inline int
mbisspace(int c)
{
    return((c & 0x7f) == c && isspace(c));
}

void
RFCMIME::rfc1522cpy(Buffer & buf, const char * value)
{
	Encoding enc;
	DtMailBoolean eight_bit = DTM_FALSE;
	DtMailBoolean strict_mime = DTM_FALSE;
	DtMailEnv error;
	DtMail::MailRc *mail_rc = _session->mailRc(error);
	const char *rc_value;
	char charset[64];
	char *from_cs = NULL, *to_cs = NULL, *convertbuf = NULL;
	int convert = 0;
	int word_len = 0, mb_ret = 0, cs_len = 0;
	unsigned long tmpcount = 0;


	getCharSet(charset);
	cs_len = strlen(charset);

    error.clear();
    mail_rc->getValue(error, "strictmime", &rc_value);
	if (error.isNotSet()) {
	  strict_mime = DTM_TRUE;
	}

    // We are going to encode 8 bit data, one word at a time. This may
    // not be the best possible algorithm, but it will get the correct
    // information in the header.
    //
    for (const char * cur = value; *cur; cur++) {

	// Skip over spaces
	if (mbisspace(*cur)) {
	    buf.appendData(cur, 1);
	    continue;
	}

        // Get a group of characters (encoded-word).
        // Ensure encoded-word is not more than 75 bytes - including charset,
	// encoding, encoded-text, and delimiters as per RFC 1522.
	// Encoded-word = "=?charset?encoding?encoded-text?="
	// 7 is "=?" and "?=" and two ? and Q or B for the encoding.
        word_len = cs_len + 7;

	const char *scan_c;
	for (scan_c = cur;
	     *scan_c && !mbisspace(*scan_c) && word_len <= 75;
		 scan_c++, word_len++) {
	    if (*scan_c != (*scan_c & 0x7f)) {
		// Here, we've come across a non 7bit byte.
		// Use mblen to get entire character.
		// If the number of bytes for the entire character can fit within
		// this group of text, then advance scan_c pointer, increase word_len,
		// and mark this group as eight_bit so encoding is performed.
		mb_ret = mblen(scan_c, MB_LEN_MAX);
		if ( (mb_ret >= 0) && (word_len + mb_ret <= 75) ) {
		  scan_c += mb_ret;
		  word_len += mb_ret;
		  eight_bit = DTM_TRUE;
		  // When for loop continues, scan_c and word_len get incremented by 1
		  scan_c -= 1;
		  word_len -= 1;
		}
	    mb_ret = 0;
		}
	}

	if (eight_bit == DTM_FALSE) {
	    // Simple! Copy the chars to the output.
	    buf.appendData(cur, scan_c - cur);
	    cur = scan_c - 1;
	}
	else {
	    // We have a word here. It has 8 bit data, so we will put
	    // it out as RFC1522 chunk.
	    //
	    BufferMemory tmp(1024);

	    buf.appendData("=?", 2);
	    buf.appendData(charset, strlen(charset));

	    // Do codeset conversion
	    convert = 0;
	    tmpcount = (unsigned long) (scan_c - cur);
	    convertbuf = (char*)calloc((unsigned int) tmpcount+1, sizeof(char));
	    memcpy(convertbuf, cur, (size_t)tmpcount);
	    from_cs = _session->locToConvName();
	    to_cs = _session->targetConvName();
	    convert = _session->csConvert(
					(char **)&convertbuf,
					tmpcount, 1,
	            			from_cs, to_cs);
	    if ( from_cs ) free( from_cs );
	    if ( to_cs ) free( to_cs );

	    if (convert)
	      enc = getHdrEncodingType(
					convertbuf,
					(unsigned int) tmpcount,
					strict_mime, charset);
	    else
	      enc = getHdrEncodingType(cur, scan_c-cur, strict_mime, charset);
	    
	    // Do transfer encoding
	    switch (enc) {
	    case MIME_BASE64:
		buf.appendData("?b?", 3);
		if (convert) {
		    writeBase64(tmp, convertbuf, tmpcount);
		} else {
		    writeBase64(tmp, cur, scan_c - cur);
		}
		break;
	    case MIME_QPRINT:
		buf.appendData("?q?", 3);
		if (convert) {
		    writeQPrint(tmp, convertbuf, tmpcount);
		} else {
		    writeQPrint(tmp, cur, scan_c - cur);
		}
		break;
	    }

	    char * cp_buf = new char[tmp.getSize()];
	    BufReader * reader = tmp.getReader();

	    reader->getData(cp_buf, tmp.getSize());
	    delete reader;

	    char *tw;
	    for (tw = &cp_buf[tmp.getSize() - 1];
		 tw > cp_buf && isspace((unsigned char)*tw); tw--) {
		continue;
	    }

	    tw += 1;
	    *tw = 0;

	    buf.appendData(cp_buf, strlen(cp_buf));
	    delete [] cp_buf;

	    buf.appendData("?=", 2);

	    cur = scan_c - 1;

	    eight_bit = DTM_FALSE;
	}
    }

    crlf(buf);
}

// For OW Mailtool compatibility so that MIME mail with attachment(s) sent
// via dtmail will have OW Mailtool understandable message header identifying 
// attachment type so that OW Mailtool can display the correct attachment icon
// and invoke the correct double-click operation.
// RFCMIME::owCompat()
// Given the attribute type, get the attribute's SUNV3_TYPE, if SUNV3_TYPE
// exists, put out X-Sun-Data-Type header and SUNV3_TYPE value.
// If no type is given or there is no SUNV3_TYPE for the given attribute,
// then re-type content.
void
RFCMIME::owCompat(Buffer & buf,
         char *type,
         char *name,
         char *bp_contents,
         unsigned long bp_len)
{
    char *v3type = NULL;

    if (type) {
	   v3type = (char *)DtDtsDataTypeToAttributeValue(type,
						 "SUNV3_TYPE",
						 NULL);
	}
	if (!type || !v3type)
	{
	   char *buf_type;
	   buf_type = DtDtsBufferToDataType(bp_contents, (int) bp_len, name);
	   if (buf_type)
	   {
	       v3type = (char *)DtDtsDataTypeToAttributeValue(
							buf_type,
							"SUNV3_TYPE",
							NULL);
	       DtDtsFreeDataType(buf_type);
	   }
	}

	if (v3type) {
	   buf.appendData("X-Sun-Data-Type: ", 17);
	   buf.appendData(v3type, strlen(v3type));
	   crlf(buf);
	   DtDtsFreeAttributeValue(v3type);
	}
}
