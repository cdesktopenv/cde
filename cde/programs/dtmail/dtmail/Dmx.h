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
/* $XConsortium: Dmx.h /main/4 1996/04/21 19:55:42 drk $ */

#ifndef _DMX_HH
#define _DMX_HH

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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <sys/types.h>
#include <ctype.h>

#include <DtMail/DtMail.hh>
#include <DtMail/DtMailError.hh>
#include <DtMail/DtMailValues.hh>
#include <Dt/Dts.h>

extern const char *const	dmxversion;

typedef enum {
    DMX_NONE_STRING,
    DMX_CC_HEADER_STRING,
    DMX_DATE_HEADER_STRING,
    DMX_FROM_HEADER_STRING,
    DMX_SUBJECT_HEADER_STRING,
    DMX_TO_HEADER_STRING,
    DMX_PAGE_NUMBER_STRING,
    DMX_USER_NAME_STRING,
    /*
     * KEEP THIS LAST
     */
    DMX_NUM_STRING_TYPE_ENUM
} DmxStringTypeEnum;

typedef enum
{
    DMX_SEPARATOR_NEW_LINE,
    DMX_SEPARATOR_BLANK_LINE,
    DMX_SEPARATOR_CHARACTER_LINE,
    DMX_SEPARATOR_PAGE_BREAK,
    DMX_SEPARATOR_NEW_JOB,
    /*
     * KEEP THIS LAST
     */
    DMX_NUM_MSG_SEPARATOR_ENUM
} DmxMsgSeparatorEnum;

typedef enum
{
    DMX_PRINT_HEADERS_NONE,
    DMX_PRINT_HEADERS_STANDARD,
    DMX_PRINT_HEADERS_ABBREV,
    DMX_PRINT_HEADERS_ALL,
    /*
     * KEEP THIS LAST
     */
    DMX_NUM_PRINT_HEADERS_ENUM
} DmxPrintHeadersEnum;

typedef enum
{
	DMXCC,
	DMXFROM,
	DMXSUBJ,
	DMXCLENGTH,
	DMXSTATUS,
	DMXDATE,
	DMXTO,
	DMXV3CHARSET,
	DMXCONTENTTYPE,
	DMXNUMHDRS
} DmxHeaders;

// utils
char		*convertValueToString (DtMailValueSeq *, int);
DtMailBoolean	handleError (DtMailEnv &, char *);
char		*getStandardHeaders (DtMailHeaderLine &);
char		*errorString (DmxHeaders);

class DmxMsg
{
    public:

    typedef void (*DmxPrintOutputProc)(XtPointer, char*);

	DmxMsg (void);

	void	display (DmxPrintHeadersEnum, DmxPrintOutputProc, XtPointer);
	void	setHandle (DtMailMessageHandle &);
	void	setHeader (DtMailHeaderLine &);
	void	setMessage (DtMail::Message *);
	void	setInfo (char *);
	void	parse (void);
	char	*getPrintedHeaders (DmxPrintHeadersEnum);
	char	*getHeaders (DtMailBoolean);
	char	*getMessageHeader (DmxHeaders);
	
	
	DtMailMessageHandle	msgHandle;
	DtMailHeaderLine	msgHeader;
	DtMail::Message		*message;

	DtMail::BodyPart	**bodyParts;
	int	numBPs;

	char		*addlInfo;
	DtMailBoolean	cachedValues;
	DtMailBoolean	isNew;
	// other flags for status (read, unopened, etc.)
};


class DmxMailbox
{
private:
    void		createHeaderRequest (DtMailHeaderRequest &);
    void		printMailboxInfo (void);

    static const int	_firstIndex;
    char		*_fileName;
    DtMail::MailBox	*_mbox;
    DmxMsg		_messages[2048];
    DmxMsg		*_message;
    int			_messageCount;
public:
    DmxMailbox (char*);
    ~DmxMailbox (void);

    void		loadMessages (void);
    inline DmxMsg	*firstMessage (void)
			{
			    return (_message = &_messages[_firstIndex]);
			}
    inline DmxMsg	*nextMessage (void)
			{
			    _message++;
			    return ((_message <= &_messages[_messageCount]) ?
				    _message :
				    (DmxMsg*)NULL);
			}
    inline int		numMessages (void) { return _messageCount; }

};



#endif // _DMX_HH
