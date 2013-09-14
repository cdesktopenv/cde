$ $XConsortium: dthelpgen.msg /main/3 1995/11/07 13:13:35 rswiston $
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
1 %s -dir <directory> [-generate] [-file <name>] [-lang <language>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: Element of %s is not a directory\n
3 %s: Access denied for directory %s\nTry running as super user?\n
4 %s: Element of %s does not exist\n
5 %s: File system containing %s is full\n
6 %s: Unable to access %s - error status number %d\n
7 %s: File system containing %s is read only\n
8 %s: Requires root permission to write to %s\n
9 %s: Write to %s invalid\n
10 %s: Search Path empty\n
11 %s: 'title' resource missing\n
12 %s: 'abstract' resource missing\n
13 %s: 'volumes' resource missing\n
14 %s: 'character' set resource missing\n
15 %s: Destination directory missing\n
16 %s: Zero Family files found\n
17 %s: Zero Volume files found\n
18 %s: Unable to access current working directory - error status number %d\n
19 %s: Unable to allocate memory\n
20 %s: Invalid system language specified %s\n

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
1 C.ISO-8859-1

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 Welcome to Help Manager

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>Welcome to the Help Manager</ABBREV>                          \
<PARAGRAPH>Each of the titles listed below represents a\n               \
<ANGLE italic> product family</>\n                                      \
that has installed and registered its online help.  Each title\n        \
(and icon) is a hyperlink that lists the help within the family.</>     \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
To display a list of the help available for a product family, choose\n  \
its title (underlined text) or icon.</PARAGRAPH>                        \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
Within a product family, find the help you want to view, then\n         \
choose its title.</PARAGRAPH>                                           \
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">        \
If you need help while using help windows, press F1.</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 Help - Top Level

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>Welcome to the Help Manager</ABBREV>                           \
<LINK 0 "Help4Help How-To-Register-Help">                                \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>                         \
Note:   No Help Registered                                               \
</SIZE></WEIGHT></TYPE></></LINK>                                        \
<PARAGRAPH leftindent 3 firstindent 3>                                   \
<WEIGHT bold>No product families have registered their online help       \
files for browsing.</>  Help may be available for some applications by   \
choosing Help commands directly within the applications.</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 Generating browser information. Please wait.

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


