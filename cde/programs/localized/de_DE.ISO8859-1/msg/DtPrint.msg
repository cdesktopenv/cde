$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtPrint.msg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
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
$ **  $XConsortium: DtPrint.msg /main/10 1996/11/19 18:07:06 rws $
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


$set 1
$ *************************************************************
$ ** 							     ** 
$ ** PrintSetupBox messages and labels 			     **
$ ** 							     ** 
$ *************************************************************
$quote

$ Warning messages
$ Note: do not localize the chars %s
$ 
1 Ungültige Ressourceneinstellung für workAreaLocation.

$ Label values
$ 
2 Weitere Drucker...
3 Drucken in Datei:
4 Datei auswählen...
5 Abbrechen
6 Kopien:
7 Hilfe
8 Drucken
9 Druckername:
10 Einstellungen...
11 Druckerbeschreibung:

$ Warning messages
$ Note: do not localize the chars %s
12 Höchstzahl von Kopien überschritten.
13 Ungültiger Modus der Druckereinstellungen.
14 Modus der Druckereinstellungen kann nicht geändert werden.

$ Label values
$ 
15 Info... 

$ Warning messages
$ Note: do not localize the chars %s
16 Ungültiger Drucker
17 Drucker %s nicht auf Druckserver %s gefunden.
18 Kann nicht an Druckserver %s angeschlossen werden.
19 X-Server %s unterstützt das Drucken nicht.
20 Ein Standarddrucker konnte nicht ermittelt werden.
21 Ein X-Druckserver für %s konnte nicht gefunden werden.
22 Druckername fehlt in der Angabe: %s
23 Ungültiges Druckziel
24 Kann nicht von zusammengesetztem Text konvertieren.
25 Argumente werden für die Ressourcenkonvertierung nicht benötigt.

$ Label values
$ 
26 Fehler bei der Druckerauswahl

$ Warning messages
$ Note: do not localize the chars %s
27 Kann keine Drucker finden.

$ Label values
$ 
28 Beschreibung nicht verfügbar.
29 Datei auswählen
30 Druckerinformationen
31 Format:
32 Druckermodell:
33 Drucker:
34 Weitere Drucker
35 An Drucker schicken
36 Dateiname:
37 Druckereinstellungen-Hilfe

$set 2
$ *************************************************************
$ ** 							     ** 
$ ** _DtPrint Frame GUI messages			     **
$ ** 							     ** 
$ *************************************************************
$ 
$quote "

$ Label values for Margin Frame
$ 
1 "Ränder"
2 "Oben:"
3 "Rechts:"
4 "Unten:"
5 "Links:"
$ 
$ Label values for Header/Footer Frame
$ 
6 "Kopf- und Fußzeilen"
7 "Oben links:"
8 "Oben rechts:"
9 "Unten links:"
10 "Unten rechts:"

$set 3
$ *************************************************************
$ ** 							     ** 
$ ** Dt Print Dialog Manager Job Options GUI messages        **
$ ** 							     ** 
$ *************************************************************
$ 
$quote

$ Label values for Margin Frame
1 Post senden, sobald fertiggestellt.
2 Deckblattitel:
3 Druckbefehloptionen:
