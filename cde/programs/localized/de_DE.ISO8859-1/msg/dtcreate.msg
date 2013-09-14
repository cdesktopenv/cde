$ $TOG: dtcreate.msg /main/4 1999/09/17 17:22:35 mgreess $
$ %Z%%M%        %I%  %W% %G% %U% 
$  
$   COMPONENT_NAME: dtcreate 
$  
$   FUNCTIONS: none 
$  
$   ORIGINS: 27 
$  
$  
$   (C) COPYRIGHT International Business Machines Corp. 1993 
$   All Rights Reserved 
$   Licensed Materials - Property of IBM 
$   US Government Users Restricted Rights - Use, duplication or 
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp. 
$  
$ **   Description: 
$ **   ------------ 
$ **   This is the source message catalog file for dtcreate. 
$quote "

$set 2
$ ***************************************************************************** 
$ ** 
$ ** Message set for menu options on main window 
$ ** 
$ ***************************************************************************** 

10 "Datei"

11 "D"

15 "Öffnen..."

16 "f"

20 "Erstellen"

21 "E"

25 "Speichern"

26 "p"

30 "Beenden"

31 "B"

35 "Optionen"

36 "O"

40 "Erweiterte Funktionen anzeigen"

41 "E"

42 "Symbolansicht"

43 "S"

44 "Farbe"

46 "F"

47 "Monochrom"

48 "M"

49 "Monochromsymbole anzeigen"

50 "M"

53 "Hilfe"

54 "H"

60 "Überblick"

61 "b"

65 "Inhaltsverzeichnis"

66 "I"

70 "Aufgaben"

71 "A"

75 "Referenz"

76 "R"

80 "Hilfe für Hilfe"

81 "H"

85 "Informationen zu Aktion erstellen"

86 "e"

$set 3        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Message set for  main Create Action window 
$ ** 
$ ***************************************************************************** 

10 "Aktion Erstellen"

12 "Aktionsname (Symbolname):"

13 "Aktionssymbole:"

20 "Befehl bei Doppelklicken auf Aktionssymbol:"

21 "Bei Doppelklicken auszuführender Befehl:"

22 "Befehl beim Öffnen der Aktion (durch Doppelklicken):"

30 "Hilfetext für Aktionssymbol:"

32 "Fensterart:"

34 "Grafisch (X-Window)"

36 "Terminal (automatisches Schließen)"

38 "Terminal (manuelles Schließen)"

40 "Keine Ausgabe"

50 "Erweitert"

52 "Basis"

$set 4        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Message set for  main Create Action window advanced function area 
$ ** 
$ ***************************************************************************** 

10 "Optional"

11 "Beim Öffnen einer Aktion Benutzer nach folgendem fragen:"

12 "Dateitypen, die diese Aktion verwenden:"

13 "Hinzufügen..."

14 "Löschen"

15 "Bearbeiten..."

16 "Für Übergabe geeignete Datentypen:"

17 "Alle Dateitypen"

18 "Nur obige Liste"

$set 5        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Error and Information message set for  main Create Action window 
$ ** 
$ ***************************************************************************** 

10 "Der Aktionsname fehlt.\n\
Bitte einen Namen im Feld 'Aktionsname' eingeben."
$  

15 "Der Ausführungsbefehl fehlt.\n\
Bitte einen Befehl im Feld\n\
'Befehl beim Öffnen der Aktion (durch Doppelklicken):' eingeben."
$  

20 "Die Liste 'Dateitypen, die diese Aktion verwenden' ist leer.\n\
Es wurde 'Nur obige Liste' im Feld 'Für Übergabe geeignete \n\
Datentypen' ausgewählt.\n\
Die Auswahl 'Für Übergabe geeignete Datentypen' in 'Alle Datentypen'\n\
ändern oder mindestens einen Datentyp in der Liste hinzufügen."
$  

25 "Es ist bereits eine Aktions- und Dateitypdefinitionsdatei \n\
mit diesem Aktionsnamen vorhanden.\n\
Es besteht die Möglichkeit, das Speichern abzubrechen und \n\
den Aktionsnamen zu ändern oder die vorhandene Definitionsdatei\n\
zu überschreiben.\n\
Soll die vorhandene Definitionsdatei überschrieben werden?"
$  

30 "Es fehlt ein Variablenname im Feld 'Befehl beim Öffnen der \n\
Aktion (durch Doppelklicken):'. Einen Variablennamen für die \n\
im Feld 'Beim Öffnen einer Aktion Benutzer nach folgendem \n\
fragen' angegebenen Kennzeichnung hinzufügen.\n\
Gültige Variablennamen sind $*, $1, $2, ..., $9."
$  

35 "Diese Änderungen wurden nicht gespeichert.\n\
Sollen dieser Änderungen vor Beenden gespeichert werden?"
$  

40 "Die aktuellen Änderungen wurden nicht gespeichert.\n\
Soll diese Aktion gespeichert werden, bevor der Inhalt\n\
der Felder für die Aktion 'Neu' gelöscht wird?"
$  

45 "Die aktuellen Änderungen wurden nicht gespeichert.\n\
Sollen diese Änderungen gespeichert werden, bevor der \n\
Inhalt der Felder zum Laden einer anderen \n\
Aktionsdefinitionsdatei gelöscht wird?"
$  

50 "Der Aktionsname enthält ein oder mehrere unzulässige\n\
Zeichen.\n\
Folgende Zeichen dürfen in einem Namen nicht verwendet \n\
werden:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$  

55 "Folgende Datei konnte nicht erstellt werden:"
$  

60 "Folgende Datei konnte nicht geöffnet werden:"
$  

65 "Folgende Datei konnte nicht geladen werden, da sie\n\
ursprünglich nicht durch das Hilfsprogramm \"Aktion Erstellen\" \n\
erstellt wurde:"
$  

66 "Folgende Datei konnte nicht geladen werden, da sie auf\n\
andere Art als durch 'Aktion Erstellen' geändert wurde.\n\
Eine mögliche Ursache könnte das Ändern der Datei mit \n\
einem Texteditor oder einem anderen Programm sein."
$  

67 "Mit 'Aktion Erstellen' können nur Dateien bearbeitet\n\
werden, die mit 'Aktion Erstellen' erstellt und \n\
bearbeitet wurden."
$  

68 "Folgende Datei konnte nicht geladen werden:"
$  
$  The following messages are all part of the same message dialog 

70 "Die neue Aktion"

71 "wurde in den Standardordner gestellt."

73 "Die erstellte Aktionsdefinitionsdatei trägt folgenden Namen:"
$  

80 "Es konnten nicht alle Größen des ausgewählten Symbols\n\
gefunden werden."
$  

85 "Der Symboleditor konnte nicht gestartet werden."
$  

90 "Die folgende Datei konnte nicht analysiert werden:"
$  

95 "Folgende Symboldatei konnte nicht innerhalb des \n\
Symbolsuchpfades gefunden werden."
$  

98 "Das ausgewählte Symbol ist nicht im Symbolsuchpfad enthalten.\n\
Das Symbol muß in ein Verzeichnis im Symbolsuchpfad verschoben\n\
werden, damit daß Symbol in der Desktop-Umgebung richtig \n\
angezeigt wird.\n\
'Ok' auswählen, wenn dieses Symbol verwendet werden soll.\n\
'Abbruch' auswählen, wenn ein anderes Symbol ausgewählt werden soll."
$  
$ ONLY TRANSLATE  "Usage"  and  "filename" 

110 "Verwendung: dtcreate [<Dateiname>] [-?]"
$  

120 "Es wurde ein Dateityp ausgewählt, der sich nicht in der Liste\n\
'Dateitypen, die diese Aktion verwenden' befindet."

125 "Den zu löschenden Datentyp auswählen."

130 "Den zu editierenden Datentyp auswählen."
$  

150 "Die aktuelle Arbeit wurde nicht gespeichert."

152 "Nicht speichern"

154 "Schließen abbrechen"
$  

160 "Session konnte nicht in Datei gespeichert werden:"

162 "Session konnte nicht aus Datei wiederhergestellt werden:"
$  
$ TRANSLATORS: DO NOT TRANSLATE the word "HOME" in the following message 

170 "Die Umgebungsvariable HOME ist nicht gesetzt.\n\
Damit diese Aktion gesichert werden kann, muß die \n\
Umgebungsvariable HOME gesetzt sein."

$set 6        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** General message set for dialogs 
$ ** 
$ ***************************************************************************** 

10 "Ok"

11 "Anwenden"

12 "Abbruch"

13 "Hilfe"

14 "Inhalt löschen"

18 "Gruppe suchen"

19 "Gruppe suchen..."

20 "Symbol bearbeiten..."

31 "Aktion Erstellen - Fehler"

41 "Aktion Erstellen - Bestätigung"

46 "Aktion Erstellen - Information"

51 "Aktion Erstellen - Frage"

$set 7        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Message set for Add Datatype and Edit Datatype dialogs 
$ ** 
$ ***************************************************************************** 

10 "Dateityp hinzufügen"

11 "Dateityp bearbeiten"

14 "Name der Dateitypfamilie:"

15 "Identifizierende Merkmale:"

16 "Bearbeiten..."

17 "Hilfetext für Dateitypsymbol:"

18 "Dateitypsymbole:"

21 "Befehle"

22 "Befehl zum Öffnen dieses Dateityps:"

23 "Befehl zum Drucken dieses Dateityps:"

30 "Öffnen"

31 "Drucken"

$set 8        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Error and Information message set for 
$ **    Add Datatype and Edit Datatype dialogs 
$ ** 
$ ***************************************************************************** 

10 "Der Name der Dateitypfamilie fehlt.\n\
Einen Namen im Feld 'Name der Dateitypfamilie' eingeben."
$  

15 "Die identifizierenden Merkmale fehlen.\n\
Die Merkmale müssen angegeben werden, bevor der Dateityp\n\
erstellt werden kann. Zum Angeben der Merkmale den Knopf\n\
'Bearbeiten' neben der Liste 'Identifizierende Merkmale'\n\
auswählen."
$  

20 "Das Feld 'Name der Dateitypfamilie' enthält ein oder mehrere\n\
ungültige Zeichen.\n\
Folgende Zeichen dürfen nicht in Namen verwendet werden.\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$  

25 "Eine Dateitypfamilie mit diesem Namen ist bereits vorhanden.\n\
Bitte den Namen im Feld 'Name der Dateitypfamilie' ändern."

$set 9        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Message set for  Identifying Characteristics dialog 
$ ** 
$ ***************************************************************************** 

10 "Identifizierende Merkmale"

11 "Alle einschließen"

12 "Dateien"

13 "Ordner"

14 "mit folgender Übereinstimmung:"

20 "Namensmuster:"

21 "Berechtigungsmuster:"

22 "Lesbar"

24 "Beschreibbar"

25 "Ausführbar"

27 "UND"

28 "Inhalt:"

29 "Muster:"

30 "Typ:"

31 "Zeichenfolge"

32 "Byte (Hex)"

33 "Kurz (Dezimal)"

34 "Lang (Dezimal)"

35 "Startbyte:"

40 "Lesen:"

41 "Schreiben:"

42 "Ausführen:"

43 "Ein"

44 "Aus"

45 "Egal"

$set 10        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Error and Information message set for Identifying Characteristics dialog 
$ ** 
$ ***************************************************************************** 

10 "Namensmuster fehlt.\n\
Bei Auswahl von 'Namensmuster' muß ein \n\
Namensmuster eingegeben werden."
$  

15 "Berechtigungsauswahl fehlt.\n\
Bei Auswahl von 'Berechtigungsmuster' muß\n\
mindestens ein Auswahlpunkt unter \n\
'Berechtigungsmuster' ausgewählt werden."
$  

20 "Inhaltsmuster fehlt.\n\
Bei Auswahl von 'Inhalt' als Muster muß ein Eintrag\n\
im Musterfeld unter 'Inhalt' eingegeben werden."
$  

25 "Angabe für Startbyte fehlt.\n\
Bei Auswahl von 'Inhalt' muß eine Zahl im Feld\n\
'Startbyte' eingegeben werden."
$  

30 "Es wurde kein identifizierendes Merkmal ausgewählt.\n\
Mindestens ein Merkmal auswählen (Namensmuster, \n\
Berechtigungsmuster oder Inhalt)."
$  

45 "Das Namensmuster enthält ein oder mehrere ungültige\n\
Zeichen.\n\
Folgende Zeichen dürfen in einem Namensmuster nicht\n\
verwendet werden:\n\
  / \\ \" ' ;  (  ) ~ ! $ { } < > # @ | &"
$  

46 "Das Inhaltsmuster stimmt nicht mit dem Format für den\n\
gewählten Inhaltstyp überein.\n\
Das Inhaltsmuster im richtigen Format eingeben oder den\n\
Typ so ändern, daß er mit dem Inhaltsmuster übereinstimmt."
$  

47 "Das Feld 'Startbyte' enthält ungültige Zeichen.\n\
In diesem Feld können nur Zahlen in Dezimalformat\n\
eingegeben werden."
$  

48 "Bei Verwendung des Inhaltsmusters wird die Leistung der\n\
Dateiverwaltung stark beeinträchtigt.\n\
Die Dateiverwaltung muß in diesem Fall den Inhalt aller\n\
Dateien in einem Ordner durchsuchen, bevor ein Ordner \n\
geöffnet werden kann. Das Öffnen von Ordnern dauert \n\
daher länger.\n\
Aus diesem Grund ist es ratsam, nur Namens- und \n\
Berechtigungsmuster zu verwenden."

$set 11        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Message set for  Icon Selection Dialog 
$ ** 
$ ***************************************************************************** 

10 "Verzeichnisse"

11 "Symboldateien"

12 "Den Namen der Symboldatei eingeben"

20 "Symbole werden geladen. Bitte warten."

30 "Symbolordner"

50 "Der eingegebene Name der Symboldatei ist ungültig.\n\
Den Namen einer gültigen Symboldatei im Feld \n\
'Den Namen der Symboldatei eingeben' angeben."
$ ONLY TRANSLATE  "Empty".   Do not translate the brackets "[" and "]". 

60 "[Leer]"

$set 12        Redefine set#

$ ***************************************************************************** 
$ ** 
$ ** Message set for  Open File Dialog 
$ ** 
$ ***************************************************************************** 

10 "Filter"

11 "Verzeichnisse"

12 "Dateien"

13 "Auswahl"

20 "Datei öffnen"

25 "Aktion erstellen - Öffnen"

$set 13
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Create Action - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
