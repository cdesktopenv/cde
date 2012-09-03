$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelp.msg
$ **
$ **  Project:     CDE 1.0 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for DtHelp 
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in dialogs or are displayed to the user.
$ **        These messages are the default and they should ALL BE LOCALIZED.
$ **        Note that these messages do NOT have any identification.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the 
$ *               27th.  (Mike Wilson).
$ *
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****
$ 
$ These Messages appear in the pulldown menus for Cde Help General Help
$ dialog widget.
$ 
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the
$ second is the mnemonic (e.g. 2 P).  
$ 
$ Messages 1 through 6 are the pulldown menus for the File menu.
1 ÇÄÇÍ(P)...
2 P
3 ùÃÝð(C)
4 C
5 âää»áþá§(W)...
6 W
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 îïêû(C)
8 C 
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 ×ÄÅ¿(I)...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 ØÄÌîÅä÷î(T)
12 T
13 Å°Ç×ä²Þò(B)
14 B
15 Ø´òç(H)...
16 H
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 äÍý¨(v) - ËðÆîëºÉ·ë©Íü
18 v
19 Æøòç(C)
20 C
21 ÄÖÉ¢(T)
22 T
23 Ù¶äù(R)
24 R
25 äìçñÌÏõïíõ(M)
26 M
27 ùÃÍõëºÉ·ë©Íü(A)
28 A
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the 
$ Help Dialog.
29 óòÕù(F)
30 F
31 ä²Þò(S)
32 S
33 Þò×ì(N)
34 N
35 ëºÉ·ë©Íü(H)
36 H
37 î¾òÒ(E)
38 E
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ helps dialog and quick help dialog widgets.
$ 
50 ÇóÈëæñÕèàÒÎÎÖªÈ¢Å§¡¤
51 Ù¯èèÈÜÈëæñÕèàÒÎÎÖªÈ¢Å§¡¤
52 îÀÄ¸ßÎÉ·Ó÷àÒÎÎÖªÈ¢Å§¡¤ÚÀÍÔëæÓÑÎûîÀÄ¸ßÎÉ·Ó÷Æ«×äÄâÇôÇã¡¢ÍÐÑÒ\
ÄØä´äÚ¡¤
53 ÅÆÇóóòÕùæñÕèàÒÎÎÖªÈ¢Å§¡¤ÚÀÍÔëæÓÑÎûîÀÄ¸ßÎÉ·Ó÷Æ«×äÄâÇôÇã¡¢ÍÐ\
ÑÒÄØä´äÚ¡¤
54 ëºÉ·ë©ÍüÅä÷îàÒÎÎÖªÈ¢Å§¡¤
55 Æ­ÇÄ ID ÈíåôÄâÇôÇã¡¨
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 ÝÕÔ¶óÜÆîá£È¢ÄâÅÅßÎÚÀÍÔòÙÌ½Îûâ¾ÅÆÇóÝÙá¸¡¤
57 ÝÕÔ¶óÜÆîá£È¢ÄâÅÅßÎÝÙá¸îÀÄ¸ßÎÉ·Ó÷Åü×ä¡¤
$ 
$ Messages 58 through 62  are used when bad or null help volumes or 
$ locationId's are used.
$ 
58 ÚÀëæÓÑÎûîÀÄ¸ëºÉ·ë©ÍüÆ«×äÊôÈ´ÇøæÒ¡¢ÍÐÆÜÈíÍõïËåµÎûä²ÞòæûÕÀÄ¸¡¤È´ùÃÇøæÒîÀÄ¸ëºÉ·ë©ÍüÎûæñØ¹¡¢îùÙ¶ÈÓÜ¨ÐÂÎûë©ÍüÅÆÇµ¡¤\n
59 ÆÜÑÀÌù ID Èíåô¡¤
60 ÆÜÑÀÌùëºÉ·ë©ÍüÕ©üÕ¡¤
61 ëºÉ·ë©ÍüÕ©üÕ¡¨
62 ID Èíåô¡¨
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 ÈºÅä÷îíº÷îÄâÇôÇã¡¤
$ 
$ $ New messages added after early msg cat freeze; not in order
80 ÑÀÌùÎûëºÉ·ë©ÍüùËÐÎàÒÕæ¡¤



$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 ëºÉ·ë©Íü - Ø´òçößûªá£È¢
2 ößûªç´ÎûëºÉ·ë©ÍüÕ©üÕ¡¨
3 ùÃÝð
4 ëºÉ·ë©Íü
5 ëºÉ·ë©ÍüóòÕù
6 îÀÄ¸ßÎÉ·Ó÷
7 ëºÉ·ë©ÍüØ¹ÕÉ
8 ößûªç´ÎûëºÉ·ë©ÍüÅä÷î¡¨
9 ößûªç´ÎûëºÉ·ë©ÍüóòÕù¡¨
10 ößûªç´ÎûîÀÄ¸ßÎÉ·Ó÷¡¨
11 ößûªç´ÎûëºÉ·ë©ÍüØ¹ÕÉÅä÷î¡¨


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 ëºÉ·ë©Íü - ÇÄÇÍ
$ 10-12: standard printer dialog labels
10 ÇÍÏÐñ¢¡¨
11 Ç¹í°¡¨
12 ÏÐó¤íº÷îÓ÷íº÷î¡¨
$ 20-25: paper size label and sizes
20 ×ËÚ¯ÄËÄÑ¡¨
21 Letter 8.5 x 11 in
22 Legal 8.5 x 14 in
23 Executive 7.25 x 10.5 in
24 A4 210 x 297 mm
25 B5 182 x 257 mm
$ 30-33: help volume-specific printing options
30 ÓÑÇÄÇÍÎûÅä÷î
31 Ü¢ÈçÅä÷î
32 Ü¢ÈçÅä÷îêØÈ¹ìÒÅä÷î
33 ÆøòçêØ×ÄÅ¿
34 Õ©üÕÄãÎûÍÔÈ´Åä÷î
$ 40-42: buttons at bottom of dialog
40 ÇÄÇÍ
41 Ì½Ö¼
42 ëºÉ·ë©Íü
$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 ëºÉ·ë©ÍüÕ©üÕ¡¨
51 ëºÉ·ë©ÍüØ¹ÕÉ¡¨
52 ëºÉ·ë©ÍüóòÕù
$ 53 "Manual Page: "
53 îÀÄ¸ßÎÉ·Ó÷¡¨
54 òãë¨Ø¹ÕÉ
$ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$ 60 " "
60 
$ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5 
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 ëºÉ·ë©Íü - ×ÄÅ¿ä²Þò
2 ä²Þò
3 Ü¢ÈçÕ©üÕ
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4  (%s)
5 ÍÔÈ´Õ©üÕ
6 òÙÌ½ÎûÕ©üÕ
7 òÙÌ½Õ©üÕ...
8 üÏÆü
9 Éùðì×ÄÅ¿
10 òÓÄ«æñÕè¡¨
11 Ð©Í»
12 âäÌîä²Þò
13 ú«úýä²Þò
14 ØôÅÏä²Þò
15 ùÃÝð
16 ëºÉ·ë©Íü
17 ÚáßÀÕ©üÕ... ÊÆÌ¯ %d Ô¶
18 ä²ÞòÄã... Þ·ïõÕ©üÕ¡¨%d Ô¶
19 ÊÆÄâÌ¯×ÄÅ¿âúÆø
20 Éùðì×ÄÅ¿
21 òÓÄ«æñÕè \"%s\"
22 ÆÜòÙÌ½Õ©üÕ
23 Ú·Ä¸ÅÉòÙÌ½ä²ÞòòÙâú
$ Do NOT translate messages 25-31
$ " "
25  
26 +
27 -
$ "  +"
28  +
$ "  -"
29  -
$ 30 "       "
30        
$ 31 "         "
31          
$ DO translate messages 32 and above
$ kind of topic messages
32 Help Message
33 ëºÉ·ë©ÍüóòÕù
34 ßÎÉ·ë©ÍüÓ÷
35 ÄâÍüÎûÖªÈ¢
$ specialized title of Search Volume Selection dialog
40 ëºÉ·ë©Íü - ä²ÞòÕ©üÕòÙðå
$ additional messages (late addition caused this ordering)
50 ÊÆÄâÌ¯×ÄÅ¿âúÆø¡¤îùè©æÜä²ÞòÍÔÈ´Õ©üÕ¡¤
51 ÊÆÄâÌ¯×ÄÅ¿âúÆø¡¤îùè©æÜËðÆîÌ§ÅìùÃõïÇó¡¤
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 È´ùÃëºÉ·ë©ÍüÎûëºÉ·ë©Íü 
2 ÇÄÇÍ ...
3 ùÃÝð
4 ÝÕÔ¶óÜÆîá£È¢ÄâÅÅßÎ¡ÖÈ´ùÃëºÉ·ë©ÍüÎûëºÉ·ë©Íü¡×Åü×ä!
5 Å°Ç×ä²Þò
6 ëºÉ·ë©Íü ...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be 
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 ÆîÎÎ¡¨%s...\n
2 \t-helpVolume <Help Volume File>\n
3 \t-locationId  <ID>\n
5 \t-file  <ASCII Text File>\n
6 \t-man\n
7 \t-manPage <Unix Man Page>\n\n\n
$ man page dialog
11 îÀÄ¸ßÎÉ·Ó÷
12 îÀÄ¸ßÎÉ·Ó÷¡¨
13 üÏÆüîÀÄ¸ßÎÉ·Ó÷
$ button labels
16 á¸ÊÖ
17 ùÃÝð
18 Å°Ç×ä²Þò
19 ÇÄÇÍ ...
$ title of helpview window
20 ëºÉ·ë©ÍüèÖÒàðÂ


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box 
$ (e.g. Quick help close button)
2 ùÃÝð
3 ÄùÝåòãë¨¡¨àÒÎÎÊÎÌ½äìçñ\nDtHelpReturnSelectedWidget ÄãöË¡¤\n
4 ÄùÝåòãë¨¡¨àÒÎÎÊÎÌ½õïíõ\nDtHelpReturnSelectedWidget() ÄãöË¡¤\n
5 ëºÉ·ë©Íüòãë¨ 
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 Äó÷®ÙÚÈç
11 ÄâÓÑÙÚÈç
12 ëºÉ·ë©ÍüÕ©üÕã÷ÙÚÈçÄ¡Ô¶ÌÔÅïÉ±ÇØ¡¤\nÉ±ÇØ '%s' ÆÜÌùåø¡¤
13 ëºÉ·ë©ÍüÕ©üÕã÷ÙÚÈçÄ¡Ô¶ÌÔÅïÉ±ÇØ¡¤\nÒ³Ä§ÇøÇÀááã÷¡¢ÈÜÙ¯ÙÚÈçÌÔÅïÎûÅü×äÝ·ùÃÝð¡¤\nÌÔÅïÒ³¡¨ %s
$ End Message set 8


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area  
1 Õ©üÕ¡¨
$ Messages 2 though 5 are used in the Button Box area of our general 
$ help dialog.
2 Å°Ç×ä²Þò
3 Ø´òç...
4 ×ÄÅ¿...
5 Þ¡ìÒ


$set 10 
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 ëºÉ·ë©Íü - Õ©üÕòÙðå
2 ëºÉ·ë©ÍüÕ©üÕ
3 îùòÙðåÄ¡Ô¶ÍÐÇéÔ¶ëºÉ·ë©ÍüÕ©üÕ
4 ùÃÝð
5 ëºÉ·ë©Íü


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 ùÃÝð
2 ÊÕÇé ...
3 Å°Ç×ä²Þò
4 ÇÄÇÍ ...
5 ëºÉ·ë©Íü ...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)Message catalog source $XConsortium: DtHelp.msg /main/3 1995/11/08 14:03:11 rswiston $

