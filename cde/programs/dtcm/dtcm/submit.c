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
/*******************************************************************************
**
**  submit.c
**
**  $XConsortium: submit.c /main/5 1996/10/03 10:58:04 drk $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <LocaleXlate.h>
#include <RFCMIME.h>
#include "rerule.h"			/* FALSE */


static char **
arpaPhrase(const char * name)
{
    register char c;
    register const char *cp;
    char * cp2;
    int gotlt, lastsp, didq;
    int nesting;
    const char * last_comma = name;
    int biggest = 0;
    int n_addrs = 0;
    int cur_addr;
    int distance;
    char ** addrs;
    const char * comma;
    const char * start;
    
    if (name == (char *) 0) {
	return(NULL);
    }

    /* We need to figure out what is the biggest possible address.
       This will be the maximum distance between commas.
       */
    for (comma = name; *comma; comma++) {
	if (*comma == ',') {
	    n_addrs += 1;
	    distance = comma - last_comma;
	    biggest = biggest < distance ? distance : biggest;
	    last_comma = comma;
	}
    }
    distance = comma - last_comma;
    biggest = biggest < distance ? distance : biggest;
    biggest += 2; /* Just in case. */

    cur_addr = 0;
    addrs = (char **)malloc((n_addrs + 1) * sizeof(char *));

    cp2 = (char *)malloc(biggest);
    addrs[cur_addr++] = cp2;

    gotlt = 0;
    lastsp = 0;
    start = name;
    for (cp = name; (c = *cp++) != 0;) {
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
	      done:
	    *cp2 = 0;

	    cp2 = (char *)malloc(biggest);
	    addrs[cur_addr++] = cp2;

	    lastsp = 1;
	    break;
	    
	  case ',':
	    *cp2++ = c;
	    if (gotlt != '<') {
		gotlt = 0;
		goto done;
	    }
	    break;
	    
	  case '<':
	    cp2 = addrs[cur_addr - 1];
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
    addrs[cur_addr] = NULL;
    return(addrs);
}

static char *
formatMessage(char ** addrs, const char * subject, const char * body)
{
    char * 	msg;
    int		size = 0;
    int		line;
    char **	to;
    int isAllASCII;
    char hdr_buf[1024];
    _DtXlateDb db = NULL;
    char plat[_DtPLATFORM_MAX_LEN];
    int execver;
    int compver;
    int body_len;
    char digest[16];
    char mime_type[64];
    char tmpbuf[20];
    char *ret_locale = NULL;
    char *ret_lang = NULL;
    char *ret_codeset = NULL;
    char default_charset[64];
    char *NewBuf = NULL;
    unsigned long _len = 0;
    Encoding enc;

    /* Figure out how big we need the buffer to be. */
    for (to = addrs; *to; to++) {
	size += strlen(*to);
	size += 2; /* Leave room for the , */
    }

    size += strlen(subject);
    size += strlen(body);

    /* We will need space for the header names, a blank line, and
       other general formatting things. We could be exact, but
       1024 is more than enough and give us some spare.
       */
    size += 1024;

    msg = (char *)malloc(size);

    strcpy(msg, "To: ");
    line = 4;

    for (to = addrs; *to; to++) {
	strcat(msg, *to);
	if (*(to + 1) != NULL) {
	    strcat(msg, ", ");
	    line += strlen(*to);
	    if (line > 72) {
		strcat(msg, "\n    ");
		line = 0;
	    }
	}
    }
    strcat(msg, "\nSubject: ");

    /* Encode the body of the message */
    
    /* 1) Open Lcx data bases */

    if ((_DtLcxOpenAllDbs(&db) == 0) &&
        (_DtXlateGetXlateEnv(db,plat,&execver,&compver) != 0))
	{
		_DtLcxCloseDb(&db);
    		strcat(msg, subject);
    		if (msg[strlen(msg) - 1] == '\n') {
			msg[strlen(msg) - 1] = 0;
    		}

    		strcat(msg, "\nMime-Version: 1.0\n");
    		strcat(msg, "Content-Type: text/plain;charset=us-ascii\n\n");
    		strcat(msg, body);
	}
    else
	{

		body_len = strlen(body);

		hdr_buf[0]='\0';
		
		strcpy(mime_type,"text/plain");
		rfc1522cpy(hdr_buf,subject);	
		strcat(hdr_buf,"Mime-Version: 1.0\n");
		isAllASCII= CvtStr((char *)NULL,(void *)body,(unsigned long)body_len,(void**)&NewBuf, &_len, CURRENT_TO_INTERNET);

		enc = getEncodingType(body,body_len,FALSE);
       /*
         * Here is an ugly adjustment again. If mime_type is text/plain and if
         * ret_codeset is ISO-2022-JP/KR/TW/CN, we have to always use
         * enc = MIME_7BIT
         * This means if the user inputs UDC/VDC into the e-mail body,
         * fold7 may convert it to the string with MSB-on character and
         * dtmail passes it to sendmail as if I had all 7bit chars.
         */
		getCharSet(default_charset);
		DtXlateOpToStdLocale(DtLCX_OPER_MIME, default_charset,
					&ret_locale, &ret_lang, &ret_codeset);
            	if ( !strncasecmp( mime_type, "text/plain", 10 ) &&
                 ( !strncasecmp( ret_codeset, "ISO-2022-JP", 11 ) ||
                   !strncasecmp( ret_codeset, "ISO-2022-KR", 11 ) ||
                   !strncasecmp( ret_codeset, "ISO-2022-TW", 11 ) ||
                   !strncasecmp( ret_codeset, "ISO-2022-CN", 11 )   ) ) 
                	enc = MIME_7BIT;

		memset(digest,0,sizeof(digest));
		md5PlainText(body,body_len,digest);
		writeContentHeaders(hdr_buf,mime_type,enc,(char *)digest,isAllASCII);
		strcat(hdr_buf,"\n");
		strcat(hdr_buf,"Content-Length: ");
		if (( NewBuf != NULL) && ( _len != 0))
		{
			sprintf(tmpbuf,"%ld",_len);
			strcat(hdr_buf,tmpbuf);
			strcat(hdr_buf,"\n");
			strcat(msg,hdr_buf);
			strncat(msg,NewBuf,_len);
			strcat(hdr_buf,"\n");
		}
		else
		{
			sprintf(tmpbuf,"%d",body_len);
			strcat(hdr_buf,tmpbuf);
			strcat(hdr_buf,"\n");
			strcat(msg,hdr_buf);
			strcat(msg,body);
		}
	}


    return(msg);
}

static int
deliver(char ** addrs, char * msg)
{
    char ** 	argv;
    int 	fd[2];
    int 	c_pid;
    int 	status;
    int		n_addrs;
    int		cp;

    for (n_addrs = 0; addrs[n_addrs]; n_addrs++) {
	continue;
    }

    argv = (char **)malloc((n_addrs + 2) * sizeof(char *));
    argv[0] = "/usr/lib/sendmail";

    for (cp = 0; addrs[cp]; cp++) {
	argv[cp + 1] = addrs[cp];
    }
    argv[cp + 1] = NULL;

    if(-1 == pipe(fd)) {
	fprintf(stderr, "pipe() failed %d '%s'\n", errno, strerror(errno));
	exit(EXIT_FAILURE);    
    }

    c_pid = fork();
    if (c_pid < 0) {
	return(c_pid);
    }

    if (c_pid == 0) { /* The child. */
	dup2(fd[0], STDIN_FILENO);

	execvp("/usr/lib/sendmail", (char *const *)argv);
	_exit(1); /* This had better never happen! */
    }
    else { /* The parent. */
	if(-1 == write(fd[1], msg, strlen(msg))) {
	    fprintf(stderr, "write() failed %d '%s'\n", errno, strerror(errno));
	    exit(EXIT_FAILURE);    
	}
	close(fd[0]);
	close(fd[1]);

	waitpid(c_pid, &status, 0);
    }

    return(status);
}

int
submit_mail(const char * to,
	    const char * subject,
	    const char * body)
{
    char ** 	addrs;
    char **	ad;
    char *	msg;
    int		status;

    /* 
      Parse the address list so we can form a reasonable one
      for the user to see in the message.
      */
    addrs = arpaPhrase(to);

    msg = formatMessage(addrs, subject, body);

    status = deliver(addrs, msg);

    for (ad = addrs; *ad; ad++) {
	free(*ad);
    }
    free(addrs);

    free(msg);

    return(status);
}
