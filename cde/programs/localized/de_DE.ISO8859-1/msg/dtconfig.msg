$ $XConsortium: dtconfig.msg /main/6 1996/10/19 14:26:41 rws $
$ ***************************************************************************** 
$ ** 
$ **  File:        dtconfig.msg 
$ ** 
$ **  Project:     CDE Session Management (dtconfig) 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dtconfig 
$ ** 
$ **  Message set 10: IBM 
$ **  Message set 20: Sun 
$ ** 
$ ***************************************************************************** 
$ ***************************************************************************** 
$ **  Message set 10: For IBM platforms 
$ ***************************************************************************** 
$quote "

$set 10

1 "Kann nur mit root-Berechtigung ausgeführt werden. \n"
$  

2 "Die Desktop Umgebung ist nun die Standardbenutzerschnittstelle. Diese \n\
Umgebung wird für alle Benutzer beim Anmelden erscheinen. Damit die\n\
Änderung wirksam wird, einen Systemabschluß durchführen und das System\n\
neu starten.\n"
$  

3 "Die Befehlszeileneingabe ist nun die Standardbenutzerschnittstelle.\n\
Sie wird für alle Benutzer beim Anmelden erscheinen. Damit die\n\
Änderungen wirksam wird, einen Systemabschluß durchführen und\n\
das System neu starten.\n"

4 "Die Desktop Umgebung ist bereits die Standardbenutzerschnittstelle.\n"
$  

5 "Die Befehlszeileneingabe ist bereits die Standardbenutzerschnittstelle.\n"
$  

6 "Verwendung: %s\n\
  -e (automatischen Start von dtlogin aktivieren)\n\
  -d (automatischen Start von dtlogin inaktivieren)\n\
  -kill (dtlogin abbrechen)\n\
  -reset (dtlogin zurücksetzen - Konfigurationsdateien erneut lesen)\n"
$  

7 "Zu viele Argumente \n"
$  

8 "ein Argument benötigt \n"
$  

9 "xdm konnte nicht inaktiviert werden, Bitte die Hilfeseite zu xdm\n\
heranziehen, um xdm zu inaktivieren und noch einmal versuchen. \n\
 "
$  

10 "Dtlogin ist nicht aktiv\n"
$ ***************************************************************************** 
$ **  Message set 20: For Sun Platforms 
$ ***************************************************************************** 

$set 20

$  

1 "CDE-Konfigurationsprogramm"
$  

2 "%s -d     (automatischen Start inaktivieren)"

3 "%s -e     (automatischen Start aktivieren)"

4 "%s -kill  (dtlogin abbrechen)"

5 "%s -reset (dtlogin zurücksetzen)"

6 "%s -p     (Druckeraktion aktualisieren)"
$  
$ In above messages 2 through 6, only translate words between 
$ left "(" and right "). 
$  

7 "%s: Dieses Skript muß als root ausgeführt werden"

8 "Verwendung: Zu viele Argumente"

9 "Verwendung: Ein Argument erforderlich"

10 "Option gescheitert"

11 "%s konnte nicht ausgeführt werden"

12 "Inaktivieren gescheitert"

13 "%s konnte nicht entfernt werden"

14 "fertig"

15 "Automatischer Desktop-Start inaktiviert."

16 "Aktivieren gescheitert"

17 "%s konnte nicht gefunden werden"

18 "%s konnte nicht erstellt werden"

19 "%s konnte nicht erstellt werden"

20 "Automatischer Desktop-Start aktiviert."

21 "Abbrechen von dtlogin beendet."

22 "Konfigurationsressourcen von dtlogin wurden erneut geladen."

23 "Aktualisieren der Druckeraktion beendet."

24 "Verwendung:"
$ 
25 " %s -inetd    (inetd.conf /usr/dt daemons)"
26 " %s -inetd.ow (inetd.conf /usr/openwin daemons)"
$ 
$	In above messages 25 and 26, only translate words between
$	left "(" and right ").
$ 
27 "Beim nächsten Systemstart werden die folgenden Dateien vom Verzeichnis /usr/dt/bin abgerufen."
$ 
28 "    rpc.ttdbserverd         (ToolTalk)"
29 "    rpc.cmsd                (Kalender-Manager)"
30 "    dtspcd                  (Unterprozeßsteuerung)"
$ 
$  Keep column allignment on messages 28 thru 30.  
$  Only translate what's beteen ( )'s
$ 
31 "Beim nächsten Systemstart werden die folgenden Dateien vom Verzeichnis /usr/openwin/bin abgerufen."
$ 
32 "    rpc.ttdbserverd         (ToolTalk)"
33 "    rpc.cmsd                (Kalender-Manager)"
$ 
$  Keep column allignment on messages 32 thru 33.  
$  Only translate what's beteen ( )'s
$ 
