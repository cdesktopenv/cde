$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtWidget.vmsg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt widget
$ **  library.
$ **
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: DtWidget.msg /main/6 1996/06/19 11:28:04 cde-dec $
$ *****************************************************************************
$ *****************************************************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There MAY be three types of messages in this file:
$ 
$  1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$  2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$  3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************


$set 2
$ *************************************************************
$ ** 							     ** 
$ ** Common messages shared by all DT widgets                **
$ ** 							     ** 
$ *************************************************************
$ Default value for DtNdialogCloseButtonLabel resource.
$ This string is used for all "Close" buttons in all dialogs.
1 Close

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 Help

$set 3
$ *************************************************************
$ ** 							     ** 
$ **  This set of messages is used by the DtEditor widget.  **
$ ** 							     ** 
$ *************************************************************

$ **********************************************
$ **                         module: Editor.c **
$ **  Messages for the Format/Settings dialog **
$ **********************************************

$ Default value for DtNformatSettingsDialogTitle resource,
$ title of format settings dialog
1 Format Settings

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 Right Margin:
3 Left Margin:

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 Left Align
5 Right Align
6 Justify
7 Center

$ Default values for DtNformatParagraphButtonLabel and 
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 Paragraph
9 All

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 Spell

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 Find/Change

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 Misspelled Words:

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 Find:

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 Change To:

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in Find/Change dialog
25 Find
26 Change
27 Change All

$ ******************************************************************
$ ** 					      module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 Unable to find the string %s in the current document.

$ Title for general information dialog (e.g. "Find/Change dialog can't 
$ find word in document.").
31 Information

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line 
40 Line:

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 Total:

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode. 
42 Overstrike

$ Default value for DtNinsertLabel resource,
$ label used to indicate Editor is in insert mode. 
$ The default value is a blank string.
43 

$ ******************************************************************
$ ** 					      module: SearchCalls **
$ ** Message displayed when Find/Replace/Spell fails to execute   **
$ ** the spell filter.                                            **
$ ******************************************************************
$ Error message if there is a problem running the spell filter
$ Message 34 and 35 are part of the same message for the error
$ dialog. 
$ do not translate %s
$ "NEW-"
50 Error executing spell filter.
51 Check to see if filter, %s, is installed and in the path.

$ Title for Error dialog
$ "NEW-"
52 Editor Error

$set 4
$ *************************************************************
$ ** 							     ** 
$ ** MenuButton messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsage
$ Note: do not localize the chars XmNmenuPost
1 XmNmenuPost has illegal value.

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget: Invalid Parent.
3 DtMenuButtonWidget: Invalid Submenu.

$set 5
$ *************************************************************
$ ** 							     ** 
$ ** ComboBox  	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget: Invalid alignment resource (defaulting to DtALIGNMENT_CENTER).
$ Note: do not localize the chars marginHeight 
2 DtComboBoxWidget: Invalid marginHeight resource (defaulting to 2).
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget: Invalid marginWidth resource (defaulting to 2).
$ Note: do not localize the chars horizontalSpacing
4 DtComboBoxWidget: Invalid horizontalSpacing resource (defaulting to 0).
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget: Invalid verticalSpacing resource (defaulting to 0).
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget: Invalid orientation resource (defaulting to DtRIGHT).
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget: Invalid itemCount resource (defaulting to 0).
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget: Invalid selectedPosition resource (defaulting to 0).
$ Note: do not localize the chars textField
9 DtComboBoxWidget: Unable to set textField resource.
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget: Unable to find item to set (DtComboBoxSetItem).
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget: Unable to find item to select (DtComboBoxSelectItem).
12 Resizing failed.  Maybe longest item is wider than space allowed.

$ Label value
13 ComboBox

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType: wrongParameters, String to XmRType conversion needs no extra arguments

$ Warning message
15 DtComboBoxDeletePos: wrongParameters, Invalid position value

$set 6
$ *************************************************************
$ ** 							     ** 
$ ** SpinBox   	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget: Invalid arrowSensitivity resource (defaulting to DtARROWS_SENSITIVE).
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget: Invalid alignment resource (defaulting to DtALIGNMENT_CENTER).
$ Note: do not localize the chars intialDelay 
3 DtSpinBoxWidget: Invalid initialDelay resource (defaulting to 250).
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget: Invalid marginHeight resource (defaulting to 2).
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget:  Invalid marginWidth resource (defaulting to 2).
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget: Invalid arrowLayout resource (defaulting to DtARROWS_BEGINNING).
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget: Invalid repeatDelay resource (defaulting to 200).
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget: Invalid itemCount resource (defaulting to 0).
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget: Invalid position resource (defaulting to 0).
10 DtSpinBoxWidget: Invalid position resource (defaulting to minimum).
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget:  Invalid decimalPoints resource (defaulting to 0).
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget: Invalid minimum resource (defaulting to maximum).
$ Note: do not localize the chars textField
13 DtSpinBoxWidget:  Unable to set textField resource.
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget: Unable to find item to set (DtSpinBoxSetItem).

$ Label value
15 SpinBox
