$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 11:53:14 rswiston $
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dthelpprint.msg  
$ **  
$ **  Project:     CDE 1.0 Help  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dthelpprint  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1994 Hewlett-Packard Company  
$ **         All Rights reserved  
$ **  
$ **  
$ **     There are two types of messages in this file:  
$ **  
$ **     1) Messages that appear in printed output  
$ **        These messages are the default and they should all be localized.  
$ **        These messages are marked with LOCALIZE THESE MESSAGES.  
$ **  
$ **     2) Messages that should not be localized.  
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.  
$ **  
$ *****************************************************************************  
$ **  
$ **  
$ **  ------------------------- MODIFICATION RECORD --------------------------  
$ *   Major Mods, 7/11/94 for dthelpprint  
$ *  
$ **  ----------------------- MODIFICATION RECORD END ------------------------  
$ *****************************************************************************  
$ *************************************<+>*************************************  
$ Use the double quote char around all messages  
$quote "


$set 2        Redefine set#

$ *****  Module: PrintUtil.c *****  
$   
$set 3
$ *****  Module: PrintTopics.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  

1 "%s Fehler: helpType ist ein Thema, es wurde jedoch kein helpVolume angegeben.\n"

2 "%s Fehler: Hilfeband %s kann nicht gefunden werden\n"

3 "%s Fehler: Fehler bei der Verarbeitung des Hilfebands %s\n"

4 "%s Fehler: Speicherzuordnung fehlgeschlagen\n"

5 "%s Fehler: Themeninformation nicht abrufbar:\n\
Band %s, locationId %s\n"

6 "%s Fehler: Temporäre Datei %s kann nicht geöffnet werden\n"
$ String used to replace $SECTNUM when printing the index section  

10 "Index"
$ String used to replace $SECTNUM when printing the table of contents section  

11 "Inhaltsverzeichnis"
$ ** DO NOT LOCALIZE THESE MESSAGES **  
$ index entry, page number  

20 "%s, %d\n"
$ index subentry, page number  

21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint  

30 "__GENERATED-INDEX"
$ string used by can't get a topic title associated with an index subentry  

31 ""

$set 4        Redefine set#

$ *****  Module: PrintManStrFile.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  

1 "%s Fehler: helpType ist eine Zeichenfolge, stringData wurde jedoch nicht angegeben.\n"

2 "%s Fehler: helpType ist eine dynamische Zeichenfolge, stringData wurde jedoch nicht angegeben.\n"

3 "%s Fehler: helpType ist eine Hilfeseite, manPage wurde jedoch nicht angegeben.\n"

4 "%s Fehler: helpType ist eine Datei, helpFile wurde jedoch nicht angegeben.\n"

5 "%s Fehler: Speicherzuordnung fehlgeschlagen\n"

$set 5        Redefine set#

$ *****  Module: Initialize.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  
$ Messages 1 to 29: dthelpprint usage message  
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).  
$ **DO LOCALIZE** the option argument (e.g. number) and description.  

1 "dthelpprint - Druckprogramm für CDE-Hilfe\n\n"

2 "Verwendung: dthelpprint [Optionen]\n"

3 "Optionen für Drucksteuerung:\n"

4 "\t-printer printername  Zu verwendender Drucker\n"

5 "\t-copies number        Anzahl der zu druckenden Exemplare\n"

6 "\t-outputFile filename  Ausgabe in diese Datei schreiben\n"

7 "\t-paperSize size       Inhalt für dieses Papierformat formatieren\n"
$ Do NOT localize the paper size names help_papersize_xxx  

8 "\t\tsize = { help_papersize_letter|help_papersize_legal|\n"

9 "\t\t         help_papersize_letter|help_papersize_letter|help_papersize_letter}\n"

10 "\t-display displayname  Anzeige für Abruf von Ressourcen\n"

11 "\t-name                 Bei Ressourcenabruf zu verwendender Programmname\n"

12 "\t-class                Bei Ressourcenabruf zu verwendender Klassenname\n"

13 "\t-xrm resourcestring   Weitere Ressourcen\n"

14 "Optionen für Drucksteuerung:\n"

15 "\t-helpType type        Typ der Hilfedaten\n"

16 "\t\ttype = 0 (Hilfeband), 1 (Zeichenfolge), 2 (Hilfeseite), 3 (Hilfedatei)\n"

17 "\t-helpVolume volume    Vollständiger Pfad der Hilfebanddatei\n"

18 "\t-locationId location  Name des Hilfethemas im Hilfeband drucken\n"

19 "\t-all                  Alle Themen, Inhaltsverzeichnis und Index im Hilfeband drucken\n"

20 "\t-sub                  locationId für Thema und alle Unterthemen drucken\n"

21 "\t-one                  locationId für Thema drucken\n"

22 "\t-toc                  Inhaltsverzeichnis des Hilfebands drucken\n"

23 "\t-index                Index des Hilfebands drucken\n"

24 "\t-frontMatter          Frontteil des Hilfebands drucken\n"

25 "\t-manPage manpagename  Name der Hilfeseite\n"

26 "\t-locationId location  Zu druckender Hilfetext\n"

27 "\t-helpFile filename    Datei mit Hilfetext\n"

28 "\t-jobTitle title       Titelzeichenfolge für Druckjob\n"

29 "\t-topicTitle title     Titelzeichenfolge für Hilfetext\n"
$ Warning and error messages  
$ Do NOT localize the default size name help_papersize_letter  

40 "%s Warnung: Fehlende oder ungültige Angabe für Papierformat '%s'. \n\
'help_papersize_letter' wird verwendet.\n"

41 "%s Warnung: Fehlende Angabe für Papierformat oder Länge/Breite. \n\
'help_papersize_letter' wird verwendet.\n"

42 "%s Warnung: Anzeige %s kann nicht geöffnet werden\n"

45 "Fehler: Für temporäre Datei kann kein Speicher zugeordnet werden.\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **  
$ arg order: directory prefix processid filecnt suffix  

50 "%1$s/%2$s%3$d_%4$d%5$s"

$set 6        Redefine set#

$ *****  Module: Main.c *****  
$   
$ ** LOCALIZE THESE MESSAGES **  

1 "%s Fehler: helpType %d ungültig.\n"
