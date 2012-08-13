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
 *	$TOG: V3BodyPart.C /main/8 1998/07/23 18:04:38 mgreess $
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

#include <Dt/Dts.h>

#include <DtMail/DtMail.hh>
#include "RFCImpl.hh"
#include "SunV3.hh"
#include <DtMail/Threads.hh>
#include "str_utils.h"

#include <assert.h>

// For CHARSET
#include <locale.h>
#include <DtHelp/LocaleXlate.h>

V3BodyPart::V3BodyPart(DtMailEnv & error,
		       DtMail::Message * parent,
		       const char * start,
		       const int len,
		       RFCEnvelope * body_env)
: RFCBodyPart(error, parent, start, len, body_env)
{
    // This constructor is used for single body parts. This
    // includes generic RFC822 messages, i.e. no Mime-Version
    // field.
    //
    // There really isn't anything we can do. We may have to
    // apply a content encoding, but let's wait until the user
    // expresses an interest.
    //
  error.clear();

    return;
}

// V3BodyPart:V3BodyPart -- retrieve next V3 message body part
// Arguments:
//  DtMailEnv & error		--
//  DtMail::Message * parent	--
//  const char * start		-- -> beginning of boundary separator
//  const char ** end		-- -> end of message
// Outputs:
//  *end	-- is set to the first byte past the end of the retrieved
//		part. This either points to the next body part (if any) or
// 		is 1 byte past the end of the message if no further parts.
//  _body_env	-- RFCEnvelope defining entire body part
//  _body_len	-- true length of contained body part (minus headers)
//  _body_text	-- first byte of contained body part (minus headers)
// Description:
//  This constructor is used for multi-part body parts. Given a
//  pointer to the beginning of the boundary separator and a 
//  pointer to the end of the message, reduce pointers to actual
//  body part data (minus headers) and update class variables
//  as appropriate.
//

V3BodyPart::V3BodyPart(DtMailEnv & error,
		       DtMail::Message * parent,
		       const char * start,
		       const char ** end)
: RFCBodyPart(error, parent, start, 0, NULL)
{
    // Get past the boundary separator.
    //
    const char * body_end;
    for (body_end = start; body_end <= *end && *body_end != '\n'; body_end++) {
	continue;
    }

    if (body_end > *end) {
	// no data after separator?? Bogus body part!
	_body_len = 0;
	*end = body_end;
	return;
    }

    // The body part headers begin here. We will look from here until
    // the first blank line for the end of the headers.
    //
    body_end += 1; // Chew the newline.
    const char * env_start = body_end;
    for (; body_end <= *end; body_end++) {
	if (*body_end == '\n') {
	    int blank_only = 1;
	    for (const char * blank = body_end + 1;
		 blank <= *end && *blank != '\n'; blank++) {
		if (!isspace((unsigned char)*blank)) {
		    blank_only = 0;
		    break;
		}
	    }
	    if (blank_only) {
		break;
	    }
	}
    }

    _my_env = DTM_TRUE;		// indicate that we created this envelope
    _body_env = new RFCEnvelope(error, parent, env_start, body_end - env_start + 1);

    // Now we need to find the start of the body (the real start, not
    // where the boundary begins. This is immediately after the newline
    // that is placed after the end of the headers.
    //
    _body_text = body_end + 1;
    for (; _body_text <= *end && *_body_text != '\n'; _body_text++) {
	continue;
    }
    _body_text += 1;

    // We now need to figure out where the end of the attachment is.
    // We have 2 choices. Content lines, or content length.
    //
    DtMailValueSeq	value;

    _body_env->getHeader(error, "x-sun-content-length",
			 DTM_FALSE, value);
    if (error.isNotSet()) {
      // Great! content-length is much easier.
      // We do this by computing a -> where the end of the attachment
      // is supposed to be as indicated by the specified content length.
      // If we come to the end of the message altogether, or to the start
      // of another header body, then we know we have bounded this body part.
      //
      int content_length = (int) strtol(*(value[0]), NULL, 10);
      body_end = _body_text + content_length;	// "supposed" end of body part

      for (int i = 0; i < 2; i++) {
	if (body_end == *end) {		// at end of message??
	    // Yes, this is the last header body in this message
	    // We are at the end -- force this body part to end at the
	    // end of the message and use length as computed.
	    // Mark end of this body for caller as 1 byte past end of last body header
	    // to prevent from being called again.
	    //
	    _body_len = content_length;
	    *end += 1;
	    return;
	}

	if (body_end > *end) {		// past end of message??
	    // Yes, this is the last header body in this message
	    // We are past the end -- force this body part to end at the
	    // end of the message and compute length based upon that assumption.
	    // Mark end of this body for caller as 1 byte past end of last body header
	    // to prevent from being called again.
	    //
	    *end += 1;
	    _body_len = *end - _body_text;
	    return;
	}

	// V3BodyParts have a "----------" (10 dashes) at the beginning of a 
	// line with no white space before them and a new line at the end to 
	// signal the beginning of an attachment.

	if ((*(body_end-1) == '\n') && 
	     strncmp(body_end, "----------", 10) == 0 &&
	     isTerm(body_end + 10)) { // start of another body header?
	    // Yes, this is one of many header bodies in this message
	    // Mark end of this body for caller as first byte of next body header
	    //
	    assert(body_end < *end);	// cant be last
	    _body_len = body_end - _body_text;
	    *end = body_end+1;		// current end
	    return;
	}

	// Chew through white space and test again.
	//
	while (isspace((unsigned char)*body_end)) {
	    body_end += 1;
	}
      }
    }
    else {
	error.clear();
    }

    // We didn't have content-length, or it was wrong or bogus.
    // Try for content lines.
    //
    value.clear();		// clear out any previous header(s) retrieved
    
    _body_env->getHeader(error, "x-sun-content-lines",
			 DTM_FALSE, value);
    if (error.isNotSet()) {
	// content-lines requires a bit o' muching first.
	// We do this by marching through the body counting new lines
	// until the specified number of lines have been passed. This
	// is the determined end of the body part.
	//
	int content_lines = (int) strtol(*(value[0]), NULL, 10);
	int lines = 0;
	for (body_end = _body_text;
	     body_end <= *end && lines < content_lines; body_end++) {
	    if (*body_end == '\n') {
		lines += 1;
	    }
	}

	if (body_end >= *end) {		// at end of message??
	    // Yes, this is the last header body in this message
	    // We are at or past the end -- force this body part to end at the
	    // end of the message and compute length based upon that assumption.
	    // Mark end of this body for caller as 1 byte past end of last body header
	    // to prevent from being called again.
	    //
	    *end += 1;
	    _body_len = *end - _body_text;
	    return;
	}

	// V3BodyParts have a "----------" (10 dashes) at the beginning of a 
	// line with no white space  before them and a new line at the end to 
	// signal the beginning of an attachment.

	if ((*(body_end-1) == '\n') && 
	     strncmp(body_end, "----------", 10) == 0 &&
	     isTerm(body_end + 10) == DTM_TRUE) { // start of another body header?
	    // Yes, this is one of many header bodies in this message
	    // Mark end of this body for caller as first byte of next body header
	    //
	    assert(body_end < *end);	// cant be last
	    _body_len = body_end - _body_text;
	    *end = body_end+1;		// current end
	    return;
	}
    }
    else {
	error.clear();
    }

    // We either didn't have a length, or number of lines, or they
    // didn't work. We will now apply a dangerous but last resort 
    // effort to find the end of the body by looking for the 10 dash 
    // separator.
    // V3BodyParts have a "----------" (10 dashes) at the beginning of a 
    // line with no white space  before them and a new line at the end to 
    // signal the beginning of an attachment.
    //

    for (body_end = _body_text; body_end <= (*end - 12); body_end++) {
	if (*body_end == '\n' && 
	    strncmp(body_end + 1, "----------", 10) == 0 &&
	    isTerm(body_end + 11) == DTM_TRUE) {
	      // Dangerously by successfully located what appears to be the end
	      // of this attachment.
	      //
	      *end = body_end;
	      _body_len = body_end - _body_text;
	      return;
	}
    }

    // At this point we are at the end of the message and are without
    // the benefit of content lines or content length or the discovery
    // of what appears to be a message separator. Assume this body part
    // consumes the remainder of the message. Mark end of this body for
    // caller as 1 byte past end to prevent from being called again.
    //
    *end += 1;
    _body_len = *end - _body_text;
    return;
}

V3BodyPart::~V3BodyPart(void)
{
}

#ifdef DEAD_WOOD
DtMailChecksumState
V3BodyPart::checksum(DtMailEnv & error)
{
    error.clear();

    return(DtMailCheckUnknown);
}
#endif /* DEAD_WOOD */

static int
countTypes(char ** types)
{
    int count;

    if (NULL == types) return 0;

    for (count = 0; *types; types++, count++) {
	continue;
    }

    return(count);
}

void
V3BodyPart::getContentType(DtMailEnv &error, char **v3_type)
{
    MutexLock lock_scope(_obj_mutex);
    MutexLock dt_lib_lock(_DtMutex);
    DtMailValueSeq value;

    if (v3_type)
      *v3_type = (char *)0;

    _body_env->getHeader(error, "Content-Type", DTM_FALSE, value);
    if (error.isNotSet())
      *v3_type = strdup(*(value[0]));
    else
    {
	error.clear();
	value.clear();
	_body_env->getHeader(error, "X-Sun-Data-Type", DTM_FALSE, value);
	if (error.isNotSet())
	  *v3_type = strdup(*(value[0]));
	else
	{
	    error.clear();
	    *v3_type = strdup("text");
	}
    }
}

void
V3BodyPart::getDtType(DtMailEnv & error)
{
    MutexLock lock_scope(_obj_mutex);
    MutexLock dt_lib_lock(_DtMutex);

// No need to clear the error object here because it should have
// been cleared by the object that instantiated it.
//    error.clear();

    char * v3_type;
    DtMailValueSeq value;
    _body_env->getHeader(error, "Content-Type", DTM_FALSE, value);
    if (error.isNotSet()) {
	v3_type = strdup(*(value[0]));
    }
    else {
	error.clear();
	value.clear();
	_body_env->getHeader(error, "X-Sun-Data-Type", DTM_FALSE, 
			     value);
	if (error.isNotSet()) {
	    v3_type = strdup(*(value[0]));
	}
	else {
	    error.clear();
	    v3_type = strdup("text");
	}
    }

    char ** types = DtDtsFindAttribute("SUNV3_TYPE", v3_type);

    if (NULL != types)
    {
        if (countTypes(types) == 1) {
	    // We will use the first name. It may be wrong, but
	    // it is the best we can do at this point.
	    //
	    _body_type = strdup(types[0]);
	    DtDtsFreeDataTypeNames(types);
	    free(v3_type);
	    return;
        }
        DtDtsFreeDataTypeNames(types);
    }

    // We need the bits so we can type the buffer and get
    // a type for the object. This is where things can get
    // very slow for the user.
    //
    loadBody(error);
    if (error.isSet()) {
	return;
    }

    char * name = getName(error);
    if (error.isSet()) {
	// don't care about error conditions returned by getName().
	error.clear();
    }

    // If the name is "Attachment" and the type is "text", then
    // use the type as the name to avoid the buffer being called
    // generic data.
    //
    if (name && strcasecmp(name, "attachment") == 0 &&
	strcasecmp(v3_type, "text") == 0) {
	free(name);
	name = strdup(v3_type);
    }

    char * type = DtDtsBufferToDataType(_body, _body_decoded_len, name);
    _body_type = strdup(type);
    DtDtsFreeDataType(type);
    free(name);
    free(v3_type);
//    error.clear();

}

void
V3BodyPart::loadBody(DtMailEnv &)
{
// For CHARSET
    char *to_cs = NULL, *from_cs = NULL;
    char *cs = new char[64];

// End of For CHARSET
    int do_decode = 1;

    if (_body) {
	delete [] cs;
	return;
    }

    // See if we are using an encoding.
    DtMailValueSeq value;
    DtMailEnv lerror;
    _body_env->getHeader(lerror, "X-Sun-Encoding-Info",
			 DTM_FALSE, value);
    if (lerror.isSet() || (_body_len == 0)) {
	// No Encodings.
	lerror.clear();
	_body = (char *)_body_text;
	_must_free_body = DTM_FALSE;
	_body_decoded_len = _body_len;
	// Before the codeset conversion code was put in, return here.
	do_decode = 0;
    }

	if (do_decode) {
    _body_decoded_len = 0;
    _must_free_body = DTM_TRUE;

    SunV3::decode(*(value[0]), &_body, _body_decoded_len,
		  _body_text, _body_len);
    }

// For CHARSET
	// If Content-Type is text, then get charset and do conversion.
	// If Content-Type is X-sun-attachment, then if X-Sun-Data-Type is text,
	// then get charset and do conversion.
	// If Content-Type is missing, check if X-Sun-Data-Type exists, and
	// if X-Sun-Data-Type exists and is text, then get charset and do conversion.
	// If Content-Type and X-Sun-Data-Type are missing, then assume text so get
	// charset and do conversion.
	// Else don't need codeset conversion.
    const char *cstmp = NULL;
	const char *ct = NULL;
	value.clear();
	_body_env->getHeader(lerror, "Content-Type", DTM_FALSE, value);
	if (lerror.isNotSet()) {
	   // Sun's V.3 Mail File Format requires a Content-Type header field to be
	   // "X-sun-attachment" for attachment message.  X-Sun-Data-Type header
	   // field is also required for attachment message.
	   ct = *(value[0]);
	   if ( strcasecmp(ct, "X-sun-attachment") == 0 ) {
	      value.clear();
	      _body_env->getHeader(lerror, "X-Sun-Data-Type", DTM_FALSE, value);
	      if (lerror.isNotSet()) {
	         ct = *(value[0]);
	         if ( strcasecmp(ct, "text") == 0 ) {
				// Get charset from X-Sun-Text-Type which contains the name of
				// the codeset for the text-based body part.  It is a mandatory
				// header iff X-Sun-Data-Type is of type text.
				value.clear();
				_body_env->getHeader(lerror, "X-Sun-Text-Type", DTM_FALSE, value);
				if (lerror.isNotSet()) {
                cstmp = *(value[0]);
	            strcpy(cs, cstmp);
				} else {
				// We are not returning yet.  We'll be flexible here.
				// Some mailers may not set this field.  We'll try to obtain charset
				// from X-Sun-Charset or use the locale default.  See below.
				lerror.clear();
				}
			 } else {    // Attachment not text type
	            delete [] cs;
	            return;
	         }
	      } else {
		     // Required field for attachment message not set -- return!
	             delete [] cs;
		     return;
	      }
	   } else if ( strcasecmp(ct, "text") != 0 ) {
	      delete [] cs;
	      return;
	   } 
	} else {    // Content-Type does not exist!
	   lerror.clear();
	   value.clear();
	   _body_env->getHeader(lerror, "X-Sun-Data-Type", DTM_FALSE, value);
	   if (lerror.isNotSet()) {
	      ct = *(value[0]);
	      if ( strcasecmp(ct, "text") == 0 ) {
			// Get charset from X-Sun-Text-Type which contains the name of
			// the codeset for the text-based body part.  It is a mandatory
			// header iff X-Sun-Data-Type is of type text.
			value.clear();
			_body_env->getHeader(lerror, "X-Sun-Text-Type", DTM_FALSE, value);
			if (lerror.isNotSet()) {
            cstmp = *(value[0]);
	        strcpy(cs, cstmp);
			} else {
			// We are not returning yet.  We'll be flexible here.
			// Some mailers may not set this field.  We'll try to obtain charset
			// from X-Sun-Charset or use the locale default.  See below.
			lerror.clear();
			}
	     } else {    // Attachment not text type
		delete [] cs;
	        return;
         } 
	   } else {
	   // Base on Sun's V.3 Mail File Format version 1.9:  If no Content-Type,
	   // and X-Sun-Data-Type is not set (means no/not attachment,
	   // then assume text.  Proceed with getting charset.
	   lerror.clear();
	   }
	}
	if (cstmp == NULL) {
	value.clear();
    // Get charset from charset field
	_body_env->getHeader(lerror, "X-Sun-Charset", DTM_FALSE, value);
	if (lerror.isNotSet()) {
       cstmp = *(value[0]);
	   strcpy(cs, cstmp);
	} else {    // No Charset
	   // We'll be flexible here.  If Content-Type is missing or if Content-Type
	   // is text, then we get charset but if charset is missing, we'll try to
	   // convert from the locale specific default codeset.
	   lerror.clear();
	   char *ret = NULL;
	   strcpy(cs, "DEFAULT");
	   DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
		  setlocale(LC_CTYPE, NULL),
		  NULL,
		  NULL,
		  &ret);
	   strcpy(cs, "DEFAULT");
	   strcat(cs, ".");
	   strcat(cs, ret);
           if ( ret )
	     free( ret );
	}
	}  // If cstmp is NULL

	// Handle ISO-2022-INT, RFC approved, or private encoding names
	if ( strcasecmp(cs, "ISO-2022-INT-1") == 0 ) {
	   // Need to obtain charset from encoding
	}  // RFC approved and private names are not treated differently.

	// Get iconv name from charset - this is the "from" name.
	from_cs = NULL;
	from_cs = csToConvName(cs);

	// Get current locale's iconv name - this is the "to" name.
	to_cs = NULL;
	to_cs = locToConvName();

	if ( from_cs && to_cs ) {
	  if ( strcasecmp(from_cs, to_cs) != 0 ) {
	  unsigned long tmp_len = (unsigned long) _body_decoded_len;
	  if (csConvert(&_body, tmp_len, (int)_must_free_body, from_cs, to_cs)) {
	    _must_free_body = DTM_TRUE;
		_body_decoded_len = (int) tmp_len;
      }
	  }
      }

    if ( from_cs )
	free( from_cs );
    if ( to_cs )
	free ( to_cs );
// End of For CHARSET
    delete [] cs;
}

char *
V3BodyPart::getDescription(DtMailEnv & error)
{
// No need to clear the error object here, whoever created it, cleared it.
//    error.clear();

    DtMailValueSeq value;
    _body_env->getHeader(error, "X-Sun-Data-Description",
			 DTM_FALSE, value);
    if (error.isNotSet()) {
	return(strdup(*(value[0])));
    }
    error.clear();	// clear error so it doesn't propogate back up.
    return(NULL);
}

char *
V3BodyPart::getName(DtMailEnv & error)
{
// No need to clear the error object here, it should be passed in already
// cleared.
//    error.clear();

    DtMailValueSeq value;
    _body_env->getHeader(error, "X-Sun-Data-Name",
			 DTM_FALSE, value);
    if (error.isNotSet()) {
	return(strdup(*(value[0])));
    }

    // Since we are returning a valid name in spite of the call to 
    // getHeader returning an error, we should clear the error.
    //
    error.clear();
    return(strdup("Attachment"));
}

void
V3BodyPart::setName(DtMailEnv & error, const char * name)
{
    _body_env->setHeader(error, "X-Sun-Data-Name", DTM_TRUE, name);
}

unsigned long
V3BodyPart::getLength(DtMailEnv & error)
{
    MutexLock lock_scope(_obj_mutex);

    loadBody(error);
    if (error.isSet()) {
	// propogate the error back to the caller
	return (0);
    }

    return(_body_decoded_len);
}

int
V3BodyPart::rfcSize(const char *, DtMailBoolean &)
{
    return(0);
}

char *
V3BodyPart::writeBodyParts(char *buf)
{
  return(buf);
}

// Do not need to implement this method because getHeader already
// returns the X-Sun-Charset value.
char *
V3BodyPart::csFromContentType(DtMailValueSeq&)
{
   return NULL;
}
