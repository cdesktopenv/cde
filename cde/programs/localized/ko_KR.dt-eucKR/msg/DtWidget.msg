$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtWidget.vmsg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt widget
$ **  library.
$ **
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: DtWidget.msg /main/4 1996/06/19 11:43:48 cde-dec $
$ *****************************************************************************
$ *****************************************************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There MAY be three types of messages in this file:
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


$set 2
$ *************************************************************
$ **                                **
$ ** Common messages shared by all DT widgets                **
$ **                                **
$ *************************************************************
$ Default value for DtNdialogCloseButtonLabel resource.
$ This string is used for all "Close" buttons in all dialogs.
1 닫기

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 도움말

$set 3
$ *************************************************************
$ **                                **
$ **  This set of messages is used by the DtEditor widget.  **
$ **                                **
$ *************************************************************

$ **********************************************
$ **                         module: Editor.c **
$ **  Messages for the Format/Settings dialog **
$ **********************************************

$ Default value for DtNformatSettingsDialogTitle resource,
$ title of format settings dialog
1 형식 설정값

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 오른쪽 여백:
3 왼쪽 여백:

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 왼쪽 줄 맞추기
5 오른쪽 줄 맞추기
6 양쪽 줄 맞추기
7 가운데 줄 맞추기

$ Default values for DtNformatParagraphButtonLabel and
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 문단
9 전체

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 철자

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 찾기/바꾸기

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 철자가 틀린 단어:

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 찾기:

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 바꾸기:

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in Find/Change dialog
25 찾기
26 바꾸기
27 전체 바꾸기

$ ******************************************************************
$ **                         module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 현재 문서에서 문자열 %s을(를) 찾을 수 없습니다.

$ Title for general information dialog (e.g. "Find/Change dialog can't
$ find word in document.").
31 정보

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line
40 줄:

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 총 줄수:

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode.
42 겹쳐쓰기 

$ Default value for DtNinsertLabel resource,
$ label used to indicate Editor is in insert mode.
$ The default value is a blank string.
43 

$ ******************************************************************
$ **                                          module: SearchCalls **
$ ** Message displayed when Find/Replace/Spell fails to execute   **
$ ** the spell filter.                                            **
$ ******************************************************************
$ Error message if there is a problem running the spell filter
$ Message 34 and 35 are part of the same message for the error
$ dialog.
$ do not translate %s
$ "NEW-"
50 Error executing spell filter.
$ "NEW-"
51 Check to see if filter, %s, is installed and in the path.

$ Title for Error dialog
$ "NEW-"
52 Editor Error


$set 4
$ *************************************************************
$ **                                **
$ ** MenuButton messages                    **
$ **                                **
$ *************************************************************
$ Warning messsage
$ Note: do not localize the chars XmNmenuPost
1 XmNmenuPost가 잘못된 값을 가지고 있습니다.

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget: 유효하지 않은 상위  
3 DtMenuButtonWidget: 유효하지 않은 부속 메뉴

$set 5
$ *************************************************************
$ **                                **
$ ** ComboBox      messages                     **
$ **                                **
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget: 유효하지 않은 alignment 자원 (기본값 DtALIGNMENT_CENTER)
$ Note: do not localize the chars marginHeight
2 DtComboBoxWidget: 유효하지 않은 marginHeight 자원(기본값 2)
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget: 유효하지 않은 marginWidth 자원(기본값 2)
$ Note: do not localize the chars horizontalSpacing
4 DtComboBoxWidget: 유효하지 않은 horizontalSpacing 자원(기본값 0)
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget: 유효하지 않은 verticalSpacing 자원(기본값 0)
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget: 유효하지 않은 orientation 자원(기본값 DtRIGHT)
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget: 유효하지 않은 itemCount 자원(기본값 0)
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget: 유효하지 않은 selectedPosition 자원(기본값 0)
$ Note: do not localize the chars textField
9 DtComboBoxWidget: textField 자원을 설정할 수 없습니다.
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget: 설정할 항목을 찾을 수 없습니다.(DtComboBoxSetItem)
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget: 선택할 항목을 찾을 수 없습니다.(DtComboBoxSelectItem)
12 크기 바꾸기에 실패했습니다.  아마도 가장 긴 항목이 허용된 빈칸보다 더 넓을 것입니다.

$ Label value
13 조합 상자(ComboBox)

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType: wrongParameters, String to XmRType 변환에는 인수가 필요하지 않습니다.

$ Warning message
15 DtComboBoxDeletePos: wrongParameters, 유효하지 않은 위치 값

$set 6
$ *************************************************************
$ **                                **
$ ** SpinBox       messages                     **
$ **                                **
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget: 유효하지 않은 arrowSensitivity 자원(기본값 DtARROWS_SENSITIVE)
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget: 유효하지 않은 alignment 자원(기본값 DtALIGNMENT_CENTER)
$ Note: do not localize the chars intialDelay
3 DtSpinBoxWidget: 유효하지 않은 initialDelay 자원(기본값 250)
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget: 유효하지 않은 marginHeight 자원(기본값 2)
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget:  유효하지 않은 marginWidth 자원(기본값 2)
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget: 유효하지 않은 arrowLayout 자원(기본값 DtARROWS_BEGINNING)
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget: 유효하지 않은 repeatDelay 자원(기본값 200)
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget: 유효하지 않은 itemCount 자원(기본값 0)
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget: 유효하지 않은 position 자원(기본값 0)
10 DtSpinBoxWidget: 유효하지 않은 position 자원(기본값: 최소값)
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget:  유효하지 않은 decimalPoints 자원(기본값 0)
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget: 유효하지 않은 minimum 자원(기본값: 최대값)
$ Note: do not localize the chars textField
13 DtSpinBoxWidget:  textField 자원을 설정할 수 없습니다.
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget: 설정할 항목을 찾을 수 없습니다.(DtSpinBoxSetItem)

$ Label value
15 돌림 상자(SpinBox)
