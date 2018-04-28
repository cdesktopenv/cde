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
 *	$TOG: DtMailTypes.h /main/9 1998/11/10 17:02:58 mgreess $
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

#ifndef _DTMAILTYPES_H
#define _DTMAILTYPES_H

#include <DtMail/DtMailValues.hh>

//
// Default file open mode when creating a new mail file (i.e. folder),
// or opening an existing one.
// NOTE: there are two create modes, one for use if mail groups are not
// required, and the other for use if mail groups are required. This
// decision is made in the creating code

#define DTMAIL_DEFAULT_CREATE_MODE      0600
#define DTMAIL_DEFAULT_CREATE_MODE_MAILGROUP	0660
#define DTMAIL_DEFAULT_CREATE_UMASK	0177
#define DTMAIL_DEFAULT_CREATE_UMASK_MAILGROUP	0117
#define DTMAIL_DEFAULT_CREATE_MAILGROUP	"mail"
#define DTMAIL_DEFAULT_OPEN_MODE        O_RDWR


typedef void * DtMailSession;
typedef void * DtMailMailBox;
typedef void * DtMailMessage;
typedef void * DtMailEnvelope;
typedef void * DtMailBodyPart;
typedef void * DtMailMessageHandle;
typedef void * DtMailHeaderHandle;
typedef void * DtMailTransport;
typedef unsigned long DtMailObjectKey;

typedef enum { DTM_FALSE, DTM_TRUE } DtMailBoolean;

/*
 * The callback ops are passed back to the client when the DtMail library
 * needs information from the user. Additional operations can be defined by
 * specific implementations. These should begin after the DTMC_RESERVED constant.
 */
typedef enum {
    DTMC_NOOP			=	0,
    DTMC_SAVE			=	1,
    DTMC_REVERT			=	2,
    DTMC_RAISE			=	3,
    DTMC_PASSWD			=	4,
    DTMC_NEWMAIL		=	5,
    DTMC_DELETEMSG		=	6,
    DTMC_BADSTATE		=	7,
    DTMC_UNLOCK			=	8,
    DTMC_LOSTLOCK		=	9,
    DTMC_QUERYLOCK		=	10,
    DTMC_READONLY		=	11,
    DTMC_READWRITEOVERRIDE	=	12,
    DTMC_DOTDTMAILLOCKFAILED	=	13,
    DTMC_INODECHANGED		=	14,
    DTMC_ACCESSFAILED		=	15,
    DTMC_SERVERACCESSFAILED	=	16,
    DTMC_SERVERPASSWORDNEEDED	=	17,
    DTMC_GETMAILCOMMANDFAILED	=	18,
    DTMC_SERVERACCESSINFO	=	19,
    DTMC_SERVERACCESSINFOERROR	=	20,
    DTMC_RESERVED 		=	8192
} DtMailCallbackOp;

typedef DtMailBoolean (*DtMailCallback)(DtMailCallbackOp op, 
					const char * path,
					const char * prompt_hint,
					void * client_data,
					...);

// This forward declaration is needed to resolve a circular dependency
// between DtMailTypes.h and DtMailError.hh
//
class DtMailEnv;

typedef void * DtMailOperationId;
typedef void (*DtMailStatusCallback)(DtMailOperationId,
				     DtMailEnv & error,
				     void * client_data);

typedef DtMailBoolean (*DtMailEventFunc)(void * client_data);

typedef struct {
    char **		header_name;
    int			number_of_names;
} DtMailHeaderRequest;

typedef struct {
    DtMailValueSeq	*header_values;
    int			number_of_names;
} DtMailHeaderLine;

typedef enum {
    DtMailMessageNew		=	1,
    DtMailMessageDeletePending	=	2,
    DtMailMessageAnswered	=	3,
    DtMailMessageMultipart	=	4,
    DtMailMessagePartial	=	8,
    DtMailMessageReservedLimit	=	8192
} DtMailMessageState;

typedef enum {
    DtMailBodyPartDeletePending	=	1,
    DtMailBodyPartReservedLimit	=	8192
} DtMailBodyPartState;

typedef enum {
    DtMailCheckUnknown		=	0,
    DtMailCheckGood		=	1,
    DtMailCheckBad		=	2
} DtMailChecksumState;

typedef enum {
    DtMailBusyState_NotBusy	=	0,
    DtMailBusyState_AutoSave	=	1,
    DtMailBusyState_NewMail	=	2
} DtMailBusyState;

typedef enum {
    DtMailFileObject		=	1,
    DtMailBufferObject		=	2,
    DtMailReservedObject	=	8192
} DtMailObjectSpace;

typedef struct {
    void *		buffer;
    unsigned long	size;
} DtMailBuffer;

/* The event packet is written to the event file descriptor.
 * This structure is only useful to programs dealing directly with
 * the DtMail library events.
 */

typedef enum {
    DTM_TARGET_MAILBOX,
    DTM_TARGET_TRANSPORT
} DtMailCallbackTarget;

typedef struct {
    DtMailObjectKey		key;
    DtMailCallbackTarget	target;
    void *			target_object;
    void *			operation;
    void *			argument;
    time_t			event_time;
} DtMailEventPacket;

typedef enum {
    DtMailLockRead	=	1,
    DtMailLockWrite	=	2
} DtMailLock;

#endif
