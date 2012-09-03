$ "$TOG: dtsession.msg /main/19 1999/09/20 13:48:24 mgreess $"; 
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *
$ *************************************<+>************************************* 
$ ***************************************************************************** 
$ ** 
$ **  File:        dtsession.msg 
$ ** 
$ **  Project:     HP DT  Session Manager (dtsession) 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dtsession 
$ ** 
$ ** 
$ ***************************************************************************** 
$ ** 
$ **     (c) Copyright 1990 Hewlett-Packard Company 
$ **     All Rights reserved 
$ ** 
$ ** 
$ ***************************************************************************** 
$ *************************************<L>************************************* 
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION--------------------- 
$ * Comment lines begin with a $ except $set which indicates the start of a  
$ * new set. 
$ * 
$ * Do not delete any comments; you may add comments for your use. 
$ * 
$ * Do not localize sets 2, 4, 6, 8, 10, 12, 14, 16. 
$ * Do not localize sets 20, 22, 24, 26, 28, 99.   
$ * They only appear in the errorlog. 
$ * 
$ * DO localize sets 18 and 29.  
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------ 
$ *************************************<L>************************************* 
$quote "

$set 2
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMain.c -- 
$ These are errorlog messages 
$ DO NOT TRANSLATE THIS SET. 

2 "Another dtsession is currently running - exiting."

3 "Screen lock when the screen saver is turned on is not available on this server.  Your screen will not be locked when the screen saver is turned on. "

$set 4

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmGlobals.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 

1 "Invalid display name - exiting."

2 "No defaults files exist.  No applications will be restarted."

3 "No defaults files exist.  No applications will be restarted."

4 "Unable to remove session directory.  Make sure write permissions exist on $HOME/.dt directory.  Invalid session files will not be removed."

5 "Unable to malloc memory for operation."

6 "Unable to lock display.  Another application may have the pointer or keyboard grabbed."

7 "Unable to open session file.  No clients will be restarted."

8 "Unable to fork client process."

9 "Unable to create HP DT directories.  Check permissions on home directory."

10 "Unable to lock display due to security restrictions."

$ The next message is added to the error log if a required temporary
$ file can not be created by the session manager.
$ The insert is the name of the temporary file.
11 "Unable to create the temporary file: %s. Make sure write permissions exist for this file. Exiting from Session Manager."

$set 6

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMsgSrv.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 

1 "Could not connect to ToolTalk message server:\n%sExiting ...\n"

2 "The HP DT message server has exited."

$set 8

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmError.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 

1 "Connection to server lost - exiting."

$set 10

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProperty.c -- 

$set 12

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProtocol.c -- 

$set 14

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmSave.c -- 

$set 16

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmRestore.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 

1 "Unable to exec process /usr/bin/X11/xrdb.  No session resources will be restored."

2 "Invalid client settings file.  No settings restored."

3 "Unable to exec %s."

4 "Unable to exec process /usr/bin/X11/dtwm.  No window manager will be started."

5 "You have reached the maximum allowed number of remote clients.  No further remote clients will be restored."

6 "Invalid display/language independent resource file.  No display/language independent resources will be restored."

7 "The wmStartupCommand resource is set to:\n\n\
      %s\n\n\
This file does not exist or is not executable.\n\
/usr/dt/bin/dtwm will be started instead.\n"

8 "An attempt to restore the following\n\
command (using the DT remote execution process)\n\
on host \"%s\" failed:\n\n\
      %s\n\n\
The following execution string will be tried:\n\n\
   %s\n\n"

9 "The following window manager did not start:\n\n\
      %s\n\n\
This message indicates you tried to start a\n\
window manager that is built into an X terminal.\n\
This will only work with X terminals that support this protocol.\n\
/usr/dt/bin/dtwm will be started instead.\n"

$set 18

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmUI.c --  
$ LOCALIZE Set 18 
$ Deleted Messages:  13, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 
$ \t\t\t\t\t29, 30, 31, 32, 33 
$ Message 1 tells the user who currently has the display locked. 

1 "Anzeige von Benutzer %s gesperrt."
$ Message 2 tells the user to enter the password for the above user to unlock 
$ the display. 

2 "Zum Entsperren Kennwort eingeben."
$ Message 3 is a prompt for the user to enter a password. 

3 "Kennwort: "
$ Message 4 is the logout message that appears when dtsession is going 
$ to save the current session information.  
$ Access this message by pressing the logout button. 

$ Message 4 is the logout message that appears when dtsession is going
$ to save the Current session information. 
$ Access this message by pressing the logout button.
$ 4 "Abmeldeprozeß gestartet...\n\n\
$ Nicht gesicherte Daten gehen unter Umständen verloren.\n\n\
$ Die aktuelle Sitzung wird gesichert; einige Anwendungen\n\
$ müssen jedoch eventuell nach erneutem Anmelden neu gestartet \n\
$ werden."

$quote

4 Die Desktop-Session wird beendet...\n\n\
Ihre aktuelle Session wird bei der Anmeldung wiederhergestellt.\n\n\
Ungespeicherte Aktualisierungen an der Anwendung gehen verloren.\n\n\
Mit der Abmeldung fortfahren?

$ Message 5 is the logout message that appears when dtsession is not going
$ to save the Current session information - but return to the Home session info
$ 5 "Abmeldeprozeß gestartet...\n\n\
$ Nicht gesicherte Daten gehen unter Umständen verloren.\n\n\
$ Diese Sitzung wird nicht gesichert. Statt dessen wird \n\
$ die Standardsitzung beim nächsten Anmelden wiederhergestellt."
5 Die Desktop-Session wird beendet...\n\n\
Ihre Standardsitzung wird bei der Anmeldung wiederhergestellt.\n\
Ihre aktuelle Session wird nicht gespeichert.\n\n\
Ungespeicherte Aktualisierungen an der Anwendung gehen verloren.\n\n\
Mit der Abmeldung fortfahren?

$ Message 6 is the title of the logout dialog box. 
$quote "
6 "Abmeldebestätigung"
$ Message 7,8,9,10 are all part of the logout dialog that appears when 
$ the user has requested to have the choice 
$ to either save the current session or return to the home session. 

$ Messages 7,8,9,10 have been OBSOLETED.
$ Do NOT translate messages 7, 8, 9 and 10
$ 7 "OBS- Abmeldeprozeß gestartet...\n\n\
$ Für die Desktop-Konfiguration beim nächsten Anmelden stehen zwei Möglichkeiten zur Auswahl:\n\n\
$ Die aktuelle Sitzung sichern, um beim nächsten Anmelden mit der aktuellen \n\
$ Konfiguration weiterzuarbeiten.\n\n\
$ oder\n\n\
$ Die aktuelle Sitzung verwerfen und die Standardsitzung wiederherstellen.\n\n\
$ In beiden Fällen gehen noch nicht gesicherte Daten verloren."

$ 8 "OBS- Abmelden:\n\
$ Aktuelle Sitzung sichern"

$ 9 "OBS- Abmelden:\n\
$ Standardsitzung wiederherstellen"

$ Message 10 is the title of the logout dialog box. 
$ 10 "OBS- Abmeldebestätigung"

$ Message 11,12 are part of the error dialog that appears when the DT 
$ messaging system dies. 
$ These messages are generated when you kill the BMS (broadcast Message 
$ Server), which is /usr/softbench/bin/softmsgsrv. 
$ Message 11 is obsolete.  Message 38 replaces message 11 
$ 11 DT Messaging System Inoperative
$ 
$ \

$ To restart:
$ 
$ \

$ 1) Save all open data files.
$ \

$ 2) Press the reset key sequence to return to the login screen.
$ \

$ 3) Log in again.
$ 
$ \

$ Note: The current session will not be saved.
$ 
$ \

$ When you are ready to begin the restart process, click [OK] and
$ \

$ proceed to save your files. 

$quote

$ 12 "Nachrichtenfehler"
12 Nachricht fehlgeschlagen.

$ Messages 15, 16, 17 are used for identical purpose as messages 1, 2,3 
$ in this set. 

$quote "
15 "Anzeige von Benutzer %s gesperrt."

16 "Zum Entsperren Kennwort eingeben."

17 "Kennwort: "
$ Message 34, 35 are messages that appear when the user started up in  
$ compatibility mode, and tries to log out.  You can only exit out of  
$ compatibility mode by using the reset key sequence. 
$ To generate this message, start in x11 (not DT) and then run dtsession. 
$ Refer to the dtsession manpage.  Then try to log out. 

34 "Diese Sitzung wurde über eine Startprozedur für X Window System gestartet.\n\n\
Es werden keine Sitzungsinformationen gespeichert.\n\n\
Zum Abmelden die Tastenfolge für Rücksetzen verwenden. "

$quote

$ 35 "Abmeldenachricht"
35 Abmeldenachricht

$ Messages 36,37 are part of the error dialog that appears when dtsession is 
$ unable to start the DT messaging system, and therefore not able to bring 
$ up the system at all. 
$ To generate this, move the message server from 
$ /usr/softbench/bin/softmsgsrv to some other directory, then start Dt. 
$ 36 "Das DT-Nachrichtensystem konnte nicht gestartet werden.\n\n\
$ Abhilfe:\n\n\
$ 1.  [OK] auswählen, um zur Anmeldeanzeige zurückzukehren.\n\n\
$ 2.  Fehlerbehebungssitzung im Optionsmenü der Anmeldeanzeige\n\
$     auswählen und am System anmelden.\n\n\
$ 3.  PrÏfen, ob der Host-Name an folgenden Stellen korrekt eingetragen ist:\n\
$      /etc/src.sh\n\
$      /etc/hosts\n\
$      /usr/adm/inetd.sec\n\n\
$ Nähere Informationen sind dem DT Benutzerhandbuch zu entnehmen."
36 Das Desktop-Nachrichtensystem konnte nicht gestartet werden.\n\n\
Zur Korrektur des Problems:\n\n\
1.  [OK] auswählen, um zum Anmeldebildschirm zurückzukehren.\n\n\
2. Failsafe-Session aus dem Optionsmenü des Anmeldebildschirms\n\
 auswählen und anmelden.\n\n\
3.  Überprüfen, ob das Desktop richtig installiert,\n\
         der Rechnername korrekt angegeben (/etc/hosts) und das\n\
         Netzwerk richtig konfiguriert ist.\n\n\
Weitere Informationen finden Sie im CDE-Benutzerhandbuch.

$quote "
37 "Aktion erforderlich"
$ Message 38 is part of the error dialog that appears when the DT 
$ messaging system dies. 
$ This messages are generated when you kill the BMS (broadcast Message 
$ Server), which is /usr/softbench/bin/softmsgsrv. 

38 "Nachrichtensystem nicht funktionsbereit.\n\n\
Um sich neu anzumelden,\n\n\
1) alle geöffneten Dateien sichern,\n\
2) vom System abmelden,\n\
3) wieder am System anmelden.\n\n\
Hinweis: Die aktuelle Sitzung wird nicht gespeichert.\n\n\
Wenn bereit, auf [OK] klicken und mit dem Sichern der \n\
Dateien fortfahren."

$ 
$ Messages 39-41 are displayed on the logout and/or session selection
$  confirmation dialogs.  These message are for dialog buttons

$quote

$ 
$ 39 "Continuer la déconnexion"
$ 40 "Annuler la déconnexion"
39 OK
40 Abbrechen
41 Hilfe

$ 
$ Messages 50-53 are for the display-specific session confirmation dialog
$ 
50 Standard
51 aktuelle 
$ Message 52 - The first '%s' in message 52 will be replaced with message 
$   50 or 51. The second '%'s in message 52 will be replaced with the 
$   value of the $DISPLAY environment variable (e.g. hostname:0).
52 Eine bildschirmspezifische '%s' Session wurde\n\
für die Anzeige '%s' ausgewählt, es existiert jedoch keine.\n\n\
Wenn Sie fortfahren, wird eine neue bildschirmspezifische Session\n\
erstellt.
53 Warnung

$ 
$ Message 55-56 are for the Session Save error dialog
$   55 is the dialog title
$   56 is the message in the dialog - do NOT localize 'save_state'
$ 
55 Fehlschlagen der Sessionspeicherung
56 Die Session kann nicht gespeichert werden, da der erforderliche\n\
Parameter 'save_state' nicht in der Nachricht enthalten war.\n\n\
Die Session wird nicht gespeichert.

$ 
$ Messages 70-71 refer to the name of the System Administration manual
$ 
70 Weitere Informationen folgen im Anschluß:
71 CDE-Benutzerhandbuch für Fortgeschrittene und Systemverwalter

$ 
$ Message 75 is used during logout if the user started a Current
$   session but used the Style Manager to return to a Home session.
$ 
75 Die Desktop-Session wird beendet...\n\n\
Ihre aktuelle Session wird gespeichert, aber Ihre Standard-\n\
Session wird bei der Anmeldung wiederhergestellt.\n\nUngespeicherte Aktualisierungen an der Anwendung gehen verloren.\n\n\
Mit der Abmeldung fortfahren?

$ 
$ Message 76 is used at the same time message 36 is used except that
$   message 76 is for Digital/OSF only and is slightly different than
$   message 36.
$ 
76 Das DT-Nachrichtensystem konnte nicht gestartet werden.\n\n\
Zur Korrektur des Problems:\n\n\
1. [OK] wählen, um zum Anmeldebildschirm zurückzukehren.\n\n\
2. Failsafe-Session aus dem Optionsmenü des Anmeldebildschirms\n\
         auswählen und anmelden.\n\n\
3.  Überprüfen, ob Ihr Rechnername korrekt in /etc/hosts zu finden ist, wenn Ihr\n\
     Netzwerk bereits konfiguriert wurde.\n\
4.  Falls Ihr Netzwerk noch nicht konfiguriert wurde, stellen Sie sicher, daß /etc/hosts\n\
     den folgenden Eintrag enthält:\n\
     127.0.0.1 localhost \n\n\
Zusätzliche Informationen finden Sie im CDE-Benutzerhandbuch.

$ 
$ Message 99 is displayed when a user logs in for the first time
$ 
99 Die neue Datei '.dtprofile' wurde Ihrem Standardverzeichnis hinzugefügt.\n\
\n\
Folgen Sie den Anweisungen in dieser Datei, um sicherzustellen, daß bei der erneuten Anmeldung\n\
Ihre Datei '.login' oder '.profile' aktiviert ist, und \n\
daß diese korrekt mit CDE zusammenarbeitet. \n\
\n\
Zusätzliche Informationen finden Sie im CDE-Benutzerhandbuch für Fortgeschrittene \n\
und Systemverwalter.

$set 20

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmWindow.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 

$quote "
1 "Client message failed.  Client information will not be saved."

2 "Invalid root window.  Can not save client information."

3 "Session restoration information not updated for client .  Invalid information may be saved."

4 "Session restoration information not updated for client %s.  Invalid information may be saved."

$set 22

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmLock.c -- 

$set 24

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- version.c -- 

$set 26

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvPalette.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 

1 "Color Server Warning: couldn't get ownership of the selection:"

2 "Color Server Warning, losing ownership of the selection:"

3 "Should never lose the selection."

4 "Warning, found more pixels then are available."

5 "Warning, can't allocate enough pixels."

$set 28

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvFile_io.c -- 
$ This entire set is for the errorlog. 
$ DO NOT TRANSLATE THIS SET. 
$ Messages 1,2,6,7,8,9 are obsolete 
$ 1 Color Server Warning, the size of file ' 
$ 2 ' is invalid
$ removing file and trying again. 
$ 6 Color Server FatalError, could not make directory 
$ 7 Color Server FatalError: couldn't open or create 
$ 8 Color Server FatalError, couldn't open file 
$ 9 Color Server FatalError, couldn't open system file 

3 "Color Server Warning, the size of file is invalid:"

4 "Color Server Warning, invalid information in '"

5 "' removing file and starting again.\n"

10 "Color Server FatalError, not enough memory to create the structure for a palette.\n"

11 "Warning: Color Server system default file is corrupt\n"

12 "Color Server Warning, could not read file %s\n"

$set 29

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmHelp.c --  
$ LOCALIZE Set 29 

1 "Hilfe für Sitzungsverwaltung"


$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$ 
$ Set 40 is for messages related to the X11R6 Session Management Protocol
$ 
$   o Do NOT localize the string '%s' in any of the messages in this set.
$ 
$set 40
$ Messages 1-7 are used when a session cannot be started.
$ Message 1 - this message will preface messages 2-7

$quote

1 Eine Session kann wegen des folgenden Fehlers\n\
nicht gestartet werden:
2 SmsInitialize fehlgeschlagen.
3 IceListenForConnections fehlgeschlagen.

$ 
$ Do not remove the whitespace at the beginning of lines 2 and 3
$   for message number 4
$ 
4 Die Authentifikationsdatei ~/.ICEauthority konnte nicht erstellt werden.\n\
   Wenn die Dateien ~/.ICEauthority-c und ~/.ICEauthority-l existieren,\n\
   müssen sie entfernt werden, bevor Ihre Session gestartet werden kann.

5 IceAddConnectionWatch fehlgeschlagen.
6 IceComposeNetworkIds fehlgeschlagen.
$ Message 7 - Do NOT translate "SESSION_MANAGER".
7 Konnte nicht die Umgebungsvariable SESSION_MANAGER erstellen.
$ 
$ Messages 8-9 occur when a session database cannot be opened.
$ 
$ Message 8 - Do NOT remove the whitespace before the last '%s'.
8 Die folgende Session-Datenbank konnte nicht geöffnet werden:\n\n\
   '%s'
$ Message 9 - Do NOT remove the whitespace before the last '%s'.
9 Die folgende Session-Datenbank enthält nicht\n\
die erforderliche Ressource '%s':\n\n\
   %s
$ 
$ Messages 10-11 are used to report errors when reading a session database
$ 
$ Message 10 - Do NOT remove the whitespace before the last '%s'.
10 Die erforderliche Eigenschaft '%s' fehlt für Client\n\n\
   %s\n\n\
in der Datei '%s'.
11 Die erforderliche Ressource '%s' fehlt für Client '%d'\n\
in der Datei '%s'.
$ 
$ Messages 12-14 used if errors occur when a session is restarted
$ 
$ Message 12 - Do NOT remove the whitespace before the last '%s'.
12 Das Verzeichnis '%s'\n\
ist für die Anwendung '%s' nicht verfügbar.\n\n\
Das folgende Verzeichnis wird benutzt:\n\n\
   %s
$ Message 13 - Do NOT remove the whitespace before the last '%s'.
13 Die folgende Anwendung kann auf dem Rechner '%s' nicht gestartet werden,\n\
da dieser Rechner nicht vom Rechner '%s' erreicht werden kann:\n\n\
   %s
$ Message 14 - Do NOT remove the whitespace before the third '%s'.
$ Message 14 - "cannonical form" in this context means an internal
$   format.  
14 Ein Versuch, die Anwendung '%s'\n\
auf dem Rechner '%s' zu starten, schlug fehl, weil das folgende Verzeichnis\n\
nicht in kanonische Form (internes Format) übersetzt werden konnte:\n\n\
   %s\n\n\
[%s]

$ 
$ Message 15 - used if the -session command line option has no value
$ 
15 Es wurde für die Befehlszeilenoption -session kein Session-Name angegeben.
$ 
$ Message 16 - used if the Save_Session message contains an unrecognized
$    argument
$ 
16 Die Meldung Save_Session enthält das unerkannte Argument '%s'.

17 Der Versuch schlug fehl, eine Clientliste von der 'Fenstersteuerung' zu holen.
$ 
$ Message 18 - the client database cannot be opened for writing.
$ Do NOT remove the whitespace before the last '%s'.
$ 
18 Die folgende Datenbankdatei kann nicht zum Schreiben geöffnet werden:\n\n\
   %s\n\n\
Die Session wird nicht gespeichert.
$ 
$ Message 19 - used if an application cannot be saved
$ 
19 Anwendung '%s' kann nicht gespeichert werden.
$ 
$ Message 20 - used if the IceAcceptConnection function fails:
$ 
20 IceAcceptConnection fehlgeschlagen.
$ 
$ Message 21 - used if a Save Session is requested and one is already
$    in progress
21 Die Session wird nicht gespeichert, da schon eine Session gerade gespeichert wird.

22 Die Abschaltung einer Session wurde durch die Anwendung '%s' abgebrochen.

$ 
$ Messages 23-25 are used when a session is saved and an application 
$    has not set its required properties
$ 
$ Message 23 - Do NOT remove the whitespace before the last '%s'.
23 Die folgende Anwendung hat die erforderliche\n\
Eigenschaft '%s' nicht eingestellt:\n\n\
   %s\n\n\
Diese Anwendung wird nicht gespeichert.

24 Anwendung '%s' wird nicht gespeichert, weil sie die\n\
erforderliche Eigenschaft '%s' nicht eingestellt hat.

25 Anwendung '%s' hat die erforderliche Eigenschaft '%s' nicht eingestellt.

$ 
$ Messages 26-27 are used when the DiscardCommand properties are
$    invoked - before an old session directory is removed.
$ Message 26 - do NOT remove the whitespace before the '%s'.
$ Message 26 - do NOT translate the word 'DiscardCommand' in
$    this message
26 Die folgende Client-Datenbank kann nicht geöffnet werden:\n\n\
   %s\n\n\
Die Eigenschaft 'DiscardCommand' kann nicht ablaufen.

$ 
$ Message 27 - do NOT translate the word 'DiscardCommand' in
$    this message
27 Der Versuch schlug fehl, die Eigenschaft 'DiscardCommand' für die\n\
Anwendung '%s' auszuführen.

$ 
$ Messages 28-29 - used when an application exists and it wants the
$    Session Manager to post a dialog with a message that describes
$    the reason the client exited.
$ Message 29 is the title for the dialog
$ 
28 Anwendung '%s' aus folgendem Grund beendet:\n\n\
%s
29 Gründe für Schließen der Anwendung

$set 99

$    DO NOT TRANSLATE THESE MESSAGES 
$    DO NOT CHANGE THESE MESSAGES 
$    DO NOT LOCALIZE THESE MESSAGES 
$    These messages are used for the version information. 

$quote "
1 "@(#)version_goes_here"

2 "\n\
@(#)Message catalog source $TOG: dtsession.msg /main/19 1999/09/20 13:48:24 mgreess $"
