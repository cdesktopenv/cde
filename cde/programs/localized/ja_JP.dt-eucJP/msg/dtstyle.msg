$ "$XConsortium: dtstyle.msg /main/11 1996/12/01 23:39:42 cde-hit $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtstyle.msg
$ **
$ **  Project:     DT Style Manager
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtstyle
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1996 Digital Equipment Corporation.
$ ** (c) Copyright 1990, 1996 Hewlett-Packard Company.
$ ** (c) Copyright 1996 International Business Machines Corp.
$ ** (c) Copyright 1996 Sun Microsystems, Inc.
$ ** (c) Copyright 1996 Novell, Inc. 
$ ** (c) Copyright 1996 FUJITSU LIMITED.
$ ** (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.  
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$ Message 1 from set 2 was deleted from the message catalog

$ _DtMessage 2 is the title of all Information dialog boxes
2 通知
$ _DtMessage 3 is the title of all Error dialog boxes
3 エラー
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 セッション・マネージャと通信できません...\n\
終了します。
 
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 スタイル・マネージャをすでに実行しています。\n2 回目の\
起動を中止しました。
6 X ツールキット・ エラーが発生しました...\n\
終了します。

$ _DtMessage 7 is the title of all Help dialog boxes
7 スタイル・マネージャ - ヘルプ

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 スタイル・マネージャ内の項目を 1 つ選択してください。

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 セッション・マネージャからスクリーンセーバ\n\
情報を獲得できません。起動の設定が間違っている\n\
可能性があります。


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 スタイル・マネージャ

$ mnemonic and label for the File menubar item.
2 F
3 ファイル(F)
$ mnemonic and label for the Help menubar item.
4 H
5 ヘルプ(H)

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 終了(x)

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 カラー
28 フォント
29 背景
30 キーボード
31 マウス
32 ビープ音
33 画面
35 起動

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 現在のセッションで使用可能な Dtstyle 復元情報はありません。

$ _DtMessage 37 is the label on the dtstyle icon
37 スタイル

$ Message 38 is the label on the Window button in the dtstyle main window.
38 ウィンドウ

$ _DtMessage 40 is replaced with message 53
$ 40 E

$ *
$ * mnemonic and label for the Help menu items.
$ * 

$ Messages 41-42 have been replaced with messages 54-55
$ Overview... menu item
$ 41 I
$ 42 Introduction...

$ Tasks... menu item 
43 T
$ Message 44 has been replaced with messages 56
$ 44 Tasks...

$ Reference... menu item
$ 45 R
$ Message 46 has been replaced with messages 57
$ 46 Reference...

$ On Item menu item
$ Keybaord menu item
47 O
48 アイテムヘルプ(O)

$ Using Help... menu item
49 U
$ Message 50 has been replaced with messages 58
$ 50 Using Help... 

$ Messages 51-52 have been replaced with messages 59-60
$ Overview menu item
$ 51 V
$ 52 Version...

$ Message 53 is replaced with message 40
53 x

$ Messages 54-55 are replacements for messages 41-42
54 v
55 概要(v)

$ Message 56 is a replacement for messages 44
56 使い方(T)

$ Message 57 is a replacement for messages 46
57 リファレンス(R)

$ Message 58 is a replacement for messages 50
58 ヘルプの使い方(U)

$ Messages 59-60 are replacements for messages 51-52
59 A
60 スタイル・マネージャについて(A)

$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 プレビュー

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 フォントの変更は、いくつかのアプリケーションでは\n\
次回起動した時から反映されます。\n\
ファイル・マネージャや、アプリケーション・マネージャ\n\
など他のアプリケーションでは、デスクトップを終了して\n\
再度ログインするまで、反映されません。

$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 選択したフォントは、このセッションを\n\
再起動すると使用されます。

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 AaBbCcDdEeFfGg0123456789
21 AaBbCcDdEeFfGg0123456789

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 サイズ

$ _DtMessage 23 is the title of the Fonts dialog
23 スタイル・マネージャ - フォント

$ _DtMessage 24 is an informative message used by the Fonts dialog
24 現在のデスクトップで使われるフォントはサイズのリストで利用することができません。\n\
新しいフォントが選ばれ適用されると、現在のフォントに戻すことはできません。\n\
スタイルマネージャ - フォントのダイアログを使って下さい。

$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 デフォルト
3 音量
4 音程
5 音長

$ _DtMessage 7 is the title of the Beep dialog box
7 スタイル・マネージャ - ビープ音


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 ログイン時:
4 このセッションを再開
5 ホームセッションを開始

$ Message 6 is obsolete - DO NOT localize message #6 from this set
$ 6 Ask me at Logout

7 ログアウト確認ダイアログ:
8 オン
9 オフ
10 ホームセッションを設定 ...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 これはホームセッションを現在のセッションに置換します。\n\n\
		 継続しますか?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 警告

$ _DtMessage 13 is an error message
13 このセッションは、Ｘウィンドウ・システム起動スクリプトで\n\
起動しました。起動設定が有効ではないので、セッションは復元\n\
できません。

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 スタイル・マネージャ - 起動

$ _DtMessage 15 is an error message
15 セッション・マネージャを実行していません。\n\
起動設定が有効ではありません。

$ _DtMessage 16 is used in the Sessin Startup dialog.  It is the
$   string that appears to right of the Startup dialog icon IF
$   the user selects a display-specfic session at the login screen.
16 特定のディスプレイのセッション:


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
1 左
2 右
3 利き腕:
4 ボタン 2:
$ _DtMessage 5 is the label of default button.
5 デフォルト

$ Messages 6,8,9 are labels of the scales.
6 ダブルクリック
8 速度
9 しきい値

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 エラー: ポインタ・マッピングの設定\n

11 転送
12 アジャスト

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 マウスの右ボタンと左ボタンは、\n\
[了解] をクリックした直後に機能を交換します。\n\
マウス・ウィンドウの [ダブルクリックのテスト]\n\
グラフィックは、ボタン 1 およびボタン 3 の\n\
位置で現在の利き腕を示しています。

$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 [了解] をクリックすると、次のセッションでは\n\
マウス・ボタン 2 が [転送] (ドラッグ)に設定されます。

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 新しいダブルクリック時間は、アプリケーションを\n\
再起動した時点で有効になります。

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 新しいダブルクリック時間は、次のセッションで\n\
有効になります。

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 テスト\nダブルクリック

$ _DtMessage 18 is the Mouse Dialog title
18 スタイル・マネージャ - マウス

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 [了解] をクリックすると、次のセッションでは\n\
マウス・ボタン 2 が [アジャスト] に設定されます。\n\
ボタン 2 はテキストのアジャストおよび選択肢の\n\
表示に使用します。\n\
テキストおよびリストの転送アクション(ドラッグ)\n\
には、ボタン 1 を使用する必要があります。

$ _DtMessage 20 is the label used for the "Button 2" toggle on
$ a two button mouse (ignored on a three button mouse)
20 メニュー

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 ファイルのアクセス権を確認してください。
3 %s は、セッションを保存するのに使用しようとしたファイルです。


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 ディレクトリ %s を開けません。
$ _DtMessage 2 - error opening the backdrop files
2 使用可能な背景ファイルがないので、\n\
スタイル・マネージャの背景は動作しません。\n\
$HOME/.dt/errorlog を確認してください。


$ _DtMessage 12 is the title of the Backdrop dialog box
12 スタイル・マネージャ - 背景


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box. 
1 スタイル・マネージャ - 画面

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 デフォルト
4 スクリーンセーバ
5 スクリーンセーバの開始
6 セーバ画面の切り替え時間
7 分
8 画面ロック
$ Message 9 has been removed 
10 オン
11 オフ
12 スクリーンセーバ:
13 画面ロック:
14 ロックの開始
15 自動画面ブランキング
16 画面ブランカ:
17 ブランキングの開始
18 フロントパネルのロック
$ Message 19 has been replaced by message 21
19 バックグラウンドのロックを使用
20 ロック画面の切り替え時間
$ Message 21 is a replacement for message 19
21 バックグラウンドをロックとして使用
22 透明ロック

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 デフォルト
3 オートリピート
4 キーを押したときの音量

$ _DtMessage 6 is the title of the Keyboard dialog box
6 スタイル・マネージャ - キーボード


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 パレット
6 追加...
7 削除...
8 変更...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 新規パレット名:

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 パレットの追加

$ Messages 12 through 14 are error messages.
12 パレット名には次の文字は使用できません:\n\n\
<space> * : ( ) [ ] { } < > ! | " / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE 
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 パレット名は 10 文字以下でなければなりません。\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 パレットの削除

$ _DtMessage 18 appears in an information dialog.
18 最後のパレットは削除できません。\n

$ _DtMessage 21 is an error message
21 警告

$ _DtMessage 23 appears in an information dialog
23 パレット '%s' を削除しますか?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 カラーサーバが動作していませんので、\n\
スタイル・マネージャのカラーは動作しません。\n\
$HOME/.dt/errorlog を確認してください。

$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 リソース 'useColorObj' が False に設定されているので、\n\
スタイル・マネージャのカラーは動作しません。

$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 新しい[使う色数]の値は、次のセッションで\n\
有効になります。

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 選択したパレットは、次のセッションで\n\
有効になります。

$ _DtMessage 29 appears in a warning dialog
29 '%s' という名前のパレットはすでに存在します。\n\
この新しいパレットは、古いパレットを上書きします。\n\
上書きしてもいいですか?

$ _DtMessage 30 is the title of the Color dialog box.
30 スタイル・マネージャ - カラー

$ _DtMessage 31-33 are the ColorUse options
$ 31 High Color
$ 32 Medium Color
$ 33 Low Color
34 白黒
35 デフォルト

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT Color Use

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog 
$ 37 DT Color Use...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 使用可能なパレット・ファイルはありませんので、\n\
スタイル・マネージャのカラーは動作しません。\n\
$HOME/.dt/errorlog を確認してください。

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39 使用する色の数:

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 色の数...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 デスクトップ用にもっと多くの色数
42 アプリケーション用にもっと多くの色数
43 アプリケーション用に最大限度の色数

$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 '%s' を削除できません。\n
12 原因を調べるには、$HOME/.dt/errorlog を確認してください。

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 警告: リソース paletteDirectories に表示されている\n\
Maximum number is %d. 
$ _DtMessage 19 - could not open a palette directory
19 ディレクトリ %s を開けません。
$ _DtMessage 20 - error opening the palette file
20 %s を開けません。
$ _DtMessage 21 - error reading a palette file
21 %s は無効なパレット・ファイルです。


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 カラー・サンプル
5 旧
6 新規
7 色をグラブ
8 カラーエディタ
9 色合い
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 R
11 G
12 B

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 警告: ポインタをグラブできません。\n
15 警告: キーボードをグラブできません。\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 スタイル・マネージャ - 配色の変更


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 スタイル・マネージャ - ウィンドウ

$ Messages 2 through 5 are labels on the Window dialog.
2 デフォルト
3 ウィンドウの動作
4 ウィンドウ・アイコン
$ Messages 5-8 have been replaced by messages 13-16 
$ 5 Focus Follows Mouse
$ 6 Click In Window For Focus
$ 7 Raise Window With Focus
$ 8 Opaque Move
9 アイコン・ボックスの使用
10 ワークスペースに配置する
11 主ウィンドウをトップにする
12 変更を反映するには、ワークスペース・マネージャを\n\
再起動しなければなりません。\n\
[了解] を押してワークスペース・マネージャを再起動するか、\n\
[取消し] を押して変更内容をリセットしてください。
$ Messages 13-16 are replacements for messages 5-8
13 ポインタでウィンドウをアクティブに
14 クリックでウィンドウをアクティブに
15 アクティブにしたウィンドウを手前に
16 移動中の内容表示

$set 19
$ The set 19 contains messages for the I18N dialog

$ 
$ Messsages 1 to 11 are labels displayed in the dialog.
$ 

$ Message 1 is the title of the Dialog.
1 スタイル・マネージャ - 国際化

$ Message 2 appears on the button used to reset the last saved values.
2 デフォルト

$ Message 3 appears as the title of the title box containing Input
$ Methods information.
3 入力メソッド

$ Message 4 appears as the title of the title box containing Input
$ Method Start Mode choice.
4 入力メソッドの起動モード

$ Message 5 appears as the title of the title box containing PreeditType
$ information.
5 前編集タイプ

$ Message 6 is the label presenting the server host names
6 サーバのホスト

$ Message 7 is the label presenting the Input Method list
7 入力メソッドのリスト

$ Message 8 and 9 are the toggle buttons for the start mode selection:
$ "Ask at login" will cause dtimsstart to present the list of Input
$ Methods at next login; "Resume Current Input Method" will cause
$ dtimsstart to use the selected IM at next login, without asking the
$ user.
8 ログイン時に決定
9 現在の入力メソッドを再開

$ Message 10 is the label displayed on the button used to move up a
$ selected element in the preedit Type list (meaning at the previous
$ position).
10 上へ移動

$ Message 11 is the label displayed on the button used to move down a
$ selected element in the preedit Type list (meaning at the next
$ position).
11 下へ移動

$ 
$ Messages 20-21 are displayed in an warning dialog when the user
$ hits the OK button.
$ 

$ Message 20 appears when the user modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is True.
$ Do NOT localize the word XmNpreeditType.
20 新しいXmNpreeditTypeの値はアプリケーションを\n\
再起動した時点で有効となります。

$ Message 21 appears when the userselects a new Input Method Server,
$ or modifies the IMS Start Mode, or modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is False.
21 選択は次のセッションで有効となります。

$ 
$ Messages 30 to 41 appear as errors in Error Dialog.
$ 

$ Do NOT localize the word HOME.
30 環境変数HOMEは定義されていません。

$ Do NOT localize the word LANG.
31 環境変数LANGは定義されていません。

32 デスクトップはこのロケールをサポートしていません。
33 ユーザの入力メソッド選択ファイルを作成できません。
34 ユーザの入力メソッド選択ファイルが見つかりません。
35 ユーザの入力メソッド選択ファイルを保存できません。
36 未知のホストです。

$ Do NOT translate dtimsstart.
37 dtimsstartが見つかりませんでした。\n\
おそらくデスクトップ環境は\n\
適切にインストールされていません。
38 選択されたホストにおけるリモート実行が失敗しました。
39 リモート実行を初期化できません。
40 選択されたホストから入力メソッド情報を獲得できません。
41 未知のエラー


$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/11 1996/12/01 23:39:42 cde-hit $"

