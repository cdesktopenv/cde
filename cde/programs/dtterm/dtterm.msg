$ $TOG: dtterm.msg /main/4 1999/09/17 13:27:14 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	Sep             f.separator\n\
	Toggle\\ Menu\\ Bar  f.send_msg %d\
	
2 %s: the -S option can not be used with the -server or -serverid options\n
3 Terminal

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132				enable/disable 80<->132 column escape seq
2 -/+aw				enable/disable autowrap
3 -bg color			background color
4 -/+bs				turn off/on Term background is select color
5 -display displayname		X server to contact
6 -e command args		command to execute
7 -fb fontset			bold text font
8 -fg color			foreground color
9 -fn fontset			normal text font
10 -geometry geom			size (in characters) and position of window
11 -help				print out this message
12 -/+iconic			start/do not start iconic
13 -/+j				enable/disable jump scroll
14 -/+kshMode			enable/disable ksh mode
15 -/+l				enable/disable logging
16 -lf filename			logging filename
17 -/+ls				enable/disable login shell
18 -/+map				enable/disable map window on pty output
19 -/+mb				enable/disable margin bell
20 -ms color			pointer color
21 -n string			specify icon name
22 -name string			client instance, icon, and title strings
23 -nb distance			specify distance for right margin bell
24 -/+rw				enable/disable reverse wrap
25 -/+sb				enable/disable scroll bar
26 -/+sf				enable/disable SUN function keys
27 -sl number[s]			number of scrolled lines [screens] to save
28 -ti name			string used for programmatic identification
29 -title string			title string for window
30 -tm string			terminal mode keywords and characters
31 -tn name			TERM environment variable name
32 -/+vb				enable/disable visual bell
33 -xrm resourcestring		additional resource specifications
34 -C				console mode
35 -Sxxd				slave mode on "ttyxx" file descriptor "d"
36 -Sxxx.d			slave mode on "ttyxxx" file descriptor "d"
37 End-Of-List
38 The -e option, if given must appear at the end of the command line,
39 otherwise the user's default shell will be started.  Options that start
40 with a plus sign (+) restore the default.
41 End-Of-List
47 %s:  bad command line option "%s"\r\n\n
48 usage:  %s
49 \r\n   
50  [%s]
51 \r\n\nType "%s -help" for a full description.\r\n\n
52 usage:\n
53 \t%s [-options ...] [-e command args]\n\n
54 where options include:\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 MEMORY FULL\nPress OK to clear

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
2 Terminal - Warning

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 MEMORY FULL\nPress OK to clear

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
4 Terminal - Man Page
5 Terminal - Help

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
2 OK
3 Cancel
4 Help
5 global
6 Cursor Control
7 Box
8 Underline
9 Cursor Style
10 Enabled
11 Disabled
12 Blinking Cursor
13 Blink Rate (milliseconds)
14 Invisible
15 blinkRate
16 Color Control
17 Inverse
18 Normal
19 Window Background
20 Scroll Behavior
21 Disabled
22 Enabled
23 Smooth Scrolling
24 Bell Control
25 Audible
26 Visible
27 Bell Type 
28 Enabled
29 Disabled
30 Margin Warning
31 Margin Distance 
32 Apply
33 Reset
34 Terminal - Global Options
$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 Default
2 Window Size
3 W
4 -iso8859-1
5 %d point
6 %.1f point
7 %.1f point
8 %.2f point
9 Default
10 Font Size
11 F

$   Window menu
12 New
13 N
14 Print
15 P
16 Print...
17 r
18 Close
19 C
20 Window
21 W

$   Edit menu
22 Copy
23 C
$ IBM fixed msg 24 for 41J
$ 24 Ctrl osfInsert
24 Ctrl<Key>osfInsert
25 Ctrl+Insert
26 Paste
27 P
$ IBM fixed msg 28 for 41J
$ 28 Shift osfInsert
28 Shift<Key>osfInsert
29 Shift+Insert
30 Edit
31 E

$   Option menu

32 Menu Bar
33 M
34 Scroll Bar
35 S
36 Global...
37 G
38 Terminal...
39 T
40 Soft Reset
41 S
42 Hard Reset
43 H
44 Reset
45 R
46 Options
47 O

$ Old Help menu (no longer used for CDE)

48 Overview
49 O
50 Tasks
51 T
52 Reference
53 R
54 On Item
55 O
56 Using Help
57 U
59 A
60 Help
61 H

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 Overview
65 v
66 Index
67 I
68 Table Of Contents
69 C
70 Tasks
71 T
72 Reference
73 R
74 Keyboard
75 K
76 Using Help
77 U
79 A
80 Help
81 H
82 About Terminal
83 About Terminal

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
2 OK
3 Cancel
4 Help
5 Keyboard Control
6 Normal
7 Application
8 Cursor Key Mode
9 Numeric
10 Application
11 Keypad Mode
12 Return Only
13 Return/Line Feed
14 Newline Sequence
15 Locked
16 Unlocked
17 User Function Keys
18 Screen Control
19 Enabled
20 Disabled
21 132 Column Switching
22 End-of-line Wrapping
23 Reverse End-of-line Wrapping
24 Apply
25 Reset
26 Terminal - Terminal Options

$set 10
$ ****** Module: sunDtTermServer.c
$ These are the messages for the ToolTalk error dialog
1 Terminal - Error
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
3 OK
