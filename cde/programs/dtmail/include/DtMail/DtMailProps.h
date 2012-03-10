/*
 *+SNOTICE
 *
 *
 *	$TOG: DtMailProps.h /main/5 1998/02/17 12:33:15 mgreess $
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

#ifndef _DTMAILPROPS_H
#define _DTMAILPROPS_H

/* 
 * The following list of properties represent the abstract names
 * that are understood by the mail library.
 */
#define DtMailMessageTo 	 	"Dt:Mail:Message:To"
#define DtMailMessageSender 	 	"Dt:Mail:Message:Sender"
#define DtMailMessageCc 	 	"Dt:Mail:Message:Cc"
#define DtMailMessageBcc 	 	"Dt:Mail:Message:Bcc"
#define DtMailMessageReceivedTime 	"Dt:Mail:Message:ReceivedTime"
#define DtMailMessageSentTime 		"Dt:Mail:Message:SentTime"
#define DtMailMessageMessageId 	 	"Dt:Mail:Message:MessageId"
#define DtMailMessageSubject 	 	"Dt:Mail:Message:Subject"
#define DtMailMessageContentLength 	"Dt:Mail:Message:ContentLength"
#define DtMailMessageStatus 	 	"Dt:Mail:Message:Status"
#define DtMailMessageDeleteState 	"Dt:Mail:Message:Delete:State"
#define DtMailMessageV3charset      "Dt:Mail:Message:V3charset"
#define DtMailMessageContentType    "Dt:Mail:Message:ContentType"

/*
 * The following 2 headers are used to retrieve the To and CC lines,
 * modified for replying to the message. This may be different than
 * the entire To and CC contents.
 */
#define DtMailMessageToReply		"Dt:Mail:Message:To:Reply"
#define DtMailMessageCcReply		"Dt:Mail:Message:Cc:Reply"

/* The following properties are defined for DtMailQueryImpl use. */
#define DtMailCapabilityPropsSupported 	 "Dt:Mail:Capability:PropsSupported"
#define DtMailCapabilityImplVersion 	 "Dt:Mail:Capability:ImplVersion"
#define DtMailCapabilityInboxName 	 "Dt:Mail:Capability:InboxName"
#define DtMailCapabilityMailspoolName 	 "Dt:Mail:Capability:MailspoolName"
#define DtMailCapabilityTransport 	 "Dt:Mail:Capability:Transport"

#endif
