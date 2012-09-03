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
$ **  $XConsortium: DtWidget.msg /main/4 1996/06/19 11:44:41 cde-dec $
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
1 关闭

$ Default value for DtNdialogHelpButtonLabel resource.
$ This string is used for all "Help" buttons in all dialogs.
2 提示

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
1 格式设定

$ Default values for DtNlMarginFieldLabel and DtNrMarginFieldLabel resources,
$ labels for margin fields in Format Settings dialog
2 右边界：
3 左边界：

$ Default values for DtNlAlignToggleLabel, DtNrAlignToggleLabel,
$ DtNjustifyToggleLabel, and DtNcenterToggleLabel resources,
$ labels for justification radio buttons in Format Settings dialog
4 左对齐
5 右对齐
6 对齐
7 居中

$ Default values for DtNformatParagraphButtonLabel and 
$ DtNformatAllButtonLabel resources,
$ labels for paragraph and all buttons in Format Settings dialog
8 段落
9 所有

$ ******************************************************************
$ **                                              module Editor.c **
$ ** Strings used in the spell checking and find/change dialogs.  **
$ ******************************************************************

$ Default value for DtNspellDialogTitle resource,
$ title of the Spell dialog
20 拼写

$ Default value for DtNfindDialogTitle resource,
$ title of the Find/Change dialog
21 查找/更改

$ Default value for DtNmisspelledListLabel resource,
$ label for list of unrecognized and misspelled words
22 拼错的字：

$ Default value for DtNfindFieldLabel resource,
$ label of field in which user types string for which to search.
23 查找：

$ Default value for DtNchangeFieldLabel resource,
$ label of field in which user types the replacement string
24 更改为：

$ Default values for DtNfindButtonLabel, DtNchangeButtonLabel, and
$ DtNchangeAllButtonLabel resources,
$ labels for find, change, and change all buttons in Find/Change dialog
25 查找
26 更改
27 更改所有的

$ ******************************************************************
$ ** 					      module: SearchDlg.  **
$ ** Message displayed when Find/Replace/Spell fails to find the  **
$ ** specified string or word.                                    **
$ ******************************************************************
$ Used to display "Unable to find the string FOO in the current document."
$ Note: Do not localize the characters %s.
30 不能在当前文档中找到字符串 %s 。

$ Title for general information dialog (e.g. "Find/Change dialog can't 
$ find word in document.").
31 信息

$ ***********************************************************
$ **                                       module Editor.c **
$ ** Messages and strings associated with the status line. **
$ ***********************************************************

$ Default value for DtNcurrentLineLabel resource,
$ label for the display showing the current insert cursor line 
40 行：

$ Default value for DtNtotalLineCountLabel resource,
$ label for the display showing the total number of lines in the document
41 总共：

$ Default value for DtNoverstrikeLabel resource,
$ label used to indicate Editor is in overstrike (overtype) mode. 
42 覆盖

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
$ ** 							     ** 
$ ** MenuButton messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsage
$ Note: do not localize the chars XmNmenuPost
1 XmNmenuPost 具有非法的值。

$ Error messsages
$ Note: do not localize the chars DtMenuButtonWidget in messages 2 and 3
2 DtMenuButtonWidget：无效的父菜单。
3 DtMenuButtonWidget：无效的子菜单。

$set 5
$ *************************************************************
$ ** 							     ** 
$ ** ComboBox  	messages 				     **
$ ** 							     ** 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtComboBoxWidget in messages 1 - 11
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
1 DtComboBoxWidget：无效的 alignment 资源（设置成默认的 DtALIGNMENT_CENTER）。
$ Note: do not localize the chars marginHeight 
2 DtComboBoxWidget：无效的 marginHeight 无效的资源（设置成默认的２）。
$ Note: do not localize the chars marginWidth
3 DtComboBoxWidget：无效的 marginWidth 资源（设置成默认的２）。
$ Note: do not localize the chars horizontalSpacing
4 DtComboBoxWidget：无效的 horizontalSpacing 资源（设置成默认的０）。
$ Note: do not localize the chars verticalSpacing
5 DtComboBoxWidget：无效的 verticalSpacing 资源（设置成默认的０）。
$ Note: do not localize the chars orientation and DtRIGHT
6 DtComboBoxWidget：无效的 orientation 资源（设置成默认的 DtRIGHT）。
$ Note: do not localize the chars itemCount
7 DtComboBoxWidget：无效的 itemCount 资源（设置成默认的０）。
$ Note: do not localize the chars selectedPosition
8 DtComboBoxWidget：无效的 selectedPosition 资源（设置成默认的０）。
$ Note: do not localize the chars textField
9 DtComboBoxWidget：不能设置 textField 资源。
$ Note: do not localize the chars DtComboBoxSetItem
10 DtComboBoxWidget：不能找到设置（DtComboBoxSetItem）的项。
$ Note: do not localize the chars DtComboBoxSelectItem
11 DtComboBoxWidget：不能找到选择（DtComboBoxSelectItem）的项。
12 缩放失败。 可能最长的项要比允许的空间还宽。

$ Label value
13 组合框

$ Error message
$ Note: do not localize the chars String and XmRType
14 cvtStringToType：参数错，String 到 XmRType 的变换不需要额外的实参

$ Warning message
15 DtComboBoxDeletePos: 参数错，非法的位置值

$set 6
$ *************************************************************
$ ** 							     ** 
$ ** SpinBox   	messages 				     **
$ ** 							     s* 
$ *************************************************************
$ Warning messsages
$ Note: do not localize the chars DtSpinBoxWidget in messages 1 - 14
$ Note: do not localize the chars arrowSensitivity and DtARROWS_SENSITIVE
1 DtSpinBoxWidget：无效的 arrowSensitivity 资源（设置成默认的 DtARROWS_SENSITIVE）。
$ Note: do not localize the chars alignment and DtALIGNMENT_CENTER
2 DtSpinBoxWidget：无效的 alignment 资源（设置成默认的 DtALIGNMENT_CENTER）。
$ Note: do not localize the chars intialDelay
3 DtSpinBoxWidget：无效的 initialDelay 资源（设置成默认的 250）。
$ Note: do not localize the chars marginHeight
4 DtSpinBoxWidget：无效的 marginHeight 资源（设置成默认的 2）。
$ Note: do not localize the chars marginWidth
5 DtSpinBoxWidget： 无效的 marginWidth 资源（设置成默认的 2）。
$ Note: do not localize the chars arrowLayout and DtARROWS_BEGINNING
6 DtSpinBoxWidget：无效的 arrowLayout 资源（设置成默认的 DtARROWS_BEGINNING）。
$ Note: do not localize the chars repeatDelay
7 DtSpinBoxWidget：无效的 repeatDelay 资源（设置成默认的 200）。
$ Note: do not localize the chars itemCount
8 DtSpinBoxWidget：无效的 itemCount 资源（设置成默认的 0）。
$ Note: do not localize the chars position in messages 9 and 10
9 DtSpinBoxWidget：无效的 position 资源（设置成默认的 0）。
10 DtSpinBoxWidget：无效的 position 资源（设置成默认的最小）。
$ Note: do not localize the chars decimalPoints
11 DtSpinBoxWidget：无效的 decimalPoints 资源（设置成默认的 0）。
$ Note: do not localize the chars minimum
12 DtSpinBoxWidget：无效的 minimum 资源（设置成默认的最大）。
$ Note: do not localize the chars textField
13 DtSpinBoxWidget： 不能设置 textField 资源。
$ Note: do not localize the chars DtSpinBoxSetItem
14 DtSpinBoxWidget：不能找到设置（DtSpinBoxSetItem） 的项。

$ Label value
15 旋转框
