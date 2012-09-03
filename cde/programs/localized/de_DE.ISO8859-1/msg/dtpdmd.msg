$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpdmd.msg
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  Dialog Manager Daemon.
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1996 Digital Equipment Corporation.
$ ** (c) Copyright 1996 Hewlett-Packard Company.
$ ** (c) Copyright 1996 International Business Machines Corp.
$ ** (c) Copyright 1996 Sun Microsystems, Inc.
$ ** (c) Copyright 1996 Novell, Inc. 
$ ** (c) Copyright 1996 FUJITSU LIMITED.
$ ** (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: dtpdmd.msg /main/2 1996/11/19 18:08:19 rws $
$ *****************************************************************************
$ *****************************************************************************

$set 1

$    /* NL_COMMENT
$ 
$	General Translator Comments
$ 
$       - The acronyms "pdm" and "PDM" are used in several
$         messages, and were derived from the english phrase
$         "Print Dialog Manager". The acronyms should not be
$         localized.
$ 
$       - The acronym "pdmd" is used in several messages,
$         and was derived from the english phrase "Print
$         Dialog Manager Daemon". The acronym should not be
$         localized.
$ 
$     */
1 Gebrauch: %1$s [Optionen]\n\
\n\
wobei [Optionen] miteinschließt:\n\
   -d <Anzeige>        Rechner:Anzeige[.Bildschirm] des X-Druckservers\n\
   -a <alt. Auswahl>  ein alternativer X-Auswahlname (\"%2$s\" Standard)\n\
   -p <Standard pdm>    ein Standard pdm, um (\"%3$s\" Standard) zu starten\n\
   -P <der pdm>        der Start-pdm, der alle anderen überstimmt\n\
   -s                  Sicherheitsprotokoll einschalten\n\
   -f <Befugnisdatei>      ein anfängliche Datei mit Befugniseintragungen\n\
   -l <log Datei>       eine Datei zur Aufzeichnung von Fehlern, usw.

2 %1$s: Fehler, kann Anzeige %2$s nicht öffnen\n

3 %1$s: Fehler, ein anderer pdm ist bereits gestartet (%2$s)\n

4 %s: Fehler, kann kein Xt signal pipe erstellen\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "SelectionRequest" in
$         messages 5, 6 and 7.
$ 
$       - Do not translate the word "XmbTextPropertyToTestList"
$         in message 7.
$ 
$     */
5 %s: Fehler, Zugang zu SelectionRequest property nicht möglich\n

6 %s: Fehler, ungültiges Format für SelectionRequest property\n

7 %s: Fehler, XmbTextPropertyToTestList für SelectionRequest property fehlgeschlagen\n

8 %s: Fehler, erstellen einer message pipe nicht möglich\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "fork" in message 9.
$ 
$     */
9 %s: Fehler, fork nicht möglich\n

10 %1$s: Fehler, kann \"%2$s\" nicht ausführen \n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "exec" in message 11.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 11.
$ 
$     */
11 %1$s: exec Meldung in %2$s eingetragen\n\
   exec Fehlermeldung = \"%3$s\"\n\
   exec Fehlercode    = %4$s\n\
   pdm                = \"%5$s\"\n\
   -display           = \"%6$s\"\n\
   -vdisplay          = \"%7$s\" \n

12 %1$s: Fehler, kann die endgültige %2$s\n\
Meldung an den anfordernden Klienten im Auftrag des\n\
PDM wegen eines X-Fehlers nicht schicken\n

13 %1$s: Fehler, kann die endgültige %2$s\n\
Meldung an den anfordernden Klienten im Auftrag des\n\
PDM wegen eines XIO-Fehlers nicht schicken\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "stderr" in message 14.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 14.
$ 
$     */
14 %1$s: pdm Meldung in %2$s eingetragen\n\
   pdm       = \"%3$s\"\n\
   -display  = \"%4$s\"\n\
   -vdisplay = \"%5$s\"\n\
   Beenden-code = \"%6$d\"\n\
   stderr    = \"%7$s\" \n

15 beigefügte pdmd Fehler= \n\"%s\" \n

16 %1$s: Fehler, kann die log Datei \"%2$s\" nicht öffnen. stderr Meldung logging wird ausgeschaltet\n

