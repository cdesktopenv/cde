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
/* $TOG: DmxMessage.C /main/6 1998/07/24 16:18:17 mgreess $ */

/*
 *+SNOTICE
 *
 *	$:$
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
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */

#include "Dmx.h"
// For CHARSET
#include <LocaleXlate.h>
#include <locale.h>
#include <strings.h>

#include "Application.h"
#include "DtMail/DtMail.hh"
#include "DtMailTypes.h"
#include "MailSession.hh"
#include "RoamApp.h"
#include "str_utils.h"


DmxMsg::DmxMsg (void)
{
	// initialize everything
	message = NULL;
	addlInfo = NULL;
	numBPs = 0;
	cachedValues = DTM_FALSE;
	msgHandle = NULL;
	msgHeader.header_values = 0;
	msgHeader.number_of_names = 0;
	bodyParts = NULL;
	isNew = DTM_FALSE;

	return;
}

void
DmxMsg::setHandle (DtMailMessageHandle &h)
{
	msgHandle = h;
	return;
}

void
DmxMsg::setHeader (DtMailHeaderLine &h)
{
	msgHeader = h;
	return;
}

void
DmxMsg::setMessage (DtMail::Message *m)
{
	message = m;
	return;
}

void
DmxMsg::setInfo (char *info)
{
	addlInfo = strdup (info);
	return;
}

char *
DmxMsg::getMessageHeader (DmxHeaders which)
{
	int		i = 0, length = 0;
	int		buflength = 0;
	char		*rtn;
	const char	*str;

	if (which >= DMXNUMHDRS)
	  return (char *) NULL;

	length = msgHeader.header_values[which].length ();
	if (length == 0)
	    str = errorString(which);
	else
	    str = *((msgHeader.header_values[which])[0]);

	//need to free this after using it
	rtn = strdup(str);
    	return (rtn);
}
	

	

char *
DmxMsg::getHeaders (DtMailBoolean abbreviated_only)
{
    DtMailEnv		error;
    DtMail::Session	*m_session = theRoamApp.session()->session(); 
    DtMail::MailRc	*mail_rc = m_session->mailRc(error);
    DtMail::Envelope	*env = message->getEnvelope(error);

    DtMailHeaderHandle	hdr_hnd;
    char		*name;
    DtMailValueSeq	value;

    // Code from MsgScrollingList - display_message().
    // We're trying to reduce heap size by not allocating and 
    // deleting space in every loop iteration.  So just have a 
    // fixed size buffer initially.
    // 

    // Initial line size.  When not enough, allocate more.
    int			buffer_size = 2048;   
    char		*buffer = new char [buffer_size];
    int			count = 0;
    int			hdr_num = 0;
    char		*newline = "\n";
    char		*separator = ": ";
    int			val = 0;

    //
    // Iterate through each header in the message and add it
    // to the buffer.
    //
    for (hdr_hnd = env->getFirstHeader(error, &name, value), *buffer = '\0';
	 hdr_hnd && !error.isSet();
	 hdr_hnd = env->getNextHeader(error, hdr_hnd, &name, value), hdr_num++)
    {
        if (abbreviated_only == DTM_TRUE &&
	    (hdr_num != 0 || strcmp(name, "From") != 0))
	{
	    DtMailEnv ierror;
	    if (mail_rc->ignore(ierror, name))
	    {
		free(name);
		value.clear();
		continue;
	    }
	}
	    
	for (val=0;  val<value.length();  val++)
	  count += strlen(name) +
		   strlen(*(value[val])) +
		   strlen(separator) +
		   strlen(newline) + 1;

	if (count > buffer_size)
	{
	    // Need to increase buffer size.
	    char	*new_buffer;
	    
	    buffer_size *= 2;
	    new_buffer = new char [buffer_size];
	    memset(new_buffer, 0, buffer_size);

	    strcpy(new_buffer, buffer);
	    delete [] buffer;
	    buffer = new_buffer;
	}

	for (val=0;  val<value.length();  val++)
	{
	    strcat(buffer, name);
		
	    if (hdr_num != 0 || strcmp(name, "From") != 0)
	      strcat(buffer, separator);
	    else
	      strcat(buffer, " ");
		
	    strcat(buffer, *(value[val]));
	    strcat(buffer, newline);
	}
	value.clear();
	free(name);
    }

    //
    // Need to free this after using;
    //
    return buffer;
}
	

	

char *
DmxMsg::getPrintedHeaders (DmxPrintHeadersEnum header_format)
{
    char *newline = "\n";
    char *buffer = NULL;

    switch (header_format)
    {
	case DMX_PRINT_HEADERS_NONE:
	    buffer = new char [strlen(newline) + 1];
	    strcpy(buffer, (const char *) newline);
	    break;
	case DMX_PRINT_HEADERS_STANDARD:
	    buffer = getStandardHeaders(msgHeader);
	    break;
	case DMX_PRINT_HEADERS_ABBREV:
	    buffer = getHeaders(DTM_TRUE);
	    break;
	case DMX_PRINT_HEADERS_ALL:
	    buffer = getHeaders(DTM_FALSE);
	    break;
	default:
	    fprintf (stderr, "error in DmxMsg::display\n");
	    break;
    }

    //
    // Need to free this after using;
    //
    return buffer;
}


void
DmxMsg::display (
		DmxPrintHeadersEnum header_format,
		DmxPrintOutputProc print_proc,
		XtPointer stream)
{
    DtMailEnv		env;
    DtMailBoolean	FirstIsText = DTM_FALSE;
    DtMail::BodyPart	*firstPart = NULL, *nextpart = NULL;
    char		*buf = NULL,
    			*description = NULL,
    			*name = NULL,
    			*newline = NULL,
    			*sunDataDescription = NULL,
			*type = NULL;

    void		*contents = NULL;
    unsigned long	length = 0;
    int			mode = 0;

    // For CHARSET
    char		v3_cs[64],
			*mime_cs = NULL,
			*from_cs = NULL,
			*to_cs = NULL;

	
    // read in body part info
    if (cachedValues != DTM_TRUE)
      parse ();

    firstPart = bodyParts [0];

    firstPart->getContents(env,
		(const void **) &contents, 
		&length,
		NULL,	//type
		NULL,	//name
		NULL,	//mode
		NULL);	//description

    if (handleError(env, "getContents") == DTM_TRUE)
      exit (1);

    // For CHARSET
    DtMailValueSeq	value;
    DtMailBoolean	err = DTM_FALSE;

    // Get the bodypart's charset - Try MIME first then V3
    firstPart->getHeader(env, DtMailMessageContentType, DTM_TRUE, value);
    if (env.isNotSet()) {
	mime_cs = firstPart->csFromContentType(value);
    } else {
	env.clear();
	value.clear();
	firstPart->getHeader(env, DtMailMessageV3charset, DTM_TRUE, value);
	if (env.isNotSet()) {
	    strcpy(v3_cs, *(value[0]));
	} else {
	     err = DTM_TRUE;
	     env.clear();
	     value.clear();
	}
    }

    // If cannot obtain bodypart's charset header, then maybe this message
    // has only one bodypart, then in this case the charset header maybe
    // among the message's envelope (main message headers).
    // Get the envelope of the message (in order to access the headers)
    DtMail::Envelope	*envelope = NULL;
    if (err == DTM_TRUE) {
	envelope = message->getEnvelope(env);
	err = DTM_FALSE;
#ifdef DEBUG
        env.logError(
		DTM_FALSE,
		"DEBUG dtmailpr: Looking at main message header\n");
#endif
    }

    //   Check for MIME charset header and then for V3 charset header
    if (envelope != NULL) {
        envelope->getHeader(env, DtMailMessageContentType, DTM_TRUE, value);
        if (env.isNotSet()) {
            mime_cs = firstPart->csFromContentType(value);
        } else {
	    err = DTM_TRUE;
	    env.clear();
        }
        if (mime_cs == NULL || err == DTM_TRUE) {
            value.clear();
            envelope->getHeader(env, DtMailMessageV3charset, DTM_TRUE, value);
            if (env.isNotSet()) {
                strcpy(v3_cs, *(value[0]));
            } else {
	  	err = DTM_TRUE;
                env.clear();
            }
        }
    } else {
#ifdef DEBUG
	env.logError(DTM_FALSE, "DEBUG dtmailpr: envelope is null\n");
#endif
	env.clear();
    }

    // Default codeset in case mime_cs and v3_cs are both null.
    if ((mime_cs == NULL) && (strlen(v3_cs) == 0)) {
	char *ret = NULL;
	firstPart->DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
					setlocale(LC_CTYPE, NULL),
		 			NULL,
		 			NULL,
		 			&ret);
	strcpy(v3_cs, "DEFAULT");
	strcat(v3_cs, ".");
	strcat(v3_cs, ret);
	free(ret);
    }

    // Get iconv from and to codeset and do conversion.
    int	converted = 0;
    if (mime_cs) {
        from_cs = firstPart->csToConvName(mime_cs);
#ifdef DEBUG
        env.logError(DTM_FALSE, "DEBUG dtmailpr: mime_cs = %s\n", mime_cs);
#endif
    } else {
       from_cs = firstPart->csToConvName(v3_cs);
#ifdef DEBUG
       env.logError(DTM_FALSE, "DEBUG dtmailpr: v3_cs = %s\n", v3_cs);
#endif
    }
    to_cs = firstPart->locToConvName();

#ifdef DEBUG
    if ( from_cs == NULL )
      env.logError(DTM_FALSE, "DEBUG dtmailpr: from_cs is NULL\n");
    else
      env.logError(DTM_FALSE, "DEBUG dtmailpr: from_cs = %s\n", from_cs);

    if ( to_cs == NULL )
      env.logError(DTM_FALSE, "DEBUG dtmailpr: to_cs is NULL\n");
    else
      env.logError(DTM_FALSE, "DEBUG dtmailpr: to_cs = %s\n", to_cs);
#endif

    if ( from_cs && to_cs ) {
        if ( strcasecmp(from_cs, to_cs) != 0 ) {
            converted = firstPart->csConvert(
					(char **)&contents,
					length,
					0,
					from_cs,
					to_cs);
#ifdef DEBUG
	    env.logError(DTM_FALSE,
			 "DEBUG dtmailpr: converted = %d\n", converted);
#endif
        }
    }
    if ( mime_cs )
      free ( mime_cs );
    if ( from_cs )
      free( from_cs );
    if ( to_cs )
      free ( to_cs );
 
    // End of For CHARSET

    newline = new char [2];
    newline[0] = '\n';
    newline[1] = '\0';

    //
    // Print out the message headers.
    //
    buf = getPrintedHeaders(header_format);
    print_proc(stream, buf);
    print_proc(stream, newline);
    delete buf;

    //
    // Print out the message body.
    //
    buf = new char [length + 1];
    memset (buf, 0, (unsigned int) length + 1);
    memmove (buf, contents, (unsigned int) length);
    buf [length] = '\0';	// null-terminate that puppy
    print_proc(stream, buf);
    print_proc(stream, newline);
    delete [] buf;

    // For CHARSET
    if (converted && contents)
      free(contents);

    // No attachments?  We're done.
    if (numBPs < 2)
	return;

    int		i = 0, attbuflen = 0;
    char	*attbuf = NULL;
    char	*sunbuf = NULL;

    print_proc(stream, newline);
    for (i = 1; i < numBPs ; i++)
    {
	nextpart = bodyParts [i];

	if (nextpart == NULL)
	  fprintf (stderr, "Error getting part!\n");

	length = 0;
	type = "";
	sunDataDescription = "";
	description = "";
	name = "";
	mode = -1;
		
	nextpart->getContents(env,
			NULL,
			&length,
			&type,
			&name,
			&mode,
			&sunDataDescription);
	if (handleError (env, "getContents") == DTM_TRUE)
	  exit (1);

	if (type == NULL)
	  type = "(type unknown)";

	if (name == NULL)
	  name = "(name unknown)";

	if (sunDataDescription == NULL)
	{
	    description = "";
	} else {
	    // should add bracket or something
	    sunbuf = new char [strlen (sunDataDescription) + 10];
	    sprintf(sunbuf, " (%s)", sunDataDescription);
	    description = sunbuf;
	}

	attbuflen = strlen(name) + strlen(type) + strlen(description);
	attbuf = new char [attbuflen + 64];
	sprintf(attbuf,
		"[%d] \"%s\"%s, %s, %ld bytes",
		i,
		name,
		description,
		type,
		length);
	print_proc(stream, attbuf);
	print_proc(stream, newline);
	delete [] attbuf;

	if (sunbuf != NULL)
	  delete [] sunbuf;
    }

    return;
}

void
DmxMsg::parse (void)
{
	// store the body parts for later reference

	DtMailEnv			env;
	DtMailBoolean		FirstIsText = DTM_FALSE;
	DtMail::BodyPart	*part = NULL, *nextpart = NULL;
	char			*type = NULL, *attr = NULL;

	int	bc = message->getBodyCount (env);
	if (handleError (env, "getBodyCount") == DTM_TRUE)
		exit (1);

	part = message->getFirstBodyPart (env);
	if (handleError (env, "getFirstBodyPart") == DTM_TRUE)
		exit (1);

	part->getContents (env, NULL, NULL, &type, NULL, NULL, NULL);
	if (handleError (env, "getContents") == DTM_TRUE)
		exit (1);

	bodyParts = new (DtMail::BodyPart *[bc]);
	cachedValues = DTM_TRUE;
	
	// cache values
	bodyParts [0] = part;
	numBPs++;


	if (type != NULL)
	{
		attr = DtDtsDataTypeToAttributeValue (type,
						DtDTS_DA_IS_TEXT,
						NULL);
		if (attr != NULL)
		{
			FirstIsText = DTM_TRUE;
		}
		//free (type);	// it's allocating some data for us
	} else {
		FirstIsText = DTM_FALSE;
	}

	// No attachments?  We're done.
	if (bc < 2)
		return;

	int	i;

	for (i = 1; i < bc; i++)
	{
		nextpart = NULL;
		nextpart = message->getNextBodyPart (env,
						part);
		if (handleError (env, "getNextBodyPart") == DTM_TRUE)
			exit (1);

		if (nextpart == NULL)
			fprintf (stderr, "Error getting part!\n");


		bodyParts [i] = nextpart;
		numBPs++;

		part = nextpart;
	}
}

