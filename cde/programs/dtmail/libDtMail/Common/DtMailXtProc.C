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
 *	$XConsortium: DtMailXtProc.C /main/4 1996/04/21 19:47:56 drk $
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

#include <stdio.h>
#include <unistd.h>

#include <DtMail/DtMail.hh>
#include <DtMail/DtMailXtProc.h>

extern "C" void
DtMailXtInputProc(XtPointer client_data,
		  int * file_desc,
		  XtInputId *)
{
    DtMail::Session *session = (DtMail::Session *)client_data;

    // We read one event packet. The call back caller is then
    // invoked to fire the appropriate callback.
    //
    DtMailEventPacket	packet;

    read(*file_desc, &packet, sizeof(DtMailEventPacket));

    if (session->validObjectKey(packet.key) == DTM_FALSE) {
	return;
    }

    switch(packet.target) {
      case DTM_TARGET_MAILBOX:
      {
	DtMail::MailBox * self = (DtMail::MailBox *)packet.target_object;
	
	self->callCallback((DtMailCallbackOp)((long)packet.operation), 
			   packet.argument);
	break;
      }

      case DTM_TARGET_TRANSPORT:
      {
	DtMail::Transport * self = (DtMail::Transport *)packet.target_object;
	self->callCallback((DtMailOperationId)packet.operation,
			   packet.argument);
	break;
      }

      default:
	break;
    }

    return;
}

void
DtMailProcessClientEvents(void)
{
#if 0
//    if (DtMailDamageContext) {
//	XtInputMask events = XtAppPending(DtMailDamageContext);
//	if (events & (XtIMXEvent | XtIMAlternateInput)) {
//	    XtAppProcessEvent(DtMailDamageContext, XtIMXEvent | XtIMAlternateInput);
//	}
//    }
#endif
}
