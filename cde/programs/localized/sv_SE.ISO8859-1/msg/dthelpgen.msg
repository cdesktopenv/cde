$ $XConsortium: dthelpgen.msg /main/3 1995/11/08 13:33:37 rswiston $
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
1 %s -dir <katalog> [-generate] [-file <namn>] [-lang <språk>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: Element av %s är inte en katalog\n
3 %s: Du saknar behörighet till %s\nFörsök köra som superanvändare?\n
4 %s: Element av %s finns inte\n
5 %s: Filsystemet som innehåller %s är fullt\n
6 %s: Det går inte att få tillgång till %s - felstatus %d\n
7 %s: Filsystemet som innehåller %s kan endast läsas\n
8 %s: Det krävs superanvändarbehörighet för att ändra %s\n
9 %s: Det går inte att ändra %s\n
10 %s: Sökvägen är tom\n
11 %s: Resursen 'title' saknas\n
12 %s: Resursen 'abstract' saknas\n
13 %s: Resursen 'volumes' saknas\n
14 %s: Resursen 'character' saknas\n
15 %s: Målkatalogen saknas\n
16 %s: Inga kategorifiler hittades\n
17 %s: Inga volymfiler hittades\n
18 %s: Det gick inte att få tillgång till den aktuella katalogen - felstatus %d\n
19 %s: Minnestilldelningen misslyckades\n
20 %s: Felaktigt systemspråk angivet %s\n

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
1 sv_SE.ISO-8859-1

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 Välkommen till Hjälp

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>Välkommen till Hjälp</ABBREV>                                 \
<PARAGRAPH>Var och en av titlarna som visas nedan representerar en\n    \
<ANGLE italic> produktkategori</>\n                                       \
vars hjälp är installerad och registrerad. Varje titel\n                \
(och ikon) är en hypertextlänk till hjälpen inom kategorin.</>           \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
Du kan ta fram en lista över tillgänglig hjälp inom en produktkategorin\n  \
genom att välja dess titel (understruken text) eller ikon.</PARAGRAPH>  \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
Inom produktkategorin väljer du sedan titeln på den hjälp\n              \
du vill se.</PARAGRAPH>                                                 \
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">        \
Om du behöver hjälp i ett hjälpfönster ska du trycka på F1.</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 Hjälp - Översta nivån

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>Välkommen till Hjälp</ABBREV>                                  \
<LINK 0 "Help4Help How-To-Register-Help">                                \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>                         \
Observera:\ \ \ Ingen hjälp finns registrerad                                            \
</SIZE></WEIGHT></TYPE></></LINK>                                        \
<PARAGRAPH leftindent 3 firstindent 3>                                   \
<WEIGHT bold>Ingen produktkategori har gjort sina hjälpfiler tillgängliga  \
för sökning.</> När du använder vissa tillämpningar kommer du ändå att   \
kunna få hjälp av hjälpfunktioner inom tillämpningarna.</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 Hjälpförteckningen skapas. Vänta.

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


