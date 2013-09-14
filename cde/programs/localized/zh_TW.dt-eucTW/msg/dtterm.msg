$ $XConsortium: dtterm.msg /main/3 1995/11/08 14:08:49 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	Sep             f.separator\n\
	Å¢ßÐ\\Åü×ä\\âÉ  f.send_msg %d\
	
2 %s¡¨-S òÙâúÄâ×äÌÏ -server ÍÐ -serverid òÙâúØÙÇÙËðÆî\n
3 ÜÜê¦ñ¢

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132				ÚöÙ¯/Ì½Ö¼ 80<->132 Èç÷ËßÐ
2 -/+aw				ÚöÙ¯/Ì½Ö¼ÈÜÙ¯ßÐÈç
3 -bg color			Ó¬ßß÷íÈã
4 -/+bs				ùÃÝð/âäÚöÜÜê¦ñ¢Ó¬ßßÈ©òÙÌ½Îû÷íÈã
5 -display displayname		ÓÑÝÙÚÙÎû X ÈùÎ¤ðÂ
6 -e command args		ÓÑÙÚÈçÎûÌÔÅï
7 -fb fontset			ÜÏüÕÇóÊ°
8 -fg color			Ð©ßß÷íÈã
9 -fn fontset			ÆßÚ¦ÇóÊ°
10 -geometry geom			áþá§ÎûÄËÄÑ (ÅèÇóÄ÷Ò³ÞÌÈí) êØÈíåô
11 -help				üÏÆüÆÛÐ¬Ø¹ÕÉ
12 -/+iconic			ÚöÙ¯/ÄâÚöÙ¯è·çù
13 -/+j				ÚöÙ¯/Ì½Ö¼æüßÐÚÖÙ¯àëÓò
14 -/+kshMode			ÚöÙ¯/Ì½Ö¼ ksh í¼èè
15 -/+l				ÚöÙ¯/Ì½Ö¼ÅÊë¢
16 -lf filename			ÅÊë¢óòÇØ
17 -/+ls				ÚöÙ¯/Ì½Ö¼àôÄ« shell
18 -/+map				ÚöÙ¯/Ì½Ö¼ pty òÓÅøÎûè×óÜáþá§
19 -/+mb				ÚöÙ¯/Ì½Ö¼ù±ÒÌçÉ
20 -ms color			ÑÀíº÷íÈã
21 -n string			ÑÀÌùè·çùÇØê¢
22 -name string			ÐêÅÂñ¢èÒËó¡£è·çùêØíº÷îÇóÈë
23 -nb distance			ÑÀÌùÆ­ù±ÒÌçÉÎûâÁ÷â
24 -/+rw				ÚöÙ¯/Ì½Ö¼Å°Ç×ßÐÈç
25 -/+sb				ÚöÙ¯/Ì½Ö¼ÚÖÙ¯âÉ
26 -/+sf				ÚöÙ¯/Ì½Ö¼ SUN Åü×äõï
27 -sl number[s]			ÓÑóÃÇôÎûÚÖÙ¯Èçí°[ò­èÝí°]
28 -ti name			É¢Ò³á£È¢øüÉ±ÆîÎûÇóÈë
29 -title string			áþá§Îûíº÷îÇóÈë
30 -tm string			ÜÜê¦ñ¢í¼èèùÃõïÇóêØÇóÄ÷
31 -tn name			TERM ô¿èºüÈí°ÇØê¢
32 -/+vb				ÚöÙ¯/Ì½Ö¼áþú¼çÉ
33 -xrm resourcestring		Ì§ÅìÎûæñäãÝ½Öª
34 -C				ÅäÚÕÆºí¼èè
35 -Sxxd				ÇãÚ·úèí¼èè "ttyxx" ÎûóòÕùßÀÓÝÄ÷ "d" 
36 -Sxxx.d			ÇãÚ·úèí¼èè "ttyxxx" ÎûóòÕùßÀÓÝÄ÷ "d" 
37 End-Of-List
38 ÇñÎªÓÑÑÀÌù -e òÙâú¡¢ÆÆÆÒâüÅøÜ¢ÇãÌÔÅïÈçá¸Ê¢¡¢
39 ÉÁÐ¬ËðÆîÏ¯ÄùÌùÎû shell äÄÚöÙ¯¡¤Ð©ÓòÅûÈ´ÅûæÀ (+)
40 ÎûòÙâú¡¢Æ«ÅèÇßß¦ÄùÌùÔ«¡¤
41 End-Of-List
47 %s¡¨ÌÔÅïÈçòÙâúòãë¨ "%s"\r\n\n
48 ÆîÎÎ¡¨%s
49 \r\n   
50  [%s]
51 \r\n\nõïÄ« "%s -help" É»Æ«ÚµÌ¯ÉùðìÎûë©Íü¡¤\r\n\n
52 ÆîÎÎ¡¨\n
53 \t%s [-options ...] [-e command args]\n\n
54 òÙâúÅýÑÄ¡¨\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 Ø´ðÙüÕÄØéÈ\nÑº¡ÖíýÌù¡×É»Æ«ÛÒØæ

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
2 ÜÜê¦ñ¢ - úÀÉË

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 Ø´ðÙüÕÄØéÈ\nÑº¡ÖíýÌù¡×É»Æ«ÛÒØæ

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
4 ÜÜê¦ñ¢ - Man Page
5 ÜÜê¦ñ¢ - ëºÉ·ë©Íü

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
2 íýÌù
3 Ì½Ö¼
4 ëºÉ·ë©Íü
5 ðìüÕ
6 à¬íºÚÕÌ±
7 ÅÉãÛ
8 ÍµîÀ
9 à¬íºíµÈ¢
10 ÚöÙ¯
11 Ì½Ö¼
12 ØàøÈà¬íº
13 ØàøÈÛû (ÛÅÒð)
14 ö¡÷¯
15 blinkRate
16 ÷íÈãÚÕÌ±
17 Å°Æõ
18 ÆßÚ¦
19 áþá§Ó¬ßß
20 ÚÖÙ¯ÅÉÈ¢
21 Ì½Ö¼
22 ÚöÙ¯
23 ÆÍäìÚÖÙ¯
24 çÉôøÚÕÌ±
25 ôøÓö 
26 áþú¼
27 çÉôøùËÐÎ 
28 ÚöÙ¯
29 Ì½Ö¼
30 ù±ÒÌúÀÉË
31 ù±ÒÌâÁ÷â 
32 É»Ì«óÜÆî
33 ÓìÝÃ
34 ÜÜê¦ñ¢ - ðìüÕÎûòÙâú
$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 ÄùÌùÔ«
2 áþá§ÄËÄÑ(W)
3 W
4 -iso8859-1
5 %d öµ
6 %.1f öµ
7 %.1f öµ
8 %.2f öµ
9 ÄùÌùÔ«
10 ÇóÊ°ÄËÄÑ(F)
11 F

$   Window menu
12 âää»áþá§(N)
13 N
14 ÇÄÇÍ(P)
15 P
16 ÇÄÇÍ(r)...
17 r
18 ùÃÝð(C)
19 C
20 áþá§(W)
21 W

$   Edit menu
22 îïêû(C)
23 C
24 Ctrl osfInsert
25 Ctrl+Insert
26 ö´â²(P)
27 P
28 Shift osfInsert
29 Shift+Insert
30 î¾òÒ(E)
31 E

$   Option menu

32 Åü×äâÉ(M) 
33 M 
34 ÚÖÙ¯âÉ(S)
35 S
36 ðìüÕ(G)...
37 G
38 ÜÜê¦ñ¢(T)...
39 T
40 ÝÔÈ¢ÓìÝÃ(S)
41 S
42 àýÈ¢ÓìÝÃ(H)
43 H
44 ÓìÝÃ(R)
45 R
46 òÙâú(O)
47 O

$ Old Help menu (no longer used for CDE)

48 äÍý¨(O)
49 O
50 ÄÖÉ¢(T)
51 T
52 Ù¶ÈÓæñÕè(R)
53 R
54 ùÃÍõâúÆø(O)
55 O
56 ËðÆîëºÉ·ë©Íü(U)
57 U
59 A
60 ëºÉ·ë©Íü(H)
61 H

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 äÍý¨(v)
65 v
66 ×ÄÅ¿(I)
67 I
68 Æøòç(C)
69 C
70 ÄÖÉ¢(T)
71 T
72 Ù¶ÈÓæñÕè(R)
73 R
74 õïíõ(K)
75 K
76 ËðÆîëºÉ·ë©Íü(U)
77 U
79 A
80 ëºÉ·ë©Íü(H)
81 H
82 ùÃÍõÜÜê¦ñ¢
83 ùÃÍõÜÜê¦ñ¢

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
2 íýÌù
3 Ì½Ö¼
4 ëºÉ·ë©Íü
5 õïíõÚÕÌ±
6 ÆßÚ¦
7 óÜÆîá£È¢
8 à¬íºõïí¼èè
9 í°Ô«
10 óÜÆîá£È¢
11 ëºÉ·õïÜÚí¼èè
12 ö×Èí
13 ö×Èí/ßÐÈç
14 ßÐÈçâûÊ©
15 ÷ÕÌù
16 æØØæ÷ÕÌù
17 ËðÆîÏ¯Åü×äõï
18 ò­èÝÚÕÌ±
19 ÚöÙ¯
20 Ì½Ö¼
21 ÷ËßÐÈ© 132 Èç
22 Èçá¸Ê¢ßÐÈç
23 Å°Ç×Èçá¸Ê¢ßÐÈç
24 É»Ì«óÜÆî
25 ÓìÝÃ
26 ÜÜê¦ñ¢ - ÜÜê¦ñ¢òÙâú
