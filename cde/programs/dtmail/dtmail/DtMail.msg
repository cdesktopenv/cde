$ 
$   COMPONENT_NAME: desktop
$ 
$   FUNCTIONS: none
$ 
$   ORIGINS: 27,118,119,120,121
$ 
$   This module contains IBM CONFIDENTIAL code. -- (IBM
$   Confidential Restricted when combined with the aggregated
$   modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ 
$ +SNOTICE
$ 
$  $TOG: DtMail.msg /main/40 1999/09/14 15:42:03 mgreess $
$ 
$  RESTRICTED CONFIDENTIAL INFORMATION:
$ 
$  The information in this document is subject to special
$  restrictions in a confidential disclosure agreement between
$  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
$  document outside HP, IBM, Sun, USL, SCO, or Univel without
$  Sun's specific written approval.  This document and all copies
$  and derivative works thereof must be returned or destroyed at
$  Sun's request.
$ 
$  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
$ 
$ +ENOTICE
$ 
$		     Common Desktop Environment
$ 
$   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
$   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
$   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
$   (c) Copyright 1993, 1994, 1995 Novell, Inc.
$   (c) Copyright 1995 Digital Equipment Corp.
$   (c) Copyright 1995 Fujitsu Limited
$   (c) Copyright 1995 Hitachi, Ltd.
$                                                                   
$ 
$                     RESTRICTED RIGHTS LEGEND                              
$ 
$ Use, duplication, or disclosure by the U.S. Government is subject to
$ restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
$ Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
$ for non-DOD U.S. Government Departments and Agencies are as set forth in
$ FAR 52.227-19(c)(1,2).
$ 
$ Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
$ International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
$ Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
$ Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
$ Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
$ Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
$ Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan

$quote "

$ 
$ Strings from dtmail/RoamApp.C

$set 1
1 "OK" 
$     /* NL_COMMENT
$      * This is a menu label, please refer to the resource file "Dtmail" for
$      * appropriate mnemonics tranlation
$      */
2 "Mail Options..." 
3 "Vacation" 
4 "Updating properties..." 


$ 
$ Strings from dtmail/RoamMenuWindow.C

5 "Cancel" 
6 "Mailer" 
7 "Mailer" 
8 "Mailer" 
9 "Delete" 
10 "Next" 
11 "Previous" 
12 "Reply to Sender" 
13 "Print" 
14 "Sender" 
15 "Subject" 
16 "Date and Time" 
17 "Size" 
$ 	/* NL_COMMENT
$        * Menu labels in Main Mailer (from RoamMenuWindow.C).
$        * The following menu labels require translation plus parenthesized 
$        * mnemonics for all Asian locales. Please refer to the resource file 
$        * "Dtmail" for each mnemonics.
$ 	 * Message 18-22,25,28,29 are menu labels for "Mailbox" pull down menu.
$        * Message 30,31,33-40 are menu labels for "Message" pull down menu. 
$        * Message 43-45 are menu labels for "Edit" pull down menu. 
$        * Message 47--55 are menu labels for "View" pull down menu. 
$        * Message 56-63 are menu labels for "Compose" pull down menu. 
$        * Message 64,65,221 are menu labels for "Move" pull down menu. 
$        * Message 66,68,69 are menu labels for "Attachment" pull down menu. 
$        * Message 71-77 are menu labels for "Help" pull down menu. 
$ 	 */
18 "Mailbox" 
19 "Check for New Mail" 
20 "Open Inbox" 
21 "Open Inbox" 
22 "New..." 
23 "Mailer - New" 
24 "New" 
25 "Open..." 
26 "Mailer - Open" 
27 "Open" 
28 "Destroy Deleted Messages" 
29 "Close" 
30 "Open" 
31 "Save As Text..." 
32 "Mailer - Message - Save As Text" 
33 "Copy To" 
34 "Print..." 
35 "Find..." 
36 "Select All" 
37 "Delete" 
38 "Undelete Last" 
39 "Undelete From List..." 
40 "Message" 
41 "Move" 
42 "Mailer - Messages" 
43 "Copy" 
44 "Select All" 
45 "Edit" 
46 "Mailer - Text" 
47 "Next" 
48 "Previous" 
49 "Abbreviated Headers" 
50 "By Date/Time" 
51 "By Sender" 
52 "By Subject" 
53 "By Size" 
54 "By Status" 
55 "View" 
56 "New Message" 
57 "New, Include All" 
58 "Forward Message" 
59 "Reply to Sender" 
60 "Reply to All" 
61 "Reply to Sender, Include" 
62 "Reply to All, Include" 
63 "Compose" 
64 "Move" 
65 "Other Mailboxes..." 
66 "Save As..." 
67 "Mailer - Attachments - Save As" 
68 "Select All" 
69 "Attachments" 
70 "Mailer - Attachments" 
71 "Overview" 
72 "Tasks" 
73 "Reference" 
74 "On Item" 
75 "Using Help" 
76 "Help" 
77 "About Mailer..." 
78 "Mailer is confused about the state of this mailbox.\nClose the mailbox by choosing OK and then restart Mailer." 
79 "Converting... %d percent complete" 
80 "Mailer" 

$ 
$ Strings from dtmail/AttachArea.C

81 "Mailer" 
82 "Mailer" 
83 "Mailer" 


$ 
$ Strings from dtmail/Attachment.C

84 "%s is an executable attachment. Do you want to run it?" 
85 "Mailer" 
86 "Mailer" 
87 "Mailer" 
88 "Mailer" 


$ 
$ Strings from dtmail/RoamCmds.C

89 "Mailer - Other Mailboxes" 
90 "Move" 
91 "Printing..." 
92 "OK" 
93 "Save" 
94 "Save" 
95 "Save" 
96 "Empty" 
97 "Rename" 
98 "Mailer - Rename" 
99 "Mailer - Close"
100 "OK" 
101 "Cancel" 
102 "You are already using the forwarding facility for\nsomething other than Vacation. While Vacation is\nrunning, Vacation will be appended to this other\nforwarding activity. Is it still OK to start Vacation?\0" 
103 "Mailer" 
104 "You are already running the vacation program in your .forward file.\nConsult documentation on how to stop it and remove it from your .forward file.\nTry this command after fixing that problem.\0" 
105 "Cannot open .vacation.msg file -- No write permission." 
106 ".vacation.msg file exists. Replace with new text?" 
107 "Cannot open .vacation.msg file -- No write permission." 
108 "I am on vacation" 
$ 	    /* NL_COMMENT
$ 	     * This is the default value of the subject field in the
$ 	     * message that gets returned to the sender when vacation
$ 	     * is turned on.
$ 	     */
109 "I'm on vacation.\nYour mail regarding \"$SUBJECT\" will be read when I return.\n" 


$ 
$ Strings from dtmail/MsgScrollingList.C

110 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
111 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
112 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
113 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
114 "N" 


$ 
$ Strings from dtmail/Undelete.C

115 "Undelete" 
116 "Close" 

$ 
$ Strings from dtmail/SendMsgDialog.C

$ 	    /* NL_COMMENT
$            * Menu labels in Compose dialog (from SendMsgDialog.C).
$            * The following menu labels require translation plus parenthesized 
$            * mnemonics for all Asian locales. Please refer to the resource file 
$            * "Dtmail" for each mnemonics.
$ 	     * Message 117,121,122,125,127-129 are menu labels for "File" pull down menu.
$            * Message 130-134,139,140,155,156 are menu labels for "Edit" pull down menu. 
$            * Message 141,142,145,147-150,226 are menu labels for "Attachment" pull down menu. 
$            * Message 152-154,157 are menu labels for "Format" pull down menu. 
$            * Message 56-63 are menu labels for "Compose" pull down menu. 
$            * Message 64,65,221 are menu labels for "Move" pull down menu. 
$            * Message 66,68,69 are menu labels for "Attachment" pull down menu. 
$            * Message 71-77 are menu labels for "Help" pull down menu. 
$            * Message 241-244 are header field labels for SendMsgDialog.
$ 	     */
117 "Send" 
118 "Close" 
119 "New Message" 
120 "Mailer" 
121 "File" 
122 "Include..." 
123 "Mailer - Include" 
124 "Include" 
125 "Save As Text..." 
126 "Mailer - Save As Text" 
127 "Log Message" 
128 "Send As" 
129 "Close" 
130 "Edit" 
131 "Undo" 
132 "Cut" 
133 "Copy" 
134 "Paste" 
135 "Paste Special" 
136 "Bracketed" 
137 "Indented" 
138 "Clear" 
139 "Delete" 
140 "Select All" 
141 "Attachments" 
142 "Add File..." 
143 "Mailer - Add" 
144 "Add" 
145 "Save As..." 
146 "Mailer - Attachments - Save As" 
147 "Delete" 
148 "Undelete" 
149 "Rename" 
150 "Select All" 
$       /* NL_COMMENT
$        * This is an obsolete message.  It is replaced by message 226
$        * in set 1.
$        */
151 "Show Pane" 
152 "Format" 
153 "Word Wrap" 
154 "Settings..." 
155 "Find/Change..." 
156 "Check Spelling..." 
157 "Templates" 
158 "Mailer - Attachments" 
159 "Mailer - Compose" 
160 "New Message" 
$       /* NL_COMMENT
$        * This is the label for a toggle item in a menu.  When the item
$        * is set to "Show List", the Attachment List is mapped in the
$        * Compose Window.  This message replaces message 151 in set 1.
$        */


$ 
$ Strings from dtmail/ViewMsgDialog.C

161 "Copy" 
162 "Select All" 
163 "Edit" 
164 "Save As..." 
165 "Mailer - Attachments - Save As" 
166 "Select All" 
167 "Attachments" 
168 "Mailer - Attachments" 
169 "Mailer - Text" 
170 "Overview" 
171 "Tasks" 
172 "Reference" 
173 "On Item" 
174 "Using Help" 
175 "Help" 
176 "About Mailer..." 
177 "Mailer" 


$ 
$ Strings from dtmail/DtMailGenDialog.C

$  /* NL_COMMENT
$   * 178 and 179 are obsolete messages, replaced by messages
178 "Mailer - About Dtmail" 
179 "Dtmail Version 1.0" 
180 "OK" 
181 "OK" 
182 "Cancel" 

$ 
$ Strings from dtmail/FindDialog.C

183 "Find" 
$  /* NL_COMMENT
$   * This is an obsolete message.  Replaced by message 220 in set 1
$   */
184 "Find & Select All" 
185 "Clear" 
186 "Close" 
187 "Help" 
188 "To:" 
189 "From:" 
190 "Subject:" 
191 "Cc:" 
192 "Mailer - Find" 
193 "Forward" 
194 "Backward" 
$  /* NL_COMMENT
$   * This message replaces message 184 in set 1
$   */



$ 
$ Strings from dtmail/Editor.C

195 "------------- Begin Forwarded Message -------------\n\n" 
196 "------------- Begin Included Message -------------\n\n" 
197 "------------- End Forwarded Message -------------\n\n" 
198 "------------- End Included Message -------------\n\n" 

$ 
$ Strings from dtmail/XmTextEditor.C

199 "\n------------- Begin Included Message -------------\n" 
200 "\n------------- End Included Message -------------\n" 

$ 
$ Strings from dtmail/DtEditor.C

201 "\n------------- Begin Included Message -------------\n" 
202 "\n------------- End Included Message -------------\n" 

$ 
$ Strings from dtmail/ComposeCmds.C

203 "Mailer" 
204 "Unable to create a compose window." 
205 "Mailer" 
206 "Unable to create a compose window." 
207 "Mailer" 
208 "Unable to create a compose window." 
209 "Mailer" 
210 "Unable to create a compose window." 
211 "Mailer" 
212 "The template does not exist." 
213 "Mailer" 
214 "The template appears to be corrupt." 
215 "Mailer" 
216 "There is not enough memory to load the template." 
217 "Mailer" 
218 "The template appears to be corrupt." 

$ 
$ Strings from dtmail/SortCmd.C

219 "Sorting..." 

220 "Select All" 
221 "Inbox" 

222 "%a %b %d %k:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %k:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
223 "%a %b %d %H:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %H:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
224 "%a %b %d %k:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %k:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
225 "%a %b %d %H:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %H:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
226 "Show List" 

$ String from Undelete.C, Deleted Messages window title
227 "Mailer - Deleted Messages"

$ Strings from SendMsgDialog.C, Add Bcc, Delete Bcc menu buttons
228 "Add"
229 "Delete"
230 "Send"

$  /* NL_COMMENT
$   * This string is displayed on the find dialog status line
$   * when searching for a matching message.
$   */
231 "Searching..."

$        /* NL_COMMENT
$         * These strings are displayed on the find dialog status line
$         * when one or more matching messages are found.  The first
$         * string is displayed when there is one matching message,
$         * and the second string is displayed when there is more than
$         * one.  The %d is the number of messages that matched.
$         */
232 "1 message selected"
233 "%d messages selected"

$  /* NL_COMMENT
$   * This string is displayed on the find dialog status line when
$   * no matching messages were found.
$   */
234 "No matches were found"

$  /* NL_COMMENT
$   * These strings replace strings 178 and 179.  They are used in
$   * the "About Mailer" dialog box.
$   */
235 "Mailer - About Mailer"
236 "Mailer Version %d.%d.%d"
$  /* NL_COMMENT
$   * String 237 is the Copy button on Move Other Mailboxes dialog (from RoamCmds.C)
$   */
237 "Copy"

$ 
$ DTMC_BADSTATE
$ The mailbox is in a state which cannot be deciphered by the Mailer, i.e.
$ portions of the file have been deleted by another process (the Mailer
$ only knows how to handle other processes adding to the end of the mailbox.
$ 
$ "-NEW"
238 "Mailer is confused about the state of this mailbox.\nIt may be that another process has rewritten this mailbox in an unexpected way.\n\nSelecting OK will cause the Mailer to close and reopen the mailbox.\nNOTE that any changes made to the mailbox since the last save may be lost."

$ 
$ DTMC_ACCESSFAILED
$ The Mailer cannot perform routine file operations on the mailbox file,
$ a.k.a. stat, fstat, write, ...
$ 
$ "-NEW"
239 "Mailer can no longer access this mailbox.\nIt may be that another process has deleted the mailbox file.\n\nSelecting OK will cause the Mailer to close and reopen the mailbox.\nNOTE that any changes made to the mailbox since the last save may be lost.\n\nSelecting CANCEL will leave the mailbox unchanged."

$ 
$ DTMC_INODECHANGED
$ The mailbox has been rewritten by another process.
$ 
$ "-NEW"
240 "Mailer can no longer access this mailbox.\nAnother process has rewritten the mailbox file changing the inode.\n\nSelecting OK will cause the Mailer to close and reopen the mailbox.\nNOTE that any changes made to the mailbox since the last save may be lost.\n\nSelecting CANCEL will leave the mailbox unchanged."

$ 
$ Messages 241-244 are header field labels for SendMsgDialog.
$ 
$ "-NEW"
241 "To"
$ "-NEW"
242 "Subject"
$ "-NEW"
243 "Cc"
$ "-NEW"
244 "Bcc"

$ 
$ Message 245 is the label for the Open command in the File menu
$ Message 246 is the label for the command in the OpenContainer menu
$	which opens Mailboxes not in the users folder list.
$ 
$ "-NEW"
245 "Open"
$ "-NEW"
246 "Other Mailboxes..."

$ 
$ Message 247 is the label for the Open command in the File menu
$ Message 248 is the label for the command in the OpenContainer menu
$	which opens Mailboxes not in the users folder list.
$ 
$ "-NEW"
247 "Other Aliases..."
$ "-NEW"
248 "Mailer - Aliases"

$ 
$ Messages 249-255 are strings used to generate attachment summaries
$                  in included messages.
$ 
$ "-NEW"
249 "------------- Begin Included Attachment -------------\n\n" 
$ "-NEW"
250 "------------- End Included Attachment -------------\n\n" 
$ "-NEW"
251 "       Attachment Name:  "
$ "-NEW"
252 "     Attachment DtType:  "
$ "-NEW"
253 "Attachment ContentType:  "
$ "-NEW"
254 "Attachment Description:  "
$ "-NEW"
255 "------------------ Attachments ------------------\n" 

$ "-NEW"
$ DTMC_SERVERACCESSFAILED:
$ Mailer failed in attempt to retrieve messages from the mail server
256 "Mail server access failed:\n%s"

$ "-NEW"
$ Label on the button which causes the "Mail Retrieval" options to be displayed
257 "Options..."

$ "-NEW"
$ DTMC_GETMAILCOMMANDFAILED:
$ Mailer failed in attempt to retrieve messages via custom getmail command
258 "User Getmail command failed:\n%s"

$ "-NEW"
$ /* NL_COMMENT
$  * The "%a %b %d %k:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
259 "%a %b %d  %Y"
260 "%a %b %d %k:%M"
261 "%a %b %d %H:%M"

$ "-NEW"
262 "Dead Letter Message"

$ "-NEW"
263 "The attachment '%s' is '%d' bytes.\nAdd as attachment?"

$set 2
1 "Mailer" 
2 "ToolTalk is not initialized. Mailer cannot run without ToolTalk.\nTry starting /usr/dt/bin/dtsession, or contact your System Administrator." 

3 "Unable to initialize windows. Exiting.\n" 
4 "Mailer has not been properly installed,\nand cannot run because the execution group\nis incorrectly set."
5 "You do not have permission to view %s" 
$ 	    /* NL_COMMENT
$ 	     * The %s is the name of the mailbox the user doesn't have
$ 	     * permission to view.
$ 	     */
6 "The mailbox %s is a directory and can not be opened." 
7 "Unable to empty %s. It does not exist." 
8 "Container %s was emptied" 
9 "Error - wasn't able to empty %s" 
10 "Mailer" 
11 "%s" 
12 "Mailer" 
13 "Mailer" 
14 "An attachment needs to be selected before issuing the\n\"Save As\" command to save to a file." 

15 "Error - Unable to get mailrc." 
16 "No message selected." 
$ 	/* NL_COMMENT
$ 	 * No mail message has been selected by the user.
$ 	 */


17 "You have edited \"%s\". Delete anyway?" 
18 "Error: Cannot include file %s" 
19 "An attachment needs to be selected before issuing the\n\"Save As\" command to save to a file." 


20 "An attachment needs to be selected before issuing the\n\"Save As\" command to save to a file." 

21 "Mailer" 
22 "There were unrecoverable syntax errors found in the ~/.mailrc file.\nCheck for more messages on terminal. Fix the errors and restart dtmail.\nIf you choose to continue you will not be able to save any changes made\nin the options dialog to file."
23 "Continue"
24 "Exit"
25 "There were unrecoverable syntax errors found in the ~/.mailrc file.\nMail options settings can be applied in the current session but can\nnot be saved to the file." 

$ 
$ Messages for file locking dialog displayed from the Advanced option pane.
$ 
$ "-NEW"
26 "The value of the file locking option has changed.\nTo take immediate effect, folders need to be closed and reopened.\n  o Click on 'Reopen' to reopen folders,\n    making the new option take effect immediately.\n  o Click on 'Continue' to avoid reopening folders.\n    The new option will take effect the next time you open a folder.\n  o Click on 'Cancel' to cancel the Apply and return to editing options."
$ "-NEW"
27 "Reopen"
$ "-NEW"
28 "Continue"
$ "-NEW"
29 "Cancel"

$ 
$ Messages for ToolTalk errors.
$ 
$ "-NEW"
30 "%s returned ToolTalk error: %s\n"

$set 3
1 "Auto-saving..." 

2 "Initializing..." 
3 "The mailbox %s does not exist.\nCreate a mailbox with this name?"
4 "Mailer" 
5 "OK" 
6 "The mailbox %s is locked.\nYou can manually unlock the mailbox and try again\nor contact your System Administrator."
7 "Mailer" 
8 "OK" 
9 "OK" 
10 "OK" 
11 "OK" 
12 "%s" 
13 "Message %d of %d, %d new, %d deleted" 
14 "Message %d of %d, %d new, %d deleted" 
$ 	/* NL_COMMENT
$ 	 * The user will see the following message display as:
$ 	 * "Message 3 of 10, 2 new, 6 deleted"
$ 	 * This means ??? -- Explain to translator.
$ 	 */
15 "Saving..." 
16 "Mailer" 
$ 		/* NL_COMMENT
$ 		 * This dialog comes up when the user tries to quit the
$ 		 * mailbox and the user is asked if he wants to destroy
$ 		 * the messages marked for deletion.
$ 		 */
$		/* NL_COMMENT
$		 * Messages 16 and 17 are no longer being used.  They are
$		 * being replaced by messages 86 and 87.
$		 */
17 "Destroy the messages you have marked\nfor deletion in this mailbox?"
18 "OK" 
19 "Cancel" 
20 "Opening mailbox..." 
21 "%s already exists.\nOverwrite?"
22 "Mailer" 
23 "Unable to overwrite %s.\nCheck file permissions and retry." 
24 "Mailer" 
25 "Loading container..." 
26 "Folder Summary Information" 
27 "Loading..." 
28 "Empty container" 
29 "OK" 
30 "OK" 
31 "OK" 
32 "Destroying deleted messages..." 
33 "OK" 

34 "%s does not exist." 
35 "Unable to open %s." 
36 "Unable to allocate memory." 
37 "Summary of attachments" 
38 "attachment" 
39 "attachments" 
$ 	/* NL_COMMENT
$ 	 * "attachments" is the plural form of "attachment".
$ 	 */
40 "attachments" 
41 "deleted" 

42 "%s already exists. Replace?" 
43 "Unable to replace %s." 
44 "Unable to create %s." 
45 "Unable to create %s." 


46 "Empty container" 
47 "%s already exists.\nOverwrite?" 
48 "Mailer" 
49 "Unable to overwrite %s.\nCheck file permissions and retry."
50 "Mailer" 
51 "Unable to create %s." 
52 "Mailer" 
53 "Unable to write to %s." 
54 "Mailer" 
55 "Unable to create %s." 
56 "Mailer" 
57 "Rename attachment as" 
58 "The Compose window contains text or\nattachments that will be lost if\nthe window is closed.\nClose the Compose window?"
59 "No Memory" 
60 "There is not enough memory to load the existing .vacation.msg file." 
61 "Mailer" 
62 "The existing .vacation.msg file appears to be corrupt." 


63 "Select a message to move." 
64 "Select a message to copy." 
65 "%d messages moved to %s" 
66 "%d message moved to %s" 
67 "%d messages copied to %s" 
68 "%d message copied to %s" 
69 "View already exists. Raising it." 
$ 	/* NL_COMMENT
$ 	* The current mail message selected is already displayed in a separate
$ 	* window.  Therefore this 'separate' window will be raised in front
$ 	* of existing windows so the user can see it.
$ 	*/



70 "Writing dead letter..." 
71 "Mailer" 
72 "OK" 
73 "Cancel" 
74 "OK" 

75 "OK" 
76 "OK" 

77 "You have an attachment open that may have unsaved changes.\nSending this message will break the connection to the open\n attachment. Any unsaved changes will not be part of the\n message. You can use Save As to save Changes after the\n connection is broken, but the changes will not be part of\n the attachment."
78 "Initializing Mail Options..."
79 "Destroy and Quit"
80 "Don't destroy, Quit"
81 "This attachment may contain commands that can cause serious\ndamage.  It is recommended that you only execute it after you\nare certain it is safe to do so.\n\nPress OK if you are certain it is safe,\nCancel to cancel execution."
82 "Someone else is using this mailbox.\nWould you like to demand exclusive access?"
83 "Mailer is unable to obtain exclusive access to this mailbox.\nWould you like to open this mailbox read-only?"

$            /* NL_COMMENT
$             * The following sentence means %d number of mail messages have
$             * been deleted from the mail folder.  This is the plural form
$             * of the message that gets printed if more than one message
$             * is moved.
$             */
84 "%d messages deleted"

$            /* NL_COMMENT
$             * The following sentence means %d number of mail messages have
$             * been deleted from the mail folder.  This is the singular
$             * form of the message that gets printed if only one message
$             * is moved.
$             */
85 "%d message deleted"
86 "Checking for new mail..."
$		/* NL_COMMENT
$		 * This dialog comes up when the user tries to quit the
$		 * mailbox.  The user is asked if they want to destroy
$		 * the deleted messages.
$		 */
87 "Mailer - Close"
88 "Destroy the deleted messages and close this mailbox?"
89 "Destroy and Close"
90 "Retain and Close"
$        /* NL_COMMENT
$         * Post a dialog explaining that the action was invalid
$         */
91 "Cannot execute invalid action."
$        /* NL_COMMENT
$         * Post a dialog explaining that the action failed.
$         */
92 "Executing action failed!"
93 "The selected icon %s is an executable.\nClick OK to run it or Cancel to quit."

94 "Mailer is unable to obtain exclusive access to this\nmailbox because the system is not responding.\n\nFor this time only, you can choose to open this mailbox\nread-only, or to open it read-write without exclusive\naccess (use only if no one else is using this mailbox)."
95 "Read-only"
96 "Read-Write"

$ 
$ "-NEW"
$ 
$ This message appears when dtmail tries to open a mailbox which has been
$ opened by another dtmail process.
$ 
$ Parameters:
$   1.  Error message saying that the <mailbox>.dtmail lockfile has been found.
$   2.  The mailbox path.
$   3.  "OK"
$   4.  "Cancel"
$   5.  The <mailbox>.dtmail lockfile path.
$ 
97 "%s\n\nThis may indicate that another Mailer has opened\n%s\n\nYou may open this mailbox READ ONLY by choosing '%s'.\n\nOtherwise, you may choose '%s',\nmake sure that another Mailer has not opened this mailbox,\ndelete %s,\nand reopen this mailbox."

$ 
$ "-NEW"
$ 
$ Messages 98-100 check for problems in the paths specified by users of
$ the file selection box.
$ 
98 "Search permission denied on a component of the path,\n'%s'."
99 "A component of the path is not a directory,\n'%s'."
100 "A component of the path does not exist,\n'%s'."

$set 5
1 "Mailer" 
2 "Mailer" 
3 "Sending this message as is will\n	    cause you to lose any changes\n	    to the attachments."
$    /* NL_COMMENT
$     * The user tried to send a messages without saving changes in
$     * some open attachments.  This warning makes sure that is what
$     * the user intended.
$     */
4 "Select only one attachment\n	    and then choose rename"
$       /* NL_COMMENT
$        * Mailer ran out of memory.  Ask the user to quit some other
$        * applications so there will be more memory available.
$        */
5 "Some of the addresses in the message are incorrect,\nand do not refer to any known users in the system.\nPlease make sure all of the addresses are valid and try again."
$       /* NL_COMMENT
$        * Mailer ran out of memory.  Ask the user to quit some other
$        * applications so there will be more memory available.
$        */
6 "Mailer does not have enough memory\navailable to send this message.\nTry quitting other applications and\nresend this message."
$       /* NL_COMMENT
$        * There was an error from the mail transport (sendmail).
$        */
7 "An error occurred while trying to send your message.\nCheck to make sure the message was received.  If not,\nyou may have to resend this message."
$       /* NL_COMMENT
$        * User trying to send a message that has no addressed recipient.
$        */
8 "Try Send after specifying  recipient(s) of the message in \nthe To:, Cc:, or Bcc: fields."

$set 12
1 "Attachment" 
2 "Attachments" 
3 "displayCount" 
4 "Cannot attach FIFO files: %s" 
5 "Cannot attach character special files: %s" 
6 "Cannot attach directories: %s" 
7 "Cannot attach block special files: %s" 
8 "Cannot attach socket files: %s" 


$set 13
1 "Cannot create" 
2 "Unable to write" 

$ 
$ Strings from dtmail/AttachCmds.C

$set 14
1 "Add Attachment" 
2 "Unable to open %s" 
3 "Cannot attach FIFO files: %s" 
4 "Cannot attach character special files: %s" 
5 "Cannot attach directories: %s" 
6 "Cannot attach block special files: %s" 
7 "Cannot attach socket files: %s" 
8 "Unable to open %s" 
9 "Mailer - Rename Attachment" 
10 "Description" 
11 "Save Attachment As" 

$set 15
1 "You have made unsaved changes.\nYou may save your changes, discard your changes,\nor return to your previous place in the dialog."
2 "Save"
3 "Discard"
$ 
$ The followings are added by IBM for 41J release
$ 
$set 20
1 "The mailbox %s is already open."
$ The above message is used in RoamMenuWindow::open
$ See AIXC cmvc defect 175920
2 "Use local name: "
$ The above message is used in OptCmd::init_advanced_pane()
$ see AIX cmvc defect 176608
3 "Read Only"
$ The above message is used in RoamMenuWindow::mapnotify()
$ see AIX cmvc defect 176608

$ 
$	SETS 21 - 25 WERE ADDED TO SUPPORT CDEnext PRINTING
$ 
$	The following set was added to support CDEnext printing based on
$	the Xp Print Server.
$ 
$set 21
$ 
$	Messages 1 - 11 are used in DmxPrintJob.
$ 
$ 
$	This is the status message that gets displayed in the MainWindow
$	and RoamMenuWindow status lines when a print job is being executed.
$	%s = the name of the temporary file containing the message(s) to be
$	printed.
1 "Printing %s ..."

$	Messages 2 - 4 are used in the error popup which gets displayed
$	when an invalid margin specifier has been supplied by the user.
$ 
$	Message 2 is the actual error messagactual error message
$	Message 3 is the title which is displayed in the error popup.
$	Message 4 is the label on the OK button of the error popup.
2 "One of the following margin specifiers \n has incorrect syntax: \n %s \n %s \n %s \n %s \nContinue using default margins?"
3 "Mailer"
4 "OK"

$	Messages 5 - 11 are format string used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 5 is the format string used to make a Cc header/footer.
$	Message 6 is the format string used to make a Date header/footer.
$	Message 7 is the format string used to make a From header/footer.
$	Message 8 is the format string used to make a Subj't header/footer.
$	Message 9 is the format string used to make a To header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
5 "Cc:  %s"
6 "Date:  %s"
7 "From:  %s"
8 "Subject:  %s"
9 "To:  %s"
10 "Page %d of %d"
11 "Mail For:  %s"

$ 
$	Messages 12 - 13, 20 are used in DmxPrintSetup to construct the
$	app-specific printing options for DtMail which will be included
$	in the DtPrintSetupBox.
$ 
$	Message 12 is the label used on the Toggle Button to indicate that
$	multiple messages should be printed in separate spool jobs.
$	Message 13 is the label used on the Toggle Button to indicate that
$	lines which would extend beyond the edge of the printed page be 
$	"wrapped" to the next line.
12 "Print Separately"
13 "Use Word Wrap"


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
$	NOTE:  'Print to File' should be translated as in DtPrint.msg
$		Set 1, Message 3.
14 "'Print to File' was unsuccessful."
15 "'Print to File' completed successfully."

$       NL_COMMENT 
$          Messages 16 - 19 are obsolete, and have been replaced by 24
$ 
$	Messages 16 - 19 are status messages for the popup PDM function.
$ 
$	Message 16 indicates that there was no pdm deamon running.
$	Message 17 indicates that the pdm deamon was unable to start a pdm.
$	Message 18 indicates that the pdm couldnt connect to the video display.
$	Message 19 indicates that the pdm couldnt connect to the print display.
16 "No PDM found in the environment."
17 "Cannot start the PDM."
18 "PDM is not authorized to connect to Video display."
19 "PDM is not authorized to connect to Print display."

$	Message 20 is the label used on the Push Button to bring up
$	the printing options dialog from the DtPrintSetupDialog.
20 "More ..."
$	Message 21 is used for the title string on the DtPrintSetupDialog.
21 "Mailer - Print Setup"

$	Message 22 is used to alert the user to the fact that the mailer
$		   cannot close the mailer window until a task (usually
$		   a print job) has terminated.
22 "Close pending:  waiting for task to terminate ..."

$	Message 23 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
23 "The X Print Server is temporarily out of resources"

$       Messages 24 & 25 are used to handle errors from the PDM
24	"Print Dialog Manager (PDM) error - setup failed."
25	"Continue"

$ 
$	The messages in this set are used in pulldown menu for the
$	header/footer $	frame which is part of the "Printing" pane
$	in the Options dialog.
$ 
$	Messages 1 - 8 are used in DmxPrintOptions.
$ 
$ 
$	Message 1 specifies that the page header/footer field be empty.
$	Message 2 put the Cc msg header in the page header/footer field.
$	Message 3 put the Date msg header in the page header/footer field.
$	Message 4 put the From msg header in the page header/footer field.
$	Message 5 put the Subject msg header in the page header/footer field.
$	Message 6 put the To msg header in the page header/footer field.
$	Message 7 put the Page Nmbr msg header in the page header/footer field.
$	Message 8 put the User Name msg header in the page header/footer field.
$set 22
1 "Empty"
2 "CC Header"
3 "Date Header"
4 "From Header"
5 "Subject Header"
6 "To Header"
7 "Page Number"
8 "User Name"

$ 
$	The messages in this set are used in pulldown menu for the
$	Printed Message Headers option which is part of the "Printing" pane
$	in the Options dialog.
$ 
$	Messages 1 - 4 are used in DmxPrintOptions.
$ 
$	Message 1 don't print any message headers.
$	Message 2 print the "standard" msg headers (To, From, Subject, Cc).
$	Message 3 print the "abbreviated" msg headers as defined by the user.
$	Message 4 print all message headers.
$ 
$set 23

1 "None"
2 "Standard"
3 "Abbreviated"
4 "All"

$ 
$	The messages in this set are used in pulldown menu for the
$	Printed Message Headers option which is part of the "Printing" pane
$	in the Options dialog.
$ 
$	Messages 1 - 5 are used in DmxPrintOptions.
$ 
$	Message 1 use a New Line (CRLF) to separate multiple messages.
$	Message 2 use a Blank Line (2xCRLF) to separate multiple messages.
$	Message 3 use a Line of characters to separate multiple messages.
$	Message 4 start each new message on a new page.
$	Message 5 print each message in it's own spool job.
$ 
$set 24
1 "New Line"
2 "Blank Line"
3 "Character Line"
4 "New Page"
5 "New Job"

$ 
$	The following set was added to support CDEnext printing based on
$	the Xp Print Server.
$ 
$	Messages 1 - 2 are used in DmxPrintOptions.
$ 
$ 
$set 25
$ 
$	Label for the option menu used to specify which message headers are
$	printed.
1 "Printed Message Headers:  "
$ 
$	Label for the option menu used to specify which how multiple messages
$	are separated.
2 "Separate Multiple Messages With:  "

$ 
$	The following set was added to support CDEnext printing based on
$	the Xp Print Server.
$ 
$	Messages 1-4 are used in error dialogs to indicate an invalid
$		     entry for margin specifiers in the printing options pane.
$	Message 5 is the label string on the button to dismiss the error dialog.
$ 
$ 
$set 26
1 "Top Margin specifier is invalid:  "
2 "Right Margin specifier is invalid:  "
3 "Bottom Margin specifier is invalid:  "
4 "Left Margin specifier is invalid:  "
5 "Continue"

$ 
$	The following set was added to support the Mail Retrieval option pane.
$ 
$	Messages 1-5 Used in the Server Protocol option menu to indicate the
$		     set of mail server network protocols which are supported.
$	Messages 6-15 Are used to label the options in the pane.
$ 
$ 
$set 27
1 "Auto"
2 "POP3"
3 "APOP"
4 "IMAP"
5 "POP2"

6 "Automatic (system) delivery"

7 "Mail server retrieval"
8 "Server Protocol:"
9 "Server Name:"
10 "User Name:"
11 "Password:"
12 "Remember password"
13 "Delete from server after retrieval"
14 "Retrieve old messages"

15 "Custom retrieval"
16 "User Getmail command:"

17 "Null INBOX path."
18 "INBOX folder path:"
