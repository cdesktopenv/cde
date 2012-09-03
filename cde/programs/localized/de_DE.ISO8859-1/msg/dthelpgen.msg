$ $XConsortium: dthelpgen.msg /main/4 1996/08/11 16:05:13 pascale $
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
$  
$quote "

$set 1
$ 
$ _DtMessage 1 is the usage message
$ 
$ Localize only the words 'directory', 'name', and 'language'.
$ 'directory' means the directory name in which the resulting files
$ will be placed. 'name' mean what base name to give the files placed
$ in 'directory'. 'language' means which localized versions of help files
$ to look for.
$ 

1 "%s [-dir <Verzeichnis>] [-generate] [-file <Name>] [-lang <Sprache>]\n"
$ 
$ Mesages 2-18 are error messages.
$ 

2 "%s: Element von %s ist kein Verzeichnis\n"

3 "%s: Zugriff auf Verzeichnis %s verweigert\n\
Ausführung als Superuser versuchen?\n"

4 "%s: Element von %s ist nicht vorhanden\n"

5 "%s: Dateisystem mit %s ist voll\n"

6 "%s: Kein Zugriff auf %s - Fehlerstatusnummer %d\n"

7 "%s: Für Dateisystem mit %s besteht nur Lesezugriff\n"

8 "%s: Zum Schreiben in %s ist Root-Berechtigung erforderlich\n"

9 "%s: Schreiben in %s nicht möglich\n"

10 "%s: Suchpfad leer\n"

11 "%s: Ressource 'title' fehlt\n"

12 "%s: Ressource 'abstract' fehlt\n"

13 "%s: Ressource 'volumes' fehlt\n"

14 "%s: Ressource 'character' fehlt\n"

15 "%s: Zielverzeichnis fehlt\n"

16 "%s: Keine Familiendateien gefunden\n"

17 "%s: Keine Paketdateien gefunden\n"

18 "%s: Kein Zugriff auf aktuelles Arbeitsverzeichnis - Fehlerstatusnummer %d\n"

19 "%s: Es kann kein Speicher zugeordnet werden\n"

20 "%s: Ungültige Sprache: %s\n"
$ 

$set 2        Redefine set#

$ 
$ Specifies the character set used to create this file
$ 
$ This must be localized for the various languages. That is
$ for Japanese shift JIS, it would be 'ja_JP.SJIS'; for Japanese
$ EUC it would be 'ja_JP.eucJP'. For files written using
$ HP Roman8 character set it would be '<lang+terr>.HP-ROMAN8', etc.
$ The set of allowable locale strings can be found in
$ /usr/dt/config/svc/CDE.lcx
$ 

1 "de_DE.ISO-8859-1"
$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 

2 "Willkommen im Hilfemanager"
$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 

3 "<ABBREV>Willkommen im Hilfemanager</ABBREV>        \
<PARAGRAPH>Jeder der nachfolgend aufgeführten Titel stellt eine \n\
<ANGLE italic> Produktfamilie</>\n\
dar, für die die zugehörige Online-Hilfe installiert und registiert ist. Alle Titel\n\
(und Symbole) sind sogenannte 'Hyperlinks', mit denen die Hilfetexte innerhalb der Familie aufgelistet werden.</>\
<PARAGRAPH after 0 first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\"> \
Zum Anzeigen der verfügbaren Hilfethemen für\
eine Produktfamilie den zugehörigen Titel (unterstrichener Text) bzw. das zugehörige Symbol auswählen.\
<PARAGRAPH after 0 first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\"> \
Innerhalb der Produktfamilie das gewünschte Thema suchen und den zugehörigen Titel auswählen. \n\
<PARAGRAPH first 1 left 3 label \"<CHAR C.DT-SYMBOL-1><0xB7></>\"> \
Durch Drücken von F1 kann jederzeit Hilfe aufgerufen werden.</PARAGRAPH>"
$ 
$ Specifies the Volume Title
$ 

4 "Hilfe - Hauptthema"
$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 

$quote
5 <ABBREV>Willkommen im Hilfemanager</ABBREV>\
<LINK 0 \"Help4Help How-To-Register-Help\"> \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>\
 Hinweis:\ \ \ Keine Hilfe

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 

$quote "
6 "dthelpgen"
$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 

7 "Informationen für Anzeigeprogramm werden generiert. Bitte warten."
$ 

$set 3        Redefine set#

$ 
$ ******* Text Formatting Templates ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ 

1 "<TOPIC charset %s>"

2 "<TITLE><TYPE serif><WEIGHT bold><SIZE 14>%s</SIZE></WEIGHT></TYPE></TITLE>"

3 "<PARAGRAPH before 1 first 1 left 1>"

4 "<CHARACTERSET %s>"

5 "<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>%s</></></></>"

6 "<PARAGRAPH before 1 first 1 left 1 graphic %s glink %s gtypelink 0>"

7 "<PARAGRAPH before 1 first 1 left 1 graphic %s glink \"%s %s\" gtypelink 1>"
