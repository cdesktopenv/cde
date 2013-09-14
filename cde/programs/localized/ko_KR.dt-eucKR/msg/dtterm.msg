$ $XConsortium: dtterm.msg /main/3 1995/11/08 13:19:01 rswiston $
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
       메뉴\\ 막대\\ 토글  f.send_msg %d\

2 %s: -S 선택사항을 -server 또는 -serverid 선택사항과 함께 사용할 수 없습니다.\n

$set 2
$ ****** Module: DtTermSyntax.c
$ These are the usage messages.
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132               80<->132칸 프린터 제어 코드 사용가능/사용불가
2 -/+aw                자동 줄내리기 사용가능/사용불가
3 -bg color            배경색
4 -/+bs                단말기 배경색 선택 켜기/끄기
5 -display displayname 접속할 X 서버
6 -e command args      실행할 명령
7 -fb fontset          굵은체 문자 글꼴
8 -fg color            전경색
9 -fn fontset          보통체 문자 글꼴
10 -geometry geom      창의 (문자 크기)와 위치
11 -help               이 메세지를 출력합니다.
12 -/+iconic           아이콘화 시작/시작하지 않음
13 -/+j                점프 이동 사용가능/사용불가
14 -/+kshMode          ksh 모드 사용가능/사용불가
15 -/+l                기록 사용가능/사용불가
16 -lf filename        파일 이름 기록
17 -/+ls               로그인 쉘 사용가능/사용불가
18 -/+map              pty 출력에서의 대응 창 사용가능/사용불가
19 -/+mb               여백 벨 사용가능/사용불가
20 -ms color           가리키개 색상
21 -n string           아이콘 이름 지정
22 -name string        클라이언트 예, 아이콘, 그리고 제목 문자열
23 -nb distance        오른쪽 여백 벨의 거리 지정
24 -/+rw               거꾸로 줄내리기 사용가능/사용불가
25 -/+sb               이동 막대 사용가능/사용불가
26 -/+sf               SUN 기능 키 사용가능/사용불가
27 -sl number[s]       저장할 이동된 줄 수 [screens]
28 -ti name            프로그램적 구별에 사용되는 문자열
29 -title string       창의 제목 문자열
30 -tm string          단말기 모드 키워드와 문자
31 -tn name            TERM 환경 변수 이름
32 -/+vb               시각 벨 사용가능/사용불가
33 -xrm resourcestring 추가 자원 사양
34 -C                  콘솔 모드
35 -Sxxd               "ttyxx" 파일 서술어 "d"에서의 슬레이브 모드
36 -Sxxx.d             "ttyxxx" 파일 서술어 "d"에서의 슬레이브 모드
37 End-Of-List
38 -e 선택사항은 반드시 명령 행의 끝에 나타나야 합니다.
39 그렇지 않으면 사용자의 기본 쉘이 시작됩니다. 플러스 부호(+)로 시작하는
40 선택사항은 기본값을 복원합니다.
41 End-Of-List
47 %s:  잘못된 명령 행 선택사항 "%s"\r\n\n
48 사용법:  %s
49 \r\n
50  [%s]
51 \r\n\n완전한 설명을 보려면 "%s -help"를 입력하십시오.\r\n\n
52 사용법:\n
53 \t%s [-options ...] [-e command args]\n\n
54 여기서, 선택사항으로는 다음과 같은 것들이 있습니다.\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 메모리 꽉참\n예를 눌러 지우십시오.

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
1 Dtterm - 경고

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 메모리 꽉참\n예를 눌러 지우십시오

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
2 Dtterm - 온라인 설명서
3 Dtterm - 도움말

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title.
$ The rest are resource types and values - look at the dialog box and
$ documentation for full specs.
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
1 dtterm - 글로벌 선택사항
2 예 
3 취소
4 도움말
5 글로벌
6 커서 조절도구
7 상자
8 밑줄
9 커서 모양
10 사용 가능
11 사용 불가
12 깜박임 커서
13 깜박임 속도 (밀리초)
14 보이지 않음
15 BlinkRate
16 색상 조절도구
17 반전
18 정상
19 창 배경
20 화면이동 작동
21 사용 불가
22 사용 가능
23 부드러운 화면이동
24 벨 조절도구
25 청각
26 시각
27 벨 유형
28 사용 가능
29 사용 불가
30 여백 경고
31 여백 거리
32 적용
33 다시 설정
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
1 기본값
2 창 크기W
3 W
4 -ksc*
5 %d 포인트
6 %.1f 포인트
7 %.1f 포인트
8 %.2f 포인트
9 기본값
10 글꼴 크기F
11 F

$   Window menu
12 새 창N
13 N
14 인쇄P
15 P
16 인쇄r...
17 r
18 닫기C
19 C
20 창W
21 W

$   Edit menu
22 복사하기C
23 C
24 Ctrl osfInsert
25 Ctrl+Insert
26 붙이기P
27 P
28 Shift osfInsert
29 Shift+Insert
30 편집E
31 E

$   Option menu

32 메뉴 막대M
33 M
34 이동 막대S
35 S
36 글로벌G...
37 G
38 단말기T...
39 T
40 부분 다시 설정S
41 S
42 전체 다시 설정H
43 H
44 다시 설정R
45 R
46 선택사항O
47 O

$ Old Help menu (no longer used for CDE)

48 개요O
49 O
50 작업T
51 T
52 참조R
53 R
54 항목O
55 O
56 도움말 사용U
57 U
58 dtterm에 관하여A
59 A
60 도움말H
61 H

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 개요v
65 v
66 색인I
67 I
68 차례C
69 C
70 작업T
71 T
72 참조R
73 R
74 키보드K
75 K
76 도움말 사용U
77 U
78 dtterm에 관하여A
79 A
80 도움말H
81 H

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
1 dtterm - 단말기 선택사항
2 예 
3 취소
4 도움말
5 키보드 조절도구
6 일반
7 응용프로그램
8 커서 키 모드
9 숫자
10 응용프로그램
11 키패드 모드
12 Return만 사용
13 Return/Line Feed
14 줄바꿈 순서
15 잠금
16 잠금 해제
17 사용자 기능 키
18 화면 조절도구
19 사용가능
20 사용불가
21 132칸 전환
22 줄내리기
23 줄올리기
24 적용
25 다시 설정
