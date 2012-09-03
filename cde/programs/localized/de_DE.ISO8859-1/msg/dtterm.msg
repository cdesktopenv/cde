$ $TOG: dtterm.msg /main/7 1999/09/17 13:27:39 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
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
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************
$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************
$quote "

$set 1
$ ****** Module: DtTermMain.c
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 "Sep             f.separator\n\
\tMenüleiste\\ umschalten  f.send_msg %d\t"

2 "%s: die Option -S kann nicht mit der Option -server oder -serverid verwendet werden\n"

3 "Terminal"

$set 2        Redefine set#

$ ****** Module: DtTermSyntax.c
$ These are the usage messages.
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 "-/+132\t\t\t\tAktivieren/InAktivieren der Escape-Sequenz für 80<->132 Spalten"

2 "-/+aw\t\t\t\tAktivieren/Inaktivieren des autom. Umbruchs (autowrap)"

3 "-bg Farbe\t\t\tHintergrundfarbe"

4 "-/+bs\t\t\t\tEin-/Ausschalten der Auswahlfarbe für Term-Hintergrund"

5 "-display Anzeigename\t\tX-Server, zu dem Verbindung hergestellt werden soll"

6 "-e Befehlsargumente\t\tAuszuführender Befehl"

7 "-fb Schriftartgruppe\t\t\tFette Textschrift"

8 "-fg Farbe\t\t\tVordergrundfarbe"

9 "-fn Schriftartgruppe\t\t\tNormale Textschrift"

10 "-geometry Geom.\t\t\tGröße (in Zeichen) und Position des Fensters"

11 "-help\t\t\t\tDiese Meldung ausgeben"

12 "-/+iconic\t\t\tSymboldarstellung starten/nicht starten"

13 "-/+j\t\t\t\tAktivieren/Inaktivieren des sprungweisen Bilddurchlaufs"

14 "-/+kshMode\t\t\tAktivieren/Inaktivieren des ksh-Modus"

15 "-/+l\t\t\t\tAktivieren/Inaktivieren der Protokollierung"

16 "-lf Dateiname\t\t\tDateinamenprotokollierung"

17 "-/+ls\t\t\t\tAktivieren/Inaktivieren der Anmelde-Shell"

18 "-/+map\t\t\t\tAktivieren/Inaktivieren des Zuordnungsfensters bei pty-Ausgabe"

19 "-/+mb\t\t\t\tAktivieren/Inaktivieren des Randsignals"

20 "-ms Farbe\t\t\tZeigerfarbe"

21 "-n Zeichenfolge\t\t\tAngabe des Symbolnamens"

22 "-name Zeichenfolge\t\t\tZeichenfolgen für Client-Instances, Symbole und Titel"

23 "-nb Abstand\t\t\tAngabe des Abstands für Randsignal rechts"

24 "-/+rw\t\t\t\tAktivieren/Inaktivieren des umgekehrten Umbruchs"

25 "-/+sb\t\t\t\tAktivieren/Inaktivieren der Bildlaufleiste"

26 "-/+sf\t\t\t\tAktivieren/Inaktivieren der SUN-Funktionstasten"

27 "-sl Anzahl[s]\t\t\tAnzahl der zu sichernden Blätterzeilen [screens]"

28 "-ti Name\t\t\tFür programmtechnische Identifikation verwendete Zeichenfolge"

29 "-title Zeichenfolge\t\t\tTitelzeichenfolge für Fenster"

30 "-tm Zeichenfolge\t\t\tSchlüsselwörter und Zeichen für Terminal-Modus"

31 "-tn Name\t\t\tName der Umgebungsvariable TERM"

32 "-/+vb\t\t\t\tAktivieren/Inaktivieren des optischen Signals"

33 "-xrm Ressourcenzeichenfolge\t\tZusätzliche Ressourcenspezifikationen"

34 "-C\t\t\t\tKonsolmodus"

35 "-Sxxd\t\t\t\tSlave-Modus bei \"ttyxx\"-Dateideskriptor \"d\""

36 "-Sxxx.d\t\t\tSlave-Modus bei \"ttyxxx\"-Dateideskriptor \"d\""

37 "End-Of-List"

38 "Die Option -e muß am Ende der Befehlszeile stehen. Andernfalls wird die"

39 "Standard-Shell des Benutzers gestartet. Durch Optionen, die mit einem"

40 "Pluszeichen beginnen, werden die Standardeinstellungen wiederhergestellt."

41 "End-Of-List"

47 "%s:  ungültige Befehlszeilenoption \"%s\"\r\n\n"

48 "Verwendung:  %s"

49 "\r\n\
   "

50 "[%s]"

51 "\r\n\n\
Für vollständige Beschreibung \"%s -help\" eingeben.\r\n\n"

52 "Verwendung:\n"

53 "\t%s [-options ...] [-e Befehlsargumente]\n\n"

54 "Mögliche Optionsangaben sind:\n"

$set 3        Redefine set#

$ ****** Module: TermFunction.c
$ This is a warning message dialog box

1 "SPEICHER VOLL\n\
Mit OK löschen"

$set 4        Redefine set#

$ ****** Module: TermPrim.c
$ This is the title of a dialog box

$ 1 OBS- "Dtterm - Warnung"

$ This is the title of a dialog box
2 "Terminal - Warnung"

$set 5        Redefine set#

$ ****** Module: TermFunction.c
$ This is a warning message dialog box

1 "SPEICHER VOLL\n\
Mit OK löschen"

$set 6        Redefine set#

$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.

1 "80x24 132x24"

$ 2 OBS- "Dtterm - Handbuchseite"

$ 3 OBS- "Dtterm - Hilfe"

$ Messages 4 and 5 are titles for dialog boxes.
4 "Terminal - Hilfeseite"

5 "Terminal - Hilfe"

$set 7        Redefine set#

$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title.
$ The rest are resource types and values - look at the dialog box and
$ documentation for full specs.
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 

1 "dtterm - Globale Optionen"

2 "OK"

3 "Abbrechen"

4 "Hilfe"

5 "global"

6 "Cursorsteuerung"

7 "Box"

8 "Unterstreichung"

9 "Cursordarstellung"

10 "Aktiviert"

11 "Deaktiviert"

12 "Blinkender Cursor"

13 "Blinkfrequenz (ms)"

14 "Unsichtbar"

15 "blinkRate"

16 "Farbsteuerung"

17 "Umgekehrt"

18 "Normal"

19 "Fensterhintergrund"

20 "Blätterverhalten"

21 "Deaktiviert"

22 "Aktiviert"

23 "Unterbrechungsfreier Bilddurchlauf"

24 "Signalsteuerung"

25 "Akustisch"

26 "Optisch"

27 "Signalart"

28 "Aktiviert"

29 "Deaktiviert"

30 "Randwarnung"

31 "Randabstand"

32 "Anwenden"

33 "Zurücksetzen"

34 "Terminal - Globale Optionen"

$set 8        Redefine set#

$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size").
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.

1 "Standard"

2 "Fenstergröße"

3 "F"

4 "-iso8859-1"

5 "%d Punkt"

6 "%.1f Punkt"

7 "%.1f Punkt"

8 "%.2f Punkt"

9 "Standard"

10 "Schriftgröße"

11 "S"
$   Window menu

12 "Erstellen"

13 "r"

14 "Drucken"

15 "D"

16 "Drucken..."

17 "k"

18 "Schließen"

19 "c"

20 "Fenster"

21 "F"
$   Edit menu

22 "Kopieren"

23 "K"

24 "Strg osfInsert"

25 "Strg+Einfg"

26 "Einfügen"

27 "E"

28 "Umschalt osfInsert"

29 "Umschalt+Einfg"

30 "Bearbeiten"

31 "B"
$   Option menu

32 "Menüleiste"

33 "M"

34 "Bildlaufleiste"

35 "B"

36 "Global..."

37 "G"

38 "Terminal..."

39 "T"

40 "Zurücksetzen ohne Löschen"

41 "o"

42 "Zurücksetzen mit Löschen"

43 "m"

44 "Zurücksetzen"

45 "Z"

46 "Optionen"

47 "O"
$ Old Help menu (no longer used for CDE)

48 "Überblick"

49 "b"

50 "Aufgaben"

51 "A"

52 "Referenzinformationen"

53 "R"

54 "Kontexthilfe"

55 "K"

56 "Hilfe für Hilfe"

57 "H"

58 "Informationen zu dtterm"

59 "I"

60 "Hilfe"

61 "H"

62 "Alt+F4"

63 "dtterm"
$ Current CDE Help menu

64 "Überblick"

65 "b"

66 "Index"

67 "x"

68 "Inhaltsverzeichnis"

69 "I"

70 "Aufgaben"

71 "A"

72 "Referenzinformationen"

73 "R"

74 "Tastatur"

75 "T"

76 "Hilfe für Hilfe"

77 "H"

78 "Informationen zu dtterm"

79 "I"

80 "Hilfe"

81 "H"

82 "Informationen zu Terminal"

83 "Informationen zu Terminal"

$set 9        Redefine set#

$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog

1 "dtterm - Terminal-Optionen"

2 "OK"

3 "Abbrechen"

4 "Hilfe"

5 "Tastatursteuerung"

6 "Normal"

7 "Anwendung"

8 "Modus der Positioniertasten"

9 "Numerisch"

10 "Anwendung"

11 "Modus für numerischen Tastenblock"

12 "Nur Rücklauf (CR)"

13 "Rücklauf/Zeilenumbruch (CR/LF)"

14 "Eingabe für neue Zeile"

15 "Gesperrt"

16 "Sperre aufgehoben"

17 "Benutzerfunktionstasten"

18 "Fenstersteuerung"

19 "Aktiviert"

20 "Deaktiviert"

21 "Spaltenumbruch bei 132"

22 "Umbruch am Zeilenende"

23 "Umgekehrter Zeilenvorschub"

24 "Anwenden"

25 "Zurücksetzen"

26 "Terminal - Terminal-Optionen"

$set 10
$ ****** Module: sunDtTermServer.c
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Terminal - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
