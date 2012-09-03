$ "$XConsortium: dtlogin.msg /main/13 1996/12/20 15:03:16 rws $" 
$ *************************************<+>************************************* 
$ ***************************************************************************** 
$ ** 
$ **  File:        dtlogin.msg 
$ ** 
$ **  Project:     CDE dtlogin 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dtlogin 
$ ** 
$ ** 
$ ***************************************************************************** 
$ ** 
$ **     (c) Copyright 1990 Hewlett-Packard Company 
$ **     All Rights reserved 
$ ** 
$ ** 
$ ***************************************************************************** 
$ ** 
$ ** 
$ **  ------------------------- MODIFICATION RECORD -------------------------- 
$ * 
$ *  
$ **  ----------------------- MODIFICATION RECORD END ------------------------ 
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
$quote "

$set 1
$ set 1 is labels 
$  
$ messages 1 through 6 are button labels on the login screen 

1 "Bitte den Benutzernamen eingeben"

2 "Bitte das Kennwort eingeben"

3 "OK"

4 "Erneute Eingabe"

5 "Optionen"

6 "Hilfe"
$  
$ messages 7 through 11 are options button menu items 
$  

7 "Anmeldeanzeige zurücksetzen"

8 "Befehlszeilenanmeldung"

9 "Version..."

10 "einfache Motif-Session"

11 "Sprachen"
$  
$ messages 12 through 14 are miscellaneous button labels 
$  

12 "DT starten"

13 "Abbrechen"

14 "Hilfe"
$  
$ message 15, 17 are more option button menu items  
$ message 16 is the welcome message 
$  

15 "DT-Lite-Session"

16 "Willkommen auf Rechner"

17 "Normaler Desktop"
$  
$ message 18-24 
$  

18 "* Desktop-Anmeldung wird ausgesetzt...\r\n\
*\r\n"

19 "* Für Anmelde-Eingabeaufforderung Eingabetaste drücken.\r\n\
*\r\n"

20 "* Anmelden. Desktop-Anmeldung wird nach dem Abmelden wiederaufgenommen.\r\n\
*\r\n"

21 "* Desktop-Anmeldung wird wiederaufgenommen...\r\n\
*\r\n"

22 "* Starten der Desktop-Anmeldung auf Anzeige %1$s...\r\n\
*\r\n"

23 "* Vor dem Anmelden auf Desktop-Anmeldeanzeige warten.\r\n\
*\r\n"

24 "* Der X-Server kann nicht auf Anzeige %1$s gestartet werden...\r\n\
*\r\n"
$ messages 25 is a options button menu item 

25 "Session"
$  
$ 26 replaces 16 as the greeting after a user name has been  
$ entered. The %s is replaced by the user name. 
$  

26 "Willkommen: %s"

$ 
$ 27 replaces 10 as the label for the failsafe option menu item
$ 
$quote

27 Fehlerbehebungssession

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room  
$  for some shared sample source message additions. 
$ 
73 Letzter Desktop des Benutzers
$ 
$  Following used in Language menu.  
$  "to" used in phrases like "ja to zh" locales
74 bis


$set 2

$ set 2 is error messages 
$  

$quote "
1 "Ungültige Anmeldung; bitte wiederholen."

2 "Wechsel ins Benutzerverzeichnis nicht möglich."

3 "Keine Anmeldung möglich - maximale Anzahl von Benutzern angemeldet."
$ message 4 is a lengthy message that should be localized to correspond to  
$ the language.   

4 "\n\
Es muß erstmalig ein Kennwort für den Benutzereintrag \n\
eingegeben werden, oder das aktuelle Kennwort ist abgelaufen.\n\n\
Nach dem Festlegen des Kennworts mit diesem Kennwort \n\
erneut anmelden."

5 "Fehler beim Anmelden, ungültige Benutzerkennung."

6 "Fehler beim Anmelden, ungültige Gruppenkennung."

7 "Fehler beim Anmelden, ungültige Prüfkennung."

8 "Fehler beim Anmelden, ungültiges Prüfsignal."

9 "Anmeldungen sind momentan gesperrt."

10 "Nur Root darf %1$s ausführen\n"

11 "Standardsprache wird neu erstellt aus %1$s\n"

12 "Konfigurations- und Server-Dateien werden aktualisiert\n"

13 "Konfigurationsdatei %1$s wird erneut gelesen\n"

14 "Server-Datei %1$s wird erneut gelesen\n"

15 "Zugriffsdatei %1$s wird erneut gelesen\n"

16 "dtlogin: Kein Speicher mehr in Routine\n"

$set 3

$ set 3 is help text 
$  
$ message 1 is generated when the help button on the login screen is clicked. 
$ Comments for messsage #1:
$ 
$ Under [Options] there is an item called "Session".  Under
$ Session there are five items and these items have been
$ translated in other files.  To be consistent with the other
$ translations, the translations used below should be the same
$ as the other translations.  The items and their file are
$ listed below:
$ 
$   1. Current   - _common.session.tmsg ; set 1, message 2
$ 
$   2. Home      - _common.session.tmsg ; set 1, message 1
$ 
$   3. <display-name> - Current   - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Current" should
$         be the same as _common.session.tmsg ; set 1, message 2
$ 
$   4. <display-name> - Home      - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Home" should
$         be the same as _common.session.tmsg ; set 1, message 1
$ 
$ 
1 "\n\
Hilfe für Anmeldung:\n\
\n\
In der Anmelde-Anzeige erfolgt die Identifikation gegenüber dem System.\n\
\n\
Zum Anmelden wie folgt vorgehen:\n\
       1) Den Benutzernamen eingeben und die < Eingabetaste > drücken\n\
           oder auf [OK] klicken. (Falls noch kein Benutzer eingerichtet wurde,\n\
           für den Benutzernamen \"root\" eingeben).\n\
       2) Das Kennwort eingeben und die <Eingabetaste> drücken oder\n\
           auf [OK] klicken.\n\
[Erneut starten]\n\
   Mit dieser Auswahl werden die Eingaben gelöscht, sodaß Benutzername\n\
   und Kennwort erneut eingegeben werden können.\n\
\n\
[Optionen]\n\
   Mit dieser Auswahl können die Session-Optionen geändert werden:\n\
\n\
   Wenn sich der Mauszeiger über [Optionen] befindet, Maustaste 1\n\
   drücken und gedrückt halten.  Den Zeiger über die Auswahlmöglichkeiten\n\
   ziehen. Durch Loslassen der Maustaste über einer Auswahl wird der\n\
   entsprechende Auswahlpunkt ausgewählt.\n\
   Folgende Optionen stehen zur Auswahl:\n\n\
   Sprachen - zum Ändern der Session-Sprache\n\
   Session - zum Auswählen einer grafischen Umgebung\n\
      Aktuelle - Startet Ihre zuletzt gestartete Session\n\
      Standard - Startet Ihre Standardsession (sofern eingerichtet)\n\
      <Anzeigename> - Aktuelle - startet Ihre zuletzt gestartete Session\n\
          für die jeweilige Anzeige\n\
      <Anzeigename> - Standard - startet Ihre Standardsession für\n\
          die jeweilige Anzeige\n\
      Fehlerbehebungssession - startet eine Fehlerbehebungssession.\n\
   Befehlszeilenanmeldung - zum Umschalten in eine Befehlszeilenumgebung\n\
   Anmeldeanzeige zurücksetzen - zum Neustarten des X-Servers\n\
\n\
Zum Aktualisieren der Anzeige mit einer Maustaste auf dem Hintergrund\n\
z.B. wenn die Anzeige aufgrund von Systemmeldungen nicht\n\
mehr lesbar ist.\n\
\n\
Wird weitere Hilfe benötigt, zunächst anmelden und anschließend auf das\n\
Symbol für die Hilfeverwaltung im Popup-Menü des Informationsmanagers\n\
im Bedienfeld klicken (Buch mit dem Informationssymbol).\n\
Anschließend im Hilfeband für die Anmeldeverwaltung nachschlagen.\n"


$ message 2 is generated when you attempt to start a system whose id is 
$ set to unknown or if it is not configured for networking. 

2 "\n\
Der Rechner hat keinen eindeutigen Namen. Der Name wurde\n\
gegenwärtig auf \"unknown\" gesetzt. Es wird empfohlen dem\n\
Rechner einen anderen Namen zu geben.\n\n"
$ message 3 is generated when the help button on the chooser screen is clicked. 

3 "\n\
Hilfe zur Auswahl:\n\n\
Zur Auswahl des Servers an dem die Anmeldung erfolgen\n\
soll, den Auswahlschirm verwenden.\n\n\
Der Auswahlschirm enthält eine Liste mit jeweils zwei\n\
Teilen für jeden Eintrag. Der erste Teil (Server-Name)\n\
beinhaltet den Netzwerknamen für jeden verfügbaren\n\
Server. Der zweite Teil (Server-Information) beinhaltet\n\
unterschiedliche Informationen, abhängig von der\n\
Anmeldesteuerung, die auf dem jeweiligen Server\n\
ausgeführt wird.\n\
Sofern ein Server den CDE Desktop ausführt, sollte\n\
er Informationen zu der Benutzeranzahl (oder\n\
tty-Geräteanzahl), die aktuell am System arbeitet,\n\
und zu der durchschnittlichen Lastverteilung\n\
(durchschnittliche CPU-Verwendung) der letzten 5, 10,\n\
und 15 Minuten bereitstellen.\n\n\
Um den Server, an dem die Anmeldung erfolgen soll,\n\
auszuwählen, die Zeile mit dem Namen des gewünschten\n\
Servers aus der Liste auswählen.\n\n\
Nach erfolgter Auswahl die Taste 'OK' drücken. Dadurch\n\
wird der Anmeldeschirm des ausgewählten Servers angezeigt.\n\n\
[Liste aktualisieren]\n\
   Löscht die Liste und erstellt sie nach einer\n\
   erneuten Abfrage der Server im Netzwerk, die\n\
   Anmeldungen erlauben.\n\n\
[Optionen]\n\
   Ermöglicht das Ändern der Optionen von Sessions:\n\n\
   Den Mauszeiger auf [Optionen] stellen, die linke\n\
   Maustaste drücken und festhalten. Den Zeiger über\n\
   die Auswahlpunkte ziehen. Durch Loslassen der\n\
   Maustaste wird das Element unter dem Zeiger\n\
   ausgewählt. Folgende Auswahl steht zur Verfügung:\n\n\
   Sprache                     - um die Sprache für die\n\
                                 Session zu ändern\n\
   Anmeldefenster zurücksetzen - Um den X-server\n\
                                 neu zu starten\n\n\
Mit einer beliebigen Maustaste auf den Hintergrund\n\
klicken, um die Anzeige zu erneuern. Z.B. wenn durch\n\
eine Systemmeldung die Anzeige unleserlich wird.  \n\n"

$set 4

$ set 4 is log error messages 
$  

1 "Zugriffsdatei \"%1$s\", Host \"%2$s\" nicht gefunden\n"

2 "ReadHostEntry\n"

3 "Zugriffsdatei %1$s, Anzeige %2$s unbekannt\n"

4 "Kein Zugriff auf Steuerdatei %1$s, es werden keine XDMCP-Anforderungen ausgeführt\n"

5 "Fehler bei Identifikationsüberprüfung von Domänen/OS: %1$s (%2$s/%3$s)"

6 "Server-Berechtigungsdatei %1$s kann nicht geöffnet werden\n"

7 "Server-Berechtigungsdatei %1$s kann nicht geschrieben werden\n"

8 "saveAddr"

9 "Fehler beim Abrufen der Netzwerk-Schnittstellen-Konfiguration"

10 "setAuthNumber"

11 "Berechtigungsdatei %1$s oder Sicherung %2$s kann nicht gesperrt werden\n"

12 "Berechtigung kann nicht positioniert werden\n"

13 "pid-Datei  %1$s kann nicht erstellt/gesperrt werden\n"

14 "pid-Datei %1$s, anderer Dtlogin wird ausgeführt (pid %2$d)\n"

15 "Kein Zugriff auf Server-Datei %1$s\n"

16 "NewDisplay"

17 "errorLogFile %1$s kann nicht geöffnet werden\n"

18 "TrimErrorLog(): Maximale Größe der Fehlerprotokolldatei ist 200 KB.\n"

19 "Fehlender Bildschirmname in Server-Datei\n"

20 "Fehlender Bildschirmtyp für %1$s\n"

21 "Rechnername %1$s ungültig. DISPLAY wird auf ':0' gesetzt.\n"

22 "Bildschirmtyp %1$s für Bildschirm %2$s nicht akzeptiert\n"

23 "Zeitlimitüberschreitung bei Pseudo-Zurücksetzen.\n"

24 "GetResource"

25 "kein Speicherbereich für Argumentneuzuordnung\n"

26 "Konfigurationsdatei %1$s kann nicht geöffnet werden\n"

27 "Zusätzliche Argumente in Befehlszeile:"

28 "StartServer(): keine Argumente\n"

29 "Server %1$s kann nicht ausgeführt werden\n"

30 "StartServer() fork gescheitert, inaktiv (sleeping)\n"

31 "Server unerwartet 'gestorben'\n"

32 "Blockierung bei XOpenDisplay(%1$s) #%2$d, Abbruch.\n"

33 "Öffnen des Servers #%1$d für %2$s gescheitert, kein erneuter Versuch\n"

34 "Schwerwiegender E/A-Fehler %d (%1$s)\n"

35 "X-Fehler\n"

36 "Sitzungsstart gescheitert\n"

37 "ACHTUNG: Bildschirm %1$s konnte nicht gesichert werden\n"

38 "WARNUNG: Tastatur an Bildschirm %1$s konnte nicht gesichert werden\n"

39 "BLS - Benutzerinformationen nicht abrufbar.\n"

40 "BLS - Geschützte Benutzerinformationen nicht abrufbar.\n"

41 "BLS - Sensitivitätsstufe konnte nicht überprüft werden.\n"

42 "BLS - Wechsel zu Benutzer %1$s nicht möglich.\n"

43 "AFS - setgroups()-Fehler bei Sitzungsstart\n"

44 "Setuid für Benutzer %s gescheitert, errno = %d\n"

45 "Kein Standard-Verzeichnis %1$s für Benutzer %2$s, / wird verwendet\n"

46 "Session-Ausführung gescheitert: %1$s\n"

47 "Session verfügt über keinen Befehl/keine Argumente\n"

48 "Session kann nicht für %s gestartet werden, fork gescheitert\n"

49 "Dtlogin kann Client nicht abbrechen (kill)\n"

50 "%1$s kann nicht ausgeführt werden\n"

51 "fork für Ausführung von %1$s nicht möglich\n"

52 "Ungültiger Identifikationsname \"%1$s\". AFS, Kerberos oder Standard verwenden.\n"

53 "Dtgreet %1$s kann nicht für %2$s ausgeführt werden\n"

54 "fork für Ausführung von Dtgreet %1$s nicht möglich\n"

55 "Socket-Erstellung bei Anschluß %1$d gescheitert\n"

56 "Fehler beim Binden des Socket-Adresse %1$d, errno = %2$d\n"

57 "makeEnv"

58 "setEnv"

59 "parseArgs"

60 "Beim Durchsuchen des Verzeichnisses %1$s wurden %2$d Sekunden überschritten. \n\
Suchvorgang wird abgebrochen...\n"

61 "%s gehört mehr als %1$d Gruppen an, %2$s ignoriert\n"

62 "Fehler bei audwrite(): aufrufender Benutzer ist nicht Superuser\n"

63 "Fehler bei audwrite(): ungültige Ereignisnummer in Prüfsatz.\n"

64 "Fehler bei audwrite(): errno = %1$d\n"

65 "AFS-Identifikationsüberprüfung gescheitert: %1$s\n"

66 "Benutzereintrag für %1$s abgelaufen. Systemadministrator verständigen.\n"

67 "Server inaktiv (dead)  %1$s\n"

68 "Logo-Bitmap-Datei %1$s kann nicht geöffnet werden\n"

69 "Nachrichtenkatalog \"%1$s\" kann nicht gelesen werden. Standardsprache wird verwendet."

70 "Bildschirm %1$s konnte nicht initialisiert werden, Server möglicherweise blockiert.\n"

71 "(%1$s): Breite der Anmeldemaske überschreitet Bildschirmbreite. \n\
Maske wird in Pixel angegeben.\n"

72 "(%1$s) Höhe der Anmeldemaske überschreitet Bildschirmhöhe, Standardwert wird verwendet. \n\
Maske wird in Pixel angegeben.\n"

$set 5

$ set 5 is for other stuff specific to chooser. 

1 "Server für Anmeldung auswählen:"
$ 1 is the title/instructions for the chooser dialog 

2 "Server-Name                    Server-Information"
$                                ^ 
$                                | This needs to start in this column 
$ 2 is the headings for the list 

3 "Liste aktualisieren"

$set 99

$    DO NOT TRANSLATE THESE MESSAGES 
$    DO NOT CHANGE THESE MESSAGES 
$    DO NOT LOCALIZE THESE MESSAGES 
$    These messages are used for the version information. 

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/13 1996/12/20 15:03:16 rws $"
