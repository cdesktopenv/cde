$ $XConsortium: dtpad.msg /main/4 1995/12/08 09:35:45 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpad.msg
$ **
$ **  Project:     DT Text Editor client (sometimes called dtpad)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtpad
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ *****************************************************************************
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ *****************************************************************************
$ **

$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************

$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
1 도움말


$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 파일 열기

$ label for the File text field in the file selection box
$ ***UNUSED*** This label is no longer used and need not be localized.
2 열려는 파일:

$ Title of the Include dialog
3 파일 포함

$ Label for the File text field in the file selection box
$ ***UNUSED*** This label is no longer used and need not be localized.
4 포함할 파일:

$ Error when printing a file and a temporary copy can not be saved.
5 임시 파일을 만들 수 없습니다.


$set 5
$ ***************************************************************************
$ **                                                               dtpad.c **
$ **                                                             fileDlg.c **
$ **                                                                       **
$ ** Messages and strings used in dialogs for all operations involving the **
$ ** "File" menu items.                                                    **
$ ***************************************************************************
$ These compose the dialog which appears if the user attempts to exit, open
$ a different file, drop a file, or start a "New" session with unsaved
$ changes to the current file.

$ This is used to build: "Save changes to FILENAME?"
1 변경사항을 %s에 저장하겠습니까?

$ This is used as-is, whenever the current file is unnamed.
2 현재 파일의 이름이 없습니다.\n저장하겠습니까?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED*** Currently, dropped files are INSERTED in the text rather than
$          opened for editing so this message is no longer used and need
$          not be localized.
3 %s의 현재 복사본을\n%s을(를) 편집하기 전에\n저장하겠습니까?

$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.
5 %s 파일이 이미 있습니다.\n해당 파일을 이 편집 세션의 내용으로 겹쳐쓰겠습니까?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 경고

$ Title of "Save As" dialog.
7 새이름 저장

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
8 줄내린 줄의 끝에 줄바꿈 문자가 추가됨
9 줄바꿈 문자가 추가되지 않음. [Return]으로 구분된 줄바꿈만\n보존됩니다.

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 아니오
15 예
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 저장?

$ Title of general error dialog
17 오류

$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message
18 Exit Confirmation

$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.
20 정보

$ Default file name in Save As dialog box.
21 UNTITLED

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 %1$s을(를) 저장할 수 없습니다.\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 파일을 저장할 수 없습니다.\n파일 이름을 지정하지 않았습니다.

34 아무에게도 파일에 대한 쓰기 권한이 없습니다.

35 지정된 파일은 디렉토리입니다.

36 지정된 파일은 특수 문자 장치입니다.

37 지정된 파일은 블럭 모드 장치입니다.

$ ***UNUSED*** This message should never appear since currently there is
$          only one supported file format - text.  And this type is
$          hard coded into dtpad.
38 유효하지 않은 파일 형식

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 원인을 알 수 없음 - 디스크 공간이 부족한 것같습니다.

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 변경사항을 저장하겠습니까?

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 변경사항을 "%s"에 저장하겠습니까?

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
42 새이름으로 복사하기

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 파일로 복사하기

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 줄맞추기 줄의 끝에 줄바꿈 문자를 추가하십시오.
45 줄바꿈 문자를 추가하지 마십시오. [Return]에 의해 만들어진 줄 분리만이 \n보존됩니다.
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This
$ new message is in response to bug DTS CISlx19530.
$ 
46 파일에 쓰기가 허용되지 않거나\n파일이 경로에 없습니다.


$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 이 파일은 읽기 전용으로 열렸습니다.
2 이 파일은 없습니다.
3 디렉토리를 편집할 수 없습니다.
4 특수 문자 장치 파일을 편집할 수 없습니다.
5 블럭 모드 장치 파일을 편집할 수 없습니다.
6 포함된 null 문자들이 파일에서 분리되었습니다.
7 파일을 로드할 수 없습니다. (메모리 부족)
8 파일을 사용할 수 없습니다. (상태를 알 수 없음)
9 파일을 읽을 수 없습니다. (읽기 권한?)

$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 열람 전용
11 읽기 전용


$set 7
$ ************************************
$ **                         main.c **
$ **                        dtpad.c **
$ ** Prefix string for all dialogs. **
$ ************************************
$ bogus?? blank string gotten but otherwise unused in main()
$ DO NOT LOCALIZE string 1
1 

$ The name used in the title bar of every window.
$ Be sure to end this message with a space character.
2 문서 편집기 - 

$ Default file name used in the main window title.
$ ***UNUSED*** This mesage has been replaced by message 6 of this set and
$          need not be localized.
3 (이름없음)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 데스트탑 서비스를 초기화할 수 없습니다 ... 문서 편집기를 마치는 중입니다.

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 디스플레이를 열 수 없습니다 ... 문서 편집기를 마치는 중입니다.

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (이름없음)
$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 오류: dtpad 서버가 %s에서\n\
이 디스플레이를 위한 서비스를 하고 있습니다.
8 오류: dtpad 서버가 이 디스플레이를\n\
위한 서비스를 하고 있습니다.
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' is a bad option.\n\
사용법: %s...\n\
   -saveOnClose\n\
\tDtpad는 문서 편집기를 닫을 때 저장되지 않은 변경을\n\
\t자동으로 대화없이 저장합니다.\n\n\
   -missingFileWarning\n\
\t파일이 지정될 때 그 파일이 없거나 사용할 수 없는 경우에\n\
\t경고 대화 상자를 나타냅니다.\n\n\
   -noReadOnlyWarning\n\
\t파일이 지정될 때, 그 파일에 대한 쓰기 권한이 없으면,\n\
\t나타난 경고 대화 상자를 비활성화시킵니다.\n\n\
   -noNameChange\n\
\t읽었던 파일과 다른 이름으로 문서를 저장할 때\n\
\t현재 문서와 연관된 기본 파일 이름을 변경하지\n\
\t않습니다.\n\n\
   -viewOnly\n\
\t편집 윈도우에서 문서의 편집을 허용하지 않습니다.\n\
\t문서 편집기는 문서 열람기로 작동하게 됩니다.\n\n\
   -statusLine\n\
\t편집 윈도우의 아래쪽에 상태 줄을 표시합니다.\n\
\t상태 줄은 현재 문자 커서가 있는 줄의 줄 번호를\n\
\t보여줍니다.\n\n\
   -wrapToFit\n\
\t초기에 줄맞춤 모드를 켭니다.\n\n\
   -workspaceList WorkspaceList\n\
\t지정된 작업장 또는 작업장들에 문서 편집기\n\
\t윈도우의 현재 호출에 대한 편집 윈도우가\n\
\t나타납니다.\n\n\
   -session SessionFile\n

10 \t문서 편집기의 모든 편집 윈도우와 이전 CDE 종료\n\
\t전에 적용되던 설정을 복원합니다. 이 선택사항이 \n\
\t지정되면 명령 프롬프트의 다른 선택사항들은 모두\n\
\t무시됩니다.\n\n\
   -standAlone\n\
\t문서 편집기의 현재 호출을 문서 편집기 서버에\n\
\t독립된 자체의 윈도우와 문서 처리로 실행하게\n\
\t합니다.\n\n\
   -noBlocking\n\
\t문서 편집기 서버가 요청자의 편집 요구를 처리할\n\
\t수 있으면 바로 문서 편집기 요청자 처리를\n\
\t종료합니다.\n\n\
   -server\n\
\t현재 수행중이지 않으면 문서 편집기 서버를\n\
\t디스플레이용 모든 부속 편집 요구를 처리하도록\n\
\t시작합니다.\n\n\
   -exitOnLastClose\n\
\t문서 편집기 서버 프로세스를 디스플레이의\n\
\t마지막 편집 윈도우를 닫을 때 종료시킵니다.\n\
\t이 선택사항은 서버 프로세스에만 적용되므로\n\
\t-server 선택사항과 함께만 사용됩니다.\n\n\
선택사항에 대한 자세한 정보는 dtpad 온라인 설명서를 참조하십시오.
$ 

$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 파일 사용 권한을 점검하십시오.
2  %s은(는) 세션을 저장하는 데 사용한 파일입니다.


$set 11
$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************

$ ***UNUSED***     Messages 1-14 are no longer used and need not be localized.
$          All of the mnemonics and labels relating to the "Help" menu
$          have been replaced by messages 81-96.

$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
1 H
2 도움말H

$ mnemonic and label for the "Overview" item
3 V
4 개요V...

$ mnemonic and label for the "Tasks" item
5 T
6 작업T...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 참조R...

$ mnemonic and label for the "On Item" item
9 O
10 항목O

$ mnemonic and label for "Using Help" item
11 U
12 도움말 사용U...

$ mnemonic and label for "About Text Editor" item
13 A
14 문서 편집기에 관하여A...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 파일F

$ mnemonic and label for the "New" item
17 N
18 새 파일N

$ mnemonic and label for the "Open" item
19 O
20 열기O...

$ mnemonic and label for the "Include" item
21 I
22 포함I...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 S
24 저장S

$ mnemonic and label for the "Save As" item
25 A
26 새이름 저장A...

$ mnemonic and label for the "Print" item
27 P
28 인쇄P...

$ mnemonic and label for the "Close" item
29 C
30 닫기C

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 E
32 편집E

$ mnemonic and label for the "Undo" menu item
33 U
34 원래대로U

$ mnemonic and label for the "Cut" menu item
35 t
36 자르기t

$ mnemonic and label for the "Copy" menu item
37 C
38 복사하기C

$ mnemonic and label for the "Paste" menu item
39 P
40 붙이기P

$ mnemonic and label for the "Clear" menu item
41 e
42 다시입력e

$ mnemonic and label for the "Delete" menu item
43 D
44 없애기D

$ ***UNUSED*** Messages 45-50 are no longer used and need not be localized.
$          They have been replaced by messages 101-106 of this set.
$ mnemonic and label for the "Select All" menu item
$ 45 A
$ 46 Select All

$ mnemonic and label for the "Find/Change" menu item
$ 47 F
$ 48 Find/Change...

$ mnemonic and label for the Check Spelling menu item
$ 49 S
$ 50 Check Spelling...

$ *
$ * Mnemonics and labels for the "Format" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
51 r
52 형식r

$ mnemonic and label for the "Settings..." menu item
53 S
54 설정값S...

$ mnemonic and label for the "Paragraph" menu item
55 P
56 문단P

$ mnemonic and label for the "All" menu item
57 l
58 전체l

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 O
60 선택사항O

$ mnemonic and label for the "Overstrike" toggle button
61 O
62 겹쳐쓰기O 

$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***  This label and mnemonic are no longer used and need not be
$               localized.  They've been replaced by messages 111 and 112.
63 W
64 줄내리기W

$ mnemonic and label for the "Status Line" toggle button
65 S
66 상태 표시S

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED***     This nmemonic and label are no longer used and need not be
$          localized.  They've been replaced by messages 87 and 88.
71 C
72 차례C...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 저장(필요)S


$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 H
82 도움말H

$ mnemonic and label for the "Overview" item
83 v
84 개요v

$ mnemonic and label for the "Tasks" item
85 T
86 작업T

$ nmemonic and label for the "Table of Contents" item
87 C
88 차례C

$ mnemonic and label for the "Reference" item
89 R
90 참조R

$ mnemonic and label for the "On Item" item
91 O
92 항목O

$ mnemonic and label for "Using Help" item
93 U
94 도움말 사용U

$ mnemonic and label for "About Text Editor" item
95 A
96 문서 편집기에 관하여A


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 S
102 전체 선택S

$ mnemonic and label for the "Find/Change" menu item
103 F
104 찾기/바꾸기F...

$ mnemonic and label for the Check Spelling menu item
105 k
106 철자 검사k...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***  This label is no longer used and need not be localized.
107 A
108 새이름으로 복사하기...

$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 F
110 파일로 복사하기F...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 W
112 줄맞추기W


$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 문서 편집기내의 항목을 선택해야 합니다.


$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 검사점(checkpoint) 저장이 실패했습니다.
2 검사점(checkpoint) 응답이 실패했습니다.

$ NEW message after the SAMPLE IMPLEMENTATION.... more errors/warnings can
$ be detected while reading in a file via ToolTalk.
3 Unable to set contents of Text Editor (unknown reason).
