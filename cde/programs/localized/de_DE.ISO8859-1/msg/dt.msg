$ "$TOG: dt.msg /main/7 1999/09/16 14:52:56 mgreess $"; 
$ *************************************<+>************************************* 
$ ***************************************************************************** 
$ ** 
$ **  File:        dt.msg 
$ ** 
$ **  Project:     DT 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for DT library. 
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
$ **  $TOG: dt.msg /main/7 1999/09/16 14:52:56 mgreess $ 
$ ***************************************************************************** 
$ *************************************<+>************************************* 
$ ***************************************************************************** 
$  
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS ***** 
$  
$ There may be three types of messages in this file: 
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
$ *****  Module: Action.c  ***** 
$ DO NOT TRANSLATE or CHANGE or LOCALIZE  messages 1 and 2 from set 2 
$ These messages are the result from incorrect actions. 
$ Do not remove the two spaces after the ":" in message 3. 
$ The name of an action will follow the spaces at the end of message 3. 
$ _DtMessage 5 is a prompt stating that the user must supply information 
$   for the action.  Execute the "TERM_PROMPT" action for an example. 

1 "%1$s%2$s%3$s"

2 "%1$s%2$s"

3 "Aktion:  "

4 "[Fehler]"

5 "Bitte folgendes eingeben:"

6 "Es wurden mehr Parameter angegeben, als für die ausgewählte Aktion erforderlich sind.\n\
'Ok' auswählen, wenn zusätzliche Parameter ignoriert werden sollen.\n\n\
'Abbruch' auswählen, um die Aktion zu beenden."
$ NOTE: trailing spaces ARE important in message 7! 

7 "Auf den folgenden Rechner konnte nicht zugegriffen werden:\n\n\
        "

8 "\n\n\
Überprüfen, ob die entsprechende ferne Datenzugriffsverbindung\n\
hergestellt wurde.\n\n"
$ ** The arguments for message #9 are: <action name>, <file name>, <file type>. 

9 "Die Aktion \"%1$s\" wurde nicht gefunden\n\
oder die Aktion ist nicht anwendbar auf die Datei\n\
    \"%2$s\"\n\
mit dem Datenattribut \"%3$s\"\n\n"
$ ** The argument for message #10 is: <action name> 

10 "Die Aktion \"%s\" wurde nicht gefunden.\n"
$ ** Message number 11 is difficult to generate without an internal error. 

11 "Die folgende Datei wurde nicht gefunden:\n\n\
       "
$ Do NOT localize the word "MAP". 

12 "Die Aktion kann nicht ausgeführt werden, da sie zu viele\n\
MAP-Ebenen enthält, oder das Mapping ist \"rückbezüglich\"."

13 "Die angeforderte Aktion kann nicht aufgerufen werden.\n\n\
Besteht Zugriff auf die folgenden Rechner?\n\
\t(%s)\n\
Ist das folgende Programm vorhanden?\n\
(/usr/dt/bin/dttypes ausführen, um Aktionen und Programme abzustimmen.)\n\n\
Ist auf dem System nicht mehr genügend Speicherkapazität für weitere Prozesse vorhanden?"

14 "Die Ausführung dieser Anforderung ist gescheitert."

15 "Die Ausführung dieser Anforderung ist gescheitert.\n\
Es konnte keine ToolTalk-Verbindung hergestellt werden:\n\n%s"

16 "Die Ausführung dieser Anforderung ist gescheitert."

17 "Die Ausführung dieser Aktion ist aus folgendem Grund gescheitert.\n\n\
    %s."

18 "Beim Versuch, eines der Dateiargumente zuzuordnen, ist\n\
ein Fehler aufgetreten."
$ ** The arguments for message #19 are: <action name>, <buffer type>. 

19 "Die Aktion \"%1$s\" wurde nicht gefunden oder ist \n\
nicht anwendbar auf Puffer des Typs: \n\n\
\"%2$s\""
$ **  MESSAGE NUMBER 20 IS OBSOLETE. 

20 "Unable to create tmp file: \"%1$s\" \n\
for action: \"%2$s\""

21 "Nicht unterstützte Eingabeobjektklasse \"%d\"\n\
für die Aktion \"%s\"."

22 "Temporäre Datei kann nicht im Verzeichnis \"%s\"\n\
für die Aktion \"%s\" erstellt werden."

23 "Temporäre Datei \"%s\"\n\
für die Aktion \"%s\" kann nicht geöffnet werden."

24 "Temporäre Datei \"%s\"\n\
für die Aktion \"%s\" kann nicht geschrieben werden."

$set 3

$ ****  Module: CmdMain.c **** 
$ These messages are generated by the command invocation library. 
$ Note that set 3, messages 1, 3, 4, 12, 13, 16 have been obsoleted. 

2 "Der Versuch, in das Verzeichnis\n\n\
   %1$s\n\n\
vom Rechner \"%2$s\" zu wechseln, scheiterte.\n\n\
Eingabe und Berechtigungen überprüfen und sicherstellen, \n\
daß das Verzeichnis vorhanden ist."
$ Do NOT remove the leading spaces from any of the lines of the following message. 

5 "Der Versuch, folgenden Befehl auf dem Rechner\n\
\"%1$s\" auszuführen, scheiterte:\n\n\
   %2$s\n\n\
Überprüfen, ob das Programm vorhanden ist, die richtigen Berechtigungen\n\
aufweist und ausführbar ist."
$ Do NOT remove the leading spaces from any of the lines of the following message. 

6 "Der Versuch, den folgenden Befehl auszuführen, scheiterte:\n\n\
   %s\n\n\
Überprüfen, ob das Programm vorhanden ist, die richtigen Berechtigungen\n\
aufweist und ausführbar ist."
$ Do NOT remove the leading spaces from any of the lines of the following message. 
$ Do NOT localize the word "PATH". 

7 "Diese Aktion kann nicht gestartet werden, da das folgende\n\
Terminal-Emulatationsprogramm nicht ausgeführt werden kann:\n\n\
   %s\n\n\
Überprüfen, ob das Programm vorhanden ist, die richtigen Berechtigungen\n\
aufweist und ausführbar ist.\n\
Dieser Fehler ist möglicherweise aufgetreten, weil das Programm\n\
nicht in der Pfadangabe (\"PATH\") enthalten ist."
$ Message number 8 is OBSOLETE. 
$ 8 This action is missing one or more of the following: 
$  
$ \

$   execution parameters, execution string or execution host. 
$  
$ \

$ Check that the action has an appropriate number of parameters. 
$ Do NOT remove the leading spaces from any of the lines of the following message. 

9 "Die Gesamtzahl Zeichen in dieser Aktion überschreitet die Grenze von \"%d\".\n\n\
Die Aktion muß möglicherweise in mehrere Aktionen aufgeteilt werden."
$ Message number 10 is OBSOLETE. 
$ 10 The shell program \"%s\" cannot be started. 
$  
$ \

$ Check that the program has the correct permissions and 
$ \

$ is executable. 

11 "Der Versuch, einen neuen Prozeß auf dem Rechner \"%s\" zu starten, scheiterte.\n\n\
Zum Fortsetzen muß möglicherweise ein nicht mehr benötigter Prozeß auf diesem\n\
Rechner gestoppt werden."
$ Message number 14 is OBSOLETE. 
$ 14 An attempt to log the output from a remote host failed. 
$  
$ \

$ To continue, you may need to stop an existing process. 

15 "Der Versuch, das Ausgabeprotokoll von einem fernen Rechner zu protokollieren,\n\
scheiterte.\n\n\
Zum Fortsetzen muß möglicherweise ein vorhandener Prozeß gestoppt werden."
$ Do NOT remove the leading spaces from any of the lines of the following message. 
$ Do NOT localize the words "WINDOW_TYPE". 

17 "WINDOW_TYPE \"%1$s\" dieser Aktion ist unbekannt.\n\
Mögliche Angaben für WINDOW-TYPE sind:\n\n\
  %2$s, %3$s, %4$s,\n\
  %5$s, %6$s oder %7$s"
$ Do NOT remove the leading spaces from any of the lines of the following message. 

18 "Diese Aktion kann nicht gestartet werden, da das DT-Unterprozeßprogramm\n\
   %s\n\n\
nicht ausgeführt werden kann. Überprüfen, ob das Programm die richtigen\n\
Berechtigungen aufweist und ausführbar ist."
$ MESSAGE 19 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 
$ MESSAGE 19 IN SET 3 IS OBSOLETE 
$ Do NOT remove the leading spaces from any of the lines of the following message. 

19 "An attempt to run 'xhost' on Xserver host \"%s\" failed.\n\n\
To suppress this message and disable automatic xhost'ing, run the\n\
\"EditResources\" action and add the following resource:\n\n\
   *autoXhosting:  False\n"
$ Do NOT remove the leading spaces from any lines in the following message. 

20 "Diese Aktion kann nicht auf dem Rechner \"%1$s\" ausgeführt\n\
werden, da das folgende erforderliche Programm nicht vorhanden\n\
oder nicht ausführbar ist:\n\n\
   %2$s\n"
$ MESSAGE 21 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 

21 "The action \"%1$s\" was successfully executed on host \"%2$s\"."
$ MESSAGE 22 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 

22 "The directory \"%1$s\" on host \"%2$s\"\n\
could not be converted to a network path.\n\
(%3$s)"
$ MESSAGE 23 IN SET 3 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 

23 "The stderr logfile \"%1$s\" on host \"%2$s\"\n\
could not be converted to a network path.\n\
(%3$s)"

24 "Diese Aktion kann nicht ausgeführt werden,\n\
da der Rechner \"%s\" nicht erreichbar ist."

25 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da der Service \"%2$s\" auf diesem Rechner nicht\n\
ordnungsgemäß konfiguriert ist."

26 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da der Service auf dem Rechner \"%2$s\" nicht \n\
konfiguriert ist."

27 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da dem Service \"%2$s\" die Benutzer-ID \"%3$d\" auf\n\
auf dem Rechner \"%4$s\" zugeordnet ist und dies nicht mit dem\n\
Benutzernamen/der Benutzer-ID auf dem Rechner \"%5$s\" \n\
übereinstimmt."

28 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da für den Benutzer \"%2$s\" kein Benutzereintrag \n\
auf diesem Rechner vorhanden ist."

29 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da kein Pfadname für die Authentikationsdatei erstellt \n\
werden kann."

30 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da die Authentifikationsdatei auf diesem Rechner nicht\n\
geöffnet werden kann.\n\n\
Mögliche Ursache ist eine falsche Konfiguration des home-Verzeichnisses\n\
im Netzwerk."

31 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da die Umgebungsgröße \"%2$d\" Byte überschreitet."

32 "Diese Aktion kann auf dem Rechner \"%1$s\" nicht ausgeführt\n\
werden, da der Rechner \"%2$s\" nicht zur Benutzung des Services\n\
\"%3$s\" berechtigt ist.\n\n\
Zur Behebung dieses Fehlers den Rechner \"%4$s\" zum Eintrag \n\
für den Service \"%5$s\" in der Datei \"%6$s\" auf dem \n\
Rechner \"%7$s\" hinzufügen."

$set 4

$ *****  Module: DbReader.c  ***** 
$  
$  MESSAGES 1-11 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 
$  
$ Do NOT remove the leading spaces from any of the lines of the following message. 

1 "A \"%s\" definition in the file:\n\
  \"%s\"\n\
is missing a name.  Add the name\n\
  to the definition.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message. 
$ Do NOT localize the word "END". 
$ THIS MESSAGE IS OBSOLETE 

2 "The definition \"%s\" in the file\n\
  \"%s\"\n\
is incomplete. The definition may be missing\n\
the \"END\" field.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message. 

3 "A multi-line field in the definition \"%s\"\n\
in the file\n\
  \"%s\"\n\
is incomplete.  A \"\\\" character may be missing\n\
in the multi-line field.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message. 

4 "The definition \"%s\" in the file\n\
  \"%s\"\n\
has too many fields.\n"
$ Do NOT localize the word "Dt.TypesDirs". 

5 "An attempt to read the action and filetypes databases failed.\n\
This may be caused by the resource \"Dt.TypesDirs\" being incorrectly set.\n"
$ Do NOT localize the strings "'{'" or "'}'". 

6 "The definition \"%s\" in the file\n\
  \"%s\"\n\
  has an end symbol '}' with no start symbol '{'.\n"
$ Do NOT localize the strings "'{'" or "'}'". 

7 "The definition \"%s\" in the file\n\
  \"%s\"\n\
  has an unexpected start symbol '{'.\n"
$ Do NOT localize the strings "'{'" or "'}'". 

8 "The definition \"%s\" in the file\n\
  \"%s\"\n\
  is incomplete. The definition may be missing\n\
  the \"}\" field.\n"
$ Do NOT localize the strings "'{'" or "'}'". 

9 "The definition \"%s\" in the file\n\
  \"%s\"\n\
  is missing a start symbol '{'.\n"

10 "The DtDbVersion variable can only be set\n\
at the beginning of the file.  The remainder of the file\n\
'%s' is being ignored.\n"

11 "The file '%s'\n\
contains an invalid DtDbVersion identifier.\n"
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
$ $ 
$ set 5 (Module: Filetype.c) is OBSOLETE.  Do NOT reuse this set number. 
$ $ 
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 

$set 6

$ *****  Module: ActionDb.c  ***** 
$  
$  MESSAGES 1-11 IN SET 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE 
$  
$ Do NOT remove the leading spaces from any of the lines of the following message. 

1 "The action definition \"%s\" in the file:\n\
  \"%s\"\n\
does not have any fields.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message. 

2 "The action definition \"%s\" in the file\n\
  \"%s\"\n\
contains the following unrecognized field name and value:\n\
\"%s\"\n"
$ Message number 3 is OBSOLETE. 
$ 3 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ does not have the required field \"%s\". 
$  

4 "The \"%s\" field in the action definition \"%s\"\n\
in the file \"%s\"\n\
has an unrecognized action type.\n"
$ Do NOT remove the leading spaces from any of the lines of the following message. 
$ Do NOT localize the words "TYPE MAP". 

5 "The action definition \"%s\" in the file\n\
  \"%s\"\n\
is not mapped properly.  The \"%s\" field\n\
should be: TYPE MAP action_mapped_to.\n"
$ Message number 6 is OBSOLETE. 
$ 6 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ contains a MAP to itself. 
$  
$ Message number 7 is OBSOLETE. 
$ 7 The \"%s\" field in the action definition \"%s\" 
$ \

$ in the file \"%s\" 
$ \

$ has the unrecognized value \"%s\". 
$  
$ Do NOT remove the leading spaces from any of the lines of the following message. 
$ Do NOT localize the word "COMMAND". 

8 "The action definition \"%s\" in the file\n\
  \"%s\"\n\
is a \"COMMAND\" action type but lacks the\n\
required field \"%s\".\n"
$ Message number 9 is OBSOLETE. 
$ 9 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ is a \"MESSAGE\" action type but lacks the 
$ \

$ required field \"%s\". 
$  
$ Message number 10 is OBSOLETE. 
$ 10 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ is a \"MESSAGE\" action type but lacks the 
$ \

$ required field \"%s\". 
$  
$ Do NOT remove the leading spaces from any of the lines of the following message. 

11 "The action definition \"%s\" in the file\n\
  \"%s\"\n\
has the illegal value \"%s\" in the \"%s\" field.\n\
\""

12 "Die Aktionsdefinition \"%s\" in der Datei\n\
  \"%s\"\n\
enthält doppelte Felder mit dem Namen:\n\
 \"%s\"."

13 "Die Aktionsdefinition \"%s\" in der Datei\n\
  \"%s\"\n\
enthält ungültige Felder für Aktionen des Typs \"%s\".\n"

14 "Die Aktionsdefinition \"%s\" in der Datei\n\
  \"%s\"\n\
enthält nicht das erforderliche Feld \"%s\"\n\
 für Aktionen des Typs \"%s\".\n"
$ Message number 15 is OBSOLETE. 
$ 15 The action definition \"%s\" in the file 
$ \

$   \"%s\" 
$ \

$ contains a NULL field value, for the \"%s\" field. 
$  

16 "Die Aktionsdefinition \"%s\" in der Datei\n\
  \"%s\"\n\
enthält einen ungültigen Wert \"%s\" im Feld \"%s%s%s\".\n\
\""
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
$ $ 
$ set 22 (Module: HelpUtil.c) is OBSOLETE.  Do NOT reuse this set number. 
$ $ 
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 

$set 28

$ *****  Module:  DtUtil.c  ***** 
$ The following are button labels. 

1 "OK"

2 "Abbrechen"

3 "Hilfe"

4 "Anwenden"

5 "Schließen"
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
$ $ 
$ set 44 (Module: start_bms.c) is OBSOLETE.  Do NOT reuse this set number. 
$ $ 
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
$ $ 
$ set 46 (Module: Utility.c) is OBSOLETE.  Do NOT reuse this set number. 
$ $ 
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 

$set 48

$ *****  Module:  UserMsg.c  ***** 
$ _DtMessage 1 uses the format of the "strftime" system call.  See the 
$ manual page for more information on "strftime".  WARNING: follow 
$ the man page closely because incorrect format strings could cause 
$ an application to abort. 

1 "%a %b %d %H:%M:%S %Y\n"
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
$ $ 
$ set 49 (Module: Annotate.c) is OBSOLETE.  Do NOT reuse this set number. 
$ $ 
$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 


$ Set 50 - message logging service 
$ 
$ The messages in this set will only appear in the DT log file.
$ The messages are used to identify messages written to this log file.
$ 
$ File: MsgLog.c
$ 
$set 50
1 INFORMATION
2 STDERR
3 FEHLERBEHEBUNG
4 WARNUNG
5 FEHLER
6 UNBEKANNT


$set 99

$    DO NOT TRANSLATE THESE MESSAGES 
$    DO NOT CHANGE THESE MESSAGES 
$    DO NOT LOCALIZE THESE MESSAGES 
$    These messages are used for the version information. 

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $TOG: dt.msg /main/7 1999/09/16 14:52:56 mgreess $"
