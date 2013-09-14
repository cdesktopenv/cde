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
$ **  $XConsortium: DtWidget.msg /main/4 1996/06/19 11:45:23 cde-dec $
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
1 ùÃÝð

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 ëºÉ·ë©Íü

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
1 ÖªÈ¢ÝÃÌù

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 Æ­ù±ÒÌ¡¨
3 ÆÊù±ÒÌ¡¨

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 ïêÆÊè×ëý
5 ïêÆ­è×ëý
6 ÉùÇÀè×ëý
7 Í¢Äã

$ Default values for DtNformatParagraphButtonLabel and 
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 Ñôæ¯
9 ÇÀÝå

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 Ñ»Çó

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 ÞòÊÆ/ÊÕÊÑ

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 Ñ»Çóòãë¨¡¨

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 ÞòÊÆ¡¨

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 ÊÕÊÑÒ³¡¨

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in Find/Change dialog
25 ÞòÊÆ
26 ÊÕÊÑ
27 ÇÀÝåÊÕÊÑ

$ ******************************************************************
$ ** 					      module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 ÇãÜ¢ÈçÅÆÇµÄãÊÆÄâÌ¯ÇóÈë %s¡¤

$ Title for general information dialog (e.g. "Find/Change dialog can't 
$ find word in document.").
31 æñØ¹

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line 
40 ÈçæÀ¡¨

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 ôêÈçí°¡¨

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode. 
42 ÷»ìÑ

$ Default value for DtNinsertLabel resource,
$ label used to indicate Editor is in insert mode. 
$ The default value is a blank string.
$quote "
43 ""
$quote

$ ******************************************************************
$ **                                          module: SearchCalls **
$ ** Message displayed when Find/Replace/Spell fails to execute   **
$ ** the spell filter.                                            **
$ ******************************************************************
$ Error message if there is a problem running the spell filter
$ Message 34 and 35 are part of the same message for the error
$ dialog.
$ do not translate %s
$ "NEW-"
50 Error executing spell filter.
$ "NEW-"
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
1 XmNmenuPost ÎûÔ«ÄâÇÙÎÎ¡¤

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget¡¨àÒÕæÎûÄ¸ìÒÅü×äÏÐ¡¤
3 DtMenuButtonWidget¡¨àÒÕæÎûÈ¹Åü×äÏÐ¡¤

$set 5
$ *************************************************************
$ ** 							     ** 
$ ** ComboBox  	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget¡¨àÒÕæÎû alignment æñäã (ÄùÌùÔ«Ò³ DtALIGNMENT_CENTER)¡¤
$ Note: do not localize the chars marginHeight 
2 DtComboBoxWidget¡¨àÒÕæÎû marginHeight æñäã (ÄùÌùÔ«Ò³ 2)¡¤
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget¡¨àÒÕæÎû marginWidth æñäã (ÄùÌùÔ«Ò³ 2)¡¤
$ Note: do not localize the chars horizontalSpacing
4 DtComboBoxWidget¡¨àÒÕæÎû horizontalSpacing æñäã (ÄùÌùÔ«Ò³ 0)¡¤
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget¡¨àÒÕæÎû verticalSpacing æñäã (ÄùÌùÔ«Ò³ 0)¡¤
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget¡¨àÒÕæÎû orientation æñäã (ÄùÌùÔ«Ò³ DtRIGHT)¡¤
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget¡¨àÒÕæÎû itemCount æñäã (ÄùÌùÔ«Ò³ 0)¡¤
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget¡¨àÒÕæÎû selectedPosition æñäã (ÄùÌùÔ«Ò³ to 0)¡¤
$ Note: do not localize the chars textField
9 DtComboBoxWidget¡¨àÒÎÎÝÃÌù textField  æñäã¡¤
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget¡¨àÒÎÎÊÆÌ¯âúÆøËôÝÃÌù (DtComboBoxSetItem)¡¤
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget¡¨àÒÎÎÊÆÌ¯âúÆøËôòÙÌ½ (DtComboBoxSelectItem)¡¤
12 ÊÕÊÑÄËÄÑÆÂÚõ¡¤Æ«×äÑÒÞ²ÏÛÎûâúÆøâ¾ç´Æ«ÆîìÏÐùÍÔðÒÓ³¡¤

$ Label value
13 ê·ÇÙÅÉÕú

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType¡¨Ù¶í°òãë¨¡¢String Ì¯ XmRType Îû÷ËßÐÄâëæÓÑ÷ìÆÀÎûÙ¶í°

$ Warning message
15 DtComboBoxDeletePos¡¨Ù¶í°òãë¨¡¢àÒÕæÎûÈíåôÔ«

$set 6
$ *************************************************************
$ ** 							     ** 
$ ** SpinBox   	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget¡¨àÒÕæÎû arrowSensitivity æñäã (ÄùÌùÔ«Ò³ DtARROWS_SENSITIVE)¡¤
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget¡¨àÒÕæÎû alignment æñäã (ÄùÌùÔ«Ò³ DtALIGNMENT_CENTER)¡¤
$ Note: do not localize the chars intialDelay 
3 DtSpinBoxWidget¡¨àÒÕæÎû initialDelay æñäã (ÄùÌùÔ«Ò³ 250)¡¤
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget¡¨àÒÕæÎû marginHeight æñäã (ÄùÌùÔ«Ò³ 2)¡¤
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget¡¨àÒÕæÎû marginWidth æñäã (ÄùÌùÔ«Ò³ 2)¡¤
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget¡¨àÒÕæÎû arrowLayout æñäã (ÄùÌùÔ«Ò³ DtARROWS_BEGINNING)¡¤
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget¡¨àÒÕæÎû repeatDelay æñäã (ÄùÌùÔ«Ò³ 200)¡¤
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget¡¨àÒÕæÎû itemCount æñäã (ÄùÌùÔ«Ò³ 0)¡¤
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget¡¨àÒÕæÎû position æñäã (ÄùÌùÔ«Ò³ 0)¡¤
10 DtSpinBoxWidget¡¨àÒÕæÎû position æñäã (ÄùÌùÔ«Ò³ minimum)¡¤
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget¡¨àÒÕæÎû decimalPoints æñäã (ÄùÌùÔ«Ò³ 0)¡¤
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget¡¨àÒÕæÎû minimum æñäã (ÄùÌùÔ«Ò³ maximum)¡¤
$ Note: do not localize the chars textField
13 DtSpinBoxWidget¡¨àÒÎÎÝÃÌù textField æñäã¡¤
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget¡¨àÒÎÎÊÆÌ¯âúÆøËôÝÃÌù (DtSpinBoxSetItem)¡¤

$ Label value
15 òÙâúÅÉÕú
