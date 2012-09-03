$ 
$   COMPONENT_NAME: desktop
$ 
$   FUNCTIONS: mnemonic
$		name
$ 
$   ORIGINS: 27,118,119,120,121
$ 
$   This module contains IBM CONFIDENTIAL code. -- (IBM
$   Confidential Restricted when combined with the aggregated
$   modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelp.msg
$ **
$ **  Project:     CDE 1.0 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for DtHelp 
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in dialogs or are displayed to the user.
$ **        These messages are the default and they should ALL BE LOCALIZED.
$ **        Note that these messages do NOT have any identification.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the 
$ *               27th.  (Mike Wilson).
$ *
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****
$ 
$ These Messages appear in the pulldown menus for Cde Help General Help
$ dialog widget.
$ 
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the
$ second is the mnemonic (e.g. 2 P).  
$ 
$ Messages 1 through 6 are the pulldown menus for the File menu.
1 印刷(P)...
2 P
3 閉じる(C)
4 C
5 新規ウィンドウ(W)...
6 W
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 コピー(C)
8 C 
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 索引(I)...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 ホーム・トピック(T)
12 T
13 前のトピックへ(B)
14 B
15 ヒストリ(H)...
16 H
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 概要(v) - ヘルプの使い方
18 v
19 目次(C)
20 C
21 使い方(T)
22 T
23 リファレンス(R)
24 R
25 マウスとキーボードの操作(M)
26 M
27 ヘルプについて(A)
28 A
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the 
$ Help Dialog.
29 ファイル(F)
30 F
31 検索(S)
32 S
33 ナビゲート(N)
34 N
35 ヘルプ(H)
36 H
37 編集(E)
38 E
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ help dialog and quick help dialog widgets.
$ 
50 文字列データをフォーマットできません。
51 動的文字列データをフォーマットできません。
52 マニュアル・ページをフォーマットできません。指定されたマニュアル・ページは存在しないか、変形しています。
53 テキスト・ファイル・データをフォーマットできません。指定されたテキスト・ファイルは存在しないか、変形しています。
54 ヘルプ・トピックをフォーマットできません。
55 存在しない位置 ID:
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 このアプリケーション内では、選択したハイパーテキスト・リンクをサポートしていません。
57 このアプリケーションは、マニュアル・ページへのリンクをサポートしていません。
$ 
$ Messages 58 through 62  are used when bad or null help volumes or 
$ locationId's are used.
$ 
58 指定したオンライン・ヘルプは、インストールされていないか、適切なヘルプ検索パスにありません。オンライン・ヘルプのインストールについては、製品のマニュアルを参照してください。\n
59 位置 ID を指定していません。
60 ヘルプ・ボリュームを指定していません。
61 ヘルプ・ボリューム:
62 位置 ID:
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 存在しないトピック・タイトル
$ 
$ End Message set 2
$ New messages added after early msg cat freeze; not in order
80 指定したヘルプタイプが無効です。



$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 ヘルプ - ヒストリ・ブラウザ
2 既読ヘルプ・ボリューム:
3 閉じる
4 ヘルプ
5 ヘルプ・ファイル
6 マニュアル・ページ
7 ヘルプ・メッセージ
8 既読ヘルプ・トピック:
9 既読ヘルプ・ファイル:
10 既読マニュアル・ページ:
11 既読ヘルプ・メッセージ:


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 ヘルプ - 印刷
$ 10-12: standard printer dialog labels
10 プリンタ:
11 印刷部数:
12 バナーページ・タイトル:
$ 20-25: paper size label and sizes
20 用紙サイズ:
21 レター 8.5 x 11 in
22 リーガル 8.5 x 14 in
23 エグゼクティブ 7.25 x 10.5 in
24 A4 210 x 297 mm
25 B5 182 x 257 mm
$ 30-33: help volume-specific printing options
30 印刷するトピック
31 現在のトピック
32 現在のトピックとサブトピック
33 目次と索引
34 ボリューム内の全トピック

$ 40-42: buttons at bottom of dialog
40 印刷
41 取消し
42 ヘルプ

$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 ヘルプ・ボリューム: 
51 ヘルプ・メッセージ
52 ヘルプ・ファイル

$ 53 "Manual Page: "
53 マニュアル・ページ: 
54 エラー・メッセージ

$ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$ 60 " "
60 
$ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5 
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 ヘルプ - 索引検索
2 検索
3 現在のボリューム
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4  (%s)
5 全ボリューム
6 選択
7 ボリュームを選択...
8 表示
9 完全な索引
10 エントリ指定:
11 行先指定
12 検索を開始
13 検索を継続
14 検索を中止
15 閉じる
16 ヘルプ
17 ボリュームを探しています... %d 個、見つかりました。
18 検索中です... ボリュームの残り: %d
19 索引エントリはありません。
20 全索引
21 エントリ指定 \"%s\"
22 ボリュームを選択していません。
23 上記の検索オプションを選択してください。
$ Do NOT translate messages 25-31
$ " "
25  
26 +
27 -
$ "  +"
28  +
$ "  -"
29  -
$ 30 "       "
30        
$ 31 "         "
31          
$ DO translate messages 32 and above
$ kind of topic messages
32 ヘルプ・メッセージ
33 ヘルプ・ファイル
34 マニュアル・ページ
35 未知のフォーマット
$ specialized title of Search Volume Selection dialog
40 ヘルプ - 検索ボリューム選択
$ additional messages (late addition caused this ordering)
50 索引エントリが見つかりません。[全ボリューム] を試してください。
51 索引エントリが見つかりません。別の単語を試してください。
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 ヘルプの使い方
2 印刷 ...
3 閉じる
4 このアプリケーションには [ヘルプの使い方] はありません。
5 前のトピックへ
6 ヘルプ ...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be 
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 使用方法: %s...\n
2 \t-helpVolume <ヘルプ・ボリューム・ファイル>\n
3 \t-locationId  <ID>\n
5 \t-file  <ASCII テキスト・ファイル>\n
6 \t-man\n
7 \t-manPage <Unix マニュアル・ページ>\n\n\n
$ man page dialog
11 マニュアル・ページ
12 マニュアル・ページ:
13 マニュアル・ページを表示
$ button labels
16 終了
17 閉じる
18 前のトピックへ
19 印刷 ...
$ title of helpview window
20 ヘルプ・ビューア


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box 
$ (e.g. Quick help close button)
2 閉じる
3 内部エラー: マウスをグラブできません。\nDtHelpReturnSelectedWidget が異常終了しました。\n
4 内部エラー: キーボードをグラブできません。\nDtHelpReturnSelectedWidget() が異常終了しました。\n
5 ヘルプ・エラー
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 実行
11 実行禁止
12 ヘルプ・ボリュームがコマンド別名を実行しようとしました。\n\
別名 '%s' が定義されていません。
13 ヘルプ・ボリュームがコマンドを実行しようとしました。\n\
セキュリティのため、自動コマンド実行はオフになっています。\n\
実行しようとしたコマンドは %s です。
14 ヘルプ・ボリュームがrootユーザとしてコマンドを実行しようとしましたが、\n\
\"%s\" のホーム・ディレクトリ ($HOME) がrootユーザのホーム・ディレクトリと \n\
一致しません。予期しないコマンドを実行して終わりました。\n\n\
コマンド: \"%s\"\n\n\
注: 将来は無効にします。\n\
\"su root\" よりむしろ \"su - root\" を実行してください。


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area  
1 ボリューム: 
$ Messages 2 though 5 are used in the Button Box area of our general 
$ help dialog.
2 前のトピックへ
3 ヒストリ...
4 索引...
5 トップレベル


$set 10 
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 ヘルプ - ボリューム選択
2 ヘルプ・ボリューム
3 1 つ以上のヘルプ・ボリュームを選択
4 閉じる
5 ヘルプ


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 閉じる
2 続き ...
3 前のトピックへ
4 印刷 ...
5 ヘルプ ...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)Message catalog source $XConsortium: DtHelp.msg /main/5 1996/10/16 03:12:26 cde-hit $

