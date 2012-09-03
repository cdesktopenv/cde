$ $XConsortium: dthelpgen.msg /main/3 1995/11/08 13:16:38 rswiston $
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
1 %s -dir <directory> [-generate] [-file <name>] [-lang <language>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: %s의 요소는 디렉토리가 아닙니다.\n
3 %s: 디렉토리 %s에 대한 사용이 거부되었습니다.\n수퍼 유저로 수행해 보겠습니까?\n
4 %s: %s의 요소는 존재하지 않습니다.\n
5 %s: %s을(를) 포함하는 파일 시스템이 꽉 찼습니다.\n
6 %s: %s을(를) 사용할 수 없습니다. - 오류 상태 번호 %d\n
7 %s: %s을(를) 포함하는 파일 시스템은 읽기 전용입니다.\n
8 %s: %s에 쓰려면 루트 사용 권한이 필요합니다.\n
9 %s: %s에 쓰기가 유효하지 않습니다.\n
10 %s: 찾기 경로가 비었습니다.\n
11 %s: 'title' 자원이 빠졌습니다.\n
12 %s: 'abstract' 자원이 빠졌습니다.\n
13 %s: 'volumes' 자원이 빠졌습니다.\n
14 %s: 'character' 세트 자원이 빠졌습니다.\n
15 %s: 목적지 디렉토리가 빠졌습니다.\n
16 %s: 제품군 파일이 발견되지 않았습니다.\n
17 %s: 볼륨 파일이 발견되지 않았습니다.\n
18 %s: 현재 작업 디렉토리를 사용할 수 없습니다. - 오류 상태 번호 %d\n
19 %s: 메모리를 할당할 수 없습니다.\n
20 %s: 유효하지 않은 시스템 언어 %s을(를) 지정했습니다.\n

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
1 ko_KR.dt-eucKR

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 도움말 관리자에 들어오신 것을 환영합니다.

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>도움말 관리자에 들어오신 것을 환영합니다.</ABBREV>              \
<PARAGRAPH>다음의 제목들은 온라인 도움말에\n                  \
설치되어 있고 등록된 <ANGLE italic> 제품군</>을\n          \
나타냅니다. 각 제목(및 아이콘)은\n                                       \
제품군 내의 도움말을 나열하는 하이퍼링크입니다.</>                       \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
제품군에서 사용할 수 있는 도움말의 목록을 보려면,\n                    \
그 제목(밑줄친 문자) 또는 아이콘을 선택하십시오.</PARAGRAPH>             \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
제품군 안에서 열람할 도움말을 찾은 후, \n                                \
그 제목을 선택하십시오.</PARAGRAPH>                                     \
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">         \
도움말 창을 사용하는 중에 도움말이 필요하면 F1을 누르십시오.</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 도움말 - 최상위

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>도움말 관리자에 들어오신 것을 환영합니다.</ABBREV>              \
<LINK 0 "Help4Help How-To-Register-Help">                                \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>                         \
주:\ \ \ 등록된 도움말이 없습니다.                                        \
</SIZE></WEIGHT></TYPE></></LINK>                                        \
<PARAGRAPH leftindent 3 firstindent 3>                                   \
<WEIGHT bold>훑어보기 위한 온라인 도움말 파일을 등록한 제품군이          \
없습니다.</>  응용프로그램 안에서 직접 도움말 명령을 선택하면            \
일부 응용프로그램에 대한 도움말을 사용할 수 있습니다.</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 열람기 정보를 만드는 중입니다. 잠시 기다려 주십시오.

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


