$ $XConsortium: dthelpgen.msg /main/3 1995/11/08 14:06:29 rswiston $
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
1 %s -dir <Æøòç> [-generate] [-file <ÇØê¢>] [-lang <ë£ËÆ>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: %s ÄæÄ÷×ÃÄâÑÒÆøòç\n
3 %s: Æøòç %s ÄæÇôÌ½Ý·ÍÜ\nÓÑÅèØíâêËðÆîÏ¯ËÎÅ¡ÇÂæÜÄ¡È¹ãÃ?\n
4 %s: %s ÄæÄ÷×ÃÄâÇôÇã\n
5 %s: ÅýÉÖ %s ÄæóòÕùË·ÜÓÄØéÈ\n
6 %s: àÒÎÎÇôÌ½ %s ¢± òãë¨ÎíèèæÀî£ %d\n
7 %s: ÅýÉÖ %s ÄæóòÕùË·ÜÓÆ·×äûôÌ½\n
8 %s: ëæÓÑÕüËðÆîÏ¯ÅèìÑÄ« %s\n
9 %s: ìÑÄ« %s Äâè×\n
10 %s: ä²ÞòæûÕÀÑÒÏ¨Îû\n
11 %s: ÅÎ×Í 'title' æñäã\n
12 %s: ÅÎ×Í 'abstract' æñäã\n
13 %s: ÅÎ×Í 'volumes' æñäã\n
14 %s: ÅÎ×Í 'character' âõæñäã\n
15 %s: ÅÎ×ÍÆøíºÆøòç\n
16 %s: ÆÜàõÜ¢Ë·ÇÄóòÕù\n
17 %s: ÆÜàõÜ¢Õ©üÕóòÕù\n
18 %s: àÒÎÎÇôÌ½ÆøÐ©ÄÖÉ¢Æøòç ¢± òãë¨ÎíèèæÀî£ %d\n
19 %s: àÒÎÎØÙåôØ´ðÙüÕ\n
20 %s: ÑÀÌùÎûË·ÜÓë£ËÆ %s Äâè×\n

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
1 zh_TW.EUC-TW

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 ûãÏÒËðÆîëºÉ·ë©Íüê§Ü¡ÔÞ

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>ûãÏÒËðÆîëºÉ·ë©Íüê§Ü¡ÔÞ</ABBREV>                          \
<PARAGRAPH>Ä¶ÇÄÇÖâúíº÷îÉáÅîÏÐ\n               \
<ANGLE italic> Ä¡ê¡ÄØÇãîÀÄ¸ëºÉ·ë©Íü</>\n                                      \
ÇøæÒËäâ¡ÅõÎûÜ¨ÐÂË·ÇÄ¡¤ Êäâúíº÷î\n        \
(êØè·çù) ÝçÑÒÇÄÅøæÚË·ÇÄÄãëºÉ·ë©ÍüÎûâ¾ÝÙá¸¡¤</>     \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
Ó¼ÓÑüÏÆüÜ¨ÐÂË·ÇÄÄãÆ«ÆîÎûëºÉ·ë©ÍüÎûÇÄÏÐ¡¢îùòÙðå\n  \
Ì§íº÷î (Ä¶íºÅÆÇó) ÍÐè·çù¡¤</PARAGRAPH>                        \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
ÇãÄ¡Ü¨ÐÂË·ÇÄÄã¡¢îùÞòÊÆÚÀÓÑóôáþÎûëºÉ·ë©Íü¡¢àÓÑ¥\n         \
òÙðåÌ§íº÷î¡¤</PARAGRAPH>                                           \
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">        \
ÇñÎªÚÀÇãËðÆîëºÉ·ë©Íüáþá§ÕëëæÓÑëºÉ·ë©Íü¡¢îùÑº F1¡¤</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 ëºÉ·ë©Íü ¢± Þ¡ìÒ

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>ûãÏÒËðÆîëºÉ·ë©Íüê§Ü¡ÔÞ</ABBREV>                                \
<LINK 0 "Help4Help How-To-Register-Help">                                \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>                         \
îùÎÃãô¡¨   ÊôÈ´ÄØâ¡ÅõÎûëºÉ·ë©Íü                                          \
</SIZE></WEIGHT></TYPE></></LINK>                                        \
<PARAGRAPH leftindent 3 firstindent 3>                                   \
<WEIGHT bold>ÊôÈ´Ü¨ÐÂË·ÇÄßäâ¡ÅõÌ§îÀÄ¸ëºÉ·ë©ÍüóòÕùËòößûª.</>              \
ëºÉ·ë©ÍüÆ«åîÆñÄ¡ËèóÜÆîá£È¢ËðÆî¡¢îùÎþÚÙÇãóÜÆîá£È¢ÄãòÙðåëºÉ·ë©ÍüÌÔÅï¡¤</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 Ü¨Æíößûªá£È¢æñÕèÄã¡¤ îùá¡Ô·¡¤

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
