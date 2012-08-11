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
 *	$XConsortium: DtMail.h /main/4 1996/04/21 19:44:43 drk $
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

#ifndef _DTMAIL_H
#define _DTMAIL_H

#include <Tt/tttk.h>
#include "DtMailTypes.h"
#include "DtMailProps.h"
#include "DtMailError.hh"
#include "DtMailValues.hh"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)
    /* Mail Session and Administration API */
    /* API spec sections 3.2.1 - 3.2.8 */

    extern DtMailSession DtMailCreateSession(DtMailEnv *,
				const char *);

    extern void DtMailCloseSession(DtMailEnv *,
				DtMailSession,
				DtMailBoolean);

    extern int DtMailGetMinorCode(DtMailEnv *);

    extern const char * DtMailGetErrorString(DtMailEnv *);

    extern const char ** DtMailEnumerateImpls(DtMailSession,
					DtMailEnv *);
    extern void DtMailSetDefaultImpl(DtMailSession,
					DtMailEnv *,
					const char *);
    extern const char * DtMailGetDefaultImpl(DtMailSession,
					DtMailEnv *);
    extern void DtMailQueryImpl(DtMailSession,
					DtMailEnv *,
					const char *,
					const char *,
					...);

    
    /* MailBox methods. */
    /* API spec sections 3.2.9 - 3.2.20 */

    extern DtMailMailBox DtMailMailBoxConstruct(DtMailSession,
						DtMailEnv *,
						DtMailObjectSpace,
						void *,
						DtMailCallback,
						void *);

    extern void DtMailMailBoxDestruct(DtMailMailBox);

    extern void DtMailMailBoxCreate(DtMailMailBox,
					DtMailEnv *);

    extern void DtMailMailBoxOpen(DtMailMailBox,
					DtMailEnv *,
					DtMailBoolean);

    extern DtMailMessageHandle DtMailMailBoxGetFirstMessageSummary (DtMailMailBox,
					DtMailEnv *,
					const DtMailHeaderRequest *,
					DtMailHeaderLine **);

    extern DtMailMessageHandle DtMailMailBoxGetNextMessageSummary (DtMailMailBox,
					DtMailEnv *,
					DtMailMessageHandle,
					const DtMailHeaderRequest *,
					DtMailHeaderLine **);

    extern DtMailMessage DtMailMailBoxGetMessage(DtMailMailBox,
					DtMailEnv *
					DtMailMessageHandle);

    extern DtMailMessage DtMailMailBoxGetFirstMessage(DtMailMailBox,
					DtMailEnv *);

    extern DtMailMessage DtMailMailBoxGetNextMessage(DtMailMailBox,
					DtMailEnv *,
					DtMailMessage);

    extern DtMailMessage DtMailMailBoxNewMessage(DtMailMailBox,
					DtMailEnv *);

    extern const char * DtMailMailBoxImpl(DtMailMailBox,
					DtMailEnv *);

    /* Header API Methods */
    /* API spec sections 3.2.21 - 3.2.23 */

    extern DtMailHeaderRequest * DtMailHeaderRequestCreate(DtMailEnv *,
					const int);

    extern void DtMailHeaderRequestDestroy(DtMailHeaderRequest *,
					DtMailEnv *);

    extern void DtMailHeaderLineDestroy (DtMailHeaderLine *,
					DtMailEnv *);


    /* Message methods. */
    /* API spec sections 3.2.24 - 3.2.35 */

    extern DtMailMessage DtMailMessageConstruct(DtMailEnv *,
						DtMailSession,
						const char *,
						DtMailCallback,
						void *);

    extern void DtMailMessageCreate(DtMailMessage,
					DtMailEnv *);

    extern void DtMailMessageOpen(DtMailMessage,
					DtMailEnv *
					DtMailBoolean);

    extern void DtMailMessageDestruct(DtMailMessage);

    extern DtMailEnvelope DtMailMessageGetEnvelope(DtMailMessage,
							DtMailEnv *);

    extern int DtMailMessageGetBodyCount(DtMailMessage,
						DtMailEnv *);

    extern DtMailBodyPart DtMailMessageGetFirstBodyPart(DtMailMessage,
						DtMailEnv *);

    extern DtMailBodyPart DtMailMessageGetNextBodyPart(DtMailMessage, 
							 DtMailEnv *, 
							 DtMailBodyPart);
    
    extern DtMailBodyPart DtMailMessageNewBodyPart(DtMailMessage,
							DtMailEnv *,
							DtMailBodyPart);

    extern void DtMailMessageNewBodyPartOrder (DtMailMessage,
						DtMailEnv *,
						DtMailBodyPart *,
						int);

// TOGO	    extern CMContainer DtMailMessageContainer(DtMailMessage,
// TOGO							DtMailEnv *);

    extern const char * DtMailMessageImpl(DtMailMessage,
						DtMailEnv *); 

    /* Envelope Methods */
    /* API spec sections 3.2.36 - 3.2.41 */

    extern DtMailHeaderHandle DtMailEnvelopeGetFirstHeader(DtMailEnvelope, 
							   DtMailEnv *, 
							   char **, 
							   DtMailValueSeq *);

    extern DtMailHeaderHandle DtMailEnvelopeGetNextHeader(DtMailEnvelope, 
							  DtMailEnv *, 
							  DtMailHeaderHandle,
							  char **, 
							  DtMailValueSeq *);
    
    extern void DtMailEnvelopeGetHeader(DtMailEnvelope, 
					DtMailEnv *,
					const char *,
					const DtMailBoolean,
					DtMailValueSeq *);

    extern void DtMailEnvelopeSetHeaderSeq(DtMailEnvelope, 
					   DtMailEnv *,
					   const char *, 
					   const DtMailValueSeq *);

    extern void DtMailEnvelopeSetHeader(DtMailEnvelope, 
					DtMailEnv *,
					const char *, 
					const DtMailValueSeq *,
					DtMailBoolean);

    extern void DtMailEnvelopeDestruct(DtMailEnvelope);


    /* BodyPart methods. */
    /* API spec sections 3.2.42 - 3.2.49 */

    extern DtMailHeaderHandle DtMailBodyPartGetFirstHeader(DtMailBodyPart,
						DtMailEnv *,
						char **,
						DtMailValueSeq **);

    extern DtMailHeaderHandle DtMailBodyPartGetNextHeader(DtMailBodyPart,
						DtMailEnv *,
						DtMailHeaderHandle,
						char **,
						DtMailValueSeq **);

    extern void DtMailBodyPartGetHeader(DtMailBodyPart,
						DtMailEnv *,
						const char *,
						const DtMailBoolean,
						DtMailValueSeq *);

    extern void DtMailBodyPartSetHeaderSeq(DtMailBodyPart,
						DtMailEnv *,
						const char *,
						const DtMailValueSeq *);

    extern void DtMailBodyPartSetHeader(DtMailBodyPart,
						DtMailEnv *,
						const DtMailValueSeq *,
						DtMailBoolean);

    extern void DtMailBodyPartGetContents(DtMailBodyPart,
					DtMailEnv *, 
					void **, 
					unsigned long *,
					char **,
					char **,
					int *,
					char *);

    extern void DtMailBodyPartSetContents(DtMailBodyPart,
					DtMailEnv *,
				 	const void *, 
					const unsigned long,
					const char *,
					const char *,
					const int,
					const char *);

    extern void DtMailBodyPartDestruct(DtMailBodyPart);


    /* Transport Methods. */
    /* API spec sections 3.2.57 - 3.2.59 */

    extern DtMailTransport DtMailTranportConstruct(DtMailSession,
					DtMailEnv *,
					const char *);

    extern const char ** DtMailTransportEnumerate(DtMailSession,
					DtMailEnv *);

    extern int DtMailTransportSubmit(DtMailTransport,
					DtMailEnv *,
					DtMailMessage);

#else /* __STDC__ */


#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif
