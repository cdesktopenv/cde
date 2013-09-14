$ $TOG: SUNW_TOOLTALK.msg /main/5 1999/09/16 13:43:13 mgreess $
$quote "

$set 1

3 "clnt_create bei rpc.ttdbserverd für %s fehlgeschlagen%s"

4 "Fehler: rpc.ttdbserverd kann nicht auf %s ausgeführt werden"

6 "Die Server-Version (%s) entspricht nicht der Version in den Datenbanktabellen (%s). Bitte einen Datenbank-Server der Version %s verwenden."

7 "Versuch die Datenbanktabelle %s:%s zu öffnen ist fehlgeschlagen"

14 "Das Merkmal %s des X-Root-Fensters, das diese Session vorgeschlagen hat, konnte nicht gelöscht werden."

15 "Die Adreßversion ist %d, verstanden wird nur %d! (Adresse: <%s>)"

17 "Syntaktische Analyse des folgenden Namens der X-Anzeige nicht möglich: \"%s\"."

18 "Eine Meldung konnte nicht an Datei \"%s\" angehängt werden aufgrund des internen Fehlers %d"

19 "Die alte Session <%s> konnte nicht von der Anforderungsliste \"%s\" gelöscht werden aufgrund des internen Fehlers %d.."

20 "Die nicht angeforderte Session <%s> konnte nicht von der Dateianforderungsliste \"%s\" gelöscht werden aufgrund des internen Fehlers %d.."

21 "tttracefile-Syntaxfehler in Zeile:\n\
%s"

22 "tttracefile konnte nicht in $%s verarbeitet werden"

23 "Keine Ablaufverfolgung zulässig, wenn tatsächliche UID und effektive UID nicht übereinstimmen."

24 "Anforderung wird akzeptiert"

25 "Leeres Dokument und keine Datei"

26 "Die Kennung der Meldung kann nicht abgerufen werden; die Operation kann möglicherweise aufgrund von tt_message_arg_val() nicht abgebrochen werden."

27 "Titel kann nicht abgerufen werden; das Dokument kann aufgrund von tt_message_arg_val() nicht betitelt werden."

28 "Unbekannte relative Position der Operationsnummer"

$set 2


2 "Nicht unterscheidbare Signaturen für ptype %s - gefunden."

3 "Kein verbindungsorientierter Transport"

4 "Ein ToolTalk-Client ist ausgefallen, bevor ihm signalisiert werden konnte, daß er eine fällige Meldung abrufen soll."

5 "Die Verbindung zum ToolTalk-Client ist verlorengegangen, als ihm signalisiert wurde, er solle eine Meldung abrufen."

6 "Andere laufende Session wurde festgestellt (Host=%s, PID=%d)"

7 "Zur angegebenen Session (Host=%s, PID=%d) konnte keine Verbindung hergestellt werden; sie ist vermutlich abgestürzt..."

8 "Syntaktische Analyse der vorgeschlagenen Session-ID nicht möglich; wird mit eigener Session-ID überschrieben..."

9 "%s ist eine Typendatenbank Version %d. Von dieser Version können nur Versionen bis %d gelesen werden."

10 "Die Typen folgender Typendatenbank konnten nicht decodiert werden: %s. Sie ist möglicherweise beschädigt."

11 "%s wurde überschrieben"

12 "Unbekanntes Attribut <%s> der ToolTalk-Signatur wird ignoriert..."

13 "ttce2xdr fehlgeschlagen (Status=%d); Typen in \"Benutzer\"-Datenbank Classing Engine sind nicht umgesetzt..."

14 "Doppelt vorhandener ISAM-Datensatz"

15 "ISAM-Datei ist nicht offen"

16 "Unzulässiges ISAM-Argument"

17 "Unzulässiger ISAM-Tasten-Deskriptor"

18 "Zu viele ISAM-Dateien offen"

19 "Fehlerhaftes ISAM-Dateiformat"

20 "Nicht exklusiver ISAM-Zugriff"

21 "ISAM-Datensatz gesperrt"

22 "ISAM-Taste ist bereits vorhanden"

23 "Derie ISAM-Schlüssel ist der Hauptschlüssel"

24 "Ende oder Anfang der ISAM-Datei"

25 "Kein ISAM-Datensatz gefunden"

26 "Kein aktueller ISAM-Datensatz"

27 "ISAM-Datei gesperrt"

28 "Name der ISAM-Datei zu lang"

29 "ISAM kann keinen Speicher zuordnen"

30 "ISAM RPC-Zeitüberschreitung"

31 "Unterbrochene ISAM TCP/IP-Verbindung"

32 "Verbindung zum ISAM-Server kann nicht hergestellt werden"

33 "ISAM-Daten konnten nicht importiert werden"

34 "Kein lokaler SAM-Dämon"

35 "Interner schwerwiegender Fehler bei ISAM"

36 "Nichtübereinstimmung von Lokal/LANG bei ISAM"

$set 3


2 "Kind (%d) aufgrund von Signal %d%s beendet"

3 " (Kernspeicherauszug)"

4 "Kind (%d) wurde weder beendet noch hat es eine Information empfangen"

5 "Clients können aufgrund von %m nicht erweitert werden"

6 "Kein Bereich zu verwalten. Die Optionen -c, -p, -d verwenden oder $DISPLAY einstellen."

7 "Option -S kann nicht mit -c verwendet werden."

8 "ttSession des Kindes mit Status %d beendet."

9 "ttSession des Kindes aufgrund von Signal %d%s beendet."

10 " (Kernspeicherauszug)"

11 "ttSession des Kindes wurde nicht beendet noch wurde eine Information empfangen."

12 "Start"

13 "Typen wurden erneut gelesen"

14 "Fehlerhafte Typen; alte Typen beibehalten"

15 "$OPENWINHOME nicht eingestellt"

16 "\n\
Verwendung: ttsession [-a unix|des][-d Anzeige][-spStvhNX"

17 "][-c [Befehl]            \n\
 -c [Befehl] \tstartet eine Prozeßstruktursitzung und führt den Befehl in dieser Session aus.\n\
 Die folgenden Optionen werden an den Befehl übergeben. Standardeinstellung: $SHELL\n\
 -p\tstartet eine Prozeßstruktursitzung und gibt die Kennung dieser Session aus. \n\
 -d Anzeige\tstartet eine X-Session auf der Anzeige.\n\
 -a unix|des\tstellt die Ebene der Identifikationsüberprüfung für den Server ein.\n\
 -s\tstumm; es werden keine Warnungen ausgegeben.\n\
 -S\tnicht in den Hintergrund verzweigen.\n\
 -N\tdie Anzahl der zulässigen Clients maximieren\n\
 -t\tMeldungsverfolgung einschalten\n\
 -X\tfür statische Typen XDR-Datenbanken verwenden (Standardeinstellung)\n"

18 "-E     für statische Typen Classing Engine verwenden\n"

19 "\n\
-v     Versionsnummer ausgeben\n\
-h            diese Meldung ausgeben.\n\n\
 Signalschnittstelle:\n\
kill - USR1 ttSession_PID            Meldungsverfolgung umschalten\n\
kill - USR2 ttSession_PID            statische Typen erneut lesen"

20 "Beenden"

$set 4


2 "Verwendung:\n\
 tt_type_comp [-s] [-d db] [-mM] Quellendatei\n\
 tt_type_comp [-s] [-d db] -r Typ ...\n\
 -M\tQuellentypen in angegebene Datenbank mischen, existierende Typen nicht aktualisieren\n\
 -m\tmischen, aber existierende Typen aktualisieren. Standardeinstellung.\n\
 -r\tQuellentypen aus der angegebenen Datenbank löschen\n\
 -d db\tzu bearbeitende Datenbank. Entweder: user, system oder network. Standard: user\n\
 -G\tToolTalk-Datenbank-Server bereinigen (garbage collection).\n\n\
 tt_type_comp [-sE] -p|O|P [-d db]\n\
 tt_type_comp [-s]  -p|O|P komp_Datei\n\
 -O\tNamen aller gelesenen O-Typen auf stdout ausgeben\n\
 -P\tNamen aller gelesenen P-Typen auf stdout ausgeben\n\
 -p\talle gelesenen ToolTalk-Typen formatiert auf stdout ausgeben\n\
 -E\tClassing Engine-Datenbank(en) anstelle von XDR-Datenbank(en) verwenden\n\
 -d db\tzu lesende Datenbanken. Entweder: user, system oder network. Standard: alle\n\n\
 tt_type_comp [-s] -x [-o komp_Datei] Quellendatei\n\
 -x\tTypen aus Quellendatei kompilieren (oder stdin, falls Datei \"-\" ist)\n\
 -o\tkompilierte Typen in komp_Datei schreiben (oder stdout, falls Datei \"-\" ist)\n\
 \tStandard: Quellendatei.xdr oder \"types.xdr\", falls Quelle stdin\n\n\
 tt_type_comp [-hv]\n\
 -v\tVersionsnummer ausgeben\n\
 -h\tdiesen Hilfetext ausgeben\n\
 -s\tkeine Statusmeldungen ausgeben\n\n\n\
 Diese cpp-Optionen werden an folgenden Befehl übergeben:\n\
           -undef -Dname -IVerzeichnis -Uname -YVerzeichnis"

3 "Semantischer Fehler in Typendatei"

4 "Keine gültige ToolTalk-Typendatei"

5 "In der Typendatenbank Classing Engine können keine ToolTalk-Typen gelesen werden."

6 "Das Mischen von Tabellen der Datenbank Classing Engine wird nicht mehr unterstützt."

7 "Lesen der Typen in Datenbank %s nicht möglich - Versionen stimmen nicht überein."

8 "Lesen der Typen in Datenbank %s nicht möglich"

9 "$OPENWINHOME nicht eingestellt"

10 "Datenbank %s kann nicht für das Schreiben initialisiert werden"

11 "Alte Definition für %s konnte nicht entfernt werden"

12 "Überschreiben"

13 "Writing"

14 "Could not add new definition for %s"

15 "Could not remove old definition for %s"

16 "Überschreiben"

17 "Schreiben"

18 "Neue Definition für %s konnte nicht hinzugefügt werden"

19 "Nichtübereinstimmende Versionen bei kompilierten Typen"

20 "Typen der Datenbank können nicht gelesen werden"

21 "$OPENWINHOME nicht eingestellt"

22 "Typen in Datenbank %s können nicht gelesen werden - Versionen stimmen nicht überein"

23 "Typen in Datenbank %s können nicht gelesen werden"

24 "$OPENWINHOME nicht eingestellt"

25 "otype %s wird entfernt\n"

26 "ptype %s wird entfernt\n"

27 "Ausgabe wurde in %s geschrieben\n"

28 "Ungültige Datenbank: %s"

29 "Nur eine der Optionen -O -P -m -M -p -r -x angeben"

30 "Es erfolgte keine Vorverarbeitung, weil: %s: %s"

31 "otype-Vererbungszyklus unter Beteiligung von %s"

32 "Übergeordneter Typ %s von otype %s ist nicht vorhanden"

33 "ptype %s ist nicht vorhanden, wurde jedoch von otype %s als Implementor benannt"

34 "Im otype %s wurden mehrere verarbeitende Prozesse definiert"

35 "Vererbte Signatur in otype %s ist im Elter nicht vorhanden"

36 "Zwei ptypes, %s und %s, mit demselben verarbeitenden Prozeß"

37 "Übergeordneter Typ %s von otype %s ist nicht vorhanden"

38 "Zyklische otype-Vererbungshierarchie - {"

39 "Doppelt vorhandene Definition von otype %s"

40 "Doppelt vorhandene Definition von ptype %s"

$set 5


3 "Sicherheitsverletzung: RPC-Anforderung für Öffnen einer Datei, die keine ToolTalk-Datenbank ist."

4 "Alle Daten, die mit einer alten Version (<= 1.0.2) von rpc.ttdbserverd geschrieben wurden, werden ignoriert, wenn eine neue Version (>= 1.1) von rpc.ttdbserverd verwendet wurde."

5 "Alle Daten, die mit einer alten Version (<= 1.0.2) von rpc.ttdbserverd geschrieben wurden, werden ignoriert, wenn eine neue Version (>= 1.1) von rpc.ttdbserverd verwendet wurde."

6 "Die Version von rpc.ttdbserverd (%s) stimmt nicht mit der Version der Datenbanktabellen (%s) überein. Bitte rpc.ttdbserverd ab Version %s installieren."

7 "Verwendung:\n\
rpc.ttdbserverd [-S] [-n] [-m DTMOUNTPOINT]\n\
rpc.ttdbserverd [-S] [-v]"

8 "'garbage collection thread' kann nicht gestartet werden. thr_create()\n"

9 "fork() für 'garbage collection' kann nicht ausgeführt werden.\n"

$set 6


2 "Verwendung:\n\
ttdbck [-f Datei] [-k Objektschlüssel] [-t Typ] [-bx] \n\
[-impa] [-IZ] [-F neuer_Dateiname] [-T neuer_Typ] [Mount_Punkte]\n"

3 "ttdbck: Es muß eine Auswahloption [-fkt] oder eine Diagnoseoption [-b x]\n\
 angegeben werden, wenn eine Reparaturoption [-FTZ] angegeben wurde.\n"

4 "Nicht übereinstimmende Versionen bei kompilierten Typen"

5 "Die Typen dieser Datenbank können nicht gelesen werden."

6 "ttdbck: 'ttdbck -I' versuchen.\n"

7 "Auswahl nach Dateiname: %s\n"

8 "Auswahl nach Objekt-ID-Schlüssel: "

9 "Auswahl nach Typ: %s\n"

10 "Fehlerhaft geformte Definitionseinheiten durch Diagnose\n"

11 "Verweis auf nicht vorhandene Definitionseinheiten bei Diagnose\n"

12 "Anzeige-IDs\n"

13 "Anzeige verbindlicher Daten\n"

14 "Anzeige von Merkmal- und Wertdaten\n"

15 "Aufrufen der NetISAM-Funktion isrepair() vor Inspektion\n"

16 "Reparatur durch Einstellen auf Typ: %s\n"

17 "Reparatur durch Löschen\n"

18 "Testhilfeausdruck Ebene %d\n"

19 "Reparatur durch Einstellung auf Datei: %s\n"

20 "Fehler: keine Datei für Spezifikation.\n"

21 "Fehler: kein Typ für Spezifikation.\n"

22 "Fehler: \"%s\" ist kein installierter otype.\n"

23 "Fehler: Spezifikation beinhaltet mehrere Werte für Typenmerkmal.\n"

24 "Fehler: "

25 "ttdbck: keine Fehler festgestellt.\n"

26 "Fehler: "

$set 7


2 "Verwendung: %s {ctx}[fhpPv[v]] [Zieldatei] Pfadname ...\n\
            %s {ctx}fL[hpPRv[v]] tttarfile Pfadname ...\n\
            %s -v\n\
            %s -h\n"

3 "\tc           ein Archiv erstellen\n\
\tt           den Inhalt eines Archivs auflisten\n\
\tx           Auszug aus einem Archiv\n\
\tf           nächstes Argument <Zieldatei> als Archiv verwenden\n\
\th           symbolische Verbindungen befolgen\n\
\tL           tar(1) nicht aufrufen\n\
\tp           Dateimodi beibehalten\n\
\tP           (root) Eigentümer, Änderungszeit, etc. des Objekts nicht beibehalten\n\
\tR           keine Unterverzeichnisse verwenden\n\
\tv           ausführlicher Statusausgabe\n\
\tvv          Statusausgabe mit allen Details\n\
\t-v          Versionsnummer ausgeben und Ende\n\
\t-h[elp]     diesen Hilfetext ausgeben\n\
  \n"

4 "%s: Objekttyp konnte nicht im Archivdatenstrom gelesen werden.\n"

5 "%s: Archivversion konnte nicht im Archivdatenstrom gelesen werden.\n"

6 "%s: Archivversion %d festgestellt, obwohl Version %d erwartet wurde.\n"

7 "%s: Im Archiv wurde ein Objekt unbekannten Typs (%d) gefunden.\n"

8 "%s: Objekttyp konnte nicht im Archivdatenstrom gelesen werden.\n"

9 "%s: Archivversion konnte nicht im Archivdatenstrom gelesen werden.\n"

10 "%s: Archivversion %d festgestellt, obwohl Version %d erwartet wurde.\n"

11 "%s: Im Archiv wurde ein Objekt unbekannten Typs (%d) gefunden.\n"

$set 8


2 "%s: Entfernen der ToolTalk-Objekte von %s aufgrund von %s nicht möglich\n"

3 "%s: Verschieben der ToolTalk-Objekte von \"%s\" bis \"%s\" aufgrund von %s nicht möglich\n"

4 "%s: Es wird kein Versuch unternommen, die folgenden ToolTalk-Objekte zu verschieben:\n"

5 "Verwendung: %s [-] [-fL] Pfad1 Pfad2\n\
            %s [-] [-fL] Pfad1 [Pfad2 ...] Verzeichnis\n\
            %s -v\n\
            %s -h\n"

6 "\t-L           mv(1) nicht ausführen\n\
\t-v            Versionsnummer ausgeben und quittieren\n\
\t-h            diesen Hilfetext ausgeben\n"

7 "%s: Entfernen der ToolTalk-Objekte von %s aufgrund von %s nicht möglich\n"

8 "Verwendung: %s [-] [-%s] %s ...\n\
            %s -v\n\
            %s -h\n"

9 "Datei"

10 "Verzeichnis"

11 "\t-L          %s(1) nicht ausführen\n\
\t-v          die Versionsnummer ausgeben und quittieren\n\
\t-h[elp]    diesen Hilfetext ausgeben\n"

12 "%s: Entfernen der ToolTalk-Objekte von %s aufgrund von %s nicht möglich\n"

13 "Verwendung: %s [-pL] Datei1 Datei2\n\
            %s [-prRL] Pfad1 [Pfad2 ...] Verzeichnis\n\
            %s -v\n\
            %s -h\n"

14 "\t-L           cp(1) nicht ausführen\n\
\t-v           die Versionsnummer ausgeben und quittieren\n\
\t-h           diesen Hilfetext ausgeben\n"

$set 9


2 "Verwendung: %s [-0FCa][-o Ausgabedatei] [-S Session | Befehl [Optionens]]\n\
            %s [-e Prozedur | -f Script_Datei][-S Session | Befehl [Optionen]]\n\
 -0               Meldungsverfolgung in Session ausschalten oder den Befehl\n\
                   ohne Meldungsverfolgung (d.h. nur API-Ablaufverfolgung) ausführen\n\
 -F               Allen durch Befehl verzweigten oder später in der Session mit\n\
                   ttsession(1) gestarteten Kindern folgen\n\
 -C               Keine Ablaufverfolgung für ToolTalk-API-Aufrufen\n\
 -a               Alle Attribute, Argumente und zugehörigen Steckplätze\n\
                   für verfolgte Meldungen ausgeben. Die Standardeinstellung\n\
                   ist eine einzeilige Zusammenfassung.\n\
 -e Prozedur      Einstellungen für tttracefile(4) in Prozedur lesen\n\
 -f Script_Datei   Einstellungen für tttracefile(4) in Script-Datei lesen. \n\
 \"-\": stdin.\n\
 -o Ausgabedatei   Ausgabe. \n\
 \"-\": stdout. Standardeinstellung: stdout für Session-Verfolgung, \n\
                 stderr (von tttrace) für Befehlsverfolgung\n\
 -S Session     Zu verfolgende Session. Standardeinstellung:\n\
                  siehe Befehl tt_default_session() \n\
Befehl des ToolTalk-Client zum Aufrufen und für Ablaufverfolgung\n"

3 "%s: Session <%s> unterstützt nicht die Session-Verfolgung. Stattdessen kill -USR1 verwenden. Siehe ttsession(1).\n"

$set 10


2 "Übergabe von unbekannter Tt_address an Tts_address::add() wird ignoriert\n"

3 "Übergabe von unbekannter Tt_address an Tts_address::remove() wird ignoriert\n"

4 "Übergabe von unbekannter Adresse an tts_address::tts_set() wird ignoriert\n"

5 "Übergabe von unbekannter Adresse an tts_address::tts_unset() wird ignoriert\n"

6 "Lesen von unbekannter Adresse durch tts_address::load() wird ignoriert\n"

7 "Tts_arglist::Tts_arglist - Fehler beim Lesen des Arguments - \n\
\t"

8 "Tts_arglist::load - Unbekannter Argumententyp festgestellt - BARG verwenden\n"

9 "Abstrakte Klasse für Tts_arg::assign (Tt_pattern) aufgerufen - wird ignoriert\n"

10 "Abstrakte Klasse für Tts_arg::assign(Tt_message) aufgerufen - wird ignoriert\n"

11 "Abstrakte Klasse für Tts_arg::generate aufgerufen - wird ignoriert\n"

12 "Abstrakte Klasse für Tts_arg::save aufgerufen - wird ignoriert\n"

13 "Abstrakte Klasse für Tts_arg::load aufgerufen - wird ignoriert\n"

14 "Tts_arg::mode_to_chars ungültiger Modus - TT_INOUT verwenden\n"

15 "Tts_arg::chars_to_mode ungültiger Modus - TT_INOUT verwenden\n"

16 "Übergabe von unbekannter Tt_category an Tts_category::set() wird ignoriert\n"

17 "Tts_category::generate() hat eine Meldung angefordert - wird ignoriert\n"

18 "Lesen von unbekannter Kategorie durch tts_category::load() wird ignoriert\n"

19 "Übergabe von unbekannter Tt_class an Tts_class::add() wird ignoriert\n"

20 "Übergabe von unbekannter Tt_class an Tts_class::remove() wird ignoriert\n"

21 "Übergabe von unbekannter Klasse an tts_class::tts_set() wird ignoriert\n"

22 "Übergabe von unbekannter Klasse an tts_class::tts_unset() wird ignoriert\n"

23 "Lesen von unbekannter Klasse durch tts_class::load() wird ignoriert\n"

24 "Tts_connection::universal_callback - keine Musterformung für Suchmuster!\n"

25 "Tts_cntxtlist::Tts_cntxtlist - Fehler beim Lesen des Kontexts - \n\
\t"

26 "Tts_contextlist::load - Unbekannter Kontexttyp festgestellt - BCNTXT verwenden\n"

27 "Abstrakte Klasse für Tts_context::assign (Tt_pattern) aufgerufen - wird ignoriert\n"

28 "Abstrakte Klasse für Tts_context::assign (Tt_pattern) aufgerufen - wird ignoriert\n"

29 "Abstrakte Klasse für Tts_context::generate aufgerufen - wird ignoriert\n"

30 "Abstrakte Klasse für Tts_context::save aufgerufen - wird ignoriert\n"

31 "Abstrakte Klasse für Tts_context::load aufgerufen - wird ignoriert\n"

32 "Übergabe von unbekannter Tt_disposition an Tts_disposition::add() wird ignoriert\n"

33 "Übergabe von unbekannter Tt_disposition an Tts_disposition::remove() wird ignoriert\n"

34 "Übergabe von unbekannter Disposition an tts_disposition::tts_set() wird ignoriert\n"

35 "Übergabe von unbekannter Disposition an tts_disposition::tts_unset() wird ignoriert\n"

36 "Lesen von unbekannter Disposition durch tts_disposition::load() wird ignoriert\n"

37 "%s wurde aufgerufen\n"

38 "ToolTalk-Fehler: %s\n\
        Ursprung: Quellendatei %s, Zeile %d\n"

39 "Warnung: %sUrsprung: Quellendatei %s, Zeile %d\n"

40 " Tts_message_molding::send - Es wurde versucht, eine nicht initialisierte Meldungsformung zu senden.\n"

41 "Übergabe von unbekanntem Tt_scope an Tts_scope::add() wird ignoriert\n"

42 "Übergabe von unbekanntem Tt_scope an Tts_scope::remove() wird ignoriert\n"

43 "Übergabe von unbekanntem Bereich an tts_scope::tts_set() wird ignoriert\n"

44 "Übergabe von unbekanntem Bereich an tts_scope::tts_unset() wird ignoriert\n"

45 "Lesen von unbekanntem Bereich durch tts_scope::load() wird ignoriert\n"

46 "Übergabe von unbekanntem Tt_state an Tts_state::add() wird ignoriert\n"

47 "Übergabe von unbekanntem Tt_state an Tts_state::remove() wird ignoriert\n"

48 "Übergabe von unbekanntem Status an tts_state::tts_set() wird ignoriert\n"

49 "Übergabe von unbekanntem Status an tts_state::tts_unset() wird ignoriert\n"

50 "Tts_state::generate() hat eine Meldung angefordert - wird ignoriert\n"

51 "Lesen von unbekanntem Status durch tts_state::load() wird ignoriert\n"

82 "Hilfe zu TTSnoop aufrufen"

83 "-------------------------"

84 "Hilfe für einzelne Knöpfe und Einstellungen kann angefordert"

85 "werden, indem die Hilfetaste gedrückt wird, wobei sich die Maus"

86 "auf der Taste oder der interessierenden Einstellung befindet. Um zu sehen, welche api-Aufrufe von"

87 "ttsnoop verwendet werden, beim Aufrufen von ttsnoop den Schalter -t verwenden."

88 ""

89 "Arbeiten mit TTSnoop"

90 "--------------------"

91 "Zum Anzeigen von Meldungen die Einstellungsauswahl 'Start' auswählen."

92 ""

93 "Zum Ausschalten der Meldungen die Einstellungsauswahl 'Stop' auswählen."

94 ""

95 "Zum Einschränken der angezeigten Meldungen wie folgt vorgehen:"

96 "      1) den Knopf 'Suchmuster' auswählen"

97 "      2) den Suchmustertyp eingeben, der angezeigt werden soll"

98 "      3) den Knopf 'Anwenden' auswählen"

99 ""

100 "Zum Markieren von angezeigten Informationen wie folgt vorgehen:"

101 "      1) den Knopf 'Anzeige' auswählen"

102 "      2) die Einträge markieren, die hervorgehoben werden sollen"

103 "      3) den Knopf 'Anwenden' auswählen"

104 ""

105 "Zum Senden einer Meldung wie folgt vorgehen:"

106 "      1) den Knopf 'Meldung' auswählen"

107 "      2) die Meldung zusammenstellen"

108 "      3) den Knopf 'Meldung senden' auswählen"

109 ""

110 "Zum Speichern einer Meldung wie folgt vorgehen:"

111 "      1) den Knopf 'Meldung' auswählen"

112 "      2) die Meldung zusammenstellen"

113 "      3) den Knopf 'Meldung hinzufügen' auswählen"

114 ""

115 "Zum Senden einer gespeicherten Meldung wie folgt vorgehen:"

116 "      Die Meldung im Menü 'Meldung senden' auswählen"

117 ""

118 "Zum Löschen des Meldungsausgabefensters den Knopf 'Löschen' auswählen."

119 ""

120 "Allgemeine Informationen zu ToolTalk"

121 "------------------------------------"

122 "Bevor mit ToolTalk objektbezogene Meldungen gesendet und empfangen"

123 "werden, muß ein Prozeß mit der Meldungsübergabefunktion registriert"

124 "werden. Beim Registrieren gibt der Prozeß folgende Informationen"

125 "an ToolTalk:"

126 ""

127 "      Der Prozeßtyp (ptype) für den Prozeß."

128 "      Mit dieser Information kann die Meldungsübergabefunktion"

129 "      zu diesem ptype gehörige Meldungen zu diesem"

130 "      Prozeß leiten."

131 ""

132 "      Die Sessions, an denen der Prozeß teilnimmt."

133 ""

134 "      Die Dokumente, die der Prozeß überwacht."

135 "      In diesen Dokumenten enthaltene Meldungen"

136 "      an Objekte können dann zu diesem Prozeß"

137 "      geleitet werden."

138 ""

139 "Hierbei ist zu beachten, daß generierte Suchmuster"

140 "bei diesen Registrierungen generell geändert werden."

141 "Es ist möglich, die API-Aufrufe zur Suchmustermanipulation"

142 "für die Überwachung oder Verarbeitung objektorientierter Meldungen"

143 "zu verwenden. Die hier gegebenen Beschreibungen für Methoden sind jedoch viel"

144 "kürzer, da die ausführlichen Erläuterungen bereits in den Typendefinitionen"

145 "enthalten sind."

146 ""

147 "tt_session_join verwenden, um an einer Sitzung teilzunehmen."

148 "Hierfür ist die Session-ID der Session erforderlich,"

149 "an der der Benutzer teilnehmen möchte."

150 ""

151 "Wenn das Werkzeug die ToolTalk-Services nicht mehr benötigt,"

152 "die Session mit tt_session_quit verlassen."

153 ""

154 "Wenn das Werkzeug eine Datei lädt, muß es Bestandteil der"

155 "Sammlung der diese Datei anfordernden Prozesse werden, indem"

156 "tt_file_join() aufgerufen wird. Wenn die Datei nicht mehr benötigt"

157 "wird, die Prozeßsammlung durch Aufrufen von"

158 "tt_file_quit() verlassen. Einige Werkzeuge können mehrere Dateien"

159 "gleichzeitig öffnen. Bei anderen Werkzeugen ist nur das Öffnen von jeweils einer Datei möglich."

160 "Arbeiten mit TTSnoop"

161 "--------------------"

162 "Zum Anzeigen von Meldungen die Einstellungsauswahl 'Start' auswählen."

163 "Zum Ausschalten der Meldungen die Einstellungsauswahl 'Stop' auswählen."

164 ""

165 "Zum Einschränken der angezeigten Meldungen wie folgt vorgehen:"

166 "        1) den Knopf 'Suchmuster' auswählen"

167 "        2) den Suchmusterstyp angeben, der angezeigt werden soll"

168 "        3) den Knopf 'Anwenden' auswählen"

169 ""

170 "Zum Markieren von angezeigten Informationen wie folgt vorgehen:"

171 "        1) den Knopf 'Anzeige' auswählen"

172 "        2) die Einträge markieren, die hervorgehoben werden sollen"

173 "        3) den Knopf 'Anwenden' auswählen"

174 ""

175 "Zum Senden einer Meldung wie folgt vorgehen:"

176 "        1) den Knopf 'Meldungen' auswählen"

177 "        2) die Meldung zusammenstellen"

178 "        3) den Knopf 'Meldung senden' auswählen"

179 ""

180 "Zum Speichern einer Meldung wie folgt vorgehen:"

181 "        1) den Knopf 'Meldungen' auswählen"

182 "        2) die Meldung zusammenstellen"

183 "        3) den Knopf 'Meldung hinzufügen' auswählen"

184 ""

185 "Zum Senden einer gespeicherten Meldung wie folgt vorgehen:"

186 "        Die Meldung im Menü 'Meldung senden' auswählen."

187 ""

188 "Zum Löschen des Meldungsausgabefensters den Knopf 'Löschen' auswählen."

189 ""

190 "Allgemeine Informationen zu ToolTalk"

191 "------------------------------------"

192 "Bevor mit ToolTalk objektbezogene Meldungen gesendet und empfangen"

193 "werden, muß ein Prozeß mit der Meldungsübergabefunktion registriert"

194 "werden. Beim Registrieren gibt der Prozeß folgende Informationen an ToolTalk:"

195 ""

196 "      Der Prozeßtyp (ptype) für den Prozeß."

197 "      Mit dieser Information kann die Meldungsübergabefunktion"

198 "      zu diesem ptype gehörige Meldungen zu diesem Prozeß leiten."

199 ""

200 "      Die Sessions, an denen der Prozeß teilnimmt."

201 ""

202 "      Die Dokumente, die der Prozeß überwacht. In diesen Dokumenten"

203 "      enthaltene Meldungen an Objekte können dann zu diesem"

204 "      Prozeß geleitet werden."

205 ""

206 "Hierbei ist zu beachten, daß generierte Suchmuster bei diesen Registrierungen"

207 "generell geändert werden. Es ist möglich, die API-Aufrufe zur Suchmustermanipulation"

208 "für die Überwachung oder Verarbeitung objektorientierter Meldungen zu verwenden."

209 "Die hier gegebenen Beschreibungen für Methoden sind jedoch viel kürzer, da die ausführlichen"

210 "Erläuterungen bereits in den Typendefinitionen enthalten sind."

211 ""

212 "tt_session_join verwenden, um an einer Sitzung teilzunehmen. Hierfür ist die"

213 "Session-ID der Session erforderlich, an der der Benutzer teilnehmen möchte."

214 ""

215 "Wenn das Werkzeug die ToolTalk-Services nicht mehr benötigt,"

216 "die Session mit tt_session_quit verlassen."

217 ""

218 "Wenn das Werkzeug eine Datei lädt, muß es Bestandteil der Sammlung der"

219 "diese Datei anfordernden Prozesse werden, indem tt_file_join() aufgerufen"

220 "wird. Wenn die Datei nicht mehr benötigt wird, die Prozeßsammlung durch"

221 "Aufrufen von tt_file_quit() verlassen. Einige Werkzeuge können mehrere"

222 "Dateien gleichzeitig öffnen. Bei anderen Werkzeugen ist nur das Öffnen von jeweils einer Datei möglich."

223 "Das Empfangen  und Anzeigen von Meldungen starten/stoppen."

224 "Das Meldungsfenster löschen."

225 "Anzeige"

226 "-------"

227 "Wenn für den Prozeß eine Meldung eingeht, wird ein Datei-Deskriptor"

228 "aktiv. Je nach Struktur des Werkzeugs können die Werkzeuge von Window"

229 "System einen Funktionsaufruf anfordern, wenn der Datei-Deskriptor aktiv ist,"

230 "oder den Datei-Deskriptor in einen select(3)-Aufruf einschließen, mit"

231 "dem eine Reihe von Datei-Deskriptoren gesperrt wird."

232 "In beiden Fällen wird nach Aktivwerden des Datei-Deskriptors"

233 "tt_message_receive() aufgerufen, um eine Griffläche für die Meldung anzufordern, die die"

234 "eingehende Meldung enthält. Die Aufrufe tt_message_*() können zur Überprüfung"

235 "der Meldungsattribute verwendet werden, um festzustellen, welche Aktion erforderlich ist."

236 "Antworten auf gesendete Meldungen können durch Vergleich der Grifflächen"

237 "festgestellt werden. Dazu werden für die Anwendung wichtige Informationen in die"

238 "Meldung mit dem Aufruf tt_message_user_set() oder bestimmte Funktionsaufrufe in Meldungen"

239 "und Suchmustern mit den Aufrufen"

240 "tt_message_callback_add und tt_pattern_callback_add gestellt."

241 "Meldungen"

242 "---------"

243 "Zum Senden einer Meldung zunächst eine neue Meldung zuordnen, diese mit den erforderlichen"

244 "Informationen ausfüllen und sie anschließend senden. Alle Antworten werden in derselben Meldung angezeigt."

245 "Wenn die Meldung nicht mehr benötigt wird, den Speicher freimachen (für"

246 "angeforderte Ausgabewerte, nachdem alle erforderlichen Ausgabewerte"

247 "herauskopiert wurden; bei Hinweisen ist dies unmittelbar nach dem Senden der Fall)."

248 ""

249 "Zum Zuordnen der neuen Meldung tt_message_create() verwenden. Damit wird für die Meldung"

250 "eine \"Griffläche\" oder ein \"verdeckter Zeiger\" ausgegeben. Mit dieser Griffläche kann"

251 "in späteren Aufrufen auf diese Meldung verwiesen werden."

252 ""

253 "Zum Angeben der Meldungsinformationen die Aufrufe tt_message_*_set() verwenden."

254 "Für jedes Meldungsattribut ist ein spezieller Aufruf verfügbar."

255 ""

256 "Es müssen Klasse, Adresse, Operation und Argurmente gesetzt werden."

257 ""

258 "Die Meldung mit tt_message_send() senden."

259 "Suchmuster"

260 "----------"

261 "Da Meldungen vom Sendeprozeß nicht explizit an einen bestimmten Empfänger gerichtet werden,"

262 "wird der Empfänger mit einem \"Suchmusterabgleich\" bestimmt. Die Werkzeuge"

263 "registrieren Beschreibungen der Meldungen, an denen sie interessiert sind,"

264 "und ToolTalk benutzt diese Beschreibungen, um die Meldung weiterzuleiten."

265 "Es gibt verschiede Arten von Suchmustern zur Beschreibung von Meldungen,"

266 "die vom Werkzeug verarbeitet und überwacht werden sollen."

267 "Werkzeuge, die Meldungen empfangen wollen, erklären ihr Interesse daran"

268 "entweder dynamisch innerhalb der Laufzeit oder statisch während der Installation. Eine dynamische"

269 "Registrierung besteht aus einer Reihe von Suchmustern, mit denen alle Meldungen"

270 "verglichen werden. Wenn eine Meldung mit dem Suchmuster übereinstimmt, kann das Werkzeug, von dem das"

271 "Suchmuster registriert wurde, zum Empfangen der Meldung ausgewählt werden."

272 ""

273 "Bevor mit ToolTalk prozeßbezogene Meldungen empfangen werden können, muß mit der"

274 "Meldungsübergabefunktion ein Prozeß registriert werden. Beim Registrieren gibt der Prozeß"

275 "Suchmuster an ToolTalk weiter. ToolTalk gibt anschließend die dem Suchmuster entsprechenden Meldungen"

276 "an den Prozeß aus. Die Suchmuster können auf zwei Arten generiert werden:"

277 ""

278 "Statisch, durch einen ptype. Ein Prozeß kann der Meldungsübergabefunktion seinen"

279 "ptype mitteilen. ToolTalk generiert daraufhin Suchmusterter für jede Signatur"

280 "des ptype. Die so generierten Suchmuster können Herstellen einer Verbindung zu und Quittieren von"

281 "Sessions und Dateien geändert werden, wie bei der Übergabe von objektorientierten Meldungen."

282 ""

283 "Dynamisch. Ein Prozeß kann Suchmuster \"fortlaufend\" erstellen und sie mit"

284 "ToolTalk registrieren. Ein typisches Beispiel hierfür ist ein Dienstprogramm für"

285 "Meldungsprotokollierung, das alle durchlaufenden Meldungen durch Anzeige in einem Fenster"

286 "überwacht. Ein solches Dienstprogramm kann als Testhilfe für Meldungsprotokolle"

287 "hilfreich sein."

288 ""

289 "Diese Anwendung arbeitet nur mit dynamischen Suchmustern."

290 ""

291 "Zum Registrieren eines Suchmusters muß zunächst ein neues Suchmuster zugeordnet werden."

292 "Anschließend müssen die erforderlichen Informationen angegeben werden, worauf das Suchmuster registriert werden kann."

293 "Wenn das Suchmuster nicht mehr benötigt wird (d. h., wenn die dazu passende Meldung nicht mehr von Bedeutung ist),"

294 "sollte es aus dem Speicher entfernt werden."

295 ""

296 "Zum Zuordnen des neuen Suchmusters tt_pattern_create() verwenden. Damit wird eine"

297 "\"Griffläche\" oder ein \"verdeckter Zeiger\" für das Suchmuster ausgegeben. Mit dieser Griffläche kann"

298 "bei folgenden Aufrufen auf dieses Suchmuster verwiesen werden."

299 ""

300 "Zum Angeben der Suchmusterinformationen die Aufrufe tt_pattern_*_add() verwenden. Für"

301 "jedes Suchmusterattribut ist einer dieser Aufrufe verfügbar. Für jedes Attribut"

302 "Attribut im Suchmuster können mehrere Werte angegeben werden. Das Suchmusterattribut"

303 "stimmt mit einem Meldungsattribut überein, wenn einer der Werte im Suchmuster"

304 "mit dem Wert in der Meldung übereinstimmt."

305 ""

306 "Diese Anwendung arbeitet mit nicht ausschließlichen Einstellungen und kommagetrennten"

307 "Werten für die verschiedenen Attribute."

308 ""

309 "Zum Abgleichen von Meldungen ohne Berücksichtigung des Wertes in einem bestimmten Attribut"

310 "muß nur das Attribut des Suchmusters übergangen werden."

311 ""

312 "Wenn kein Suchmusterattribut angegeben wurde, wertet ToolTalk das Meldungsattribut als"

313 "übereinstimmend. Je weniger Suchmusterattribute angegeben werden, desto mehr Meldungen"

314 "können empfangen werden."

315 ""

316 "Wurden für ein Suchmusterattribut mehrere Werte angegeben, muß einer dieser Werte"

317 "mit dem Attributwert der Meldung übereinstimmen. Wenn keiner der Werte übereinstimmt,"

318 "kann die Anwendung von ToolTalk nicht mehr als Empfänger betrachtet werden."

319 ""

320 "Die Attribute 'Kategorie' und 'Bereich' müssen immer angegeben werden."

321 ""

322 "Wenn das Suchmuster vollständig ist, kann es mit tt_pattern_register() registriert werden."

323 "Anschließend nach Bedarf durch Auswahl des Knopfes 'Anwenden' eine Verbindung zu Sessions"

324 "oder Dateien herstellen."

325 ""

326 "Der Haupteffekt des Herstellens von Verbindungen zu Dateien und Sessions ist das Aktualisieren gerade"

327 "registrierter Suchmuster. Alle nach Herstellen der Verbindung registrierten Suchmuster"

328 "werden nicht aktualisiert. Es sollten entweder alle Suchmuster vor Herstellen einer Verbindung registriert"

329 "oder die Verbindungen nach Registrierung weiterer Suchmuster erneut hergestellt werden."

330 "Dies gilt ebenso für Suchmusters, die implizit durch Herstellen einer Verbindung zu ptypes registriert wurden."

331 "Wenn Meldungen über das Fenster 'Meldungen' hinzugefügt werden, wird das zu diesem"

332 "Knopf gehörende Menü größer. Eine Meldung kann durch Auswählen gesendet werden."

333 "Adresse:"

334 "Da es verschiedene Arten von Werkzeugen gibt und diverse Gruppen von Werkzeugen"

335 "zu verschiedenen Zeiten und von verschiedenen Benutzern verwendet werden, ist es für"

336 "den Sender einer Meldung oft nicht möglich, den Empfänger der Meldung genau zu identifizieren."

337 "Stattdessen gibt der Sender einen Operationsnamen an, aus dem die Bedeutung der Meldung hervorgeht,"

338 "und möglicherweise ein Objekt, an das die Meldung gerichtet ist, bzw. den"

339 "Objekttyp (otype)."

340 ""

341 "Für objektorientierte Meldungen Objekt oder otype verwenden. Wenn die Adresse ein Verarbeitungsprozeß"

342 "ist, diesen angeben."

343 ""

344 "        TT_PROCEDURE: Operation und Argumente angegeben."

345 ""

346 "        TT_OBJECT:    Objekt, Operation und Argumente angegeben."

347 ""

348 "        TT_HANDLER:   Verarbeitender Prozeß, Operation und Argumente angegeben."

349 ""

350 "        TT_OTYPE:     Otype, Operation und Argumente angegeben."

351 "Verarbeitender Prozeß:"

352 "Wenn die genaue Prozeßkennung des verarbeitenden Prozesses bekannt ist, kann die Meldung direkt"

353 "an diesen Prozeß gerichtet werden. Der übliche Weg hierfür ist, eine allgemeine Anfrage"

354 "für einen Prozeß zu stellen und der Antwort dann das Attribut des verarbeitenden Prozesses"

355 "zu entnehmen, worauf weitere Meldungen an den verarbeitenden Prozeß gerichtet werden können."

356 "Auf diese Weise können zwei Prozesse über die Meldungsweiterleitung im Rundsendebetrieb aufeinandertreffen"

357 "und in einen Dialog miteinander treten."

358 "Ptype des verarbeitenden Prozesses:"

359 "Wenn der ptype des Prozesses bekannt ist, von dem die Meldung verarbeitet wird, diesen angeben."

360 "Der ptype kann durch Konsultieren der Definition für den ptype, die auch Detailinformationen"

361 "zum Protokoll enthält, in Erfahrung gebracht werden."

362 "Objekt:"

363 "Die Objektkennung des Objektes angeben, an das die Meldung gesendet werden soll."

364 "Otype:"

365 "Objekttyp."

366 "Operation:"

367 "Die Operation angeben, die die Nachricht oder Anfrage beschreibt."

368 "Die Definition für den otype des Zielobjekts nachlesen, um den Namen"

369 "der Operation festzustellen."

370 "Opnum:"

371 "Operationsnummer. Diese wird zum Unterscheiden zwischen überlasteten Operationen"

372 "(Operationen mit demselben Namen, aber unterschielichen Argumententypen) und zur"

373 "Vereinfachung der \"internen\" Zuteilung eines Clients (Aufrufen"

374 "der korrekten internen Prozedur für eine gegebene Meldung) verwendet."

375 "Bereich:"

376 "Eine Bereichssitzung verwenden, wenn Meldungen von anderen Prozessen derselben"

377 "Session gewünscht werden. Eine Bereichsdatei verwenden, wenn Meldungen über eine Datei gewünscht"

378 "werden."

379 ""

380 "      TT_SESSION:          Empfangen von Meldungen von anderen Prozessen derselben"

381 "                           Session."

382 ""

383 "      TT_FILE:             Empfangen von Meldungen über die Datei, zu der eine Verbindung hergestellt wurde."

384 ""

385 "      TT_BOTH:             Empfangen von Meldungen über eine Datei und die"

386 "                           Session."

387 ""

388 "      TT_FILE_IN_SESSION:  Empfangen von Meldungen zur Datei, zu der während dieser"

389 "                           Session eine Verbindung hergestellt wurde."

390 "Session:"

391 "Für eine Bereichs-Session die Session (standardmäßig mit tt_default_session())"

392 "angeben."

393 "Datei:"

394 "Für einen Dateibereich den Namen der Datei angeben."

395 "Kategorie:"

396 "Mit Kategorieüberwachung arbeiten, wenn die Meldungen nur angezeigt werden sollen."

397 "Kategorienverarbeitung verwenden, um die Meldung von keinem anderen Prozeß verarbeitet werden soll."

398 ""

399 "    TT_OBSERVE: Überwachungsprozesse erhalten nur Meldungskopien zu ihrer"

400 "                Information. Eine Meldung kann von einer Reihe von Prozessen"

401 "                überwacht werden. Selbst wenn es sich bei der Meldung um eine Anforderung handelt,"

402 "                können die Überwachungseinrichtungen keine Werte an den Sender zurückgeben. Die von den"

403 "                Überwachungseinrichtungen ausgeführten Aktionen beeinflussen oft die"

404 "                interaktive Anzeige der zugrundeliegenden Werkzeugdaten."

405 ""

406 "    TT_HANDLE:  Die verarbeitenden Prozesse führen tatsächlich eine auf der Meldung basierende"

407 "                Aktion aus. Eine gegebene Meldung wird jeweils nur von einem Prozeß"

408 "                verarbeitet. Wenn es sich bei der Meldung um eine Anforderung handelt,"

409 "                ist der Prozeß, bei dem Werte ausgegeben werden, der verarbeitende Prozeß."

410 "                Die von einem verarbeitenden Prozeß ausgeführten Aktionen beeinflussen in der Regel"

411 "                die beständig gespeicherte Darstellung der Werkzeugdaten."

412 "Klasse:"

413 "Eine Klassenanfrage für Meldungen verwenden, die Werte zurückgeben, oder für die eine Rückmeldung"

414 "gewünscht wird, wenn die Meldung verarbeitet oder in eine Warteschlange gestellt wird"

415 "bzw. wenn ein Prozeß zur Verarbeitung der Anforderung gestartet wird. Eine Klassennachricht für"

416 "Meldungen verwenden, die andere Prozesse über Ereignisse informieren."

417 ""

418 "    TT_NOTICE:   Hinweise sind Meldungen, die andere Werkzeuge über das Eintreten eines"

419 "                 Ereignisses informieren. Werkzeuge, die Hinweise senden, erwarten keine"

420 "                 Rückantwort. Der Sender gibt den anderen Werkzeugen"

421 "                 lediglich aktuelle Informationen."

422 ""

423 "     TT_REQUEST: Anforderungen sind Meldungen, die ein anderes Werkzeug auffordern,"

424 "                 eine Aktion auszuführen. Das anfordernde Werkzeug erwartet in den meisten Fällen"

425 "                 die Ausgabe von Werten. Diese Werte werden als Antwort auf die Anforderung"

426 "                 ausgegeben. Auch wenn keine Werte ausgegeben werden, sendet das die Anforderung"

427 "                 verarbeitende Werkzeug eine Antwort, aus der hervorgeht, ob"

428 "                 die Anforderung erfolgreich war oder fehlgeschlagen ist."

429 "Disposition:"

430 "Gibt an, welche Aktion auszuführen ist, wenn die Meldung nicht von einem der laufenden"

431 "Prozesse verarbeitet werden kann. Aktion 'Warteschlange', wenn die Meldung in eine Warteschlange gestellt werden woll, bis sie von einem der Prozesse"

432 "mit dem ptype des verarbeitenden Prozesses registriert wird. Aktion 'Start',  wenn ein Prozeß mit dem ptype des verarbeitenden"

433 "Prozesses gestartet werden soll."

434 ""

435 "    TT_QUEUE: Stellen der Meldung in eine Warteschlange, bis ein Prozeß des richtigen Typs"

436 "              die Meldung empfangen kann."

437 ""

438 "    TT_START: Versuch, einen Prozeß mit dem korrekten ptype zu starten, sofern"

439 "              kein solcher läuft."

440 ""

441 "Hierbei ist zu beachten, daß die Werte für Tt_disposition addiert werden können, so daß"

442 "TT_QUEUE+TT_START sowohl bedeutet, daß die Meldung in eine Warteschlange gestellt wird, als auch,"

443 "daß ein Prozeß gestartet wird. Dies kann hilfreich sein, wenn der Start fehlschlägt (oder vom Benutzer"

444 "verhintert wird), um sicherzustellen, daß die Meldung verarbeitet wird, sobald ein Prozeß"

445 "erfolgreich startet."

446 "Status:"

447 "Werte für das Statusattribute einer Meldung. Folgende Werte und Bedeutungen"

448 "sind möglich:"

449 ""

450 "    TT_CREATED:  Die Meldung wurde erstellt, jedoch noch nicht gesendet. Nur der"

451 "                 Sender einer Meldung kann eine Meldung mit diesem Stauts sehen."

452 ""

453 "    TT_SENT:     Die Meldung wurde gesendet, jedoch noch nicht verarbeitet."

454 ""

455 "    TT_HANDLED:  Die Meldung wurde verarbeitet; die ausgegebenen Werte sind gültig."

456 ""

457 "    TT_FAILED:   Die Meldung konnte nicht an einen verarbeitenden Prozeß weitergeleitet werden."

458 ""

459 "    TT_QUEUED:   Die Meldung wurde zur späteren Weiterleitung in eine Warteschlange gestellt."

460 ""

461 "    TT_STARTED:  Es wird versucht, einen Prozeß zu starten, um die Meldung zu verarbeiten."

462 ""

463 "    TT_REJECTED: Die Meldung wurde von einem möglichen verarbeitenden Prozeß zurückgewiesen."

464 "                 Dieser Status kann nur vom zurückweisenden Prozeß eingesehen werden."

465 "                 ToolTalk ändert den Status in TT_SENT, bevor die Meldung an einen"

466 "                 anderen möglichen verarbeitenden Prozeß weitergeleitet wird."

467 "Status:"

468 "Eine eindeutige Zahl, die die Ergebnisse der Antwortmeldung angibt."

469 "Diese Uahl muß größer als TT_ERR_LAST (2047) sein."

470 "Statuszeichenfolge:"

471 "Eine Textbeschreibung des Meldungsstatus."

472 "Sender:"

473 "Kennzeichnet den Prozeß, der die Meldung gesendet hat."

474 "Ptype des Senders:"

475 "Der ptype des Prozesses, der die Meldung gesendet hat."

476 "UID:"

477 "Die effektive Benutzerkennung des Prozesses, der die Meldung gesendet hat."

478 "GID:"

479 "Die effektive Gruppenkennung des Prozesses, der die Meldung gesendet hat."

480 "Modus:"

481 "Gibt den Modus eines Meldungsarguments an. Folgende Werte und Bedeutungen"

482 "sind möglich:"

483 ""

484 "    TT_IN:    Das Argument wird vom Sender geschrieben und vom verarbeitenden"

485 "              Prozeß sowie möglichen Überwachungseinrichtungen gelesen."

486 ""

487 "    TT_OUT:   Das Argument wird vom verarbeitenden Prozeß geschrieben und vom Sender"

488 "              möglichen antwortenden Überwachungseinrichtungen gelesen."

489 ""

490 "    TT_INOUT: Das Argument wird vom Sender, dem verarbeitenden Prozeß sowie"

491 "              möglichen Überwachungseinrichtungen geschrieben und gelesen."

492 "Typ:"

493 "Den Typ kennzeichnende Zeichenfolge. Übliche Werte sind \"string\" und \"int\","

494 "obwohl auch eigene entwickelt werden können."

495 "Wert:"

496 "Tatsächliche Daten des Wertes in Abhängigkeit vom Typ."

497 "Wenn der Knopf 'Anwenden' gedrückt wird, pressed, werden die Werte der angegebenen"

498 "Einstellungen für jede empfangene Meldung angezeigt."

499 "Nach erneutem Drücken werden die angezeigten Werte auf den letzten Status"

500 "oder wenn dieser später aufgetreten ist, den Anfangswert zurückgesetzt."

501 "Editieren des Empfangskontexts:"

502 "Kontext, der empfangen werden soll, hinzufügen, löschen oder ändern."

503 "Suchmustervergleichsfunktion"

504 "----------------------------"

505 "Da Meldungen vom sendenden Prozeß nicht ausdrücklich an einen bestimmten Empfänger"

506 "gesendet werden, wird der Empfänger durch einen \"Suchmustervergleich\" bestimmt."

507 "Die Werkzeuge registrieren Beschreibungen der sie interessierenden Meldungen, und"

508 "ToolTalk nutzt diese Beschreibungen, um die Meldung weiterzuleiten."

509 "Zur Beschreibung der Meldungen, die das Werkzeug verarbeiten oder überwachen will,"

510 "gibt es verschiedene Gruppen von Suchmustern. Werkzeuge, die Meldungen"

511 "empfangen wollen, geben ihr Interesse entweder dynamisch während der Laufzeit"

512 "oder statisch während der Installation an. Eine dynamische Registrierung"

513 "besteht aus einer Reihe von Suchmustern, mit denen alle Meldungen"

514 "verglichen werden. Wenn eine Meldung mit dem"

515 "Suchmuster übereinstimmt, kann das Suchmuster zum Empfangen der Meldung"

516 "ausgewählt werden."

517 ""

518 "Bevor mit ToolTalk prozeßbezogene Meldungen empfangen werden können, muß mit der"

519 "Meldungsübergabefunktion ein Prozeß registriert werden. Beim Registrieren gibt der Prozeß"

520 "Suchmuster an ToolTalk weiter. ToolTalk gibt anschließend die dem Suchmuster entsprechenden Meldungen"

521 "an den Prozeß aus. Die Suchmuster können auf zwei Arten generiert werden:"

522 ""

523 "Statisch, durch einen ptype. Ein Prozeß kann der Meldungsübergabefunktion seinen"

524 "ptype mitteilen. ToolTalk generiert daraufhin Suchmusterter für jede Signatur"

525 "des ptype. Die so generierten Suchmuster können Herstellen einer Verbindung zu und Quittieren von"

526 "Sessions und Dateien geändert werden, wie bei der Übergabe von objektorientierten Meldungen."

527 ""

528 "Dynamisch. Ein Prozeß kann Suchmuster \"fortlaufend\" erstellen und sie mit"

529 "ToolTalk registrieren. Ein typisches Beispiel hierfür ist ein Dienstprogramm für"

530 "Meldungsprotokollierung, das alle durchlaufenden Meldungen durch Anzeige in einem Fenster"

531 "überwacht. Ein solches Dienstprogramm kann als Testhilfe für Meldungsprotokolle"

532 "hilfreich sein."

533 ""

534 "Diese Anwendung arbeitet nur mit dynamischen Suchmustern."

535 ""

536 "Zum Registrieren eines Suchmusters muß zunächst ein neues Suchmuster zugeordnet werden."

537 "Anschließend müssen die erforderlichen Informationen angegeben werden, worauf das Suchmuster registriert werden kann."

538 "Wenn das Suchmuster nicht mehr benötigt wird (d. h., wenn die dazu passende Meldung nicht mehr von Bedeutung ist),"

539 "sollte es aus dem Speicher entfernt werden."

540 ""

541 "Zum Zuordnen des neuen Suchmusters tt_pattern_create() verwenden. Damit wird eine"

542 "\"Griffläche\" oder ein \"verdeckter Zeiger\" für das Suchmuster ausgegeben. Mit dieser Griffläche kann"

543 "bei folgenden Aufrufen auf dieses Suchmuster verwiesen werden."

544 ""

545 "Zum Angeben der Suchmusterinformationen die Aufrufe tt_pattern_*_add() verwenden. Für"

546 "jedes Suchmusterattribut ist einer dieser Aufrufe verfügbar. Für jedes Attribut"

547 "Attribut im Suchmuster können mehrere Werte angegeben werden. Das Suchmusterattribut"

548 "stimmt mit einem Meldungsattribut überein, wenn einer der Werte im Suchmuster"

549 "mit dem Wert in der Meldung übereinstimmt."

550 ""

551 "Diese Anwendung arbeitet mit nicht ausschließlichen Einstellungen und kommagetrennten"

552 "Werten für die verschiedenen Attribute."

553 ""

554 "Zum Abgleichen von Meldungen ohne Berücksichtigung des Wertes in einem bestimmten Attribut"

555 "muß nur das Attribut des Suchmusters übergangen werden."

556 ""

557 "Wenn kein Suchmusterattribut angegeben wurde, wertet ToolTalk das Meldungsattribut als"

558 "übereinstimmend. Je weniger Suchmusterattribute angegeben werden, desto mehr Meldungen"

559 "können empfangen werden."

560 ""

561 "Wurden für ein Suchmusterattribut mehrere Werte angegeben, muß einer dieser Werte"

562 "mit dem Attributwert der Meldung übereinstimmen. Wenn keiner der Werte übereinstimmt,"

563 "kann die Anwendung von ToolTalk nicht als Empfänger betrachtet werden."

564 ""

565 "Die Attribute 'Kategorie' und 'Bereich' müssen immer angegeben werden."

566 ""

567 "Wenn das Suchmuster vollständig ist, kann es mit tt_pattern_register() registriert werden."

568 "Anschließend nach Bedarf durch Auswahl des Knopfes 'Anwenden' eine Verbindung zu Sessions"

569 "oder Dateien herstellen."

570 ""

571 "Der Haupteffekt des Herstellens von Verbindungen zu Dateien und Sessions ist das Aktualisieren gerade registrierter Suchmuster."

572 "Alle nach Herstellen der Verbindung registrierten Suchmuster werden nicht aktualisiert. Es sollten entweder alle Suchmuster vor"

573 "Herstellen einer Verbindung registriert oder die Verbindungen nach Registrierung weiterer Suchmuster erneut hergestellt werden."

574 "Dies gilt ebenso für Suchmusters, die implizit durch Herstellen einer Verbindung zu ptypes registriert wurden."

575 "Wenn Meldungen über das Fenster 'Meldungen' hinzugefügt werden, wird das zu diesem"

576 "Knopf gehörende Menü größer. Eine Meldung kann durch Auswählen gesendet werden."

577 "Adresse:"

578 "Da es verschiedene Arten von Werkzeugen gibt und diverse Gruppen von Werkzeugen"

579 "zu verschiedenen Zeiten und von verschiedenen Benutzern verwendet werden, ist es für"

580 "den Sender einer Meldung oft nicht möglich, den Empfänger der Meldung genau zu identifizieren."

581 "Stattdessen gibt der Sender einen Operationsnamen an, aus dem die Bedeutung der Meldung hervorgeht,"

582 "und möglicherweise ein Objekt, an das die Meldung gerichtet ist, bzw. den"

583 "Objekttyp (otype)."

584 ""

585 "Für objektorientierte Meldungen Objekt oder otype verwenden. Wenn die Adresse ein Verarbeitungsprozeß"

586 "ist, diesen angeben."

587 ""

588 "        TT_PROCEDURE: Operation und Argumente angegeben."

589 ""

590 "        TT_OBJECT:    Objekt, Operation und Argumente angegeben."

591 ""

592 "        TT_HANDLER:   Verarbeitender Prozeß, Operation und Argumente angegeben."

593 ""

594 "        TT_OTYPE:     Otype, Operation und Argumente angegeben."

595 "Objekt:"

596 "Die Objektkennung des Objektes angeben, an das die Meldung gesendet werden soll."

597 "Otype:"

598 "Objekttyp."

599 "Operation:"

600 "Die Operation angeben, die die Nachricht oder Anfrage beschreibt."

601 "Die Definition für den otype des Zielobjekts nachlesen, um den Namen"

602 "der Operation festzustellen."

603 "Bereich:"

604 "Eine Bereichssitzung verwenden, wenn Meldungen von anderen Prozessen derselben"

605 "Session gewünscht werden. Eine Bereichsdatei verwenden, wenn Meldungen über eine Datei gewünscht"

606 "werden."

607 ""

608 "      TT_SESSION:          Empfangen von Meldungen von anderen Prozessen derselben"

609 "                           Session."

610 ""

611 "      TT_FILE:             Empfangen von Meldungen über die Datei, zu der eine Verbindung hergestellt wurde."

612 ""

613 "      TT_BOTH:             Empfangen von Meldungen über eine Datei und die Session."

614 ""

615 "      TT_FILE_IN_SESSION:  Empfangen von Meldungen zur Datei, zu der während dieser"

616 "                           Session eine Verbindung hergestellt wurde."

617 "Datei:"

618 "Für einen Dateibereich den Namen der Datei angeben."

619 "Session:"

620 "Für eine Bereichs-Session, die Session (standardmäßig durch tt_default_session())"

621 "angeben."

622 "Kategorie:"

623 "Mit Kategorieüberwachung arbeiten, wenn die Meldungen nur angezeigt werden sollen."

624 "Kategorienverarbeitung verwenden, um die Meldung von keinem anderen Prozeß verarbeitet werden soll."

625 ""

626 "    TT_OBSERVE: Überwachungsprozesse erhalten nur Meldungskopien zu ihrer"

627 "                Information. Eine Meldung kann von einer Reihe von Prozessen"

628 "                überwacht werden. Selbst wenn es sich bei der Meldung um eine Anforderung handelt,"

629 "                können die Überwachungseinrichtungen keine Werte an den Sender zurückgeben. Die von den"

630 "                Überwachungseinrichtungen ausgeführten Aktionen beeinflussen oft die"

631 "                interaktive Anzeige der zugrundeliegenden Werkzeugdaten."

632 ""

633 "    TT_HANDLE:  Die verarbeitenden Prozesse führen tatsächlich eine auf der Meldung basierende"

634 "                Aktion aus. Eine gegebene Meldung wird jeweils nur von einem Prozeß"

635 "                verarbeitet. Wenn es sich bei der Meldung um eine Anforderung handelt,"

636 "                ist der Prozeß, bei dem Werte ausgegeben werden, der verarbeitende Prozeß."

637 "                Die von einem verarbeitenden Prozeß ausgeführten Aktionen beeinflussen in der Regel"

638 "                die beständig gespeicherte Darstellung der Werkzeugdaten."

639 "Klasse:"

640 "Eine Klassenanfrage für Meldungen verwenden, die Werte zurückgeben, oder für die eine Rückmeldung"

641 "gewünscht wird, wenn die Meldung verarbeitet oder in eine Warteschlange gestellt wird"

642 "bzw. wenn ein Prozeß zur Verarbeitung der Anforderung gestartet wird. Eine Klassennachricht für"

643 "Meldungen verwenden, die andere Prozesse über Ereignisse informieren."

644 ""

645 "    TT_NOTICE:   Hinweise sind Meldungen, die andere Werkzeuge über das Eintreten eines"

646 "                 Ereignisses informieren. Werkzeuge, die Hinweise senden, erwarten keine"

647 "                 Rückantwort. Der Sender gibt den anderen Werkzeugen"

648 "                 lediglich aktuelle Informationen."

649 ""

650 "     TT_REQUEST: Anforderungen sind Meldungen, die ein anderes Werkzeug auffordern,"

651 "                 eine Aktion auszuführen. Das anfordernde Werkzeug erwartet in den meisten Fällen"

652 "                 die Ausgabe von Werten. Diese Werte werden als Antwort auf die Anforderung"

653 "                 ausgegeben. Auch wenn keine Werte ausgegeben werden, sendet das die Anforderung"

654 "                 verarbeitende Werkzeug eine Antwort, aus der hervorgeht, ob"

655 "                 die Anforderung erfolgreich war oder fehlgeschlagen ist."

656 "Status:"

657 "Werte für das Statusattribute einer Meldung. Folgende Werte und Bedeutungen"

658 "sind möglich:"

659 ""

660 "    TT_CREATED:  Die Meldung wurde erstellt, jedoch noch nicht gesendet. Nur der"

661 "                 Sender einer Meldung kann eine Meldung mit diesem Stauts sehen."

662 ""

663 "    TT_SENT:     Die Meldung wurde gesendet, jedoch noch nicht verarbeitet."

664 ""

665 "    TT_HANDLED:  Die Meldung wurde verarbeitet; die ausgegebenen Werte sind gültig."

666 ""

667 "    TT_FAILED:   Die Meldung konnte nicht an einen verarbeitenden Prozeß weitergeleitet werden."

668 ""

669 "    TT_QUEUED:   Die Meldung wurde zur späteren Weiterleitung in eine Warteschlange gestellt."

670 ""

671 "    TT_STARTED:  Es wird versucht, einen Prozeß zu starten, um die Meldung zu verarbeiten."

672 ""

673 "    TT_REJECTED: Die Meldung wurde von einem möglichen verarbeitenden Prozeß zurückgewiesen."

674 "                 Dieser Status kann nur vom zurückweisenden Prozeß eingesehen werden."

675 "                 ToolTalk ändert den Status in TT_SENT, bevor die Meldung an einen"

676 "                 anderen möglichen verarbeitenden Prozeß weitergeleitet wird."

677 "Disposition:"

678 "Gibt an, welche Aktion auszuführen ist, wenn die Meldung nicht von einem der laufenden"

679 "Prozesse verarbeitet werden kann. Aktion 'Warteschlange', wenn die Meldung in eine Warteschlange gestellt werden woll, bis sie von einem der Prozesse"

680 "mit dem ptype des verarbeitenden Prozesses registriert wird. Aktion 'Start',  wenn ein Prozeß mit dem ptype des verarbeitenden"

681 "Prozesses gestartet werden soll."

682 ""

683 "    TT_QUEUE: Stellen der Meldung in eine Warteschlange, bis ein Prozeß des richtigen Typs"

684 "              die Meldung empfangen kann."

685 ""

686 "    TT_START: Versuch, einen Prozeß mit dem korrekten ptype zu starten, sofern"

687 "              kein solcher läuft."

688 ""

689 "Hierbei ist zu beachten, daß die Werte für Tt_disposition addiert werden können, so daß"

690 "TT_QUEUE+TT_START sowohl bedeutet, daß die Meldung in eine Warteschlange gestellt wird, als auch,"

691 "daß ein Prozeß gestartet wird. Dies kann hilfreich sein, wenn der Start fehlschlägt (oder vom Benutzer"

692 "verhintert wird), um sicherzustellen, daß die Meldung verarbeitet wird, sobald ein Prozeß"

693 "erfolgreich startet."

694 "Sender:"

695 "Kennzeichnet den Prozeß, der die Meldung gesendet hat."

696 "Ptype des Senders:"

697 "Der ptype des Prozesses, der die Meldung gesendet hat."

698 "Argumente:"

699 "Alle Argumente für die Operation angeben. tt_message_arg_add verwenden,"

700 "um die einzelnen Argumente hinzuzufügen. Für jedes Argument muß folgendes angegeben werden:"

701 "Modus (Eingabe, Ausgabe oder Ein-/Ausgabe), Typ und den Wert, sofern der Modus Eingabe oder Ein-/Ausgabe ist."

702 "Modus:"

703 "Gibt den Modus eines Meldungsarguments an. Folgende Werte und Bedeutungen"

704 "sind möglich:"

705 ""

706 "    TT_IN:    Das Argument wird vom Sender geschrieben und vom verarbeitenden"

707 "              Prozeß sowie möglichen Überwachungseinrichtungen gelesen."

708 ""

709 "    TT_OUT:   Das Argument wird vom verarbeitenden Prozeß geschrieben und vom Sender"

710 "              möglichen antwortenden Überwachungseinrichtungen gelesen."

711 ""

712 "    TT_INOUT: Das Argument wird vom Sender, dem verarbeitenden Prozeß sowie"

713 "              möglichen Überwachungseinrichtungen geschrieben und gelesen."

714 "Typ:"

715 "Den Typ kennzeichnende Zeichenfolge. Übliche Werte sind \"string\" und \"int\","

716 "obwohl auch eigene entwickelt werden können."

717 "Wert:"

718 "Tatsächliche Daten des Wertes in Abhängigkeit vom Typ."

719 "Knopf 'Hinzufügen':"

720 "Hinzufügen eines Arguments mit Modus, Typ und Wert."

721 "Knopf 'Löschen':"

722 "Löschen des ausgewählten Arguments."

723 "Knopf 'Ändern':"

724 "Die Werte für das ausgewählte Argument in die aktuellen Werte für Modus, Typ und"

725 "Wert ändern."

726 "Knopf 'Anwenden':"

727 "Registrieren, um die diesem Suchmuster entsprechenden Meldungen zu empfangen."

728 "Name:"

729 "Name dieses Kontexts"

730 "Wert:"

731 "Ein dem Typ entsprechender Wert,"

732 ""

733 "z. B. 55 für den Typ 'int', \"fred\" für den Typ 'char' oder ^V^L^X für"

734 "den Typ 'bytes'."

735 "Kontext hinzufügen:"

736 "Die aktuellen Werte der Fehler 'Name' und 'Wert' in die Kontextliste"

737 "eingeben."

738 "Kontext löschen:"

739 "Den ausgewählten Eintrag aus der Kontextliste löschen."

740 "Kontext ändern:"

741 "Für den ausgewählten Eintrag die Werte der Felder 'Name' und 'Wert' einsetzen."

742 "Sendekontext editieren:"

743 "Dialogfenster zum Editieren der Kontextliste, die gesendet werden soll."

744 "Meldungen"

745 "---------"

746 "Zum Senden einer Meldung zunächst eine neue Meldung zuordnen, diese mit den erforderlichen"

747 "Informationen ausfüllen und sie anschließend senden. Alle Antworten werden in derselben Meldung angezeigt."

748 "Wenn die Meldung nicht mehr benötigt wird, den Speicher freimachen (für"

749 "angeforderte Ausgabewerte, nachdem alle erforderlichen Ausgabewerte"

750 "herauskopiert wurden; bei Hinweisen ist dies unmittelbar nach dem Senden der Fall)."

751 ""

752 "Zum Zuordnen der neuen Meldung tt_message_create() verwenden. Damit wird für die Meldung"

753 "eine \"Griffläche\" oder ein \"verdeckter Zeiger\" ausgegeben. Mit dieser Griffläche kann"

754 "in späteren Aufrufen auf diese Meldung verwiesen werden."

755 ""

756 "Zum Angeben der Meldungsinformationen die Aufrufe tt_message_*_set() verwenden."

757 "Für jedes Meldungsattribut ist ein spezieller Aufruf verfügbar."

758 ""

759 "Es müssen Klasse, Adresse, Operation und Argurmente gesetzt werden."

760 ""

761 "Die Meldung mit tt_message_send() senden."

762 "Adresse:"

763 "Da es verschiedene Arten von Werkzeugen gibt und diverse Gruppen von Werkzeugen"

764 "zu verschiedenen Zeiten und von verschiedenen Benutzern verwendet werden, ist es für"

765 "den Sender einer Meldung oft nicht möglich, den Empfänger der Meldung genau zu identifizieren."

766 "Stattdessen gibt der Sender einen Operationsnamen an, aus dem die Bedeutung der Meldung hervorgeht,"

767 "und möglicherweise ein Objekt, an das die Meldung gerichtet ist, bzw. den"

768 "Objekttyp (otype)."

769 ""

770 "Für objektorientierte Meldungen Objekt oder otype verwenden. Wenn die Adresse ein Verarbeitungsprozeß"

771 "ist, diesen angeben."

772 ""

773 "        TT_PROCEDURE: Operation und Argumente angegeben."

774 ""

775 "        TT_OBJECT:    Objekt, Operation und Argumente angegeben."

776 ""

777 "        TT_HANDLER:   Verarbeitender Prozeß, Operation und Argumente angegeben."

778 ""

779 "        TT_OTYPE:     Otype, Operation und Argumente angegeben."

780 "Verarbeitender Prozeß:"

781 "Wenn die genaue Prozeßkennung des verarbeitenden Prozesses bekannt ist, kann die Meldung direkt"

782 "an diesen Prozeß gerichtet werden. Der übliche Weg hierfür ist, eine allgemeine Anfrage"

783 "für einen Prozeß zu stellen und der Antwort dann das Attribut des verarbeitenden Prozesses"

784 "zu entnehmen, worauf weitere Meldungen an den verarbeitenden Prozeß gerichtet werden können."

785 "Auf diese Weise können zwei Prozesse über die Meldungsweiterleitung im Rundsendebetrieb aufeinandertreffen"

786 "und in einen Dialog miteinander treten."

787 "Ptype des verarbeitenden Prozesses:"

788 "Wenn der ptype des Prozesses bekannt ist, von dem die Meldung verarbeitet wird, diesen angeben."

789 "Der ptype kann durch Konsultieren der Definition für den ptype, die auch Detailinformationen"

790 "zum Protokoll enthält, in Erfahrung gebracht werden."

791 "Objekt:"

792 "Die Objektkennung des Objektes angeben, an das die Meldung gesendet werden soll."

793 "Otype:"

794 "Objekttyp."

795 "Operation:"

796 "Die Operation angeben, die die Nachricht oder Anfrage beschreibt."

797 "Die Definition für den otype des Zielobjekts nachlesen, um den Namen"

798 "der Operation festzustellen."

799 "Bereich:"

800 "Eine Bereichssitzung verwenden, wenn Meldungen von anderen Prozessen derselben"

801 "Session gewünscht werden. Eine Bereichsdatei verwenden, wenn Meldungen über eine Datei gewünscht"

802 "werden."

803 ""

804 "      TT_SESSION:          Empfangen von Meldungen von anderen Prozessen derselben"

805 "                           Session."

806 ""

807 "      TT_FILE:             Empfangen von Meldungen über die Datei, zu der eine Verbindung hergestellt wurde."

808 ""

809 "      TT_BOTH:             Empfangen von Meldungen über eine Datei und die Session."

810 ""

811 "      TT_FILE_IN_SESSION:  Empfangen von Meldungen zur Datei, zu der während dieser"

812 "                           Session eine Verbindung hergestellt wurde."

813 "Session:"

814 "Für eine Bereichs-Session die Session (standardmäßig mit tt_default_session())"

815 "angeben."

816 "Datei:"

817 "Für einen Dateibereich den Namen der Datei angeben."

818 "Klasse:"

819 "Eine Klassenanfrage für Meldungen verwenden, die Werte zurückgeben, oder für die eine Rückmeldung"

820 "gewünscht wird, wenn die Meldung verarbeitet oder in eine Warteschlange gestellt wird"

821 "bzw. wenn ein Prozeß zur Verarbeitung der Anforderung gestartet wird. Eine Klassennachricht für"

822 "Meldungen verwenden, die andere Prozesse über Ereignisse informieren."

823 ""

824 "    TT_NOTICE:   Hinweise sind Meldungen, die andere Werkzeuge über das Eintreten eines"

825 "                 Ereignisses informieren. Werkzeuge, die Hinweise senden, erwarten keine"

826 "                 Rückantwort. Der Sender gibt den anderen Werkzeugen"

827 "                 lediglich aktuelle Informationen."

828 ""

829 "     TT_REQUEST: Anforderungen sind Meldungen, die ein anderes Werkzeug auffordern,"

830 "                 eine Aktion auszuführen. Das anfordernde Werkzeug erwartet in den meisten Fällen"

831 "                 die Ausgabe von Werten. Diese Werte werden als Antwort auf die Anforderung"

832 "                 ausgegeben. Auch wenn keine Werte ausgegeben werden, sendet das die Anforderung"

833 "                 verarbeitende Werkzeug eine Antwort, aus der hervorgeht, ob"

834 "                 die Anforderung erfolgreich war oder fehlgeschlagen ist."

835 "Disposition:"

836 "Gibt an, welche Aktion auszuführen ist, wenn die Meldung nicht von einem der laufenden"

837 "Prozesse verarbeitet werden kann. Aktion 'Warteschlange', wenn die Meldung in eine Warteschlange gestellt werden woll, bis sie von einem der Prozesse"

838 "mit dem ptype des verarbeitenden Prozesses registriert wird. Aktion 'Start',  wenn ein Prozeß mit dem ptype des verarbeitenden"

839 "Prozesses gestartet werden soll."

840 ""

841 "    TT_QUEUE: Stellen der Meldung in eine Warteschlange, bis ein Prozeß des richtigen Typs"

842 "              die Meldung empfangen kann."

843 ""

844 "    TT_START: Versuch, einen Prozeß mit dem korrekten ptype zu starten, sofern"

845 "              kein solcher läuft."

846 ""

847 "Hierbei ist zu beachten, daß die Werte für Tt_disposition addiert werden können, so daß"

848 "TT_QUEUE+TT_START sowohl bedeutet, daß die Meldung in eine Warteschlange gestellt wird, als auch,"

849 "daß ein Prozeß gestartet wird. Dies kann hilfreich sein, wenn der Start fehlschlägt (oder vom Benutzer"

850 "verhintert wird), um sicherzustellen, daß die Meldung verarbeitet wird, sobald ein Prozeß"

851 "erfolgreich startet."

852 "Ptype des Senders:"

853 "Der ptype des Prozesses, der die Meldung gesendet hat."

854 "Status:"

855 "Eine eindeutige Zahl, die die Ergebnisse der Antwortmeldung angibt."

856 "Diese Uahl muß größer als TT_ERR_LAST (2047) sein."

857 "Statuszeichenfolge:"

858 "Eine Textbeschreibung des Meldungsstatus."

859 "Argumente:"

860 "Alle Argumente für die Operation angeben. tt_message_arg_add verwenden,"

861 "um die einzelnen Argumente hinzuzufügen. Für jedes Argument muß folgendes angegeben werden:"

862 "Modus (Eingabe, Ausgabe oder Ein-/Ausgabe), Typ und den Wert, sofern der Modus Eingabe"

863 "oder Ein-/Ausgabe ist."

864 "Modus:"

865 "Gibt den Modus eines Meldungsarguments an. Folgende Werte und Bedeutungen"

866 "sind möglich:"

867 ""

868 "    TT_IN:    Das Argument wird vom Sender geschrieben und vom verarbeitenden"

869 "              Prozeß sowie möglichen Überwachungseinrichtungen gelesen."

870 ""

871 "    TT_OUT:   Das Argument wird vom verarbeitenden Prozeß geschrieben und vom Sender"

872 "              möglichen antwortenden Überwachungseinrichtungen gelesen."

873 ""

874 "    TT_INOUT: Das Argument wird vom Sender, dem verarbeitenden Prozeß sowie"

875 "              möglichen Überwachungseinrichtungen geschrieben und gelesen."

876 "Typ:"

877 "Den Typ kennzeichnende Zeichenfolge. Übliche Werte sind \"string\" und \"int\","

878 "obwohl auch eigene entwickelt werden können."

879 "Wert:"

880 "Tatsächliche Daten des Wertes in Abhängigkeit vom Typ."

881 "Knopf 'Hinzufügen':"

882 "Hinzufügen eines Arguments mit Modus, Typ und Wert."

883 "Knopf 'Löschen':"

884 "Löschen des ausgewählten Arguments."

885 "Knopf 'Ändern':"

886 "Die Werte für das ausgewählte Argument in die aktuellen Werte für Modus, Typ und"

887 "Wert ändern."

888 "Knopf 'Senden':"

889 "Senden dieser Meldung. Darüberhinaus Ausgabe der Tooltalk-Aufrufe"

890 "zum Erstellen und Senden dieser Meldung an die Standardausgabeeinheit."

891 "Nam:"

892 "Name dieses Kontexts."

893 "Wert:"

894 "Wert dieses Kontexts, interpretiert als Typ."

895 "Hinzufügen:"

896 "Die Werte in den Textfeldern als einen neuen Kontext hinzufügen."

897 "Löschen:"

898 "Den derzeit ausgewählten Kontext löschen."

899 "Ändern:"

900 "Für den Kontext den aktuellen Inhalt der Textfelder"

901 "einsetzen."

902 "Pipe konnte nicht für Filterausgabe geöffnet werden.\n"

903 "Tts_string_list:generate für Meldung aufgerufen - wird ignoriert.\n"

$set 11


2 "TT_OK\tAnforderung erfolgreich."

3 "TT_WRN_NOTFOUND\tDas Objekt konnte nicht verschoben werden, da es nicht gefunden wurde."

4 "TT_WRN_STALE_OBJID\tDas Objektattribut in der Meldung wurde durch ein aktuelles ersetzt. Die Position aktualisieren, von der die Objektkennung abgerufen wurde."

5 "TT_WRN_STOPPED\tDie Anfrage wurde von der Filterprozedur gestoppt."

6 "TT_WRN_SAME_OBJID\tDas verschobene Objekt hat die gleiche Objektkennung."

7 "TT_WRN_START_MESSAGE\tDieser Prozeß wurde durch diese Meldung gestartet. Diese Meldung sollte beantwortet werden, auch wenn es sich nur um einen Hinweis handelt."

8 "TT_WRN_APPFIRST\tDieser Code sollte nicht verwendet werden."

9 "TT_WRN_LAST\tDieser Code sollte nicht verwendet werden."

10 "TT_ERR_CLASS\tDer übergebene Wert für Tt_class ist ungültig."

11 "TT_ERR_DBAVAIL\tEine erforderliche Datenbank ist nicht verfügbar. Diese Bedingung ist möglicherweise nur vorübergehend, so daß ein späterer erneuter Versuch sinnvoll wäre."

12 "TT_ERR_DBEXIST\tEine erforderliche Datenbank ist nicht vorhanden. Die Datenbank muß zunächst erstellt werden, da die Aktion sonst nicht ausgeführt werden kann."

13 "TT_ERR_FILE\tDateiobjekt konnte nicht gefunden werden."

14 "TT_ERR_MODE\tDer Wert für Tt_mode ist ungültig."

15 "TT_ERR_ACCESS\tEs wurde versucht, auf eine durch das Sicherheitssystem nicht erlaubte Weise auf ein ToolTalk-Objekt zuzugreifen."

16 "TT_ERR_NOMP\tEs wird kein ttsession-Prozeß ausgeführt. Möglicherweis wurde tt_open() noch nicht aufgerufen. Wenn dieser Code von tt_open() ausgegeben wird, konnte ttsession nicht gestartet werden. Dies bedeutet generell,daß ToolTalk nicht auf diesem System installiert ist."

17 "TT_ERR_NOTHANDLER\tDies ist nur dem verarbeitenden Prozeß für die Meldung möglich."

18 "TT_ERR_NUM\tDer übergebene ganzzahlige Wert ist ungültig."

19 "TT_ERR_OBJID\tDie übergebene Objektkennung bezieht sich auf keine vorhandene Objektspezifikation."

20 "TT_ERR_OP\tDie Syntax des übergebenen Operationsnamens ist ungültig."

21 "TT_ERR_OTYPE\tDer Name des übergebenen Objekttyps entspricht keinem Namen eines installierten Objekttyps."

22 "TT_ERR_ADDRESS\tDer übergebene Wert für Tt_address ist ungültig."

23 "TT_ERR_PATH\tEines der Verzeichnisse im übergebenen Dateipfad ist nicht vorhanden oder kann nicht gelesen werden."

24 "TT_ERR_POINTER\tDer übergebene verdeckte Zeiger (Griffläche) weist nicht auf ein Objekt des richtigen Typs."

25 "TT_ERR_PROCID\tDie übergebene Prozeßkennung ist ungültig."

26 "TT_ERR_PROPLEN\tDer übergebene Merkmalwert ist zu lang."

27 "TT_ERR_PROPNAME\tDie Syntax des übergebenen Merkmalnamens ist ungültig."

28 "TT_ERR_PTYPE\tDer Name des übergebenen Prozeßtyps entpricht keinem Namen eines installierten Prozeßtyps."

29 "TT_ERR_DISPOSITION\tDer übergebene Wert für Tt_disposition ist ungültig."

30 "TT_ERR_SCOPE\tDer übergebene Wert für Tt_scope ist ungültig."

31 "TT_ERR_SESSION\tDer Name der übergebenen Session-ID entspricht keinem Namen einer aktiven Session."

32 "TT_ERR_VTYPE\tDer übergebene Name des Wertetyps ist ungültig."

33 "TT_ERR_NO_VALUE\tKein Merkmalwert mit dem angegebenen Namen und der angegebenen Nummer vorhanden."

34 "TT_ERR_INTERNAL\tInterner Fehler (bug)"

35 "TT_ERR_READONLY\tDas Attribut kann nicht geändert werden."

36 "TT_ERR_NO_MATCH\tEs konnte kein verarbeitender Prozeß für diese Meldung gefunden werden, und als Disposition wurde nicht Warteschlange oder Start angegeben."

37 "TT_ERR_UNIMP\tFunktion nicht implementiert."

38 "TT_ERR_OVERFLOW\tZu viele aktive Meldungen (später erneut versuchen)."

39 "TT_ERR_PTYPE_START\tVersuch fehlgeschlagen, instance für ptype zu starten."

40 "TT_ERR_CATEGORY\tFür das Suchmusterobjekt wurde keine Kategorie definiert."

41 "TT_ERR_DBUPDATE\tDie Datenbank ist inkonsistent: Objekt wurde zuerst von einem anderen Aufruf tt_spec_write aktualisiert."

42 "TT_ERR_DBFULL\tDie Tooltalk-Datenbank ist voll."

43 "TT_ERR_DBCONSIST\tDie Datenbank ist beschädigt, oder die Zugriffsinformation ist nicht vollständig (ttdbck ausführen)."

44 "TT_ERR_STATE\tDie Tt_message hat einen für die gewünschte Operatoin ungültigen Tt_state."

45 "TT_ERR_NOMEM\tSpeicher voll."

46 "TT_ERR_SLOTNAME\tDie Syntax des Steckplatznamens ist ungültig."

47 "TT_ERR_XDR\tDie übergebene XDR-Prozedur wurde als Null-Länge gewertet, oder der Aufruf dieser Prozedur."

48 "TT_ERR_NETFILE\tDie Syntax des übergebenen Namens der Netzwerkdatei ist ungültig."

49 "TT_DESKTOP_UNMODIFIED\tDie Operation kann für nicht geänderte Definitionseinheiten nicht angewendet werden."

50 "TT_MEDIA_ERR_SIZE\tDer für die Größe angegebene Wert ist zu groß oder zu klein."

51 "TT_MEDIA_ERR_FORMAT\tDas Datenformat entspricht nicht dem bisherigen Format dieser Daten."

52 "TT_ERR_APPFIRST\tDieser Code sollte nicht verwendet werden."

53 "TT_ERR_LAST\tDieser Code sollte nicht verwendet werden."

54 "TT_STATUS_LAST\tDieser Code sollte nicht verwendet werden."

55 "Unbekannter Wert für Tt_status."

56 "TT_DESKTOP_EPERM\tNicht Superuser"

57 "TT_DESKTOP_ENOENT\tDatei oder Verzeichnis existiert nicht"

58 "TT_DESKTOP_EINTR\tSystemaufruf wurde unterbrochen"

59 "TT_DESKTOP_EIO\tFehler bei Ein-/Ausgabe"

60 "TT_DESKTOP_EAGAIN\tKeine Prozesse mehr"

61 "TT_DESKTOP_ENOMEM\tNicht genügend Speicherplatz"

62 "TT_DESKTOP_EACCES\tBerechtigung verweigert"

63 "TT_DESKTOP_EFAULT\tUngültige Adresse"

64 "TT_DESKTOP_EEXIST\tDatei existiert bereits"

65 "TT_DESKTOP_ENODEV\tGerät existiert nicht"

66 "TT_DESKTOP_ENOTDIR\tIst kein Verzeichnis"

67 "TT_DESKTOP_EISDIR\tIst ein Verzeichnis"

68 "TT_DESKTOP_EINVAL\tUngültiges Argument"

69 "TT_DESKTOP_ENFILE\tÜberlauf in Dateitabelle"

70 "TT_DESKTOP_EMFILE\tZu viele Dateien geöffnet"

71 "TT_DESKTOP_ETXTBSY\tTextdatei wird bearbeitet"

72 "TT_DESKTOP_EFBIG\tDatei ist zu groß"

73 "TT_DESKTOP_ENOSPC\tKein Speicherplatz mehr verfügbar"

74 "TT_DESKTOP_EROFS\tDateisystem nur im Lesezugriff"

75 "TT_DESKTOP_EMLINK\tZu viele Verbindungen"

76 "TT_DESKTOP_EPIPE\tPipe beschädigt"

77 "TT_DESKTOP_ENOMSG\tKeine Nachricht von gewünschtem Typ"

78 "TT_DESKTOP_EDEADLK\tGegenseitige Sperre"

79 "TT_DESKTOP_ENODATA\tKeine Daten verfügbar"

80 "TT_DESKTOP_EPROTO\tProtokollfehler"

81 "TT_DESKTOP_ENOTEMPTY\tVerzeichnis ist nicht leer"

82 "TT_DESKTOP_ETIMEDOUT\tZeitlimit für Verbindung überschritten"

83 "TT_DESKTOP_EALREADY\tOperation läuft bereits"

84 "TT_DESKTOP_ECANCELED\tOperation abgebrochen"

85 "TT_DESKTOP_ENOTSUP\tNicht unterstützt"

86	"TT_ERR_TOOLATE\tThis must be the first call made into the ToolTalk API and can therefore no longer be performed."

87	"TT_ERR_AUTHORIZATION\tThe user is not authorized to connect to this ToolTalk session."

88	"TT_AUTHFILE_ACCESS\tThe user is not authorized to access the ToolTalk authority file."

89	"TT_AUTHFILE_LOCK\tCannot lock the ToolTalk authority file."

90	"TT_AUTHFILE_LOCK_TIMEOUT\tCannot lock the ToolTalk authority file."

91	"TT_AUTHFILE_UNLOCK\tCannot unlock the ToolTalk authority file."

92	"TT_AUTHFILE_MISSING\tThe ToolTalk authority file is missing."

93	"TT_AUTHFILE_ENTRY_MISSING\tThe ToolTalk authority file is missing an entry for this session."

94	"TT_AUTHFILE_WRITE\tCannot write the ToolTalk authority file."

95	"TT_ERR_VERSION_MISMATCH\tThis client is using a different RPC or program version from the server of this ToolTalk session."
