
$ "$XConsortium: dtlogin.msg /main/3 1995/11/08 13:17:29 rswiston $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlogin.msg
$ **
$ **  Project:     CDE dtlogin
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlogin
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
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
$set 1
$ set 1 is labels
$ 
$ messages 1 through 6 are button labels on the login screen
1 사용자 이름을 입력하십시오.
2 암호를 입력하십시오.
3 예 
4 처음부터 다시
5 선택사항
6 도움말
$ 
$ messages 7 through 11 are options button menu items
$ 
7 로그인 화면 다시 설정  
8 명령 행 로그인
9 버전...
10 단일 창 세션
11 언어
$ 
$ messages 12 through 14 are miscellaneous button labels
$ 
12 DT 시작
13 취소
14 도움말
$ 
$ message 15, 17 are more option button menu items
$ message 16 is the welcome message
$ 
15 DT Lite 세션
16 환영합니다.
17 일반 데스크탑
$ 
$ message 18-24
$ 
18 * 데스크탑 로그인 일시중단...\r\n*\r\n
19 * 로그인 프롬프트를 원하면 [Enter]를 누르십시오.\r\n*\r\n
20 * 로그인. 데스크탑 로그인은 로그아웃 직후에 다시 계속됩니다.\r\n*\r\n
21 * 데스크탑 로그인중 입니다....\r\n*\r\n
22 * 디스플레이%1$s에서 데스크탑 로그인을 시작하는 중 입니다....\r\n*\r\n
23 * 로그인하기 전에 데스크탑 로그인 화면이 나올 때까지 기다려 주십시오.\r\n*\r\n
24 * X-server는 디스플레이 %1$s에서 시작할 수 없습니다...\r\n*\r\n
$ messages 25 is a options button menu item
25 세션
$ 
$ 26 replaces 16 as the greeting after a user name has been
$ entered. The %s is replaced by the user name.
$ 
26 %s 사용자를 환영합니다.

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room
$  for some shared sample source message additions.
$ 
73 사용자의 이전 데스크탑

$set 2
$ set 2 is error messages
$ 
1 로그인이 잘못되었습니다. 다시 해 주십시오.
2 홈 디렉토리로 바꿀 수 없습니다.
3 죄송합니다. 최대 사용자 수가 이미 로그인했습니다.
$ message 4 is a lengthy message that should be localized to correspond to
$ the language.
4 \n\
사용자의 등록에 대한 초기 암호를 선택해야 합니다. 또는\n\
현재 암호가 만기되었습니다.\n\
\n\
암호를 설정한 후에 새 암호를 사용하여\n\
다시 로그인해야 합니다.
5 로그인 오류, 유효하지 않은 사용자 id.
6 로그인 오류, 유효하지 않은 그룹 id.
7 로그인 오류, 유효하지 않은 감사 id.
8 로그인 오류, 유효하지 않은 감사 플래그.
9 지금은 로그인을 할 수 없습니다.
10 루트 사용자만이 %1$s을(를) 수행할 수 있습니다.\n
11 %1$s에서 기본 언어 목록을 다시 만드는 중입니다.\n
12 구성 파일과 서버 파일을 다시 조사하는 중입니다.\n
13 구성 파일 %1$s을(를) 다시 읽는 중입니다.\n
14 서버 파일 %1$s을(를) 다시 읽는 중입니다.\n
15 사용 파일 %1$s을(를) 다시 읽는 중입니다.\n
16 dtlogin: 루틴의 메모리가 부족합니다.\n
$set 3
$ set 3 is help text
$ 
$ message 1 is generated when the help button on the login screen is clicked.
1 \n\
로그인 도움말:\n\
\n\
컴퓨터가 사용자 자신을 구별하게 하려면 로그인 화면을 사용하십시오.\n\
\n\
로그인하려면:\n\
       1) 사용자 이름을 입력하고 <Enter>를 누르거나 [예]를 누르십시오.\n\
          (사용자를 만들지 않았다면,사용자 이름으로 "root"를\n\
          입력하십시오.)\n\
       2) 암호를 입력하고 <Enter>를 누르거나 [예]를 누르십시오.\n\
\n\
[처음부터 다시]\n\
   사용자 이름을 다시 입력하고 로그인할 수 있도록 항목을\n\
   지웁니다.\n\
\n\
[선택사항]\n\
   세션 선택사항을 바꿀 수 있습니다.\n\
\n\
   마우스 가리키개를 [선택사항] 단추 위에 갖다 놓고 마우스 단추 1을\n\
   누르십시오. 가리키개를 끌어 선택하려는 항목에 갖다 놓고\n\
   마우스 단추를 놓으면 그 항목이 선택됩니다.\n\
   이 때 다음과 같은 항목을 선택할 수 있습니다.\n\
\n\
   언어 - 세션 언어를 바꾸려면\n\
   세션 - 그래픽 환경을 선택하려면\n\
     일반 데스크탑 - Common Desktop Environment를 원할 때\n\
     비상안전 세션 - X-윈도우 환경을 원할 때\n\
   명령 행 로그인 - 명령 행 환경으로 전환하려면\n\
   로그인 화면 다시 설정 - X-서버를 다시 시작하려면\n\
\n\
시스템 메세지 때문에 화면이 보이지 않는다든지 해서\n\
화면을 다시 그리려면 마우스 단추로 배경의 아무 곳이나\n\
누르십시오.\n\
\n\
자세한 도움말은, 먼저 로그인한 후, 프론트 패널의\n\
도움말 관리자 아이콘(물음표가 있는 책 그림)을 누르십시오.\n\
그런 다음, 로그인 관리자 도움말 볼륨을 찾으십시오.\n\

$ message 2 is generated when you attempt to start a system whose id is
$ set to unknown or if it is not configured for networking.
2 \n\
사용중인 워크스테이션이 고유이름을 가지고 있지 않습니다. 그것은 현재\n\
"알수없음"으로 설정되어 있습니다. 이 워크스테이션의 이름을 지정하는\n\
것이 좋습니다.\n\
\n\
$ message 3 is generated when the help button on the chooser screen is clicked.
3 \n\
선택자 도움말:\n\
\n\
로그인할 서버를 선택하려면 선택자 화면을 사용하십시오.\n\
\n\
선택자 화면에는 각 항목마다 두 부분을 가진 목록이 있습니다.\n\
첫번째 부분(서버 이름)은 사용할 수 있는 각 서버의 이름을\n\
담고 있습니다. 두번째 부분(서버 정보)은\n\
각 서버에서 수행중인 로그인 관리자에 따라\n\
다른 정보를 포함하게 됩니다. 서버가 CDE 데스크탑을\n\
수행중이라면, 그것은 현재 사용중인 사용자(또는 tty 장치) 수에\n\
관한 정보를 제공하며, 최근 5, 10, 그리고\n\
15분에 대한 평균값(평균 CPU 사용)을\n\
로드합니다.\n\
\n\
로그인할 서버를 선택하려면, 목록에서 원하는 서버의 이름이 담긴\n\
줄을 선택하기만 하면 됩니다.\n\
\n\
선택을 하고 나서 예 단추를 누르십시오.\n\
이때 사용자가 선택한 서버에서 로그인 화면이 떠오릅니다.\n\
\n\
[목록 새로 그리기]\n\
   이것은 네트워크의 서버를 조회하여 로그인할 수 있는 상태인지\n\
   알아보아서 목록을 지우고 다시 만들게됩니다.\n\
\n\
[선택사항]\n\
   세션 선택사항을 바꿀 수 있습니다.\n\
\n\
   [선택사항] 단추에 마우스 가리키개를 갖다 놓고 마우스 단추 1을\n\
   누르고 계십시오. 가리키개를 끌어 선택하려는 항목에 갖다 놓고\n\
   마우스 단추를 놓으면 해당 항목이 선택됩니다.\n\
   이때 다음과 같은 항목을 선택할 수 있습니다.\n\
\n\
   언어 - 세션 언어를 바꾸려면\n\
   로그인 화면 다시 설정 - X-서버를 다시 시작하려면\n\
\n\
시스템 메세지 때문에 화면이 보이지 않는다든지 해서\n\
화면을 다시 그리려면 마우스 단추로 배경의 아무 곳이나\n\
누르십시오.\n\
\n\


$set 4
$ set 4 is log error messages
$ 

1 사용 파일 \"%1$s\", 호스트 \"%2$s\"을(를) 찾을 수 없습니다.\n
2 ReadHostEntry\n
3 사용 파일 %1$2s, 디스플레이 %1$2s을(를) 알 수 없습니다.\n
4 사용 제어 파일 %1$s을(를) 열 수 없습니다. XDMCP 요구가 승인되지 않습니다.\n
5 정의역/OS 확증 오류: %1$s (%2$s/%3$s)
6 서버 확증 파일 %1$s을(를) 열 수 없습니다.\n
7 서버 확증 파일 %1$s을(를) 써넣을 수 없습니다.\n
8 saveAddr
9 네트워크 인터페이스 구성을 가져오는 중의 문제
10 setAuthNumber
11 확증 파일 %1$s 또는 백업 %2$s을(를) 잠글 수 없습니다.\n
12 확증을 제자리로 옮길 수 없습니다.\n
13 pid 파일 %1$s을(를) 만들거나 잠글 수 없습니다.\n
14 pid 파일 %1$s을(를) 잠글 수 없습니다. 또 다른 Dtlogin이 수행중입니다. (pid %2$d)\n
15 서버 파일 %1$s을(를) 사용할 수 없습니다.\n
16 NewDisplay
17 errorLogFile %1$s을(를) 열 수 없습니다.\n
18 TrimErrorLog(): 오류 기록 파일의 최대 크기는 200Kb입니다.\n
19 서버 파일에 디스플레이 이름이 빠졌습니다.\n
20 %1$s에 대한 디스플레이 유형이 빠졌습니다.\n
21 Hostname %1$s이(가) 유효하지 않습니다. DISPLAY를 ':0'으로 설정하십시오.\n
22 디스플레이 %2$s에 대한 디스플레이 유형 %1$s을(를) 받아들일 수 없습니다.\n
23 의사 다시 설정이 시간 종료했습니다.\n
24 GetResource
25 인수를 다시 할당할 공간이 없습니다.\n
26 구성 파일 %1$s을(를) 열 수 없습니다.\n
27 명령 행에 불필요한 인수:
28 StartServer(): 인수가 없습니다.\n
29 서버 %1$s을(를) 실행할 수 없습니다.\n
30 StartServer() 분기가 실패했습니다. 휴면중입니다.\n
31 서버가 예기치 않게 중단되었습니다.\n
32 XOpenDisplay(%1$s)에서 #%2$d을(를) 시도하는 중에 정지했습니다. 중단하는 중입니다.\n
33 %2$s에 대한 서버 열기 시도 %1$s이(가) 실패했습니다. 포기하는 중입니다.\n
34 치명적 입출력 오류 %d (%1$s)\n
35 X 오류\n
36 세션을 시작하지 못했습니다.\n
37 경고: 디스플레이 %1$s을(를) 보호할 수 없습니다.\n
38 경고: 디스플레이 %1$s의 키보드를 보호할 수 없습니다.\n
39 BLS - 등록된 사용자 정보를 가져올 수 없었습니다.\n
40 BLS - 보호된 등록된 사용자 정보를 가져올 수 없습니다.\n
41 BLS - 감도 수준을 확인할 수 없습니다.\n
42 BLS - 다음 사용자로 바꿀 수 없습니다. %1$s.\n
43 AFS - 세션을 시작하는 중의 setgroups() 실패\n
44 사용자 %s에 대한 Setuid가 실패했습니다. errno = %d\n
45 사용자 %2$s에 대한 홈 디렉토리 %1$s이(가) 없습니다. /를 사용합니다.\n
46 세션 실행이 실패했습니다. %1$s\n
47 세션에 명령/인수가 없습니다.\n
48 %s에 대한 세션을 시작할 수 없습니다. 분기가 실패했습니다.\n
49 Dtlogin이 클라이언트를 없앨 수 없습니다.\n
50 %1$s을(를) 실행할 수 없습니다.\n
51 %1$s을(를) 실행하기 위해 분기할 수 없습니다.\n
52 부적당한 확증 이름 \"%1$s\". AFS, Kerberos 또는 기본값을 사용하십시오.\n
53 %2$s에 대해 Dtgreet %1$s을(를) 실행할 수 없습니다.\n
54 Dtgreet %1$s을(를) 실행하기 위해 분기할 수 없습니다.\n
55 포트 %1$d에서 소켓을 만들지 못했습니다.\n
56 오류 바인딩 소켓 주소 %1$d, errno = %2$d\n
57 makeEnv
58 setEnv
59 parseArgs
60 %1$s 디렉토리의 조사가 %2$d 초를 초과했습니다. 조사를 종료하는 중...\n
61 %s이(가) %1$d 개 이상의 그룹에 속합니다. %2$s을(를) 무시합니다.\n
62 audwrite() 오류: 호출자가 수퍼유저가 아닙니다.\n
63 audwrite() 오류: 감사 레코드에 유효하지 않은 이벤트 번호\n
64 audwrite() 오류: errno = %1$d\n
65 AFS 확증 실패: %1$s\n
66 %1$s에 대한 계정이 만료되었습니다. 시스템 관리자에게 문의하십시오.\n
67 서버가 종료했습니다.  %1$s\n
68 로고 비트맵 파일을 열 수 없습니다. %1$s\n
69 \"%1$s\" 메세지 카탈로그를 읽을 수 없습니다. 기본 언어를 사용합니다.
70 디스플레이 %1$s을(를) 초기화할 수 없습니다. 서버가 잡힐 수 있습니다.\n
71 (%1$s): 로그인 대화상자 폭이 디스플레이 한계를 초과합니다. 기본값을 사용합니다. 대화상자 단위는 픽셀입니다.\n
72 (%1$s) 로그인 대화상자 높이가 디스플레이 한계를 초과합니다. 기본값을 사용합니다. 대화상자 단위는 픽셀입니다.\n
$set 5
$ set 5 is for other stuff specific to chooser.
1 로그인할 서버를 선택하십시오.
$ 1 is the title/instructions for the chooser dialog
2 서버 이름                      서버 정보
$                                ^
$                                | This needs to start in this column
$ 2 is the headings for the list
3  목록 새로 그리기 
$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/3 1995/11/08 13:17:29 rswiston $
