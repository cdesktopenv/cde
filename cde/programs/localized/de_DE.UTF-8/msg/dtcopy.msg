$ "$XConsortium: dtcopy.msg /main/7 1996/11/19 18:07:32 rws $"
$ ***************************************************************************** 
$ ** 
$ **  File:        dtcopy.msg 
$ ** 
$ **  Project:     CDE File Manager 
$ ** 
$ ***************************************************************************** 

$quote "

$set 2        Define initial set#

$ ****** Module: main_dtcopy.c ****** 

3 "Datei"

4 "Dateien"

5 "Unterordner"

6 "Unterordner"

7 "Fehler: keine"

8 "Fehler: %d"

10 "Verwendung: %s [Optionen ...] Quelle Ziel\n"

11 "Dateimanager - Verzeichnis verschieben"

12 "Dateimanager - Verzeichnis kopieren"

13 "%s konnte aufgrund von Fehlern nicht entfernt werden."

14 "Beendet."

15 "Abbruch bei"
$ ** Message 20 - 32 
$ ** LongFileOpNames ... message number = op + 20. 

20 "Verarbeitung"

21 "Öffnen"

22 "Öffnen"

23 "Öffnen"

24 "Lesen eines Links"

25 "Löschen"

26 "Kopieren"

27 "Erstellen eines Verzeichnisses"

28 "Kopieren eines Links"

29 "Erstellen eines Links"

30 "Verarbeitung"

31 "ist eine Kopie"

32 "ist ein Link"

33 "wird umbenannt"

$set 3        Redefine set#

$ ****** Module: copydialog.c ****** 

1 "Wiederaufnehmen"

2 "Unterbrechen"

3 "Verschieben von:"

4 "Kopieren von:"

5 "nach:"

7 "Kopiervorgang wird abgebrochen.\n\
Die bereits kopierte Dateien verbleiben im Zielverzeichnis."

10 "Verschieben wird abgebrochen.\n\
Die bereits verschobene Dateien verbleiben im Zielverzeichnis."

11 "Ordner kopieren - Warnung"

12 "Ordner kopieren - Warnung"

13 "Kopieren abbrechen"

14 "Kopieren vortsetzen"

15 "Verschieben abbrechen"

16 "Verschieben vortsetzen"

17 "verschoben"

18 "kopiert"

19 "Kann den Ordner nicht auf sich selbst verschieben\n\
%s\n"

20 "Kann den Ordner nicht in sich selbst kopieren\n\
%s\n"


$set 4        Redefine set#

$ ****** Module: errordialog.c ****** 

1 "Fehler %s\n\
%s"

2 "Bestätigung bei zukünftigen Fehlern?"

3 "Ordner kopieren - Fehler"

4 "Ordner verschieben - Fehler"

5 "Objekt verschieben - Fehler"

6 "Objekt kopieren - Fehler"

7 "Objekt in Papierkorb - Fehler"

8 "Das Objekt \n\n\
    %s\n\n\
kann aufgrund fehlender Berechtigungen nicht in den \n\
Papierkorb gestellt werden.\n\n\
Im Dialogfenstermenü des Objekts oder im Menü 'Ausgewählt'\n\
die Option 'Berechtigungen ändern' auswählen, um die \n\
Leseberechtigung für das Objekt zu aktivieren.\n\n\n\
Anmerkung: Handelt es sich bei diesem Objekt um einen \n\
Ordner, muß auch Leseberechtigung für alle Objekte \n\
im Ordner vorliegen, bevor der Ordner in den Papierkorb \n\
gestellt werden kann."

9 Kann nicht geöffnet werden %s


$set 5        Redefine set#

$ ****** Module: overwrtdialog.c ****** 

1 "Eine Datei mit dem Namen\n\
%s, \n\
existiert schon in\n\
%s"

2 "Ordner verschieben - Datei überschreiben"

3 "Vorhandene Datei überschreiben"

4 "Vorhandene Datei umbenennen in:"

5 "Neues Objekt umbenennen in:"

6 "Diese Datei auslassen"

7 "Gleiches Verfahren bei zukünftigen Namenskonflikten?"

8 "Aktion:"

9 "Ordner kopieren - Datei überschreiben"

10 "%s\n\
existiert bereits: einen neuen Namen eingeben."

11 "Datei überschreiben - Fehler"

12 "%s\n\
kann nicht umbenannt werden in\n\
%s\n\
%s"

$set 6        Redefine set#

$ ****** Module: utils.c ****** 
$ ** Button labels. 

1 "OK"

2 "Unterbrechen"

3 "Abbruch"

4 "Hilfe"

5 "Hilfe für Verzeichnis kopieren"

6 "Weiter"

7 "Kopieren abbrechen"

8 "Verschieben abbrechen"
