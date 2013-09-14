$ *************************************<+>************************************* 
$ ***************************************************************************** 
$ ** 
$ **  File:        dthelp.msg 
$ ** 
$ **  Project:     CDE 1.0 Help 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for DtHelp  
$ ** 
$ ** 
$ ***************************************************************************** 
$ ** 
$ **     (c) Copyright 1990 Hewlett-Packard Company 
$ **         All Rights reserved 
$ ** 
$ ** 
$ **     There are two types of messages in this file: 
$ ** 
$ **     1) Messages that appear in dialogs or are displayed to the user. 
$ **        These messages are the default and they should ALL BE LOCALIZED. 
$ **        Note that these messages do NOT have any identification. 
$ ** 
$ **     2) Messages that should not be localized. 
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES. 
$ ** 
$ ***************************************************************************** 
$ ** 
$ ** 
$ **  ------------------------- MODIFICATION RECORD -------------------------- 
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the  
$ *               27th.  (Mike Wilson). 
$ * 
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze 
$ * 
$ **  ----------------------- MODIFICATION RECORD END ------------------------ 
$ ***************************************************************************** 
$ *************************************<+>************************************* 
$quote "

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c ***** 
$  
$ These Messages appear in the pulldown menus for Cde Help General Help 
$ dialog widget. 
$  
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the 
$ second is the mnemonic (e.g. 2 P).   
$  
$ Messages 1 through 6 are the pulldown menus for the File menu. 

1 "Drucken..."

2 "D"

3 "Schließen"

4 "c"

5 "Neues Fenster..."

6 "F"
$ Messages 7 through 8 are the pulldown menus for the Edit menu. 

7 "Kopieren"

8 "K "
$ Messages 9 through 10 are the pulldown menus for the Search menu. 

9 "Index..."

10 "I"
$ Messages 11 through 16 are the pulldown menus for the Navigate menu. 

11 "Hauptthema"

12 "T"

13 "Zurück"

14 "Z"

15 "Abfolge..."

16 "A"
$ Messages 17 through 28 are the pulldown menus for the Help menu. 

17 "Überblick - Hilfe für Hilfe"

18 "w"

19 "Inhaltsverzeichnis"

20 "I"

21 "Aufgaben"

22 "A"

23 "Referenzinformationen"

24 "R"

25 "Maus und Tastatur"

26 "M"

27 "Informationen zu Hilfe"

28 "b"
$  
$ Messages 29 through 36 are the menu labels and mnemonics for for the  
$ Help Dialog. 

29 "Datei"

30 "D"

31 "Suchen"

32 "S"

33 "Navigation"

34 "N"

35 "Hilfe"

36 "H"

37 "Bearbeiten"

38 "B"
$  
$  
$ >>>>> Skip #39 through #49 
$  
$ Messages 50 through 55 are used to display formatting errors within the 
$ help dialog and quick help dialog widgets. 
$  

50 "Zeichenfolgedaten konnten nicht formatiert werden."

51 "Dynamische Zeichenfolgedaten konnten nicht formatiert werden."

52 "Elektronische Handbuchseite (man page) konnte nicht formatiert werden. Die angeforderte elektronische Handbuchseite ist entweder nicht vorhanden oder beschädigt.Die angeforderte elektronische Handbuchseite ist entweder nicht vorhanden oder beschädigt."

53 "Textdateidaten konnten nicht formatiert werden. Die angeforderte Textdatei ist entweder nicht vorhanden oder beschädigt."

54 "Hilfethema konnte nicht formatiert werden."

55 "Nicht vorhandene Standort-ID:"
$  
$ Messages 56 and 67 are displayed when a hypertext link type is selected but 
$ not supported within the current application. 
$  

56 "Die ausgewählte Hypertext-Verbindung wird in dieser Anwendung nicht unterstützt."

57 "Verbindungen zu elektronischen Handbuchseiten (man pages) werden von dieser Anwendung nicht unterstützt."
$  
$ Messages 58 through 62  are used when bad or null help volumes or  
$ locationId's are used. 
$  

58 "Die angeforderte Online-Hilfe ist entweder nicht installiert oder befindet sich nicht im richtigen Suchpfad. Informationen zur Installation der Online-Hilfe enthält die Produktdokumentation.\n"

59 "Keine Standort-ID angegeben."

60 "Kein Hilfeband angegeben."

61 "Hilfeband:"

62 "Standort-ID:"
$  
$ >>>>> Skip #63 through #69 
$  
$  
$ Message 70 is used when a null value for DtNtopicTitle is used. 
$  

70 "Nicht vorhandener Thementitel."
$  
$ New messages added after early msg cat freeze; not in order 

80 "Der angegebene Hilfetyp ist ungültig."

$set 3        Redefine set#

$ ****  Module: History.c **** 
$ These messages are used as labels for the History Dialog. 

1 "Hilfe - Protokollanzeige"

2 "Aufgerufene Hilfebände:"

3 "Schließen"

4 "Hilfe"

5 "Hilfedateien"

6 "Hilfeseiten"

7 "Hilfenachrichten"

8 "Aufgerufene Hilfethemen:"

9 "Aufgerufene Hilfedateien:"

10 "Aufgerufene Hilfeseiten:"

11 "Aufgerufene Hilfenachrichten:"

$set 4        Redefine set#

$ *****  Module: Print.c  ***** 
$ These messages are used as labels for the Print Dialog. 
$ 1: dialog title 

1 "Hilfe - Drucken"
$ 10-12: standard printer dialog labels 

10 "Drucker:"

11 "Exemplare:"

12 "Titel des Deckblatts:"
$ 20-25: paper size label and sizes 

20 "Papierformat:"

21 "Letter-Format (8,5 x 11 Zoll)"

22 "Legal-Format (8,5 x 14 Zoll)"

23 "Executive-Format (7,25 x 10,5 Zoll)"

24 "A4 210 x 297 mm"

25 "B5 182 x 257 mm"
$ 30-33: help volume-specific printing options 

30 "Zu druckende Themen"

31 "Aktuelles Thema"

32 "Aktuelles Thema unter Unterthemen"

33 "Inhaltsverzeichnis und Index"

34 "Alle Themen im Hilfeband"
$ 40-42: buttons at bottom of dialog 

40 "Drucken"

41 "Abbrechen"

42 "Hilfe"
$ These messages are used to describe the kind of content to be printed 
$ 50 "Help Volume: " 

50 "Hilfeband: "

51 "Hilfemeldung"

52 "Hilfedatei"
$ 53 "Manual Page: " 

53 "Hilfeseite:"

54 "Fehlermeldung"
$ Don't translate message 60 unless it is not an empty string 
$ This is the default value that goes in the Printer field 
$ 60 " " 

60 ""
$ Don't translate message 61 
$ This is the default value that goes in the Copies field 

61 "1"

$set 5        Redefine set#

$ *****  Module: GlobSearch.c  ***** 
$ Message 1 is the label used to label the dialog 

1 "Hilfe - Indexsuche"

2 "Suchen"

3 "Aktuell"
$ DO NOT TRANSLATE MESSAGE 4 
$ " (%s)" 

4 "(%s)"

5 "Alle Bände"

6 "Ausgewählt"

7 "Bände auswählen..."

8 "Anzeigen"

9 "Vollständiger Index"

10 "Einträge mit:"

11 "Gehe zu"

12 "Suche starten"

13 "Suche fortsetzen"

14 "Suche stoppen"

15 "Schließen"

16 "Hilfe"

17 "Suche nach Bänden... Gefunden: %d"

18 "Suche... Verbleibende Bände: %d"

19 "Keine Indexeinträge gefunden"

20 "Vollständiger Index"

21 "Einträge mit \"%s\""

22 "Keine Bände ausgewählt"

23 "Obige Suchoptionen auswählen"
$ Do NOT translate messages 25-31 
$ " " 

25 " "

26 "+"

27 "-"
$ "  +" 

28 "  +"
$ "  -" 

29 "  -"
$ 30 "       " 

30 "       "
$ 31 "         " 

31 "         "
$ DO translate messages 32 and above 
$ kind of topic messages 

32 "Hilfenachricht"

33 "Hilfedatei"

34 "Elektronische Handbuchseite (man page)"

35 "Unbekanntes Format"
$ specialized title of Search Volume Selection dialog 

40 "Hilfe - Bandauswahl suchen"
$ additional messages (late addition caused this ordering) 

50 "Keine Indexeinträge gefunden. In allen Bänden suchen."

51 "Keine Indexeinträge gefunden. Anderes Wort versuchen."

$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.

$quote
60 (

61 )

$set 6        Redefine set#
$ *****  Module: HelpAccess.c  ***** 
$ These messages are used for the Help on Help dialog in Cde Help widgets. 

$quote "
1 "Hilfe zum Hilfesystem"

2 "Drucken ..."

3 "Schließen"

4 "Keine Hilfe zum Hilfesystem bei dieser Anwendung!"

5 "Zurück"

6 "Hilfe ..."

$set 7        Redefine set#

$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  ***** 
$ These messages are used by the dthelpview program.  They need to be  
$ localized for dthelpview, however, they are not directly used by libDtHelp 
$ Messages 1 through 7 are command line usage response messages. 
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate 
$ the argument descriptor name (e.g. <Help Volume File>) 

1 "Verwendung: %s...\n"

2 "\t-helpVolume <Hilfeband>\n"

3 "\t-locationId  <Hilfeadresse>\n"

5 "\t-file  <ASCII-Textdatei>\n"

6 "\t-man\n"

7 "\t-manPage <Unix-Hilfeseite>\n\n\n"
$ man page dialog 

11 "Hilfeseite"

12 "Hilfeseite:"

13 "Hilfeseite anzeigen"
$ button labels 

16 "Beenden"

17 "Schließen"

18 "Vorheriges Thema"

19 "Drucken ..."
$ title of helpview window 

20 "Hilfeanzeige"

$set 8        Redefine set#

$ *****  Module: HelpUtil.c  ***** 
$ Message 1 is the label used in the definition link dialog box  
$ (e.g. Quick help close button) 

2 "Schließen"

3 "Interner Fehler: Kein Mauszugriff\n\
DtHelpReturnSelectedWidget abgebrochen.\n"

4 "Interner Fehler: Kein Tastaturzugriff\n\
DtHelpReturnSelectedWidget() abgebrochen.\n"

5 "Fehler bei Hilfe"
$ Messages 10 and up were added after message catalogs were taken 
$ for translation.  They are not in any order or chosen location. 

10 "Trotzdem ausführen"

11 "Nicht ausführen"

12 "Vom Hilfeband wurde versucht, einen Befehlsalias auszuführen.\n\
Der Alias '%s' ist nicht definiert."

13 "Vom Hilfeband wurde versucht, einen Befehlsalias auszuführen.\n\
Die automatische Befehlsausführung ist aus Sicherheitsgründen ausgeschaltet.\n\
Der Befehl lautet:  %s"

14 "Der Hilfeband wollte einen Befehl als root ausführen, das\n\
Home-Verzeichnis von  \"%s\"  ($HOME) stimmt jedoch nicht mit dem root\n\
Home-Verzeichnis des Benutzers überein. Dies könnte die Ausführung unerwarteter\n\
Befehle zur Folge haben.\n\
\n\
Der Befehl lautet:  \"%s\"\n\
\n\
Hinweis:  um dies in Zukunft zu vermeiden:\n\
 Führen Sie \"su - root\"  anstatt \"su root\" aus.\n"
$ End Message set 8 

$set 9        Redefine set#

$ *****  Module: PathArea.c  ***** 
$ These messages are used in the general Help Dialog Widget. 
$ Message 1 is the label used to label Topic Tree Area   

1 "Band: "
$ Messages 2 though 5 are used in the Button Box area of our general  
$ help dialog. 

2 "Zurück"

3 "Abfolge..."

4 "Index..."

5 "Oberste Ebene"

$set 10        Redefine set#

$ *****  Module: VolSelect.c  ***** 
$ Message 1 is the default label used to label the volume selection dialog. 

1 "Hilfe - Bandauswahl"

2 "Hilfebände"

3 "Ein oder mehrere Hilfebände auswählen"

4 "Schließen"

5 "Hilfe"

$set 11        Redefine set#

$ *****  Module: HelpQuickD.c  ***** 
$ These messages are used as labels for the quick help dialog. 

1 "Schließen"

2 "Weitere ..."

3 "Vorheriges Thema"

4 "Drucken ..."

5 "Hilfe ..."

$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES 
$    DO NOT CHANGE THESE MESSAGES 
$    DO NOT LOCALIZE THESE MESSAGES 
$    These messages are used for the version information. 

1 "@(#)version_goes_here"

2 "\n\
@(#)Message catalog source $XConsortium: DtHelp.msg /main/6 1996/10/19 14:26:01 rws $"
