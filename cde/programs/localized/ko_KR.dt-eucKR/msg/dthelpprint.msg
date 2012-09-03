$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 13:16:52 rswiston $
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
1 "%s 오류: helpType은 도움말 볼륨이지만, helpVolume을 지정하지 않았습니다.\n"
2 "%s 오류: 도움말 볼륨 %s을(를) 찾을 수 없습니다.\n"
3 "%s 오류: 도움말 볼륨 %s을(를) 처리중에 문제가 발생했습니다.\n"
4 "%s 오류: 메모리 할당이 실패했습니다.\n"
5 "%s 오류: 주제 정보를 가져올 수 없습니다.\n볼륨 %s, locationId %s\n"
6 "%s 오류: 임시 파일 %s을(를) 열 수 없습니다.\n"
$ String used to replace $SECTNUM when printing the index section
10 "색인"
$ String used to replace $SECTNUM when printing the table of contents section
11 "차례"
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
1 "%s 오류: helpType은 문자열이지만, stringData를 지정하지 않았습니다.\n"
2 "%s 오류: helpType은 동적 문자열이지만 stringData를 지정하지 않았습니다.\n"
3 "%s 오류: helpType은 온라인 설명서이지만, manPage를 지정하지 않았습니다.\n"
4 "%s 오류: helpType은 파일이지만, helpFile을 지정하지 않았습니다.\n"
5 "%s 오류: 메모리 할당이 실패했습니다.\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint - 도움말에 대한 인쇄 프로그램\n\n"
2 "사용법: dthelpprint [options]\n"
3 "인쇄 방법을 제어하는 선택사항들:\n"
4 "\t-printer printername  사용할 프린터\n"
5 "\t-copies number        인쇄매수\n"
6 "\t-outputFile filename  출력을 써넣을 파일\n"
7 "\t-paperSize size       내용을 서식화할 용지 크기\n"
$ Do NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx
8 "\tsize = { help_papersize_letter|help_papersize_legal|\n"
9 "\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display displayname  자원을 가져올 디스플레이\n"
11 "\t-name                 자원을 가져올 때 사용한 프로그램 이름\n"
12 "\t-class                자원을 가져올 때 사용한 클래스 이름\n"
13 "\t-xrm resourcestring   추가 자원\n"
14 "인쇄할 내용을 제어하는 선택사항들:\n"
15 "\t-helpType type        도움말 데이타의 유형\n"
16 "\ttype = 0 (도움말 볼륨), 1 (문자열), 2 (온라인 설명서), 3 (도움말 파일)\n"
17 "\t-helpVolume volume    도움말 볼륨 파일의 전체 경로\n"
18 "\t-locationId location  해당 볼륨에 있는 도움말 주제의 이름\n"
19 "\t-all                  도움말 볼륨에 있는 모든 주제, 차례, 색인을 인쇄\n"
20 "\t-sub                  주제 locationId와 모든 부속주제를 인쇄\n"
21 "\t-one                  주제 locationId를 인쇄\n"
22 "\t-toc                  도움말 볼륨 차례를 인쇄\n"
23 "\t-index                도움말 볼륨 색인을 인쇄\n"
24 "\t-frontMatter          도움말 볼륨에 관한 사항을 먼저 인쇄\n"
25 "\t-manPage manpagename  온라인 설명서의 이름\n"
26 "\t-stringData string    인쇄할 도움말 문장\n"
27 "\t-helpFile filename    도움말 문장을 담은 파일\n"
28 "\t-jobTitle title       인쇄 작업에 대한 제목 문자열\n"
29 "\t-topicTitle title     도움말 문장에 대한 제목 문자열\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s 경고: 용지 크기 '%s'이(가) 잘못되었습니다.\nhelp_papersize_letter를 사용합니다.\n"
41 "%s 경고: 용지 크기, 높이 또는 폭 값이 빠졌습니다.\nhelp_papersize_letter를 사용합니다.\n"
42 "%s 경고: 디스플레이 %s을(를) 열 수 없습니다.\n"
45 "오류: 임시 파일에 메모리를 할당할 수 없습니다.\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s 오류: 잘못된 helpType %d\n"

