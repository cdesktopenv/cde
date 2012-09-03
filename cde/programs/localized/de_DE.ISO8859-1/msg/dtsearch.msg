$ $XConsortium: dtsearch.msg /main/7 1996/10/30 17:35:16 cde-ibm $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.

$   COMPONENT_NAME: austext
$ 
$   FUNCTIONS: database
$ 
$   ORIGINS: 27
$ 
$   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$   combined with the aggregated modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1994,1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.

$ ********************** DTSEARCH.MSG ************************
$ $Id: dtsearch.msg /main/7 1996/10/30 17:35:16 cde-ibm $
$ December 1994.
$ Messages specific to DtSearch.
$ Symbolic msg sets replaced with numbers for portability.
$ Basically there is a msgset for each module.

$ $Log$
$ Revision 2.15  1996/04/10  22:54:07  miker
$ Revision 2.14  1996/04/10  19:47:18  miker
$ Revision 2.13  1996/03/20  19:32:13  miker
$ Revision 2.12  1996/03/13  22:53:07  miker
$ Revision 2.11  1996/03/05  16:04:30  miker
$ Revision 2.10  1996/02/01  17:16:48  miker
$ Revision 2.9  1995/12/27  21:17:19  miker
$ Revision 2.8  1995/12/27  16:04:46  miker
$ Revision 2.7  1995/12/07  23:26:31  miker
$ Revision 2.6  1995/12/01  16:10:25  miker
$ Revision 2.5  1995/10/26  16:21:21  miker
$ Revision 2.4  1995/10/20  21:30:42  miker
$ Revision 2.3  1995/10/19  20:52:30  miker
$ Revision 2.2  1995/10/03  21:37:17  miker
$ Revision 2.1  1995/09/22  17:54:34  miker

$quote "

$ General comments about all messages in this file:
$ - Any message that begins with a %1$s or %s is using that variable
$   to represent the name of the executable which is causing the
$   message to be output.
$ - Any variables specified in Usage messages should be translated
$   consistently.

$ ****************************************************
$ set 1 formerly set MS_misc, used many places
$set 1

1 "<leer>"
$ message 2 can be translated to match local conventions.  It is used
$ in output files for information.
2 "%d.%m.%Y,%H:%M:%S"
3 "*** %1$sKeinen Speicher mehr bei %2$s, %3$lu Bytes benötigt! ***\n"
4 "%1$s Version %2$s.\n"
$ message 6 refers to any value which is null when printing information.
6 "<unbekannt>"
8 "%1$s Keine gültigen Datenbanken mehr."
9 "%1$s Ungültiges Befehlszeilenargument '%2$s'.\n"
10 "%1$s Unbekanntes Befehlszeilenargument '%2$s' ignoriert.\n"
11 "%1$s Ungültiger Datenbankname '%2$s'.\n"
$ In message 13, do not translate "dtsrcreate." It is an executable name.
13 "%1$s Kein DB-Satz in Datenbank '%2$s'.\n\
 Der Grund ist normalerweise, daß die Datenbank nicht initialisiert werden kann (dtsrcreate ablaufen lassen)."
14 "%1$s Programmschema-Version '%2$s' nicht kompatibel mit\n\
 Datenbank '%3$s' Version '%4$s'.\n"
15 "%1$s Abbruchsignal %2$d empfangen.\n"
16 "<Keine Umgebungsvariable>"
17 "<Kein Server-Dämon>"
18 "%1$s Datenbankname nicht angegeben.\n"
19 " ,\t\n"
21 "%1$s Exit-Code = %2$d.\n"
$ Message 22 can be translated.  It represents a human-readable date like
$ "Thursday, Jan 5, 1996, 6:30 pm".  The "%5$I" is 12 hour clock time, but
$ it could be changed to "%H" for 24 hour clock time.
22 "%A, %b. %d %Y, %H:%M"
23 "%1$s: Version %2$s. %3$s ablaufen lassen.\n"
$ Message 24 should not translate ".fzk".
24 "%1$s: Aktuelles Arbeitsverzeichnis = '%2$s', Datei .fzk = '%3$s'\n"
25 "%1$s Abbruch wegen Programmfehler.\n"
27 "%1$s: %2$s: Ungültiges Dateiformat."
96 "%s Trefferliste ist leer."
301 "%1$s Eingabetext ist %2$s."
$ Msg 362 is a generic system message.  The format is
$ "<program name> <filename>: <system error>" where <system error>
$ is a system errno which will already be translated.
362 "%1$s %2$s: %3$s"
$ Msg 400 is like "<Program name>: <file name>, line <line number>:..."
400 "%1$s: %2$s, Zeile %3$ld: Ungültige Zeichen in Wort '%4$s'."
$ Msg 423 is like "<Program name> Word <word> in <file name> is a duplicate."
423 "%1$s Wort '%2$s' in %3$s ist doppelt."
1596 "%1$s %2$s Kann Prüfdatei %3$s: %4$s nicht öffnen."

$ ****************************************************
$ set 2 formerly set MS_ausapi (ausapi.*)
$set 2

1 "%1$s Ungültiger oder nicht verfügbarer Datenbankname '%2$s'."
$ Msg 7 should not translate "dbnames" or "dbcount."
7 "%s dbnames angegeben, aber dbcount nicht angegeben."
20 "%1$s Ungültiger search_type '%2$c'."
30 "%s Leere Abfrage. Keine Suche durchgeführt."
$ Msg 37 should not translate "DtSearchInit."  It is a function name.
37 "%s Der erste API-Funktionsaufruf muß DtSearchInit() sein.\n"
40 "%1$s Leerer cleartext. Keine Hervorhebungen durchgeführt."
$ Msg 115 must have the date formatted in the way specified.
115 "%1$s '%2$s' ist eine ungültige oder unvollständige Datumszeichenfolge.\n\
Das korrekte Format lautet '[yy]yy [mm [dd]]'."
$ Msg 187 must not translate "LOGNAME."  It is an environment variable.
187 "%1$s Die Umgebungsvariablen userid und LOGNAME fehlen."
198 "%s Das 'Vor'-Datum ist dasselbe oder ein späteres Datum als das 'Nach'-Datum.\n\
  Es würden keine Datensätze zurückgegeben."
216 "\n%1$s %2$s Signal %3$d empfangen.\n"
294 "%1$s Programmfehler in DtSearchResultsToText."
437 "%s Abfrage enthält ungültige Navigator-Zeichenfolge."
492 "%1$s Es existiert keine aktuelle Trefferliste."
498 "%1$s Ungültige Argumente für DtSearchResultsToText."
621 "%1$s %2$s wurde schon initialisiert."
806 "%1$s Abfrage unzulänglich oder Suchoptionen\n\
inkompatibel mit Datenbank '%2$s'; Suche kann nicht begonnen werden."
$ Msg 1072 "stem" is the root of a word.
1072 "%1$s Programmfehler: Wortstamm-Anzahl (%2$d) größer als Höchstzahl (%3$d)."
1342 "%1$s Suchfunktion-Fehler %2$d für Anforderung hervorheben für\n\
Datenbank '%3$s', Treffer-Wortzahl=%4$ld, Suchtyp='%5$c', Text='%6$.30s'"


$ ****************************************************
$ set 5 formerly set MS_chandel, used in dtsrhan.c
$set 5

3 "Ausgabedatei '%1$s' existiert bereits.\n"
$ Msg 4 needs to use the same 2 characters from messages 4 and 5.
$ The "q" is not needed because it is assumed that any character
$ other than "a" or "o" means to quit the process.
4 "Anhängen, überschreiben oder beenden? [a,ü,b] "
$ Msg 5 is the shortcut key for "a" or Append from message 4.
5 "a"
$ Msg 6 is the shortcut key for "o" or overwrite from message 4.
6 "ü"
7 "Kann Ausgabedatei '%1$s' nicht öffnen.\n"
11 "\nFehler - Kann Profildatei '%1$s' nicht öffnen.\n"
12 "Fehlerzeile %1$d: Ungültige Kennung '%2$s'.\n"
13 "Fehlerzeile %1$d - Kennung '%2$s' hat keine(n) Wert(e).\n"
14 "Fehlerzeile %1$d - Null- oder ungültiger Wert für '%2$s'.\nfehlerhaftes Token: %3$s.\n"
$ Msg 15 should not translate "ANY."
15 "Fehlerzeile %1$d - die Spalte für die Kennung '%2$s' wurde auf ANY eingestellt,\n\
  aber es existiert keine identifizierende Signatur-Zeichenfolge.\n"
16 "Fehlerzeile %1$d - Null- oder ungültiger Wert für 'Kennung '%2$s'\n\
  fehlerhaftes Token: %3$s.\n"
18 "Fehlerzeile %1$d - fehlender Wert für Kennung '%2$s'\n"
23 "Warnzeile %1$d: Begrenzungssymbol neu definiert.\n"
25 "Fehlerzeile %1$d: Begrenzungssymbol nicht als 'oben' oder 'unten' angegeben.\n"
26 "Fehlerzeile %1$d: ungültiges Token '%1$s'.\n"
27 "Fehlerzeile %1$d: ungültiges Token '%2$s'.\n"
$ Msg 34 needs to match the 'all' from msgs 36 and 37.
34 "alle"
$ Msg 35 needs to match the 'none' from msgs 36 and 37.
35 "keine"
$ Msg 36 needs 'all' to match msg 34 and 'none' to match msg 35.
36 "Fehlerzeile %1$d: Abbildungsmodus muß 'alle' oder 'keine' sein -'%2$s' nicht erkannt.\n"
$ Msg 37 needs "all" to match msg 34 and "none" to match msg 35.
37 "Fehlerzeile %1$d: Textmodus muß 'alle' oder 'keine' sein - '%2$s' nicht erkannt.\n"
38 "Warnzeile %1$d: Tastenzeichen neu definiert.\n"
39 "Fehlerzeile %1$d: ungültiges Tastenzeichen:'%2$c'.\n"
40 "Warnzeile %1$d - Tastenfeld neu definiert.\n"
43 "Fehlerzeile %1$d - ungültige Kennung '%2$s' für Taste.\n"
$ Msg 45 should not translate "discard."
45 "Fehlerzeile %1$d: unbekannte Option für 'discard': '%2$s'.\n"
$ Msg 46 should not translate "delblanklines."
46 "Fehlerzeile %1$d: unbekannte Option für 'delblanklines': '%2$s'.\n"
47 "Fehlerzeile %1$d: Unbekannte Option für Abstract:'%2$s'\n"
48 "Fehlerzeile %1$d -unbekannter Kennungstyp '%2$s'.\n"
49 "Fehler - Begrenzungssymbol nicht definiert.\n"
50 "Fehler - Tastentyp-Zeichen noch nicht definiert.\n"
51 "Fehler - Taste noch nicht definiert.\n"
52 "Fehler - Als '%1$s' definiertes Begrenzungssymbol noch nicht gefunden.\n"
53 "Fehler - Als '%1$s' definiertes Begrenzungssymbol referenziert eine physische Zeile im Datensatz.\n\
   Da das Begrenzungssymbol die physischen Zeilen definiert,\n\
  kann es selbst nicht als physische Zeile referenziert werden.\n"
54 "Fehler - Keine Zeilenkennung für das Feld '%1$s' stimmt mit '%2$s' überein.\n"
55 "Fehler - die Liste Feld enthält/enthält nicht enthielt\n\
  das Feld '%1$s', welches noch nicht definiert wurde.\n"
56 "Fehler - die Liste Bild enthält/enthält nicht enthielt\n\
  die Zeile '%1$s', welche noch nicht definiert wurde.\n"
57 "Fehler- die Liste Text enthält/enthält nicht enthielt\n\
  die Zeile '%1$s', welche noch nicht definiert wurde.\n"
58 "Fehler - die Tastendefinition referenziert das Feld '%1$s',\n\
 welches noch nicht definiert wurde.\n"
59 "Fehler - die Abstraktdefinition referenziert das Feld '%1$s',\n\
 welches noch nicht definiert wurde.\n"
68 "Warnung - die für die Taste notwendigen Felder wurden nicht gefunden.\n\
  Der Datensatz %1$ld, der mit\n\
  %2$s begann, wird verworfen.\n"
69 "Warnung - Datensatz %1$ld wird als bedeutungslos erachtet und verworfen...\n\
  Datensatz begann mit: %2$.60s\n"
70 " Kann Eingabedatei nicht öffnen '%1$s'.\n"
$ Msg 71 should not translate ".fzk."
71 "\n\
VERWENDUNG: %1$s [Optionen] <profile> <infile> [<outfile>]\n\
  -m         Alles außer Fehlermeldungen ausschalten.\n\
  -w<N>      Zielbildschirmbreite auf <N> ändern.\n\
  -oo 	Überschreibung der Ausdatei im voraus billigen.\n\
  -oa	Anhängen an die Ausdatei im voraus billigen.\n\
  <Profil>  Eingabedatei mit Profilen von zu verarbeitenden\n\
             Datensätzen.\n\
  <infile>   Eingabedatei mit tatsächlichen Datensätzen.\n\
  <outfile>  Name der Ausgabedatei im Format .fzk (%2$s).\n\n"
72 "Ungültige Bildschirmbreite angegeben.\n"
75 "'%1$s' ist ein ungültiger Ausgabemodus.\n"
76 "Unbekanntes Befehlszeilenargument '%1$s'.\n"
77 "Fehlender erforderlicher Profildateiname.\n"
78 "Fehlender erforderlicher Eingabedateiname.\n"
79 "Fehler - bei Verwendung von stdin als Eingabe ist ein Ausgabedateiname erforderlich!\n"
80 "Profildatei und Eingabedatei haben denselben Namen:'%1$s'.\n"
81 "Eingabedatei und Ausgabedatei haben denselben Namen:'%1$s'.\n"
82 "Profildatei und Ausgabedatei haben denselben Namen:'%1$s'.\n"
83 "        Profildatei: %1$s\n"
$ Msg 84 should not translate "stdin."
84 "          Eingabedatei: stdin\n"
85 "          Eingabedatei: %1$s\n"
$ Msg 86 should not translate "stdout."
86 "         Ausgabedatei: stdout\n"
87 "         Ausgabedatei: %1$s\n"
88 "%1$s. %2$s %3$s Textfilter.\n"
89 "\n  Jeder Punkt steht für %ld Datensätze...\n"
90 "\nFehler beim Schließen der Ausgabedatei - Platte voll?\n"
91 "Beendung aufgrund von Fehlern in der Profildatei.\n"
92 "\n%1$s: Normale Beendigung. %2$ld Datensätze verarbeitet. Exit-Code = 0.\n"
93 "Fehlerzeile %1$d - Zeichenfolge '%2$s' nicht in doppelte Anführungszeichen eingeschlossen.\n"
110 "Warnzeile %d - Datumfeld neu definiert.\n"
111 "Fehlerzeile %1$d - ungültige Kennung '%2$s' für Datum.\n"
115 "%1$s Standardobjektdaten werden '%2$s' sein.\n"
116 "Fehler - Datum referenziert nicht definiertes Feld '%s'.\n"
124 "%1$s Kann nicht an Ausgabedatei '%2$s' schreiben:\n  %3$s\n"
$ Msg 125 should not be translated.
125 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"
130 "%1$s Kann Sprachverarbeitungsdateien\n%2$s nicht laden.\n"
133 "\n\
Warnung - '%1$s' ist eine ungültige Datumsangabe.\n\
  Datum '%2$s' wird für die Datensatznummer %3$ld verwendet, die mit %4$.30s begann.\n"


$ ****************************************************
$ set 6 formerly set MS_ve
$set 6

26 "Wird dummy_workproc() genannt.\n"
142 "%1$s Client-Fehler: Datensatz mit ungültiger\n\ Datenbankadresse %2$ld (%3$d:%4$ld, x'%5$08.8lx') für Datenbank '%6$s' angefordert."
156 "%1$s Nicht kompatible Datenverdichtungstabelle für Datenbank '%2$s' benutzt.\n\
  Datenbank wurde mit %3$ld verdichtet; der Entkomprimierer der Maschine ist %4$ld.\n"
157 "DTSRVE157 Client-Programmfehler: Leere Datenbankadresse in usrblk.dba."
167 "%1$s Keine Daten in Datenbank '%2$s'."
178 "%1$s Datenbank '%2$s' Version '%3$s' nicht kompatibel\n\
  mit Version '%4$s' der Maschine."
183 "%1$s Konnte nicht die Semaphore-Datei '%2$s' der Benutzernotizen öffnen: %3$s.\n"
185 "%1$s Die maximale Wortlänge %3$d der Datenbank '%2$s' ist zu kurz."
199 "%1$s Konnte auf die Benutzernotizen Semaphor '%2$s' innerhalb von %3$d Versuchen nicht zugreifen.\n"
230 "%1$s Konnte nicht die Benutzernotizen-Sicherungsdatei '%2$s' öffnen: %3$s."
231 "%1$sDatenbank '%2$s' hat ungültige Abstraktgröße %3$d."
251 "%1$s Datenbank '%2$s' ist verfälscht:\n\
Datensatzzahlen und Datenbankadressen sind nicht kompatibel.\n"
$ Msgs 310 and 311 can be appended to msg 309 to produce output.
$ For example, one message might be
$ "<Programname> User notes disabled for entire Engine."
309 "%1$s Benutzernotizen deaktiviert "
$ Msg 310 can be appended to msg 309 to produce output.
310 "für gesamte Maschine."
$ Msg 311 can be appended to msg 309 to produce output.
311 "für Datenbank '%1$s'."
$ Msg 332 can be translated to local timestamp.  It currently outputs
$ something like "1996/03/04 at 16:40 CDT"
332 "%d/%m/%Y um %H:%M MEZ"
333 "\n <Benutzernotiz von '%1$s' an %2$s> angehängt.\n"
398 "DTSRVE398 NULL Abfrage-Zeichenfolge."

$ ****************************************************
$ set 7 formerly set MS_uiutil, no longer used in uiutil/dtsrutil.c.

$ ****************************************************
$ set 8 formerly set MS_loadocf, for module ocf.c.

$ All words in set 8 which are capitalized should not be translated.
$ They represent keywords in a configuration file.  Words like PATH,
$ SEARCH, MAXHITS, FILEIO, KEYTYPE, FILEPATH, KEYTYPES, KEYTYPE,
$ UPPERKEYS, FALSE, and HOME.

$set 8

$ Msg 1 is "<Programname><filename>: <keyword> keyword is obsolete."
1 "%1$s%2$s: %3$s Schlüsselwort ist veraltet."
$ Msg 140 "(missing)" refers to a token which is expected to be found but
$ is missing.  The token is from an input string of words being indexed.
140 "(fehlt)"
$ Msgs 142-168 are combined into "<token>: <error>" where msg 142 or a
$ default file name can be <token> and msgs 143, 165, 166, and 168
$ can be <error>.
142 "Rest der Zeile"
143 "ist leer."
165 "hat unbekannte ID."
166 "wurde zuvor angegeben."
168 "hat keine Pfadangabe."
184 "%1$s %2$s: Mehr als 8 Zeichen im Datenbanknamen '%3$.16s'.\n"
$ Msg 221 should not translate "PATH."
221 "%1$s %2$s: Datenbank '%3$s', Kein PATH angegeben, Anweisung ignoriert.\n"
$ Msg 275 should not translate "SEARCH."
275 "%1$s %2$s: Ungültiger SEARCH-Wert ignoriert."
$ Msg 304 should not translate "MAXHITS."
304 "%1$s %2$s: Datenbank '%3$s', ungültiger MAXHITS-Wert.\n"
352 "%1$s %2$s: Ungültiger Wert %3$s, Zeile ignoriert.\n"
365 "%1$s %2$s: Wert %3$s fehlt."
414 "%1$s Kann Konfigurationsdatei '%2$s' nicht öffnen:\n  %3$s."
457 "Datensätze '%1$c'"
477 "%1$s %2$s: Fehlender Datenbankname nach Schlüsselwort '%3$s'.\n"
495 "%1$s %2$s: '%3$.16s' ist ungültiges Schlüsselwort. Zeile %4$d ignoriert.\n"
$ Msg 607 should not translate "FILEIO."
607 "%1$s %2$s: Ungültiger FILEIO-Wert.  Lesen und Schreiben in der Benutzerdatei verboten."
610 "%1$s %2$s: Wert %3$s ist '%4$s'. Sollte positiver Integer sein. Zeile ignoriert."
$ Msg 684 should not translate "KEYTYPE."
684 "%1$s %2$s: Datenbank '%3$s', Ungültiges KEYTYPE-Zeichen '%4$c'."
732 "%1$s %2$s: Datenbank '%3$s', Nicht mehr als %4$d Tastentypen zugelassen.\n"
$ Msg 903 should not translate "FILEPATH."
903 "%1$s %2$s: FILEPATH '%3$s' %4$s. Zeile ignoriert."
929 "%1$s hat Konfigurationsdatei des alten Stils '%2$s' geöffnet."
$ Msg 986 should not translate "KEYTYPES."
986 "%1$s %2$s: KEYTYPES fehlen für Datenbank '%3$s'."
$ Msg 1002 should not translate "KEYTYPE."
1002 "%1$s %2$s: Datenbank '%3$s', Doppeltes KEYTYPE-Zeichen '%4$c'.\n"
1007 "%1$s%2$s: Leere Zeichenfolge %3$s ignoriert."
$ Msg 1011 should not translate "KEYTYPE," "UPPERKEYS," or "FALSE."
1011 "%1$s %2$s: Datenbank '%3$s': Auf Datensätze mit\n\
  KEYTYPE-Zeichen '%4$c' in Kleinschreibung kann nicht zugegriffen werden.\n\
 UPPERKEYS = FALSE setzen, wenn Kleinschreibung gewünscht wird."
1404 "%1$s Status nicht verfügbar für Konfigurationsdatei '%2$s': %3$s"
$ Msg 1643 should not translate "DTSROCFPATH," "HOME," "/etc/inittab,"
$ or "/etc/inetd.conf."
1643 "%1$sKann Konfigurationsdatei '%2$s'\n\
an keiner der folgenden Speicherstellen finden oder öffnen:\n\
  DTSROCFPATH-Verzeichnis =     %3$s,\n\
  aktuelles Arbeitsverzeichnis = %4$s,\n\
  HOME-Verzeichnis =            %5$s,\n\
  /etc/inittab-Verzeichnis =    %6$s,\n\
  /etc/inetd.conf-Verzeichnis = %7$s."
1869 "%1$s %2$s: Konfigurationsdatei muß wenigstens eine Datenbank angeben."

$ ****************************************************
$ set 9 formerly set MS_oeinit, used in oeinit.c and opendblk.c.
$set 9

$ Msg 99 should not translate "open_dblk".  It is a function name.
99 "%s Programmierfehler: Ungültige Eingabe für open_dblk()."
137 "%1$s Version %2$s der Benutzerschnittstelle und Maschinenversion %3$s sind nicht kompatibel."
202 "%s Initialisierung aufgrund von Fehlern in der Konfigurationsdatei fehlgeschlagen."
265 "%s Keine gültigen Datenbanken mehr."
266 "%1$s Initialisierung aufgrund von Fehlern in Datenbank-,\n\
  Sprach- oder verwandten Dateien fehlgeschlagen."
317 "%1$s Kann Datenbankdatei '%2$s' nicht öffnen.\n\
  Errno %3$d = %4$s\n\
  %5$s entfernt '%6$s' aus der Liste verfügbarer Datenbanken."
318 "\n  Dies kann normalerweise durch Angabe eines gültigen\n\
  Datenbank-PFAD in der Bereichs-Konfigurationsdatei korrigiert werden."
1404 "%1$s Datenbank '%2$s' wird aus der Liste verfügbarer Datenbanken entfernt, da der Status für die Datei %3$s nicht verfügbar ist: %4$s"

$ ****************************************************
$ set 10 formerly set MS_oe, used in oe.c and dbchange.c.
$set 10
48 "OE48 Anforderung abgebrochen: Datenbank '%1$s' in dieser Anlage nicht verfügbar."
71 "OE71 %1$s ist abgelaufen."
$ Msg 76 should not translate "dblk".  It is a structure name.
76 "OE76 Programmfehler: Kann dblk für Datenbank '%1$s' nicht finden."
217 "OE217 Programmfehler: Datensatztext zu lang."
367 "OE367: Benutzerschnittstellenfehler. %1$d ist ein ungültiger Anforderungscode.\n"
429 "OE429 Kann Löschdatei '%1$s' nicht öffnen: %2$s. Anforderung ignoriert."
440 "OE440 Es wurden keine Datensatz-Tastentypen in Datenbank '%1$s' ausgewählt."
490 "OE490 %1$s Maschine permanent deaktiviert."
516 "OE516 Nicht mehr als %1$d Zeichen in Suchzeichenfolge zugelassen."
523 "OE523 Anforderung verweigert: Erste Anforderung muß Maschineninitialisierung sein."
753 "%1$s Semantische Kategorien für Datenbank '%2$s' nicht definiert."
1300 "%1$s Kann nicht auf Status der Datenbankdatei\n  '%2$s' zugreifen: %3$s"
1313 "%1$s *** ANFORDERUNG ABGEBROCHEN *** %2$s Maschine neu initialisiert\n\
  aufgrund von Änderung der Datei %3$s, die wahrscheinlich durch\n\
  Aktualisierung einer oder mehrerer Datenbanken verursacht wurde."


$ ****************************************************
$ set 11 formerly set MS_cravel, used in dtsrload.c.
$set 11
1 "\n\
VERWENDUNG: %1$s -d<dbname> [Optionen] <infile>\n\
       Aufgelistete Standard-Dateinamenerweiterungen können außer Kraft gesetzt werden.\n\
  -d<dbname>  1 - 8 Zeichen langer Datenbankname, plus optionales Pfad-Präfix.\n\
              Dateinamenerweiterungen werden automatisch angehängt.\n\
  -t<etxstr>  Ende der Begrenzungssymbol-Zeichenfolge des Textdokuments. Standard ist '\\f\\n'.\n\
  -c          Die Datensatzzahl der Datenbank durch das Zählen von Datensätzen initialisieren.\n\
  -p<N>       Alle <N> Datensätze (Standard %2$d) einen Statuspunkt drucken.\n\
  -h<N> 	     Größe der Hash-Tabelle für doppelte Datensatz-IDs von %3$ld auf <N> ändern.\n\
              -h0 bedeutet, daß keine Kopien existieren, bitte nicht danach suchen.\n\
  -e<path>    Pfad-Dateiname der Huffman-Codiertabelle (Standard %4$s).\n\
  <infile>    Eingabe [Pfad]Dateiname. Standarderweiterung %5%s.\n"
2 "%1$s: Exit-Code = %2$d\n"
13 "%1$s Überprüfung doppelter Datensatz-IDs deaktiviert.\n"
14 "\n%1$s Unbekanntes Befehlszeilenargument '%2$s'.\n"
15 "\nErforderlicher Eingabedateiname fehlt.\a"
16 "\nKein Datenbankname angegeben (Argument -d).\a"
17 "%1$s Gesamte Datensatzzahl in der Datenbank wird durch tatsächliches Zählen... initialisiert.\n"
18 "%1$s: Schemaversion '%2$s' = %3$s, Datensatzzahl = %4$ld, letzter Steckplatz = %5$ld.\n"
19 "%1$s: Endgültige Datensatzzahl der Datenbank = %2$ld, letzter Steckplatz = %3$ld.\n"
20 "%1$s %2$ld Eingabe-Datensätze verarbeitet in %3$ldm %4$lds, (%5$ld%%).\n\
  %6$ld Kopien, %7$ld neue, %8$ld Aktualisierungen.\n"
29 "%1$s Kann Status für %2$s nicht holen: %3$s\n"
30 "%1$s Datei %2$s ist leer.\n"
31 "%1$s: Jeder Punkt = %2$d Datensätze verarbeitet.\n"
33 "%1$s Anzahl verworfener Datensätze %2$ld: Taste zu lang:\n  '%3$s'.\n"
34 "%1$s Kein Speicher mehr! -h arg auf kleinere Zahl setzen\n\
  oder Anzahl von Eingabedatensätzen reduzieren."
35 "%1$s: Anzahl verworfener doppelter Datensätze %2$ld '%3$s'.\n"
246 "\n%1$s '%2$s' ist ungültiger Pfad-/Wörterbuchname.\n"
522 "%s Diese Version von %s unterstützt keine semantischen Datenbanken.\n"
$ Msg 579 should not translate ".fzk."
579 "%1$s Anzahl verworfener Datensätze %2$ld: Ungültiges Dateiformat .fzk.\n"
582 "%1$sUngültiges Statuspunkt-Argument '%2$s' ignoriert.\n"
666 "%1$sProgrammierfehler: Unbekannter Status = %2$d.\n"
717 "%1$s Wird abgebrochen: Unverdichteter Text wird noch nicht unterstützt.\n"
844 "%1$sWird nach %2$ld Warnungen abgebrochen.\n\
  Anzahl der Warnungen wird auf der Befehlszeile angegeben.\n"
926 "%1$s Arg -m ignoriert: %2$s: %3$s.\n"
927 "%1$sAnzahl verworfener Datensätze %2$ld: Erstes Zeichen (Tastentyp) der Taste\n\
  '%3$s' ist nicht alphanumerisch.\n"
1048 "%1$sDatenbankobjekte unverändert, da Eingabedatei effektiv leer.\n"
1086 "%1$s Datensatz '%2$s' hat ungültiges Datumformat--laufendes Datum wird verwendet.\n"
1153 "%1$s ID der aktuellen Datenverdichtungstabelle in '%2$s' ist %3$ld.\n\
  Datenbank '%4$s' zuvor mit Tabelle %5$ld verdichtet.\n"
1215 "%1$s Warnung! Datensatz '%2$s' hat keinen Text.\n"


$ ****************************************************
$ set 12 formerly set MS_initausd, used in dtsrcreate.c.
$set 12

$ Msg 3 should not translate "dtsearch.dbd."
3 "\n\
VERWENDUNG: %1$s [Optionen] <dbname>\n\
       Erstellt und initialisiert DtSearch/AusText Datenbankdateien.\n\
  -q       Keine Informationsmeldungen drucken.\n\
  -o       Ok, schon existierende Datenbank zu überschreiben.\n\
  -a<N>    Maximale Abstraktgröße auf <N> (Standard des jeweiligen Typs) setzen.\n\
  -d<dir>  Wo die Datei dtsearch.dbd zu finden ist, falls nicht im Verzeichnis <dbname> vorhanden.\n\
  -wn<N>   Minimale Wortlänge in <N> ändern. Standard ist %2$d.\n\
  -wx<N>   Maximale Wortlänge in <N> ändern. Standard der jeweiligen Sprache.\n\
  ---------- Datenbanktyp ----------\n\
  -fd      Typ DtSearch. Keine Dokumente, nur Dokumentreferenzen\n\
           in Abstrakts (Standard).\n\
  -fa      Typ AusText. Dokumente werden im zentralen Server-Magazin gespeichert.\n\
  ------------ Unterstützte Sprachen ------------\n\
  -l<N>    Anzahl der Sprachen auf <N> setzen. Der Standard ist 0. Unterstützte Werte:\n\
           0 Englisch-ASCII\n\
           1 Englisch-Latein1\n\
           2 Spanisch\n\
           3 Französisch\n\
           4 Italienisch\n\
           5 Deutsch\n\
           6 Japanisch-autoknj\n\
           7 Japanisch-knjlist\n\
  <dbname> Optionales Pfad-Präfix, dann 1 - 8 Zeichen\n\
           langer Datenbankname. Nicht 'austext' oder 'dtsearch' angeben.\n"

5 "DTSRCREATE Minimale Wortlänge %1$d größer als maximale Wortlänge %2$d.\n"
6 "DTSRCREATE Minimale Wortlänge auf %1$d angepaßt.\n"
8 "DTSRCREATE Maximale Wortlänge auf %1$d angepaßt.\n"
9 "DTSRCREATE Die Angabe kleiner minimaler Wortlängen kann eine umfangreiche\n\
  Bearbeitung der Stoppwort-Datei erfordern, um bedeutend\n\
  erhöhte Voraussetzungen für die Indexspeicherung zu verhindern.\n"
10 "DTSRCREATE Die Angabe großer maximaler Wortlängen kann die\n\
 Speichervoraussetzungen bedeutend erhöhen.\n"
$ Msg 12 needs option "y" for "yes" to be translated the same as in msg 15.
$ Option "n" does not need a translation because the code assumes anything
$ other than the translated "y" option means "no."
12 "\nDatei '%s' existiert bereits.\n\
Ist es ok, sie und andere Datenbankdateien zu überschreiben? [J,N] "
$ Msg 13 should not translate "load_custom_language."
13 "%s Warnung! Sie haben eine nicht unterstützte, spezifische Sprache angegeben.\n\
  Sie müssen Ihre eigenen Sprachladeprogramme zu jeder Programmlaufzeit\n\
  in der Benutzerfunktion 'load_custom_language' bereitstellen, um auf diese Datenbank zugreifen zu können.\a\n"

14 "\n\
%s Semantische Verarbeitung ist nur für englischsprachige Datenbanken verfügbar.\n\a"
$ Msg 15 needs "y" to match the "yes" option in msg 12.
15 "J"
24 "DTSRCREATE Datenbank '%1$s' erfolgreich initialisiert.\n"
98 "DTSRCREATE %1$s Ungültige ID für Huffman-Verdichtungstabelle."
186 "DTSRCREATE186 Kann '%1$s' nicht in '%2$s' umbenennen:\n  %3$s\n"
205 "DTSRCREATE205 Kann Zugriffsrechte für '%1$s' nicht ändern: %2$s\n"
213 "%1$s Kann '%2$s' nicht öffnen:\n  %3$s.\7\n"
214 "%1$s Kann '%2$s' nicht nach '%3$s' kopieren:\n  %4$s\n"
230 "DTSRCREATE230 Konnte Datenbank '%1$s' nicht öffnen.\n"
239 "DTSRCREATE239 Konnte Datenbank '%1$s' nicht initialisieren.\n"
244 "DTSRCREATE244 Kann '%1$s' nicht entfernen:\n  %2$s\n"
326 "DTSRCREATE326 Kann Zugriffsrechte von '%1$s' nicht ändern:\n  %2$s\n\
  Alle Datenbankdateien müssen les- und schreibbar sein.\n\
  Kann möglicherweise Datenbankdateien nicht öffnen oder initialisieren.\n"
327 "  Möglicherweise jedoch nicht brauchbar, es sei denn, alle Datenbankdateien\n\
  verfügen sowohl über Lese- als auch Schreibrechte."
433 "%1$sAngepaßte maximale Abstractgröße bis zu %2$hd.\n"
509 "DTSRCREATE509 Konnte nicht den Kopfzeilen-Datensatz der Datenbank initialisieren.\n"

$ ****************************************************
$ set 13 formerly set MS_vista.
$ Msg numbers 1-100 and 900-1000 match
$ user_error[] and system_error[] respectively in dberr.h.
$ Used in dberr.c, ausdopen.c, and opendblk.c.

$ Msgs 1-100 and 900-1000 in set 13 are generated during database
$ input/output.  They are all fatal errors and should be treated as
$ standalone messages.  They are not combined with other messages to
$ produce errors.  The messages come from a third party vendor, so we
$ do not intend to modify the vendor code and make complete sentences
$ out of them.

$set 13

$ For msgs 1-100, see comments at top of set 13.
1 "Datenbank nicht geöffnet."
2 "Ungültiger Set"
3 "Ungültiger Datensatz"
4 "Ungültige Datenbank"
5 "Ungültiger Feldname"
6 "Ungültige db_address"
7 "Kein aktueller Datensatz"
8 "Set hat keinen aktuellen Eigentümer."
9 "Set hat kein aktuelles Glied."
10 "Tastenwert erfordert."
11 "Ungültiger Sperrwert"
12 "Datensatz ist Eigentümer von nicht-leerem/n Set(s)."
13 "Datensatz ist Glied von Set(s)."
14 "Glied hat bereits Eigentümer."
15 "Feld ist eine zusammengesetzte Taste."
16 "Datensatz nicht verbunden mit Set."
17 "Feld ist keine gültige Taste."
18 "Datensatz ist nicht legaler Eigentümer des Sets."
19 "Datensatz ist nicht legales Glied des Sets."
20 "Fehler in d_setpages (Datenbank geöffnet oder fehlerhafter Parameter)."
21 "Nicht kompatible Wörterbuchdatei"
22 "Ungültiger Versuch, den Systemdatensatz zu löschen"
23 "Ungültiger Versuch, gesperrten Set/Datensatz zu sperren"
24 "Versuch des Zugriffs auf ungesperrten Set/Datensatz"
25 "ID für Datenbewegung nicht angegeben."
26 "Datenbewegung bereits aktiv."
27 "Datenbewegung nicht aktiv."
28 "Datenbewegung kann aufgrund von gesperrten Dateien nicht beginnen."
29 "Kann keine Sperren innerhalb einer Datenbewegung aufheben."
30 "Zu viele geänderte Datenbankseiten"
31 "Kann Datenbank nicht außerhalb einer Datenbewegung aktualisieren."
32 "Exklusiver Zugriff erfordert."
33 "Schreibsperren für statische Dateien nicht zugelasssen."
34 "Nicht angegebene oder doppelte Benutzer-ID"
35 "Datenbank-, Pfad- oder Dateiname zu lang."
36 "Ungültige Dateinummer wurde an d_renfile übergeben."
37 "Feld ist kein optionales Tastenfeld."
38 "Feld ist nicht im aktuellen Datensatztyp definiert."
39 "Datensatz/Feld hat/in eine(r) zusammengesetzte(n) Taste."
40 "Ungültige Datensatz- oder Set-Nummer"
41 "Datensatz/Set nicht zeitgestempelt"
42 "Fehlerhaftes DBUSERID (enthält nicht-alphanumerische Zeichen)"
43 "NetBIOS wurde noch nicht auf der Maschine installiert."
44 "Falsche Elementanzahl in DBDPATH"
45 "Falsche Elementanzahl in DBFPATH"
46 "Kein aktueller Datensatztyp"
47 "Ungültige Tabellensortierzeichenfolge für Land"
48 "Datenbank nicht geschlossen."
$ Msg 301 should not translate "errno."
301 "\n*** System E/A errno %1$d = %2$s\n"
302 "\n\
*** Die gewöhnliche Ursache für diesen Fehlertyp ist eine fehlende\n\
*** oder schreibgeschützte Datenbankdatei, oder eine Systembeschränkung\n\
*** der Anzahl von geöffneten Dateien wurde überschritten."
303 "(Speicherstelle nicht angegeben.)"
$ Msg 304 should not translate "db_VISTA."
304 "\n*** Automatische Wiederherstellung von db_VISTA läuft...\n"
$ Msg 311 should not translate "db_status."
311 "*** Datenbankfehler in %1$s, db_status = %2$d: %3$n"
312 "Programmierfehler."
313 "Unbekannter Fehler."
315 "Datenbank nicht verfügbar, da sie gerade von anderen Anwendern benutzt wird."
348 "%1$s Ungültiger Datenbankname '%2$s'."
378 "%1$s Konnte folgende Zeichenfolge für den Datenbanknamen nicht öffnen:\n  '%2$s'"
379 "%1$s Konnte Datenbank '%2$s' nicht öffnen:\n  %3$s."

$ For msgs 900-1000, see comments at top of set 13.
900 "Kein Platz mehr in Datei."
901 "Systemfehler"
902 "Seitenfehler"
$ Msg 903 should not translate "dio."
903 "Keine Arbeitsdatei in dio"
904 "Kann nicht genügend Speicherplatz zuteilen."
905 "Fehler beim Öffnen der Datei"
906 "Kann nicht auf die taf-Datei der Datenbank zugreifen."
907 "Fehler in der taf/Protokolldatei der Datenbank"
908 "Widersprüchliche Datenbanksperren"
909 "Dateidatensatzgrenze überschritten."
910 "Tastendatei ist widersprüchlich."
911 "Höchstzahl gleichzeitiger Benutzer überschritten."
912 "Fehler bei der Suche in Datenbankdatei."
913 "Ungültige Datei angegeben."
914 "Fehler beim Lesen von einer Datenbank-/Überlaufdatei "
915 "Synchronisationsfehler bei der Sperrverwaltung"
916 "Unterbrechung bei der Überprüfung der Fehlerbereinigung"
917 "Netzwerkkommunikationsfehler"
918 "Automatische Wiederherstellung läuft."
919 "Fehler beim Schreiben an eine Datenbank-/Überlaufdatei"
920 "Keine Sperrverwaltung installiert."
921 "DBUSERID wird bereits verwendet."
922 "Sperrverwaltung ist beschäftigt."


$ ****************************************************
$ set 14 formerly set MS_cborodin, used in dtsrindex.c.
$set 14

1 "%1$s. %2$s betreiben.\n"
2 "%1$s: Warnungen wurden erkannt."
$ Msg 6 should not be translated.
6 "put_addrs_2_dtbs_addr_file() nitems=%1$d, batchsz=%2$ld\n"
$ Msg 9 should not translate "d99."
9 "\n\
\n%1$s Verfälschte Datei d99 für das Wort '%2$s',\n\
  Datenbankadresse %3$ld @ Dateiposition %4$ld => Bitvektor[%5$ld],\n\
  jedoch maximale Bitvektor-Zuteilung = %6$ld.\n"
15 "%1$s Weniger als 2 Bytes in Eingabe.\n"
17 "\n\
VERWENDUNG: %1$s -d<dbname> [Optionen] <infile>\n\
       Aufgelistete Standard-Dateinamenerweiterungen können außer Kraft gesetzt werden.\n\
  -d<dbname>  1 - 8 Zeichen langer Datenbankname, plus optionales Pfad-Präfix.\n\
  -t<etxstr>  Begrenzungssymbol-Zeichenfolge für Ende des Textdokuments. Standard ist '\\f\\n'.\n\
  -r<N> 	     1 Datensatz-pro-Punkt übergeben von %2$d auf <N> ändern.\n\
  -b<N>       Maximale Stapelgröße von %3$ld auf <N> ändern.\n\
  -c<N>       Seitenwechsel-Cache der Datenbank von %4$ld 1K Seiten auf <N> 1K Seiten ändern.\n\
              <N> >= 16 by powers of?? 2. Anfangs nur kleine Änderungen anbringen.\n\
  -i<N>       Eingabepuffergröße von Standard %5$d auf <N> ändern.\n\
  -h<N>      Größe der Hash-Tabelle für doppelte Datensatz-IDs von %6$ld auf <N> ändern.\n\
              -h0 bedeutet, daß keine Kopien existieren, bitte nicht danach suchen.\n\
  <infile>    Name der Eingabedatei [Pfad]. Standarderweiterung %7$s.\n"
21 "\n\
\n%s '%1$s' Datensatz überläuft Wortzähler-Datenfeld.\n\
  Zahl der Datensätze %2$ld > maxdba %3$ld, dba=%4$ld, sld00=%5$d, offs=%6$d.\n"
22 "\n\
\n%1$s Datensatz '%2$s' in Datenbank (dba=%3$ld)\n\
  überläuft Bitvektor-Zuteilung (%4$ld >= %5$ld).\n"
33 "\n%1$s Anzahl eingehender Datensätze überschritt %2$d.\n\
  Dies hat normalerweise den Fehler 'Kein Platz mehr für Seitenwechsel' in Pass 2\n\
  und die Verfälschung der Datenbank zur Folge. Entweder die eingehende Datei aufteilen, um\n\
  die Datensatzzahl zu verringern, oder die Option -b benutzen und nochmals starten.\n"
40 " %1$ld doppelte Datensätze ignoriert."
41 "Keine doppelten Datensätze gefunden."
164 "\n%1$s: %2$s Abgebrochen wegen Signal %3$d. Datenbank %4$s\n\
  möglicherweise verfälscht. Sicherungsdatenbank wiederherstellen.\n"
206 "%1$s: Exit-Code = %2$d, Verstrichene Gesamtzeit %3$ldm %4$lds.\n"
$ Msg 280 should not be translated.
280 "\n%1$s num_addrs_for_word (%2$ld) >= batchsz (%3$ld).\n"
288 "%1$s Abgebrochen. Es sind keine Wörter in der Eingabedatei %2$s.\n"
374 "\n%1$s Kein Speicher mehr!\n\
  Die eingehende Datei in mehrere kleinere Dateien aufteilen und es erneut versuchen.\n"
539 "%1$s Überprüfung doppelter Datensatz-IDs deaktiviert.\n"
550 "%1$s '%2$s' ist ungültiger Pfad/Datenbankname.\n"
558 "%1$s Ungültige Eingabepuffergröße '%2$s'.\n"
567 "%1$s Unbekanntes Befehlszeilenargument '%2$s'.\n"
577 "%1$s Ungültiges Argument '%2$s'. Standard -r%3$d wird benutzt.\n"
580 "%1$s Erforderlicher Eingabedateiname fehlt.\n"
589 "%1$s Kein Datenbankname angegeben (Argument -d).\a\n"
595 "%1$s Ungültiges Stapelgrößeargument '%2$s'.\n"
600 "%1$sCachegröße wieder an %2$d angepaßt.\n"
659 "\n%1$s: %2$s Datensatz wird verworfen, Taste zu lang:\n  '%3$s'.\n"
713 "\n%1$s Das Wort '%2$s' kommt in %3$ld Datensätzen vor,\n\
  aber es befinden sich nur %4$ld Datensätze in der Datenbank!\n\
  (Dies könnte ein guter Kandidat für die Stoppliste sein!)\n"
727 "\n%1$s Anzahl der dbaddr-Datensätze (%2$ld)\n\
überschreitet Bitvektor-Zuteilung (%3$ld).\n"
$ Msg 776 should not translate "d99."
776 "%1$s Schreibfehler in Datei d99: %2$s\n"
$ Msg 848 should not translate "d99."
848 "\n%1$s Konnte %2$ld Bytes (%3$ld Datenbanken) der Datei d99 nicht lesen\n\
an relativer Position %4$ld. Anzahl gelesener Datenbanken (Rückkehrcode) = %5$ld.\n"
849 "\n%1$s: Wortzahl %2$ld, %3$.0f%% beendet. Geschätzte %4$lum %5$02lus bis Beendung.\n"
$ Msg 875 should not translate "fseek" or "d99."
875 "\n%1$s Konnte nicht fseek d99 Datei zu relativer Position %2$ld.\n"
1068 "%1$s Kann neue umgekehrte Indexdatei '%2$s' nicht öffnen: %3$s\n"
1083 "%1$s Kann Eingabedatei '%2$s' nicht lesen: %3$s\n"
1097 "%1$s Wird wegen Fehlern beim Laden der Sprachdateien abgebrochen.\n"
1108 "%1$s: Pass 1 wird begonnen, Datensätze von '%2$s' werden gelesen.\n\
   Jeder Punkt = %3$d Datensätze.\n"
$ Msg 1129 should not translate ".fzk."
1129 "%1$s: %2$s Ungültiges .fzk-Dateiformat.\n"
1168 "%1$s: %2$s '%3$s' verworfen, Taste nicht in Datenbank.\n"
1190 "\n%1$s: Datensatzzahl %2$lu, %3$.0f%% beendet. Geschätzte %4$lum %5$02lus bis zur Beendung von Pass 1.\n"
1225 "%1$s: Pass 1 beendet in %2$lum %3$lus, %4$lu Datensätze gelesen.\n\
  %5$s, %6$lu Wörter analysiert.\n"
1233 "%1$s: Pass 2 wird begonnen: Stapelindex-Positionierung und Datenbank-Aktualisierung.\n\
  Jeder Punkt = %2$d Wörter.\n"
1246 "%1$s: Pass 2 beendet in %2$lum %3$lus, %4$lu Wörter aktualisiert.\n"
1402 "%1$s: Doppelte Datensatzzahl %2$lu '%3$s' verworfen.\n"


$ ****************************************************
$ set 15 formerly set MS_delsfx, now MS_lang, used in lang.c and jpn.c
$set 15

$ Msg 4 should not translate "dblk."  It is a structure name.
4 "%s dblk nicht initialisiert.\n"
$ Msg 5 should not translate "parg."
5 "%s Programmfehler: parg enthält weder Datei noch Zeichenfolge.\n"

8 "\n\
%1$s '%2$.*s...' ist größer\n\
als die maximale Wortlänge der Datenbank '%3$s'."

10 "%s Programmfehler: Unbekannter Parserstatus.\n"
12 "%1$s '%2$.120s' ist kein gültiges Wort in der Datenbank '%3$s'."

17 "\n\
%1$s '%2$s' ist weniger als die\n\
minimale Wortlänge der Datenbank '%3$s'."

19 "%1$s Das Wort '%2$s' ist in der Datenbank '%3$s' nicht indiziert."
20 "%1$s Programmfehler: Unbekannter jstate %2$d.\n"
21 "%1$s '%2$.*s' ist kein gültiges Japanisches Wort."
23 "%1$s Das Wort '%2$s' in Zeile %3$ld ist zu kurz."
24 "%1$s Keine Japanischen Wörter in der Wortdatei '%2$s'."
25 "%1$s Pfad der Datenbank '%2$s' ist zu lang: '%3$s'."
31 "%s Stemmer-Erweiterungsdatei wurde noch nicht geladen.\n"
$ Msg 41 should not translate "Paice."
41 "%1$s %2$s: Ungültige Paice-Regel beginnend bei '%3$s'."
$ Msg 42 should not translate "Paice."
42 "%1$s %2$s: Paice-Regeln in falscher Reihenfolge beginnend bei '%3$s'."
45 "%s Kein Speicher mehr!\n"
50 "Englisch-ASCII"
$ Msg 51 refers to the Latin 1 codeset.
51 "Englisch-Latein1"
52 "Spanisch"
53 "Französisch"
54 "Italienisch"
55 "Deutsch"
56 "Japanisch-comp"
57 "Japanisch-.knj"


$ ****************************************************
$ set 16 formerly set MS_vewords, now MS_boolsrch, used in boolsrch.c.
$set 16

$ Msg 2 is like "<program name> Part of ... term <word> which is
$ not in database <database name>."
2 "\n\
%1$s Ein Teil einer Kollokationsoperation enthält\n\
den Begriff '%2$s', der sich nicht in der Datenbank '%3$s' befindet."
$ Msg 6, variable 3 is the database name.
6 "\n\
%1$s Datenbankfehler. Das Wort '%2$s' ist\n\
in der Datenbank '%3$s' aufgelistet, hat aber keinen Index-Datensatz."
$ Msg , variable 3 is the database name.
11 "%1$s Das Wort '%2$s' wurde in der Datenbank '%3$s' nicht gefunden."
$ Msg 14 should not translate "WHITLIM."  The message is like
$ "<Program name> <word in query> has more than <some number> hits."
14 "\n\
%1$s '%2$s' hat mehr als %3$ld Treffer.\n\
Bitte aus der Abfrage entfernen oder den WHITLIM-\n\
Wert in der Konfigurationsdatei der Suchfunktion erhöhen."
15 "\n\
%s Diese Abfrage erfordert das Auffinden jedes\n\
Dokuments in der Datenbank, das keines der\n\
Abfragewörter enthält. Dieser Suchtyp könnte\n\
ungewöhnlich lange dauern."

$ Msg 28 should not translate "d99."  Variable 2 is the database name.
28 "%1$s Datenbank-Lesefehler in %2$s.d99."
$ Msg 32 should not translate "d99."  The message is formed using either
$ msg 33 or 34 as variable 2, and it is like "<program name> Database
$ Error: Word <word> in database <database name> has invalid d99 record
$ number <number>."
32 "\n\
%1$s Datenbankfehler: %2$s '%3$s'\n\
in Datenbank '%4$s' hat ungültige d99-Datensatzzahl %5$ld."
$ Msg 33 is used for variable 2 of message 32.
33 "Wort"
$ Msg 34 is used for variable 2 of message 32.
34 "Stamm von"
$ Msg 35 should not translate "stemct," and "pmsz."
35 "%1$s Programmfehler: stemct=%2$d pmsz=%3$d\n"

$ ****************************************************
$ set 17 formerly set MS_vestatis
$set 17

104 "Kann gemeinsam benutzten Speicher nicht abtrennen."
107 "Qsort-Stapelüberlauf.\n"
110 "Kann gemeinsam benutzten Speicher nicht entfernen."
115 "Kein gemeinsam benutzter Speicher verfügbar."
310 "%1$s Kein Speicher mehr! %2$ld Bytes benötigt.\n"
314 "Kein Speicher mehr!\n"
421 "Gesamttrefferzahl = %1$ld. Treffer verworfen, die über die angegebene Höchstzahl hinausgingen."
$ Msg 437 should not be translated.
437 "%1$s db_status = %2$d, dba = %3$d:%4$ld (x'%5$08.8lx'), vistano = %6$d.\n"
504 "Kann cur_ditto nicht zuteilen.\n"
677 "Abfragefeld ist leer."
990 "Das System sortiert gerade. Bitte warten."


$ ****************************************************
$ set 23 formerly set MS_dbrec

$ Set 23 has several words in parenthesis which should not be translated.
$ For example, msg 3: "objkey,"  msg 5: "fzkeysz," msg 12: "abstrsz," 
$ msg 24: "minwordsz," msg 26: "maxwordsz," ...  The single words in
$ parenthesis should not be translated.
$ Msgs like 10, where the entire text is in parentheses should be translated.

$set 23

1 "---------- Systemwerte für Datenbank '%1$s' ----------\n"
2 "Die Schemaversionnummer (version) ist '%1$s'.\n"
$ Msg 3 should not translate "sizeof" or "objkey."
3 "Maximale Objekttastengröße (sizeof(objkey)) ist %1$ld Bytes.\n"
$ Msg 4 "Huge" means very, very large.
4 "Optionale 'Riesen'-Tasten aktiviert.\n"
5 "Die Höchstzahl semantischer Kategorien (fzkeysz) ist %1$d.\n"
10 "  (Semantischer Zugriff auf Objekte ist deaktiviert)."
12 "Die maximale Länge einer Abstract-Zeichenfolge (abstrsz) ist %1$d.\n"
14 "  (Abstracts werden in dieser Datenbank nicht verwendet)."
20 "Abstracts sind %1$sverdichtet.\n"
22 "Parsing language is number %1$d, %2$s??.\n"
24 "Die minimale Wortlänge (minwordsz) ist %1$d.\n"
26 "Die maximale Wortlänge (maxwordsz) ist %1$d.\n"
30 "Die Anzahl von .d00 Steckplätze pro Objekt (recslots) ist %1$d.\n"
36 "  (Die Höchstzahl von Datenbankobjekten ist %1$ld).\n"
40 "Die ID der Huffman-Verdichtungstabelle (hufid) ist %1$ld.\n"
42 "  (Die Verdichtung ist in dieser Datenbank deaktiviert)."
44 "  (Eine spezifische Verdichtungstabelle wurde noch nicht festgelegt)."
50 "Der Maschinenzugriff auf Daten kann von Objekt zu Objekt variieren."
54 "Datenobjekte sind nicht direkt von der Maschine aus zugänglich."
56 "Datenobjekte werden intern als verdichteter Text gespeichert."
60 "Nur Referenzen der Serverdatei auf Objekte werden im verdichteten Text gespeichert."
64 "Nur Referenzen der Clientdatei auf Objekte werden im verdichteten Text gespeichert."
68 "Objekttasten sind Referenzen der Serverdatei auf die Objekte."
72 "Objekttasten sind Referenzen der Clientdatei auf die Objekte."
74 "Rererenzen der Serverdatei auf Objekte werden in den 'Riesen'-Tasten gespeichert."
80 "Rererenzen der Serverdatei auf Objekte werden in den Abstracts gespeichert."
86 "Referenzen der Clientdatei auf Objekte werden in den Abstracts gespeichert."
90 "Fehler: Bedeutung des Wertes or_dbaccess (%1$hd) ist unbekannt.\n"
100 " Magazin blobs?? sind %1$sverdichtet.\n"
110 " Magazin blobs?? werden in dieser Datenbank nicht benutzt."
120 "Die Datenbankschalter (dbflags) sind 0x%1$lx:\n"
$ Msgs 124 and 125 are used in the variable in messages 130 and 140.
$ For example: "Inverted index INCLUDES words exactly as parsed."
124 "ENTHÄLT"
125 "ENTHÄLT NICHT"
$ Msgs 126 and 127 are used in the variable in messages 160, 162, 164.
$ For example, "Use of optional 'huge' keys is ENABLED."
126 "AKTIVIERT"
127 "DEAKTIVIERT"
$ Msg 130 uses msgs 124 and 125 in the variable.
130 "  Umgekehrter Index %1$s die Wörter genau, wie sie analysiert wurden.\n"
$ Msg 140 uses msgs 124 and 125 in the variable.
140 " Umgekehrter Index %1$s Wortstämme.\n"
$ Msg 160 uses msgs 126 and 127 in the variable.
160 "  Die Verwendung optionaler 'Riesen'-Tasten ist %1$s.\n"
$ Msg 162 uses msgs 126 and 127 in the variable.
162 "  Markieren-zum-Löschen ist %1$s.\n"
$ Msg 164 uses msgs 126 and 127 in the variable.
164 "  Das Anhängen von Benutzernotizen ist %1$s.\n"
$ Msg 170 is followed by either msg 172 or msg 174.
$ For example, "Text characters are a SINGLE byte wide."
170 "  Textzeichen sind %1$s breit.\n"
172 "ZWEI Bytes"
174 "ein EINZELNES Byte"
200 "Die aktuelle Anzahl von Datenbankobjekten (reccount) ist %1$ld.\n"
210 "Die zuletzt aktuell benutzte Steckplatznummer (maxdba) ist %1$ld.\n"
310 "\n\
VERWENDUNG: %1$s <dbname>\n"
330 "Konnte Datenbank '%1$s' nicht öffnen.\n%2$s\n"
340 "Kein Datensatz dbrec in der Datenbank '%1$s'.\n"
350 "Kann Datensatz dbrec in der Datenbank '%1$s' nicht lesen.\n%2$s\n"

$ ****************************************************
$ set MS_dtsrkdump is set 25.  5/20/96.  dtsrkdump.c
$set 25

1 "%s Abgebrochen wegen Programmfehler.\a\n"
$ Msg 2 should not be translated
2 "%1$c dba=%2$d:%3$-7ld ofs=%4$-9ld adr=%5$-6ld fre=%6$ld\n"
$ Msg 3 is like "<program name> <RCS revision number>, engine
$ <DtSearch version>.  <time stamp>."
3 "%1$s %2$s, Maschine %3$s. %4$s.\n"
$ Msg 4, option "-p" can include decimal points -- this is handled by
$ using sscanf, so if sscanf can read numbers with commas in them, then
$ the "-p" option will process that correctly.  atof is used on what
$ sscanf reads in.
4 "\n\
VERWENDUNG: %1$s -o|w|ow [-v] [-t<N> | -p<N>] <dbname>\n\
       Liest die DtSearch-Tastendateien und druckt eine Zusammenfassung.\n\
  -o        Untersuchte Tasten sind OBJEKT-Datensatztasten.\n\
  -w        Untersuchte Tasten sind umgekehrte Index-WÖRTER.\n\
  -v        VERBOSE-Modus, listet jede Taste auf.\n\
  -t<N>     Schwellenwert. Setzt die Optionen w und v und listet nur Wörter\n\
            mit >= <N> Adressen auf. Alle Wörter werden aufgelistet, wenn <N> = 1.\n\
  -p<N>     Ein anderer Schwellenwert. Derselbe wie -t, außer daß <N> ein Prozentsatz\n\
            der gesamten Datenbank ist (<N> kann ein Dezimalkomma enthalten).\n\
            Beispielsweise druckt -p99.9 jedes Wort aus, das in\n\
            99.9%% oder mehr der Datensätze vorkommt--eine ausgezeichnete Möglichkeit,\n\
            Kandidaten für die Stopliste zu finden.\n\
            Wenn w und v ohne Schwellenwert gesetzt werden, ist der Standard -t%2$d.\n\
  <dbname>  1 - 8 Zeichen langer Datenbankname mit optionalem Pfad-Präfix.\n"
5 "%1$s Ungültiger Prozentwert %2$lf.\a\n"
53 "%1$s Ungültiger Schwellenwert.\a\n"
55 "%1$s Unbekanntes Befehlszeilenargument '%2$c'.\a\n"
56 "%1$s Erforderlicher Datenbankname fehlt.\a\n"
57 "%1$s Entweder -o oder -w muß angegeben werden.\a\n"
58 "%1$s Ungültiger Datenbankname '%2$s'.\a\n"
$ Msg 60 should not be translated.
60 "%1$s: '%2$s' reccount=%3$ld maxdba=%4$ld recslots=%5$hd minw=%6$hd maxw=%7$hd\n"
70 "%1$s Listet nur Wörter auf, die in %2$ld oder mehr Datensätzen vorkommen.\n"
80 "%1$s: Listet alle Wörter in der Datenbank auf.\n"
$ Msg 100 should not be translated.
100 "dba x%1$08lx, %2$6ld\n"
$ Msg 110 is like "in <directory name>"
110 " in %1$s"
120 "Objektübersicht für '%1$s'%2$s:\n"
130 "Objektzahl nach Tastentypen:"
160 "GESAMT-Objektzahl = %1$ld\n"
$ Msg 170 should not translate "DBA."
170 "DBA des größten Objektes  = %1$ld\n"
180 "%1$s: * Mit Sternchen markierte Wörter kommen in jedem Datensatz vor.\n"
200 "Wörterübersicht für '%1$s'%2$s:\n"
210 "GESAMT-Wortzahl = %1$ld\n"

$ ****************************************************
$ set MS_dtsrclean is set 26.  5/21/96.  dtsrclean.c
$set 26

1 "\n\
VERWENDUNG: %1$s [Optionen] <dbname> <newpath>\n\
       Verdichtet ungenutzten d99-Platz und erklärt die Verweise d00-d99 für gültig.\n\
  -p<N>     Statuspunkte werden alle <N> Datensätze gedruckt (Standard %lu).\n\
            Eine vollständige Statusmeldung wird alle %d Punkte gedruckt.\n\
  -oy       Genehmigt die Überlagerung schon existierender d99/d2<N> Dateien in newpath.\n\
  -on       Erzwingt Beendigung, wenn d99/d2<N> Dateien schon in newpath existieren.\n\
  -g        Erklärt d99 und d00 Verweise für gültig, behebt die Verfälschung der Datei d99 und gewährleistet\n\
            eine genaue Datensatzzählung. Verwenden Sie ebenfalls -c0, um die Verfälschung der gesamten Datenbank zu beheben.\n\
  -v<fname> Wie -v, jedoch schreibt außerdem alle d00 nicht von d99 referenzierten Datensätze in einem Format, das in das Dateiformat .fzk extrahiert werden kann, an fname.n\
.\n\
  -c<N>     Beendet das Programm, wenn mehr als <N> verfälschte/unvollständige Verweise (Standard %d) vorhanden sind.\n\
            Dieser Verfälschungshöchstwert wird durch -c0 außer Kraft gesetzt.\n\
  <dbname>  1 - 8 Zeichen langer Datenbankname = die alten d99/d2<N> Dateien, die aktualisiert werden sollen.\n\
            Im lokalen Verzeichnis gefundene Dateien.\n\
  <newpath> Gibt an, wo die neuen d99/d2<N> Dateien abgelegt werden.\n\
            Wenn das erste Zeichen kein Schrägstrich ist, ist der Pfad relativ zum lokalen Verzeichnis.\n\
EXIT-CODES:\n\
  0: Voller Erfolg.  1: Warnung.  2: Job wurde noch nicht gestartet.\n\
  3-49: Job endete vorzeitig, die alten Dateien sind ok, die neuen nicht brauchbar.\n\
  50-99: Schwerwiegender Fehler, selbst die alte Datenbank kann verfälscht sein.\n\
  100+: Strg-C, abbrechen und alle anderen Signalunterbrechungen verursachen vorzeitige\n\
     Beendigung, und die neuen Dateien können unbrauchbar sein. Signal = Exit-Code - 100.\n"
2 "%1$s: %2$s Verdichtung %3$d%% (ungef. %4$lu KB) in %5$ld:%6$02ld min:sek.\n"
$ Msg 3 should not translate "d99."  "WORDS" should be translated.
3 "%1$s: %2$ld WÖRTER in %3$s.d99 gezählt.\n"
4 "%1$s: Keine verfälschten Verweise entdeckt.\n"
5 "%1$s: Exit-Code = %2$d.\n"
$ Msg 6 should not be translated.
6 "  DBA = %1$d:%2$ld (x%3$02x:%4$06lx),  orig addr val = x%5$08lx\n\
  Word='%6$c%7$s' offset=%8$ld addrs=%9$ld free=%10$d\n"
7 "%1$s: Wird von alten d2<N> Dateien auf %2$s kopiert...\n"
$ Msg 11 is like "<program name> Version <version number>. Run <date program
$ was run>."
11 "%1$s Version %2$s. %3$s betreiben.\n"
$ Msg 14 should not translate "d99."
14 "%1%s: Alte Dateien: '%2$s.d2x, .d99'.\n"
$ Msg 15 should not translate "d99."
15 "%1$s: Neue Dateien: '%2$s.d2x, .d99'.\n"
24 "%1$s: Eine oder mehrere neue Dateien existieren bereits.\n"
$ Msg 25 should not be translated.
25 "%1$s: curr reccnt=%2$ld, mxdba=%3$ld, sl/rec=%4$ld, tot#adr=%5$ld.\n"
26 "%1$s: Datei d00 wird gelesen. Jeder Punkt steht für ca. %2$ld Datenbank-Dokumente...\n"
$ Msg 27 should translate DOCUMENTS.
27 "%1$s: %2$ld DOKUMENTE in %3$s.d00 bestätigt.\n"
$ Msg 28 should translate DOCUMENTS.
28 "%1$s: %2$ld DOKUMENTE tatsächlich nicht in %3$s.d00 = Anzahl von %4$ld sind dort gespeichert.\n\
  Diese Zahl wird in der neuen Datei d00 korrigiert.\n"
29 "%1$s: Wird in %2$s verdichtet. Jeder Punkt steht für ca. %3$lu Wörter...\n"
$ Msg 45 needs the "y" for "yes" to match msg 46.
45 "    Überlagerungsdateien im neuen Verzeichnis? [J/N] "
$ Msg 46 must match the "yes" option from msg 45.
46 "J"
66 "%1$s Wird nach Unterbrechungssignal %2$d abgebrochen.\n"
$ Msg 111 should not translate "d99."
111 "*** %1$s DBA in d99 = -1. Möglicher Überlauf in Erweiterungsbereich\n\
 aufgrund von falschen Zählungswerten in der Datei d2x.\n"
159 "%1$s Unbekanntes Argument: '%2$s'.\n"
177 "%1$s Ungültiges Argument %2$.2s.\n"
193 "%1$s Wird bei %2$ld verfälschten Verweisen abgebrochen.\n"
194 "%1$s %2$s %3$ld verfälschte(r)/unvollständige(r) Verweis(e) entdeckt.\n"
210 "%1$s Erforderliches Argument dbname fehlt.\n"
211 "%1$s Erforderliches Argument newpath fehlt.\n"
212 "%1$s Zu viele Argumente.\n"
$ Msg 222 should not translate "d99" or "DBA."
222 "*** %1$s DBA in d99 nicht in d00, Steckplatz > maximale Anzahl von Dokumenten.\n"
229 "%1$s Ungültiger Datenbankname '%2$s'.\n"
301 "%1$s Das Argument -m ist nicht mehr nötig.\n"
$ Msg 333 should not translate "d99" or "d00."
333 "*** %1$s DBA in d99 existiert nicht in d00.\n"
393 "%1$s Die alten und neuen Verzeichnisse sind identisch: '%2$s'.\n"
439 "%1$s Kann %2$s nicht öffnen: %3$s\n"
440 "%1$s Kann nicht auf Status von %2$s zugreifen: %3$s\n"
$ Msg 444 should not translate "d99."  Do not translate starting with "DBA = ..."
444 "*** %1$s Datensatz d00 '%2$s' wird nicht in d99 referenziert.\n\
  DBA = %3$d:%4$ld (x%5$02x:%6$06lx).\n"
463 "%1$s Das Befehlszeilenargument erlaubt keine Dateiüberlagerung.\n"
$ Msg 465 "Validate" should match the description for the "-v" option in msg 1.
465 "%1$s WARNUNG: Kann keinen Speicher für Bitvektor zuteilen.\n\
  Der Modus 'Gültigkeitsprüfung' ist abgeschaltet.\n"
489 "%1$s Schreibfehler in %2$s: %3$s.\n"
499 "%1$s %2$s ist leer.\n"
517 "%1$s Lesefehler in %2$s: %3$s.\n"
$ Msg 561 should not translate "DBA," "maxdba," or "dbrec."
561 "%1$s DBA '%2$d:%3$ld' (Datensatzzahl %4$ld) in d00 überschreitet Gesamtzahl der Adressen %5$ld;\n\
Bitvektor-Überlauf, weil maxdba %6$ld in dbrec falsch ist.\n"
$ Msg 657 is like "<program name> Read error on <filename>: <system errno>."
657 "%1$s Lesefehler in %2$s: %3$s.\n"
$ Msg 665 is like "<program name> Write error on <filename>: <system errno>."
665 "%1$s Schreibfehler in %2$s: %3$s.\n"
$ Msg 802 is like "<program name> Unable to open <filename> to output
$ unreferenced d00 records: <system errno>."  Do not translate "d00."
802 "%1$s Kann '%2$s' nicht öffnen, um nicht referenzierte Datensätze d00 auszugeben:\n  %3$s\n"

$ ****************************************************
$ set MS_dui is set 27.  5/21/96.  dui.c
$set 27
1 "%1$s DtSearch Version %2$s.\n\
VERWENDUNG: %3$s [-Optionen] <query>...\n\
       Optionen, die mit demselben Zeichen beginnen, schließen sich gegenseitig aus.\n\
  -ss         Suchtyp S (Wortstämme) anstatt der standardmäßigen statistischen Suche.\n\
  -sw         Suchtyp W (exakte Wörter) anstatt der standardmäßigen statistischen Suche.\n\
  -pa         Abstracts drucken (Standard).\n\
  -pk         Datensatztasten drucken.\n\
  -pb         Sowohl Abstracts als auch Datensatztasten drucken.\n\
  -p<N> 	     Den <N>ten Datensatz aus der Trefferliste abrufen und drucken.\n\
  -k<C>       Suchen auf Tasten beschränken, die mit dem Zeichen <C> beginnen.\n\
  -d<dbname>  1 - 8 Zeichen langer Datenbankname. Standardmäßig\n\
              die erste Datenbank in der Konfigurationsdatei der Anlage.\n\
  -m<N>       Höchstzahl von zurückgegebenen Treffern. Der Standardwert ist %4$d.\n\
  <query>     Set von Suchwörtern im Rest der Befehlszeile.\n\
              Die Abfrage kann in Anführungsstriche eingeschlossen sein, um Boolesche Operatoren einzuschließen.\n"
4 "Datenbank '%1$s' wird durchsucht...\n"
9 "<kein Abstract verfügbar>"
117 "%1$s Ungültiges Befehlszeilenargument '%2$s'.\a\n"
125 "%1$s Keine Abfrage angegeben.\a\n"
165 "%1$s DtSearch nicht initialisiert:\n  %2$s\n"
190 "%1$s Keine Treffer gefunden.\n"
195 "%1$s Suche fehlgeschlagen.\a\n"
200 "%1$s Text für Dokument '%2$s' nicht verfügbar.\n"
203 "%1$s Konnte keine verfügbaren Tastentypen bestimmen:\n  %2$s\n"
224 "%1$s Keine Datensatztasten in Datenbank '%3$s' beginnen mit '%2$c'.\n"
247 "%1$s ----- Maschinenmeldungen nach Initialisierung -----\n\%2$s\n"
248 "%1$s ----- Ende der Maschineninitialisierungsmeldungen -----\n"

$ ****************************************************
$ set MS_boolpars is set 28.   5/22/96.
$set 28

1 "\n\
%1$s Diese Suchzeichenfolge ist eine ungültige\n\
Boolesche Abfrage. Bitte neu formulieren und erneut versuchen."

2 "%1$s Abfragefeld ist leer."

$ Msg 3 should not translate "@".
3 "\n\
%1$s Kollokations-Operator (@) darf\n\
nur zwischen zwei Wörtern stehen."

$ Msg 4 variable 2 is <database name>.
4 "\n\
%1$s Ein oder zwei Wörter in der\n\
Abfrage sind nicht in der Datenbank '%2$s' gespeichert."

$ Msg 5 "AND" should be the local equivalent to the Boolean Operator
$ for "AND."  "&" should not be translated.
5 "\n\
%1$s Boolesche Operatoren müssen zwischen\n\
Wörtern oder Ausdrücken stehen. Zwei aufeinander folgende Wörter\n\
ohne Operator werden vom Operator UND (&)\n\
als getrennt interpretiert."

6 "%1$s Ausdruck in Klammern fehlt."

$ Msg 5 "NOT" should be the local equivalent to the Boolean Operator
$ for "NOT."  "~" should not be translated.
7 "\n\
%1$s Der Operator NICHT (~) muß auf der linken\n\
Seite des Wortes oder Ausdrucks stehen, den er qualifiziert."

8 "%1$s Ungültige Boolesche Abfrage. Syntaxfehler-Nummer %2$d."

9 "%1$s Zu viele Ausdrücke in Boolescher Abfrage."

$ Msg 10 variable 2 is <database name>.
10 "%1$s Kollokationssuchen stehen für die Datenbank '%2$s' nicht zur Verfügung."

$ Msg 11 should not translate "@."
11 "\n\
%1$s Der Kollokations-Operator '%2$.*s' ist ungültig.\n\
Das korrekte Format ist '@<N>', wobei <N> größer als Null ist."

$ Msg 12 should not translate "@."
12 "\n\
%1$s Der Kollokations-Operator (@) ist\n\
zwischen identischen Wörtern nicht erlaubt."

13 "%1$s Das Wort '%2$.*s' ist ungültig."

14 "%1$s Die Zeichenfolge '%2$.*s' ist nicht ein einzelnes Wort."

15 "\n\
%1$s Diese Abfrage kann keine Datensätze\n\
logisch zurückgeben. Bitte neu formulieren und erneut versuchen."

16 "\n\
%1$s Diese Abfrage gibt die gesamte Datenbank\n\
'%2$s' zurück. Bitte neu formulieren und erneut versuchen."


$ ****************************************************
$ set 29 symbolically named MS_tomita, used in tomita.c (dtsrdelete)
$set 29

1 "\n\
\n%1$s Unterbrechung %2$d empfangen.\n\
  Programm wird nach dem aktuellen Löschstapel gestoppt.\n"

3 "%1$s: Exit-Code = %2$d.\n"
4 "%1$s Datenbank '%2$s' nicht gefunden.\n"
5 "Zur Auswahl stehen:"
8 "%1$s Ungültiges Eingabeformat: %2$.30s...\n"
$ Msg 24 is like <Program name> Database <dbname>, <record id> not found."
24 "%1$s Datenbank %2$s, '%3$s' nicht gefunden.\n"

25 "\n\
NICHT FORTFAHREN, wenn eine der folgenden Bedingungen gegeben ist:\n\
-> Falls die Eingabedatei, die zu löschende Datensatz-IDs auflistet, nicht\n\
 den Namen '%1$s' hat.\n\
-> Falls Benutzer noch auf die betroffene(n) Datenbank(en) zugreifen.\n\
-> Falls Datenbankdateien noch nicht gesichert wurden.\n\n\
Wenn Sie sicher sind, daß Sie den Löschvorgang beginnen möchten, geben Sie jetzt 'J' ein... "

$ Msg 26 is "<program name> <number of files> read, <number> deleted,
$ <time> elapsed. Database <dbname>: Current record count = <number>,
$ Batch size = <number of records>."
26 "%1$s %2$ld gelesen, %3$ld gelöscht, %4$ldh %5$2ldm %6$2lds vergangen.\n\
  Datenbank '%7$s': Aktuelle Datensatzzahl = %8$ld, Stapelgröße = %9$d.\n"
$ Msg 27 is "<program name> <number of records> records read from input file.
$ <number> were deleted and <number> were not found in <time>." 
27 "%1$s %2$ld Datensätze aus der Eingabedatei gelesen. %3$ld wurden gelöscht und\n\
  %4$ld wurden in %5$ld Stunden, %6$ld Minuten, %7$ld Sekunden nicht gefunden.\n"

28 "  oder eine durchschnittliche Löschzeit von %1$ld Minuten, %2$ld Sekunden pro Datensatz.\n"
$ Msg 29 is "<program name>. Run <time stamp>."
29 "%1$s. %2$s betreiben.\n"

$ Msg 30 should not translate "-y".
30 "\n\
VERWENDUNG: %1$s [Optionen]\n\
  -i    Name der Eingabedatei. Falls nicht angegeben, gilt der Standardwert %2$s.\n\
  -d[v] Anweisungen für die Fehlerbereinigung drucken.\n\
        -dv schaltet die verbose (Datensatz-für-Datensatz) Fehlerbereinigung ein.\n\
  -t<N> Gewünschte Höchstzahl von Sekunden Laufzeit.\n\
        Strg-C/Unterbrechung stoppt ebenfalls den Löschvorgang beim nächsten Datensatz.\n\
  -n<N> Die Anzahl von Datensätzen in einem Stapel von %5$d auf <N> ändern.\n\
  -y    Antwortet automatisch mit 'Ja' auf die Aufforderung zur Bestätigung des Löschmodus.\n\
  -d    Ablaufverfolgung von Löschvorgängen.\n"

31 "%1$s Unbekanntes Argument '%2$s'.\n"
32 "%1$s Kann Eingabedatei '%2$s' nicht öffnen.\n"
33 "\n%1$s Kann Ausgabedatei '%2$s' nicht öffnen.\n"
34 "Suchfunktion wird initialisiert...\n"
36 "Normale Maschinen-Abschaltung.\n"


$ ****************************************************
$ set 30 named MS_huff has messages for hencode.c, hdecode.c, huffcode.c
$set 30

$ Msg 1 variable 2 is <filename>, variable 3 is <system errno>.
1 "%1$s: Kann Huffman-Codierdatei '%2$s' nicht öffnen: '%3$s' Exit-Code = 2.\n"
2 "\n\
VERWENDUNG: %1$s <filename>\n"
3 "%1$s Konnte Eingabedatei '%2$s nicht öffnen.\n"
4 "%1$s Gesamtzahl von Zeichen = %2$ld.\n"

10 "%1$s Kein Speicher mehr!\n"
$ Msg 11 variable 2 is <tree file name>, variable 3 is <system errno>.
11 "%1$s Kann Baumdatei '%2$s' nicht öffnen: %3$s.\n"
$ Msg 12 variable 2 is <tree file name>.  Do not translate "hctree."
12 "%1$s Ungültiges Format hctree '%2$s'.\n"
$ Msg 13 should not translate "hctree_ids."
13 "%1$s Nicht kompatible hctree_ids.\n"
14 "%1$s Kann CIPHER-Datei '%2$s' nicht öffnen.\n"
$ Msg 15 should not translate "EOF."
15 "%1$s Unerwartetes EOF '%2$s'.\n"

30 "183 Bitzeichenfolgen sind zu groß geworden. Sie haben wahrscheinlich\n\
bei stark unausgewogenen Anzahlen von Zeichen die Literale abgeschaltet.\n"
31 "191 Programmierfehler: Versucht noch immer, nach Erstellung\n\
des Stamms einen Huffman-Code-Baum zu bauen.\n"
32 "(Nicht-ASCII-Zeichen, High Bit-Set)"
33 "366 Ungültiges Dateiformat für %1$s.\n"
34 "424 Datei '%1$s' konnte nicht zum Schreiben geöffnet werden. Bestätigen Sie, daß sie nicht schreibgeschützt ist.\n"
35 "\n\
VERWENDUNG: huffcode [-l<N> | -l-] [-o] <huffname> [<infile>]\n\
  -l<N>      gibt die Schwellenwert-Zahl für 'Literal' an. Alle Zeichen, die\n\
             <= <N> Mal vorkommen, werden mit dem Huffman-Literal codiert. Der Standardwert ist -l0;\n\
             Literal-Codierung ist nur für Bytes mit Null-Anzahlen möglich.\n\
  -l-        schaltet die Literal-Codierung ab. Das Abschalten der Literal-Codierung in unausgewogen\n\
             Bäumen führt zu EXTREM LANGEN Bitzeichenfolge-Codes--bitte nur abschalten, wenn\n\
             Sie wissen, daß die Eingabe eine gut ausgewogene Binärdatei ist.\n\
  -o         genehmigt im voraus das Überschreiben aller aktuell existierenden Decodierdateien.\n\
  <huffname> ist das Dateinamen-Präfix für die Huffman-Code-Dateien.\n\
             Falls die Codierdatei (%1$s) bereits existiert, werden ihr die Bytezahlen aus der Eindatei\n\
             hinzugefügt; andernfalls wird sie neu erstellt.\n\
             Die Decodierdatei (%2$s) wird für jeden Programmablauf neu erstellt.\n\
  <infile>   ist eine Eingabedatei, die zu zählende Bytes enthält.\n\
             Sie kann ausgelassen werden, wenn die Codierdatei bereits existiert.\n"
36 "'%1$s' ist ein ungültiges Argument.\n"
37 "576 Das Dateinamen-Präfix des Huffman-Codes fehlt.\n"
$ Msg 38 needs to have "y" translated to match msg 39"
38 "Die Decodierdatei '%s' existiert bereits. Überschreiben? [J/N]\n"
$ Msg 39 needs "y" to match the "yes" option in msg 38.
39 "J"
40 "HUFFCODE-Version %1$s\n"
41 "Die Huffman-Code-Tabellen werden neu erstellt.\n"
42 "Tabelle '%1$s' enthält bereits %2$ld Kilobytes von vorherigen Programmabläufen.\n"
43 "645 Eingabedatei nicht angegeben und Tabellendatei '%s'\n\
existiert nicht -- nichts zu tun.\n"
44 "Konnte weder Eingabedatei '%1$s' öffnen noch auf Status zugreifen: %2$s\n"
45 "Eingabedatei '%1$s' enthält ca. %2$ld Kilobytes.\n"
46 "\n%1$ld%% beendet. %2$2ld Kilobytes gelesen. Schätzungsweise %3$3ld Sekunden bis Beendigung.\n"
$ Msg 47 variable 2 is a time stamp, variable 2 is from either msg 48 or 49,
$ variables3 and 4 are filenames.
47 "Identifizierender Zeitstempel ist '%1$ld'.\n\
Huffman-Code-Tabellen werden in '%3$s' und '%4$s' %2$s...\n"
$ Msg 48 is used in msg 47 as variable 2.
48 "erstellt"
$ Msg 49 is used in msg 47 as variable 2.
49 "neu gebaut"

$ ********************** DTSEARCH.MSG ************************
