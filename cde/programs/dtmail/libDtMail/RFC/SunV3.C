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
 *	$TOG: SunV3.C /main/9 1998/07/24 16:10:05 mgreess $
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

#include <EUSCompat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <poll.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>

/*
** file included for INFTIM
*/ 
#if defined(SunOS)
#include <stropts.h>
#elif defined(HPUX)
#include <sys/poll.h>
#elif defined(_AIX) || defined(__linux__)
#define INFTIM (-1)             /* Infinite timeout */
#endif

#include <sys/wait.h>

#include <Dt/Dts.h>

#include "SunV3.hh"
#include <DtMail/DtMailP.hh>
#include <DtMail/IO.hh>
#include "str_utils.h"


SunV3::SunV3(DtMail::Session * session)
: RFCFormat(session)
{
}

SunV3::~SunV3(void)
{
}

static DtMailBoolean
is7bit(const char * bp, unsigned long len)
{
    for (const unsigned char * scan = (const unsigned char *)bp; 
	 ((const char *)scan) < (bp + len); scan++) {
	if (*scan != (*scan & 0x7f)) {
	    return(DTM_FALSE);
	}
    }

    return(DTM_TRUE);
}

static int
countLines(const char * bp, const unsigned long len)
{
    int lines = 0;
    for (const char * scan = bp; scan < (bp + len); scan++) {
	if (*scan == '\n') {
	    lines += 1;
	}
    }

    return(lines);
}

void
SunV3::formatBodies(DtMailEnv & error,
		    DtMail::Message & msg,
		    DtMailBoolean include_content_length,
		    char ** extra_headers,
		    Buffer & buf)
{
// For CHARSET
    char *from_cs = NULL, *to_cs = NULL;
	DtMailBoolean sevenbit = DTM_TRUE;
// End of For CHARSET
	char v3type[64];
	char charset_name[64];

    error.clear();

    // We will use a buffer to track the extra headers we put
    // on a message.
    //
    BufferMemory hdr_buf(1024);

    int body_count = msg.getBodyCount(error);
    if (error.isSet()) {
	return;
    }

    // If this is a single part message, then formatting is much
    // easier.
    //
    if (body_count <= 1) {
	// This is where we deviate a little from Sun V3 format. In
	// SunV3, you can have a single part, that is an attachment.
	// Our semantics say that any message with an attachment has
	// at least 2 parts. Given this semantic, we can be a little
	// more relaxed here because we know this first part must	
	// be something like text.
	//
	hdr_buf.appendData("Content-Type: text", 18);
	crlf(hdr_buf);

	unsigned long bp_len;
	const char * tmp_ptr;
	char * bp_contents=NULL;
	char * name=NULL;
	DtMail::BodyPart * bp = msg.getFirstBodyPart(error);
	if (error.isSet()) {
	    return;
	}
	bp->getContents(error,
			(const void **)&tmp_ptr,
			&bp_len,
			NULL,
			&name,
			NULL,
			NULL);

	if (error.isSet()) {
	    if (name != NULL) free(name);
	    return;
	}

	if (bp_len > 0) {
		bp_contents = (char*)malloc((unsigned int)bp_len);
		memcpy(bp_contents, (char*)tmp_ptr, (size_t)bp_len);
	}

	getV3Type(bp, v3type);

	// Should only call is7bit once for every body part.
	// Calling is7bit more than once means scanning an entire body part again.
	sevenbit = is7bit(bp_contents, bp_len);

// For CHARSET
    	if (strcasecmp(v3type, "text") == 0) {
    		if (sevenbit == DTM_FALSE) {
			if (bp_contents) {
				from_cs = NULL;
    				from_cs = _session->locToConvName();
				to_cs = NULL;
    				to_cs = _session->targetConvName();
    				(void) _session->csConvert((char **)&bp_contents, 
					bp_len, 1, from_cs, to_cs);
				if ( from_cs )
				  free( from_cs );
				if ( to_cs )
				  free( to_cs );
			}
	
			getCharSet(charset_name, "V3");
		
			hdr_buf.appendData("X-Sun-Charset: ", 15);
			hdr_buf.appendData(charset_name, strlen(charset_name));
		} else {   // 7 bit
			hdr_buf.appendData("X-Sun-Charset: ", 15);
			hdr_buf.appendData("us-ascii", 8);
		}
		crlf(hdr_buf);
	}
// End of For CHARSET

	buf.appendData(bp_contents, (int) bp_len);

	free(name);
    }
    else {
	// This is a Sun V3 multipart message. We need to set the global
	// headers to indicate this.
	//
	hdr_buf.appendData("Content-Type: X-Sun-Attachment", 30);
	crlf(hdr_buf);

	// We will simply loop through each part and process it in turn.
	//
	DtMail::BodyPart *bp = msg.getFirstBodyPart(error);
	if (error.isSet()) {
	    return;
	}
	for ( ;bp && error.isNotSet(); bp = msg.getNextBodyPart(error, bp)) {

	    // Skip this body part if it is deleted.
	    if (bp->flagIsSet(error, DtMailBodyPartDeletePending))
		continue;

	    // First, put out the message separator. It is a very
	    // weak line of 10 dashes.
	    //
	    buf.appendData("----------", 10);
	    crlf(buf);
	    unsigned long bp_len;
	    const char * tmp_ptr;
	    char * bp_contents=NULL;
	    char * name=NULL;
	    bp->getContents(error,
			    (const void **)&tmp_ptr,
			    &bp_len,
			    NULL,
			    &name,
			    NULL,
			    NULL);

	    // What can be done if getContents returns an error???
	    if (error.isSet()) {
		if (name != NULL) free(name);
		return;
	    }

	    // Check for an empty bodypart, bp_len = 0 or
	    // bp_contents is NULL.  Actually, if the bodypart is 
	    // empty, then both conditions should hold.
	    //
	    if (bp_len == 0 || !tmp_ptr) {
		if (name) free(name);
		continue;
	    }

	    bp_contents = (char*)malloc((unsigned int)bp_len);
	    memcpy(bp_contents, (char*)tmp_ptr, (size_t)bp_len);

	    // Now we need to write the content headers.
	    //
	    getV3Type(bp, v3type);

	   // Should only call is7bit once for every body part.
	   // Calling is7bit more than once means scanning an entire body part again.
	   sevenbit = is7bit(bp_contents, bp_len);

// For CHARSET
       if ( strcasecmp(v3type, "text") == 0 ) {
	   if ( sevenbit == DTM_FALSE ) {
		from_cs = NULL;
		from_cs = _session->locToConvName();
		to_cs = NULL;
		to_cs = _session->targetConvName();
		if (_session->csConvert((char **)&bp_contents, bp_len, 1, from_cs, to_cs)) {
			getCharSet(charset_name, "V3");
			buf.appendData("X-Sun-Charset: ", 15);
			appendString(buf, charset_name);
			crlf(buf);
		}   // End of if csConvert()
		if ( from_cs )
		  free( from_cs );
		if ( to_cs )
		  free( to_cs );
	   } else {   // 7 bit
	   	buf.appendData("X-Sun-Charset: ", 15);
	     	appendString(buf, "us-ascii");
	     	crlf(buf);
	   }
	}
// End of For CHARSET

	    buf.appendData("X-Sun-Data-Type: ", 17);
	    appendString(buf, v3type);
	    crlf(buf);

	    buf.appendData("X-Sun-Data-Description: ", 24);
	    appendString(buf, v3type);
	    crlf(buf);

	    if (!name) {
		name = strdup("Attachment");
	    }

	    buf.appendData("X-Sun-Data-Name: ", 17);
	    appendString(buf, name);
	    crlf(buf);

		// V3 compatible with OW Mailtool
		// 1. Do not uuencode text attachment containing 8 bit
	    if ( strncmp(bp_contents, "From ", 5) != 0 &&
			 strcmp(v3type, "default-app") != 0 ) {
		int lines = countLines(bp_contents, bp_len);
		DtMailBoolean need_trailing_crlf = DTM_FALSE;

		if (bp_contents[bp_len - 1] != '\n') {
		    lines += 1;
		    need_trailing_crlf = DTM_TRUE;
		}

		char tmp_xl[20];
		sprintf(tmp_xl, "%d", lines);

		buf.appendData("X-Sun-Content-Lines: ", 21);
		appendString(buf, tmp_xl);

		crlf(buf);
		crlf(buf);

		buf.appendData(bp_contents, (int) bp_len);

		if (need_trailing_crlf) {
		    crlf(buf);
		}
	    }
	    else {
		// We need to encode the buffer, to get the length,
		// then we insert it into the output buffer.
		//
		BufferMemory len_buf(8192);
		uuencode(len_buf, name, bp_contents, bp_len);

		int clen = len_buf.getSize();

		char * cbuf = new char[clen];
		if (cbuf == NULL) {
		    error.setError(DTME_NoMemory);
		    return;
		}

		BufReader * rd = len_buf.getReader();
		rd->getData(cbuf, clen);

		int lines = countLines(cbuf, clen);

		buf.appendData("X-Sun-Encoding-Info: uuencode", 29);
		crlf(buf);
		buf.appendData("X-Sun-Content-Lines: ", 21);
		char tmp_cl[20];
		sprintf(tmp_cl, "%d", lines);
		appendString(buf, tmp_cl);
		crlf(buf);
		crlf(buf);

		buf.appendData(cbuf, clen);
		delete [] cbuf;
		delete rd;
	    }

	    free(bp_contents);
	    bp_contents = NULL;
	    
	    free(name);
	    name = NULL;
	}
    }

    error.clear();

    if (include_content_length) {
	hdr_buf.appendData("Content-Length: ", 16);
	char tmpbuf[20];
	sprintf(tmpbuf, "%d", buf.getSize());
	hdr_buf.appendData(tmpbuf, strlen(tmpbuf));
	crlf(hdr_buf);
    }

    *extra_headers = new char[hdr_buf.getSize() + 1];

    BufReader * rdr = hdr_buf.getReader();

    rdr->getData(*extra_headers, hdr_buf.getSize());
    (*extra_headers)[hdr_buf.getSize()] = 0;

    delete rdr;
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
SunV3::formatHeaders(DtMailEnv & error,
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

// SunV3::decode -- perform any decodings and return clear text
// Arguments:
//  const char * enc_info -- Sun V3 encoding info specification
//  char ** outputBp	-- -> place where -> clear text is placed on output
//  int & outputLen	-- number of bytes of clear text placed on output
//  const char * inputBp -- -> encoded source text
//  const unsigned long inputLen -- number of bytes in encoded source text
// Outputs:
//  **outputBp	-- will contain a -> a newly allocated buffer containing
//		the decoded message - don't forget to deallocate when done
//  off		-- contains the number of bytes in the decoded message at **buf
// Description
//  This function will decode the specified source message according to the
//  encoding information supplied. 
//
void
SunV3::decode(const char * enc_info, 
	      char ** outputBp, 
	      int & outputLen, 
	      const char * inputBp, 
	      const unsigned long inputLen)
{
    // First we have to determine how the message was encoded.
    // The encoding methods are listed, left to right in the order
    // they were performed. We have to reverse the order of operations
    // to get back to the original decoded value.
    //
    char  *enc_str = strdup(enc_info); // V3 did not have that many schemes.
    char * encodings[4];

    char * cur = enc_str;
    char * end;
    int n_enc = 0;
    while(*cur && (end = strchr(cur, ','))) {
	*end = 0;
	encodings[n_enc++] = cur;
	cur = end + 1;
	while(*cur && isspace(*cur)) {
	    cur += 1;
	}
    }

    if (*cur) {
	encodings[n_enc++] = cur;
    }

    // At this point we have a sequence of encodings that must be
    // used in order to obtain the final clear text of the message.
    //

    char * interimBp = (char *)inputBp;
    unsigned long interimLen = inputLen;

    for (int enc = n_enc - 1; enc >= 0; enc--) {

	// next pass through decoder - if output is non-null, then
	// it is the output from a previous pass, in which case it
	// becomes the input to the second pass - we deallocate any
	// old interim results, and then use output as the new interim
        // results for the next decoding pass
	//

	assert (interimBp != NULL);
	assert (interimLen);
 	if (*outputBp) {
	    if (interimBp) {
	    	if (interimBp != inputBp)
			free(interimBp);
		interimBp = 0;
		interimLen = 0;
	    }
	    assert(outputLen);
	    interimBp = *outputBp;
	    *outputBp = 0;
	    interimLen = outputLen;
	    outputLen = 0;
	}

	// At this point, outputBp and outputLen are not set
	// The various decoding routines must allocate their
	// own final storage and set output/output_len accordingly
	//
        
	if (strcasecmp(encodings[enc], "uuencode") == 0) {
	    if (uudecode(outputBp, outputLen, interimBp, interimLen) == 0)
	        continue;
	}

	else if ( (strcasecmp(encodings[enc], "compress") == 0)
		|| (strcasecmp(encodings[enc], "default-compress") == 0)) {
	    if (uncompress(outputBp, outputLen, interimBp, interimLen) == 0)
	        continue;
	}

	// An encoding we cant handle?? PUNT!
	//

	if (interimBp != inputBp) {	// in an interim buffer - slosh to output side
	    *outputBp = interimBp;
	    interimBp = 0;
	    outputLen = (int) interimLen;
	    interimLen = 0;
	}
	else {	// original source - must copy into newly allocated buffer
	    *outputBp = (char *)malloc((size_t) inputLen);
	    memcpy(*outputBp, inputBp, (size_t) inputLen);
	    outputLen = (int) inputLen;
	}
    }

    assert(outputBp != NULL);
    assert(*outputBp != NULL);
    assert(outputLen >= 0);

    // Done decoding the input
    // free up space used to hold copies of encoding rules and return
    //
    free(enc_str);
}

// decode_uue_char -- decode a single uuencoded character
// Arguments:
//  const int val	-- uuencoded character
// Outputs:
//  return char		-- un-uencoded character
// Description
//  Given a single uuencoded character (which is represented as
//  a 7-bit printable ascii character) return an unencoded 6-bit
//  character which can be used to assemble real encoded characters
//

static inline char
decode_uue_char(const int val)
{
    return((val - ' ') & 077);
}

static inline void
fill_copy(unsigned char * out, 
	  const unsigned char * in, 
	  const unsigned char * in_end, 
	  int size_needed)
{
    int in_size = in_end - in + 1;
    memcpy(out, in, in_size);

    memset(out + in_size, ' ', size_needed - in_size);
    out[size_needed - 1] = 0;
}

const unsigned char *
decode_uue_line(char * buf,
	    int & off,
	    const unsigned char * encodedBp, 
	    const unsigned long decodedLen,
	    const unsigned char * encodedEndBp)
{
    unsigned char line_buf[100];
    const unsigned char * line = line_buf;
    const unsigned char * nextEncodedLineStart;
    const unsigned char * nl;
    unsigned long encodedLen;
    unsigned long countDown = decodedLen;

    // See if the line ends with a new line, and is within the range
    // of the buffer.
    // First, compute the length of the encoded byte stream on this line.
    // Given the decoded length, we can do this because the encodings are
    // emitted in groups of four bytes for every three bytes encoded.
    //
    encodedLen = ((decodedLen/3)+((decodedLen%3)!=0))*4;
    if ((encodedBp + encodedLen) > encodedEndBp || encodedBp[encodedLen] != '\n') {
	// Well, looks like some trailing white space was lost in
	// transmission. Lets copy the line and fill in what is needed.
	// (note: if this does happen the source could be corrupted, as the
	// encoded length is derived from the supposed number of decoded
	// characters we have to generate; if the former is off, how can
	// we yield correct bytes for the latter??)
	//

	for (nl = encodedBp; nl <= encodedEndBp && *nl != '\n'; nl++) {
	    continue;
	}

	if (nl < (encodedBp + encodedLen)) {
	    assert(sizeof(line_buf) >= encodedLen);
	    fill_copy(line_buf, encodedBp, nl, (int) encodedLen);
	}
	else {
	    // We will ignore extra characters at the end of the line.
	    // (great...)
	    line = encodedBp;
	}
	
	nextEncodedLineStart = nl + 1;
    }
    else {
	line = encodedBp;
	nextEncodedLineStart = encodedBp + encodedLen + 1;
    }

    // Now the fun begins - obtain a -> the next free byte in the output stream
    // and then spin through the input stream decoding quartets of uuencoded data
    // into triplets of clear text, abstaining as necessary to provide for the
    // last few bytes of the file which may not be mod 3 in length. This routine
    // has been optimized for performance.
    //
    char *lbp = buf+off;	// -> next free byte in output stream
    const unsigned char * encE = (encodedBp + encodedLen);	// -> past last input byte
    for (const unsigned char * enc = encodedBp; enc < encE; enc += 4, countDown -= 3) {
	if (countDown >= 3) {
	  *lbp++ = decode_uue_char(*enc) << 2 | decode_uue_char(*(enc + 1)) >> 4;
	  *lbp++ = decode_uue_char(*(enc + 1)) << 4 | decode_uue_char(*(enc + 2)) >> 2;
	  *lbp++ = decode_uue_char(*(enc + 2)) << 6 | decode_uue_char(*(enc + 3));
        }
        else if (countDown >= 2) {
	  *lbp++ = decode_uue_char(*enc) << 2 | decode_uue_char(*(enc + 1)) >> 4;
	  *lbp++ = decode_uue_char(*(enc + 1)) << 4 | decode_uue_char(*(enc + 2)) >> 2;
        }
        else if (countDown >= 1) {
	  *lbp++ = decode_uue_char(*enc) << 2 | decode_uue_char(*(enc + 1)) >> 4;
        }
    }
    off += (int) decodedLen;	// bump offset by the # of bytes we appended 
    return(nextEncodedLineStart);
}

// SunV3::uncompress -- decode compressed data stream into clear text data stream
// Arguments:
//  outputBp	-- -> location to receive -> clear text results
//  outputLen	-- will contain # of bytes of clear text results returned
//  inputBp	-- -> compressed data stream
//  inputLen	-- number of bytes of compressed data
// Outputs:
//  outputBp	-- contain -> allocated storage containing clear text results
//  outputLen	-- number of bytes of clear text results contained at outputBp
// Returns:
//  == 0 -- successful
//  != 0 -- not successful
// Description:
//  Decode the input data stream using the uncompress algorithm, returning
//  the clear text results of the operation. This is performed by forking
//  a copy of uncompress to allow it to perform the actual processing.
// Notes:
//  For the sake of convenience, implementation of uncompress is done here
//  in a single monolithic function. If another 4n process needs to be added
//  for other types of decoding, the fork and processing functions should
//  be broken out into their own individual routines.
//

int
SunV3::uncompress(char ** outputBp, int & outputLen, const char * inputBp, const unsigned long inputLen)
{
    // We are passed a -> a place to store a pointer to the decoded information
    // and a call-by-ref of a place to store the number of bytes stored. Must 
    // allocate the clear text buffer. Unfortunately, the nature of the compression
    // algorithm makes it imposible to predict ahead of time how many bytes are
    // going to be needed to hold the results. So we allocate an initial buffer
    // that is 2x the size of the input (allowing for 50% compression). We grow
    // the buffer as necessary during the uncompression operation, and shrink it
    // to fit when the final clear text stream size becomes known

    struct pollfd pollFDS[2];			// structure to use for poll() call
    const int childInputFD = 0;			// index into pollFDS to send input to child
    const int childOutputFD = 1;		// index into pollFDS to read output from child

    long int interimOutputLength = 0;
    const long int blockSize = 4096;	// i/o done at least at this size
    long int interimOutputLimit = (inputLen+(blockSize-(inputLen%blockSize)));
    unsigned char *interimOutputBuffer = (unsigned char *)malloc((size_t)interimOutputLimit);
    assert(interimOutputBuffer != NULL);
    unsigned char *interimBp = interimOutputBuffer;

    // Fork off the uncompress function so its ready to process the data
    // 

    int inputPipe[2];		// input pipe descriptors (from child point of view)
    int outputPipe[2];		// output pipe descriptors (from child point of view)
    const int pipeReader = 0;	// pipe[0] is read side of pipe
    const int pipeWriter = 1;	// pipe[1] is write side of pipe
    pid_t childPid;		// pid for child process
    int childStatus;		// placeholder for child exit status

    if (pipe(inputPipe) == -1)			// obtain pipe for child's input
	return(1);
    if (pipe(outputPipe) == -1) {		// obtain pipe for child's output
	(void) SafeClose(inputPipe[pipeReader]);
	(void) SafeClose(inputPipe[pipeWriter]);
	return(1);
    }

    if (!(childPid = fork())) {			// child process
        // Need to clean up a bit before exec()ing the child
	// Close all non-essential open files, signals, etc.
	// NOTE: probably reduce priv's to invoking user too???
	//
	long maxOpenFiles = sysconf(_SC_OPEN_MAX);

	if (maxOpenFiles < 32)		// less than 32 descriptors?
	  maxOpenFiles = 1024;		// don't believe it--assume lots
	for (int sig = 1; sig < NSIG; sig++)
	  (void) signal(sig, SIG_DFL);			// reset all signal handlers
	if (SafeDup2 (inputPipe[pipeReader], STDIN_FILENO) == -1)	// input pipe reader is stdin
	  _exit (1);					// ERROR: exit with bad status
	(void) SafeClose(inputPipe[pipeWriter]);	// input pipe writer n/a
	if (SafeDup2 (outputPipe[pipeWriter], STDOUT_FILENO) == -1)	// output pipe writer is stdout
	  _exit(1);					// ERROR: exit with bad status
	(void) SafeClose(outputPipe[pipeReader]);	// output pipe reader n/a
	// NOTE: we leave standard error output open
	for (int cfd = 3; cfd < maxOpenFiles; cfd++)
	  (void) SafeClose(cfd);			// close all open file descriptors
	(void) execl("/usr/bin/uncompress", "uncompress", "-qc", (char *)0); // try direct route first
        (void) execlp("uncompress", "uncompress", "-qc", (char *)0);	// failed - try via path
	_exit (1);					// failed!? return error exit code
    }

    if (childPid == -1)					// fork failed??
      return(1);					// yes: bail
    
    (void) SafeClose(inputPipe[pipeReader]);		// input pipe reader n/a
    (void) SafeClose(outputPipe[pipeWriter]);		// output pipe writer n/a

#if defined(O_NONBLOCK)
    fcntl(inputPipe[pipeWriter], F_SETFL, O_NONBLOCK);	// we don't want to block writing to child
#elif defined(FNBIO)
    (void) fcntl(inputPipe[pipeWriter], F_SETFL, FNBIO);	// we don't want to block writing to child
#endif

    // Ok, uncompress is out there spinning its wheels waiting for us
    // enter a poll loop responding to file descriptor events
    //

    pollFDS[childInputFD].fd = inputPipe[pipeWriter];	// write input for child process here
    pollFDS[childInputFD].events = POLLOUT;	// tell us when data may be written w/o blocking
    pollFDS[childInputFD].revents = 0;		// no events pending 
    pollFDS[childOutputFD].fd = outputPipe[pipeReader];	// read output from child process here
    pollFDS[childOutputFD].events = POLLIN;	// tell us when data may be read w/o blocking
    pollFDS[childOutputFD].revents = 0;		// no events pending

    unsigned char * currentInputBp = (unsigned char *)inputBp; // track -> input
    unsigned long currentInputCount = inputLen;	// input bytes left to process

    while (poll(pollFDS, 2, INFTIM) > 0) {
	// process events on file descriptors
	// in case two events happen at once, handle the reading from the
	// process first, to make room for data that then may be written

	// process reading output from the child
	// Expand buffer as necessary to contain further data
	//
	if (pollFDS[childOutputFD].revents & POLLIN) {

	    // if there is less than blockSize free bytes left in clear text stream,
	    // expand the buffer by 10% before reading further data from child
	    //
	    if ((interimOutputLimit - interimOutputLength) < blockSize) {
		long int delta = (long int) (interimOutputLimit * 1.10);
		delta = delta+(blockSize-(delta%blockSize));
		interimOutputBuffer = (unsigned char *)realloc(interimOutputBuffer, (size_t)delta);
		assert(interimOutputBuffer != NULL);
		interimOutputLimit = delta;
		interimBp = interimOutputBuffer + interimOutputLength;
	    }

	    // attempt to read as much data from the child as possible
	    //
	    ssize_t readCount = SafeRead(pollFDS[childOutputFD].fd, interimBp, (size_t)(interimOutputLimit - interimOutputLength));
	    if (readCount == -1) {		// error - nuke child and bail
		(void) kill(childPid, SIGKILL);
		break;
            }
	    interimOutputLength += readCount;
	    interimBp += readCount;
	    if (readCount == 0)				// end of file from the child?
		(void) SafeClose(pollFDS[childOutputFD].fd);	// yes, close down child output pipe
	}

	// process writing input to the child
	//
        if (pollFDS[childInputFD].revents & POLLOUT) {

	  // child input queue has foom for more data
	  // if no more data to send, close the child input pipe down
	  //
	  if (currentInputCount == 0) {
	      (void) SafeClose(pollFDS[childInputFD].fd);
	      continue;
	  }

	  // attempt to send as much data as the child will accept
	  //
	  ssize_t writeCount = SafeWrite(pollFDS[childInputFD].fd, currentInputBp, (size_t)currentInputCount);
	  if (writeCount == -1) {		// error - nuke child and bail
	      (void) kill(childPid, SIGKILL);
	      break;
          }
	  currentInputCount -= writeCount;	// bump back by # bytes child accepted
	  currentInputBp += writeCount;		// bump forward index into input stream
	  continue;
        }

	// if both the input and output file descriptors become invalid, then
	// we are done processing the data stream; otherwise, continue to spin
	//
	if ( (pollFDS[childInputFD].revents & (POLLHUP|POLLNVAL)) && (pollFDS[childOutputFD].revents & (POLLHUP|POLLNVAL)) )
	    break;
    }

    // all done processing data -- perform cleanup work 
    //
    (void) SafeClose(inputPipe[pipeWriter]);	// make sure child input pipe closed
    (void) SafeClose(outputPipe[pipeReader]);	// make sure child output pipe closed

    while (SafeWaitpid(childPid, &childStatus, 0) >= 0)	// retrieve child status
	;

    // Hard choices follow: for some reason the uncompress function has exited
    // with a non-zero status - *something* has failed. If so, toss any output
    // that may have been generated, and return a failure indication
    //
    if (childStatus != 0) {
	assert(interimOutputBuffer != NULL);
	free((char *)interimOutputBuffer);	// toss any output
	return(1);
    }

    // All is well - fixup callers output variables and return
    // Also, reduce size of allocated buffer so that unused space
    // is returned to the free pool
    //

    assert(interimOutputLength == (interimBp-interimOutputBuffer));
    if (interimOutputLength < interimOutputLimit)
      interimOutputBuffer = (unsigned char *)realloc(interimOutputBuffer, (size_t)(interimOutputLength+1));
    assert(interimOutputBuffer != NULL);
    
    outputLen = (int)interimOutputLength;	// stuff output length in caller's variable
    *outputBp = (char *)interimOutputBuffer;	// stuff -> clear text in caller's variable
    return(0);
}

// SunV3::uudecode -- decode uuencoded data stream into clear text data stream
// Arguments:
//  outputBp	-- -> location to receive -> clear text results
//  outputLen	-- will contain # of bytes of clear text results returned
//  inputBp	-- -> uuencoded data stream
//  inputLen	-- number of bytes of uuencoded data
// Outputs:
//  outputBp	-- contain -> allocated storage containing clear text results
//  outputLen	-- number of bytes of clear text results contained at outputBp
// Returns:
//  == 0 -- successful
//  != 0 -- not successful
// Description:
//  Decode the input data stream using the uudecode algorithm, returning
//  the clear text results of the operation.
//

int
SunV3::uudecode(char ** outputBp, int & outputLen, const char * inputBp, const unsigned long inputLen)
{
    // We are not really interested in the "begin <mode> <name>" line,
    // so let's blow by it.
    //
    const char * line_1 = inputBp;

    if (strncmp(inputBp, "begin ", 6) == 0) {
	for (line_1 = inputBp; line_1 < (inputBp + inputLen); line_1++) {
	    if (*line_1 == '\n') {
		break;
	    }
	}
    }
    line_1 += 1;

    if (line_1 >= (inputBp + inputLen)) {
	return(1);
    }

    // We are passed a -> a place to store a pointer to the decoded information
    // and a call-by-ref of a place to store the number of bytes stored. Must
    // allocate the buffer. Fortunately, the uuencoding algorithm is regular
    // encoding 3 8-bit bytes into 4 6-bit bytes, so we can allocate a buffer
    // that is big enough to hold the decoded results, plus a little slosh 
    // because the length we compute from includes algrithmic overhead.
    //

    const int totalOutputLen = (int)((inputLen/4)*3);
    *outputBp = (char *)malloc(totalOutputLen);
    outputLen = 0;

    // The first character of each line tells us how many characters
    // to the next new line. We will loop through each line, and
    // decode the characters. Any line shortages are made up with
    // spaces for the decoding algorithm.
    //
    // Note: a properly uuencoded file is ended with two lines:
    //  1- a line beginning with a space (which works out to 0 bytes)
    //  2- the word "end" on a line by itself.
    //
    // In bug #1196898 the following end sequence was encountered:
    //   "L9&%N8V4@=&AE('1I9&4N+BXB"CX@"0D)(" @(" @1RX@0G)O;VMS"CX@"@H@"
    //   ""
    //   "end"
    //   ""
    //   "Janice Anthes, SE                janice.anthes@west.sun.COM"
    //
    // Where there was a blank line with no leading space in it before
    // the "end" line, so we need to add:
    //  3- a completely blank line (which is an invalid uuencoded line)
    //  
    //
    for (const unsigned char * encodedBp = (const unsigned char *)line_1; 
	 (const char *)encodedBp < (inputBp + inputLen);) {

        if ( (*encodedBp == '\n')
	  || ( (*encodedBp == 'e')
	       && (*(encodedBp+1) == 'n')
	       && (*(encodedBp+2) == 'd')
	       && (*(encodedBp+3) == '\n') ) ) {
	  // fix for bug #1196899 - completely blank line
	  break;
	}
	
	int decodedLen = decode_uue_char(*encodedBp);	// # of DECODED bytes

	if (decodedLen <= 0 || decodedLen > 45) {
	    // End of the buffer.
	    break;
	}

	encodedBp += 1;	// bop past the # decoded bytes character
	encodedBp = decode_uue_line(*outputBp,
				 outputLen,
				 encodedBp,
				 decodedLen,
				 (const unsigned char *)(inputBp + inputLen));
	assert(outputLen <= totalOutputLen);
    }

    // Done decoding the entire source - reduce size of allocated buffer
    // so that unused space is returned to the free pool
    //

    if (outputLen < totalOutputLen)
	*outputBp = (char *)realloc(*outputBp, outputLen+1);

    return(0);	// success
}

static inline char
encode_uue_char(const int val)
{
    return((val & 077) + ' ');
}

void
SunV3::encode_uue_line(Buffer & buf,
		   const unsigned char * unencodedBp,
		   const unsigned long unencodedLen)
{
    unsigned long triplets = unencodedLen - (unencodedLen % 3);
    char enc[4];

    for (const unsigned char * cur = unencodedBp; cur < (unencodedBp + triplets - 2); cur += 3) {
	enc[0] = encode_uue_char(*cur >> 2);
	enc[1] = encode_uue_char((*cur << 4) & 060 | (*(cur + 1) >> 4) & 017);
	enc[2] = encode_uue_char((*(cur + 1) << 2) & 074 | (*(cur + 2) >> 6) & 03);
	enc[3] = encode_uue_char(*(cur + 2) & 077);
	buf.appendData(enc, 4);
    }

    crlf(buf);
}

void
SunV3::uuencode(Buffer & buf,
		const char * path,
		const char * bp,
		const unsigned long len)
{
    const unsigned char * ubp = (const unsigned char *) bp;
    const unsigned char * cur;

    // Write out the initial information.
    //
    buf.appendData("begin 600 ", 10);
    buf.appendData(path, strlen(path));
    crlf(buf);

    // We will scan the input buffer, converting to the uuencoded
    // representation. Each line will have 45 characters, not including
    // the initial "M", or trailing line termination.
    //
    unsigned long whole_lines = len - (len % 45);
    unsigned long column = 0;
    for (cur = ubp; cur < (ubp + whole_lines - 2); cur += 45) {
	buf.appendData("M", 1);
	encode_uue_line(buf, cur, 45);
#ifdef DEAD_WOOD
	DtMailProcessClientEvents();
#endif
    }

    // Write the partial line.
    //
    int left_over = (int)(len % 45);
    char size_char = encode_uue_char(left_over);
    buf.appendData(&size_char, 1);

    // uuencode is really memory unfriendly. We need to make sure we
    // don't read past the end of any buffers. We also want to right
    // characters, so we will round the left_over size up to an even
    // grouping of 3 bytes.
    //
    unsigned char safe_buf[45];
    memset(safe_buf, 0, sizeof(safe_buf));
    memcpy(safe_buf, cur, left_over);
    left_over += (3 - (left_over % 3));

    encode_uue_line(buf, safe_buf, left_over);

	crlf(buf);
	buf.appendData("end", 3);
	crlf(buf);
}

void
SunV3::getV3Type(DtMail::BodyPart * bp, char * v3type)
{
    // Get the Dt type name from the body part.
    //
    char * type;
    DtMailEnv error;

    bp->getContents(error,
		    NULL,
		    NULL,
		    &type,
		    NULL,
		    NULL,
		    NULL);

    // Look it up in the data typing system. Hopefully we will
    // get a db based mime name.
    //
    char * db_type = DtDtsDataTypeToAttributeValue(type,
						   "SUNV3_TYPE",
						   NULL);

    // See if we call this text. If so, then it will be text/plain,
    // if not then application/octet-stream
    //
    char * text_type = DtDtsDataTypeToAttributeValue(type,
						     DtDTS_DA_IS_TEXT,
						     NULL);

    if (db_type) {
	strcpy(v3type, db_type);
    }
    else {
	if (text_type && strcasecmp(text_type, "true") == 0) {
	    strcpy(v3type, "text");
	}
	else {
	    strcpy(v3type, "default");
	}
    }

    free(type);
    if (db_type) {
	free(db_type);
    }
    if (text_type) {
	free(text_type);
    }

    return;
}
