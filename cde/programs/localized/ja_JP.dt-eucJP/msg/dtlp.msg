
$ "$XConsortium: dtlp.msg /main/6 1996/10/16 03:16:39 cde-hit $";

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlp.msg
$ **
$ **  Project:     DT printer utility (dtlp)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlp
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1990, 1992, 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1994 International Business Machines Corp.
$ ** (c) Copyright 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1994 Novell, Inc.
$ **
$ **     All Rights reserved
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * You don't need to localize the following sets:
$ *
$ * Please localize the following sets:
$ *  sets 1
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$quote "

$set 1
$ 
$  -- The first group of messages all appear on the Print dialog --
$ 


$  This is the message that appears in the "Printer Name:" field when no
$  printer name has been specified and no there is no default printer.
10 "デフォルト"

$  This is the label that appears in the title bar of the print dialog.
20 "印刷"

$  This is the label for the field indicating the file that will be printed.
30 "ファイル:"

$  This is the label for the field indicating the name of the printer.
40 "プリンタ:"

$  This is the message that will appear in the "Printer:" field if no printer
$  has been specified.
50 "デフォルト"

$  This is the label for the field indicating the number of copies to print.
60 "印刷部数:"

$  This is the label for the field indicating the title for the banner page.
70 "バナーページのタイトル:"

$  This is the label for the field indicating if page numbers are to be added.
80 "ページ番号印刷"

$  This is thef label for the field indicating additional options for the
$  "lp" command.
90 "印刷コマンドオプション:"

$  This is the label on the left button. Pushing this button causes the
$  dialog to disappear and the print operation to start.
100 "印刷"

$  This is the label on the center button. Pushing this button causes the
$  dialog to disappear and cancels the print operation.
110 "取消し"

$  This is the label on the right button. Pushing this button causes a
$  help dialog to be posted.
120 "ヘルプ"

$ 
$  -- The following are error messages. --
$ 

$  This message is posted when there is a problem with the file specified. 
$  The file name will be appended to the message. One additional message
$  from the list below will also be included in the dialog to provide
$  more details.
130 "印刷できません:"

$  This message is added to the error dialog when the file is not printable 
$  or does not exist.
132 "指定のファイルは印刷可能なファイルではないか、存在しません。"

$  This message is added to the error dialog when the file is not readable 
$  by the user invoking dtlp.
134 "ユーザは、そのファイルの読み取り権を持っていません。"

$  This message is added to the error dialog when there is a general failure 
$  in setting up to print.
136 "ファイルが存在するかどうか確認してください。"

$  This is the label that appears in the title bar of the error dialog.
160 "印刷情報"

$ 
$  -- Strings used in the Print dialog and Select Printer dialog in CDEnext --
$ 
$  This is the label for the field indicating the file(s) that will be printed.
170 "ファイル名: "

$  This is used as the window title of the Select Printer dialog and a button
$  in that dialog.
180 "プリンタ選択"

$  This is the label for the printer list in the Select Printer dialog.
190 "有効なプリンタ:"

$  This is the label for the text box in the Select Printer dialog.
200 "プリンタ情報:"

$ 
$  -- The following are error messages added for CDEnext --
$ 
$  File is not printable or does not exist.
210 "そのファイルは印刷可能なファイルではないか、存在しません。"

$  File is not printable or does not exist.
220 "システム上でプリンタ定義がされていないか、プリンタが印刷可能状態にありません。"

$  Printer does not exist.
230 "プリンタは存在しません。"

$  Printer is not enabled.
240 "プリンタは印刷可能状態にありません。"

$  No default printer.
250 "デフォルトプリンタではありません。"

$  Printer not accepting requests.
260 "プリンタはリクエストを受け付けません。"

$  Number of copies is not a positive integer.
270 "印刷部数は正数でなければなりません。"


$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/6 1996/10/16 03:16:39 cde-hit $
