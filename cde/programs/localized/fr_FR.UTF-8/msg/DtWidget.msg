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
$ **  $XConsortium: DtWidget.msg /main/6 1996/10/19 15:26:06 rws $    
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

1 "Fermer"
$ Default value for DtNdialogHelpButtonLabel resource.    
$ This string is used for all "Help" buttons in all dialogs.    

2 "Aide"

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

1 "Paramètres de mise en forme"
$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,    
$ labels for margin fields in Format Settings dialog    

2 "Marge droite:"

3 "Marge gauche:"
$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,    
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,    
$ labels for justification radio buttons in Format Settings dialog    

4 "Aligné à gauche"

5 "Aligné à droite"

6 "Justifié"

7 "Centré"
$ Default values for DtNformatParagraphButtonLabel and     
$ DtNformatAllButtonLabel resources,    
$ labels for paragraph and all buttons in Format Settings dialog    

8 "Paragraphe"

9 "Tout"
$ ******************************************************************    
$ **                                              module Editor.c **    
$ ** Strings used in the spell checking and find/change dialogs.  **    
$ ******************************************************************    
$ Default value for DtNspellDialogTitle resource,    
$ title of the Spell dialog    

20 "Orthographe"
$ Default value for DtNfindDialogTitle resource,    
$ title of the Find/Change dialog    

21 "Rechercher/Remplacer"
$ Default value for DtNmisspelledListLabel resource,    
$ label for list of unrecognized and misspelled words    

22 "Mots mal orthographiés:"
$ Default value for DtNfindFieldLabel resource,    
$ label of field in which user types string for which to search.    

23 "Rerchercher:"
$ Default value for DtNchangeFieldLabel resource,    
$ label of field in which user types the replacement string    

24 "Modifier en:"
$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and    
$ DtNchangeAllButtonLabel resources,    
$ labels for find, change, and change all buttons in Find/Change dialog    

25 "Rechercher"

26 "Modifier"

27 "Tout modifier"
$ ******************************************************************    
$ ** \t\t\t\t\t      module: SearchDlg.  **    
$ ** Message displayed when Find/Replace/Spell fails to find the  **    
$ ** specified string or word.                                    **    
$ ******************************************************************    
$ Used to display "Unable to find the string FOO in the current document."    
$ Note: Do not localize the characters %s.    

30 "La chaîne %s est introuvable dans le document en cours."
$ Title for general information dialog (e.g. "Find/Change dialog can't     
$ find word in document.").    

31 "Informations"
$ ***********************************************************    
$ **                                       module Editor.c **    
$ ** Messages and strings associated with the status line. **    
$ ***********************************************************    
$ Default value for DtNcurrentLineLabel resource,    
$ label for the display showing the current insert cursor line     

40 "Ligne:"
$ Default value for DtNtotalLineCountLabel resource,    
$ label for the display showing the total number of lines in the document    

41 "Total:"
$ Default value for DtNoverstrikeLabel resource,    
$ label used to indicate Editor is in overstrike (overtype) mode.     

42 "Surimprimer"
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

50 Erreur d'exécution du filtre orthographe.
51 Vérifiez que le filtre, %s, est installé et sur le chemin.

$ Title for Error dialog
52 Erreur d'éditeur

$set 4        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** MenuButton messages \t\t\t\t     **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Warning messsage    
$ Note: do not localize the chars XmNmenuPost    

$quote "
1 "La valeur de XmNmenuPost est incorrecte."
$ Error messsages    
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3    

2 "DtMenuButtonWidget: Parent incorrect."

3 "DtMenuButtonWidget: Sous-menu incorrect."

$set 5        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** ComboBox  \tmessages \t\t\t\t     **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Warning messsages    
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11    
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER    

1 "DtComboBoxWidget: Ressource d'alignement incorrecte (par défaut, utilisation de DtALIGNMENT_CENTER)."
$ Note: do not localize the chars marginHeight     

2 "DtComboBoxWidget: Ressource marginHeight incorrecte (par défaut, utilisation de la valeur 2)."
$ Note: do not localize the chars marginWidth    

3 "DtComboBoxWidget: Ressource marginWidth incorrecte (par défaut, utilisation de la valeur 2)."
$ Note: do not localize the chars horizontalSpacing    

4 "DtComboBoxWidget: Ressource horizontalSpacing incorrecte (par défaut, utilisation de la valeur 2)."
$ Note: do not localize the chars verticalSpacing    

5 "DtComboBoxWidget: Ressource verticalSpacing incorrecte (par défaut, utilisation de la valeur 0)."
$   

6 "DtComboBoxWidget: Ressource d'orientation incorrecte (par défaut, utilisation de DtRIGHT)."
$ Note: do not localize the chars itemCount    

7 "DtComboBoxWidget: Ressource itemCount incorrecte (par défaut, utilisation de la valeur 0)."
$ Note: do not localize the chars selectedPosition    

8 "DtComboBoxWidget: Ressource selectedPosition inccorrecte (par défaut, utilisation de la valeur 0)."
$ Note: do not localize the chars textField    

9 "DtComboBoxWidget: Impossible de définir la ressource textField."
$ Note: do not localize the chars DtComboBoxSetItem    

10 "DtComboBoxWidget: Elément à définir introuvable (DtComboBoxSetItem)."
$ Note: do not localize the chars DtComboBoxSelectItem    

11 "DtComboBoxWidget: Elément à sélectionner introuvable (DtComboBoxSelectItem)."

12 "Le redimensionnement n'a pas abouti. Il est possible que la taille de l'élément le plus long excède l'espace alloué."
$ Label value    

13 "Boîte de dialogue mixte"
$ Error message    
$ Note: do not localize the chars String and XmRType    

14 "cvtStringToType: Paramètres incorrects, la conversion de String en XmRType ne requiert aucun argument supplémentaire."
$ Warning message    

15 "DtComboBoxDeletePos: Paramètres incorrects, valeur d'emplacement incorrecte."

$set 6        Redefine set#

$ *************************************************************    
$ ** \t\t\t\t\t\t\t     **     
$ ** SpinBox   \tmessages \t\t\t\t     **    
$ ** \t\t\t\t\t\t\t     **     
$ *************************************************************    
$ Warning messsages    
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14    
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE    

1 "DtSpinBoxWidget: Ressource arrowSensitivity incorrecte (par défaut, utilisation de DtARROWS_SENSITIVE)."
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER    

2 "DtSpinBoxWidget: Ressource d'alignement incorrecte (par défaut, utilisation de DtALIGNMENT_CENTER)."
$ Note: do not localize the chars intialDelay     

3 "DtSpinBoxWidget: Ressource initialDelay incorrecte (par défaut, utilisation de la valeur 250)."
$ Note: do not localize the chars marginHeight    

4 "DtSpinBoxWidget: Ressource marginHeight incorrecte (par défaut, utilisation de la valeur 2)."
$ Note: do not localize the chars marginWidth    

5 "DtSpinBoxWidget: Ressource marginWidth incorrecte (par défaut, utilisation de la valeur 2)."
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING    

6 "DtSpinBoxWidget: Ressource arrowLayout incorrecte (par défaut, utilisation de DtARROWS_BEGINNING)."
$ Note: do not localize the chars repeatDelay    

7 "DtSpinBoxWidget: Ressource repeatDelay incorrecte (par défaut, utilisation de la valeur 200)."
$ Note: do not localize the chars itemCount    

8 "DtSpinBoxWidget: Ressource itemCount incorrecte (par défaut, utilisation de la valeur 0)."
$ Note: do not localize the chars position in messages 9 and 10    

9 "DtSpinBoxWidget: Ressource d'emplacement incorrecte (par défaut, utilisation de la valeur 0)."

10 "DtSpinBoxWidget: Ressource d'emplacement incorrecte (par défaut, utilisation de la valeur minimale)."
$ Note: do not localize the chars decimalPoints    

11 "DtSpinBoxWidget:  Ressource decimalPoints incorrecte (par défaut, utilisation de la valeur 0)."
$ Note: do not localize the chars minimum    

12 "DtSpinBoxWidget: Ressource minimale incorrecte (par défaut, utilisation de la valeur maximale)."
$ Note: do not localize the chars textField    

13 "DtSpinBoxWidget: Impossible de définir la ressource textField."
$ Note: do not localize the chars DtSpinBoxSetItem    

14 "DtSpinBoxWidget: Element à définir introuvable (DtSpinBoxSetItem)."
$ Label value    

15 "Sélecteur rotatif"
