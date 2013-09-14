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
1 인쇄P...
2 P
3 닫기C
4 C
5 새 창W...
6 W
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 복사하기C
8 C
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 색인I...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 홈 주제T
12 T
13 되돌아가기B
14 B
15 기록H...
16 H
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 개요v - 도움말 사용
18 v
19 차례C
20 C
21 작업T
22 T
23 참조R
24 R
25 마우스와 키보드M
26 M
27 도움말에 관하여A
28 A
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the
$ Help Dialog.
29 파일F
30 F
31 찾기S
32 S
33 찾아가기N
34 N
35 도움말H
36 H
37 편집E
38 E
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ help dialog and quick help dialog widgets.
$ 
50 문자열 데이타를 서식화할 수 없습니다.
51 동적 문자열 데이타를 서식화할 수 없습니다.
52 온라인 설명서를 서식화할 수 없습니다. 요구한 온라인 설명서가 없거나 \
손상되었습니다.
53 문자 파일 데이타를 서식화할 수 없습니다. 요구한 문자 파일이 없거나\
손상되었습니다.
54 도움말 주제를 서식화할 수 없습니다.
55 없는 위치 ID:
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 선택된 하이퍼텍스트 링크는 이 응용프로그램에서 지원되지 않습니다.
57 이 응용프로그램은 온라인 설명서에 대한 링크를 지원하지 않습니다.
$ 
$ Messages 58 through 62  are used when bad or null help volumes or
$ locationId's are used.
$ 
58 요구한 온라인 도움말이 설치되지 않았거나 적절한 도움말 찾기 경로에 있지 않습니다. 온라인 도움말 설치에 대한 정보는 해당 제품의 설명서를 참조하십시오.\n
59 위치 ID를 지정하지 않았습니다.
60 도움말 볼륨을 지정하지 않았습니다.
61 도움말 볼륨:
62 위치 ID:
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 없는 주제 제목.
$ 
$ New messages added after early msg cat freeze; not in order
80 지정한 도움말 유형이 유효하지 않습니다.



$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 도움말 - 기록 열람기
2 열람한 도움말 볼륨:
3 닫기
4 도움말
5 도움말 파일
6 온라인 설명서
7 도움말 메세지
8 열람한 도움말 주제:
9 열람한 도움말 파일:
10 열람한 온라인 설명서:
11 열람한 도움말 메세지:


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 도움말 - 인쇄
$ 10-12: standard printer dialog labels
10 프린터:
11 인쇄 매수:
12 표지 제목:
$ 20-25: paper size label and sizes
20 용지 크기:
21 Letter 8.5 x 11 in
22 Legal 8.5 x 14 in
23 Executive 7.25 x 10.5 in
24 A4 210 x 297 mm
25 B5 182 x 257 mm
$ 30-33: help volume-specific printing options
30 인쇄할 주제
31 현재 주제
32 현재 주제와 부속 주제
33 차례와 색인
34 볼륨의 모든 주제

$ 40-42: buttons at bottom of dialog
40 인쇄
41 취소
42 도움말

$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 도움말 볼륨:
51 도움말 메세지
52 도움말 파일

$ 53 "Manual Page: "
53 온라인 설명서:
54 오류 메세지

$ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$ 60 " "
$quote "
60 ""
$quote

$ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 도움말 - 색인 찾기
2 찾기
3 현재 볼륨
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4 (%s)
5 모든 볼륨
6 선택
7 볼륨 선택...
8 보기
9 전체 색인
10 항목:
11 찾아가기
12 찾기 시작
13 찾기 계속
14 찾기 멈춤
15 닫기
16 도움말
17 볼륨 조사...  %d개 찾음
18 찾는 중... 남은 볼륨수: %d개
19 찾는 색인 항목이 없습니다.
20 전체 색인
21 "%s"을(를) 가진 항목
22 선택된 볼륨이 없습니다.
23 위 찾기 선택사항을 선택하십시오.
$ Do NOT translate messages 25-31
$quote "
$ " "
25 " "
26 "+"
27 "-"
$ "  +"
28 "  +"
$ "  -"
29 "  -"
$ 30 "       "
30 "       "
$ 31 "         "
31 "         "
$quote
$ DO translate messages 32 and above
$ kind of topic messages
32 도움말 메세지
33 도움말 파일
34 온라인 설명서
35 알 수 없는 형식
$ specialized title of Search Volume Selection dialog
40 도움말 - 볼륨 선택 찾기
$ additional messages (late addition caused this ordering)
50 찾는 색인 항목이 없습니다.  모든 볼륨을 찾아보십시오.
51 찾는 색인 항목이 없습니다.  또 다른 단어를 찾아 보십시오.
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 도움말 사용법
2 인쇄 ...
3 닫기
4 도움말 사용법은 이 응용프로그램에 대하여 제공되지 않습니다.
5 되돌아가기
6 도움말 ...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 사용법: %s...\n
2 \t-helpVolume <도움말 볼륨 파일>\n
3 \t-locationId  <ID>\n
5 \t-file  <ASCII 문자 파일>\n
6 \t-man\n
7 \t-manPage <Unix 온라인 설명서>\n\n\n
$ man page dialog
11 온라인 설명서
12 온라인 설명서:
13 온라인 설명서 보기
$ button labels
16 마침
17 닫기
18 되돌아가기
19 인쇄 ...
$ title of helpview window
20 도움말 열람기


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box
$ (e.g. Quick help close button)
2 닫기
3 내부 오류: 마우스를 잡을 수 없습니다.\nDtHelpReturnSelectedWidget가 중단되었습니다.\n
4 내부 오류: 키보드를 잡을 수 없습니다.\nDtHelpReturnSelectedWidget()가 중단되었습니다.\n
5 도움말 오류
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 일단 실행하십시오.
11 실행하지 마십시오.
12 도움말 볼륨이 명령 별명을 실행하려 했습니다.\n별명 '%s'이(가) 정의되지 않았습니다.
13 도움말 볼륨이 명령을 실행하려 했습니다.\n보안상 자동 명령 실행이 꺼져 있습니다.\n명령은 다음과 같습니다. %s
$ End Message set 8


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area
1 볼륨:
$ Messages 2 though 5 are used in the Button Box area of our general
$ help dialog.
2 되돌아가기
3 기록...
4 색인...
5 대주제 


$set 10
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 도움말 - 볼륨 선택
2 도움말 볼륨
3 하나 이상의 도움말 볼륨을 선택하십시오.
4 닫기
5 도움말


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 닫기
2 고급...
3 되돌아가기
4 인쇄 ...
5 도움말 ...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $XConsortium: DtHelp.msg /main/3 1995/11/08 13:12:26 rswiston $"

