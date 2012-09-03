$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 14:06:41 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelpprint.msg
$ **
$ **  Project:     Cde1 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthelpprint
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.
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

$set 2
$ *****  Module: PrintUtil.c *****
$ 

$set 3
$ *****  Module: PrintTopics.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s òãë¨¡¨ëºÉ·ë©ÍüùËÉ±ÑÒÅä÷î¡¢ÈþÆÜÑÀÌùëºÉ·ë©ÍüÕ©üÕ\n"
2 "%s òãë¨¡¨àÒÎÎÌùÈíëºÉ·ë©ÍüÕ©üÕ %s\n"
3 "%s òãë¨¡¨ÙÂ÷îÝ¨Ü¡ëºÉ·ë©ÍüÕ©üÕ %s\n"
4 "%s òãë¨¡¨Ø´ðÙüÕØÙåôÆÂÚõ\n"
5 "%s òãë¨¡¨àÒÎÎÌ½ÚµÅä÷îæñØ¹¡¨\nÕ©üÕ %s¡¢Id Èíåô %s\n"
6 "%s òãë¨¡¨àÒÎÎâäÚöí²ÕëÍÌóòÕù %s\n"
$ String used to replace $SECTNUM when printing the index section
10 "×ÄÅ¿"
$ String used to replace $SECTNUM when printing the table of contents section
11 "ÄùÕ©ÏÐÖª"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ index entry, page number
20 "%s, %d\n"
$ index subentry, page number
21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint
30 "__GENERATED-INDEX"
$ string used by can't get a topic title associated with an index subentry
31 ""

$set 4
$ *****  Module: PrintManStrFile.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s òãë¨¡¨ëºÉ·ë©ÍüùËÉ±ÑÒÇóÈë¡¢ÈþÆÜÑÀÌùÇóÈëæñÕè¡¤\n"
2 "%s òãë¨¡¨ëºÉ·ë©ÍüùËÉ±ÑÒÙ¯èèÇóÈë¡¢ÈþÆÜÑÀÌùÇóÈëæñÕè¡¤\n"
3 "%s òãë¨¡¨ëºÉ·ë©ÍüùËÉ±ÑÒîÀÄ¸ßÎÉ·Ó÷¡¢ÈþÆÜÑÀÌùîÀÄ¸ßÎÉ·Ó÷¡¤\n"
4 "%s òãë¨¡¨ëºÉ·ë©ÍüÑÒóòÕù¡¢ÈþÆÜÑÀÌùëºÉ·ë©ÍüóòÕù¡¤\n"
5 "%s òãë¨¡¨Ø´ðÙüÕØÙåôÆÂÚõ¡¤\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint ¢± ëºÉ·ë©ÍüÇÄÇÍá£È¢\n\n"
2 "ËðÆîÅÉÎÎ¡¨ dthelpprint [options]\n"
3 "ÚÕÌ±ÇñÈôÇÄÇÍÎûòÙâú¡¨\n"
4 "\t-printer printername  ÛÃËðÆîÄæÇÍÏÐñ¢\n"
5 "\t-copies number        ÛÃÇÄÇÍÄæóòÕùí°Æø\n"
6 "\t-outputFile filename  ÙòòÓÅøìÑÄ«ÆÛóòÕù\n"
7 "\t-paperSize size       ØÜè×Å¡Ó÷ðÂÄËÄÑÄæÖªÈ¢ÄùÕ©\n"
$ DO NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx
8 "\t\tsize = { help_papersize_letter|help_papersize_legal|\n"
9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display displayname  üÏÆüÇóÈôÝ¨Ì½Úµæñäã\n"
11 "\t-name                 Ì½ÚµæñäãÕëËðÆîÎûá£È¢ÇØê¢\n"
12 "\t-class                Ì½ÚµæñäãÕëËðÆîÎûá«×ÈÇØê¢\n"
13 "\t-xrm resourcestring   ÷ìÆÀÎûæñäã\n"
14 "ÚÕÌ±ÛÃÇÄÇÍÄæÎìÎûòÙâú¡¨\n"
15 "\t-helpType type        ëºÉ·ë©ÍüæñÕèÎûùËÉ±\n"
16 "\t\ttype = 0 (help volume), 1 (string), 2 (man page), 3 (help file)\n"
17 "\t-helpVolume volume    ëºÉ·ë©ÍüÕ©üÕóòÕùÎûÉùðìæûÕÀ\n"
18 "\t-locationId location  Õ©üÕÄãëºÉ·ë©ÍüÅä÷îÎûÇØê¢\n"
19 "\t-all                  ÇÄÇÍëºÉ·ë©ÍüÕ©üÕÄãÍÔÈ´Åä÷î¡¢Æøòç¡¢ÌÏ×ÄÅ¿\n"
20 "\t-sub                  ÇÄÇÍÅä÷î Id ÈíåôêØÇÀÝåÄÍ÷î\n"
21 "\t-one                  ÇÄÇÍÅä÷î Id Èíåô\n"
22 "\t-toc                  ÇÄÇÍëºÉ·ë©ÍüÕ©üÕÆøòç\n"
23 "\t-index                ÇÄÇÍëºÉ·ë©ÍüÕ©üÕ×ÄÅ¿\n"
24 "\t-frontMatter          ÇÄÇÍëºÉ·ë©ÍüÕ©üÕÐ©ÅÉËçâú\n"
25 "\t-manPage manpagename  îÀÄ¸ÅÅßÎÓ÷ÎûÇØê¢\n"
26 "\t-stringData string    ÛÃÇÄÇÍÎûëºÉ·ë©ÍüÆÛÅÆ\n"
27 "\t-helpFile filename    ÅýÉÖëºÉ·ë©ÍüÆÛÅÆÎûóòÕù\n"
28 "\t-jobTitle title       ÇÄÇÍÄÖÉ¢Äæíº÷îÇóÈë\n"
29 "\t-topicTitle title     ëºÉ·ë©ÍüÆÛÅÆÄæíº÷îÇóÈë\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s úÀÉË¡¨ÄâÇÙÎÎÎû×ËÚ¯ÄËÄÑ '%s'¡¤ËðÆî help_papersize_letter¡¤\n"
41 "%s úÀÉË¡¨×ÍÅ·×ËÚ¯ÄËÄÑ¡£ØíÐùÍÐìÏÐùÔ«¡¤ËðÆî help_papersize_letter¡¤\n"
42 "%s úÀÉË¡¨àÒÎÎâäÚöüÏÆü %s\n"
45 "òãë¨¡¨àÒÎÎÒ³í²ÕëÍÌóòÕùØÙåôØ´ðÙüÕ¡¤\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s òãë¨¡¨ëºÉ·ë©ÍüùËÉ± %d Äâè×.\n"



