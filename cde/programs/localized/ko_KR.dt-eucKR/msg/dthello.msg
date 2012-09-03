
$ "$XConsortium: dthello.msg /main/3 1995/11/08 13:16:27 rswiston $";

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthello.msg
$ **
$ **  Project:     HP DT login transition program (dthello)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthello
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990, 1991 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
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

$set 2
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --
$    Do not translate. Blank message, please ignore
1 

$    Message #2 is no longer used. It has been replaced with messag #3
2 COSE\n\
User Enviroment\n\
시작\n\n

$    Default startup message (written before the contents
$    of the default file, /etc/copyright).
3 공통 데스크탑 환경\n\
시작\n\n


$set 4
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --
$    This entire set is for error messages
$    DO NOT TRANSLATE THESE MESSAGES
1 %s: 디스플레이를 열 수 없습니다.\n
2 %s: 시간 종료는 반드시 양수이어야 합니다.\n
3 %1$s: 최대 %2$d 개의 파일이 허용됩니다. %3$s을(를) 건너뛰는 중입니다.\n

$ Messages 4 and 5 are both used when there is a syntax error on
$ the command line invoking dthello
$ 
4 사용법: %s [-display <display>] [-fg <color>] [-bg <color>]\\\n
5 [-font <font>] [-string <message>] [-file <name>] [-timeout <seconds>]\n

$ _DtMessage 6 occurs when an font can't be loaded
$  the likely causes: (1) the font was incorrectly specified
$                    (2) it is not available on this server
6 %1$s: 디스플레이 %2$s은(는) 글꼴 %3$s을(를) 인식하지 못합니다.\n

7 %1$s: 파일 %2$s을(를) 열 수 없습니다.\n
8 %1$s: 배경을 %2$s(으)로 설정할 수 없습니다. %3$s을(를) 사용합니다.\n
9 %1$s: 전경을 %2$s(으)로 설정할 수 없습니다. %3$s을(를) 사용합니다.\n
10 %s: 메모리 부족 (SeparateTextLines)\n

$set 37
$ 
$    Sun platform specific messages
1 오픈윈도우즈 데스크탑 시작\n

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dthello.msg /main/3 1995/11/08 13:16:27 rswiston $
