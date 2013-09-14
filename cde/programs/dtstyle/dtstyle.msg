$ "$XConsortium: dtstyle.msg /main/6 1996/08/16 17:02:45 pascale $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtstyle.msg
$ **
$ **  Project:     DT Style Manager
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtstyle
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1996 Digital Equipment Corporation.
$ ** (c) Copyright 1990, 1996 Hewlett-Packard Company.
$ ** (c) Copyright 1996 International Business Machines Corp.
$ ** (c) Copyright 1996 Sun Microsystems, Inc.
$ ** (c) Copyright 1996 Novell, Inc. 
$ ** (c) Copyright 1996 FUJITSU LIMITED.
$ ** (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.  
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$ Message 1 from set 2 was deleted from the message catalog

$ _DtMessage 2 is the title of all Information dialog boxes
2 Notice
$ _DtMessage 3 is the title of all Error dialog boxes
3 Error
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 Cannot communicate with the session\n\
manager... Exiting.

$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 Style Manager is already running,\n\
second attempt aborted.
6 An X Toolkit error occurred...\n\
Exiting.

$ _DtMessage 7 is the title of all Help dialog boxes
7 Style Manager - Help

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 You must select an item\n\
within the Style Manager.

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 Could not obtain screen saver information\n\
from the session manager. Start up settings\n\
may be incorrect.


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 Style Manager

$ mnemonic and label for the File menubar item.
2 F
3 File
$ mnemonic and label for the Help menubar item.
4 H
5 Help

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 Exit

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 Color
28 Font
29 Backdrop
30 Keyboard
31 Mouse
32 Beep
33 Screen
35 Startup

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 No Dtstyle restore information available for current session

$ _DtMessage 37 is the label on the dtstyle icon
37 Style 

$ Message 38 is the label on the Window button in the dtstyle main window.
38 Window

$ _DtMessage 40 is replaced with message 53
$ 40 E

$ *
$ * mnemonic and label for the Help menu items.
$ * 

$ Messages 41-42 have been replaced with messages 54-55
$ Overview... menu item
$ 41 I
$ 42 Introduction...

$ Tasks... menu item 
43 T
$ Message 44 has been replaced with messages 56
$ 44 Tasks...

$ Reference... menu item
$ 45 R
$ Message 46 has been replaced with messages 57
$ 46 Reference...

$ On Item menu item
$ Keybaord menu item
47 O
48 On Item

$ Using Help... menu item
49 U
$ Message 50 has been replaced with messages 58
$ 50 Using Help... 

$ Messages 51-52 have been replaced with messages 59-60
$ Overview menu item
$ 51 V
$ 52 Version...

$ Message 53 is replaced with message 40
53 x

$ Messages 54-55 are replacements for messages 41-42
54 v
55 Overview

$ Message 56 is a replacement for messages 44
56 Tasks

$ Message 57 is a replacement for messages 46
57 Reference

$ Message 58 is a replacement for messages 50
58 Using Help

$ Messages 59-60 are replacements for messages 51-52
59 A
60 About Style Manager

$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 Preview

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 The changes to fonts will show up in some\n\
applications the next time they are started.\n\
Other applications, such as file manager and\n\
application manager, will not show the font\n\
changes until you Exit the desktop and then log\n\
back in.

$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 The selected font will be used\n\
when you restart this session.

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 AaBbCcDdEeFfGg0123456789
21 AaBbCcDdEeFfGg0123456789

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 Size

$ _DtMessage 23 is the title of the Fonts dialog
23 Style Manager - Font

$ _DtMessage 24 is an informative message used by the Fonts dialog
24 The font that is currently used for your desktop is not\n\
available in the Size list. If a new font is selected and\n\
applied, you will not be able to return to the current font\n\
using the Style Manager - Font dialog.

$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 Default
3 Volume
4 Tone
5 Duration

$ _DtMessage 7 is the title of the Beep dialog box
7 Style Manager - Beep  


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 At Login:
4 Resume current session
5 Return to Home session

$ Message 6 is obsolete - DO NOT localize message #6 from this set
$ 6 Ask me at Logout

7 Logout Confirmation Dialog:
8 On
9 Off
10 Set Home Session ...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 This will replace your home session with\n\
the current session.\n\n \
                 Continue?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 Warning

$ _DtMessage 13 is an error message
13 This session was started from an X Window System\n\
startup script.  Startup settings are not valid,\n\
since the session cannot be restored.

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 Style Manager - Startup

$ _DtMessage 15 is an error message
15 The Session Manager is not running.\n\
Startup settings are not valid.

$ _DtMessage 16 is used in the Sessin Startup dialog.  It is the
$   string that appears to right of the Startup dialog icon IF
$   the user selects a display-specfic session at the login screen.
16 Display-specific session:


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
1 Left
2 Right
3 Handedness:
4 Button 2:
$ _DtMessage 5 is the label of default button.
5 Default

$ Messages 6,8,9 are labels of the scales.
6 Double-Click
8 Acceleration
9 Threshold

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 ERROR: setting pointer mapping\n

11 Transfer
12 Adjust

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 The right and left mouse buttons will swap\n\
functions immediately after you click on OK.\n\
The `Test Double-Click' graphic in the Mouse\n\
window shows the current handedness by the \n\
positions of Button 1 and Button 3.

$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 If you click on OK, mouse button 2 will be set\n\
to `transfer' (drag) at your next session.

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 The new double-click time will take effect\n\
as applications are restarted.

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 The new double-click time will take effect\n\
at your next session.

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 Test\nDouble-Click

$ _DtMessage 18 is the Mouse Dialog title
18 Style Manager - Mouse

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 If you click on OK, mouse button 2 will be set\n\
to `adjust' at your next session. Button 2 may\n\
be used to adjust text and list selections.\n\
Transfer actions (drag) on text and lists will\n\
require the use of Button 1.

$ _DtMessage 20 is the label used for the "Button 2" toggle on
$ a two button mouse (ignored on a three button mouse)
20 Menu

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 Check file permissions.
3 %s is the file that would have been used to save your session\n


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 Could not open directory %s. 
$ _DtMessage 2 - error opening the backdrop files
2 The backdrop portion of the Style Manager\n\
will not operate because there are no backdrop\n\
files available. Check $HOME/.dt/errorlog.


$ _DtMessage 12 is the title of the Backdrop dialog box
12 Style Manager - Backdrop


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box. 
1 Style Manager - Screen

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 Default
4 Screen Saver
5 Start Saver
6 Time Per Saver
7 minutes
8 Screen Lock
$ Message 9 has been removed 
10 On
11 Off
12 Screen Saver:
13 Screen Lock:
14 Start Lock
15 Auto Screen Blanking
16 Screen Blanker:
17 Start Blanking
18 Front Panel Lock
$ Message 19 has been replaced by message 21
19 Use Lock Backgrounds
20 Time Per Background
$ Message 21 is a replacement for message 19
21 Use Backgrounds For Lock
22 Transparent Lock

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 Default
3 Auto Repeat
4 Click Volume

$ _DtMessage 6 is the title of the Keyboard dialog box
6 Style Manager - Keyboard


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 Palettes
6 Add...
7 Delete...
8 Modify...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 New palette name:

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 Add Palette

$ Messages 12 through 14 are error messages.
12 The palette name cannot contain\n\
these characters:\n\n\
<space> * : ( ) [ ] { } < > ! | " / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE 
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 The palette name must be\n10 characters or less.\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 Delete Palette

$ _DtMessage 18 appears in an information dialog.
18 Can't delete the last palette.\n

$ _DtMessage 21 is an error message
21 Warning

$ _DtMessage 23 appears in an information dialog
23 Delete palette '%s'?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 The color portion of the Style Manager\n\
will not operate because the color server\n\
is not running.  Check $HOME/.dt/errorlog.

$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 The color portion of the Style Manager\n\
will not operate because the resource\n\
'useColorObj' is set to False.

$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 The new Color Use value will take effect\n\
at your next session.

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 The selected palette will take effect\n\
at your next session.

$ _DtMessage 29 appears in a warning dialog
29 A palette named '%s' aleady exists.\n\
This new palette will overwrite the old one.\n\
Is this what you want to do?

$ _DtMessage 30 is the title of the Color dialog box.
30 Style Manager - Color

$ _DtMessage 31-33 are the ColorUse options
$ 31 High Color
$ 32 Medium Color
$ 33 Low Color
34 Black and White
35 Default

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT Color Use

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog 
$ 37 DT Color Use...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 The color portion of the Style Manager\n\
will not operate because there are no palette\n\
files available.  Check $HOME/.dt/errorlog.

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39 Number Of Colors To Use:

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 Number Of Colors...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 More Colors for Desktop
42 More Colors for Applications
43 Most Colors for Applications

$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 Unable to delete '%s'.\n
12 Check $HOME/.dt/errorlog for hints.

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 Warning, Too many directories listed in the resource paletteDirectories,\n\
Maximum number is %d. 
$ _DtMessage 19 - could not open a palette directory
19 Could not open directory %s. 
$ _DtMessage 20 - error opening the palette file
20 Could not open %s. 
$ _DtMessage 21 - error reading a palette file
21 %s is an invalid palette file.


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 Color Sample
5 Old
6 New
7 Grab Color
8 Color Editor
9 Hue
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 R
11 G
12 B

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 Warning, couldn't grab pointer.\n
15 Warning, couldn't grab Keyboard.\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 Style Manager - Modify Color


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 Style Manager - Window

$ Messages 2 through 5 are labels on the Window dialog.
2 Default
3 Window Behavior
4 Window Icons
$ Messages 5-8 have been replaced by messages 13-16 
$ 5 Focus Follows Mouse
$ 6 Click In Window For Focus
$ 7 Raise Window With Focus
$ 8 Opaque Move
9 Use Icon Box
10 Place On Workspace
11 Allow Primary Windows On Top
12 The workspace manager must be restarted in\n\
order for your changes to take effect.\n\
Press OK to restart the workspace manager\n\
or Cancel to reset the changes.
$ Messages 13-16 are replacements for messages 5-8
13 Point In Window To Make Active
14 Click In Window To Make Active
15 Raise Window When Made Active
16 Show Contents During Move

$set 19
$ The set 19 contains messages for the I18N dialog

$ 
$ Messsages 1 to 11 are labels displayed in the dialog.
$ 

$ Message 1 is the title of the Dialog.
1 Style Manager - Internationalization

$ Message 2 appears on the button used to reset the last saved values.
2 Default

$ Message 3 appears as the title of the title box containing Input
$ Methods information.
3 Input Method

$ Message 4 appears as the title of the title box containing Input
$ Method Start Mode choice.
4 Input Method Start Mode

$ Message 5 appears as the title of the title box containing PreeditType
$ information.
5 Preedit Type

$ Message 6 is the label presenting the server host names
6 Server Host :

$ Message 7 is the label presenting the Input Method list
7 Input Methods

$ Message 8 and 9 are the toggle buttons for the start mode selection:
$ "Ask at login" will cause dtimsstart to present the list of Input
$ Methods at next login; "Resume Current Input Method" will cause
$ dtimsstart to use the selected IM at next login, without asking the
$ user.
8 Ask at login
9 Resume Current Input Method

$ Message 10 is the label displayed on the button used to move up a
$ selected element in the preedit Type list (meaning at the previous
$ position).
10 Move Up

$ Message 11 is the label displayed on the button used to move down a
$ selected element in the preedit Type list (meaning at the next
$ position).
11 Move Down

$ 
$ Messages 20-21 are displayed in an warning dialog when the user
$ hits the OK button.
$ 

$ Message 20 appears when the user modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is True.
$ Do NOT localize the word XmNpreeditType.
20 The new XmNpreeditType value will take effect\n\
as applications are restarted.

$ Message 21 appears when the userselects a new Input Method Server,
$ or modifies the IMS Start Mode, or modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is False.
21 Your selection will take effect\n\
at your next session.

$ 
$ Messages 30 to 41 appear as errors in Error Dialog.
$ 

$ Do NOT localize the word HOME.
30 The environment variable HOME is not defined.

$ Do NOT localize the word LANG.
31 The environment variable LANG is not defined.

32 The desktop does not support this locale.
33 Cannot create the user Input Method Selection file.
34 Cannot find the user Input Method Selection file.
35 Cannot save the user Input Method Selection file.
36 Unknown host

$ Do NOT translate dtimsstart.
37 dtimsstart was not found.\n\
Perhaps the desktop environment\n\
is not properly installed.

38 Remote execution failed on\n the selected host.
39 Cannot initialize remote execution.
40 Unable to get Input Method information from the selected host.
41 Unknown error


$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/6 1996/08/16 17:02:45 pascale $"

