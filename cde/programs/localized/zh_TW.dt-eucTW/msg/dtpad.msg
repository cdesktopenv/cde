$ $XConsortium: dtpad.msg /main/4 1995/12/08 09:50:26 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpad.msg
$ **
$ **  Project:     DT Text Editor client (sometimes called dtpad)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtpad
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ *****************************************************************************
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ *****************************************************************************
$ **

$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************

$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
1 ëºÉ·ë©Íü


$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 âäÚöóòÕù

$ label for the File text field in the file selection box
$ ***UNUSED***	This label in no longer used and need not be localized.
2 ÓÑâäÚöÎûóòÕù¡¨

$ Title of the Include dialog
3 ÉÖÑÄóòÕù

$ Label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
4 ÓÑÉÖÑÄÎûóòÕù¡¨

$ Error when printing a file and a temporary copy can not be saved.
5 àÒÎÎĞúÇ¡í²Õëóò¡¤


$set 5
$ ***************************************************************************
$ **                                                               dtpad.c **
$ **                                                             fileDlg.c **
$ **                                                                       **
$ ** Messages and strings used in dialogs for all operations involving the **
$ ** "File" menu items.                                                    **
$ ***************************************************************************
$ These compose the dialog which appears if the user attempts to exit, open
$ a different file, drop a file, or start a "New" session with unsaved
$ changes to the current file.

$ This is used to build: "Save changes to FILENAME?"
1 ÓÑóÃÇô %s ÎûÔºÊÑİåÅ¡ãÃ?

$ This is used as-is, whenever the current file is unnamed.
2 Ü¢ÈçóòÕùÊôÈ´óòÇØ¡¤\nÚÀÓÑóÃÇôİÕÔ¶óòÕùãÃ?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***	Currently, dropped files are INSERTED in the text rather than
$		opened for editing so this message is no longer used and need
$		not be localized.
3 ÚÀÓÑÇ¿óÃÇô %s ÄæÑ¥¡¢ÇÂî¾òÒ %s ãÃ?

$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.
5 óòÕù %s ÄØÇôÇã¡¤\nÚÀã÷Åèä»î¾òÒÎûÄùÕ©\nÓììÑæÚóòÕùãÃ?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 úÀÉË

$ Title of "Save As" dialog.
7 Æ¶Çôä»óò

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***	These labels are no longer used and need not be localized.
$		They've been replaced by messages 44 and 45
8 ÇãÈÜÙ¯ßĞÈçÎûÈçá¸Ê¢ÅûÄ¸ßĞÈçÇóÄ÷¡¤
9 ÄâÅûÄ«ßĞÈçÇóÄ÷¡¤Æ·ÏñÖçÅè [Return] Ü¨ÆíÎû\nßĞÈç¡¤

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 ÉÁ
15 ÑÒ
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 ÓÑóÃÇôãÃ?

$ Title of general error dialog
17 òãë¨

$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message
18 Exit Confirmation

$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.
20 æñØ¹

$ Default file name in Save As dialog box.
21 UNTITLED

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 àÒÎÎóÃÇô %1$s¡¤\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 àÒÎÎóÃÇôóòÕù¡¤\nÆÜÑÀÌùóòÇØ¡¤

34 æÚóòÕùÄâÄøİÂÇ¶ÈôÄ©ìÑÄ«¡¤

35 æÚóòÕùÑÒÄ¡Ô¶Æøòç¡¤

36 æÚóòÕùÑÒÄ¡Ô¶ÇóÄ÷ÖÖÖ±æÒåô¡¤

37 æÚóòÕùÑÒÄ¡Ô¶Ù´Ñôí¼èèæÒåô¡¤

$ ***UNUSED***	This message should never appear since currently there is
$		only one supported file format - text.  And this type is
$		hard coded into dtpad.
38 àÒÕæÎûóòÕùÖªÈ¢¡¤

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 Ü¡ÆñÄâÍü - Æ«×äÑÒéöé÷Ï¨âæÄâËÍ¡¤

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 ÓÑóÃÇôüÈÊÕİåÅ¡ãÃ¡©

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 ÓÑÙòüÈÊÕİåÅ¡óÃÇôÌ¯ "%s" ãÃ¡©

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***	This title is no longer used and need not be localized.
42 îïêûÒ³

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 îïêûÌ¯óòÕù

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 ÇãÈÜÙ¯ßĞÈçÎûÈçá¸Ê¢ÅûÄ«ßĞÈçÇóÄ÷¡¤
45 ÄâÅûÄ«ßĞÈçÇóÄ÷¡¤ Æ·ÏñÖçÅè [Return] Ü¨ÆíÎû\nßĞÈç¡¤
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This
$ new message is in response to bug DTS CISlx19530.
$ 
46 Æ«×äÑÒóòÕùàÒÎÎìÑÄ«\nÍĞÌ¯æÚóòÕùÎûæûÕÀÄâÇôÇã¡¤


$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 æÚóòÕùÆ·×äûôÌ½¡¤
2 æÚóòÕùÄâÇôÇã¡¤
3 àÒÎÎî¾òÒÆøòç¡¤
4 àÒÎÎî¾òÒÇóÄ÷ÖÖòÓæÒåôóòÕù¡¤
5 àÒÎÎî¾òÒÙ´Ñôí¼èèæÒåôóòÕù¡¤
6 ÆñóòÕùÍäÅøÄæÄùĞúÏ¨Ô«
7 àÒÎÎç¥Ä«óòÕù (Ø´ğÙüÕÄâËÍ)¡¤
8 àÒÎÎÇôÌ½óòÕù (àÒÎÎÚµÏ¡Ì§Îíèè)¡¤
9 àÒÎÎûôÌ½óòÕù (Æ«×äàÒûôÌ½İÂÆ«ûâ)¡¤
$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 Æ·×äèÖÒà
11 ÙÄûô


$set 7
$ ************************************
$ **                         main.c **
$ **                        dtpad.c **
$ ** Prefix string for all dialogs. **
$ ************************************
$ bogus?? blank string gotten but otherwise unused in main()
$ DO NOT LOCALIZE string 1
$quote "
1 ""
$quote

$ The name used in the title bar of every window.
$ Be sure to end this message with a space character.
2 ÅÆÇóî¾òÒğÂ - 

$ Default file name used in the main window title.
$ ***UNUSED***	This mesage has been replaced by message 6 of this set and
$		need not be localized.
3 (unnamed)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 àÒÎÎØÄÌîİÃÌùÖ¤Ä¸ê§Ü¡Ë·ÜÓÎ¤Ù­... á¸ÊÖÅÆÇóî¾òÒğÂ¡¤

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 àÒÎÎâäÚöüÏÆüğÂ... á¸ÊÖÅÆÇóî¾òÒğÂ¡¤

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (UNTITLED)
$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 òãë¨: Ä¡Ô¶ dtpad ÈùÎ¤ğÂ %s\n\
ÄØåîÇãÎ¤Ù­æÚüÏÆüğÂ¡¤
8 òãë¨: Ä¡Ô¶ dtpad ÈùÎ¤ğÂ\n\
ÄØåîÇãÎ¤Ù­æÚüÏÆüğÂ¡¤
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' ÑÒòãë¨ÎûòÙâú¡¤\n\
ÆîÎÎ: %s...\n\
   -saveOnClose\n\
\tÓ¼È´ÆÜóÃÇôÎûÊÕÊÑÅâÓÑùÃİğÅÆÇóî¾òÒğÂÕë,\n\
\tDtpad äÄÈÜÙ¯ËäòûÕÊÕÊÇâóÃÇôÜ¢ÈçÅÆÇó¡¤\n\n\
   -missingFileWarning\n\
\tÊäåµÑÀÌùÄ¡Ô¶ÄâÇôÇãÍĞàÒÎÎÇôÌ½ÎûóòÕùÕë,\n\
\tÉ»üÏÆüÄ¡Ô¶úÀÉËè×æãÕú¡¤\n\n\
   -noReadOnlyWarning\n\
\t££úåµËğÆîÏ¯ÑÀÌùÄ¡Ô¶£¦ÙÈ´ìÑÄ«İÂÆ«ûâÎûóòÕùÕë,\n\
\tÌ½Ö¼úÀÉËè×æãÕúüÏÆü¡¤\n\n\
   -noNameChange\n\
\tÑÀÆüåµÄùÅÆóÃÇôÈ©ÄâÇÑÍõûôÌ½ÕëÎûÇØê¢Õë,\n\
\tÄâÊÕÊÑÌÏÜ¢ÈçÄùÅÆÒŞùÃÎûÄùÌùóòÕùÇØê¢¡¤\n\n\
   -viewOnly\n\
\tÄâÄøİÂÇãî¾òÒáşá§Äãî¾òÒÅÆÇó, Ù×ÆÛÄ¸ÑÒÙò\n\
\tÅÆÇóî¾òÒğÂüÈÈ©ÅÆÇóèÖÒàğÂ¡¤\n\n\
  -statusLine\n\
\tÇãî¾òÒáşá§Íµê¦üÏÆüÄ¡ÈçÎíèèÈç¡¤ æÚÎíèèÈç\n\
\täÄüÏÆüÅÆÇóà¬íºÆøĞ©ÍÔÇãÈíåôÎûÈçæÀ¡¤\n\n\
  -wrapToFit\n\
\tâäÚöÈÜÙ¯ßĞÈçí¼èè¡¤\n\n\
   -workspaceList WorkspaceList\n\
\tÇãÑÀÌùÎûÄÖÉ¢Ï¨âæÄãîşÆîÜ¢ÈçÎûÅÆÇó\n\
\tî¾òÒğÂ, ÅèüÏÆüî¾òÒáşá§¡¤\n\n\
   -session SessionFile\n

10 \tè×ÍÔÈ´ÎûÅÆÇóî¾òÒáşá§ÌÏĞ©È¹ CDE ùÃñ¢ÕëàõÆíÕæÆî\n\
\tÎûİÃÌùÔ«Çßß¦ÅÆÇóî¾òÒğÂ¡¤ åµÑÀÌùÈºòÙâúÕë, ÍÔÈ´Ì§\n\
\tÅìÎûÌÔÅïÈçòÙâúÒÕäÄİ·ÍÁÜ©¡¤\n\n\
   -standAlone\n\
\tÚ°ÓãÜ¢ÈçîşÆîÎûÅÆÇóî¾òÒğÂÇãÌ§ÆÛËÎÎûáşá§Äã, \n\
\tâĞÈçÈÜÄ×ÎûÅÆÇóİ¨Ü¡, ÈÔñÁÇ¡ÍõÅÆÇóî¾òÒğÂÈù\n\
\tÎ¤ğÂÄæÆÀ¡¤\n\n\
  -noBlocking\n\
\tÇãÅÆÇóî¾òÒğÂÈùÎ¤ğÂÆ«İ¨Ü¡ÓÑÊåÏ¯Îûî¾òÒÓÑÊåÕë,\n\
\tÉ»Ì«ÜÜÅÏÅÆÇóî¾òÒğÂÓÑÊåÏ¯Îûá£Ê©¡¤\n\n\
  -server\n\
\tÚ°ÓãÅÆÇóî¾òÒğÂÈùÎ¤ğÂÚöÙ¯ (Ó¼ÆÆÌşÆÜÙÚÈçÎûæã)\n\
\tÅèİ¨Ü¡æÚüÏÆüğÂâëÑ¥ÈÔËôÎûî¾òÒÓÑÊå¡¤\n\n\
  -exitOnLastClose\n\
\tÑÀÌùåµüÏÆüğÂÄ¸Ş²Ñ¥Ä¡Ô¶î¾òÒáşá§ùÃİğÕë,\n\
\tÜÜÅÏÅÆÇóî¾òÒğÂÈùÎ¤ğÂá£Ê©¡¤\n\
\tÆÆÆ·ØÙÇÙ -server òÙâúËğÆî, ÇŞÒ³ÆÆÆ·ïËÆî\n\
\tÍõÈùÎ¤ğÂá£Ê©¡¤\n\n\
È´ùÃİÕËèòÙâúÎûæÛÜØæñÕè, îùÙ¶ïå dtpad îÀÄ¸ßÎÉ·Ó÷¡¤\n\n
$ 


$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 óôÑçóòÕùİÂÆ«ûâ¡¤
2  %s ÑÒÆîËôóÃÇôÚÀâêÑôÉ¢äÆÎûóòÕù¡¤


$set 11
$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************

$ ***UNUSED*** 	Messages 1-14 are no longer used and need not be localized.
$		All of the mnemonics and labels relating to the "Help" menu
$		have been replaced by messages 81-96.

$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
1 H
2 ëºÉ·ë©Íü(H)

$ mnemonic and label for the "Overview" item
3 V
4 äÍı¨(V)...

$ mnemonic and label for the "Tasks" item
5 T
6 ÄÖÉ¢(T)...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 Ù¶ÈÓæñÕè(R)...

$ mnemonic and label for the "On Item" item
9 O
10 ùÃÍõâúÆø(O)

$ mnemonic and label for "Using Help" item
11 U
12 ËğÆîëºÉ·ë©Íü(U)...

$ mnemonic and label for "About Text Editor" item
13 A
14 ùÃÍõÅÆÇóî¾òÒğÂ(A)...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 óòÕù(F)

$ mnemonic and label for the "New" item
17 N
18 ä»ìÁ(N)

$ mnemonic and label for the "Open" item
19 O
20 âäÚö(O)...

$ mnemonic and label for the "Include" item
21 I
22 ÉÖÑÄ(I)...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 S
24 óÃÇô(S)

$ mnemonic and label for the "Save As" item
25 A
26 Æ¶Çôä»óò(A)...

$ mnemonic and label for the "Print" item
27 P
28 ÇÄÇÍ(P)...

$ mnemonic and label for the "Close" item
29 C
30 ùÃİğ(C)

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 E
32 î¾òÒ(E)

$ mnemonic and label for the "Undo" menu item
33 U
34 õäÔÏ(U)

$ mnemonic and label for the "Cut" menu item
35 t
36 ÙªÌ½(t)

$ mnemonic and label for the "Copy" menu item
37 C
38 îïêû(C)

$ mnemonic and label for the "Paste" menu item
39 P
40 ö´â²(P)

$ mnemonic and label for the "Clear" menu item
41 e
42 ÛÒØæ(e)

$ mnemonic and label for the "Delete" menu item
43 D
44 É´Øæ(D)

$ ***UNUSED***	Messages 45-50 are no longer used and need not be localized.
$		They have been replaced by messages 101-106 of this set.
$ mnemonic and label for the "Select All" menu item
$ 45 A
$ 46 Select All

$ mnemonic and label for the "Find/Change" menu item
$ 47 F
$ 48 Find/Change...

$ mnemonic and label for the Check Spelling menu item
$ 49 S
$ 50 Check Spelling...

$ *
$ * Mnemonics and labels for the "Format" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
51 r
52 ÖªÈ¢(r)

$ mnemonic and label for the "Settings..." menu item
53 S
54 İÃÌù(S)...

$ mnemonic and label for the "Paragraph" menu item
55 P
56 Ñôæ¯(P)

$ mnemonic and label for the "All" menu item
57 l
58 ÇÀİå(l)

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 O
60 òÙâú(O)

$ mnemonic and label for the "Overstrike" toggle button
61 O
62 ÷»ìÑí¼èè(O)

$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***	This label and mnemonic are no longer used and need not be
$		localized.  They've been replaced by messages 111 and 112.
63 W
64 ÈÜÙ¯ßĞÈç(W)

$ mnemonic and label for the "Status Line" toggle button
65 S
66 ÎíèèÈç(S)

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** 	This nmemonic and label are no longer used and need not be
$		localized.  They've been replaced by messages 87 and 88.
71 C
72 Æøòç(C)...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 ÆÒâüóÃÇô(S)


$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 H
82 ëºÉ·ë©Íü(H)

$ mnemonic and label for the "Overview" item
83 v
84 äÍı¨(v)

$ mnemonic and label for the "Tasks" item
85 T
86 ÄÖÉ¢(T)

$ nmemonic and label for the "Table of Contents" item
87 C
88 Æøòç(C)

$ mnemonic and label for the "Reference" item
89 R
90 Ù¶ÈÓæñÕè(R)

$ mnemonic and label for the "On Item" item
91 O
92 ùÃÍõâúÆø(O)

$ mnemonic and label for "Using Help" item
93 U
94 ËğÆîëºÉ·ë©Íü(U)

$ mnemonic and label for "About Text Editor" item
95 A
96 ùÃÍõÅÆÇóî¾òÒğÂ(A)


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 S
102 ÇÀİåòÙÌ½(S)

$ mnemonic and label for the "Find/Change" menu item
103 F
104 ŞòÊÆ(F)/ÊÕÊÑ...

$ mnemonic and label for the Check Spelling menu item
105 k
106 Ñ»ÇóóôÑç(k)...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***	This label is no longer used and need not be localized.
107 A
108 îïêûÒ³(A)...
$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 F
110 îïêûÌ¯óòÕù(F)...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 W
112 ÈÜÙ¯ßĞÈç(W)


$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 ÚÀÆÒâüÇãÅÆÇóî¾òÒğÂÄãòÙÌ½Ä¡Ô¶âúÆø¡¤


$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 óôÑçöµóÃÇôÆÂÚõ¡¤
2 óôÑçöµÇßóÜÆÂÚõ¡¤

$ NEW message after the SAMPLE IMPLEMENTATION.... more errors/warnings can
$ be detected while reading in a file via ToolTalk.
3 àÒÎÎİÃÌùÅÆÇóî¾òÒğÂÎûÄùÕ©¡¤ (ÔÏÇŞÄâÍü).
