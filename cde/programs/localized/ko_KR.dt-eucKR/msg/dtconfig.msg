$ $XConsortium: dtconfig.msg /main/3 1995/11/08 13:15:49 rswiston $
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *
$ *****************************************************************************
$ **
$ **  File:        dtconfig.msg
$ **
$ **  Project:     CDE Session Management (dtconfig)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtconfig
$ **
$ **  Message set 10: IBM
$ **  Message set 20: Sun
$ **
$ *****************************************************************************

$ *****************************************************************************
$ **  Message set 10: For IBM platforms
$ *****************************************************************************

$set 10
1 수퍼유저만이 실행시킬 수 있습니다.\n
$ 
2 CDE 환경이 현재 기본 사용자 인터페이스로 설정되어 있습니다.\n\
이 인터페이스는 시스템의 모든 사용자에 대하여 로그인할 때 나타납니다.\n\
이 변화를 보려면 시스템을 종료하고 다시 시작해야 합니다.\n
$ 
3 명령 행이 현재 기본 사용자 인터페이스로 설정되어 있습니다.\n\
이 인터페이스는 시스템의 모든 사용자에 대하여 로그인할 때 나타납니다.\n\
이 변화를 보려면 시스템을 종료하고 다시 시작해야 합니다.\n
4 CDE 환경이 이미 기본 사용자 인터페이스로 설정되어 있습니다.\n 
$ 
5 명령 행이 이미 기본 사용자 인터페이스로 설정되어 있습니다.\n
$ 
6 사용법: %s\n\
  -e (dtlogin의 자동시작을 사용 가능으로)\n\
  -d (dtlogin의 자동시작을 사용 불가로)\n\
  -kill (dtlogin을 kill)\n\
  -reset (dtlogin을 다시 설정 - 구성파일을 다시 읽음)\n
$ 
7 인수가 너무 많습니다. \n
$ 
8 인수 하나가 필요합니다. \n
$ 
9  xdm을 사용 불가 상태로 할 수 없습니다. xdm 사용 불가 상태에 대한\n\
온라인 설명서를 참조하고 다시 해 보십시오.\n 
$ 
10 Dtlogin이 실행중이지 않습니다. \n

$ *****************************************************************************
$ **  Message set 20: For Sun Platforms
$ *****************************************************************************

$set 20
$ 
1  CDE 구성 유틸리티 
$ 
2   %s -d     (자동시작을 사용 불가로)
3   %s -e     (자동시작을 사용 가능으로)
4   %s -kill  (dtlogin을 kill)
5   %s -reset (dtlogin을 다시 설정)
6   %s -p     (프린터 활동 갱신)
$ 
$	In above messages 2 through 6, only translate words between
$	left "(" and right ").
$ 
7 %s: 이 스크립트가 루트로 실행중이지 않습니다. 
8 사용법: 인수가 너무 많습니다. 
9 사용법: 인수 하나가 필요합니다. 
10 선택사항이 실패했습니다.
11 %s을(를) 실행할 수 없습니다.
12 사용 불가 상태가 실패했습니다.
13 %s을(를) 없앨수 없습니다.
14 마침
15 데스크탑 자동 시작이 사용 불가 상태입니다.
16 사용 가능 상태가 실패했습니다.
17 %s을(를) 찾을 수 없습니다.
18 %s을(를) 만들 수 없습니다.
19 %s을(를) 만들 수 없습니다.
20 데스크탑 자동 시작이 사용 가능 상태입니다.
21 dtlogin이 완전히 kill되었습니다.
22 dtlogin 구성 자원이 다시 로드되었습니다. 
23 프린터 활동 갱신을 모두 마쳤습니다. 
24 사용법:
25  %s -inetd    (inetd.conf /usr/dt 데몬)
26  %s -inetd.ow (inetd.conf /usr/openwin 데몬)
$ 
$	In above messages 25 and 26, only translate words between
$	left "(" and right ").
$ 
27 다음 시스템 부트시, 다음과 같은 것들이 /usr/dt/bin에서 실행될것임
$ 
28     rpc.ttdbserverd         (툴토크)
29     rpc.cmsd                (달력)
30     dtspcd                  (부프로세스 조종)
$ 
$  Keep column allignment on messages 28 thru 30.  
$  Only translate what's beteen ( )'s
$ 
31 다음 시스템 부트시, 다음과 같은 것들이 /usr/openwin/bin에서 실행될것임
$ 
32     rpc.ttdbserverd         (툴토크) 
33     rpc.cmsd                (달력)
$ 
$  Keep column allignment on messages 32 thru 33.  
$  Only translate what's beteen ( )'s
$ 
