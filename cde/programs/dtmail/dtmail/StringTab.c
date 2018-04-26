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
 *	$XConsortium: StringTab.c /main/3 1995/11/06 16:15:56 rswiston $
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
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <X11/Intrinsic.h>

String ApplicationFallbacks[] = {
"Dtmail*interval:	5",
"Dtmail*Next:		True",
"Dtmail*Next*default:	True",
"Dtmail*PrintScript: lp -dpubs3 ",
"Dtmail*From*label:	From:",
"Dtmail*Text*label:	Text:",
"Dtmail*Start*default:	True",
"Dtmail*Message_List*scrollBarDisplayPolicy:	STATIC",
"Dtmail*Message_List*visibleItemCount:	15",
"Dtmail*Message_List*width:	600",
"Dtmail*Message_List*listSizePolicy:	VARIABLE",
"Dtmail*Message_List*viewHeight:	7",
"Dtmail*Message_List*doubleClickInterval: 400",
"Dtmail*Message_List*selectionPolicy:      EXTENDED_SELECT",
"Dtmail*Work_Area*Text*editMode:	MULTI_LINE_EDIT",
"Dtmail*find*To.columns: 1",
"Dtmail*find*From.columns: 1",
"Dtmail*find*Subject.columns: 1",
"Dtmail*find*Cc.columns: 1",
"Dtmail*ComposeDialog*HeaderArea*form_*columns: 70",
"Dtmail*ComposeDialog*menubar*File.mnemonic: F",
"Dtmail*ComposeDialog*menubar*File.Include.mnemonic: I",
"Dtmail*ComposeDialog*menubar*File.Log_Message.mnemonic: L",
"Dtmail*ComposeDialog*menubar*File.Log_Message.indicatorType: XmONE_OF_MANY",
"Dtmail*ComposeDialog*menubar*File.Log_Message.set: TRUE",
"Dtmail*ComposeDialog*menubar*File.Log_Message.visibleWhenOff: TRUE",
"Dtmail*menubar*Send.mnemonic: S",
"Dtmail*menubar*Send.accelerator: Ctrl<Key>S",
"Dtmail*Send.acceleratorText: Ctrl+S",
"Dtmail*ComposeDialog*menubar*File.Send_As.mnemonic: n",
"Dtmail*ComposeDialog*menubar*File.Close.mnemonic: C",
"Dtmail*ComposeDialog*menubar*File.Close.accelerator: Alt<Key>F4",
"Dtmail*ComposeDialog*menubar*File.Close.acceleratorText: Alt+F4",
"Dtmail*ComposeDialog*Undo.mnemonic: U",
"Dtmail*ComposeDialog*menubar*Edit.Undo.accelerator: Ctrl<Key>Z",
"Dtmail*ComposeDialog*menubar*Edit.Undo.acceleratorText: Ctrl+Z",
"Dtmail*ComposeDialog*Cut.mnemonic: t",
"Dtmail*ComposeDialog*menubar*Edit.Cut.accelerator: Ctrl<Key>X",
"Dtmail*ComposeDialog*menubar*Edit.Cut.acceleratorText: Ctrl+X",
"Dtmail*Copy.mnemonic: C",
"Dtmail*ComposeDialog*menubar*Edit.Copy.accelerator: Ctrl<Key>C",
"Dtmail*ComposeDialog*menubar*Edit.Copy.acceleratorText: Crtl+C",
"Dtmail*ComposeDialog*Paste.mnemonic: P",
"Dtmail*ComposeDialog*menubar*Edit.Paste.accelerator: Ctrl<Key>V",
"Dtmail*ComposeDialog*menubar*Edit.Paste.acceleratorText: Crtl+V",
"Dtmail*ComposeDialog*Paste_Special.mnemonic: l",
"Dtmail*ComposeDialog*Paste_Special.Bracketed.accelerator: Ctrl<Key>B",
"Dtmail*ComposeDialog*Paste_Special.Bracketed.acceleratorText: Ctrl+B",
"Dtmail*ComposeDialog*Paste_Special.Indented.accelerator: Ctrl<Key>I",
"Dtmail*ComposeDialog*Paste_Special.Indented.acceleratorText: Ctrl+I",
"Dtmail*ComposeDialog*Clear.mnemonic: e",
"Dtmail*menubar*Delete.mnemonic: D",
"Dtmail*RoamMsgsPopup*Delete.mnemonic: D",
"Dtmail*ComposeDialog*menubar*Edit.Delete.accelerator: Delete",
"Dtmail*ComposeDialog*menubar*Edit.Delete.acceleratorText: Delete",
"Dtmail*Select_All.mnemonic: S",
"Dtmail*ComposeDialog*menubar*Edit.Find/Change.mnemonic: F",
"Dtmail*ComposeDialog*menubar*Edit.Find/Change.accelerator: Ctrl<Key>F",
"Dtmail*ComposeDialog*menubar*Edit.Find/Change.acceleratorText: Ctrl+F",
"Dtmail*ComposeDialog*menubar*Edit.Check_Spelling.mnemonic: k",
"Dtmail*ComposeDialog*Add_File.mnemonic: F",
"Dtmail*Save_As.mnemonic: A",
"Dtmail*ComposeDialog*Undelete.mnemonic: U",
"Dtmail*ComposeDialog*menubar*Attachments.Rename.mnemonic: R",
"Dtmail*ComposeDialog*menubar*Attachments.Show_List.mnemonic: L",
"Dtmail*ComposeDialog*menubar*Attachments.Show_List.indicatorType: XmONE_OF_MANY",
"Dtmail*ComposeDialog*menubar*Attachments.Show_List.set: TRUE",
"Dtmail*ComposeDialog*menubar*Attachments.Show_List.visibleWhenOff: TRUE",
"Dtmail*ComposeDialog*menubar*Format.mnemonic: r",
"Dtmail*ComposeDialog*menubar*Format.Word_Wrap.mnemonic: W",
"Dtmail*ComposeDialog*menubar*Format.Word_Wrap.indicatorType: XmONE_OF_MANY",
"Dtmail*ComposeDialog*menubar*Format.Word_Wrap.set: TRUE",
"Dtmail*ComposeDialog*menubar*Format.Word_Wrap.visibleWhenOff: TRUE",
"Dtmail*ComposeDialog*menubar*Format.Settings.mnemonic: S",
"Dtmail*ComposeDialog*menubar*Format.Templates.mnemonic: T",
"Dtmail*menubar*Mailbox.mnemonic: x",
"Dtmail*menubar*Check_for_New_Mail.mnemonic: M",
"Dtmail*RoamMsgsPopup*Check_for_New_Mail.mnemonic: M",
"Dtmail*menubar*Mailbox.Check_for_New_Mail.accelerator: Ctrl<Key>M",
"Dtmail*menubar*Mailbox.Check_for_New_Mail.acceleratorText: Ctrl+M",
"Dtmail*menubar*Mailbox.Open_Inbox.mnemonic: I",
"Dtmail*menubar*Mailbox.New.mnemonic: N",
"Dtmail*menubar*Mailbox.Open.mnemonic: O",
"Dtmail*menubar*Mailbox.Destroy_Deleted_Message.mnemonic: D",
"Dtmail*menubar*Mailbox.Mail_Options.mnemonic: p",
"Dtmail*menubar*Mailbox.Mail_Options.accelerator: Ctrl<Key>I",
"Dtmail*menubar*Mailbox.Mail_Options.acceleratorText: Ctrl+I",
"Dtmail*menubar*Mailbox.Close.mnemonic: C",
"Dtmail*menubar*Mailbox.Close.accelerator: Alt<Key>F4",
"Dtmail*menubar*Mailbox.Close.acceleratorText: Alt+F4",
"Dtmail*menubar*Message.mnemonic: M",
"Dtmail*menubar*Message*Open.mnemonic: O",
"Dtmail*menubar*Save_As_Text.mnemonic: A",
"Dtmail*RoamMsgsPopup*Save_As_Text.mnemonic: A",
"Dtmail*menubar*Message*Copy_To.mnemonic: C",
"Dtmail*menubar*Print.mnemonic: P",
"Dtmail*RoamMsgsPopup*Print.mnemonic: P",
"Dtmail*menubar*Message*Print.accelerator: Ctrl<Key>P",
"Dtmail*menubar*Message*Print.acceleratorText: Ctrl+P",
"Dtmail*menubar*Message*Find.mnemonic: F",
"Dtmail*menubar*Message*Delete.accelerator: Ctrl<Key>D",
"Dtmail*menubar*Message*Delete.acceleratorText: Ctrl+D",
"Dtmail**menubar*Undelete_Last.mnemonic: L",
"Dtmail*RoamMsgsPopup*Undelete_Last.mnemonic: L",
"Dtmail*menubar*Message*Undelete_From_List.mnemonic: U",
"Dtmail*menubar*Edit.mnemonic: E",
"Dtmail*menubar*Edit.Copy.accelerator: Ctrl<Key>C",
"Dtmail*menubar*Edit.Copy.acceleratorText: Ctrl+C",
"Dtmail*menubar*Attachments.mnemonic: A",
"Dtmail*menubar*View.mnemonic: V",
"Dtmail*menubar*View.Next.mnemonic: N",
"Dtmail*menubar*View.Previous.mnemonic: P",
"Dtmail*menubar*View.Abbreviated_Headers.mnemonic: A",
"Dtmail*menubar*View.Abbreviated_Headers.indicatorType: XmONE_OF_MANY",
"Dtmail*menubar*View.Abbreviated_Headers.set: TRUE",
"Dtmail*menubar*View.Abbreviated_Headers.visibleWhenOff: TRUE",
"Dtmail*menubar*View.By_Date/Time.mnemonic: D",
"Dtmail*menubar*View.By_Sender.mnemonic: S",
"Dtmail*menubar*View.By_Subject.mnemonic: b",
"Dtmail*menubar*View.By_Size.mnemonic: z",
"Dtmail*menubar*View.By_Status.mnemonic: t",
"Dtmail*menubar*Compose.mnemonic: p",
"Dtmail*menubar*Compose.New_Message.mnemonic: M",
"Dtmail*menubar*Compose.New_Message.accelerator: Ctrl<Key>N",
"Dtmail*menubar*Compose.New_Message.acceleratorText: Ctrl+N",
"Dtmail*menubar*Compose.New__Include_All.mnemonic: N",
"Dtmail*menubar*Compose.Forward_Message.mnemonic: F",
"Dtmail*menubar*Reply_to_Sender.mnemonic: R",
"Dtmail*RoamMsgsPopup*Reply_to_Sender.mnemonic: R",
"Dtmail*menubar*Compose.Reply_to_Sender.accelerator: Ctrl<Key>R",
"Dtmail*menubar*Compose.Reply_to_Sender.acceleratorText: Ctrl+R",
"Dtmail*menubar*Compose.Reply_to_All.mnemonic: A",
"Dtmail*menubar*Compose.Reply_to_Sender__Include.mnemonic: S",
"Dtmail*menubar*Compose.Reply_to_All__Include.mnemonic: I",
"Dtmail*menubar*Move.mnemonic: o",
"Dtmail*RoamMsgsPopup*Move.mnemonic: o",
"Dtmail*Inbox.mnemonic: I",
"Dtmail*Other_Mailboxes.mnemonic: O",
"Dtmail*menubar*Help.mnemonic: H",
"Dtmail*menubar*Help.Overview.mnemonic: v",
"Dtmail*menubar*Help.Tasks.mnemonic: T",
"Dtmail*menubar*Help.Reference.mnemonic: R",
"Dtmail*menubar*Help.On_Item.mnemonic: O",
"Dtmail*menubar*Help.Using_Help.mnemonic: U",
"Dtmail*menubar*Help.About_Mailer.mnemonic: A",
"*Message_View*Work_Area.width: 600",
"*Message_View*Work_Area.height: 400",
"Dtmail*separateViewWindow*Work_Area.width: 600",
"Dtmail*separateViewWindow*Work_Area.height: 400",
"Dtmail*Message_Send*scrollHorizontal:	False",
"Dtmail*Message_Send*wordWrap:		True",
"Dtmail*Message_View*scrollHorizontal:	False",
"Dtmail*Message_View*wordWrap:		True",
"Dtmail*Mail_View*scrollVertical:	True",
"Dtmail*menubar*marginHeight:	1",
"Dtmail*menubar*tearOffModel:	TEAR_OFF_DISABLED",
/*
"Dtmail*menubar*Compose.tearOffModel:	TEAR_OFF_ENABLED",
"Dtmail*menubar*Move.tearOffModel:	TEAR_OFF_ENABLED",
*/
"Dtmail*AttachAreaMenuBar*marginHeight:	1",
"Dtmail*HeaderArea*To*marginHeight:	1",
"Dtmail*HeaderArea*Subject*marginHeight:	1",
"Dtmail*HeaderArea*Cc*marginHeight:	1",
"Dtmail*View*Separator*separatorType: SHADOW_ETCHED_IN",
"Dtmail*ComposeDialog*Text*columns: 72",
"Dtmail*Work_Area*Text*columns: 72",
NULL
};
