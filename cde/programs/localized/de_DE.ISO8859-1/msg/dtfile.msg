$ "$TOG: dtfile.msg /main/11 1999/09/16 09:42:50 mgreess $" 
$ *************************************<+>************************************* 
$ ***************************************************************************** 
$ ** 
$ **  File:        dtfile.msg 
$ ** 
$ **  Project:     Common Desktop Environment dtfile 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dtfile 
$ **  Any additional messages are to be added to this file by hand, 
$ **  and the associated index number placed in the code. 
$ ** 
$ ***************************************************************************** 
$ ** 
$ **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company 
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
$ 03/25/93 H. Amro            removed the guilty $ from message 30 
$  
$  
$  
$ ***************************************************************************** 
$quote "

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)******** 
$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is 
$   used when the user has specified a title for dtfile windows 
$ Messages 2 and 4 are labels for the Change Folder dialog 
$ Message 1 is the title for Change Folder errors 
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages 

1 "Fehler beim Wechseln des Ordners."

2 "Neuer Ordner:"

3 "Ordner wechseln         "

4 "Rechner:"

7 "Der folgende Ordnername ist ungültig:\n\n\
%s:%s"

8 "In diesen Ordner kann nicht gewechselt werden. Es \n\
können nur Ordner unterhalb des aktuellen Ordner \n\
angezeigt werden. Es kann kein absoluter Pfad \n\
für den neuen Ordner angegeben werden."

9 "Diese Ansicht befindet sich im eingeschränkten Modus. \n\
Der Wechsel in den angegebenen Ordner ist nicht möglich, \n\
weil dieser sich nicht im relativen Pfad des \n\
eingeschränkten Ordners befindet.\n"

10 "Dateimanager - Ordner wechseln"

11 "Neuer Ordnername nicht abrufbar."

12 "Bitte Ordnernamen eingeben."

13 "Gehe zum Fehler"

14 "Zielordner:"

15 "Gehe zu"

16 "Systemname:"

17 "Dateimanager - Gehe zu"

18 "Name des Zielordners fehlt.\n\
   Einen Namen für den Ordner angeben oder von der Liste auswählen."

19 "Der folgende Ordnername ist ungültig.\n\n\
%s"

20 "Nicht möglich, zum Ordner\n\n\
%s\n\n\
zu wechseln, Dieser Ordner kann nicht angezeigt\n\
werden, da es sich um einen eingeschränkten Ordner handelt."

$set 3        Redefine set#

$ ****** Module: ChangeDirP.c ****(developer comment only)******** 
$ Messages 2-6 are labels for the folder Status Line 
$ Message 3 is required for SUN systems 
$ Message 2-9 are obsolete 

2 "%d Dateien %d verdeckt"

3 "%1$d Dateien %2$d verdeckt"

4 "%3d Dateien ..."

5 "Lesen ..."

6 "%d Verdeckt"

7 "%d Elemente %d Verdeckt"

8 "%1$d Elemente %2$d Verdeckt"

9 "%3d Elemente ..."

10 "%d Element(e)"

11 "%d Element(e)..."

12 "%d Element(e) %d Verdeckt"

$set 4        Redefine set#


$set 5        Redefine set#


$set 6        Redefine set#


$set 7        Redefine set#


$set 8        Redefine set#


$set 9        Redefine set#

$ ****** Module: File.c ****(developer comment only)******** 
$ Messages 6 and 7 are the title and text for an action error message 
$ Messages 8 and 9 are rename error messages 
$ Messages 11-19 are the title and text for warning messages displayed 
$   when an object/set of objects is moved/copied/linked to a folder 
$   containing object(s) with the same name 

6 "Aktionsfehler"

7 "Für %s sind keine Aktionen definiert.\n"

8 "Diese Datei wird bereits im Arbeitsbereich angezeigt \n\
und es ist existiert eine gleichnamige Datei im \n\
Arbeitsbereich vorhanden. Bitte einen anderen Namen \n\
vergeben."

9 "Gleichnamige Datei existiert bereits.\n\
Bitte einen anderen Namen vergeben."

11 "Dateimanager - Verschieben/Kopieren/Verweis Warnung"
$ Messages 12-15 deal with multiple files/folders 

12 "Folgende Dateien (oder Ordner)\n\
existieren bereits in\n\
%s:"

13 "\n\
Durch das Kopieren werden diese Dateien überschrieben. \n\
          Fortfahren?\n"

14 "\n\
Durch das Verschieben werden diese Dateien überschrieben. \n\
          Fortfahren?\n"

15 "\n\
Durch das Erstellen eines Links werden diese Dateien überschrieben. \n\
          Fortfahren?\n"
$ Messages 16&17 deal with a single file/folder 

16 "Ein Objekt mit Bezeichnung\n\
%s\n\
ist bereits im Ordner \n\
%s vorhanden"

17 "Ein Ordner mit der Bezeichnung\n\
%s\n\
ist bereits im Ordner\n\
%s vorhanden"
$ Labels in rename/replace and merge/replace dialogs 

20 "Wie soll fortgefahren werden?"

21 "Vorhandenes Objekt ersetzen"

22 "Vorhandens Objekt umbenennen in:"

24 "Inhalte der Ordner zusammenfügen"

25 "Vorhanden Ordner ersetzen"

27 "OK"

28 "Abbrechen"

29 "Hilfe"

30 "Verschiebvorgang abbrechen"

31 "Kopiervorgang abbrechen"

32 "Verweiserstellung abbrechen"
$ Window title for overwrite dialogs 

33 "Dateimanager - Warnung: Verschieben/Kopieren/Verweis"

34 "Dateimanager - Warnung: Verschieben"

35 "Dateimanager - Warnung: Kopieren"

36 "Dateimanager - Warnung: Verweis"

37 "Objekt umbenennen - Fehler"

38 "%s\n\
ist bereits vorhanden: Einen anderen Namen wählen."

39 "%s\n\
kann nicht ubenannt werden in\n\
%s\n\
%s"

40 "Objekt ersetzen - Fehler"

51 "kopiert wird"

52 "verschoben wird"

53 "Verweis erstellt wird"

54 "geändert wird"

56 "Vorhandene Objekte ersetzen"

57 "Vorhandene Objekte durch Anhängen einer Zahl umbenennen"

58 "kopiert"

59 "verschoben"

60 "ein Verweis erstellt"

61 "geändert"

68 "1 Objekt %s."

69 "%d Objekte %s."

70 "%s\n\
%d Objekte haben den gleichen Namen, wie Objekte im Ordner\n\
%s"

71 "%s\n\
1 Objekt hat den gleichen Namen, wie ein Objekt im Ordner\n\
%s"

72 "Kopiervorgang"

73 "Verschiebvorgang"

74 "mit einem Verweis erstellt"

75 "Operation"

76 "kopieren"

77 "verschieben"

78 "Verweis erstellen"

79 "Operation"

80 "%s abgebrochen - Der Ordner war nicht %s.\n\n\
Der zu ersetzende Ordner konnte nicht gelöscht werden, weil \n\
ein Objekt in dem Ordner nicht gelöscht werden konnte. Einige\n\
andere Objekte in diesem Ordner sind möglicherweise bereits\n\
gelöscht worden bevor der Prozess %s angehalten wurde."

81 "%s\n\n\
Die wahrscheinlichste Ursache für dieses Problem sind fehlende\n\
Schreibberechtigungen für das Objekt, das nicht gelöscht werden\n\
konnte. Die Schreibberechtigungen ändern, oder den Ordner, der\n\
%s umbenennen, so daß er einen anderen Namen hat."

82 "%s Abgebrochen\n\n\
%s\n\
kann nicht gelöscht werden.\n\n\
Die wahrscheinlichste Ursache für dieses Problem\n\
sind fehlende Schreibbrechtigungen für die Datei.\n\
Die Schreibberechtigungen ändern, oder die Datei, die\n\
%s umbenennen, so daß sie einen anderen Namen hat."

83 "%s abgebrochen - Keines der Objekte wurde %s.\n\n\
 Das folgende Objekt konnte nicht automatisch umbenannt werden\n\
%s\n\n\
%d Objekte wurden umbenannt bevor der Prozess %s angehalten wurde."

84 "%s\n\n\
Die wahrscheinlichste Ursache für dieses Problem sind fehlende \n\
Schreibberechtigungen für oben angegebenes Objekt. Die Berechtigungen\n\
ändern, oder das Objekt, das %s umbenennen, so daß es einen anderen\n\
Namen hat."

85 "%s abgebrochen - Keines der Objekte wurde %s.\n\n\
 Das folgende Objekt konnte nicht automatisch umbenannt werden\n\
%s\n\n\
1 Objekt wurde umbenannt bevor der Prozess %s anhalten wurde."

86 "%s abgebrochen - keines der Objekte wurde %s.\n\n\
Ein Ordner, der ersetzt werden sollte, konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Einige Objekte in dem Ordner wurden möglicherweise gelöscht  \n\
bevor der Prozeß %s angehalten wurde.\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache für dieses Problem sind fehlende\n\
Schreibberechtigungen für den oben genannten Ordner. Die \n\
Berechtigungen ändern, oder den Ordner, der %s umbenennen, so daß\n\
er einen anderen Namen hat."

87 "%s abgebrochen - keines der Objekte wurde %s.\n\n\
Ein Ordner, der ersetzt werden sollte, wurde nicht gelöscht.\n\
   Ordner: %s\n\n\
Einige Objekte in diesem Ordner wurden möglicherweise\n\
gelöscht, bevor der PRozess %s angehalten wurde.\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache für dieses Problem sind fehlende\n\
Schreibberechtigungen für oben genannten Ordner. Berechtigungen\n\
ändern oder den Ordner, der %s umbenennen, so daß er einen\n\
anderen Namen hat."

88 "%s abgebrochen - keines der Objekte wurde %s.\n\n\
Eine Datei, die ersetzt werden sollte, konnte nicht gelöscht werden.\n\
    Datei: %s\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache für dieses Problem sind fehlende\n\
Schreibberechtigungen für oben genannte Datei. Die Berechtigungen\n\
ändern, oder die Datei, die %s umbenennen, so daß sie einen anderen\n\
Namen hat."

89 "%s abgebrochen - keines der Objekte wurde %s.\n\n\
Eine Datei, die ersetzt werden sollte, konnte nicht gelöscht werden.\n\
    Datei: %s\n\n\
1 weiteres Objekt wurde gelöscht\n\n\
Die wahrscheinlichste Ursache für dieses Problem sind fehlende\n\
Schreibberechtigungen für oben genannte Datei. Berechtigungen\n\
ändern, oder die Datei, die %s umbenennen, so daß sie einen\n\
anderen Namen hat."

90 "Namenskonflikt.\n\
  Dieses Objekt ist auf dem Hintergrund des Arbeitsbereichs.\n\
  Ein anderes Objekt auf dem Hintergrund hat bereits den Namen\n\
  der eingegeben wurde.\n\
  Bitte einen anderen Namen auswählen."

91 "Kopiervorgang beenden"

92 "Verweiserstellung beenden"

93 "Verschieben beenden"

94 "Fehler"

95 "Verschieben abgebrochen - der Ordner wurde nicht verschoben.\n\n\
Der Ordner konnte nicht gelöscht werden, da ein in dem Ordner\n\
enthaltenes Objekt nicht gelöscht werden konnte.\n\
Möglicherweise wurden bereits andere in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das Objekt keine\n\
Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder den betreffenden Ordner\n\
manuell umbenennen, so daß er einen anderen Namen hat."

96 "Kopieren abgebrochen - der Ordner wurde nicht kopiert.\n\n\
Der Ordner konnte nicht gelöscht werden, da ein in dem Ordner\n\
enthaltenes Objekt nicht gelöscht werden konnte.\n\
Möglicherweise wurden bereits andere in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das Objekt keine\n\
Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder den betreffenden Ordner\n\
manuell umbenennen, so daß er einen anderen Namen hat."

97 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Der Ordner konnte nicht gelöscht werden, da ein in dem Ordner\n\
enthaltenes Objekt nicht gelöscht werden konnte.\n\
Möglicherweise wurden bereits andere in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das Objekt keine\n\
Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder den betreffenden Ordner\n\
manuell umbenennen, so daß er einen anderen Namen hat."

98 "Operation abgebrochen - der Ordner wurde nicht geändert.\n\n\
Der Ordner konnte nicht gelöscht werden, da ein in dem Ordner\n\
enthaltenes Objekt nicht gelöscht werden konnte.\n\
Möglicherweise wurden bereits andere in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das Objekt keine\n\
Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder den betreffenden Ordner\n\
manuell umbenennen, so daß er einen anderen Namen hat."

99 "Verschieben abgebrochen\n\n\
%s\n\
kann nicht entfernt werden.\n\n\
Die wahrscheinlichste Ursache ist, daß für die Datei\n\
keine Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder die betreffende Datei\n\
manuell umbenennen, so daß sie einen anderen Namen hat."

100 "Kopieren abgebrochen\n\n\
%s\n\
kann nicht entfernt werden.\n\n\
Die wahrscheinlichste Ursache ist, daß für die Datei\n\
keine Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder die betreffende Datei\n\
manuell umbenennen, so daß sie einen anderen Namen hat."

101 "Verweiserstellung (Link) abgebrochen\n\n\
%s\n\
kann nicht entfernt werden.\n\n\
Die wahrscheinlichste Ursache ist, daß für die Datei\n\
keine Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder die betreffende Datei\n\
manuell umbenennen, so daß sie einen anderen Namen hat."

102 "Operation abgebrochen\n\n\
%s\n\
kann nicht entfernt werden.\n\n\
Die wahrscheinlichste Ursache ist, daß für die Datei\n\
keine Schreibberechtigung vorliegt.\n\
Die Schreibberechtigung ändern oder die betreffende Datei\n\
manuell umbenennen, so daß sie einen anderen Namen hat."

103 "Verschieben abgebrochen - es wurde kein Objekt verschoben.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
%d Objekte wurden umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

104 "Kopieren abgebrochen - es wurde kein Objekt kopiert.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
%d Objekte wurden umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

105 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
%d Objekte wurden umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

106 "Operation abgebrochen - es wurde kein Objekt geändert.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
%d Objekte wurden umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

107 "Verschieben abgebrochen - es wurde kein Objekt verschoben.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
1 Objekt wurde umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

108 "Kopieren abgebrochen - es wurde kein Objekt kopiert.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
1 Objekt wurde umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

109 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
1 Objekt wurde umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

110 "Operation abgebrochen - es wurde kein Objekt geändert.\n\n\
Das folgende Objekt konnte nicht automatisch umbenannt werden:\n\
%s\n\n\
1 Objekt wurde umbenannt, bevor der Vorgang abgebrochen wurde.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

111 "Verschieben abgebrochen - es wurde kein Objekt verschoben.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

112 "Kopieren abgebrochen - es wurde kein Objekt kopiert.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

113 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

114 "Operation abgebrochen - es wurde kein Objekt geändert.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

115 "Verschieben abgebrochen - es wurde kein Objekt verschoben.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

116 "Kopieren abgebrochen - es wurde kein Objekt kopiert.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

117 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

118 "Operation abgebrochen - es wurde kein Objekt geändert.\n\n\
Ein Ordner konnte nicht gelöscht werden.\n\
   Ordner: %s\n\n\
Möglicherweise wurden bereits einige in dem Ordner befindliche\n\
Objekte gelöscht, bevor der Vorgang abgebrochen wurde.\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für das angegebene Objekt keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder das\n\
betreffende Objekt manuell umbenennen, so daß es einen anderen Namen hat."

119 "Verschieben abgebrochen - es wurde kein Objekt verschoben.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

120 "Kopieren abgebrochen - es wurde kein Objekt kopiert.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

121 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

122 "Operation abgebrochen - es wurde kein Objekt geändert.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
%d andere Objekte wurden gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

123 "Verschieben abgebrochen - es wurde kein Objekt verschoben.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

124 "Kopieren abgebrochen - es wurde kein Objekt kopiert.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

125 "Verweiserstellung (Link) abgebrochen - es wurde kein Verweis erstellt.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

126 "Operation abgebrochen - es wurde kein Objekt geändert.\n\n\
Eine Datei konnte nicht gelöscht werden.\n\
   Datei: %s\n\n\
1 anderes Objekt wurde gelöscht.\n\n\
Die wahrscheinlichste Ursache ist, daß für die angegebene Datei keine\n\
Schreibberechtigung vorliegt. Die Schreibberechtigung ändern oder die\n\
betreffende Datei manuell umbenennen, so daß sie einen anderen Namen hat."

127 "1 Objekt umbenannt."

128 "%d Objekte verschoben."

129 "1 Objekt kopiert."

130 "%d Objekte kopiert."

131 "1 Objekt verbunden (Link)."

132 "%d Objekte verbunden (Link)."

133 "1 Objekt geändert."

134 "%d Objekte geändert."

135 "Objekt %s kann nicht in sich selbst verschoben werden.  "

136 "Objekt %s kann nicht in sich selbst kopiert werden.  "

137 "Objekt %s kann nicht mit sich selbst verbunden werden (Link).  "

138 "Folgende(s) Objekt(e) konnte(n) nicht umbenannt werden:\n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß für diese(s)\n\
Objekt(e) keine Berechtigung zum Umbenennen vorliegt."

139 "Folgende(s) Objekt(e) konnte(n) nicht ersetzt werden:\n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß für diese(s)\n\
Objekt(e) keine Berechtigung zum Ersetzen vorliegt."

140 "Neues Objekt umbenennen auf:"

141 "Neue Objekte durch Anhängen einer Nummer umbenennen."

$set 10        Redefine set#

$ ****** Module: FileDialog.c ****(developer comment only)******** 
$ Messages 1, 2, 20, and 21 are titles for error dialogs 
$ Message 3 is an error message for dialogs requiring new file names 
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used 
$   when the user has specified a title for dtfile windows 
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs 
$ Messages 10-13 and 25 are error messages for the file copy operation 
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is 
$   used when the user has specified a title for dtfile windows 
$ Message 16 is a label for the New Folder dialog 
$ Messages 22-24 are error messages for the New File/Folder operations 
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used 
$   when the user has specified a title for dtfile windows 
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used 
$   when the user has specified a title for dtfile windows 
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used 
$   when the user has specified a title for dtfile windows 

1 "Fehler beim Umbenennen der Datei"

2 "Fehler beim Kopieren der Datei"

3 "Leerzeichen sind in einem Datei- oder Ordnernamen nicht erlaubt."

5 "Datei kopieren"

6 "Name der aktuellen Datei:     "

7 "Name der neuen Datei:"

8 "Dateityp "

9 "Dateityp anzeigen"

10 "Die ursprünglich ausgewählte Datei ist nicht mehr vorhanden."

11 "Der Name einer Datei oder eines Ordners darf nicht leer sein."

13 "Ungültiger Datei- oder Ordnername."

14 "Ordner erstellen"

16 "Name des neuen Ordners:"

20 "Fehler beim Erstellen des Ordners"

21 "Fehler beim Erstellen der Datei"

22 "Datei bzw. Ordner kann nicht mit einem leeren Namen erstellt werden."

23 "Ungültiger Ordnername.      "

24 "Ungültiger Dateiname."

25 "Mit dem Kopierbefehl kann eine Datei nicht in den\n\
Arbeitsbereich kopiert werden. Den Befehl \n\
'In Arbeitsbereich stellen' im Menü 'Datei' \n\
verwenden oder die Datei im Arbeitsbereich \n\
ablegen."

26 "Datei erstellen"

27 "Dateimanager - Datei kopieren"

28 "Dateimanager - Ordner erstellen"

29 "Dateimanager - Datei erstellen"

30 "Datei verschieben"

31 "Dateimanager - Datei verschieben"

32 "Datei-Link erstellen"

33 "Dateimanager - Verweis auf Datei erstellen"

34 "Zielordner: "

35 "Objektname:"

36 "Symbol anzeigen"

37 "Ausgewähltes Objekt:"

38 "Name für Kopie:"

39 "Fehler beim Umbenennen"

40 "Dieses Objekt ist eine Aktion.\n\
   Symbolkennungen für Aktionen können nicht direkt umbenannt werden."

41 "Das richtige Symbol für diesen Datentyp wird bereits angezeigt."

42 "Objekt verschieben"

43 "Dateimanager - Objekt verschieben"

44 "Objekt kopieren"

45 "Dateimanager - Objekt kopieren"

46 "Verweis auf Objekt erstellen (Link)"

47 "Dateimanager - Verweis auf Objekt erstellen (Link)"

48 "Neuer Objektname:"

$set 11        Redefine set#

$ ****** Module: FileManip.c ****(developer comment only)******** 
$ Message 6 is the title for an error dialog 
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages 

6 "Fehler bei der Dateibearbeitung"

9 "Dateien können nicht vom Papierkorb\n\
im Arbeitsbereich abgesetzt werden."

14 "Ordnerposition kann nicht gefunden werden."

16 "Ein Ordner kann nicht in sich selbst verschoben werden.\n\
%s"

28 "%s nicht auffindbar."

30 "Ordner %s kann nicht verschoben oder umbenannt werden.\n\
Alle Ansichten des Dateimanager für einen Ordner oder \n\
seine Unterordner müssen geschlossen sein, bevor ein \n\
Ordner umbenannt werden kann."

31 "Fehler beim Lesen von %s"

32 "%s kann nicht umbenannt werden"

33 "%s kann nicht geöffnet werden"

34 "Es besteht keine Berechtigung zum Verschieben von %s.\n\
Die Datei kann jedoch kopiert werden (dazu beim \n\
Ziehen der Datei <STRG> drücken). "

35 "Ein Ordner kann nicht in sich selbst kopiert werden."

36 "Untergeordneter Prozeß kann nicht erstellt werden."

37 "Dateien aus dem Papierkorb können nicht\n\
an den Arbeitsbereich übergeben werden.\n\
Um ein Objekt aus dem Papierkorb zu löschen:\n\
   - Objekt auswählen und\n\
   - 'Zurückstellen'\n\
vom Menu 'Datei' oder dem Menü des Objekts auswählen.\n\
Anschließend kann das Objekt vom Dateimanager aus auf\n\
den Hintergrund des Arbeitsbereichs stellen."

38 "Keinen Zugriff für das Verschieben von %s\n\
Das Objekt kann jedoch kopiert werden.\n\
Zum Kopieren eines Objekts:\n\
  - Taste <Strg> drücken und halten, und\n\
  - das Objekt mit der Maus an die Stelle ziehen.\n\
Oder:\n\
  - 'Kopieren in' im Menü 'Ausgewählt' der Menuleiste verwenden."

39 "Untergeordneter Prozeß kann nicht erstellt werden.\n\
   Die maximale Prozeßanzahl für dieses System wurde erreicht.\n\
   Einige Prozesse oder Programme die gegenwärtig aktiv sind\n\
   beenden und die Funktion wiederholen."

40 "Quellen- und Zielobjekte sind gleich.\n\
%s"

41 "Der Ordner\n\
%s\n\
ist nicht vorhanden."

42 "Auf der Zieleinheit steht kein Speicherplatz\n\
   zum Kopieren zur Verfügung."

43 "Auf der Zieleinheit steht kein Speicherplatz\n\
   zum Verschieben zur Verfügung."

44 "Das Objekt %s konnte nicht verschoben werden.\n\
   Ein Objekt kann nicht in den Ordner verschoben werden, \n\
   in dem es sich bereits befindet. Vor dem Verschieben des \n\
   Objekts muß ein anderer Zielordnername eingegeben werden."

45 "Das Objekt %s konnte nicht kopiert werden.\n\
   Entweder muß der Zielordner oder der Name für die Kopie \n\
   geändert werden, bevor eine Kopie erstellt werden kann."

46 "Für das Objekt %s konnte kein Verweis erstellt werden.\n\
   Entweder muß der Zielordner oder der Name für die Kopie \n\
   geändert werden, bevor ein Verweis erstellt werden kann."

47 "Ein Ordner kann nicht in sich selbst verschoben werden."

48 "Die folgenden Objekte können nicht in den Ordner \"%s\"\n\
kopiert werden:\n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß für den\n\
Zielordner keine Schreibberechtigung vorliegt."

49 "Die folgenden Objekte können nicht in den Ordner \"%s\"\n\
verschoben werden:\n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß für den\n\
Zielordner keine Schreibberechtigung vorliegt."

50 "Die folgenden Objekte können nicht in den Ordner \"%s\"\n\
verschoben werden:\n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß keine Berechtigung\n\
zum Verschieben des Quellenobjekts vorliegt."
$   
$ Messages 51 - 56 are new since CDE/SI.     
$   

51 "Nicht möglich, Datei/Ordner zu kopieren, da\n\
auf der Platte, auf die kopiert werden soll, \n\
nicht genügend Speicherplatz vorhanden ist."

52 "Nicht möglich, Datei/Ordner zu verschieben, da\n\
auf der Platte, auf die verschoben werden soll, nicht\n\
genügend Speicherplatz vorhanden ist."

53 "Kopieren der Datei/des Ordners aufgrund interner \n\
Fehler fehlgeschlagen. Der interne Fehler ist:"

54 "Das Verschieben der Datei/des Ordners aufgrund\n\
interner Fehler fehlgeschlagen. \n\
Der interne Fehler ist:"

55 "Den Systemadminstrator benachrichtigen"

56 "(Unbekannt):"

$quote

57 Sie dürfen den Ordner nicht verschieben.\n\
%s\n\
Schreiberlaubnis ist erforderlich.

58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$   

$set 12        Redefine set#

$ ****** Module: FileMgr.c ****(developer comment only)******** 
$ Message 3 is a dialog title 
$ Messages 4-6 are movement warning or error messages 
$ Message 7 is the title for the folder popup menus 

$quote "
3 "Datei(en) verschieben (umbenennen)"

4 "Die Datei %s befindet sich momentan im Ordner %s.\n\
Die Datei wird in den Ordner %s verschoben.\n\
Sicher?"

5 "Die Datei %1$s befindet sich momentan im Ordner %2$s.\n\
Die Datei wird in den Ordner %3$s verschoben.\n\
Sicher?"

6 "Mindestens eine der übergebenen Dateien entstammt einem\n\
anderen Ordner. Alle diese Dateien werden nach %s verschoben.\n\
Sicher?"

7 "Dateimanager"

18 "Der Ordner %s ist leer."

19 "Der Ordner %s enthält keine Unterverzeichnisse."

$set 13        Redefine set#

$ ****** Module: Filter.c ****(developer comment only)******** 
$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog 
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used 
$   when the user has specified a title for dtfile windows 
$ Message 23 is a text field label for the Modify Filter List dialog 
$ Message 25 is a label for the Modify Filter List dialog 

11 "Standardwerte"

19 "Filteroptionen festlegen"

21 "Alles auswählen"

22 "Alles abwählen"

23 "Filterzeichenfolge (optional):"

24 "Dateimanager - Filteroptionen festlegen"

25 "Zu verdeckende Dateitypen auswählen:"

26 "Zusätzlich verdecken (optional):"

27 "Angezeigt"

28 "Verdeckt"

29 "Also show (Optional):"

30 "Datentypen auswählen, die verdeckt werden sollen:"

31 "Datentypen sind:"

$set 14        Redefine set#


$set 15        Redefine set#

$ ****** Module: Find.c ****(developer comment only)******** 
$ Message 1 is the title for an error dialog 
$ Messages 14 and 15 are button labels for the Find Files dialog 
$ Messages 23, 26, and 27 are Find Files error messages 
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used 
$   when the user has specified a title for dtfile windows 
$ Messages 30-40 and 42 are labels for the Find Files dialog 

1 "Fehler beim Suchen"

14 "Starten"

15 "Beenden"

23 "Ungültige Suchinformationen.\n\n\
Suchordner angeben."

26 "Ungültige Suchinformationen.\n\n\
Die ausgewählte Datei ist nicht mehr vorhanden."

27 "Keine Dateien mit den angegebenen Suchkriterien gefunden."

29 "Dateien oder Ordner suchen"

30 "Für die Dateisuche ist mindestens ein Feld auszufüllen:"

31 "Datei- oder Ordnername:"

32 "Inhalt der Datei:"
$ Messages 33-35 are used on hpux systems 

33 "Ein"

34 "Aus"

35 "Verweisen folgen: "

36 "Neues Fenster öffnen"

37 "In Arbeitsbereich stellen"

38 "Dateien gefunden (nach Inhalt):"

39 "Dateien gefunden (nach Name und Inhalt):"

40 "Dateien gefunden (nach Name):"

41 "Dateimanager - Dateien oder Ordner suchen"

42 "Ordner durchsuchen:"

43 "Fehler beim Suchen"

44 "Argument für zu durchsuchenden Ordner fehlt.\n\
Einen Namen für den Ordner angeben, ab dem die Suche beginnt."

45 "Die ausgewählte Datei ist nicht mehr vorhanden.\n\n\
Die Datei wurde nach Abschluß des Suchvorgangs gelöscht."

46 "Suchen"

47 "Eines oder mehrere Felder ausfüllen, um die Suchargumente anzugeben:"

48 "Ordner öffnen"

49 "Dateimanager - Suchen"

50 "Name des zu durchsuchenden Ordners oder Argument 'Inhalt der Datei'\n\
fehlt. Den Namen des Ordners eingeben, ab dem die Suche beginnt,\n\
oder die Zeichenfolge, nach der gesucht werden soll, angeben."

$set 16        Redefine set#


$set 17        Redefine set#


$set 18        Redefine set#

$ ****** Module: Main.c ****(developer comment only)******** 
$ Message 1 is a blank line 
$ Messages 2, 6, 17-18 and 27-28 are error messages which will ONLY APPEAR 
$   in the DT ERRORLOG FILE 
$ above no longer true for message 2 - it will be displayed - see CDExc21626
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is 
$   used when the user has specified a title for dtfile windows 
$ Messages 19-21 are error messages for invalid directory input. 
$ Message 22 is the title for an error dialog 
$ Messages 25 and 26 are usage strings 
$ Messages 29 and 30 are text for the Save Settings dialog 
$ Messages 34 and 35 are drag/drop error messages 
$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1 

1 ""
$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 
$ above no longer true - it will be displayed
2 ".dt-Ordner oder Unterordner konnten nicht erstellt werden."
$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr 

6 "X-Toolkit-Fehler aufgetreten... Beenden.\n"

16 "Als Standardoptionen speichern"
$ MESSSAGES 17-18 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 

17 "Unable to create a file to store the default options."

18 "Could not open the session file."

19 "Bei\n\
%s\n\
handelt es sich nicht um einen Ordner."

20 "Der angegebene Ordner\n\
%s\n\
ist nicht vorhanden."

21 "Ungültige Ordnerangabe %s:%s."

22 "Fehler beim Ziehen"

23 "\n\
Verwendung: %s...\n\n\
   -noview\n\n\
\tDtfile wird im Server-Modus ausgeführt.\n\n\
   -session Session-Datei\n\n\
\tDtfile wird mit der durch den Parameter Session-Datei \n\
\tangegebenen Session-Datei ausgeführt.\n\n\
   -folder Ordner[,Ordner,Ordner]\n\
   -dir Ordner[,Ordner,Ordner]\n\n\
\tDtfile zeigt ein Fenster für jeden durch den Parameter\n\
\t'Ordner' angegebenen Ordner an. Mit dem Parameter\n\
\t'Ordner' können mehrere durch Kommata getrennte Ordner\n\
\tangegeben werden. Ordner können im Format 'Pfad' oder \n\
\t'Host:Pfad' angegeben werden.\n\n\
   -title Titel\n\n\
\tDtfile verwendet den durch den Parameter 'Titel' angegebenen\n\
\tNamen als Fenstertitel.\n\n\
   -help_volume Hilfeband\n\n\
\tDtfile verwendet den durch den Parameter 'Hilfeband' angegebenen \n\
\tHilfeband.\n\n\
   -restricted\n\n\
\tDtfile zeigt keine Ordner über dem eingeschränkten Ordner.\n\
\tBei Verwendung der Option -dir ist der mit dieser Option angegebene\n\
\tOrdner der eingeschränkte Ordner. Wird die Option -dir nicht verwendet,\n\
\tist der Ordner des aktuellen Benutzers der eingeschränkte Ordner.\n\n\
   -grid on/off\n\n\
\ton     = Dateien werden in einem Gittermuster angezeigt.\n\
\toff    = Dateien werden in ihrer tatsächlichen Position angezeigt.\n\n\
   -tree on/off\n\n\
\ton     = Dateien werden im Einzelordnermodus angezeigt.\n"

24 "\toff    = Dateien werden im Ordnerbaummodus angezeigt.\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = Für den Baummodus gibt es drei Status: teilweise erweitert oder unterdrückt.\n\
\talways = Für den Baummodus gibt es zwei Status: vollständig erweitert oder unterdrückt.\n\
\tchoose = Für den Baummodus gibt es drei Status: teilweise erweitert, vollständig \n\
\terweitert oder unterdrückt\n\n\
   -order alphabetical/file_type/date/size\n\n\
\tDateien werden in der angegebenen Reihenfolge angezeigt: alphabetisch, nach Dateityp, \n\
\tnach Datum oder nach Größe.\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\tDateien werden im angegebenen Format angezeigt: nur Text, Text und große Symbole, \n\
\tText und kleine Symbole, mit Attributen.\n\n\
   -direction ascending/descending\n\n\
\tDateien werden in der angegebenen Richtung angezeigt: aufsteigend oder \n\
\tabsteigend.\n\n\
   -large_icon_width <Größe>\n\n\
   -large_icon_height <Größe>\n\n\
   -small_icon_width  <Größe>\n\n\
   -small_icon_height <Größe>\n\n\
       Die Größe des Anzeigebereichs für die Symbolabbilder im Dateimanager,\n\
       die größer als dieser Wert sind, werden auf diesen Wert verkleinert. \n\
       Die Größe des Anzeigebereichs ist standardmäßig 38 für große Symbole \n\
       und 24 für kleine Symbole.\n\n"

26 "\n\
Verwendung bei eingeschränktem Modus: %s...\n\n\
   -folder Ordner[,Ordner,Ordner]\n\
   -dir Ordner[,Ordner,Ordner]\n\n\
\t'Ordner' bezeichnet das aktuelle Ordner oder ein\n\
\tOrdner unterhalb des Benutzerverzeichnisses.\n\n"
$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 

27 "Unable to recover the saved default home settings file, will use default.\n"

28 "Unable to recover the saved default tool settings file, will use default.\n"
$ Messages 29-30 are the strings displayed in a question dialog 

29 "Sollen die aktuellen Einstellungen für Fenstergröße, \n\
Ordnungskriterien und Filtereinstellungen der \n\
Dateimanager-Ansicht gespeichert werden?\n\n\
Diese gelten für alle neuen Fenster des Dateimanagers,\n\
die vom Bedienfeld aus gestartet werden."

30 "Sollen die aktuellen Einstellungen für Fenstergröße, \n\
Ordnungskriterien und Filter der eingeschränkten \n\
Dateimanager-Ansicht gespeichert werden?\n\n\
Diese gelten für alle neuen Fenster des eingeschränkten \n\
Dateimanagers, die vom Bedienfeld aus gestartet werden."

32 "Dateimanager - Als Standardoptionen speichern"

34 "Eine Datei oder ein Ordner kann\n\
nicht in den Papierkorb kopiert oder\n\
ein Verweis darauf erstellt werden."
$ 35 You can't copy or link a file or
$ \

$ folder out of the Trash Can. 

35 "Save the current Application Manager View\n\
window size, preferences, and filter settings?\n\n\
They will be used for any new Application Manager\n\
windows started from the Front Panel."

36 "Eine Datei oder ein Ordner kann nicht aus dem Papierkorb kopiert\n\
oder ein Verweis darauf erstellt werden. Das Objekt aus dem \n\
Papierkorb in den Dateimanager verschieben. Von dort kann es\n\
kopiert oder ein Verweis darauf erstellt werden."

37 "Eine Datei oder ein Ordner kann nicht aus dem Papierkorb kopiert\n\
oder ein Verweis darauf erstellt werden. Das Objekt aus dem \n\
Papierkorb in den Dateimanager verschieben. Von dort kann es\n\
kopiert oder ein Verweis darauf erstellt werden."

38 "Ungültige Ordnerangabe, %s"

$ MESSSAGE 39 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$quote
39 Kann keine zweite Session des Dateimanagers starten, da eine Session des \n\
Dateimanagers bereits auf dem Desktop geöffnet ist. Wenn Sie eine neue Session\n\
des Dateimanagers starten möchten, müssen Sie zuerst alle aktuellen Dateimanager-Fenster schließen\n\
und alle Dateimanager-Objekte aus den Arbeitsbereichen entfernen.

$ 
$ "-NEW"
40 "Could not connect to ToolTalk:\n%s\nExiting ..."



$set 19        Redefine set#


$set 20        Redefine set#

$ ****** Module: Menu.c ****(developer comment only)******** 
$ Messages 1-8, 123 and 9 are the text and mnemonics for the dtfile menu bar 
$ Messages 10/11, 128/129, 124/125, 121/122, 126/127, 16/17, 18/19, 84/85, 
$   90/91, 22/23, 117/118, are the text and mnemonics for the Fill  
$   pulldown menu  
$ Messages 10/11, 30/33, 92/93, 34/37, 38/41, 42/94, and 97/98 are the text 
$   and mnemonics for the Folder pulldown menu 
$ Messages 46/47, 99/100, 48/51, 52/55, 101/102, 103/104, 119/120, and  
$   62/63 are the text and mnemonics for the View pulldown menu 
$ Messages 105-116 are the text and mnemonics for the Help pulldown menu 

$quote "
1 "Datei"

2 "D"

3 "Ordner"

4 "O"

5 "Ansicht"

6 "s"

7 "Ausgewählt"

8 "A"

9 "H"

10 "Erstellen..."

11 "E"

16 "Eigenschaften..."

17 "g"

18 "Suchen..."

19 "S"

22 "Papierkorb"

23 "P"

30 "Aktualisieren"

33 "A"

34 "Wechseln..."

37 "W"

38 "Zu Home"

41 "H"
$ NOTE: The mnemonic for Up is Message 94 in this set. 

42 "höheres Verzeichnis"

46 "Erstellen"

47 "E"

48 "Alles auswählen"

51 "A"

52 "Alles abwählen"

55 "b"

62 "Alle Eigenschaften speichern"

63 "l"

84 "In Arbeitsbereich stellen"

85 "W"

90 "In Papierkorb"

91 "P"

92 "Schneller Wechsel in"

93 "l"
$ NOTE: Message 94 is the mnemonic for the "Up" menu item 

94 "h"

97 "Terminal"

98 "T"

99 "Aufräumen"

100 "u"

101 "Verdeckte Dateien anzeigen"

102 "g"

103 "Filterliste ändern..."

104 "F"

105 "Übersicht"

106 "v"

107 "Aufgaben..."

108 "A"

109 "Referenzinformation"

110 "R"

111 "Zu Element"

112 "Z"

113 "Hilfe für Hilfe"

114 "H"

115 "Über Dateimanager"

116 "D"

117 "Schließen"

118 "c"

119 "Eigenschaften für Ansicht festlegen"

120 "E"

121 "Kopieren..."

122 "K"

123 "Hilfe"

124 "Verschieben..."

125 "V"

126 "Verweis erstellen..."

127 "V"

128 "Umbenennen"

129 "n"

130 "Ordner erstellen"

131 "O"

132 "Datei erstellen"

133 "D"

134 "Gehe zu..."

135 "G"

136 "Papierkorb öffnen"

137 "Umbenennen..."

138 "Neue Ansicht öffnen"

139 "N"

140 "Als Verweis kopieren..."

141 "Optionen für Ansicht festlegen"

142 "Als Standardoptionen speichern"

143 "s"

144 "Filteroption festlegen..."

145 "F"

146 "Open Terminal"

147 "O"

148 "Table Of Contents"

149 "C"

150 "Change Permissions..."

151 "Put in Trash"

152 "Move to..."

153 "Copy to..."

154 "Copy As Link..."

155 "Go Up"

156 "Show Hidden Objects"

157 "Informationen zu Anwendungsmanager"

$set 21        Redefine set#

$ ****** Module: ModAttr.c ****(developer comment only)******** 
$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is 
$   used when the user has specified a title for dtfile windows 
$ Messages 2-13 and 18 are labels for the File Properties dialog 
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE 
$ Messages 15 and 16 are file properties error messages 
$ Message 17 is the title for an error dialog 
$ Message 19 is the default file owner/group, if real one can't be determined 
$ Messages 22-28 are error messages that can be generated when dtfile 
$   tries to use the dtfile.config configuration file, this file is 
$   used to customize the File Properties dialog 

1 "Dateiberechtigungen"

2 "Dateiname: "

3 "Eigentümer: "

4 "Gruppe: "

5 "Zugriffsrechte:"

6 "Lesen"

7 "Schreiben"

8 "Ausführen"

9 "Eigentümer:"

10 "Gruppe:"

11 "Alle:"

12 "Größe (in Byte): "

13 "Geändert: "
$ DO NOT LOCALIZE MESSAGE 14 

14 "%x %X"

15 "Ungültiges Dateiattribut.\n\n\
Der angegebene Eigentümer existiert nicht."

16 "Ungültiges Dateiattribut.\n\n\
Die angegebene Gruppe existiert nicht."

17 "Fehler bei den Eigenschaften"

18 "Verweis auf: "

19 "Unbekannt"

21 "Dateimanager - Dateiberechtigungen"

22 "Konfigurationsdatei für Datei-Manger kann nicht geöffnet werden:"

24 "Unbekannter Feldname in Konfigurationsdatei für Dateimanager:"

25 "Keine Informationen in Konfigurationsdatei für Dateimanager für \n\
Dateisystemkennung gefunden"

26 "Prozeß für Dateisystemdialog kann nicht erstellt werden"

27 "Fehler bei Ausführung des Dateisystemdialogs"

28 "Es wird versucht, folgendes auszuführen:"
$ do not localize "fsDialog" 

29 "Es wurde kein Wert für das Feld 'fsDialog' in der Konfigurationsdatei von dtfile angegeben."

30 "Name:"

31 "Name des Eigentümers ist ungültig.\n\
Der angegebene Benutzer ist nicht vorhanden."

32 "Group Name is invalid.\n\n\
No group of users with this name has been created on your system.\n\
Use your system administration software if you want to create\n\
a user group with this name."

33 "Fehler bei den Zugriffsrechten"

34 "Dateimanager - Zugriffsrechte"

35 "Das Dialogfenster 'Zugriffsrechte' für das Objekt\n\n\
%s/%s\n\n\
wurde geschlossen, da dieses Objekt aus dem System gelöscht wurde."

36 "Warnung 'Zugriffsrechte ändern'"

37 "Die Zugriffsrechte für den aktuellen Ordner sollen\n\
so geändert, daß der Benutzer diesen Ordner nicht\n\
länger anzeigen kann. Er wird automatisch geschlossen\n\
und kann vom Benutzer nicht erneut geöffnet werden.\n\n\
Ist dies tatsächlich beabsichtigt?"

38 File Manager Error
39 File Open Error

$set 22        Redefine set#

$ ****** Module: ModAttrP.c ****(developer comment only)******** 
$ Messages 1 and 3 are file properties error messages 
$ Message 2 is the title for the properties error dialog 

1 "Datei kann nicht geöffnet werden, Operation abgebrochen"

2 "Fehler bei den Dateimerkmalen"

3 "Mermale für %s können nicht geändert werden"

4 "Objekt kann nicht geöffnet werden."

5 "Fehler für Zugriffsrechte festlegen"

$set 23        Redefine set#

$ ****** Module: Prefs.c ****(developer comment only)******** 
$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is 
$   used when the user has specified a title for dtfile windows 
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog 

1 "Optionen für Ansicht festlegen"

2 "Ansicht"

3 "Nach Name"

4 "Nach Name und Symbol"

5 "Nach Name und kleinem Symbol"

6 "Nach Name, Datum, Größe ..."

7 "Anzeige "

8 "Nach einfachem Ordner"

9 "Nach Ordnerbaum"

10 "Reihenfolge"

11 "Nach Dateityp"

12 "Alphabetisch  "

13 "Nach Datum"

14 "Nach Größe"

15 "Richtung "

16 "Aufsteigend"

17 "Absteigend"

18 "Plazierung"

19 "Sortiertes Gitter"

20 "Wie positioniert"

22 "Dateimanager - Optionen für Ansicht festlegen"

23 "Kopfzeilen"

24 "Symbolischer Pfad"

25 "Aktueller Ordner"

26 "Statuszeile"

27 "Nur Ordner"

28 "Ordner, dann Dateien"

29 "Ordner und Dateien"

30 "Standardwerte"

31 "Darstellung"

32 "Nur nach Namen"

33 "Mit großen Symbolen"

34 "Mit kleinen Symbolen"

35 "Zeilen und Spalten"

36 "Textpfad"

37 "Meldung Zeile"

$set 24        Redefine set#


$set 25        Redefine set#


$set 26        Redefine set#


$set 27        Redefine set#

$ ****** Module: Trash.c ****(developer comment only)******** 
$ Messages 3, 4, and 81 are titles for error warning dialogs 
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages 
$ Message 9 is the title of the trash container 
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown 
$   menu 
$ Message 73 is an error dialog title 
$ Messages 74, 75, 79, and 80 are trash warning messages 

3 "Fehler im Papierkorb"

4 "Warnung zum Papierkorb"

5 "Folgende Dateien/Ordner konnten nicht in den Papierkorb gestellt werden:\n"

6 "Alles folgende ist entweder ein nichtleerer Ordner \n\
   oder eine Datei, auf die kein Schreibzugriff besteht.\n\
   Fortfahren?\n"

7 "Es besteht keine Schreibberechtigung für folgende Dateien:    \n\
   Fortfahren?\n"

8 "Folgende Ordner sind nicht leer:     \n\
   Fortfahren?\n"

9 "Papierkorb"

24 "Zurückstellen"

26 "Z"

28 "Löschen"

30 "L"

56 "Folgende Dateien konnten nicht zurückgestellt werden:\n"
$ Message 58 supplements Message 56 

58 "(Plus %d zusätzliche Dateien)"

73 "Datei(en) löschen"

74 "Die folgende Datei befindet sich auch im Arbeitsbereich:\n\
   %s\n\
Wenn die Datei in den Papierkorb geworfen wird, verschwindet\n\
sie vom Arbeitsbereich."

75 "Die folgenden Dateien befinden sich auch im Arbeitsbereich:\n\
   %s\n\
Wenn die Dateien in den Papierkorb geworfen werden, verschwinden\n\
sie vom Arbeitsbereich."

78 "Der Dateimanager unterstützt keine Dateien oder\n\
Ordner, die Leerzeichen in ihren Namen enthalten.\n\
Löschen daher nicht möglich.\n"

79 "Die Arbeitsbereichsdatei %s befindet sich auch im Dateisystem \n\
an Position: \n\
   %s\n\
Wenn die Datei in den Papierkorb geworfen wird, verschwindet\n\
sie aus dem Dateisystem."

80 "Die Arbeitsbereichsobjekte befinden sich auch im Dateisystem \n\
an Position: \n\
   %s\n\
Wenn die Dateien in den Papierkorb geworfen werden, verschwinden\n\
sie aus dem Dateisystem."

81 "Fehler beim Löschen einer Datei/eines Ordners"

82 "Die folgende Datei konnte nicht aus dem Dateisystem gelöscht werden:\n"

83 "Die Datei .trashinfo in $HOME/.dt/Trash kann nicht geöffnet \n\
   werden. Zur Verwendung des Papierkorbs muß auf diese Datei \n\
   zugegriffen werden. Die Berechtigungen für diese Datei und \n\
   den zugehörigen Ordner überprüfen.\n"

84 "Wenn Papierkorbobjekte gelöscht werden,\n\
werden sie endgültig zerstört.    \n\n\
   Wirklich löschen?\n"

85 "Das Objekt, das gelöscht werden soll, ist ein Verweis auf ein Originalobjekt\n\
   im Dateimanager an Position:\n\
   %s\n\
   Durch Klicken auf 'OK' werden sowohl das Originalobjekt als auch\n\
   der Verweis gelöscht.\n\
   Wenn nur das Arbeitsbereichsobjekt gelöscht werden soll:\n\
      1) In diesem Dialogfenster 'Abbrechen' auswählen \n\
      2) Anschließend 'Aus Arbeitsbereich entfernen'\n\
         im Dialogfenstermenü des Arbeitsbereichsobjekts\n\
         auswählen."

86 "Die Objekte, die gelöscht werden sollen, sind Verweise auf Originalobjekte\n\
   im Dateimanager an Position:\n\
   %s\n\
   Durch Klicken auf 'OK' werden sowohl das Originalobjekt als auch\n\
   der Verweis gelöscht.\n\
   Wenn nur die Arbeitsbereichsobjekte gelöscht werden sollen:\n\
      1) In diesem Dialogfenster 'Abbrechen' auswählen \n\
      2) Anschließend 'Aus Arbeitsbereich entfernen'\n\
         im Dialogfenstermenü des Arbeitsbereichsobjekts"

87 "Objekt im Papierkorb kann nicht geöffnet werden.\n\n\
   Um ein Objekt zu öffnen die Funktion 'Zurückstellen' verwenden,\n\
   um es in den Dateimanager zurückzubringen und dort öffnen."

88 "In eine temporäre Datei kann nicht geschrieben werden.\n\
   Möglicherweise ist das Dateisystem voll."

89 "Öffnen des Papierkorbverzeichnisses nicht möglich\n\
   %s\n\
   (Authentifizierung, Problem der Zugriffsrechte)\n\n\
   Alle Papierkorboperationen werden nicht ausgeführt."

90 "Der 'Papierkorb' kann nicht initialisiert werden.\n\
   Bitte überprüfen:\n\
        1)$HOME/.dt/Trash auf Zugriffsrechte.\n\
        2)Verfügbarer Plattenplatz.\n\
        3)Authentifizierung"
$ 91 is not used 

91 "Eine durch Verweis erstellte Kopie des folgenden Objekts ist\n\
auf dem Hintergrund des Arbeitsbereichs:\n\
Das Verschieben des Objekts in den Papierkorb löscht es \n\
sowohl aus dem Dateimanager als auch vom Arbeitsbereich."
$ 92 is not used 

92 "Mehrere durch Verweis erstellte Kopien folgender Objekte sind auf\n\
dem Hintergrund des Arbeitsbereichs:\n\
Das Verschieben der Objekte in den Papierkorb löscht sie \n\
sowohl aus dem Dateimanager als auch vom Arbeitsbereich."

93 "Zugriff auf Papierkorb-Informationsdatei nicht möglich:\n\
   %s\n\
   Alle Papierkorboperationen werden nicht ausgeführt.\n\
   Häufigste Ursachen sind:\n\
     - Netz-Authentifizierung.\n\
     - Zu wenig Plattenplatz.\n\
     - Falsche Zugriffsrechte auf $HOME/.dt/Trash."

94 "Der Dateimanager unterstützt keine Dateien und Ordner mit \n\
Leerzeichen im Namen. Deshalb können sie nicht gelöscht werden.\n\
Um diese Objekte zu löschen:\n\
  - 'Terminal öffnen' aus der Menüleiste des Dateimanagers auswählen.\n\
  - Anschließend den Befehl zum Löschen eingeben."

95 "Zugriff auf Papierkorb-Verzeichnis nicht möglich:\n\
   %s\n\
   Alle Papierkorboperationen werden nicht ausgeführt.\n\
   Häufigste Ursachen sind:\n\
     - Netz-Authentifizierung.\n\
     - Zu wenig Plattenplatz.\n\
     - Falsche Zugriffsrechte."

96 "Folgende Objekte konnten nicht aus dem Dateisystem gelöscht werden:\n"

97 "Folgende Objekte konnten nicht in den Papierkorb gelegt werden:\n"

98 "(Plus %d zusätzliche Objekt(e))"
$ message 99 is not used 

99 "Keine Schreibzugriffsberechtigung für folgende Datei(en):\n\
Fortfahren?\n"

100 "Folgende(r) Ordner ist/sind nicht leer.\n\
Fortfahren?\n"

101 "Folgende(s) Objekt(e) konnten nicht zurückgestellt werden:\n"

102 "Ein durch ein Verweis kopiertes Objekt ist auf\n\
dem Hintergrund des Arbeitsbereichs:\n\
%s\n\
Verschieben des Objekts in den Papierkorb löscht es\n\
sowohl im Datei-Manger als auch im Arbeitsbereich."

103 "Mehrere durch ein Verweis kopierte Objekte sind auf\n\
dem Hintergrund des Arbeitsbereichs:\n\
%s\n\
Verschieben der Objekte in den Papierkorb löscht sie\n\
sowohl im Datei-Manger als auch im Arbeitsbereich."

104 "Keine Schreibzugriffsberechtigung für folgende Datei(en):\n\
Fortfahren?\n"

105 "Default action of a trash object will not be executed.\n\n\
    To execute the default action of this object \n\
    use 'Put Back' to return it to the File Manager\n\
    then execute it there."

106 "Informationen zu Papierkorb"

107 "Mindestens ein Objekt im folgenden Ordner konnte nicht ersetzt werden:\n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß für dieses\n\
Objekte keine Berechtigung zum Löschen vorliegt."

108 "Die folgende Datei konnte nicht gelöscht werden. \n\n\
%s\n\n\
Die wahrscheinlichste Ursache ist, daß für dieses\n\
Objekte keine Berechtigung zum Löschen vorliegt."

109 "Zurückstellen - Warnung"

110 "Es existiert bereits ein Objekt mit dem Namen\n\n\
    %s\n\n\
'Ok' auswählen, wenn das Objekt durch das Objekt aus dem\n\
Papierkorb ersetzt werden soll.\n\n\
'Abbrechen' auswählen, wenn das Objekt erhalten bleiben\n\
soll. Wenn das Objekt umbenannt wird, kann das Zurückstellen\n\
wiederholt werden, so daß beide Objekte erhalten bleiben."

111 "Zurückstellen - Fehler"

112 "Die folgenden Objekte können nicht gelöscht werden.\n\
Die wahrscheinlichste Ursache ist, daß diese Objekte\n\
bereits gelöscht sind.\n\n"

113 "Mindestens ein Objekt in den folgenden Ordnern oder der \n\
Ordner selbst kann nicht gelöscht werden.\n\
Die wahrscheinlichste Ursache ist, daß keine Berechtigung\n\
zum Löschen dieses Objekts vorliegt.\n\n"

114 "Die folgenden Dateien können nicht gelöscht werden. Die \n\
wahrscheinlichste Ursache ist, daß keine Berechtigung zum Löschen\n\
der Dateien vorliegt.\n\n"

115 "Die folgenden Objekte können nicht gelöscht werden.\n\
Das Desktop kann ohne diese Objekte nicht korrekt ausgeführt werden.\n\n"

$set 28        Redefine set#

$ ****** Module: Desktop.c ****(developer comment only)******** 
$ Messages 1-3 Workspace error messages 
$ Message 4 is the title for the Workspace popup menus 
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu 

1 "Im Arbeitsbereich existiert bereits eine gleichnamige Datei.\n\
Einen anderen Namen wählen."

2 "Im Ordner, dem die Datei entstammt, existiert bereits\n\
eine gleichnamige Datei.\n\
Einen anderen Namen wählen."

3 "Eine Datei namens %s befindet sich bereits im Arbeitsbereich.\n\
Eine weitere gleichnamige Datei kann nicht in den Arbeitsbereich \n\
gestellt werden. Um beide Dateien in den Arbeitsbereich zu stellen, \n\
muß eine von beiden umbenannt werden."

4 "Arbeitsbereichsobjekt"

5 "Aus Arbeitsbereich löschen"

6 "l"

7 "Übergeordneten Ordner öffnen"

8 "O"

9 "Ein Objekt dieses Namens existiert bereits im Arbeitsbereich.\n\
Bitte einen anderen Namen wählen."

10 "Ein Objekt dieses Namens exisitiert bereits im Orginalordner  \n\
(Dem Ordner, aus dem das Objekt stammt).\n\
Bitte einen anderen Namen wählen."

11 "Ein Objekt mit Namen %s existiert bereits auf dem Hintergrund des\n\
Arbeitsbereichs. Es kann kein weiteres Objekt gleichen Namens auf\n\
den Arbeitsbereich gestellt werden. Um beide Objekte im\n\
Arbeitsbereich zu verwenden, muß eines umbenannt werden."
$   
$ Messages 12 and 13 are new messages since the CDE/Sample.  
$   

12 "Ein ungültiger Arbeitsbereich wurde übergeben. Der Standardarbeitsbereich\n\
wird zum Arbeitsbereich 'Eins', und ein Objekt mit dem Namen:\n\
   %s\n\
ist bereits auf dem Hintergrund dieses Arbeitsbereichs vorhanden.\n\
Ein anderes Objekt mit dem gleichen Namen kann nicht in den \n\
Arbeitsbereich gestellt werden. Daher eines der beiden Objekte \n\
umbenennen."

13 "Objekt:\n\
   %s\n\
ist im Dateisystem nicht vorhanden."

$set 29        Redefine set#

$ ****** Module: Help.c ****(developer comment only)******** 
$ Message 1 is the title for the dtfile help dialog 
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is 
$   used when the user has specified a title for dtfile windows 
$ Message 3 is the text for help on the "Open New View" action 
$ Message 4 is the text for help on the "Open In Place" action 
$ Messages 5-6 are help error messages 

1 "Hilfe für Dateimanager"

2 "%s Hilfe"

3 "Durch die Aktion `%s` wird eine neue Ansicht des Dateimanagers des\n\
angeforderten Ordners geöffnet."

4 "Durch die Aktion `%s` wird der angeforderte Ordner in der aktuellen\n\
Ansicht des Dateimanagers angezeigt."

5 "Für die Aktion %s existiert kein Hilfetext"

6 "Für den Dateityp %s existiert kein Hilfetext"

7 "Die Aktion `%s` öffnet ein neues Fenster einer\n\
Dateimanager-Ansicht mit dem ausgewählten Ordner."

8 "Die Aktion `%s` öffnet den ausgewählten Ordner\n\
im Fenster mit der aktuellen Dateimanager-Ansicht."

9 "Hilfe für Papierkorb"

10 "Für mehrere ausgewählte Objekte steht keine Hilfe zur Verfügung."

$set 30        Redefine set#

$ ****** Module: SharedMsgs.c ****(developer comment only)******** 
$ The following messages are used in more than one place within the 
$ dtfile source.  Unless otherwise specified, they are error messages. 

1 "Aus %s kann nicht gelesen werden"
$ Message 2 is used to format the file attributes in 'by attributes' mode 

2 "%b %d %H:%M:%S %Y"

3 "Datei %s kann nicht vom Rechner %s verschoben oder kopiert werden.\n\
Rechner %s ist eventuell nicht richtig an Dateisystem angehängt."

4 "Datei %1$s kann nicht vom Host %2$s verschoben oder kopiert werden.\n\
Host %3$s ist eventuell nicht richtig an Dateisystem angehängt."

5 "%s kann nicht gelesen werden"

6 "Schreiben in Ordner %s nicht möglich"

7 "%s kann nicht erstellt werden"

8 "%s kann nicht gelöscht werden"

9 "Ein Ordner mit dem Namen\n\
  %s\n\
  existiert und kann nicht überschrieben werden."
$ Message 10 is the help topic string for help on a file icon or action 

10 "Beschreibung Dateityp/Aktion"

11 "Pfad kann nicht als Teil des Dateinamens angegeben werden."

12 "Ungültige Suchangaben.\n\n\
Der folgende Ordner existiert nicht oder ist nicht zugänglich:\n\n\
     %s\n"
$ Messages 13-16 are dialog titles 

13 "%s Überblick"

14 "%s Lernprogramm"

15 "Über %s"

16 "Dateimanager"
$ Message 18 is appended to ".." folders 

18 "(höher)"
$ Messages 19 and 20 are messages which appear when a user clicks item help 
$ outside the application.  19 is the title, while 20 is the message. 

19 "Fehler bei Hilfe zu Element"

20 "Es muß eine Komponente innerhalb\n\
des Dateimanagers ausgewählt werden."
$ Messages 21 and 22 are titles for error dialogs 

21 "Fehler bei Datei verschieben"

22 "Fehler bei Verweis auf Datei erstellen"

23 "'%s' kann nicht erstellt werden\n\n\
Der häufigste Grund sind unkorrekte Zugriffsrechte\n\
für die einbezogenen Dateien oder Ordner.\n\n\
Um die Zugriffsrechte zu sehen, das Objekt auswählen\n\
und 'Zugriffsrechte ändern' auswählen."

24 "Ein Objekt innerhalb des DateimanagerYou muß augewählt werden.\n\
'Zu Element'-Hilfe funktioniert nur innerhalb der Anwendung aus\n\
der sie gestartet wurde."

25 "'%s' kann nicht erstellt werden\n\n\
Der häufigste Grund sind unkorrekte Zugriffsrechte\n\
für die einbezogenen Dateien oder Ordner.\n\n\
Um die Zugriffsrechte zu sehen, das Objekt auswählen\n\
und 'Zugriffsrechte ändern' aus dem Objektmenü oder\n\
dem Menü 'Ausgewählt' auswählen."

26 "You must select an object within the Application Manager.\n\
'On Item' help only works within the application where\n\
you launched it."

$set 31        Redefine set#


$set 32        Redefine set#

$ ****** Module: dterror.src ****(developer comment only)******** 
$ Message 1 is a title for dterror 

1 "dt Fehler"

2 "Fehler beim Öffnen eines Verzeichnisses durch Dateimanager"

$set 33        Redefine set#


1 "Mehrfache Objekte"

2 "Aktueller Ordner"

3 "Fehler bei 'Objekt verschieben'"

4 "Fehler bei 'Objekt kopieren'"

5 "Fehler bei 'Verweis auf Objekt erstellen'"

6 "Der Standort, an den verschoben werden soll:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden."

7 "Der Standort, an den kopiert werden soll:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden."

8 "Der Standort, an dem der Verweis erstellt werden soll:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden."

9 "Der Standort, an den verschoben werden soll:\n\n\
   %s\n\n\
ist kein Ordner."

10 "Der Standort, an den kopiert werden soll:\n\n\
   %s\n\n\
ist kein Ordner."

11 "Der Standort, zu dem ein Verweis erstellt werden soll:\n\n\
   %s\n\n\
ist kein Ordner."

12 "Das Objekt, das verschoben werden soll:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden."

13 "Eines der Objekte, die verschoben werden sollen:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden.\n\
Keines der Objekte wird verschoben."

14 "Das Objekt, das kopiert werden soll:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden."

15 "Eines der Objekte, die kopiert werden sollen:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden.\n\
Keines der Objekte wird kopiert."

16 "Das Objekt, auf das ein Verweis erstellt werden soll:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden."

17 "Eines der Objekte, auf die Verweise erstellt werden sollen:\n\n\
   %s\n\n\
ist im Dateisystem nicht vorhanden.\n\
Auf keines der Objekte wird ein Verweis erstellt."

$set 99        Redefine set#

$ ****** Version Information ******** 
$ DO NOT TRANSLATE THESE MESSAGES 
$ DO NOT CHANGE THESE MESSAGES 
$ DO NOT LOCALIZE THESE MESSAGES 
$ These messages are used for the version information. 

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $TOG: dtfile.msg /main/11 1999/09/16 09:42:50 mgreess $"
