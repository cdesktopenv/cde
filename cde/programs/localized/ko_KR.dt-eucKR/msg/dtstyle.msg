$ "$XConsortium: dtstyle.msg /main/3 1995/11/08 13:18:44 rswiston $"

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
2 알림
$ _DtMessage 3 is the title of all Error dialog boxes
3 오류
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 세션 관리자와 통신할 수\n\
없습니다... 마치는 중입니다.
 
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 스타일 관리자가 이미 수행중입니다.\n\
두번째 시도는 중단되었습니다.
6 X 툴 키트 오류가 발생했습니다...\n\
마치는 중입니다.

$ _DtMessage 7 is the title of all Help dialog boxes
7 스타일 관리자 - 도움말

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 스타일 관리자 안에 있는\n\
항목을 선택해야 합니다.

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 화면 보호 정보를 세션 관리자에서\n\
가져올 수 없습니다. 시작 설정값이\n\
틀릴 수 있습니다.


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 스타일 관리자

$ mnemonic and label for the File menubar item.
2 F
3 파일F
$ mnemonic and label for the Help menubar item.
4 H
5 도움말H

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 마침X

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 색상
28 글꼴
29 배경
30 키보드
31 마우스
32 신호음
33 화면
35 시작

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 현재 세션에 사용할 수 있는 Dtstyle 복원 정보가 없습니다.

$ _DtMessage 37 is the label on the dtstyle icon
37 스타일

$ Message 38 is the label on the Window button in the dtstyle main window.
38 창

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
48 항목O

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
55 개요v

$ Message 56 is a replacement for messages 44
56 작업T

$ Message 57 is a replacement for messages 46
57 참조R

$ Message 58 is a replacement for messages 50
58 도움말 사용U

$ Messages 59-60 are replacements for messages 51-52
59 A
60 스타일 관리자에 관하여A

$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 미리 보기

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 글꼴에 대한 변경사항은 일부 응용프로그램이\n\
다음 번에 시작할 때 나타납니다.\n\
파일 관리자와 응용프로그램 관리자 등\n\
다른 응용프로그램은 데스크탑을 마치고\n\
다시 로그인할 때까지 글꼴 변경사항을 보여 주지\n\
않습니다.

$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 선택된 글꼴은 사용자가 이 세션을\n\
다시 시작할 때 사용됩니다.

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 한글크기모양AaBbCc012345
21 한글크기모양AaBbCc012345 

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 크기

$ _DtMessage 23 is the title of the Fonts dialog
23 스타일 관리자 - 글꼴


$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 기본값
3 볼륨
4 톤
5 길이

$ _DtMessage 7 is the title of the Beep dialog box
7 스타일 관리자 - 신호음


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 로그인할 때:
4 현재 세션으로 계속
5 홈 세션으로 시작
6 로그아웃할 때 지정 
7 로그아웃 확인 대화:
8 켜기
9 끄기
10 홈 세션 설정 ...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 이것은 홈 세션을 현재 세션으로\n\
바꾸게 됩니다.\n\n \
     계속하겠습니까?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 경고

$ _DtMessage 13 is an error message
13 이 세션은 X 윈도우 시스템 시작 스크립트에서\n\
시작되었습니다. 해당 세션을 복원할 수 없으므로\n\
시작 설정값이 유효하지 않습니다.

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 스타일 관리자 - 시작

$ _DtMessage 15 is an error message
15 세션 관리자가 수행중이지 않습니다.\n\
시작 설정값이 유효하지 않습니다.


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
1 왼쪽
2 오른쪽
3 왼손/오른손:
4 단추 2:
$ _DtMessage 5 is the label of default button.
5 기본값

$ Messages 6,8,9 are labels of the scales.
6 두번 누르기
8 가속
9 임계값

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 오류: 가리키개 대응 설정\n

11 전송
12 조정

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 오른쪽과 왼쪽 마우스 단추는 예를 누르자마자\n\
기능이 맞바뀝니다.\n\
마우스 창의 `두번 누르기 시험' 그래픽은\n\
단추 1과 단추 3의 위치에 의해\n\
현재 좌우단추위치를 보여 줍니다.

$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 예를 누르면, 마우스 단추 2가 다음 세션에서\n\
'전송'(끌기)으로 설정됩니다.

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 새 두번 누르기 시간은 응용프로그램이 다시 시작할 때\n\
유효하게 됩니다.

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 새 두번 누르기 시간은 응용프로그램이 다시 시작할 때\n\
유효하게 됩니다.

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 두번 누르기\n시험

$ _DtMessage 18 is the Mouse Dialog title
18 스타일 관리자 - 마우스

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 예를 누르면, 마우스 단추 2가 다음 세션에서\n\
조정 단추로 설정됩니다. 단추 2를 사용하면\n\
문자와 목록 선택을 조정할 수 있습니다.\n\
문자와 목록에서 전송 활동(끌기)을 수행하려면\n\
단추 1을 사용해야 합니다.

$ _DtMessage 20 is the label used for the "Button 2" toggle on
$ a two button mouse (ignored on a three button mouse)
20 메뉴

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 파일 사용 권한을 점검하십시오.
3 %s은(는) 세션을 저장하기 위해 사용했던 파일일 수도 있습니다.\n


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 디렉토리 %s을(를) 열 수 없었습니다.
$ _DtMessage 2 - error opening the backdrop files
2 스타일 관리자의 배경 부분은\n\
사용할 수 있는 배경 파일이 없기 때문에\n\
작동하지 않습니다. $HOME/.dt/errorlog를 점검하십시오.


$ _DtMessage 12 is the title of the Backdrop dialog box
12 스타일 관리자 - 배경


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box. 
1 스타일 관리자 - 화면

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 기본값
4 화면 보호 기능
5 화면 보호 시작
6 화면 보호 시간
7 분
8 화면 잠금
$ Message 9 has been removed 
10 켜기
11 끄기
12 화면 보호 기능:
13 화면 잠금:
14 잠금 시작
15 자동 화면 끄기
16 화면 끄기:
17 화면 끄기 시작
18 프론트 패널 잠금
$ Message 19 has been replaced by message 21
19 배경 잠금 사용
20 배경 잠금 시간
$ Message 21 is a replacement for message 19
21 잠금용 배경 사용
22 투명 잠금

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 기본값
3 자동 반복
4 누르기 볼륨

$ _DtMessage 6 is the title of the Keyboard dialog box
6 스타일 관리자 - 키보드


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 선택판
6 더하기...
7 없애기...
8 수정...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 새 선택판 이름:

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 선택판 더하기 

$ Messages 12 through 14 are error messages.
12 선택판 이름은 다음과 같은 문자를\n\
포함할 수 없습니다.\n\n\
<space> * : ( ) [ ] { } < > ! | " / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 선택판 이름은 반드시\n10 문자 이하로 해야 합니다.\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 선택판 없애기 

$ _DtMessage 18 appears in an information dialog.
18 마지막 선택판를 없앨 수 없습니다.\n

$ _DtMessage 21 is an error message
21 경고

$ _DtMessage 23 appears in an information dialog
23 선택판 '%s'을(를) 없애겠습니까?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 스타일 관리자의 색상 부분은\n\
색상 서버가 수행중이지 않기 때문에 작동하지\n\
않습니다.  $HOME/.dt/errorlog를 점검하십시오.

$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 스타일 관리자의 색상 부분은 자원\n\
'useColorObj'가 False로 설정되었기 때문에\n\
작동하지 않습니다.

$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 새 색상 사용 값은 다음 세션에서\n\
유효합니다.

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 선택된 선택판은 다음 세션에서\n\
유효합니다.

$ _DtMessage 29 appears in a warning dialog
29 '%s'(이)라는 선택판이 이미 있습니다.\n\
새로운 이 선택판은 이전 것에 겹쳐쓰게 됩니다.\n\
맞습니까?

$ _DtMessage 30 is the title of the Color dialog box.
30 스타일 관리자 - 색상

$ _DtMessage 31-33 are the ColorUse options
$ 31 High Color
$ 32 Medium Color
$ 33 Low Color
34 흑백
35 기본값

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT Color Use

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog 
$ 37 DT Color Use...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 스타일 관리자의 색상 부분은\n\
사용할 수 있는 선택판이 없기 때문에\n\
작동하지 않게 됩니다.  $HOME/.dt/errorlog를 점검하십시오.

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39 사용할 색상 수

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 색상 수...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 최대 색상
42 중간 색상
43 적은 색상

$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 '%s'을(를) 없앨 수 없습니다.\n
12 $HOME/.dt/errorlog를 보면 자세한 정보를 얻을 수 있습니다.

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 경고, 자원 paletteDirectories에 너무 많은 디렉토리가 나열되었습니다.\n\
최대 수는 %d입니다.
$ _DtMessage 19 - could not open a palette directory
19 디렉토리 %s을(를) 열 수 없었습니다.
$ _DtMessage 20 - error opening the palette file
20 %s을(를) 열 수 없었습니다.
$ _DtMessage 21 - error reading a palette file
21 %s은(는) 유효하지 않은 선택판 파일입니다.


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 색상 보기 
5 기존 색상
6 새 색상
7 색상 잡기
8 색상 편집기
9 색조
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 R
11 G
12 B

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 경고, 가리키개를 잡을 수 없었습니다.\n
15 경고, 키보드를 잡을 수 없었습니다.\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 스타일 관리자 - 색상 수정


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 스타일 관리자 - 창

$ Messages 2 through 5 are labels on the Window dialog.
2 기본값
3 창 동작
4 창 아이콘
$ Messages 5-8 have been replaced by messages 13-16 
$ 5 Focus Follows Mouse
$ 6 Click In Window For Focus
$ 7 Raise Window With Focus
$ 8 Opaque Move
9 아이콘 상자 사용
10 작업장에 놓기
11 최상위의 기본창을 허용
12 변경사항을 적용하려면 작업장 관리자를\n\다시 시작해야 합니다.\n\
예를 눌러 작업장 관리자를 다시 시작하거나\n\
취소를 눌러 변경사항을 다시 설정하십시오.
$ Messages 13-16 are replacements for messages 5-8
13 창을 가리키면 활성화됨
14 창을 누르면 활성화됨
15 활성화될 때 창을 앞으로 
16 옮길 때에도 내용을 보임

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/3 1995/11/08 13:18:44 rswiston $"

