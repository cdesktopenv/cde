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
$ **  $XConsortium: DtWidget.msg /main/4 1996/06/19 11:44:12 cde-dec $
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
1 Stäng

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 Hjälp

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
1 Formatering 

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 Högermarginal:
3 Vänstermarginal:

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 Vänsterställ
5 Högerställ
6 Justera
7 Centrera

$ Default values for DtNformatParagraphButtonLabel and 
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 Stycke
9 Hela dokumentet

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 Stavning

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 Sök/Ersätt 

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 Felstavade ord: 

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 Sök:

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 Ersätt med: 

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in Find/Change dialog
25 Sök
26 Ersätt 
27 Ersätt alla 

$ ******************************************************************
$ ** 					      module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 Det går inte att hitta söksträngen %s i aktuellt dokument.

$ Title for general information dialog (e.g. "Find/Change dialog can't 
$ find word in document.").
31 Information

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line 
40 Rad:

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 Totalt: 

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode. 
42 Skriv över 

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
1 XmNmenuPost har felaktigt värde. 

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget: Felaktig överordnad meny.
3 DtMenuButtonWidget: Felaktig undermeny.

$set 5
$ *************************************************************
$ ** 							     ** 
$ ** ComboBox  	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget: Felaktig alignment-resurs (standardvärdet DtALIGNMENT_CENTER används). 
$ Note: do not localize the chars marginHeight 
2 DtComboBoxWidget: Felaktig marginHeight-resurs (standardvärdet 2 används).
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget: Felaktig marginWidth-resurs (standardvärdet 2 används).
$ Note: do not localize the chars horizontalSpacing
4 DtCmboBoxWidget: Felaktig horizontalSpacing-resurs (standardvärdet 0 används).
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget: Felaktig verticalSpacing-resurs (standardvärdet 0 används).
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget: Felaktig orientation-resurs (standardvärdet DtRIGHT används).
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget: Felaktig itemCount-resurs (standardvärdet 0 används).
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget: Felaktig selectedPosition-resurs (standardvärdet 0 används).
$ Note: do not localize the chars textField
9 DtComboBoxWidget: Det går inte att markera textField-resursen. 
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget: Alternativ saknas för att ställa in (DtComboBoxSetItem). 
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget: Alternativ saknas för att markera (DtComboBoxSelectItem). 
12 Storleksökningen misslyckades. Längsta objektet är kanske större än tillgängligt utrymme. 

$ Label value
13 Kombiruta

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType: wrongParameters, konvertering av String till XmRType kräver inga extra argument. 

$ Warning message
15 DtComboBoxDeletePos: wrongParameters, Ogiltigt positionsvärde


$set 6
$ *************************************************************
$ ** 							     ** 
$ ** SpinBox   	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget: Felaktig arrowSensitivity-resurs (standardvärdet DtARROWS_SENSITIVE används).
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget: Felaktig alignment-resurs (standardvärdet DtALIGNMENT_CENTER används).
$ Note: do not localize the chars intialDelay 
3 DtSpinBoxWidget: Felaktig initialDelay-resurs (standardvärdet 250 används).
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget: Felaktig marginHeight-resurs (standardvärdet 2 används).
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget: Felaktig marginWidth-resurs (standardvärdet 2 används).
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget: Felaktig arrowLayout-resurs (standardvärdet DtARROWS_BEGINNING används).
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget: Felaktig repeatDelay-resurs (standardvärdet 200 används).
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget: Felaktig itemCount-resurs (standardvärdet 0 används).
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget: Felaktig position-resurs (standardvärdet 0 används).
10 DtSpinBoxWidget: Felaktig position-resurs (standardvärdet minimum används).
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget: Felaktig decimalPoints-resurs (standardvärdet 0 används).
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget: Felaktig minimum-resurs (standardvärdet maximum används).
$ Note: do not localize the chars textField
13 DtSpinBoxWidget: Det går inte att markera textField-resursen.
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget: Alternativet saknas för att ställa in (DtSpinBoxSetItem).

$ Label value
15 Ruta
