$ $TOG: dtpad.msg /main/16 1999/09/15 14:23:53 mgreess $
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
$quote "
$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************

$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
$quote
1 Help
$ Unrecognized arguments
2 dtpad: illegal option: %s 
$ Extra arguments
3 dtpad: extra argument: %s


$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 Open a File

$ label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
2 File to Open:

$ Title of the Include dialog
3 Include a File

$ Label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
4 File to Include:

$ Error when printing a file and a temporary copy can not be saved.
5 Unable to create a temporary file.


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
1 Save changes to %s?

$ This is used as-is, whenever the current file is unnamed.
2 The current file has no name.\nDo you want to save it?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***	Currently, dropped files are INSERTED in the text rather than
$		opened for editing so this message is no longer used and need
$		not be localized.
3 Do you want to save the current\ncopy of %s\nbefore you Edit %s?

$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.
5 The file %s already exists.\nDo you want to over write that file\nwith the contents within this edit session?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 Warning

$ Title of "Save As" dialog.
7 Save As

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***	These labels are no longer used and need not be localized.
$		They've been replaced by messages 44 and 45
8 Add newline characters to the end of word-wrapped lines.
9 Do not add newlines.  Only line breaks created by [Return]\nwill be preserved.

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 No
15 Yes
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 Save?

$ Title of general error dialog
17 Error

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
20 Information

$ Default file name in Save As dialog box.
21 UNTITLED

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 Unable to save %1$s.\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 Unable to save file.\nFile name was not specified.

34 File does not allow writing by anyone.

35 File is a directory.

36 File is a character special device.

37 File is a block mode device.

$ ***UNUSED***	This message should never appear since currently there is
$		only one supported file format - text.  And this type is
$		hard coded into dtpad.
38 Invalid file format.

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 Unknown reason - possibly not enough disk space.

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 Save changes?

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 Save changes to "%s"?

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***	This title is no longer used and need not be localized.
42 Copy As

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 Copy To File

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 Add newline characters to the end of wrap-to-fit lines.
45 Do not add newlines.  Only line breaks created by [Return]\nwill be preserved.
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This 
$ new message is in response to bug DTS CISlx19530.
$ 
46 Either file is unwritable or path to\nfile does not exist.

$ 
$ "-NEW"
$ 
47 "Operation failed due to insufficient memory.\nTry increasing swap space."

$ 
$ "-NEW"
$ 
48 "Format operation failed."

$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 This file has been opened for reading only.
2 This file does not exist.
3 Unable to edit a Directory.
4 Unable to edit a character special device file.
5 Unable to edit a block mode device file.
6 Embedded Nulls stripped from file.
7 Unable to load file (insufficient memory).
8 Unable to access file (can't determine it's status).
9 Unable to read from file (probably no read permission).

$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 View Only
11 Read Only
12 Unable to read from the file (unknown reason).


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
2 Text Editor - 

$ Default file name used in the main window title.
$ ***UNUSED***	This mesage has been replaced by message 6 of this set and
$		need not be localized.
3 (unnamed)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 Unable to initialize Desktop services ... exiting Text Editor.

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 Can not open display ... exiting Text Editor.

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (UNTITLED)

$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 ERROR: A dtpad server on %s\n\
is already serving this display.
8 ERROR: A dtpad server is already\n\
serving this display.
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' is a bad option.\n\
Usage: %s...\n\
   -saveOnClose\n\
\tDtpad automatically and silently saves the current text when there\n\
\tare unsaved changes and the Text Editor is closed.\n\n\
   -missingFileWarning\n\
\tPosts a warning dialog whenever a file name is specified and\n\
\tthe file dones not exist or cannot be accessed.\n\n\
   -noReadOnlyWarning\n\
\tDisables the warning dialog posted whenever a file is\n\
\tspecified for which the user does not have write permission.\n\n\
   -noNameChange\n\
\tIndicates that the default file name associated with the\n\
\tcurrent text is not to change when the text is saved under a\n\
\tname different than what it was read in under.\n\n\
   -viewOnly\n\
\tDisallows editing of text in the edit window, essentially\n\
\tturning the Text Editor into a Text Viewer.\n\n\
   -statusLine\n\
\tDisplays a status line at the bottom of the edit window.  The\n\
\tstatus line shows the line number of the line where the text\n\
\tcursor is currently positioned.\n\n\
   -wrapToFit\n\
\tInitially turns on wrap-to-fit mode.\n\n\
   -workspaceList WorkspaceList\n\
\tDisplays the edit window for the current invocation of the\n\
\tText Editor in the specified workspace or workspaces.\n\n\
   -session SessionFile\n

10 \tRestores the Text Editor to all text editing windows and\n\
\tsettings that were in effect at a previous CDE shutdown.  All\n\
\tother command-line options are ignored when this option is\n\
\tspecified.\n\n\
   -standAlone\n\
\tForces the current invocation of the Text Editor to do its\n\
\town text processing in its own window, independent of the\n\
\tText Editor server.\n\n\
   -noBlocking\n\
\tTerminates the Text Editor requestor process as soon as the\n\
\tText Editor server determines that it can handle the\n\
\trequestor's edit request.\n\n\
   -server\n\
\tForces a Text Editor server to be started up (if one is not\n\
\talready running) to process all subsequent edit requests for\n\
\tthe display.\n\n\
   -exitOnLastClose\n\
\tSpecifies that the Text Editor server process is to terminate\n\
\twhen the last edit window for the display is closed.  It\n\
\tshould only be used with the -server option since it only\n\
\tapplies to the server process.\n\n\
See the dtpad man page for more details on any of these options.\n\n

$ "NEW-"
11 "Exiting text editor ..."
$ "NEW-"
12 "Running in standalone mode ..."
$ "NEW-"
13 "Could not connect to ToolTalk service:\n%s\n%s\n"


$ 
$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 Check the file permissions.
2  %s is the file that would have been used to save your session.


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
2 Help

$ mnemonic and label for the "Overview" item
3 V
4 Overview...

$ mnemonic and label for the "Tasks" item
5 T
6 Tasks...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 Reference...

$ mnemonic and label for the "On Item" item
9 O
10 On Item

$ mnemonic and label for "Using Help" item
11 U
12 Using Help...

$ mnemonic and label for "About Text Editor" item
13 A
14 About Text Editor...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 File

$ mnemonic and label for the "New" item
17 N
18 New

$ mnemonic and label for the "Open" item
19 O
20 Open...

$ mnemonic and label for the "Include" item
21 I
22 Include...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 S
24 Save

$ mnemonic and label for the "Save As" item
25 A
26 Save As...

$ mnemonic and label for the "Print..." item
27 P
28 Print...

$ mnemonic and label for the "Close" item
29 C
30 Close

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 E
32 Edit

$ mnemonic and label for the "Undo" menu item
33 U
34 Undo

$ mnemonic and label for the "Cut" menu item
35 t
36 Cut

$ mnemonic and label for the "Copy" menu item
37 C
38 Copy

$ mnemonic and label for the "Paste" menu item
39 P
40 Paste

$ mnemonic and label for the "Clear" menu item
41 e
42 Clear

$ mnemonic and label for the "Delete" menu item
43 D
44 Delete

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
52 Format

$ mnemonic and label for the "Settings..." menu item
53 S
54 Settings...

$ mnemonic and label for the "Paragraph" menu item
55 P
56 Paragraph

$ mnemonic and label for the "All" menu item
57 l
58 All

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 O
60 Options

$ mnemonic and label for the "Overstrike" toggle button
61 O
62 Overstrike

$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***	This label and mnemonic are no longer used and need not be
$		localized.  They've been replaced by messages 111 and 112.
63 W
64 Word Wrap

$ mnemonic and label for the "Status Line" toggle button
65 S
66 Status Line

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** 	This nmemonic and label are no longer used and need not be
$		localized.  They've been replaced by messages 87 and 88.
71 C
72 Table of Contents...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 Save (needed)


$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 H
82 Help

$ mnemonic and label for the "Overview" item
83 v
84 Overview

$ mnemonic and label for the "Tasks" item
85 T
86 Tasks

$ nmemonic and label for the "Table of Contents" item
87 C
88 Table of Contents

$ mnemonic and label for the "Reference" item
89 R
90 Reference

$ mnemonic and label for the "On Item" item
91 O
92 On Item

$ mnemonic and label for "Using Help" item
93 U
94 Using Help

$ mnemonic and label for "About Text Editor" item
95 A
96 About Text Editor


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 S
102 Select All

$ mnemonic and label for the "Find/Change" menu item
103 F
104 Find/Change...

$ mnemonic and label for the Check Spelling menu item
105 k
106 Check Spelling...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***	This label is no longer used and need not be localized.
107 A
108 Copy As...

$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 F
110 Copy To File...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 W
112 Wrap To Fit

$ label for the "Print" item
113 Print

$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 You must select an item within the Text Editor.


$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 Checkpoint save failed.
2 Checkpoint reply failed.

$ NEW message after the SAMPLE IMPLEMENTATION.... more errors/warnings can 
$ be detected while reading in a file via ToolTalk.
3 Unable to set contents of Text Editor (unknown reason).


$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SETS 14 - 15 WERE ADDED TO SUPPORT CDEnext PRINTING
$ **
$ **	The following set was added to support CDEnext printing based on
$ **	the Xp Print Server.
$ **
$ ****************************************************************************
$ ****************************************************************************

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 14
$ **
$ ****************************************************************************
$ ****************************************************************************
$set 14
$ 
$	Messages 1 - 17 are used in printJob.
$ 
$	This is warning message is displayed when the user has provided
$	a margin specifier in the PrintSetup dialog which cannot be parsed.
$	
$quote "
1 "One of the following margin specifiers \n has incorrect syntax: \n %s \n %s \n %s \n %s \nUse default margins instead?"
$ 
$	Messages 2 - 7 are messages for errors which can occur in the
$	process of loading a dtpad document file for printing.	
$ 
2 "File does not exist."
3 "Specified file is a directory."
4 "File type error."
5 "File contains NULL characters."
6 "Unable to load file (insufficient memory)."
7 "File does not have read permissions"

$	Messages 8 - 11 are format strings used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 8 is the format string used to make a Date header/footer.
$	Message 9 is the format string used to make a DocName header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
8 "Date:  %s"
9 "Document:  %s"
10 "Page %d of %d"
11 "Document For:  %s"

$ 
$	Messages 12 - 13 are used in printSetup to construct the
$	app-specific printing options for DtMail which will be included
$	in the DtPrintSetupBox.
$ 
$	Message 12 is the string labeling the field in the GUI which
$	contains the name of the document.
$	Message 13 is the label used on the Toggle Button to indicate that
$	lines which would extend beyond the edge of the printed page be 
$	"wrapped" to the next line.
12 "Document Name: "
13 "Wrap To Fit"


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
$	NOTE: 'Print to File' should be translated as in 
$	      DtPrint.msg:Set1, Msg3.
14 "'Print to File' was unsuccessful."
15 "'Print to File' completed successfully."

$          Messages 16 - 19 are obsolete, and have been replaced by 25
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

$	Messages 20 - 22 are status messages displayed while printing
$ 
$	Message 20 indicates that the user has asked to close the editor
$		   window.  Dtpad is prevented from doing so until the
$		   active print jobs have completed.
$	Message 21 status message displayed when the print job is started.
$	Message 22 status message displayed as the print job progresses.
20 "Close pending:  waiting for task to terminate ..."
21 "Printing %s ..."
22 "Printing %s: page %d of %d ..."

$	Message 23 is used in the construction of a title string for the
$		   Print Setup dialog.
23 "Print Setup - "

$	Message 24 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
$ "NEW-"
24 "The X Print Server is temporarily out of resources"

$       Messages 25 is used to handle errors from the PDM
25	"Print Dialog Manager (PDM) error - setup failed."

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 15
$ **
$ ****************************************************************************
$ ****************************************************************************
$ 
$set 15
$ 
$	Messages 1-5 are used in printSetup.c in the pulldown menu for the
$	header/footer options in the print setup dialog.
$ 
$	Message 1 specifies that the page header/footer field be empty.
$	Message 2 put the Date in the page header/footer field.
$	Message 3 put the Document Name in the page header/footer field.
$	Message 4 put the Page Nmbr msg header in the page header/footer field.
$	Message 5 put the User Name msg header in the page header/footer field.
1 "Empty"
2 "Date"
3 "Document Name"
4 "Page Number"
5 "User Name"

$	Messages 6-9 are used to construct error messages informing users
$	that they have input an invalid print page margin specification for 
$ 
6 "Top Margin specifier is invalid:  "
7 "Right Margin specifier is invalid:  "
8 "Bottom Margin specifier is invalid:  "
9 "Left Margin specifier is invalid:  "
