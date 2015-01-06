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
/* $TOG: message.C /main/9 1998/07/24 16:08:20 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*
 *+SNOTICE
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
 *+ENOTICE
 */

#include "dmx.hh"
// For CHARSET
#include <DtHelp/LocaleXlate.h>
#include <locale.h>
#if !defined(USL) && !defined(__uxp__)
#include <strings.h>
#else 
#include <EUSCompat.h>
#endif 
#include "utils/str_utils.h"


DmxMsg::DmxMsg (void)
{
	// initialize everything
	message = NULL;
	addlInfo = NULL;
	numBPs = 0;
	cachedValues = B_FALSE;
	isCurrent = B_FALSE;
	hasAttachments = B_FALSE;
	isNew = B_FALSE;

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

void
DmxMsg::getFlags (void)
{
	DtMailEnv	env;
	DtMailBoolean	flagState;

	memset (&env, '\0', sizeof (DtMailEnv));
	flagState = DTM_FALSE;

	


	flagState = message->flagIsSet (env, DtMailMessageMultipart);
	if (handleError (env, "msg: multipart?") == B_TRUE)
		exit (1);

	if (flagState == DTM_TRUE)
	{
		hasAttachments = B_TRUE;
	} else {
		hasAttachments = B_FALSE;
	}

	flagState = message->flagIsSet (env, DtMailMessageNew);
	if (handleError (env, "msg: new?") == B_TRUE)
		exit (1);
	
	if (flagState == DTM_TRUE)
	{
		isNew = B_TRUE;
	} else {
		isNew = B_FALSE;
	}

	return;
}
	

	

char *
DmxMsg::printHeader (enum DmxHeaderType htype)
{
	DtMailEnv	env;
	char		*status, *indicator;

	if (isCurrent == B_TRUE)
	{
		indicator = ">";
	} else {
		indicator = " ";
	}	

	if (isNew == B_TRUE)
	{
		status = "N";
	} else {
		status = "O"; // how about "unread" ??
	}

	if (hasAttachments == B_TRUE)
	{
		indicator = "@";
	}

	// the addlInfo string is any extra info needed by the
	// viewer, such as a message number

	char *buffer = new char [1024];
	memset (buffer, 0, 1024);

	switch (htype)
	{
		case MSGLIST:
			sprintf (buffer, "%s%s%s %s",
				indicator, status, addlInfo,
				formatHeader (msgHeader, MSGLIST));
			break;
		case MSGHEADER:
			sprintf (buffer, "%s",
				formatHeader (msgHeader, MSGHEADER));
			break;
		default:
			printf ("error in DmxMsg::display\n");
			break;
	}


	return buffer;
}


void
DmxMsg::display (void)
{
	DtMailEnv			env;
	boolean_t		FirstIsText = B_FALSE;
	DtMail::BodyPart	*firstPart = NULL, *nextpart = NULL;
	char			*type;
	char			*description = NULL;
	char			*sunDataDescription = NULL;
	char			*name = NULL;
	void * contents = NULL;
	unsigned long length = 0;
	int mode = 0;
	char *buf = NULL;
// For CHARSET
	char *mime_cs = NULL, *from_cs = NULL, *to_cs = NULL;
	char *v3_cs = new char [64];
	
	if (cachedValues != B_TRUE)
		parse ();	// read in body part info

	firstPart = bodyParts [0];

	firstPart->getContents(env,
		(const void **) &contents, 
		&length,
		NULL,	//type
		NULL,	//name
		NULL,	//mode
		NULL);	//description

	if (handleError (env, "getContents") == B_TRUE)
		exit (1);

// For CHARSET

   DtMailValueSeq value;
   boolean_t err = B_FALSE;

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
		 err = B_TRUE;
		 env.clear();
		 value.clear();
	  }
   }
// If cannot obtain bodypart's charset header, then maybe this message
// has only one bodypart, then in this case the charset header maybe
// among the message's envelope (main message headers).
// Get the envelope of the message (in order to access the headers)
   DtMail::Envelope *envelope = NULL;
   if (err == B_TRUE) {
	  envelope = message->getEnvelope(env);
	  err = B_FALSE;
#ifdef DEBUG
      env.logError(DTM_FALSE, "DEBUG dtmailpr: Looking at main message header\n");
#endif
   }
   if (envelope != NULL) {
//   Check for MIME charset header and then for V3 charset header
     envelope->getHeader(env, DtMailMessageContentType, DTM_TRUE, value);
     if (env.isNotSet()) {
        mime_cs = firstPart->csFromContentType(value);
     } else {
	    err = B_TRUE;
	    env.clear();
     }
     if (mime_cs == NULL || err == B_TRUE) {
        value.clear();
        envelope->getHeader(env, DtMailMessageV3charset, DTM_TRUE, value);
        if (env.isNotSet()) {
           strcpy(v3_cs, *(value[0]));
        } else {
	  	   err = B_TRUE;
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
	  if (ret)
		 free(ret);
   }

// Get iconv from and to codeset and do conversion.
   int converted = 0;
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
        converted = firstPart->csConvert((char **)&contents, length, 0, from_cs, to_cs);
#ifdef DEBUG
	env.logError(DTM_FALSE, "DEBUG dtmailpr: converted = %d\n", converted);
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


	buf = new char [length + 1];
	memset (buf, 0, (size_t) length + 1);

	// have to "seek" length bytes into the
	// contents buffer
	memmove (buf, contents, (size_t) length);
	buf [length] = '\0';	// null-terminate
				// that puppy

// For CHARSET
    if (converted && contents)
	   free(contents);

	char	*numbuf = new char [10241];
	memset (numbuf, 0, 1024);

#ifdef NEVER
	// Don't want "Message 1:" appearing in print output
	sprintf (numbuf, "Messsage %s:\n%s\n",
		addlInfo, printHeader (MSGHEADER));
#endif
	puts(printHeader(MSGHEADER));
	puts(buf);

	fflush(stdout);

	// No attachments?  We're done.
	if (numBPs < 2)
		return;

	int	i = 0;

	char	*attbuf = NULL;

	printf ("\n");
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
		
		nextpart->getContents(env, NULL, &length, &type,
				&name, &mode, &sunDataDescription);
		if (handleError (env, "getContents") == B_TRUE)
			exit (1);

		if (type == NULL)
			type = "(unknown)";

		if (sunDataDescription == NULL)
		{
			description = "";
		} else {
			// should add bracket or something
			attbuf = new char [strlen (sunDataDescription) +10];
			sprintf (attbuf, " (%s)", sunDataDescription);
			description = attbuf;
		}

		if (name == NULL)
			name = "(name)";

		printf ("[%d] \"%s\"%s, ", i, name, description);
		printf ("%s, %lu bytes\n", type, length);

		if (attbuf != NULL)
			delete [] attbuf;

	}

	delete [] v3_cs;
	return;
}

void
DmxMsg::parse (void)
{
	// store the body parts for later reference

	DtMailEnv			env;
	boolean_t		FirstIsText = B_FALSE;
	DtMail::BodyPart	*part = NULL, *nextpart = NULL;
	char			*type = NULL, *attr = NULL;

	int	bc = message->getBodyCount (env);
	if (handleError (env, "getBodyCount") == B_TRUE)
		exit (1);

	part = message->getFirstBodyPart (env);
	if (handleError (env, "getFirstBodyPart") == B_TRUE)
		exit (1);

	part->getContents (env, NULL, NULL, &type, NULL, NULL, NULL);
	if (handleError (env, "getContents") == B_TRUE)
		exit (1);

	bodyParts = new (DtMail::BodyPart *[bc]);
	cachedValues = B_TRUE;
	
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
			FirstIsText = B_TRUE;
		}
		//free (type);	// it's allocating some data for us
	} else {
		FirstIsText = B_FALSE;
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
		if (handleError (env, "getNextBodyPart") == B_TRUE)
			exit (1);

		if (nextpart == NULL)
			fprintf (stderr, "Error getting part!\n");


		bodyParts [i] = nextpart;
		numBPs++;

		part = nextpart;
	}
}

