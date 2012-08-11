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
 *	$TOG: RFCTransport.C /main/18 1998/07/24 16:09:46 mgreess $
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
 *	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef I_HAVE_NO_IDENT
#endif

#ifdef __ppc
#include <DtMail/Buffer.hh>
#endif

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <signal.h>
#include <assert.h>

#include <DtMail/DtMail.hh>
#include <DtMail/DtMailP.hh>
#include "RFCImpl.hh"
#include "SigChldImpl.hh"
#include "RFCMIME.hh"
#include "SunV3.hh"
#include "AliasExpand.hh"
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>

#ifdef HAS_VFORK
  #define DTMAIL_FORK	vfork
#else
  #define DTMAIL_FORK	fork
#endif

// pipe between childHandler and XtAppAddInput
static int _transfds[2];
// list of children that have been forked
static waitentry_t *_waitlist;

// Note on error handling in wroteToFileDesc: this is used
// to write e-mail to all open *files* (e.g. recording to
// file, mail to file), so we attempt to write to all file
//
static unsigned long
writeToFileDesc(const char * buf, int len, va_list args)
{
    int * fds = va_arg(args, int *);
    int cnt = va_arg(args, int);
    DtMailBoolean strip = va_arg(args, DtMailBoolean);

    unsigned long saveErrno = 0;	// Initially no error recorded

    for (int fd = 0; fd < cnt; fd++) {
	int status = 0;
	do {
	    if (strip)
	    	status = SafeWriteStrip(fds[fd], buf, len);
	    else
	    	status = SafeWrite(fds[fd], buf, len);
	} while (status < 0 && errno == EAGAIN);

	if (status < 0 && errno != 0)		// Did an error occur??
	  saveErrno = (unsigned long)errno;	// Yes: remember "last" errno 

#ifdef DEAD_WOOD
	DtMailProcessClientEvents();
#endif /* DEAD_WOOD */
    }

    return(saveErrno);		// return last error recorded
}

RFCTransport::RFCTransport(DtMailEnv & error,
			   DtMail::Session * session,
			   DtMailStatusCallback cb,
			   void * cb_data,
			   const char * impl)
: DtMail::Transport(error, session, cb, cb_data)
{
    error.clear();

    _object_valid = new Condition;
    _object_valid->setTrue();

    _impl = strdup(impl);

    // Set up the handlers so that we are notified when a child
    // process exits and do the right thing.
    signalRegister();
}

RFCTransport::~RFCTransport(void)
{
    _object_valid->setFalse();
    free(_impl);
}

DtMailOperationId
RFCTransport::submit(DtMailEnv & error, DtMail::Message * msg, DtMailBoolean log_msg)
{
    waitentry_t * new_we;
    int child_pid;

    // Create a list of information about all child processes.
    if ((new_we = (waitentry_t *) malloc(sizeof(waitentry_t))) == NULL)
    {
	error.setError (DTME_NoMemory);
	return(NULL);
    }

    // fork a new process
    switch(child_pid = (int) fork()) {
	case -1:
	{
	    // if the fork fails, cleanup
	    free (new_we);
	    error.setError (DTME_NoMemory);
	    break;
	}
	case 0:		/* child */
	{
	    // reset all signal handlers
	    for (int sig = 1; sig < NSIG; sig++)
	    {
	        (void) signal(sig, SIG_DFL);		
	    }

	    DtMailEnv tmp_error;
	    tmp_error.clear();

	    format(tmp_error, msg, log_msg);
	    _exit((int)((DTMailError_t)tmp_error)); 
	}
	default:		/* parent */
	{
	    // Save information about each child process (sendmail)
	    // that we fork/exec.
	    new_we->pid = child_pid;
	    new_we->proc = this->_error_proc;
	    new_we->data = this->_smd;
	    new_we->next = _waitlist;
	    _waitlist = new_we;
	    return(NULL);
	}
    }

    return(NULL);
}

void
RFCTransport::format(DtMailEnv & error, DtMail::Message * dtmsg, DtMailBoolean log_msg)
{
    // Clean up the message addressing for delivery.
    //
//    DtMailEnv * thr_error = new DtMailEnv();
//    thr_error->clear();

    DtMailValueSeq value;
    char * to = NULL;
    char * cc = NULL;
    char * bcc = NULL;

    DtMail::Envelope * env = dtmsg->getEnvelope(error);

    DtMailAddressSeq addr_tokens(32);

    DtMailEnv tmp_error;
    tmp_error.clear();
    
    env->getHeader(tmp_error, DtMailMessageTo, DTM_TRUE, value);
    if (tmp_error.isNotSet()) {
	to = concatValue(value);
	DtMailAddressSeq to_tokens(16);
	arpaPhrase(to, to_tokens);
	rfcAliasExpand(error, *_session->mailRc(error), to_tokens);
	appendAddrs(addr_tokens, to_tokens);
	skinFiles(to_tokens);
	char * new_to = addrToString(to_tokens);
	env->setHeader(tmp_error, "To", DTM_TRUE, new_to);
	delete [] new_to;
	free(to);
    }
    // Clear the error structure before passing it to a function.
    tmp_error.clear();
    value.clear();

    env->getHeader(tmp_error, DtMailMessageCc, DTM_TRUE, value);
    if (tmp_error.isNotSet()) {
	cc = concatValue(value);
	DtMailAddressSeq cc_tokens(16);
	arpaPhrase(cc, cc_tokens);
	rfcAliasExpand(error, *_session->mailRc(error), cc_tokens);
	appendAddrs(addr_tokens, cc_tokens);
	skinFiles(cc_tokens);
	char * new_cc = addrToString(cc_tokens);
	env->setHeader(tmp_error, "Cc", DTM_TRUE, new_cc);
	delete [] new_cc;
	free(cc);
    }
    // Clear the error structure before passing it to a function.
    tmp_error.clear();
    value.clear();

    env->getHeader(tmp_error, DtMailMessageBcc, DTM_TRUE, value);
    if (tmp_error.isNotSet()) {
	bcc = concatValue(value);
	DtMailAddressSeq bcc_tokens(16);
	arpaPhrase(bcc, bcc_tokens);
	rfcAliasExpand(error, *_session->mailRc(error), bcc_tokens);
	appendAddrs(addr_tokens, bcc_tokens);
	free(bcc);
    }
    value.clear();

    // Note for now we ignore all errors from "rfcAliasExpand"
    //
    error.clear();

    // Now we format the message.
    // These messages do NOT include content-length as they are intended
    // only for the delivery agent. If we are writing to local files,
    // after remote delivery the local delivery will generate its own
    // messages that have content-length included
    //
    RFCFormat * fmt;
    RFCFormat * logFmt;

    if (strcmp(_impl, "Internet MIME") == 0) {
	fmt = new RFCMIME(_session);
	logFmt = new RFCMIME(_session);
      }
    else {
	fmt = new SunV3(_session);
	logFmt = new SunV3(_session);
      }

    // Prepare to fire up the delivery agent
    // The delivery agent preprocessor examines each addressee and if it is
    // really a local file (eg folders) the local file is opened and its file
    // descriptor is appended to the "log_files" array, which is allocated
    // by the delivery agent preprocessor via 'new'. Upon return to us, if
    // the log_count is > 0 we must format the message to be stored in a
    // local file (include content length) and cause the message to be
    // written to each open file
    //
    int *log_files = 0;
    int log_count = 0;

    { // Create local scope to contain headers/bodies so that they go away
      // as soon as they are no longer needed, before we potentially
      // create a duplicate set for writing to local files
      //
      BufferMemory headers(1024);
      BufferMemory bodies(8192);

      error.clear();
      //call unsetIsWriteBcc so that the Bcc field will not write to headers
      // buffer (see aix defect 177089
      fmt->unsetIsWriteBcc(); 
      fmt->msgToBuffer(error, *dtmsg, DTM_FALSE, DTM_FALSE, DTM_FALSE,
	headers, bodies);

      if (!error.isSet())
        deliver(error, addr_tokens, headers, bodies, log_msg, &log_files, log_count);
    }

    delete fmt;

    // If any log files have been opened, format this message for local
    // storage and cause the message to be written to all open files
    //
    tmp_error.clear();		// Used to cache error from logging to files
    
    if (log_count) {
      BufferMemory logHeaders(1024);
      BufferMemory logBodies(8192);

      assert(log_files != NULL);	// if log files open, must have array

      //
      // Mark the message as having been read.
      //
      env->setHeader(error, "Status", DTM_TRUE, "RO");

      //call setIsWriteBcc so that the Bcc field will write to logHeaders
      // buffer (see aix defect 177089)
      logFmt->setIsWriteBcc(); 
      logFmt->msgToBuffer(tmp_error, *dtmsg, DTM_TRUE, DTM_FALSE, DTM_FALSE,
			    logHeaders, logBodies);

      if (!tmp_error.isSet()) {
	unsigned long iterateErrno, iterateErrno1;
	
        iterateErrno = logHeaders.iterate(writeToFileDesc, log_files, log_count, DTM_TRUE);
        iterateErrno1 = logBodies.iterate(writeToFileDesc, log_files, log_count, DTM_TRUE);
	if ( (iterateErrno != 0) || (iterateErrno1 != 0) )
	  tmp_error.setError(DTME_ObjectCreationFailed);

      }

      closeLogFiles(log_files, log_count);
      delete log_files;
    }

    delete logFmt;

/* Comment out this code because it's part of old thread code.  Why
   execute code that does nothing if we don't have to?
    // Tell the requestor we're all done now.
    //
    DtMailEventPacket packet;
    packet.key = _key;
    packet.target = DTM_TARGET_TRANSPORT;
    packet.target_object = this;
    packet.operation = (void *)ThreadSelf();
    packet.argument = thr_error;
    packet.event_time = time(NULL);

    if (_object_valid->state()) {
	_session->writeEventData(error, &packet, sizeof(DtMailEventPacket));
    }
*/

    // If no error occurred during the deliver invocation, but
    // an error occurred during local file delivery, propagate
    // that error into the error returned to the caller
    //
    if (!error.isSet() && tmp_error.isSet())
      error.setError((DTMailError_t)tmp_error);
    
    return;
}

void
RFCTransport::deliver(DtMailEnv & error,
		      DtMailAddressSeq & addr_tokens,
		      Buffer & headers,
		      Buffer & bodies,
		      DtMailBoolean log_msg,
		      int **log_files,
		      int & log_count)
{
    DtMailEnv merror;
    merror.clear();

    const char * value;

    assert(log_files != NULL);		// must provide -> log_files ->
    
    // We want to make an argv list that is big enough to hold all
    // of the addresses. Of course, this may need to be expanded
    // because of local aliases, but we'll work on that.
    //
    char ** argv = (char **)malloc(sizeof(char *) * (addr_tokens.length() + 5));
    int argc = 1;

    *log_files = 0;
    log_count = 0;

    if (log_msg == DTM_TRUE) {
	const char * log;
	char *buf = NULL;

    	_session->mailRc(merror)->getValue(merror, "record", &log);
	if (merror.isSet()) {
	    log = "~/sent.mail";
	}

	if (*log != '/' && *log != '~' && *log != '+' && *log != '$') {

	    _session->mailRc(merror)->getValue(merror, "outfolder", &value);
	    buf = (char *)malloc(strlen(log) + 3);
	    if (buf != NULL) {
	        if (merror.isSet()) {
	            // "outfolder" is not set. Relative to home directory
		    strcpy(buf, "~/");
		    merror.clear();
		} else {
		    // "outfolder" is set. Relative to folder directory
		    strcpy(buf, "+");
		}
		strcat(buf, log);
		log = buf;
	    }
	}
	int fd = openLogFile(log);
	if (fd >= 0) {
	    if (!*log_files)
              * log_files = new int[addr_tokens.length() + 4];
	    (*log_files)[log_count++] = fd;
	}
	
	if (buf != NULL) {
		free(buf);
	}
    }

    // We add the correct parameter to sendmail so that it will ignore
    // lines with a single dot(.) in them.  We also check to see
    // if we should add a couple of optional parameters: metoo and
    // verbose.
    argv[argc++] = "-oi";
    _session->mailRc(merror)->getValue(merror, "metoo", &value);
    if (merror.isNotSet()) {
	// "metoo" is set.
	argv[argc++] = "-m";
    } else {
	merror.clear();
    }

    _session->mailRc(merror)->getValue(merror, "verbose", &value);
    if (merror.isNotSet()) {
	// verbose is set.
	argv[argc++] = "-v";
    } else {
	merror.clear();
    }

    // We now look at each address. If the name starts with a + or
    // "/" then look further. If the name contains an "@" we assume
    // it is probably an email address. Otherwise we assume it is
    // a file and copy it to the file system.
    //
    int is_addr;
    const char * at;
    for (int addr = 0; addr < addr_tokens.length(); addr++) {
	switch (*(addr_tokens[addr]->dtm_address)) {
	  case '+':
	  case '/':
	    is_addr = 0;
	    for (at = addr_tokens[addr]->dtm_address; *at; at++) {
		if (*at == '@') {
		    is_addr = 1;
		    break;
		}
	    }
	    if (is_addr) {
		argv[argc++] = addr_tokens[addr]->dtm_address;
	    }
	    else {
		int fd = openLogFile(addr_tokens[addr]->dtm_address);
		if (fd >= 0) {
		    if (!*log_files)
		      * log_files = new int[addr_tokens.length() + 4];
		    (*log_files)[log_count++] = fd;
		}
	    }
	    break;
	    
	  default:
	    argv[argc++] = addr_tokens[addr]->dtm_address;
	}
    }
    
    argv[argc] = NULL;

    launchSendmail(error, headers, bodies, argv);
    
    assert((!log_count && ! *log_files) || (log_count && *log_files));
    
    free(argv);
}

static void
skin_comma(char * buf)
{
    char * last_c = &buf[strlen(buf) - 1];

    while(last_c > buf && isspace((unsigned char)*last_c)) {
	*last_c = 0;
	last_c -= 1;
    }

    if (last_c > buf && *last_c == ',') {
	*last_c = 0;
    }
}

void
RFCTransport::arpaPhrase(const char * name, DtMailAddressSeq & tokens)
{
    register char c;
    register const char *cp;
    char *cp2;
    char *nbufp;
    char *bufend;
    int gotlt, lastsp, didq, rem;
    int nesting, extra;
    int token = 1;
    int comma = 0;
    DtMailValueAddress * addr;
    
    if (name == (char *) 0) {
	return;
    }
    /* count comma's; we may need up to one extra space per comma... */
    extra = 1;
    cp = name;
    for (;;) {
	cp = strchr(cp, ',');
	
	if (!cp) break;
	
	for(cp += 1; *cp && isspace((unsigned char)*cp); cp++) {
	    extra++;
	}
    }
    
    nbufp = (char *)malloc(strlen(name) + extra);

    char * tok_start = nbufp;
    int tok_len;
    int count_at_comma = 0;

    gotlt = 0;
    lastsp = 0;
    bufend = nbufp;
    cp = name;
    for (cp = name, cp2 = bufend; (c = *cp++) != 0;) {
	switch (c) {
	  case '(':
	    /*
	      Start of a comment, ignore it.
	      */
	    nesting = 1;
	    while ((c = *cp) != 0) {
		cp++;
		switch(c) {
		  case '\\':
		    if (*cp == 0) goto outcm;
		    cp++;
		    break;
		  case '(':
		    nesting++;
		    break;
		  case ')':
		    --nesting;
		    break;
		}
		if (nesting <= 0) break;
	    }
	  outcm:
	    lastsp = 0;
	    break;
	    
	  case '"':
	    /*
	      Start a quoted string.
	      Copy it in its entirety.
	      */
	    didq = 0;
	    while ((c = *cp) != 0) {
		cp++;
		switch (c) {
		  case '\\':
		    if ((c = *cp) == 0) goto outqs;
		    cp++;
		    break;
		  case '"':
		    goto outqs;
		}
		if (gotlt == 0 || gotlt == '<') {
		    if (lastsp) {
			lastsp = 0;
			*cp2++ = ' ';
		    }
		    if (!didq) {
			*cp2++ = '"';
			didq++;
		    }
		    *cp2++ = c;
		}
	    }
	  outqs:
	    if (didq)
		*cp2++ = '"';
	    lastsp = 0;
	    break;
	    
	  case ' ':
	  case '\t':
	  case '\n':
	    if (token && (!comma || c == '\n')) {
	      done:
		addr = new DtMailValueAddress;
		tok_len = cp2 - tok_start;
		addr->dtm_address = (char *)malloc(tok_len + 1);
		memcpy(addr->dtm_address, tok_start, tok_len);
		addr->dtm_address[tok_len] = 0;
		addr->dtm_person = NULL;
		addr->dtm_namespace = strdup(DtMailAddressDefault);
		skin_comma(addr->dtm_address);
		tokens.append(addr);

		while(*cp && isspace((unsigned char)*cp)) {
		    cp++;
		}

		tok_start = cp2;
	    }
	    break;
	    
	  case ',':
	    *cp2++ = c;
	    if (gotlt != '<') {
		bufend = cp2 + 1;
		count_at_comma = tokens.length();
		gotlt = 0;
		if (token) {
		    count_at_comma++;
		    goto done;
		}
	    }
	    break;
	    
	  case '<':
	    cp2 = bufend;
	    for (rem = (tokens.length() - 1); tokens.length() > count_at_comma;
		 rem = (tokens.length() - 1)) {
		DtMailValueAddress * bad_addr = tokens[rem];
		delete bad_addr;
		tokens.remove(rem);
	    }
	    tok_start = cp2;
	    gotlt = c;
	    lastsp = 0;
	    break;
	    
	  case '>':
	    if (gotlt == '<') {
		gotlt = c;
		break;
	    }
	    
	    /* FALLTHROUGH . . . */
	    
	  default:
	    if (gotlt == 0 || gotlt == '<') {
		if (lastsp) {
		    lastsp = 0;
		    *cp2++ = ' ';
		}
		*cp2++ = c;
	    }
	    break;
	}
    }
    *cp2 = 0;

    if (cp2 > tok_start) {
	addr = new DtMailValueAddress;
	addr->dtm_address = strdup(tok_start);
	addr->dtm_person = NULL;
	addr->dtm_namespace = strdup(DtMailAddressDefault);
	skin_comma(addr->dtm_address);
	tokens.append(addr);
    }

    free(nbufp);
}


//
// SendMsgDialog has some info that is needed here.
//
void
RFCTransport::initTransportData(int fds[2], SubProcessFinishedProc proc,
	void *ptr)
{
	_transfds[0] = fds[0];
	_transfds[1] = fds[1];
	_smd = ptr;
	_error_proc = proc;
}

//
// Pass a ptr to sendmailReturnProc to SendMsgDialog so that it can
// call it in XtAppAddInput
//
void *
RFCTransport::getSendmailReturnProc(void)
{
	return ((void *)(&RFCTransport::sendmailReturnProc));
}


void
RFCTransport::launchSendmail(DtMailEnv & error,
                             Buffer & headers,
                             Buffer & bodies,
                             char ** argv)
{
    // We need to retrieve the name of the sendmail program.
    // if none is set then we use the default mailer.
    //
    const char * mailer;
    _session->mailRc(error)->getValue(error, "sendmail", &mailer);
    if (error.isSet()) {
#if defined(USL) || defined(__uxp__)
        mailer = "/usr/ucblib/sendmail";
#else
        mailer = "/usr/lib/sendmail";
#endif
    }
 
    error.clear();
 
    argv[0] = (char *)mailer;
 
    // If we have only one arg, then everything goes to the log files.
    // Don't do the fork and exec.
    //
    if (argv[1] == NULL)
      return;
 
    // We need a pipe to write the message to sendmail.
    //
    int inputPipe[2];
    const int pipeReader = 0;           // pipe[0] is read side of pipe
    const int pipeWriter = 1;           // pipe[1] is write side of pipe
 
    if (pipe(inputPipe)==-1) {          // Attempt to get a pipe
      error.setError(DTME_NoMemory);    // this must be really bad...
      return;
    }
 
    // We need to fork and send the data to sendmail.
    // Use vfork when available because the only purpose
    // of the child is to do an exec
    //
    pid_t childPid = DTMAIL_FORK();
    if (childPid == 0) {                // The child **********


        // Need to clean up a bit before exec()ing the child
        // Close all non-essential open files, signals, etc.
        // NOTE: probably reduce priv's to invoking user too???
        //
        long maxOpenFiles = sysconf(_SC_OPEN_MAX);
 
        if (maxOpenFiles < 32)          // less than 32 descriptors?
          maxOpenFiles = 1024;          // dont believe it--assume lots
 
        for (int sig = 1; sig < NSIG; sig++)
          (void) signal(sig, SIG_DFL);  // REset all signal handlers

	// input pipe reader is stdin
        if (SafeDup2(inputPipe[pipeReader], STDIN_FILENO) == -1)
          _exit (1);                    // ERROR: exit with bad status

        // NOTE: we leave standard output and standard error output open
        (void) SafeClose(inputPipe[pipeWriter]); // input pipe writer n/a
        for (int cfd = 3; cfd < maxOpenFiles; cfd++)
          (void) SafeClose(cfd); // close all open file descriptors
	
#if 0
	printf("Command:  %s\n",mailer);
	int k=0;
	while (NULL != argv[k])
	{
	  printf("Command line %d:  %s\n", k, argv[k]);
	  k++;
	}
#endif
	(void) execvp(mailer, (char * const *)argv);

        _exit(1); // Should never get here!
    }


    // The parent
    //
    if (childPid < 0) {                                 // did the fork fail??
      error.setError(DTME_NoMemory);                    // yes: bail
      return;
    }
 
    (void) SafeClose(inputPipe[pipeReader]); // input pipe reader n/a
 
    // Sendmail files
    //
    headers.iterate(writeToFileDesc, &inputPipe[pipeWriter], 1, DTM_FALSE);
    bodies.iterate(writeToFileDesc, &inputPipe[pipeWriter], 1, DTM_FALSE);
    (void) SafeClose(inputPipe[pipeWriter]); // force EOF on mail age nt's input
 
    // Now we wait on the condition variable until the child's
    // process status is reported.
    //
    int status;
    int ret_status;
    int err_ret = SafeWaitpid(childPid, &status, 0);
 
    if (err_ret < 0) {
        // Somebody beat us to the status of the child.
        // Just assume the best possible outcome.
        //
        error.clear();
    }
    // If the low byte of the status code returned from wait
    // is 0, then the high byte is the status returned from
    // the child.  If the low byte is anything else, there
    // was an error.
    else if (lowByte(status) == 0)
    {
        ret_status = highByte(status);
        switch (ret_status) {
          case 67:
          case 68:
            error.setError(DTME_BadMailAddress);
            break;
 
          case 0:
            error.clear();
            break;
 
          default:
            error.setError(DTME_TransportFailed);
        }
    }
    else
    {
        status = -1;
    }
}


void
RFCTransport::sendmailReturnProc(void)
{
	pipedata_t 	new_pd;
	waitentry_t 	* ptr, ** prev;
	int 		status;
	DtMailEnv	error;

	// Now that the child process (sendmail) has finished, read
	// its pid and status from the transfds pipe.
	if(read(_transfds[0], &new_pd,
		sizeof(new_pd))!=sizeof(new_pd)) {
		// ERROR - can't read pipe so just return?
		error.setError (DTME_NoMemory);
		error.logError(DTM_TRUE,
		  "RFCTransport: sendmailReturnProc(): Failed to read pipe\n");
		return; 
	}
		
	ptr =  _waitlist;
	prev = &_waitlist;

	// Loop through the waitlist which is a list of all the 
	// child processes (sendmail) that the parent exec'd.  When
	// the pid in the list matches the child process that has
	// exited, we've found it.
	while(ptr) {
		if(ptr->pid == new_pd.pid) 
		    break;
		prev = &(ptr->next);
		ptr = ptr->next;
	}

	// We couldn't match the pid so just do nothing.
	if(!ptr) {
		// ERROR just return
		return;
	}
	
	*prev = ptr->next;

	// If the low byte of the status code returned from wait
	// is 0, then the high byte is the status returned from
	// the child.  If the low byte is anything else, there
	// was an error.
	if (lowByte(new_pd.status) == 0)
	{
	    status = highByte(new_pd.status);
	}
	else
	{
	    status = -1;
	}

	// Now that we've identified the child process, call the proc
	// associated with it, pass the child's pid and status and any
	// extra data.
	ptr->proc(new_pd.pid, status, ptr->data);

	// Cleanup
	free((void*)ptr);
}

//
// When a child process finishes, child_handler writes its pid
// and status onto the transfds pipe.  XtAppAddInput calls SendmailReturnProc
// to read from this pipe.
//
// The reason we don't just call SendmailReturnProc directly is twofold.
// The amount of processing should be kept to a minimum in a
// signal handler and we don't really know where X is in it's
// processing.  It may not be appropriate to pop a dialog up.
void 
RFCTransport::childHandler(void)
{
	pipedata_t d;

        // Now that the child is finished, its a zombie process
        // until we do the wait.  wait for the child and get its
        // pid and return status.  write these to the transfds pipe.
	// Be sure to reap all processes so that none get lost.
        while (d.pid = (int) waitpid ((pid_t) -1, &d.status, WNOHANG))
        {
            if (d.pid > 0)
                SafeWrite(_transfds[1], &d, sizeof(d));
            else
            {
                if (errno == ECHILD)
                    break;
            }
        }
}

//
// Listen for the child processes when they exit.  On exit from a
// child process, call child_handler and have it write to 
// XtAppAddInput which will call SendmailReturnProc.
void
RFCTransport::signalRegister(void)
{
    static int		initialized = 0;
    struct sigaction	act;

    if (initialized) return;
    initialized = 1;

#if defined(hpux) || defined(_aix) || defined(__osf__) || defined(linux) || \
    (defined(sun) && OSMAJORVERSION>=5 && OSMINORVERSION>4) || defined(CSRG_BASED)
    // SunOS 5.5 and above defined prototype for signal handler
    act.sa_handler = (void (*)(int))&RFCTransport::childHandler;
#else
    // SunOS 5.4 and before defined prototype signal handler
    act.sa_handler = (void (*)())&RFCTransport::childHandler;
#endif
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGCHLD);
    act.sa_flags = 0;
   
    sigaction(SIGCHLD, &act, NULL);

    return;
}

//
// fork/exec the child process and return the child process pid
//
int 
RFCTransport::startSubprocess(DtMailEnv &error, char * cmd,
	Buffer & headers, Buffer & bodies, char ** argv)
{
	int child_pid;
	int sendfds[2];

	error.clear();

	// Create a pipe to write any necessary information
	// from the parent process to the child process.
	if(pipe(sendfds) < 0) 
	{
	    error.setError (DTME_NoMemory);
	    return(-1);
	}

	// fork a new process
        // Use vfork when available because the only purpose
	// of the child is to do an exec
	switch(child_pid = (int) DTMAIL_FORK()) {

	    case -1:
	    {
		// if the fork fails, cleanup
		SafeClose(sendfds[0]);
		SafeClose(sendfds[1]);
	        error.setError (DTME_NoMemory);
		break;
	    }
	    case 0:		/* child */
	    {
        	// Need to clean up a bit before execing the child
		// Close all non-essential open files, signals, etc.
		// NOTE: probably reduce priv's to invoking user too???
		long maxOpenFiles = sysconf(_SC_OPEN_MAX);

		// less than 32 descriptors?
		if (maxOpenFiles < 32)		
		{
		    // dont believe it--assume lots
		    maxOpenFiles = 1024;		
		}

		// reset all signal handlers
		for (int sig = 1; sig < NSIG; sig++)
		{
		    (void) signal(sig, SIG_DFL);		
		}

		// The child process (sendmail) needs to read info
		// across the pipe from the parent process (dtmail).
		// input pipe reader is stdin
		// SafeDup2 will close stdin and dup sendfds[0] to stdin
		// then close sendfds[0]
		if (SafeDup2(sendfds[0], STDIN_FILENO) == -1)
		{
		    // ERROR: exit with bad status
	  		_exit (1);
		}
	
		// We need to close the write end of the pipe.
		// NOTE: we leave standard output and standard error output 
		// open, input pipe writer n/a
		(void) SafeClose(sendfds[1]);
	
		// close all open file descriptors
		for (int cfd = 3; cfd < maxOpenFiles; cfd++)
		{
	  	    (void) SafeClose(cfd);			
		}

		// exec sendmail
		(void) SafeExecvp(cmd, (char *const *)argv);

		// Should never get here!
		_exit(1); 					

	    }
	    default:		/* parent */
	    {
		// Close the input pipe reader
		(void) SafeClose(sendfds[0]);

		// Write the mail message to the pipe.  The child process
		// (sendmail) will read from the pipe and send the message.
    		unsigned long iterateErrno;
    
		iterateErrno = headers.iterate(writeToFileDesc, &sendfds[1],
			1, DTM_FALSE);
		if (iterateErrno == 0)
		{
		    iterateErrno = bodies.iterate(writeToFileDesc, &sendfds[1],
			1, DTM_FALSE);
		}

		if (iterateErrno != 0)
		    error.setError(DTME_ObjectCreationFailed);

		// When we are done sending the message,
		// force EOF on mail agent's input
		(void) SafeClose(sendfds[1]);

		// Don't wait for the child process (sendmail) to return.
		// Instead, we've registered for notification (SIGCHLD)
		// when the child exits and will invoke a handler to
		// do the right thing.

		return(child_pid);
	    }
	}

	return(-1);
}

char *
RFCTransport::concatValue(DtMailValueSeq & value)
{
    // Count the string sizes.
    //
    int tot_size = 0;
    int valueLength = value.length();
    for (int n = 0; n < valueLength; n++) {
	tot_size += strlen(*(value[n]));
	tot_size += 5; // Fudge for null, commas, and space.
    }

    char * str = new char[tot_size];

    *str = 0;
    for (int cp = 0; cp < valueLength; cp++) {
	strcat(str, *(value[cp]));
	if (cp != (valueLength - 1)) {
	    strcat(str, ", ");
	}
    }

    return(str);
}

void
RFCTransport::appendAddrs(DtMailAddressSeq & to, DtMailAddressSeq & from)
{
  int fromLength = from.length();
  
    for (int f = 0; f < fromLength; f++) {
	to.append(new DtMailValueAddress(*from[f]));
    }
}

void
RFCTransport::skinFiles(DtMailAddressSeq & addrs)
{
    for (int a = 0; a < addrs.length(); a++) {
	DtMailValueAddress * t_addr = addrs[a];
	if (*t_addr->dtm_address == '+' ||
	    *t_addr->dtm_address == '/') {
	    int is_addr = 0;
	    for (const char * c = t_addr->dtm_address; *c; c++) {
		if (*c == '@') {
		    is_addr = 1;
		    break;
		}
	    }

	    if (!is_addr) {
		addrs.remove(a);
		a -= 1;
	    }
	}
    }
}

char *
RFCTransport::addrToString(DtMailAddressSeq & addrs)
{
    // Compute worse case string size.
    //
    int len = 0;
    int addrsLength = addrs.length();
    for (int s = 0; s < addrsLength; s++) {
	DtMailValueAddress * s_addr = addrs[s];
	len += strlen(s_addr->dtm_address) + 5;
    }
    len += 20;

    char * addr_str = new char[len];
    *addr_str = 0;

    for (int c = 0; c < addrsLength; c++) {
	DtMailValueAddress * c_addr = addrs[c];
	if (c) {
	    strcat(addr_str, ", ");
	}
	strcat(addr_str, c_addr->dtm_address);
    }

    return(addr_str);
}

int
RFCTransport::openLogFile(const char * path)
{
    DtMailEnv error;
    error.clear();
    
    char * exp_path = _session->expandPath(error, path);
    if (error.isSet())
      return(-1);

    int fd = SafeOpen(exp_path, O_RDWR | O_APPEND | O_CREAT, 0600);
    free(exp_path);
    if (fd < 0) {
	return(fd);
    }

    // Generate the Unix From line...
    //
    passwd pw;
    GetPasswordEntry(pw);

    char *from_buf = new char[256];
    char *time_buf = new char[256];
    time_t now = time(NULL);
    SafeCtime(&now, time_buf, sizeof(time_buf));

    sprintf(from_buf, "From %s %s", pw.pw_name, time_buf);
    SafeWrite(fd, from_buf, strlen(from_buf));

    delete [] from_buf;
    delete [] time_buf;

    return(fd);
}

void
RFCTransport::closeLogFiles(int * files, int file_cnt)
{
    for (int fd = 0; fd < file_cnt; fd++) {
	SafeWrite(files[fd], "\n", 1);
	SafeClose(files[fd]);
    }
}

void
RFCWriteMessage(DtMailEnv & error,
		DtMail::Session * session,
		const char * path,
		DtMail::Message * msg)
{
    RFCFormat * fmt;
    BufferMemory headers(1024);
    BufferMemory bodies(8192);

    fmt = new RFCMIME(session);
      //call setIsWriteBcc so that the Bcc field will write to headers
      // buffer (see aix defect 177089)
    fmt->setIsWriteBcc(); 
    fmt->msgToBuffer(error, *msg, DTM_TRUE, DTM_TRUE, DTM_FALSE,
		     headers, bodies);

    if (error.isSet()) {
	return;
    }

    int fd = SafeOpen(path, O_RDWR | O_CREAT | O_TRUNC, 0600);

    if (fd < 0) {
	error.setError(DTME_ObjectCreationFailed);
	delete fmt;
	return;
    }

    char *fsname=(char *)error.getClient();

    int len=headers.getSize()+bodies.getSize();
/*
    printf("\n message body len=%d",len);
*/
    if( error.isSet() || !FileSystemSpace(path, len,&fsname) )
    {
        error.setError(DTME_OutOfSpace);
        error.setClient((void *)fsname);
        close(fd);
        remove(path);
        return;
    }

    unsigned long iterateErrno;
    
    iterateErrno = headers.iterate(writeToFileDesc, &fd, 1, DTM_FALSE);
    if (iterateErrno == 0)
      iterateErrno = bodies.iterate(writeToFileDesc, &fd, 1, DTM_FALSE);

    if (iterateErrno != 0)
      error.setError(DTME_ObjectCreationFailed);
    
    delete fmt;
    close(fd);
}
