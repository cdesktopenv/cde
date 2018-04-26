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
 *  $TOG: MailMsg.h /main/5 1998/04/22 14:17:19 mgreess $
 *
 *  RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *  The information in this document is subject to special
 *  restrictions in a confidential disclosure agreement between
 *  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *  document outside HP, IBM, Sun, USL, SCO, or Univel without
 *  Sun's specific written approval.  This document and all copies
 *  and derivative works thereof must be returned or destroyed at
 *  Sun's request.
 *
 *  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef MAILMSG_H
#define MAILMSG_H

#include <nl_types.h>
#include <Dt/MsgCatP.h>

/*
 * DtMail comment tag is NL_COMMENT (NL = National Language).
 * genmsg will extract comment blocks containing NL_COMMENT.
 */

extern nl_catd DT_catd;   /* Catgets file descriptor */

#define DTMAIL_CAT        "DtMail"
#define NL_SET            1
#define BUTTON_SET        1
#define TITLE_SET         1
#define LABEL_SET         1
#define DIALOG_SET        3
#define MSG_SET           3
#define ERR_SET           2

#ifdef XGETTEXT
#define MAILMSG(msgid, str)   dgettext(NL_SET, msgid, str)
#else
#define MAILMSG(msgid, str)   catgets(DT_catd, NL_SET, msgid, str)
#endif

#ifdef hpV4
#define GETMSG(DT_catd, NL_SET, msgid, str) _DtCatgetsCached(DT_catd, NL_SET, msgid, str)
#else
#define GETMSG(DT_catd, NL_SET, msgid, str) catgets(DT_catd, NL_SET, msgid, str)
#endif


/*  MailBox.C             msgid  100 - 199
 *  MBOX_*
 */

/*  MsgScrollingList.C    msgid  200 - 299
 *  MSGLIST_*
 */

/*  RoamCmds.C            msgid  300 - 399
 *  ROCMD_*
 */

/*  RoamMenuWindow.C      msgid  400 - 499
 *  ROMENU_*
 */

/*  SendMsgDialog.C       msgid  500 - 599
 *  SEND_*
 */

/*  Undelete.C            msgid  600 - 699
 *  UNDEL_*
 */

#endif  // MAILMSG_H
