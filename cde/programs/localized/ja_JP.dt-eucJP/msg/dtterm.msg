$ $TOG: dtterm.msg /main/5 1999/09/17 13:29:26 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
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
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	セパレータ             f.separator\n\
	メニュー\\バーの\\有無  f.send_msg %d\
	
2 %s: -S オプションは、-server オプションまたは -serverid オプションと一緒には使用できません。\n
3 端末エミュレータ

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132			80<->132 カラムのエスケープ・シーケンスを使用する／使用しない
2 -/+aw				自動折返しを使用する／使用しない
3 -bg color			バックグラウンド・カラー
4 -/+bs				端末エミュレータのバックグラウンド・カラーの選択をオン／オフにする
5 -display displayname		接続する X サーバ
6 -e command args		実行するコマンド
7 -fb fontset			ボールド・テキスト・フォント
8 -fg color			フォアグラウンド・カラー
9 -fn fontset			通常テキスト・フォント
10 -geometry geom		ウィンドウのサイズ(文字単位)と位置
11 -help			このメッセージを出力
12 -/+iconic			アイコン表示を起動する／起動しない
13 -/+j				ジャンプ・スクロールを使用可能にする／不可にする
14 -/+kshMode			ksh モードを使用可能にする／不可にする
15 -/+l				ログ採取を可能／不可にする
16 -lf filename			ログ採取ファイル名
17 -/+ls			ログイン・シェルを使用可能にする／不可にする
18 -/+map			pty 出力上のマップ・ウィンドウを使用可能にする／不可にする
19 -/+mb			マージン・ベルを使用可能にする／不可にする
20 -ms color			ポインタ・カラー
21 -n string			アイコン名を指定する
22 -name string			クライアント・インスタンス、アイコンおよびタイトル文字列
23 -nb distance			右マージン・ベルの位置を指定
24 -/+rw			逆折り返しを使用可能にする／不可にする
25 -/+sb			スクロール・バーを使用可能にする／不可にする
26 -/+sf			SUN ファンクション・キーを使用可能にする／不可にする
27 -sl number[s]		保存するスクロール行数[画面数]
28 -ti name			プログラムの認識に使われる文字列
29 -title string		ウィンドウのタイトル文字列
30 -tm string			端末モード・キーワードおよび文字
31 -tn name			TERM 環境変数名
32 -/+vb			可視ベルを使用可能にする／不可にする
33 -xrm resourcestring		追加リソース仕様
34 -C				コンソール・モード
35 -Sxxd			"ttyxx" ファイル記述子 "d" のスレープ・モード
36 -Sxxx.d			"ttyxxx" ファイル記述子 "d" のスレープ・モード
37 End-Of-List
38 -e オプションを使用する場合は、コマンド行の最後でなければなりません。
39 そうしなければ、ユーザのデフォルト・シェルが起動されます。
40 プラス記号(+)で始まるオプションは、デフォルトを復元します。
41 End-Of-List
47 %s:  コマンド行オプションが不正です。"%s"\r\n\n
48 使用方法:  %s
49 \r\n   
50  [%s]
51 \r\n\n詳細説明は、"%s -help" と入力してください。\r\n\n
52 使用方法:\n
53 \t%s [-options ...] [-e コマンド 引き数]\n\n
54 ここでは、オプションは以下のとおりです:\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 メモリがいっぱいです。\n消去するには、[了解] を押してください。

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
2 端末エミュレータ - 警告

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 メモリがいっぱいです。\n消去するには、[了解] を押してください。

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
4 端末エミュレータ - マニュアルページ
5 端末エミュレータ - ヘルプ

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
2 了解
3 取消し
4 ヘルプ
5 グローバル
6 カーソルの制御
7 ボックス
8 下線
9 カーソルの形状
10 使用する
11 使用しない
12 点滅カーソル
13 点滅間隔(ミリ秒)
14 表示しない
15 blinkRate
16 カラーの制御
17 反転
18 通常
19 ウィンドウのバックグラウンド
20 スクロール動作
21 使用しない
22 使用する
23 スムーズスクロール
24 ベルの制御
25 可聴
26 可視
27 ベルタイプ
28 使用する
29 使用しない
30 マージンベル
31 マージンベルの位置
32 適用
33 リセット
34 端末エミュレータ - グローバルオプション
$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 デフォルト
2 ウィンドウサイズ(W)
3 W
4 -iso8859-1
5 %d ポイント
6 %.1f ポイント
7 %.1f ポイント
8 %.2f ポイント
9 デフォルト
10 フォントサイズ(F)
11 F

$   Window menu
12 新規(N)
13 N
14 印刷(P)
15 P
16 印刷(r)...
17 r
18 閉じる(C)
19 C
20 ウィンドウ(W)
21 W

$   Edit menu
22 コピー(C)
23 C
$ IBM fixed msg 24 for 41J
$ 24 Ctrl osfInsert
24 Ctrl<Key>osfInsert
25 Ctrl+Insert
26 ペースト(P)
27 P
$ IBM fixed msg 28 for 41J
$ 28 Shift osfInsert
28 Shift<Key>osfInsert
29 Shift+Insert
30 編集(E)
31 E

$   Option menu

32 メニューバー(M)
33 M
34 スクロールバー(S)
35 S
36 グローバル(G)...
37 G
38 端末エミュレータ(T)...
39 T
40 ソフトリセット(S)
41 S
42 ハードリセット(H)
43 H
44 リセット(R)
45 R
46 オプション(O)
47 O

$ Old Help menu (no longer used for CDE)

48 概要(O)
49 O
50 使い方(T)
51 T
52 リファレンス(R)
53 R
54 アイテムヘルプ(O)
55 O
56 ヘルプの使い方(U)
57 U
59 A
60 ヘルプ(H)
61 H

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 概要(v)
65 v
66 索引(I)
67 I
68 目次(C)
69 C
70 使い方(T)
71 T
72 リファレンス(R)
73 R
74 キーボード(K)
75 K
76 ヘルプの使い方(U)
77 U
79 A
80 ヘルプ(H)
81 H
82 端末エミュレータについて(A)
83 端末エミュレータについて(A)

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
2 了解
3 取消し
4 ヘルプ
5 キーボード制御
6 通常
7 アプリケーション
8 カーソルキー・モード
9 数値
10 アプリケーション
11 キーパッドのモード
12 リターンのみ
13 リターン／改行
14 改行シーケンス
15 ロックする
16 ロック解除する
17 ユーザ・ファンクションキー
18 画面制御
19 使用する
20 使用しない
21 132 カラム切替え
22 行末での折返し
23 前の行末へのまわり込み
24 適用
25 リセット
26 端末エミュレータ - 端末エミュレータ・オプション

$set 10
$ ****** Module: sunDtTermServer.c
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Terminal - Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
