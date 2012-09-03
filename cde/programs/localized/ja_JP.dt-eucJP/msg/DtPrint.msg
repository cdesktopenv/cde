$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtPrint.msg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  library.
$ **
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994, 1996 Novell, Inc.
$ **  (c) Copyright 1996 Digital Equipment Corporation.
$ **  (c) Copyright 1996 FUJITSU LIMITED.
$ **  (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: DtPrint.msg /main/9 1996/10/16 03:12:47 cde-hit $
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

$quote "

$set 1
$ *************************************************************
$ ** 							     ** 
$ ** PrintSetupBox messages and labels 			     **
$ ** 							     ** 
$ *************************************************************
$ Warning messages
$ Note: do not localize the chars %s
$ 
$quote
1 workAreaLocationリソースの設定が無効です。

$ Label values
$ 
2 プリンタの追加...
3 ファイルに出力
4 ファイルの選択...
5 取消し
6 印刷部数:
7 ヘルプ
8 印刷
9 プリンタ名:
10 設定...
11 プリンタの説明:

$ Warning messages
$ Note: do not localize the chars %s
12 印刷部数は範囲外です。
13 印刷の設定モードが無効です。
14 印刷の設定モードは変更できません。

$ Label values
$ 
15 情報...

$ Warning messages
$ Note: do not localize the chars %s
16 プリンタは不正です。
17 プリンタ %s はプリント・サーバ %s 上にありません。
18 プリント・サーバ %s に接続できません。
19 Ｘサーバ %s は印刷をサポートしていません。
20 デフォルト・プリンタが決定されていません。
21 %s のＸプリント・サーバが見つかりません。
22 %s :指定からプリンタ名が抜けています。
23 印刷の出力先が無効です。
24 コンパウンド・テキストから変換できません。
25 リソース切り替えのための引き数は必要ありません。

$ Label values
$ 
26 プリンタ選択エラー

$ Warning messages
$ Note: do not localize the chars %s
27 プリンタを見つけられません。

$ Label values
$ 
28 説明を取得できません
29 選択ファイル
30 プリンタ情報
31 書式:
32 プリンタ・モデル:
33 プリンタ:
34 他のプリンタ
35 プリンタに印刷
36 ファイル名:
37 印刷の設定 - ヘルプ


$set 2
$ *************************************************************
$ ** 							     ** 
$ ** _DtPrint Frame GUI messages			     **
$ ** 							     ** 
$ *************************************************************
$ 
$quote "

$ Label values for Margin Frame
$ 
1 "マージン"
2 "上:"
3 "右:"
4 "下:"
5 "左:"
$ 
$ Label values for Header/Footer Frame
$ 
6 "ヘッダとフッタ"
7 "左上:"
8 "右上:"
9 "左下:"
10 "右下:"

$set 3
$ *************************************************************
$ ** 							     ** 
$ ** Dt Print Dialog Manager Job Options GUI messages        **
$ ** 							     ** 
$ *************************************************************
$ 
$quote

$ Label values for Margin Frame
1 完了時にメールを送信
2 バナー・ページのタイトル:
3 印刷コマンドのオプション:
