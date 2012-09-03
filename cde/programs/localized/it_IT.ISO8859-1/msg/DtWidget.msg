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
$ **  $XConsortium: DtWidget.msg /main/5 1996/10/19 16:10:37 rws $
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
1 Chiudere

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 Aiuto

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
1 Impostazioni di formato

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 Margine destro:
3 Margine sinistro:

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 Allineare a sinistra
5 Allineare a destra
6 Giustificare
7 Centrare

$ Default values for DtNformatParagraphButtonLabel and 
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 Paragrafo
9 Tutto

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 Controllo ortografico

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 Ricerca/Sostituzione

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 Parole errate:

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 Ricercare:

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 Sostituire con:

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in Find/Change dialog
25 Ricercare
26 Sostituire
27 Sostituire tutto

$ ******************************************************************
$ ** 					      module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 La stringa %s non è presente nel documento.

$ Title for general information dialog (e.g. "Find/Change dialog can't 
$ find word in document.").
31 Informazioni

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line 
40 Riga:

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 Totale righe:

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode. 
42 Sovrascrittura

$ Default value for DtNinsertLabel resource,
$ label used to indicate Editor is in insert mode. 
$ The default value is a blank string.
43 

$ ******************************************************************
$ **                                          module: SearchCalls **
$ ** Message displayed when Find/Replace/Spell fails to execute   **
$ ** the spell filter.                                            **
$ ******************************************************************
$ Error message if there is a problem running the spell filter
$ Message 34 and 35 are part of the same message for the error
$ dialog.
$ do not translate %s
50 Errore nell'esecuzione del controllo ortografico.
51 Controllare che il filtro %s sia installato e che si trovi nel percorso.

$ Title for Error dialog
52 Errore dell'Editor


$set 4
$ *************************************************************
$ ** 							     ** 
$ ** MenuButton messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsage
$ Note: do not localize the chars XmNmenuPost
1 XmNmenuPost ha un valore non ammesso.

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget: Menu principale non valido.
3 DtMenuButtonWidget: Menu secondario non valido.

$set 5
$ *************************************************************
$ ** 							     ** 
$ ** ComboBox  	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget: Risorsa alignment non valida (verrà usata l'impostazione predefinita DtALIGNMENT_CENTER).
$ Note: do not localize the chars marginHeight 
2 DtComboBoxWidget: Risorsa marginHeight non valida (verrà usato il valore predefinito 2).
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget: Risorsa marginWidth non valida (verrà usato il valore predefinito 2).
$ Note: do not localize the chars horizontalSpacing
4 DtComboBoxWidget: Risorsa horizontalSpacing non valida (verrà usato il valore predefinito 0).
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget: Risorsa verticalSpacing non valida (verrà usato il valore predefinito 0).
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget: Risorsa orientation non valida (verrà usata l'impostazione predefinita DtRIGHT).
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget: Risorsa itemCount non valida (verrà usato il valore predefinito 0).
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget: Risorsa selectedPosition non valida (verrà usato il valore predefinito 0).
$ Note: do not localize the chars textField
9 DtComboBoxWidget: Impossibile impostare la risorsa textField.
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget: Impossibile trovare l'elemento da impostare (DtComboBoxSetItem).
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget: Impossibile trovare l'elemento da selezionare (DtComboBoxSelectItem).
12 Ridimensionamento fallito. Forse l'elemento più lungo supera la lunghezza massima consentita.

$ Label value
13 ComboBox

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType: Parametri errati; la conversione da String a XmRType non richiede altri argomenti

$ Warning message
15 DtComboBoxDeletePos: wrongParameters, valore di posizione non valido

$set 6
$ *************************************************************
$ ** 							     ** 
$ ** SpinBox   	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget: Risorsa arrowSensitivity non valida (verrà usata l'impostazione predefinita DtARROWS_SENSITIVE).
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget: Risorsa alignment non valida (verrà usata l'impostazione predefinita DtALIGNMENT_CENTER).
$ Note: do not localize the chars intialDelay 
3 DtSpinBoxWidget: Risorsa initialDelay non valida (verrà usato il valore predefinito 250).
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget: Risorsa marginHeight non valida (verrà usato il valore predefinito 2).
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget:  Risorsa marginWidth non valida (verrà usato il valore predefinito 2).
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget: Risorsa arrowLayout non valida (verrà usata l'impostazione predefinita DtARROWS_BEGINNING).
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget: Risorsa repeatDelay non valida (verrà usato il valore predefinito 200).
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget: Risorsa itemCount non valida (verrà usato il valore predefinito 0).
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget: Risorsa position non valida (verrà usato il valore predefinito 0).
10 DtSpinBoxWidget: Risorsa position non valida (verrà usata l'impostazione predefinita minimum).
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget:  Risorsa decimalPoints non valida (verrà usato il valore predefinito 0).
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget: Risorsa minimum non valida (verrà usata l'impostazione predefinita maximum).
$ Note: do not localize the chars textField
13 DtSpinBoxWidget:  Impossibile impostare la risorsa textField.
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget: Impossibile trovare l'elemento da impostare (DtSpinBoxSetItem).

$ Label value
15 SpinBox
