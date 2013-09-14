$ 
$   COMPONENT_NAME: desktop
$ 
$   FUNCTIONS: mnemonic
$		name
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
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelp.msg
$ **
$ **  Project:     CDE 1.0 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for DtHelp 
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in dialogs or are displayed to the user.
$ **        These messages are the default and they should ALL BE LOCALIZED.
$ **        Note that these messages do NOT have any identification.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the 
$ *               27th.  (Mike Wilson).
$ *
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****
$ 
$ These Messages appear in the pulldown menus for Cde Help General Help
$ dialog widget.
$ 
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the
$ second is the mnemonic (e.g. 2 P).  
$ 
$ Messages 1 through 6 are the pulldown menus for the File menu.
1 Print...
2 P
3 Close
4 C
5 New Window...
6 W
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 Copy
8 C 
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 Index...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 Home Topic
12 T
13 Backtrack
14 B
15 History...
16 H
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 Overview - Using Help
18 v
19 Table Of Contents
20 C
21 Tasks
22 T
23 Reference
24 R
25 Mouse and Keyboard
26 M
27 About Help
28 A
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the 
$ Help Dialog.
29 File
30 F
31 Search
32 S
33 Navigate
34 N
35 Help
36 H
37 Edit
38 E
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ help dialog and quick help dialog widgets.
$ 
50 String data could not be formatted.
51 Dynamic string data could not be formatted.
52 Man Page could not be formatted. The requested Man Page is either not \
present, or corrupt.
53 Text file data could not be formatted. The requested text file is either \
not present, or corrupt.
54 Help topic could not be formatted.
55 Nonexistent location ID:
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 The selected Hypertext link is not supported within this application.
57 Links to Man Pages are not supported by this application.
$ 
$ Messages 58 through 62  are used when bad or null help volumes or 
$ locationId's are used.
$ 
58 The requested online help is either not installed or not in the proper help search path.  For information on installing online help, consult the documentation for the product.\n
59 No location ID specified.
60 No help volume specified.
61 Help Volume:
62 Location ID:
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 Nonexistent Topic Title.
$ 
$ New messages added after early msg cat freeze; not in order
80 The specified help type is invalid.



$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 Help - History Browser
2 Help Volumes Visited:
3 Close
4 Help
5 Help Files
6 Man Pages
7 Help Messages
8 Help Topics Visited:
9 Help Files Visited:
10 Man Pages Visited:
11 Help Messages Visited:


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 Help - Print
$ 10-12: standard printer dialog labels
10 Printer:
11 Copies:
12 Banner Page Title:
$ 20-25: paper size label and sizes
20 Paper Size:
21 Letter 8.5 x 11 in
22 Legal 8.5 x 14 in
23 Executive 7.25 x 10.5 in
24 A4 210 x 297 mm
25 B5 182 x 257 mm
$ 30-33: help volume-specific printing options
30 Topics To Print
31 Current Topic
32 Current and Subtopics
33 Table of Contents and Index
34 All Topics in Volume

$ 40-42: buttons at bottom of dialog
40 Print
41 Cancel
42 Help

$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 Help Volume: 
51 Help Message
52 Help File

$ 53 "Manual Page: "
53 Manual Page: 
54 Error message

$ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$ 60 " "
$quote "
60 ""
$quote

$ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5 
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 Help - Index Search
2 Search
3 Current
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4  (%s)
5 All Volumes
6 Selected
7 Select Volumes...
8 Show
9 Complete Index
10 Entries with:
11 Goto
12 Start Search
13 Continue Search
14 Stop Search
15 Close
16 Help
17 Scanning for volumes... Found %d
18 Searching... Volumes remaining: %d
19 No index entries found
20 Complete Index
21 Entries with "%s"
22 No volumes selected
23 Select search options above
$ Do NOT translate messages 25-31
$quote "
$ " "
25 " "
26 "+"
27 "-"
$ "  +"
28 "  +"
$ "  -"
29 "  -"
$ 30 "       "
30 "       "
$ 31 "         "
31 "         "
$quote
$ DO translate messages 32 and above
$ kind of topic messages
32 Help Message
33 Help File
34 Manual Page
35 Unknown Format
$ specialized title of Search Volume Selection dialog
40 Help - Search Volume Selection
$ additional messages (late addition caused this ordering)
50 No index entries found.  Try All Volumes.
51 No index entries found.  Try another word.
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 Help On Help 
2 Print ...
3 Close
4 Help On Help not provided for this application!
5 Backtrack
6 Help ...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be 
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 Usage: %s...\n
2 \t-helpVolume <Help Volume File>\n
3 \t-locationId  <ID>\n
5 \t-file  <ASCII Text File>\n
6 \t-man\n
7 \t-manPage <Unix Man Page>\n\n\n
$ man page dialog
11 Man Page
12 Man Page:
13 Show Man Page
$ button labels
16 Exit
17 Close
18 Backtrack
19 Print ...
$ title of helpview window
20 Help Viewer


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box 
$ (e.g. Quick help close button)
2 Close
3 Internal Error: Could not grab the mouse\nDtHelpReturnSelectedWidget aborted.\n
4 Internal Error: Could not grab the keyboard\nDtHelpReturnSelectedWidget() aborted.\n
5 Help Error 
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 Execute Anyway
11 Don't Execute
12 The help volume wanted to execute a command alias.\nThe alias '%s' is not defined.
13 The help volume wanted to execute a command.\nFor security reasons, automatic command execution is turned off.\nThe command is:  %s
14 The help volume wanted to execute a command as the root user, but the\nhome directory of  "%s"  ($HOME) does not match the root\nuser's home directory.  This could result in executing unexpected\ncommands.\n\nThe command is:  "%s"\n\nNote:  to avoid this in the future:\n execute "su - root"  rather than "su root".\n
$ End Message set 8


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area  
1 Volume: 
$ Messages 2 though 5 are used in the Button Box area of our general 
$ help dialog.
2 Backtrack
3 History...
4 Index...
5 Top Level


$set 10 
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 Help - Volume Selection
2 Help Volumes
3 Select one or more help volumes
4 Close
5 Help


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 Close
2 More ...
3 Backtrack
4 Print ...
5 Help ...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $XConsortium: DtHelp.msg /main/3 1995/10/26 12:19:02 rswiston $"

