$ $XConsortium: dtksh.msg /main/3 1996/07/24 18:57:45 drk $
$set 1
1 DtkshCvtStringToPixel: '%s' 색상이 정의되지 않았습니다.
$set 5
1 너무 많은 자원 매개변수를 지정했습니다. '%s'을(를) 건너뜁니다.
2 잘못된 자원 사양; '이름:값'의 형태로 해야 합니다: %s
3 자원 분리기호는 ':'인데 '='를 사용했습니다: %s
4 도구 세트는 이미 초기화되었습니다.
5 사용법: XtInitialize variable applicationName applicationClass [args ...]
6 사용법: %s variable name class parent [arg:val ...]
7 사용법: XtCreateApplicationShell variable name class [arg:val ...]
8 사용법: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive
9 '%s' 잡기 유형을 인식하지 못했습니다. 'GrabNone'을 사용합니다.
10 사용법: XtIsShell widget
11 사용법: XtMapWidget widget
12 사용법: XtUnmapWidget widget
13 사용법: %s variable widget
14 사용법: XtScreen variable widget
15 사용법: XtWindow variable widget
16 사용법: XtCallCallbacks widget callbackName
17 사용법: XtHasCallbacks variable widget callbackName
18 사용법: XtAddCallback widget callbackName ksh-command
19 사용법: XtRemoveCallback widget callbackName ksh-command
20 다음은 유효하지 않은 콜백 핸들입니다: %s
21 지정한 콜백이 등록되지 않았습니다.
22 사용법: XtAddEventHandler widget mask nonMaskable ksh-command
23 사용법: XtRemoveEventHandler widget mask nonMaskable ksh-command
24 지정한 이벤트 핸들러가 등록되지 않았습니다.
25 사용법: XtGetValues widget resource:variable ...
26 다음 자원 매개변수의 양식이 틀립니다: %s
27 사용법: XtSetValues widget arg:val ...
28 사용법: XtAddWorkProc variable command
29 사용법: XtRemoveWorkProc workProcId
30 workProcId 매개변수는 16진수이어야 합니다: %s
31 사용법: XtAddTimeOut variable milliseconds command
32 milliseconds 매개변수는 0보다 커야 합니다.
33 사용법: XtRemoveTimeOut intervalId
34 intervalId 매개변수는 16진수이어야 합니다: %s
35 사용법: %s widget ...
36 사용법: XBell display volume
37 사용법: %s variable screen
38 screen 매개변수가 유효하지 않습니다: %s
39 사용법: XDefineCursor display window cursorId
40 cursorId 매개변수가 유효하지 않습니다: %s
41 사용법: XUndefineCursor display window
42 사용법: XtRemoveAllCallbacks widget callbackName
43 사용법: XTextWidth variable fontName string
44 사용법: %s display drawable [args ...]
45 drawable 매개변수가 유효하지 않습니다: %s
46 destination 매개변수가 유효하지 않습니다: %s
47 인식할 수 없는 그래픽 함수 이름: %s
48 인식할 수 없는 선 스타일: %s
49 인식할 수 없는 그리기 선택사항: %s
50 무시된 나머지 점이 있었습니다.
51 사용법: XCopyArea display source dest sourceX sourceY width height destX destY [args ...]
52 '%s' 자원 유형을 '문자열'로 변환할 수 없습니다.
53 'String' 자원 유형을 '%s' 유형으로 변환할 수 없습니다.
54 사용법: XtAddInput variable [-r] fileDescriptor kshCommand
56 사용법: XtRemoveInput inputId
57 inputId 매개변수는 16진수이어야 합니다: %s
59 사용법: XFlush display
60 사용법: XSync display discard
61 사용법: XRaiseWindow display window
62 사용법: %s widget [True|False]
63 사용법: %s widget translations
64 사용법: XtUninstallTranslations widget
65 사용법: XtParent variable widget
66 사용법: XtLastTimestampProcessed variable display
67 사용법: DtSessionRestorePath widget pathVariable saveFile
68 사용법: DtSessionSavePath widget pathVariable fileVariable
69 사용법: DtShellIsIconified widget
70 사용법: DtSetStartupCommand widget command
71 사용법: DtSetIconifyHint widget boolean
73 rootWindow 매개변수가 유효하지 않습니다: %s
74 사용법: DtWsmSetCurrentWorkspace widget atom
75 작업장 원소가 유효하지 않습니다: %s
78 사용법: DtWsmSetWorkspacesOccupied display window workspaceList
79 사용법: DtGetHourGlassCursor variable display
80 사용법: DtWsmAddCurrentWorkspaceCallback variable widget ksh-command
81 사용법: DtWsmRemoveWorkspaceCallback handle
82 사용법: XtNameToWidget variable referenceWidget names
83 사용법: DtDbLoad
84 사용법: DtDbReloadNotify ksh-command
85 사용법: DtActionExists actionName
86 사용법: DtActionLabel variable actionName
87 사용법: DtActionDescription variable actionName
88 사용법: DtActionInvoke widget actionName termParms execHost contextDir useIndicator ksh-command ["FILE" fileName] ...
89 사용법: DtDtsLoadDataTypes
90 사용법: DtDtsFileToDataType variable fileName
91 사용법: DtDtsFileToAttributeValue variable fileName attrName
92 사용법: DtDtsFileToAttributeList variable fileName
93 사용법: DtDtsDataTypeToAttributeValue variable dataType attrName optName
94 사용법: DtDtsDataTypeToAttributeList variable dataType optName
95 사용법: DtDtsFindAttribute variable name value
96 사용법: DtDtsDataTypeNames variable
97 사용법: DtDtsSetDataType variable fileName dataType override
98 사용법: DtDtsDataTypeIsAction dataType
99 사용법: ttdt_open variable status variable2 toolname vendor version sendStarted
100 사용법: tttk_Xt_input_handler procId source id
101 원시 매개변수는 정수이어야 합니다: %s
102 id 매개변수는 16진수이어야 합니다: %s
103 사용법: ttdt_close status procId newProcId sendStopped
104 사용법: ttdt_session_join variable status sessId shellWidgetHandle join
105 사용법: ttdt_session_quit status sessId sessPatterns quit
106 sessPatterns 매개변수가 유효하지 않습니다: %s
107 사용법: ttdt_file_event status op patterns send
108 사용법: ttdt_file_quit status patterns quit
109 사용법: ttdt_Get_Modified pathName scope timeout
110 사용법: %s status pathName scope timeout
112 사용법: tt_error_pointer variable ttStatus
113 사용법: %s status msg
114 사용법: %s status msg msgStatus msgStatusString destroy
115 사용법: ttdt_file_join variable status pathName scope join ksh-command
116 사용법: tt_file_netfile variable status filename
117 사용법: tt_netfile_file variable status netfilename
118 사용법: tt_host_file_netfile variable status host filename
119 사용법: tt_host_netfile_file variable status host netfilename
120 사용법: XtIsSubclass widget class
121 %s은(는) 유효한 위짓트 클래스 이름이 아닙니다.
122 사용법: XtClass variable widget
$set 6
1 DtkshCvtWindowToString: 'from' 값의 크기가 유효하지 않습니다.
2 DtkshCvtHexIntToString: 'from' 값의 크기가 유효하지 않습니다.
3 DtkshCvtIntToString: 'from' 값의 크기가 유효하지 않습니다.
4 DtkshCvtBooleanToString: 'from' 값의 크기가 유효하지 않습니다.
5 DtkshCvtStringToWidget: 'from' 값의 크기가 유효하지 않습니다.
6 DtkshCvtStringToWidget: 지정된 위짓트 '%s'을(를) 찾을 수 없습니다.
7 DtkshCvtStringToCallback: 'from' 값의 크기가 유효하지 않습니다.
8 DtkshCvtCallbackToString: 'from' 값의 크기가 유효하지 않습니다.
9 DtkshCvtWidgetToString: 'from' 값의 크기가 유효하지 않습니다.
10 DtkshCvtWidgetToString: 위짓트에 대한 이름을 찾을 수 없습니다.
11 DtkshCvtWidgetClassToString: 'from' 값의 크기가 유효하지 않습니다.
12 DtkshCvtWidgetClassToString: 알 수 없는 위짓트 클래스
13 DtkshCvtStringToWidgetClass: 알 수 없는 위짓트 클래스 이름
14 DtkshCvtScreenToString: 'from' 값의 크기가 유효하지 않습니다.
$set 11
1 '%s'(이)라는 이름을 가진 필드를 구조체 '%s'에서 찾을 수 없습니다.
2 DtkshCvtCallbackToString: 내부 변환 버퍼가 넘쳤습니다.
3 자원 '%s'에 대한 (위짓트 클래스 '%s') 해시 실패
4 함수 이름을 제공하지 않았습니다.
5 명령을 실행할 수 없습니다. 도구 세트가 초기화되지 않았습니다.
6 '%s' 위짓트를 만들지 못했습니다.
7 display 매개변수가 유효하지 않습니다: %s
8 다음 특성 원소가 유효하지 않습니다: %s
9 다음 자원은 위짓트 '%s'에 대해 정의되어 있지 않습니다: %s
10 지정한 위치가 유효하지 않습니다: %s
11 지정한 글꼴이 유효하지 않습니다: %s
12 '%s' 위짓트 핸들이 기존 위짓트를 참조하지 않습니다.
13 위짓트는 'command' 위짓트이어야 합니다.
14 위짓트는 'mainWindow' 위짓트이어야 합니다.
15 위짓트는 'scale' 위짓트이어야 합니다.
16 위짓트는 'scrollBar' 위짓트이어야 합니다.
17 위짓트는 'toggleButton' 위짓트 또는 소도구이어야 합니다.
18 window 매개변수가 유효하지 않습니다: %s
19 필요한 메모리를 할당할 수 없습니다. 마칩니다.
20 부모 위짓트를 찾을 수 없습니다.
21 '%s' 기호를 찾을 수 없습니다.
22 '%s' 유형을 찾을 수 없습니다.
23 '%s' 선언을 분석할 수 없습니다. 'unsigned long'을 사용합니다.
24 알 수 없는 자식 유형: %s
25 인식할 수 없는 선택사항 플래그: %s
26 사용법: %s widget position
27 사용법: %s widget time
28 사용법: %s widget
29 dtksh 오류
30 dtksh 경고
31 사용법: %s display window
32 사용법: %s display window variable
33 사용법: %s display rootWindow variable
34 timeout 매개변수가 유효하지 않습니다: %s
35 msg 매개변수가 유효하지 않습니다: %s
36 patterns 매개변수가 유효하지 않습니다: %s
$set 14
1 위짓트 클래스를 초기화하는 중의 내부 해시 표 실패; 마칩니다.
2 지정된 '%s' 위짓트 클래스를 찾을 수 없습니다.
3 '%s' 구별자는 유효한 위짓트 핸들이 아닙니다.
4 위짓트 클래스를 찾을 수 없습니다.
5 사용법: DtLoadWidget widgetClassName widgetClassRecordName
6 지정된 '%s' 위짓트 클래스 레코드를 찾을 수 없습니다.
7 '%s' 위짓트 클래스를 초기화하는 중의 내부 해시 표 실패
8 \n%sRESOURCES FOR %s%s%s:\n
9 CONSTRAINT 
10 R
11 M
12 S
13 %-15s %-6s %-6s %-18s %-6s %s\n
14 ENV VARIABLE    HANDLE PARENT CLASS              STATUS NAME\n
15 \n사용법:\tDtWidgetInfo [widgetHandle]\n\tDtWidgetInfo -r <widgetHandle|className>\n\tDtWidgetInfo -R <widgetHandle|className>\n\tDtWidgetInfo -c [className]\n\tDtWidgetInfo -h [widgetHandle]
$set 15
1 도구 세트를 초기화할 수 없습니다.
2 다음 자원을 위짓트 만들기 시간에 설정할 수 없습니다.\n만든 후에 XtSetValues를 대신 사용하십시오: %s
3 사용법: %s variable parent name [argument:value ...]
4 사용법: DtHelpReturnSelectedWidgetId variable widget variable
5 사용법: DtHelpSetCatalogName catalogName
6 사용법: DtHelpQuickDialogGetChild variable quickHelpWidget child
7 위짓트는 'quickHelp' 위짓트이어야 합니다.
8 위짓트는 'list' 위짓트이어야 합니다.
9 사용법: %s widget position item
10 사용법: %s widget position itemList
11 사용법: %s widget item
12 사용법: XmListDeleteItemsPos widget count position
13 사용법: XmListDeleteItems widget itemList
14 사용법: XmListDeletePositions widget positionList
15 사용법: XmListGetKbdItemPos variable widget
16 사용법: XmListItemExists widget item
17 사용법: XmListItemPos variable widget item
18 사용법: XmListPosSelected widget position
19 사용법: XmListPosToBounds widget position variable variable variable variable
20 사용법: %s widget position notifyFlag
21 사용법: %s widget item notifyFlag
22 사용법: XmListSetAddMode widget boolean
23 사용법: XmListSetKbdItemPos widget position
24 사용법: XmMainWindowSetAreas mainwindow menu command hscroll vscroll work
25 'mainWindow' 핸들이 NULL입니다.
26 사용법: %s variable mainwindow
27 사용법: XmProcessTraversal widget direction
28 알 수 없는 횡단 방향: %s
29 사용법: XmInternAtom variable display name onlyIfExists
30 사용법: XmGetAtomName variable display atom
31 지정한 원소가 유효하지 않습니다: %s
32 사용법: XmGetColors widget background foreground topshadow bottomshadow select
33 배경 픽셀이 유효하지 않습니다: %s
34 사용법: XmUpdateDisplay widget
35 사용법: %s widget protocol [protocol ...]
36 지정한 원소가 유효하지 않습니다: %s
37 사용법: XmAddWMProtocolCallback widget protocol ksh-command
38 사용법: XmRemoveWMProtocolCallback widget protocol ksh-command
39 사용법: XmMenuPosition menu event
40 지정한 이벤트가 유효하지 않습니다: %s
41 사용법: XmCommandAppendValue commandWidget string
42 사용법: XmCommandError commandWidget errorMessage
43 사용법: XmCommandSetValue commandWidget command
44 사용법: XmCommandGetChild variable commandWidget child
45 사용법: XmMessageBoxGetChild variable commandWidget child
46 위짓트는 'messageBox' 위짓트이어야 합니다.
47 사용법: XmFileSelectionBoxGetChild variable widget child
48 위짓트는 'file selection box' 위짓트이어야 합니다.
49 사용법: XmSelectionBoxGetChild variable widget child
50 위짓트는 'selection box' 위짓트이어야 합니다.
51 사용법: XmScaleGetValue scaleWidget variable
52 사용법: XmScaleSetValue scaleWidget value
53 지정한 눈금 값이 유효하지 않습니다: %s
54 사용법: XmScrollBarGetValues scrollbar variable variable variable variable
55 사용법: XmScrollBarSetValues scrollbar value sliderSize increment pageIncrement notify
56 지정한 값이 유효하지 않습니다: %s
57 지정한 슬라이더 크기가 유효하지 않습니다: %s
58 지정한 증가량이 유효하지 않습니다: %s
59 지정한 쪽 증가량이 유효하지 않습니다: %s
60 사용법: XmScrollVisible scrolledWin widget leftRightMargin topBottomMargin
61 위짓트는 'scrolledWindow' 위짓트이어야 합니다.
62 볼 수 있게 만들 위짓트가 없습니다.
63 지정한 왼쪽/오른쪽 여백이 유효하지 않습니다: %s
64 지정한 위쪽/아래쪽 여백이 유효하지 않습니다: %s
65 사용법: %s widget state notify
66 사용법: catopen variable catName
67 사용법: catclose catId
68 사용법: catgets variable catId setNum msgNum dftMsg
69 위짓트는 'text' 또는 'textField' 위짓트이어야 합니다.
70 사용법: %s variable widget
71 사용법: %s widget boolean
72 지정한 매개변수가 유효하지 않습니다: %s
73 사용법: %s widget lines
74 사용법: %s widget maxLength
75 사용법: %s widget string
76 사용법: %s widget variable variable
77 사용법: %s widget position string
78 사용법: %s widget position variable variable
79 사용법: %s widget fromPosition toPosition string
80 지정한 'from' 위치가 유효하지 않습니다: %s
81 지정한 'to' 위치가 유효하지 않습니다: %s
82 사용법: %s widget firstPosition lastPosition time
83 지정한 첫번째 위치가 유효하지 않습니다: %s
84 지정한 마지막 위치가 유효하지 않습니다: %s
85 지정한 시간이 유효하지 않습니다: %s
86 사용법: %s variable widget x y
87 지정한 x 위치가 유효하지 않습니다: %s
88 지정한 y 위치가 유효하지 않습니다: %s
89 사용법: %s widget left right mode
90 지정한 왼쪽 위치가 유효하지 않습니다: %s
91 지정한 오른쪽 위치가 유효하지 않습니다: %s
92 사용법: %s widget start string direction variable
93 지정한 시작 위치가 유효하지 않습니다: %s
94 사용법: XmListGetSelectedPos variable widget
95 사용법: XmListGetMatchPos variable widget item
96 사용법: XmOptionLabelGadget variable widget
97 사용법: XmOptionButtonGadget variable widget
98 사용법: XmGetVisibility variable widget
99 사용법: XmGetTearOffControl variable widget
100 사용법: XmGetTabGroup variable widget
101 사용법: XmGetPostedFromWidget variable widget
102 사용법: XmGetFocusWidget variable widget
103 사용법: XmFileSelectionDoSearch widget directoryMask
$set 16
1 DtkshCvtNamedValueToString: 맞는 것이 없습니다.
2 DtkshCvtStringToNamedValue: '%s' 문자열을 변환할 수 없습니다.
$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.
$set 25
1  마침
3  수행중
4  [-n] [arg...]
5  [arg...]
6  [dir] [list]
7  [job...]
8  [n]
9  [name [pathname] ]
10  [name]
11  [top] [base]
12  expr...
13  format [arg...]
14  함수입니다.
15  키워드입니다.
16  내장 쉘입니다.
17  반출된 함수입니다.
18  정의되지 않은 함수입니다.
19  name [arg...]
20  optstring name [args...]
21  seconds
23 ${HOME:-.}/.profile
24 %c: 다음 식에 유효하지 않은 문자 - %s
25 %c: 알 수 없는 형식 지정자
26 %d-%d: 유효하지 않은 범위
27 %d: 유효하지 않은 2진 스크립트 버전
28 %s은(는) 다음에 대한 별명입니다:
29 %s은(는) 다음에 대해 반출된 별명입니다:
30 %s이(가) 빠졌습니다.
31 %s은(는) 알 수 없는 기본입니다.
32 %s: '?' 연산자에 대해서는 ':'이 와야 합니다.
33 %s: 애매합니다.
34 %s: 인수는 %job 또는 프로세스 id이어야 합니다.
35 %s: 별명을 찾을 수 없습니다.\n
36 %s: 산술 구문 오류
37 %s: 할당하려면 lvalue가 필요합니다.
38 %s: 잘못된 파일 단위 번호
39 %s: 잘못된 형식
40 %s: 잘못된 번호
41 %s: 잘못된 선택사항(들)
42 %s: 잘못된 대체
43 %s: 잘못된 트랩
44 %s: 만들 수 없습니다.
45 %s: 실행할 수 없습니다.
46 %s: 열 수 없습니다.
47 %s: 0으로 나누기
48 %s: 정의역 예외
49 %s: %s이(가) 실패했습니다.
50 %s: 파일이 이미 있습니다.
51 %s: 잘못된 함수 이름
52 %s: 유효하지 않은 별명 이름
53 %s: 유효하지 않은 규정 함수
54 %s: 유효하지 않은 반출 이름
55 %s: 유효하지 않은 함수 이름
56 %s: 유효하지 않은 이름
57 %s: 유효하지 않은 정규식
58 %s: 유효하지 않은 자체 참조
59 %s: 다음의 유효하지 않은 사용:
60 %s: 유효하지 않은 변수 이름
61 %s: 은(는) 구별자가 아닙니다.
62 %s: 은(는) 읽기 전용입니다.
63 %s: 구현되지 않은 레이블
64 %s: 한계를 초과했습니다.
65 %s: 더 많은 토큰들이 와야 합니다.
66 %s: 부모가 없습니다.
67 %s: 참조 이름이 없습니다.
68 %s: 찾을 수 없습니다.
69 %s: 구현되지 않습니다.
70 %s: 피연산자가 호환되지 않는 유형을 가지고 있습니다.
71 %s: 넘침 예외
72 %s: 매개변수가 설정되지 않았습니다.
73 %s: 매개변수가 NULL이거나 설정되지 않았습니다.
74 %s: 재귀가 너무 깊습니다.
75 %s: 참조 변수는 배열일 수 없습니다.
76 %s: pathname 인수가 필요합니다.
77 %s: 사용이 제한되어 있습니다.
78 %s: 단일성 예외
79 %s: 범위를 벗어난 첨자
80 %s: 균형이 맞지 않는 소괄호
81 %s: 알 수 없는 함수
82 %s: 알 수 없는 지역성
83 %s: 알 수 없는 연산자
84 %s: 알 수 없는 신호 이름
85 %s: 순환을 일으킬 수 있습니다.
86 (coredump)
87 -c는 인수를 요구합니다.
88 -e - 단일 인수를 요구합니다. 
89 /vpix
90 <command unknown>
91 ACE#?F#?H:[name]L#?R#?Z#?fi#?[base]lnprtux [name=[value]...]
92 AE#?F#?HL#?R#?Z#?fi#?[base]lnprtux [name=[value]...]
93 중단
94 Ad:[delim]prst#[timeout]u#[filenum] [name...]
95 알람 호출
96 잘못된 루트 노드 사양
97 잘못된 시스템 호출
98 끊긴 파이프
99 버스 오류
100 작업 조절을 시작할 수 없습니다.
101 현재 선택사항 설정값
102 DIL 신호
103 자식의 종료
104 DircabefhkmnpstuvxCI:[file]o:?[option]A:[name] [arg...]
105 DircabefhkmnpstuvxCo:?[option]A:[name] [arg...]
106 EMT 트랩
107 CPU 시간 한계를 초과했습니다.
108 파일 크기 한계를 초과했습니다.
109 유동 예외
110 HSacdfmnstv [limit]
111 정지
112 IO 신호
113 잘못된 지시
114 가로채기
115 없앰
116 LP [dir] [change]
117 메모리 고장
118 이주 프로세스
119 작업 조절도구가 없습니다.
120 전화 가로채기
121 폴링 알람
122 정전
123 프로파일 시간 알람
124 마침
125 자원이 없어짐
126 이전 tty 드라이버로 되돌리는 중입니다...
127 S [mask]
128 SIGAPOLLO
129 보안 레이블이 바뀌었습니다.
130 소켓 가로채기
131 음향이 끝났습니다.
132 멈춤 (신호)
133 멈춤 (tty 입력)
134 멈춘 프로세스가 계속되었습니다.
135 멈춤
136 멈춤(tty 출력)
137 새 tty 드라이버로 전환하는 중입니다...
138 곧 시스템이 고장납니다.
139 종료됨
140 추적/BPT 트랩
141 인식할 수 없는 버전
142 이 쉘을 종료하려면 'exit'를 사용하십시오.
143 사용자 신호 1
144 사용자 신호 2
145 버전을 정의하지 않았습니다.
146 가상 시간 알람
147 창 크기 바꾸기
148 수행중인 작업이 있습니다.
149 사용자가 작업을 멈추었습니다.
150 [_[:alpha:]]*([_[:alnum:]])
151 \n@(#)Version 12/28/93\0\n
152 \n@(#)Version M-12/28/93\0\n
153 \nreal
154 \r\n\007쉘은 비활동중이므로 60초 내에 시간 종료합니다.
155 a name...
156 a:c [command [args...] ]
157 afpv name...
158 alarm %s %.3f\n
159 alarm -r %s +%.3g\n
160 인수가 와야 합니다.
161 잘못된 디렉토리
162 잘못된 파일 단위 번호
163 잘못된 대체
164 상위 디렉토리를 사용할 수 없습니다.
165 파이프를 만들 수 없습니다.
166 임시 파일을 만들 수 없습니다.
167 분기할 수 없습니다.
168 %s을(를) 가져올 수 없습니다.
169 %s을(를) 설정할 수 없습니다.
170 알람을 설정할 수 없습니다.
171 필요한 조건(들)
172 dsf:[library] [name...]
173 e:[editor]lnrsN# [first] [last]
174 파일 끝
175 f:[format]nprsu:[filenum] [arg...]
176 fnv name...
177 hist -e "${VISUAL:-${EDITOR:-vi}}"
178 기록 파일을 열 수 없습니다.
179 구문이 틀립니다.
180 유형 %c의 유효하지 않은 인수
181 은(는) 다음의 내장 쉘 버전입니다:
182 은(는) 다음에 대해 추적한 별명입니다:
183 kill
184 줄 %d: $ 앞에 \\가 없습니다.
185 줄 %d: ${} 안의 %c는 따옴표로 묶어야 합니다.
186 줄 %d: 알 수 없는 레이블 %s
187 줄 %d: [[...]] 안의 %s는 폐기되었습니다. ((...))를 사용하십시오.
188 줄 %d: '='는 폐기되었습니다. '=='를 사용하십시오.
189 줄 %d: -a는 폐기되었습니다. -e를 사용하십시오.
190 줄 %d: %c 앞의 \\는 나중에 사용하기 위한 예비용입니다.
191 줄 %d: `...`는 폐기되었습니다. $(...)를 사용하십시오.
192 줄 %d: 애매함을 막으려면 %c을(를) 빼십시오.
193 줄 %d: 레이블 %s은(는) 무시됩니다.
194 줄 %d: 애매함을 막으려면 %c을(를) 따옴표로 묶으십시오.
195 줄 %d: 세트 %s은(는) 폐기되었습니다.
196 줄 %d: 내포된 서브쉘에는 빈칸이 필요합니다.
197 줄 %d: $id[...]에서 애매함을 막으려면 중괄호를 사용하십시오.
198 줄 %d: 연산자 %c와(과) %c을(를) 분리하려면 빈칸이나 탭을 사용하십시오.
199 ln#[signum]s:[signame] sig...
200 로그인 setuid/setgid 쉘은 금지되어 있습니다.
201 대응
202 줄바꿈
203 nlp [job...]
204 기록 파일이 없습니다.
205 조회 프로세스가 없습니다.
206 그런 작업은 없습니다.
207 그런 프로세스는 없습니다.
208 지원되지 않습니다.
209 끄기
210 켜기
211 파일 열기 한계를 초과했습니다.
212 메모리가 없습니다.
213 p [action condition...]
214 p [name[=value]...]
215 매개변수를 설정하지 않았습니다.
216 사용 권한이 거부되었습니다.
217 프로세스가 이미 있습니다.
218 ptx [name=[value]...]
219 pvV name [arg]...
220 r [varname seconds]
221 줄 %d에 구문 오류: `%s' %s
222 줄 %d에 구문 오류: 레이블 %s 중복
223 구문 오류: `%s' %s
224 sys
225 입력을 기다리는 중의 시간 종료
226 예상되지 않은
227 다목적으로 사용할 수 없습니다.
228 무제한
229 일치하지 않음
230 사용자
231 버전
232 %d에 쓰지 못했습니다.
233 $_에 우편이 있습니다.
234 0 바이트
