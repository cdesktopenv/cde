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
$ **  $XConsortium: DtWidget.msg /main/6 1996/10/19 14:45:28 rws $    
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
$quote "

$set 2
$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** Common messages shared by all DT widgets                **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Default value for DtNdialogCloseButtonLabel resource.    
$ This string is used for all "Close" buttons in all dialogs.    

1 "Cerrar"
$ Default value for DtNdialogHelpButtonLabel resource.    
$ This string is used for all "Help" buttons in all dialogs.    

2 "Ayuda"

$set 3        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ **  This set of messages is used by the DtEditor widget.  **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ **********************************************    
$ **                         module: Editor.c **    
$ **  Messages for the Format/Settings dialog **    
$ **********************************************    
$ Default value for DtNformatSettingsDialogTitle resource,    
$ title of format settings dialog    

1 "Formato Configuración"
$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,    
$ labels for margin fields in Format Settings dialog    

2 "Margen Derecho:"

3 "Margen Izquierdo:"
$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,    
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,    
$ labels for justification radio buttons in Format Settings dialog    

4 "Alineación Izquierda"

5 "Alineación Derecha"

6 "Justificar"

7 "Centrar"
$ Default values for DtNformatParagraphButtonLabel and     
$ DtNformatAllButtonLabel resources,    
$ labels for paragraph and all buttons in Format Settings dialog    

8 "Párrafo"

9 "Todos"
$ ******************************************************************    
$ **                                              module Editor.c **    
$ ** Strings used in the spell checking and find/change dialogs.  **    
$ ******************************************************************    
$ Default value for DtNspellDialogTitle resource,    
$ title of the Spell dialog    

20 "Ortografía"
$ Default value for DtNfindDialogTitle resource,    
$ title of the Find/Change dialog    

21 "Buscar/Cambiar"
$ Default value for DtNmisspelledListLabel resource,    
$ label for list of unrecognized and misspelled words    

22 "Palabras incorrectas:"
$ Default value for DtNfindFieldLabel resource,    
$ label of field in which user types string for which to search.    

23 "Buscar:"
$ Default value for DtNchangeFieldLabel resource,    
$ label of field in which user types the replacement string    

24 "Cambiar A:"
$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and    
$ DtNchangeAllButtonLabel resources,    
$ labels for find, change, and change all buttons in Find/Change dialog    

25 "Buscar"

26 "Cambiar"

27 "Cambiar Todo"
$ ******************************************************************    
$ ** \t\t\t\t\t      module: SearchDlg.  **    
$ ** Message displayed when Find/Replace/Spell fails to find the  **    
$ ** specified string or word.                                    **    
$ ******************************************************************    
$ Used to display "Unable to find the string FOO in the current document."    
$ Note: Do not localize the characters %s.    

30 "No se puede encontrar la cadena %s en el documento actual."
$ Title for general information dialog (e.g. "Find/Change dialog can't     
$ find word in document.").    

31 "Información"
$ ***********************************************************    
$ **                                       module Editor.c **    
$ ** Messages and strings associated with the status line. **    
$ ***********************************************************    
$ Default value for DtNcurrentLineLabel resource,    
$ label for the display showing the current insert cursor line     

40 "Línea:"
$ Default value for DtNtotalLineCountLabel resource,    
$ label for the display showing the total number of lines in the document    

41 "Total:"
$ Default value for DtNoverstrikeLabel resource,    
$ label used to indicate Editor is in overstrike (overtype) mode.     

42 "Pulsación"
$ Default value for DtNinsertLabel resource,    
$ label used to indicate Editor is in insert mode.     
$ The default value is a blank string.    

43 ""

$ ******************************************************************
$ **                                          module: SearchCalls **
$ ** Message displayed when Find/Replace/Spell fails to execute   **
$ ** the spell filter.                                            **
$ ******************************************************************
$ Error message if there is a problem running the spell filter
$ Message 34 and 35 are part of the same message for the error
$ dialog.
$ do not translate %s

$quote

50 Error al ejecutar el filtro de ortografía.
51 Verifique si el filtro %s está instalado y se encuentra en la ruta.

$ Title for Error dialog
52 Error del editor


$set 4        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** MenuButton messages \t\t\t\t     **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Warning messsage    
$ Note: do not localize the chars XmNmenuPost    

$quote "
1 "XmNmenuPost tiene un valor no permitido."
$ Error messsages    
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3    

2 "DtMenuButtonWidget: Padre no válido."

3 "DtMenuButtonWidget: Submenú no válido."

$set 5        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** ComboBox  \tmessages \t\t\t\t     **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Warning messsages    
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11    
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER    

1 "DtComboBoxWidget: Recurso alignment no válido (predeterminado en DtALIGNMENT_CENTER)."
$ Note: do not localize the chars marginHeight     

2 "DtComboBoxWidget: Recurso marginHeight no válido (predeterminado en 2)."
$ Note: do not localize the chars marginWidth    

3 "DtComboBoxWidget: Recurso marginWidth no válido (predetermidado en 2)."
$ Note: do not localize the chars horizontalSpacing    

4 "DtComboBoxWidget: Recurso horizontalSpacing no válido (predeterminado en 0)."
$ Note: do not localize the chars verticalSpacing    

5 "DtComboBoxWidget: Recurso verticalSpacing no válido (predeterminado en 0)."
$ Note: do not localize the chars orientation and DtRIGHT    

6 "DtComboBoxWidget: Recurso orientation no válido (predeterminado en DtRIGHT)."
$ Note: do not localize the chars itemCount    

7 "DtComboBoxWidget: Recurso itemCount no válido (predeterminado en 0)."
$ Note: do not localize the chars selectedPosition    

8 "DtComboBoxWidget: Recurso selectedPosition no válido (predeterminado en 0)."
$ Note: do not localize the chars textField    

9 "DtComboBoxWidget: No es posible establecer el recurso textField."
$ Note: do not localize the chars DtComboBoxSetItem    

10 "DtComboBoxWidget: No es posible encontrar elemento para establecer (DtComboBoxSetItem)."
$ Note: do not localize the chars DtComboBoxSelectItem    

11 "DtComboBoxWidget: No es posible encontrar elemento para seleccionar (DtComboBoxSelectItem)."

12 "Ha fallado el redimensionamiento. Puede ser que el elemento más largo sea más ancho que el espacio permitido."
$ Label value    

13 "CuadroTexto"
$ Error message    
$ Note: do not localize the chars String and XmRType    

14 "cvtStringToType: wrongParameters, String para conversión XmRType necesita argumentos adicionales."
$ Warning message    

15 "DtComboBoxDeletePos: wrongParameters, Valor de posición no válido"

$set 6        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** SpinBox   \tmessages \t\t\t\t     **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Warning messsages    
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14    
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE    

1 "DtSpinBoxWidget: Recurso arrowSensitivity no válido (predeterminado en DtARROWS_SENSITIVE)."
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER    

2 "DtSpinBoxWidget: Recurso alignment no válido (predeterminado en DtALIGNMENT_CENTER)."
$ Note: do not localize the chars intialDelay     

3 "DtSpinBoxWidget: Recurso initialDelay no válido (predeterminado en 250)."
$ Note: do not localize the chars marginHeight    

4 "DtSpinBoxWidget: Recurso marginHeight no válido (predeterminado en 2)."
$ Note: do not localize the chars marginWidth    

5 "DtSpinBoxWidget:  Recurso marginWidth no válido (predeterminado en 2)."
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING    

6 "DtSpinBoxWidget: Recurso arrowLayout no válido (predeterminado en DtARROWS_BEGINNING)."
$ Note: do not localize the chars repeatDelay    

7 "DtSpinBoxWidget: Recurso repeatDelay no válido (predeterminado en 200)."
$ Note: do not localize the chars itemCount    

8 "DtSpinBoxWidget: Recurso itemCount no válido (predeterminado en 0)."
$ Note: do not localize the chars position in messages 9 and 10    

9 "DtSpinBoxWidget: Recurso position no válido (predeterminado en to 0)."

10 "DtSpinBoxWidget:  Recurso position no válido (predeterminado en mínimo)."
$ Note: do not localize the chars decimalPoints    

11 "DtSpinBoxWidget:  Recurso decimalPoints no válido (predeterminado en 0)."
$ Note: do not localize the chars minimum    

12 "DtSpinBoxWidget: Recurso mínimun no válido (predeterminado en máximo)."
$ Note: do not localize the chars textField    

13 "DtSpinBoxWidget:  No es posible establecer el recurso textField."
$ Note: do not localize the chars DtSpinBoxSetItem    

14 "DtSpinBoxWidget: No es posible encontrar elemento para establecer (DtSpinBoxSetItem)."
$ Label value    

15 "CuadroDesplazamiento"
