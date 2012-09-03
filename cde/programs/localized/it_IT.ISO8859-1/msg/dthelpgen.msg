$ $XConsortium: dthelpgen.msg /main/3 1995/11/08 12:42:02 rswiston $
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
1 %s -dir <directory> [-generate] [-file <nome>] [-lang <lingua>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: Un elemento di %s non è una directory\n
3 %s: Accesso negato alla directory %s\nSi desidera provare come superutente?\n
4 %s: Un elemento di %s non esiste\n
5 %s: Il file system che contiene %s è pieno\n
6 %s: Impossibile accedere a %s - stato di errore numero %d\n
7 %s: Il file system che contiene %s è di sola lettura\n
8 %s: Per scrivere in %s è richiesta l'autorizzazione di superutente\n
9 %s: Operazione di scrittura in %s non valida\n
10 %s: Il percorso di ricerca è vuoto\n
11 %s: Risorsa 'title' non disponibile\n
12 %s: Risorsa 'abstract' non disponibile\n
13 %s: Risorsa 'volumes' non disponibile\n
14 %s: Risorsa 'character' non disponibile\n
15 %s: Directory di destinazione mancante\n
16 %s: Nessun file trovato per la famiglia\n
17 %s: Nessun file trovato per il volume\n
18 %s: Impossibile accedere alla directory di lavoro corrente - stato di errore numero %d\n
19 %s: Impossibile allocare la memoria\n
20 %s: La lingua di sistema %s non è valida\n

$ 
$set 2
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
1 it_IT.ISO-8859-1

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 Benvenuti nella Gestione dell'aiuto

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>Benvenuti nella Gestione dell'aiuto</ABBREV>                     \
<PARAGRAPH>Ognuno dei titoli sotto elencati rappresenta una\n              \
<ANGLE italic> famiglia di prodotti</>\n                                   \
dotata di un aiuto online installato e registrato. Ogni titolo (e icona)\n \
rappresenta un collegamento ipertestuale agli aiuti disponibili nella famiglia.</>   \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">   \
Per visualizzare la lista degli aiuti disponibili per una data famiglia,\n \
scegliere il titolo (testo sottolineato) o l'icona corrispondente.</PARAGRAPH> \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">  \
All'interno di una famiglia, cercare l'argomento su cui si desidera\n     \
ottenere informazioni e scegliere il titolo corrispondente.</PARAGRAPH>   \
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">          \
Per informazioni sull'uso delle finestre di aiuto, premere F1.</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 Aiuto - Livello superiore

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>Benvenuti nella Gestione dell'aiuto</ABBREV>                   \
<LINK 0 "Help4Help How-To-Register-Help">                                \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>                         \
Nota:\ \ \ Nessun aiuto registrato                                       \
</SIZE></WEIGHT></TYPE></></LINK>                                        \
<PARAGRAPH leftindent 3 firstindent 3>                                   \
<WEIGHT bold>Nessuna famiglia di prodotti ha registrato i propri file di \
aiuto online.</> In alcuni casi sarà possibile ottenere informazioni     \
scegliendo i comandi dell'aiuto direttamente dall'interno delle          \
applicazioni.</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 Generazione delle informazioni per il browser in corso. Attendere.

$ 
$set 3
$ 
$ ******* Text Formatting Templates ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ 
1 <TOPIC charset %s>
2 <TITLE><TYPE serif><WEIGHT bold><SIZE 14>%s</SIZE></WEIGHT></TYPE></TITLE>
3 <PARAGRAPH before 1 first 1 left 1>
4 <CHARACTERSET %s>
5 <TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>%s</></></></>
6 <PARAGRAPH before 1 first 1 left 1 graphic %s glink %s gtypelink 0>
7 <PARAGRAPH before 1 first 1 left 1 graphic %s glink "%s %s" gtypelink 1>


