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
$ **  $XConsortium: DtWidget.msg /main/5 1996/10/16 03:12:55 cde-hit $
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
$ ** 							     ** 
$ ** Common messages shared by all DT widgets                **
$ ** 							     ** 
$ *************************************************************
$ Default value for DtNdialogCloseButtonLabel resource.
$ This string is used for all "Close" buttons in all dialogs.
1 閉じる

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 ヘルプ

$set 3
$ *************************************************************
$ ** 							     ** 
$ **  This set of messages is used by the DtEditor widget.  **
$ ** 							     ** 
$ *************************************************************

$ **********************************************
$ **                         module: Editor.c **
$ **  Messages for the Format/Settings dialog **
$ **********************************************

$ Default value for DtNformatSettingsDialogTitle resource,
$ title of format settings dialog
1 書式の設定

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 右マージン:
3 左マージン:

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 左揃え
5 右揃え
6 両端揃え
7 中央揃え

$ Default values for DtNformatParagraphButtonLabel and 
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 パラグラフ
9 すべて

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 スペルチェック

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 検索／変更

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 スペルミスの単語:

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 検索:

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 変更後の単語:

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in 検索／変更 dialog
25 検索
26 変更
27 すべてを変更

$ ******************************************************************
$ ** 					      module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 現在の文書内に文字列 %s は見つかりません。

$ Title for general information dialog (e.g. "Find/Change dialog can't 
$ find word in document.").
31 インフォメーション

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line 
40 行:

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 合計:

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode. 
42 上書き

$ Default value for DtNinsertLabel resource,
$ label used to indicate Editor is in insert mode. 
$ The default value is a blank string.
43 

$ ******************************************************************
$ ** 					      module: SearchCalls **
$ ** Message displayed when Find/Replace/Spell fails to execute   **
$ ** the spell filter.                                            **
$ ******************************************************************
$ Error message if there is a problem running the spell filter
$ Message 34 and 35 are part of the same message for the error
$ dialog. 
$ do not translate %s
$ "NEW-"
50 スペル・フィルタ実行エラー
51 フィルタ %s がインストールされているかパスが正しいか確認してください。

$ Title for Error dialog
$ "NEW-"
52 エディタ・エラー

$set 4
$ *************************************************************
$ ** 							     ** 
$ ** MenuButton messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsage
$ Note: do not localize the chars XmNmenuPost
1 XmNmenuPost の値が不正です。

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget: 親が無効です。
3 DtMenuButtonWidget: サブメニューが無効です。

$set 5
$ *************************************************************
$ ** 							     ** 
$ ** ComboBox  	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget: alignment リソースが無効です。(DtALIGNMENT_CENTER を設定します。)
$ Note: do not localize the chars marginHeight 
2 DtComboBoxWidget: marginHeight リソースが無効です。(2 を設定します。)
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget: marginWidth リソースが無効です。(2 を設定します。)
$ Note: do not localize the chars horizontalSpacing
4 DtComboBoxWidget: horizontalSpacing リソースが無効です。(0 を設定します。)
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget: verticalSpacing リソースが無効です。(0 を設定します。)
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget: orientation リソースが無効です。(DtRIGHT を設定します。)
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget: itemCount リソースが無効です。(0 を設定します。)
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget: selectedPosition リソースが無効です。(0 を設定します。)
$ Note: do not localize the chars textField
9 DtComboBoxWidget: textField リソースを設定できません。
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget: 設定する項目が見つかりません。(DtComboBoxSetItem)
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget: 選択する項目が見つかりません。(DtComboBoxSelectItem)
12 サイズ変更に失敗しました。最長項目が、許されているスペースよりも長い可能性があります。

$ Label value
13 コンボボックス

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType: パラメータが不正です。String から XmRType への変換には特別な引き数は必要ありません。

$ Warning message
15 DtComboBoxDeletePos: パラメータが不正です。位置の値が無効です。

$set 6
$ *************************************************************
$ ** 							     ** 
$ ** SpinBox   	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget: arrowSensitivity リソースが無効です。(DtARROWS_SENSITIVE を設定します。)
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget: alignment リソースが無効です。(DtALIGNMENT_CENTER を設定します。)
$ Note: do not localize the chars intialDelay 
3 DtSpinBoxWidget: initialDelay リソースが無効です。(250 を設定します。)
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget: marginHeight リソースが無効です。(2 を設定します。)
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget: marginWidth リソースが無効です。(2 を設定します。)
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget: arrowLayout リソースが無効です。(DtARROWS_BEGINNING を設定します。)
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget: repeatDelay リソースが無効です。(200 を設定します。)
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget: itemCount リソースが無効です。(0 を設定します。)
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget: position リソースが無効です。(0 を設定します。)
10 DtSpinBoxWidget: position リソースが無効です。(最小値を設定します。)
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget: decimalPoints リソースが無効です。(0 を設定します。)
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget: minimum リソースが無効です。(最大値を設定します。)
$ Note: do not localize the chars textField
13 DtSpinBoxWidget: textField リソースを設定できません。
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget: 設定する項目(DtSpinBoxSetItem)が見つかりません。

$ Label value
15 スピンボックス



