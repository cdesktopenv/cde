
$ "$XConsortium: dtwm.msg /main/4 1995/12/08 09:39:59 rswiston $";

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtwm.msg
$ **
$ **  Project:     DT Workspace Manager (dtwm)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtwm
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1990, 1992, 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1994 International Business Machines Corp.
$ ** (c) Copyright 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1994 Novell, Inc.
$ **
$ **     All Rights reserved
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
$ * You don't need to localize the following sets:
$ *  sets 6, 8, 26, 28, 30, 32, 34, 38 
$ *  sets 44, 46, 48, 54, 56, 70, 76, 99
$ *
$ * Please localize the following sets:
$ *  sets 16, 22, 29, 36, 40, 52, 60, 62, 82, 84, 86.  
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 6
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmBackdrop.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Failed to create backdrop window.

3 Unable to get image %1$s for workspace %2$s.
4 Insufficient memory for backdrop window.


$set 8
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmCDecor.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for client window framing
2 Insufficient memory for client window framing
3 Insufficient memory for client window framing
4 Insufficient memory for client window framing
5 Insufficient memory for client window framing
6 Insufficient memory for client window framing
7 Insufficient memory for client window framing



$set 20
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmError.c --

$    This message goes to an error file. It is the name of component
$    reporting the error.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
1 Workspace Manager


$set 22
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmFeedback.c --

$    This is the message in the confirm dialog for toggling to and from 
$         PM (Presentation Manager) behavior 

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
2 Insufficient memory for local message string

$    This is the message in the confirm dialog for restarting mwm
$ Do not localize "Mwm"
3 Vill du starta om Mwm?

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
5 Insufficient memory for local message string

$    This is the message in the confirm dialog for quitting mwm
$ Do not localize "Mwm"
6 Vill du avsluta Mwm?

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
8 Insufficient memory for local message string

$    This is the message in the confirm dialog for logging out of DT Lite
$  You'll see it when pressing the Exit button from DT lite
9 Vill du logga ut?

$    This is the message in the confirm dialog for restarting dtwm
10 Vill du starta om arbetsytehanteraren?

$    This is the message in the confirm dialog for quitting dtwm
11 Vill du avsluta arbetsytehanteraren?

$    This is the message in the confirm dialog for toggling to standard
$         PM (Presentation Manager) behavior from custom behavior
12 Vill du växla till standardmetod?

$    This is the message in the confirm dialog for toggling back from
$         PM (Presentation Manager) behavior to custom behavior
13 Vill du växla till anpassad metod?


$set 26
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmFunction.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 The window manager restart failed. The window manager program could \
not be found or could not be executed.
2 Failed to unmanage screen %d
3 Insufficient memory to restart the window manager
4 Invalid workspace name specified for f.goto_workspace

$set 28
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmGraphics.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for graphics data


$set 29
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmHelp.c --
$    Messages 1 and 2 go to an error file.
$ YOU DON'T NEED TO LOCALIZE MESSAGES 1-2
1 Help on Item failed. Another program may have grabbed the mouse.
2 Help on Item failed. Another program may have grabbed the mouse.

$ Message 3 is an error message that appears when you are using Item Help
$ and select on something outside of the Workspace Manager
3 Du måste markera en dialogruta i\n\
arbetsytehanteraren, ett objekt i frontpanelen\n\
eller ett objekt i en panelundermeny.

$ Message 4 is the title of the Help On Item Error dialog
4 Arbetsytehanterare - Fel i objekthjälp

$set 30
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmIDecor.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Could not make icon to go in icon box
2 Insufficient memory for icon creation
3 Insufficient memory for icon creation


$set 32
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmIPC.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Unable to connect with the message server.
2 Insufficient memory to handle backdrop change.
3 Missing path name for backdrop change request.
4 Missing pixmap id for backdrop change request.


$set 34
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmIPlace.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for icon placement
2 Insufficient memory for icon placement
3 Insufficient memory for icon placement
4 Insufficient memory to create icon box data


$set 36
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmIconBox.c --

$    Messages 1 & 2 go to an error file.
$ YOU DON'T NEED TO LOCALIZE MESSAGES 1 & 2.
1 Insufficient memory to create icon box data
2 Insufficient memory to create icon box data

$    This is the last menu item in the icon box menu.  
$        This packs the icons together in the icon box. Only the
$        "PackIcons" and the "P" should be localized unless the
$        accelerator interferes with the local keyboard.  
$        The accelerator is OSF standard for this item and should only be
$        changed if this accelerator would lock out a local key use.
$ 
$        The underbar
$        is part of the syntax for mnemonics and should not be changed.
$        The letter after the underbar should match a letter (case sensitive)
$        in the item title .
$        (e.g.   CompressIcons _C Alt Shift<Key>F7 f.pack_icons\n  
$          OR
$                CompressIcons _I Alt Shift<Key>F7 f.pack_icons\n  )         
$ 
$   The mnemonic should also be chosen such such that it does not 
$        conflict with another mnemonic in the menu.

3 "Ordna ikoner" _P  Alt Skift<Key>F7 f.pack_icons\n

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
4 Insufficient memory to create icon box data


$set 38
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmImage.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Invalid icon bitmap
2 Invalid icon bitmap
3 Insufficient memory to bevel icon image
4 Insufficient memory to bevel icon image
5 Insufficient memory for bitmap %s\n
6 Insufficient memory for bitmap %s\n
7 Unable to read bitmap file %s\n
8 Invalid bitmap file %s\n
9 Icon bitmap cannot be used on this screen
10 Color icon pixmap not supported
11 %s is not in a valid icon file format.\n\
    Please examine the file and compare it to other icon files.

$set 40
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmInitWs.c --

$    Messages 1-13 and 15,17 and 19-21 go to an error file.
$ DO NOT LOCALIZE MESSAGES 1 THROUGH 13.
1 Could not open display.
2 Insufficient memory for Screen data
3 Another window manager is running on screen %d
4 Could not open second display connection.
5 Unable to manage any screens on display.
6 Cannot configure X connection
7 Cannot create enough memory for the screen names
8 Insufficient memory for Workspace data
9 Insufficient memory for displayString
10 Insufficient memory for window manager data
11 Insufficient memory for window manager data
12 Insufficient memory for screen names
13 Insufficient memory for screen names

$     This is the default title for the icon box
14 Ikoner

$    Message 15 goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
15 Insufficient memory for local message string

$    Message 17 goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
17 Insufficient memory for local message string

$    Message 19 goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
19 Insufficient memory for local message string

$    Message 20 goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
20 %1$s: %2$s on line %3$d of configuration file %4$s\n

$    Message 21 goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE
21 %1$s: %2$s on line %3$d of specification string\n

$     This is the default title for the version dialog
22 Om Arbetsytehanterare

$     This is the default title for the Dtwm help dialog
23 Arbetsytehanterare - Hjälp

$     This is the default title for the Generic help dialog
24 Arbetsytehanterare - Hjälp


$set 44
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmMain.c --
$    This entire set is written to an error file
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 
2 Insufficient memory for name of window manager


$set 46
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmManage.c --
$    This entire set is written to an error file
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory to adopt initial clients


$set 48
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmMenu.c --
$    This entire set is written to an error file
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Menu specification %s not found\n
2 Insufficient memory for menu %s\n
3 Insufficient memory for menu %s\n
4 Menu specification %s not found\n
5 Menu recursion detected for %s\n
6 Insufficient memory for menu %s\n
7 Insufficient memory for menu %s\n
8 Menu specification %s not found, using internal menu\n


$set 52
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmPresence.c --
$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
1 Unable to create Occupy Workspace dialog.

$    This tells what window the occupy workspace dialog is operating on.
2 Fönster: 

$    This is the label for the workspace list in the occupy workspace dialog
3 Arbetsytor: 

$    This is the label for the button to put a window in all workspaces
$        from the occupy workspace dialog
4 Alla arbetsytor

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
5 Insufficient memory to create occupy workspace dialog.

$    This tells what window the occupy workspace dialog is operating on.
6 Ikon:




$set 54
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmProperty.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for window management data
2 Insufficient memory for window management data
3 Insufficient memory for window management data
4 Insufficient memory for window manager flags


$set 56
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmProtocol.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for window manager data
2 Insufficient memory to XInternAtom _MOTIF_WM_QUERY_nn
3 Failed to own _MOTIF_WM_QUERY_nn selection
4 Lost _MOTIF_WM_QUERY_nn selection
5 Insufficient memory to convert _MOTIF_WM_QUERY_nn selection


$set 60
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmResParse.c --
$    Messages 1-39 are written to an error file.
$    YOU DO NOT NEED TO LOCALIZE MESSAGES 1-39, 41.
$ 
$    Message 40 allows you to alter the labels for the modifier
$    keys on the menu accelerators. You may wish to localize
$    message 40 so that these labels match the keycaps on your
$    local keyboard.
$ 
1 Insufficient memory for session geometry item
2 Insufficient memory for workspaces list in sesssion item
3 Insufficient memory for commandArgv array
4 Insufficient memory for commandArgv item
5 Insufficient memory for Dt Session Hints
6 Cannot open configuration file
7 Insufficient memory for menu accelerators
8 Unable to create a front panel by the name %s\n
9 Insufficient memory for menu
10 Insufficient memory for menu
11 Expected '{' after menu name
12 Insufficient memory for menu item
13 Insufficient memory for menu item
14 Invalid mnemonic specification
15 Insufficient memory for accelerator specification
16 Invalid accelerator specification
17 Insufficient memory
18 Missing group specification
19 Invalid group specification
20 Invalid number specification
21 Expected '{' after button set name
22 Insufficient memory for button specification
23 Invalid button specification
24 Invalid button context
25 Expected '{' after key set name
26 Insufficient memory for key specification
27 Invalid key specification
28 Invalid key context

31 Insufficient memory for screen names
32 Insufficient memory for screen names

33 %1$s: %2$s on line %3$d of configuration file %4$s\n
34 %1$s: %2$s on line %3$d of specification string\n

35 Insufficient memory for config file conversion
36 Insufficient memory to process included file: %s
37 Expected '{' 
38 Insufficient memory for host name in sesssion item
39 Could not reopen configuration file %s

$ Message 40 is used to remap the menu accelerator
$ modifier key labels to the keycaps that the user sees on the
$ local keyboard. If modifier key labels are not remapped, then
$ the key label used in the accelerator spec will appear on
$ the menu
$ 
$ Example: menu spec in dtwmrc file
$ 	 "Close"                 _C   Alt<Key>F4      f.kill
$ 
$ Appears as
$         Close       Alt+F4
$         -                  (the 'C' in Close is underlined)
$ 
$ if message 40 doesn't remap "Alt" then accelerator text is "Alt+F4"
$ if message 40 is "Alt XYZ" then accelerator text is "XYZ+F4"
$ 
$ The format of this message is a sequence of strings separated
$ by spaces. The strings are taken in pairs. If the first string
$ in a pair matches a accelerator key label, the second string
$ appears in its place on the menu. There must be an even number 
$ of strings. The case of the first string does not matter. The 
$ case of the second string is preserved.
$ 
40 
$ 
$    Messages 41 is written to an error file.
$    YOU DO NOT NEED TO LOCALIZE MESSAGE 41.
$ 
41 Insufficient memory to get LANG environment variable.


$set 62
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmResource.c --

$    Messages 1, 3, 5, 7, 9, 11, 13, 15, 17 and 19 are menu items in the 
$         built in menu. Only the title and mnemonic should be localized,
$         never change the underbar.  For example in message 1,
$        "Restore" and the "R" may be localized.  The underbar
$        is part of the syntax for mnemonics and should not be changed.
$        The letter after the underbar should match a letter (case sensitive)
$        in the item title.  
$        (e.g.   Normalize  _N  )

$ Messages 1, 3, 5, 7, 9, 11, 13, 15, 17 and 19 are not used.
$ Messages 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 to 29, 31, 33 to 38, 66-68,
$          go to an errorlog and do NOT need to be localized.
$ Messages 30 and 39-47 are not used.
$ Only messages 32, 48-57 and 59 need to be localized.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
2 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
4 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
6 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
8 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
10 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
12 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
14 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
16 Insufficient memory for local default menu.

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
18 Insufficient memory for local default menu.

$    Messages 20 to 29 go to an error file.
$ YOU DON'T NEED TO LOCALIZE MESSAGES 20 TO 29.
20 Insufficient memory for local default menu.
21 Insufficient memory for localized default system menu
22 Insufficient memory for color data
23 failed to load font: %.100s
24 Insufficient memory for workspace data
25 Invalid workspace name: "%s"
26 Duplicate workspace name: "%s"
27 Insufficient memory for workspace data
28 Insufficient memory for date resource
29 failed to load font: %.100s

$    This message goes to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS MESSAGE.
31 Insufficient memory for local message string

$    This is the default title of the "Rename Workspace" dialog
32 Byt namn på arbetsyta

$    Messages 33 to 38 go to an error file.
$ YOU DON'T NEED TO LOCALIZE MESSAGES 33 TO 38.
33 Insufficient memory for local message string
34 Unable to get color
35 Unable to get color
36 Unable to get color
37 Insufficient memory for window manager data
38 Insufficient memory for window manager data

$ Messages for the built in menu.  The built in menu will 
$ appear unless the user (through resources and app-defaults) 
$ specified a custom window menu.

$ Messages 48-57 are the definitions for the default built in
$ window menu.  These new messges allow the localizer to change the
$ accelerators if they interfere with the local keyboard.  The only item
$ that has an accelerator is Close _C Alt<Key>F4 f.kill.

$ The underbar is part of the syntax for mnemonics and should not 
$ be changed. The letter after the underbar should match a 
$ letter (case sensitive) in the item title and should not conflict
$ with another mnemonic.
$        (e.g.   Normalize  _N  )

$ The name of the function (e.g. f.normalize) should not change

$    This item will "close" the window.  This usually results in the
$    window being withdrawn.
48 Stäng _S Alt<Key>F4 f.kill

$    This item normalizes a window
49 Återställ _Å  f.restore

$    This item moves a window
50 Flytta _F  f.move

$    This item resizes a window
51 Ändra storlek _Ä  f.resize

$    This item changes a window into an icon
52 Minimera _n  f.minimize

$    This item makes a window as large as allowed (determined by a number
$         of factors)
53 Maximera _x  f.maximize

$    This item lowers a window below other windows
54 Lägg underst _L  f.lower

$    This item brings up the occupy workspace dialog.  The character
$    sequence  "\\.\\.\\."  will appear as "..." in the menu . DO NOT CHANGE 
$    THE  "\\.\\.\\."  CHARACTERS
55 Lägg på\\ arbetsyta\\.\\.\\. _y  f.workspace_presence

$    This item puts a window into all workspaces.  The character sequence
$    "\\" provides a space between "Occupy" and "all"  Use this sequence
$    to provide spaces in item titles.
56 Lägg på\\ alla\\ arbetsytor _A  f.occupy_all

$    This item removes a window from a workspace.  This item will appear in
$    in the menu as  "Unoccupy Workspace"  with the first e in the title underlined
$    to indicate this is a mnemonic for this item.
57 Ta bort från\\ arbetsytan _b  f.remove

$    This is the default title of the "occupy workspace" dialog
59 Lägg på arbetsyta

$    The following 6 items are Sun-specific.  The only difference being
$    that these items have accelerators.  They should be localized
$    similarly to their corresponding item above.

60 Återställ _Å Alt<Key>F5 f.restore
61 Flytta _F  Alt<Key>F7   f.move
62 Ändra storlek _Ä  Alt<Key>F8   f.resize
63 Minimera _n  Alt<Key>F9 f.minimize
64 Maximera _x  Alt<Key>F10 f.maximize
65 Lägg underst _L  Alt<Key>F3   f.lower

$    Messages 66 to 68 go to an error file.
$ YOU DON'T NEED TO LOCALIZE MESSAGES 66 TO 68.
66 Out of range autoRaiseDelay resource value. Must be non-negative
67 Key bindings %s not found, using builtin key bindings\n
68 Button bindings %s not found, using builtin button bindings\n

$set 70
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmWinInfo.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for client data
2 Insufficient memory for client data
3 Couldn't make icon box
4 Couldn't make the front panel.
5 Window manager cannot convert property %.100s as clientTitle/iconTitle: XmbTextPropertyToTextList
6 insufficient memory to convert property %.100s as clientTitle/iconTitle: XmbTextPropertyToTextList
7 [XmbTextPropertyToTextList]:\n     Locale (%.100s) not supported. (Check $LANG).
8 Window manager received unknown property as clientTitle/iconTitle: %.100s. Property ignored.


$set 76
$    THIS COMMENT FOR DTWM TEAM ONLY -- WmWrkspace.c --
$    This entire set is written to an error file.
$ YOU DON'T NEED TO LOCALIZE THIS SET.
1 Insufficient memory for occupy workspace property
2 Insufficient memory for occupy workspace property
3 Insufficient memory to update workspace info
4 Insufficient memory for client data
5 Insufficient Memory (ConvertNamesToIDs)
6 Insufficient Memory (ConvertNamesToIDs)
7 Insufficient memory
8 Insufficient Memory (ConvertNamesToIDs)
9 Insufficient memory to add window to workspace
10 Insufficient Memory (GetLeaderPresence)
11 Insufficient memory
12 Insufficient memory to save resources
13 Insufficient memory to save resources.
14 Maximum number of workspaces is %d. No new workspace was created.


$set 82
$    THIS COMMENT FOR DTWM TEAM ONLY -- PopupMenu.c --
$    This entire set should be localized.

$ Message 1 is the title of the Delete Locked Control Error dialog
1 Arbetsytehanterare - Fel vid borttagning av låst inställning

$ Message 2 is the title of the Delete Control dialog
2 Arbetsytehanterare - Ta bort inställning

$ Message 3 is the title of the Delete Locked Subpanel Error dialog
3 Arbetsytehanterare - Fel vid borttagning av låst panelundermeny

$ Message 4 is the title of the Delete Subpanel dialog
4 Arbetsytehanterare - Ta bort panelundermeny

$ Message 5 is the message in the Delete Locked Control Error dialog
5 Det går inte att ta bort denna inställning eftersom den är låst.

$ Message 6 is the message in the Delete Control dialog
6 Vill du verkligen ta bort denna inställning?

$ Message 7 is the message in the Delete Locked Subpanel Error dialog
7 Det går inte att ta bort denna panelundermeny eftersom den är låst.

$ Message 8 is the message in the Delete Control dialog when more than
$ exits in the subpanel to be deleted.
8 Denna panelundermeny innehåller fler än en inställning.\nVill du verkligen ta bort panelundermenyn?

$ Message 9 is the message in the Delete Control dialog when only the
$ copy of the main panel control exists in the subpanel.
9 Vill du verkligen ta bort denna panelundermeny?

$ Message 10 - 19 are the menu title, menu items, and mnemonics in the
$ Main/Subpanel Menu.
$ 
$ The entries are paired (label + mnemonic). The label appears in the
$ menu. The mnemonic is the character in the label that is underlined by
$ the menu system indicating that it is a keyboard shortcut.
$ 
$ The mnemonic must be a character in the label. Each mnemonic must be 
$ unique within this particular menu.
$ 
10 Lägg till panelundermeny
11 A
12 Ta bort panelundermeny
13 D
14 Kopiera till huvudpanel
15 C
16 Ta bort
17 D
18 Hjälp
19 H

$ Message 20 - 21 are used in the menu title for the workspace button
$ menus.  20 is used for the format "Workspace <Name>".  21 is the first
$ arg in the format.
20 %1$s %2$s
21 Arbetsytan

$ Message 22 - 30 are the menu title, menu items, and mnemonics in the
$ Switch Menu.
$ 
$ The entries are paired (label + mnemonic). The label appears in the
$ menu. The mnemonic is the character in the label that is underlined by
$ the menu system indicating that it is a keyboard shortcut.
$ 
$ The mnemonic must be a character in the label. Each mnemonic must be 
$ unique within this particular menu.
$ 
22 Växla område
23 Lägg till arbetsyta
24 A
25 Ta bort
26 D
27 Byt namn
28 R
29 Hjälp
30 H

$ Message 31 - 40 are the pieces for the deletion dialogs
31 Ta bort inställning:
32 Om du tar bort denna inställning kan du inte återställa den med\nmålområdet \"Installera ikon\".\n\nI stället kan du använda \"RestorePanel\" i programhanteraren\nför att lägga tillbaka inställningen vid ett senare tillfälle.\n\nVill du ta bort inställningen?
33 Ta bort panelundermeny:
34 Om du tar bort denna panelundermeny kan du inte återställa alla\n inställningarna i panelundermenyn med\nmålområdet \"Installera ikon\".\n\nI stället kan du använda \"RestorePanel\" i programhanteraren\n för att lägga tillbaka panelundermenyn och dess inställningar vid ett senare tillfälle.\n\nVill du ta bort panelundermenyn?

$ Used for the name of a newly created workspace.
35 Nytt

$ Used for Install Icon title in Install Icon popup menu.
36 Installera ikon


$set 84
$    THIS COMMENT FOR DTWM TEAM ONLY -- UI.c --
$    This entire set should be localized.
$ 
$ Message used for the label of the dynamic install area.
1 Installera ikon


$set 86
$    THIS COMMENT FOR DTWM TEAM ONLY -- Callback.c --
$    This entire set should be localized.

$ The next two messages are used for workspace naming errors
1 Arbetsytehanterare - fel vid namnbyte
2 Ogiltigt eller duplicerat namn på arbetsytan

$ The next three messages are used for the dynamical install error dialog.
3 Arbetsytehanterare - fel vid ikoninstallation
4 Installera ikon:

$ Messages 5 and 6 are old. They are replaced by messaged 8 and 9.
5 There is already a icon of this name.
6 Control Action Error

7 Det går inte att hitta filen.
8 Det finns redan en ikon med detta namn.
9 Arbetsytehanterare - fel vid ikonfunktion


$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtwm.msg /main/4 1995/12/08 09:39:59 rswiston $
