$ $TOG: dtinfo_start.msg /main/3 1999/09/20 13:28:58 mgreess $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.
$ 
$ ******************************************************************************
$ 
$set 1
$ The messages in this set will only appear in the users
$   "errorlog" file.
$ 
$ ******************************************************************************
$ 
$ Messages 1-5: These messages result from an error in a tt_* function
$    call.
$ 
1 Der Prozeß ttsession(1) läuft nicht und ToolTalk\n\
kann ihn nicht erneut starten.
2 Die aktuelle Standardprozeßkennung ist veraltet oder ungültig.
3 Die angegebene ToolTalk-Session ist veraltet oder ungültig.

$ 
$ Message 4: Do not localize the word DtInfo_Start.
$ 
4 Der Prozeßtyp 'DtInfo_Start' ist nicht der\n\
Name eines installierten Prozeßtyps.

5 Die Fehlernummer wird nicht erkannt.

6 Kann keine Aktion aufrufen, weil malloc(1) fehlschlug.

$ 
$ Message 7: Do not localize the word DISPLAY.  Do not localize the
$   word XtOpenDisplay().
$ 
7 XtOpenDisplay() fehlgeschlagen. Möglicherweise ist die Umgebungsvariable DISPLAY\n\
nicht gesetzt oder ungültig.

$ 
$ Message 8: Do not localize the word DtAppInitialize().
$ 
8 DtAppInitialize() fehlgeschlagen. Möglicherweise ist die Desktop-Umgebung\n\
nicht richtig installiert.

$ 
$ Message 9: Do NOT localize the word DTINFOLIBDEFAULT
$ 
9 Eine InfoBibl wurde nicht in der Nachricht '%s' zur Verfügung gestellt.\n\
Der Wert der Umgebungsvariable DTINFOLIBDEFAULT\n\
(%s) wird für die InfoBibl verwendet.

$ 
$ Message 10: Do NOT localize the word DTINFOLIBDEFAULT
$ 
10 Eine InfoBibl wurde nicht in der Nachricht '%s' zur Verfügung gestellt,\n\
und die Umgebungsvariable DTINFOLIBDEFAULT wurde nicht definiert.

$ 
$ Messages 11-16: These message are the result of the dtinfo_start
$   getting an error while trying to parse a ToolTalk message.
$ 
$   Do NOT localize the word NULL
$   
11 Die Nachrichtenoperation '%d' wird nicht unterstützt.

12 Nachricht '%s' hat nicht die erforderliche\n\
Anzahl von Argumenten '%d'.

13 Nachricht '%s' gab einen NULL-Aktionsnamen an.\n\
Aktion '%s' wird aufgerufen.

14 Nachricht '%s' gab einen NULL-Ausführungsrechner an.

15 Nachricht '%s' gab eine NULL-Sprachumgebung an.

16 Nachricht '%s' gab ein NULL-Thema an.

$set 2
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Dtinfo: Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
