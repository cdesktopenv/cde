$ $TOG: dtpad.msg /main/22 1999/09/15 14:21:59 mgreess $
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
$quote "
$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************
$quote
$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
1 ヘルプ
$ Unrecognized arguments
2 dtpad: オプション %s は不正です。
$ Extra arguments
3 dtpad: 余分な引き数 %s があります。


$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 ファイルを開く

$ label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
2 File to Open:

$ Title of the Include dialog
3 ファイルを取り込む

$ Label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
4 File to Include:

$ Error when printing a file and a temporary copy can not be saved.
5 一時ファイルを作成できません。


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
1 変更を %s に保存しますか?

$ This is used as-is, whenever the current file is unnamed.
2 現在のファイルには名前がありません。\n\
保存しますか?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***	Currently, dropped files are INSERTED in the text rather than
$		opened for editing so this message is no longer used and need
$		not be localized.
3 Do you want to save the current\ncopy of %s\nbefore you Edit %s?

$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.
5 ファイル %s はすでに存在します。\n\
この編集セッションの内容でファイルを上書きしますか?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 警告

$ Title of "Save As" dialog.
7 別名保存

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***	These labels are no longer used and need not be localized.
$		They've been replaced by messages 44 and 45
8 Add newline characters to the end of word-wrapped lines.
9 Do not add newlines.  Only line breaks created by [Return]\nwill be preserved.

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 いいえ
15 はい
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 保存しますか?

$ Title of general error dialog
17 エラー

$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message
18 終了確認

$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.
20 インフォメーション

$ Default file name in Save As dialog box.
21 タイトルなし

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 %1$s を保存できません。\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 ファイルを保存できません。\nファイル名を指定していません。

34 ファイルへの書き込みは禁止されています。

35 ファイルはディレクトリです。

36 ファイルはキャラクタ型特殊デバイスです。

37 ファイルはブロック・モード・デバイスです。

$ ***UNUSED***	This message should never appear since currently there is
$		only one supported file format - text.  And this type is
$		hard coded into dtpad.
38 Invalid file format.

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 未知の理由 - ディスク容量が不足している可能性があります。

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 変更内容を保存しますか?

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 変更内容を \"%s\" に保存しますか?

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***	This title is no longer used and need not be localized.
42 Copy As

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 ファイルにコピー

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 折返し行の最後に改行文字を追加する。
45 改行を追加しない。[リターン] による行分割のみを保存する。
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This 
$ new message is in response to bug DTS CISlx19530.
$ 
46 ファイルが書き込み禁止か、\nファイルへのパスが存在しません。

$ 
$ "-NEW"
$ 
47 "Operation failed due to insufficient memory.\nTry increasing swap space."

$ 
$ "-NEW"
$ 
48 "Format operation failed."

$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 このファイルは読み取り専用で開きました。
2 このファイルは存在しません。
3 ディレクトリは編集できません。
4 キャラクタ型特殊デバイス・ファイルは編集できません。
5 ブロック型デバイス・ファイルは編集できません。
6 ファイルに埋め込まれていたヌル文字は取り除かれました。
7 ファイルをロードできません (メモリが足りません)。
8 ファイルにアクセスできません (そのファイルの状態を定義できません)。
9 そのファイルからの読み取りはできません (読み取り権が無いと思われます)。

$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 表示専用
11 読み取り専用
12 ファイルを読み取れません (未知の理由です)。


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
2 テキスト・エディタ -

$ Default file name used in the main window title.
$ ***UNUSED***	This mesage has been replaced by message 6 of this set and
$		need not be localized.
3 (unnamed)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 デスクトップ・サービスを初期化できません...テキスト・エディタを終了します。

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 ディスプレイを開くことができません...テキスト・エディタを終了します。

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (タイトルなし)

$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 エラー: %s 上の dtpad サーバは\n\
このディスプレイに対してすでにサービス中です。
8 エラー: dtpad サーバはすでに\n\
このディスプレイに対してサービス中です。
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' は不正なオプションです。\n\
使用方法: %s...\n\
   -saveOnClose\n\
\t変更があったが保存されずにテキスト・エディタが綴じられる場合、\n\
\tDtpad は自動的に現在のテキストを保存します。\n\n\
   -missingFileWarning\n\
\t指定したファイルが存在しない場合や、アクセスできない\n\
\t場合は警告ダイアログを表示します。\n\n\
   -noReadOnlyWarning\n\
\tユーザが書き込み権を持っていないファイルが指定された場合に、\n\
\t警告ダイアログが表示されるのを行わないようにします。\n\n\
   -noNameChange\n\
\t現在のテキストを読み込み時と違うファイル名で保存する場合、\n\
\tそのテキストに関するデフォルト・ファイル名を変更しないように\n\
\tします。\n\n\
   -viewOnly\n\
\t編集ウィンドウでのテキストの編集をできないようにします。\n\
\tすなわち、テキスト・エディタをテキスト・ビューアとして用います。\n\
   -statusLine\n\
\t編集ウィンドウの下段にステータス行を表示します。\n\
\tステータス行はテキスト・カーソルの現在位置している行の行番号\n\
\tを知らせます。\n\n\
   -wrapToFit\n\
\t行折返しモードをオンにします。\n\n\
   -workspaceList WorkspaceList\n\
\t起動するテキスト・エディタの編集ウィンドウを、指定した\n\
\t1 つあるいは複数のワークスペースに表示します。\n\n\
   -session SessionFile\n

10 \t以前の CDE シャットダウン時のテキスト・エディタのすべての\n\
\tテキスト編集ウィンドウと設定を復元します。\n\
\tこのオプションが指定された場合、他のすべてのコマンド行オプ\n\
\tションは無視されます。\n\n\
   -standAlone\n\
\t起動するテキスト・エディタを、強制的にテキスト・エディタ・サ\n\
\tーバから独立し、独立した処理を、独立したウィンドウで行うよう\n\
\tにします。\n\n\
   -noBlocking\n\
\tテキスト・エディタ・サーバがテキスト・エディタ・リクエスタから\n\
\tの編集要求を処理できると判断すればすぐに、テキスト・エディタ・\n\
\tリクエスタ・プロセスを終了します。\n\n\
   -server\n\
\t(もしテキスト・エディタ・サーバがまだ実行されていない場合)\n\
\tディスプレイに対する以降のすべての編集要求を処理できるよう\n\
\t強制的にテキスト・エディタ・サーバを起動します。\n\n\
   -exitOnLastClose\n\
\tディスプレイに対する最後の編集ウィンドウが閉じられた時に、\n\
\tテキスト・エディタ・サーバ・プロセスを終了するように指定します。\n\
\tこのオプションはサーバ・プロセスにのみ適用されるので、\n\
\t-server オプションと一緒に使用しなければなりません。\n\n\
これらのオプションの詳細は dtpad のマニュアル・ページを参照してください。\n\n
$ 

$ "NEW-"
11 "Exiting text editor ..."
$ "NEW-"
12 "Running in standalone mode ..."
$ "NEW-"
13 "Could not connect to ToolTalk service:\n%s\n%s\n"

$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 ファイルのアクセス権を確認してください。
2  %s は、ユーザのセッションを保存するのに使用しようとしたファイルです。


$set 11
$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************

$ ***UNUSED*** 	Messages 1-14 are no longer used and need not be localized.
$		All of the mnemonics and labels relating to the "Help" menu
$		have been replaced by messages 81-96.

$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
1 H
2 Help

$ mnemonic and label for the "Overview" item
3 V
4 Overview...

$ mnemonic and label for the "Tasks" item
5 T
6 Tasks...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 Reference...

$ mnemonic and label for the "On Item" item
9 O
10 On Item

$ mnemonic and label for "Using Help" item
11 U
12 Using Help...

$ mnemonic and label for "About Text Editor" item
13 A
14 About Text Editor...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 ファイル(F)

$ mnemonic and label for the "New" item
17 N
18 新規(N)

$ mnemonic and label for the "Open" item
19 O
20 開く(O)...

$ mnemonic and label for the "Include" item
21 I
22 取込み(I)...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 S
24 保存(S)

$ mnemonic and label for the "Save As" item
25 A
26 別名保存(A)...

$ mnemonic and label for the "Print..." item
27 P
28 印刷(P)...

$ mnemonic and label for the "Close" item
29 C
30 閉じる(C)

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 E
32 編集(E)

$ mnemonic and label for the "Undo" menu item
33 U
34 元に戻す(U)

$ mnemonic and label for the "Cut" menu item
35 t
36 カット(t)

$ mnemonic and label for the "Copy" menu item
37 C
38 コピー(C)

$ mnemonic and label for the "Paste" menu item
39 P
40 ペースト(P)

$ mnemonic and label for the "Clear" menu item
41 e
42 消去(e)

$ mnemonic and label for the "Delete" menu item
43 D
44 削除(D)

$ ***UNUSED***	Messages 45-50 are no longer used and need not be localized.
$		They have been replaced by messages 101-106 of this set.
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
52 書式(r)

$ mnemonic and label for the "Settings..." menu item
53 S
54 設定(S)...

$ mnemonic and label for the "Paragraph" menu item
55 P
56 パラグラフ(P)

$ mnemonic and label for the "All" menu item
57 l
58 すべて(l)

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 O
60 オプション(O)

$ mnemonic and label for the "Overstrike" toggle button
61 O
62 上書き(O)

$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***	This label and mnemonic are no longer used and need not be
$		localized.  They've been replaced by messages 111 and 112.
63 W
64 Word Wrap

$ mnemonic and label for the "Status Line" toggle button
65 S
66 ステータス行(S)

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** 	This nmemonic and label are no longer used and need not be
$		localized.  They've been replaced by messages 87 and 88.
71 C
72 Table of Contents...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 保存(S) - 変更あり


$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 H
82 ヘルプ(H)

$ mnemonic and label for the "Overview" item
83 v
84 概要(v)

$ mnemonic and label for the "Tasks" item
85 T
86 使い方(T)

$ nmemonic and label for the "Table of Contents" item
87 C
88 目次(C)

$ mnemonic and label for the "Reference" item
89 R
90 リファレンス(R)

$ mnemonic and label for the "On Item" item
91 O
92 アイテムヘルプ(O)

$ mnemonic and label for "Using Help" item
93 U
94 ヘルプの使い方(U)

$ mnemonic and label for "About Text Editor" item
95 A
96 テキスト・エディタについて(A)


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 S
102 すべてを選択(S)

$ mnemonic and label for the "Find/Change" menu item
103 F
104 検索／変更(F)...

$ mnemonic and label for the Check Spelling menu item
105 k
106 スペルチェック(k)...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***	This label is no longer used and need not be localized.
107 A
108 Copy As...

$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 F
110 ファイルにコピー(F)...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 W
112 行折返し(W)

$ label for the "Print" item
113 印刷

$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 テキスト・エディタ内で項目を 1 つ選択しなければなりません。


$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 チェックポイントの保存に失敗しました。
2 チェックポイントの返信に失敗しました。

$ NEW message after the SAMPLE IMPLEMENTATION.... more errors/warnings can 
$ be detected while reading in a file via ToolTalk.
3 テキスト・エディタの内容を設定できません (原因は不明です)。


$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SETS 14 - 15 WERE ADDED TO SUPPORT CDEnext PRINTING
$ **
$ **	The following set was added to support CDEnext printing based on
$ **	the Xp Print Server.
$ **
$ ****************************************************************************
$ ****************************************************************************

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 14
$ **
$ ****************************************************************************
$ ****************************************************************************
$set 14
$ 
$	Messages 1 - 17 are used in printJob.
$ 
$	This is warning message is displayed when the user has provided
$	a margin specifier in the PrintSetup dialog which cannot be parsed.
$	
$quote "
1 "以下のマージンの指定は間違っています。: \n %s \n %s \n %s \n %s \n\
デフォルトの値で続けます。"
$ 
$	Messages 2 - 7 are messages for errors which can occur in the
$	process of loading a dtpad document file for printing.	
$ 
2 "ファイルが存在しません。"
3 指定されたファイルはディレクトリです。
4 "ファイルタイプエラー。"
5 "ファイルにヌル文字が含まれています。"
6 "ファイルを読み込めません。(メモリ不足です)"
7 "ファイルに読み込み権限がありません。"

$	Messages 8 - 11 are format strings used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 8 is the format string used to make a Date header/footer.
$	Message 9 is the format string used to make a DocName header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
8 "日付:  %s"
9 "ドキュメント:  %s"
10 "ページ番号 %d of %d"
11 "ユーザ名:  %s"

$ 
$	Messages 12 - 13 are used in printSetup to construct the
$	app-specific printing options for DtMail which will be included
$	in the DtPrintSetupBox.
$ 
$	Message 12 is the string labeling the field in the GUI which
$	contains the name of the document.
$	Message 13 is the label used on the Toggle Button to indicate that
$	lines which would extend beyond the edge of the printed page be 
$	"wrapped" to the next line.
12 "ドキュメント名: "
13 "Wrap To Fit"


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
14 "'ファイルに出力'は不成功でした。"
15 "'ファイルに出力'は成功で終了しました。"

$          Messages 16 - 19 are obsolete, and have been replaced by 25
$ 
$	Messages 16 - 19 are status messages for the popup PDM function.
$ 
$	Message 16 indicates that there was no pdm deamon running.
$	Message 17 indicates that the pdm deamon was unable to start a pdm.
$	Message 18 indicates that the pdm couldnt connect to the video display.
$	Message 19 indicates that the pdm couldnt connect to the print display.
16 "環境中にPDMが見つかりません。"
17 "PDMを起動できません。"
18 "PDMはビデオ・ディスプレイに接続を認めません。"
19 "PDMはプリント・ディスプレイに接続を認めません。"

$	Messages 20 - 22 are status messages displayed while printing
$ 
$	Message 20 indicates that the user has asked to close the editor
$		   window.  Dtpad is prevented from doing so until the
$		   active print jobs have completed.
$	Message 21 status message displayed when the print job is started.
$	Message 22 status message displayed as the print job progresses.
20 "保留を閉じます。:  待っているタスクを終了します。..."
21 "印刷中 %s ..."
22 "印刷中 %s: ページ %d の %d ..."

$	Message 23 is used in the construction of a title string for the
$		   Print Setup dialog.
23 "印刷設定 - "

$	Message 24 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
$ "NEW-"
24 "The X Print Server is temporarily out of resources"

$       Messages 25 is used to handle errors from the PDM
25	"印刷・ダイアログ・マネージャ(PDM) エラー - 設定失敗."

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 15
$ **
$ ****************************************************************************
$ ****************************************************************************
$ 
$set 15
$ 
$	Messages 1-5 are used in printSetup.c in the pulldown menu for the
$ 
$	Message 1 specifies that the page header/footer field be empty.
$	Message 2 put the Date in the page header/footer field.
$	Message 3 put the Document Name in the page header/footer field.
$	Message 4 put the Page Nmbr msg header in the page header/footer field.
$	Message 5 put the User Name msg header in the page header/footer field.
1 "なし"
2 "日付"
3 "ドキュメント名"
4 "ページ番号"
5 "ユーザ名"

$	Messages 6-9 are used to construct error messages informing users
$	that they have input an invalid print page margin specification for 
$ 
6 "上マージンの指定は無効です。:  "
7 "左マージンの指定は無効です。:  "
8 "下マージンの指定は無効です。:  "
9 "右マージンの指定は無効です。:  "
