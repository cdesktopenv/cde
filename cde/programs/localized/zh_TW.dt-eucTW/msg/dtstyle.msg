 
$ "$XConsortium: dtstyle.msg /main/4 1995/12/08 09:50:58 rswiston $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtstyle.msg
$ **
$ **  Project:     DT Style Manager
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtstyle
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.  
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$ Message 1 from set 2 was deleted from the message catalog

$ _DtMessage 2 is the title of all Information dialog boxes
2 Ý×ÉË
$ _DtMessage 3 is the title of all Error dialog boxes
3 òãë¨
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 Cannot communicate with the session\n\
manager... Exiting.
 
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 Style Manager is already running,\n\
second attempt aborted.
6 An X Toolkit error occurred...\n\
Exiting.

$ _DtMessage 7 is the title of all Help dialog boxes
7 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ëºÉ·ë©Íü

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 ÚÀÆÒâüÇãÆÀÐÎÈ¢íµê§Ü¡Ï¯\n\
ÄãòÙðåÄ¡Ô¶âúÆø¡¤

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 Could not obtain screen saver information\n\
from the session manager. Start up settings\n\
may be incorrect.


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 ÆÀÐÎÈ¢íµê§Ü¡Ï¯

$ mnemonic and label for the File menubar item.
2 F
3 óòÕù(F)
$ mnemonic and label for the Help menubar item.
4 H
5 ëºÉ·ë©Íü(H)

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 á¸ÊÖ

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 ÷íÈã
28 ÇóÊ°
29 Ó¬ßßÌùÈí
30 õïíõ
31 äìçñ
32 èµôø
33 ò­èÝ
35 ÚöÙ¯

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 No Dtstyle restore information available for current session

$ _DtMessage 37 is the label on the dtstyle icon
37 ÆÀÐÎÈ¢íµ 

$ Message 38 is the label on the Window button in the dtstyle main window.
38 áþá§

$ _DtMessage 40 is replaced with message 53
$ 40 E

$ *
$ * mnemonic and label for the Help menu items.
$ * 

$ Messages 41-42 have been replaced with messages 54-55
$ Overview... menu item
$ 41 I
$ 42 Introduction...

$ Tasks... menu item 
43 T
$ Message 44 has been replaced with messages 56
$ 44 Tasks...

$ Reference... menu item
$ 45 R
$ Message 46 has been replaced with messages 57
$ 46 Reference...

$ On Item menu item
$ Keybaord menu item
47 O
48 ùÃÍõâúÆø(O)

$ Using Help... menu item
49 U
$ Message 50 has been replaced with messages 58
$ 50 Using Help... 

$ Messages 51-52 have been replaced with messages 59-60
$ Overview menu item
$ 51 V
$ 52 Version...

$ Message 53 is replaced with message 40
53 x

$ Messages 54-55 are replacements for messages 41-42
54 v
55 äÍý¨(v)

$ Message 56 is a replacement for messages 44
56 ÄÖÉ¢(T)

$ Message 57 is a replacement for messages 46
57 Ù¶ÈÓ(R)

$ Message 58 is a replacement for messages 50
58 ËðÆîëºÉ·ë©Íü(U)

$ Messages 59-60 are replacements for messages 51-52
59 A
60 ùÃÍõÆÀÐÎÈ¢íµê§Ü¡Ï¯(A)

$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 çßûª

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 ÇóÊ°ÎûÊÑüÈÇãÄ¶È¹ÚöÙ¯ÑÜËèóÜÆîá£\n\
È¢ÕëäÄüÏÜ¢ÅøËô¡¤Ì§ïõÎûóÜÆîá£È¢¡¢\n\
ËóÇñóòÕùê§Ü¡Ï¯ÌÏóÜÆîá£È¢ê§Ü¡Ï¯¡¢\n\
ØæÏæÚÀá¸ÊÖÖ¤Ä¸ê§Ü¡Ë·ÜÓËäÇÂÓìä»àô\n\
Ä«¡¢ÉÁÐ¬ÄâäÄÇ¡É»Å°ÑÐÅøËô¡¤

$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 Óìä»ÚöÙ¯ÝÕÔ¶âêÑôÉ¢äÆÄæÑ¥¡¢\n\
òÙÌùÎûÇóÊ°ÞóäÄÝ·ËðÆî¡¤

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 AaBbCcDdEeFfGg0123456789
21 AaBbCcDdEeFfGg0123456789

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 ÄËÄÑ

$ _DtMessage 23 is the title of the Fonts dialog
23 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ÇóÊ°

$ _DtMessage 24 is an informative message used by the Fonts dialog 

24 ÄËÄÑÇÄÏÐÄãÊôÈ´ÚÀÆøÐ©Ö¤ÓòÄ¸ËðÆî\n\
ÎûÇóÐÎ. ÇñÎªòÙÌ½ËäóÜÆîä»ÇóÐÎ,\n\
É»ËðÆîÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ÇóÐÎè×æã,\n\
Ä¾àÒÎÎÇßÌ¯ÆøÐ©ÎûÇóÐÎ.

$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 ÄùÌùÔ«
3 ÓöâØ
4 Óöîþ
5 ßæâæ

$ _DtMessage 7 is the title of the Beep dialog box
7 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - èµôø  


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 ÇãàôÄ«Õë¡¨
4 ú«úýÜ¢ÈçâêÑôÉ¢äÆ
5 ÏÓÇßØÄÌîâêÑôÉ¢äÆ
6 àôÅøÕëææÙÂ
7 àôÅøíýë¥è×æã¡¨
8 âäÚö
9 ùÃÝð
10 ÝÃÌùØÄÌîâêÑôÉ¢äÆ...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 ÇñÈºÚÀÎûØÄÌîâêÑôÉ¢äÆÙòäÄÅè\n\
Ü¢ÈçâêÑôÉ¢äÆÌ½Åî¡¤\n\n\
                 ÓÑú«úýãÃ?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 úÀÉË

$ _DtMessage 13 is an error message
13 ÝÕÔ¶âêÑôÉ¢äÆÑÒÆñÄ¡Ô¶ X áþá§Ë·ÜÓ\n\
ÚöÙ¯ÌÔÅïâõÍÔÚöÙ¯¡¤ÈþÚöÙ¯ÝÃÌùàÒÕæ¡¢\n\
ÇÞÒ³ÝÕÔ¶âêÑôÉ¢äÆàÒÎÎÇßß¦¡¤

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ÚöÙ¯

$ _DtMessage 15 is an error message
15 âêÑôÉ¢äÆê§Ü¡Ï¯ÊôÈ´ÇãÙÚÈç¡¤\n\
ÚöÙ¯ÝÃÌùàÒÕæ¡¤


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
1 ÆÊ
2 Æ­
3 ÆîÅÃÜäèë¡¨
4 ÑºâÚ 2¡¨
$ _DtMessage 5 is the label of default button.
5 ÄùÌùÔ«

$ Messages 6,8,9 are labels of the scales.
6 ÝÙÑºÌ¥Ä¶
8 ÅûÝÚ
9 õ§ÒÌÔ«

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 òãë¨¡¨ÝÃÌùÑÀíºè×óÜ\n

11 ã®ØÊ
12 îþðì

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 ÚÀÑºÄ¡Ä¶¡ÖíýÌù¡×ÄæÑ¥¡¢Æ­ù±êØÆÊù±\n\
äìúèÑºâÚÎûÅü×äÇ¡É»Ç¨ßÐ¡¤Ú·äìçñáþá§\n\
¡ÖàÁæÜÝÙÑºÌ¥Ä¶¡×è·Ê°ÎûÑºâÚ 1 êØÑºâÚ 3\n\
ÎûÈíåô¡¢É»Æ«ÚµÏ¡ÆøÐ©ÆîÅÃÜäèëÎûÝÃÌù¡¤\n\

$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 ÇñÎªÚÀÑºÄ¡Ä¶¡ÖíýÌù¡×¡¢ÇãÄ¶Ä¡Ô¶âêÑôÉ¢äÆÄã¡¢\n\
äìçñÑºâÚ 2 ÞóäÄÝ·ÝÃÌùÈ©¡Ö÷ËßÐ¡×(ÍîÈ³)¡¤

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 óÜÆîá£È¢Óìä»ÚöÙ¯ÄæÑ¥¡¢ä»ÎûÝÙÑº\n\
Ì¥Ä¶ÝÚÐùÞóäÄÆíÕæ¡¤

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 ÇãÄ¶Ä¡Ô¶âêÑôÉ¢äÆÄã¡¢ä»ÎûÝÙÑºÌ¥Ä¶ÝÚÐù\n\
É»Æ«ÆíÕæ¡¤

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 àÁæÜ\nÝÙÑºÌ¥Ä¶

$ _DtMessage 18 is the Mouse Dialog title
18 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - äìçñ

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 ÇñÎªÚÀÑºÄ¡Ä¶¡ÖíýÌù¡×¡¢ÇãÄ¶Ä¡Ô¶âêÑôÉ¢\n\
äÆÄã¡¢äìçñÑºâÚ 2 ÞóäÄÝ·ÝÃÌùÈ©¡Öîþðì¡×¡¤\n\
ÑºâÚ 2 Æ«ÅèÆîËôîþðìÅÆÇóêØÇÄÆüòÙðå¡¤ÅÆÇó\n\
êØÇÄÆüÎûÎûã®ØÊÙ¯É¢ (ÍîÈ³) Ð¬âüËðÆîÑºâÚ 1¡¤\n\

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 óôÑçóòÕùÝÂÆ«ûâ¡¤
3 %s ÑÒÆîËôóÃÇôÚÀÎûâêÑôÉ¢äÆÎûóòÕù\n


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 àÒÎÎâäÚöÆøòç %s¡¤
$ _DtMessage 2 - error opening the backdrop files
2 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ÎûÓ¬ßßÌùÈíÝåÅ¡\n\
àÒÎÎçªÉ¢¡¢ÇÞÒ³ÊôÈ´Ó¬ßßÌùÈíóòÆ«\n\
Æî¡¤îùóôÑç $HOME/.dt/errorlog¡¤


$ _DtMessage 12 is the title of the Backdrop dialog box
12 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - Ó¬ßßÌùÈí


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box. 
1 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ò­èÝ

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 ÄùÌùÔ«
4 ò­èÝÒÜçÙæÒåô
5 ÚöÙ¯ÒÜçÙæÒåô
6 ÇÖÉ±ÒÜçÙæÒåôÕëâæÝÃÌù
7 Å¡úÌ
8 ò­èÝ÷ÕÌù
$ Message 9 has been removed 
10 âäÚö
11 ùÃÝð
12 ò­èÝÒÜçÙæÒåô¡¨
13 ò­èÝ÷ÕÌù¡¨
14 ÚöÙ¯÷ÕÌù
15 ò­ìçÈÜÙ¯ØôäØ
16 ò­ìçÈÜÙ¯ØôäØá£È¢
17 ÚöÙ¯ò­ìçØôäØá£È¢
18 Ð©ÅÉÓòÎ²÷ÕÌù
$ Message 19 has been replaced by message 21
19 ËðÆî÷ÕÌùÓ¬ßß
20 ÇÖÉ±Ó¬ßßÕëâæÝÃÌù
$ Message 21 is a replacement for message 19
21 ËðÆîÓ¬ßß÷ÕÌù
22 ÝàÍü÷ÕÌù

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 ÄùÌùÔ«
3 ÈÜÙ¯Óìîï
4 ÑºõïÓöâØ

$ _DtMessage 6 is the title of the Keyboard dialog box
6 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - õïíõ


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 îþÈãíõ
6 ìÁÅû...
7 É´Øæ...
8 ÔºÊÑ...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 ä»ÎûîþÈãíõÇØê¢¡¨

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 ìÁÅûîþÈãíõ

$ Messages 12 through 14 are error messages.
12 îþÈãíõÇØê¢Äâ×äÅýÉÖ\n\
Ä¶ÇÄÇóÄ÷¡¨\n\n\
<space> * : ( ) [ ] { } < > ! | \" / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE 
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 îþÈãíõÇØê¢ÆÒâüÇã 10 Ô¶ÇóÄ÷ÅèÄù¡¤\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 É´ØæîþÈãíõ

$ _DtMessage 18 appears in an information dialog.
18 àÒÎÎÉ´ØæÞ²Ñ¥Ä¡Ô¶îþÈãíõ¡¤\n

$ _DtMessage 21 is an error message
21 úÀÉË

$ _DtMessage 23 appears in an information dialog
23 ÓÑÉ´ØæîþÈãíõ '%s' ãÃ?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 ÆÀÐÎÈ¢íµê§Ü¡Ï¯Îû÷íÈãÝåÅ¡àÒÎÎ\n\
çªÉ¢¡¢ÇÞÒ³÷íÈãÈùÎ¤ðÂÊôÈ´ÇãÙÚÈç¡¤\n\
îùóôÑç $HOME/.dt/errorlog¡¤

$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 ÆÀÐÎÈ¢íµê§Ü¡Ï¯Îû÷íÈãÝåÅ¡àÒÎÎ\n\
çªÉ¢¡¢ÇÞÒ³æñäã 'useColorObj' ÝÃÌùÒ³\n\
False¡¤

$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 ä»Îû¡Ö÷íÈãËðÆî¡×Ô«ÇãÚÀÄ¶Ä¡Ô¶âêÑô\n\
É¢äÆÕëÉ»äÄÆíÕæ¡¤

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 òÙÌ½ÎûîþÈãÎ²ÇãÚÀÄ¶Ä¡Ô¶âêÑô\n\
É¢äÆÕëÉ»äÄÆíÕæ¡¤

$ _DtMessage 29 appears in a warning dialog
29 ÇØÒ³ '%s' ÎûîþÈãíõÄØåîÇôÇã¡¤\n\
ÝÕÔ¶ä»îþÈãíõÙòäÄÓììÑ÷®îþÈãíõ¡¤\n\
ÚÀã÷ÝÕíµØøãÃ?

$ _DtMessage 30 is the title of the Color dialog box.
30 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ÷íÈã

$ _DtMessage 31-33 are the ColorUse options
$ 31 High Color
$ 32 Medium Color
$ 33 Low Color
34 ã©Æõ
35 ÄùÌùÔ«

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT Color Use

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog 
$ 37 DT Color Use...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 ÆÀÐÎÈ¢íµê§Ü¡Ï¯Îû÷íÈãÝåÅ¡àÒÎÎ\n\
çªÉ¢¡¢ÇÞÒ³ÊôÈ´îþÈãíõóòÕùÆ«Æî¡¤\n\
îùóôÑç $HOME/.dt/errorlog¡¤

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39 ÓÑËðÆîÎû÷íÈãí°Æø¡¨

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 ÷íÈãí°Æø...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 ÊÕÇéÎûÖ¤Ä¸ê§Ü¡Ë·ÜÓ÷íÈã
42 ÊÕÇéÎûóÜÆîá£È¢÷íÈã
43 ÊÕÇéÎûóÜÆîá£È¢÷íÈã

$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 àÒÎÎÉ´Øæ '%s'¡¤\n
12 îùóôÑç $HOME/.dt/errorlog¡¢ÚÀÆ«Ú·ÄãÚµÌ¯ßÈÆü¡¤

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 Warning, Too many directories listed in the resource paletteDirectories,\n\
Maximum number is %d. 
$ _DtMessage 19 - could not open a palette directory
19 Could not open directory %s. 
$ _DtMessage 20 - error opening the palette file
20 Could not open %s. 
$ _DtMessage 21 - error reading a palette file
21 %s is an invalid palette file.


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 ÷íÈãî¯Ëó
5 ÷®
6 ä»
7 ÊÎÌ½÷íÈã
8 ÷íÈãî¾òÒðÂ
9 ÈãÐù
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 Òø
11 êº
12 ÷±

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 Warning, couldn't grab pointer.\n
15 Warning, couldn't grab Keyboard.\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - ÔºÊÑ÷íÈã


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 ÆÀÐÎÈ¢íµê§Ü¡Ï¯ - áþá§

$ Messages 2 through 5 are labels on the Window dialog.
2 ÄùÌùÔ«
3 áþá§ÈçÒ³
4 áþá§è·çù
$ Messages 5-8 have been replaced by messages 13-16 
$ 5 Focus Follows Mouse
$ 6 Click In Window For Focus
$ 7 Raise Window With Focus
$ 8 Opaque Move
9 ËðÆîè·çùÅÉÕú
10 åôÍõÄÖÉ¢Ï¨âæÄã
11 ÄøÝÂÅäÓÑáþá§åôÍõÞ¡ìÒ
12 ÄÖÉ¢Ï¨âæê§Ü¡Ï¯ÆÒâüÓìä»ÚöÙ¯¡¢ÚÀ\n\
ÍÔÉ¢ÎûüÈÊÕÄßäÄÆíÕæ¡¤Ñº¡ÖíýÌù¡×Æ«Åè\n\
Óìä»ÚöÙ¯ÄÖÉ¢Ï¨âæê§Ü¡Ï¯¡¢Ñº¡ÖÌ½Ö¼¡×\n\
Ð¬Æ«ÓìÝÃÝÕËèüÈÊÕ¡¤
$ Messages 13-16 are replacements for messages 5-8
13 îùÑÀÌ¯áþá§ÄãËðÌ§È©Ò³Ü¢Èçáþá§
14 îùÑºÄ¡Ä¶áþá§ËðÌ§È©Ò³Ü¢Èçáþá§
15 È©Ò³Ü¢Èçáþá§Ñ¥ßÈÈÝÐ©ê¦
16 ÜÃÙ¯ÕëüÏÆüÄùÕ©

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/4 1995/12/08 09:50:58 rswiston $



