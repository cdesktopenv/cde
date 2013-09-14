$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *

$ "$XConsortium: dtlogin.msg /main/9 1996/11/15 13:52:10 barstow $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlogin.msg
$ **
$ **  Project:     CDE dtlogin
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlogin
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *
$ * 
$ **  ----------------------- MODIFICATION RECORD END ------------------------
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

$set 1
$ set 1 is labels
$ 
$ messages 1 through 6 are button labels on the login screen

1 ユーザ名を入力してください　
2 パスワードを入力してください
3 了解
4 やり直し
5 オプション
6 ヘルプ

$ 
$ messages 7 through 11 are options button menu items
$ 

7 ログイン画面のリセット
8 コマンド行ログイン
9 バージョン...
$ message 10 is replaced by 27
10 復旧セッション
11 言語

$ 
$ messages 12 through 14 are miscellaneous button labels
$ 
12 DT の開始
13 取消し
14 ヘルプ


$ 
$ message 15, 17 are more option button menu items 
$ message 16 is the welcome message
$ 
15 DT Lite セッション
16 ようこそ！
17 通常のデスクトップ

$ 
$ message 18-24
$ 
18 * Suspending Desktop Login...\r\n*\r\n
19 * Press [Enter] for a login prompt.\r\n*\r\n
20 * Log in. Desktop Login will resume shortly after you log out.\r\n*\r\n
21 * Resuming Desktop Login...\r\n*\r\n
22 * Starting Desktop Login on display %1$s...\r\n*\r\n
23 * Wait for the Desktop Login screen before logging in.\r\n*\r\n
24 * The X-server can not be started on display %1$s...\r\n*\r\n

$ messages 25 is a options button menu item
25 セッション

$ 
$ 26 replaces 16 as the greeting after a user name has been 
$ entered. The %s is replaced by the user name.
$ 
26 ようこそ %s
$ 
$ 27 replaces 10 as the label for the failsafe option menu item
$ 
27 復旧セッション

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room  
$  for some shared sample source message additions. 
$ 
73 ユーザが最後に使ったデスクトップ
$ 
$  Following used in Language menu.  
$  "to" used in phrases like "ja to zh" locales
74 to

$set 2
$ set 2 is error messages
$ 

1 ログインが正しくありません。再度行って下さい。
2 ホーム・ディレクトリへ移動できません。
3 最大数のユーザがすでにログインしています。
$ message 4 is a lengthy message that should be localized to correspond to 
$ the language.  
4 \n\
アカウントに対する初期パスワードを選択する必要があります。そうしないと、現在のパスワードが期限切れになります。\n\
\n\
パスワードを設定後は、新パスワードを使用してログインしなければなりません。\n
5 ログイン・エラー、ユーザ ID が無効です。
6 ログイン・エラー、グループ ID が無効です。
7 ログイン・エラー、監査 ID が無効です。
8 ログイン・エラー、監査フラグが無効です。
9 現在はログインできません。
10 root だけが %1$s の実行を希望しています。\n
11 デフォルト言語リストを %1$s で再構築中です。\n
12 構成ファイルと サーバ・ファイルの両方を再スキャン中です。\n
13 構成ファイル %1$s を再読み取り中です。\n
14 サーバ・ファイル %1$s を再読み取り中です。\n
15 アクセス・ファイル %1$s を再読み取り中です。\n
16 dtlogin: ルーチンのメモリがなくなりました。\n


$set 3
$ set 3 is help text
$ message 1 is generated when the help button on the login screen is clicked.
1 \n\
ログイン・ヘルプ:\n\
\n\
ユーザ自身をコンピュータに識別させるには、ログイン画面を使用します。\n\
\n\
ログイン方法:\n\
       1) ユーザ名を入力し、<Enter> キーを押すか、[了解]をクリックします。\n\
	  (ユーザを登録していない場合、ユーザ名として "root" を入力します。)\n\
       2) パスワードを入力し、<Enter> キーを押すか、[了解]をクリックします。\n\
\n\
[やり直し]\n\
   入力を消去しますので、ユーザ名を再入力してもう一度ログインできます。\n\
  \n\
[オプション]\n\
   セッション・オプションを変更できます:\n\
\n\
   [オプション]ボタンの上にマウス・ポインタを置いて、マウス・ボタン 1 を\n\
   押したままにします。ポインタを選択肢にドラッグします。\n\
   選択する項目の上にポインタを置いて、マウス・ボタンを離します。\n\
   これで、次のものを選択できます:\n\
\n\
   言語 - セッション言語を変更します。\n\
   セッション - グラフィック環境を選択します。\n\
     通常のデスクトップ - 共通デスクトップ環境\n\
     復旧セッション - Ｘウィンドウ環境\n\
   コマンド行ログイン - コマンド行環境に切り替えます。\n\
   ログイン画面のリセット - Ｘサーバを再起動します。\n\
\n\
バックグラウンドの任意の場所で、任意のマウス・ボタンをクリックすると、\n\
画面を再表示します。たとえば、システム・メッセージによって画面が読め\n\
なくなった場合に使用します。\n\
\n\
ヘルプ情報の詳細は、まずログインして、次にフロントパネルの\n\
ヘルプ・マネージャ・アイコン(疑問符が付いた本)をクリックしてください。\n\
次に、ログイン・マネージャのヘルプ・ボリュームを見つけてください。\n\


$ Comments for messsage #1:
$ 
$ Under [Options] there is an item called "Session".  Under
$ Session there are five items and these items have been
$ translated in other files.  To be consistent with the other
$ translations, the translations used below should be the same
$ as the other translations.  The items and their file are
$ listed below:
$ 
$   1. Current   - _common.session.tmsg ; set 1, message 2
$ 
$   2. Home      - _common.session.tmsg ; set 1, message 1
$ 
$   3. <display-name> - Current   - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Current" should
$         be the same as _common.session.tmsg ; set 1, message 2
$ 
$   4. <display-name> - Home      - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Home" should
$         be the same as _common.session.tmsg ; set 1, message 1
$ 
$ 1 NEW-
$ 
$ 1 \n\
$ Login Help:\n\
$ \n\
$ Use the login screen to identify yourself to the computer.\n\
$ \n\
$ To log in:\n\
$        1) Type your user name and press <Enter> or click [OK].\n\
$           (if you have not created a user, enter "root" as the\n\
$           user name)\n\
$        2) Type your password and press <Enter> or click [OK].\n\
$ \n\
$ [Start Over]\n\
$    Erases your entries so you can re-enter your user name and\n\
$    try again.\n\
$ \n\
$ [Options]\n\
$    Allows you to change session options:\n\
$ \n\
$    With the mouse pointer over the [Options] button, press and\n\
$    hold mouse button 1.  Drag the pointer through the choices.\n\
$    Releasing the mouse button with the pointer on an item will\n\
$    select that item. You may choose:\n\
$ \n\
$    Language - to change the session language\n\
$    Session - to select preferred user Desktop\n\
$       Current - Starts your most recent session\n\
$       Home - Starts your home session (if you set one)\n\
$       <display-name> - Current - starts your most recent session for\n\
$           the given display\n\
$       <display-name> - Home - starts your home session for\n\
$           the given display\n\
$       FailSafe Session - Starts a failsafe session.\n\
$    Command Line Login - to switch to a command line environment\n\
$    Reset Login Screen - to restart the X-server\n\
$ \n\
$ Click anywhere on the background with any mouse button to\n\
$ refresh the screen, for example, if system messages make the\n\
$ screen unreadable.\n\
$ \n\
$ For more help information, log in first, then click on the Help\n\
$ Manager icon that is in the Information Manager slide-up menu\n\
$ in the Front Panel (Book with an information symbol).\n\
$ Then find the Login Manager help volume.\n


$ message 2 is generated when you attempt to start a system whose id is
$ set to unknown or if it is not configured for networking.
2 \n\
ワークステーション名が一意ではありません。現在の設定は "unknown"\n\
です。このワークステーションに名前を付けることをお勧めします。\n\
\n\

$ message 3 is generated when the help button on the chooser screen is clicked.
3 \n\
チューザ・ヘルプ:\n\
\n\
ログインするサーバを選択するには、チューザ画面を使用します。\n\
\n\
チューザ画面には、各エントリに対して 2 つの部分で構成されるリストが\n\
あります。第一の部分(サーバ名)は、使用可能な各サーバのネットワーク名\n\
です。第二の部分(サーバ情報)には、各サーバ上で実行するログイン・\n\
マネージャによって異なる情報があります。サーバが CDE デスクトップを\n\
実行中の場合は、現在使用中のユーザ数(または tty デバイス)およびその時点\n\
から 5 分前、10 分前、15 分前の平均負荷(CPU の平均使用状況)を\n\
表示します。\n\
\n\
ログインするサーバを選択するには、サーバ名を含むリストの行を選択する\n\
だけです。\n\
\n\
選択したら、[了解] ボタンを押します。\n\
これで、選択したサーバのログイン画面が表示されます。\n\
\n\
[リストの更新]\n\
   リストを消去し、ネットワーク上のサーバがログインを許可しているかどうか\n\
   を照会してリストを再構築します。\n\
\n\
[オプション]\n\
   セッションのオプションを変更できます:\n\
\n\
   [オプション]ボタンの上にマウス・ポインタを置いてマウス・ボタン 1 を \n\
   押したままにします。ポインタを選択肢にドラッグします。\n\
   選択する項目の上にポインタを置いて、マウス・ボタンを離します。\n\
   これで、次のものを選択できます:\n\
\n\
   言語 - セッション言語を変更します。\n\
   ログイン画面のリセット - Ｘサーバを再起動します。\n\
\n\
バックグラウンドの任意の場所で、任意のマウス・ボタンをクリックして、\n\
画面を再表示します。たとえば、システム・メッセージによって画面が読め\n\
なくなった場合に使用します。\n\
\n\


$set 4
$ set 4 is log error messages
$ 

1 アクセス・ファイル "%1$s"、ホスト "%2$s" が見つかりません。\n
2 ReadHostEntry\n
3 アクセス・ファイル %1$s、未知のディスプレイ %2$s です。\n
4 アクセス・コントロール・ファイル %1$s を開けません。XDMCP 要求が認可されていません。\n
5 ドメイン/OS 認証エラー: %1$s (%2$s/%3$s)
6 サーバ認証ファイル %1$s を開けません。\n
7 サーバ認証ファイル %1$s に書き込めません。\n
8 saveAddr
9 ネットワーク・インタフェース構成の取得障害です。
10 setAuthNumber
11 認証ファイル %1$s またはバックアップ %2$s をロックできません。\n
12 認証を位置に移動できません。\n
13 プロセス ID ファイル %1$s を作成／ロックできません。\n
14 プロセス ID ファイル %1$s をロックできません。他の Dtlogin を実行中です。(プロセス ID %2$d)\n
15 サーバ・ファイル %1$s にアクセスできません。\n
16 NewDisplay
17 エラー・ログ・ファイル %1$s を開けません。\n
18 TrimErrorLog(): エラー・ログ・ファイルの最大サイズは 200 K バイトです。\n
19 サーバ・ファイルにディスプレイ名がありません。\n
20 %1$s のディスプレイ・タイプがありません。\n
21 ホスト名 %1$s が無効です。DISPLAY 環境変数を ':0' に設定します。\n
22 ディスプレイ %2$s のディスプレイ・タイプ %1$s を許容できません。\n
23 疑似リセットがタイムアウトしました。\n
24 GetResource
25 引き数 realloc のスペースがありません。\n
26 構成ファイル %1$s を開けません。\n
27 余分な引き数がコマンド行にあります:
28 StartServer(): 引き数がありません。\n
29 サーバ %1$s を実行できません。\n
30 StartServer() フォークが失敗しました。休眠します。\n
31 サーバが予期できない原因で落ちました。\n
32 XOpenDisplay(%1$s) の試み #%2$d がハングしました。中止します。\n
33 サーバを開こうという試み #%1$d が %2$s のため失敗しました。中止します。\n
34 致命的 IO エラー %d (%1$s)\n
35 X エラー\n
36 セッションの開始に失敗しました。\n
37 警告: ディスプレイ %1$s は保護できません。\n
38 警告: ディスプレイ %1$s のキーボードは保護できません。\n
39 BLS - アカウント情報を獲得できません。\n
40 BLS - 保護されたアカウント情報を獲得できません。\n
41 BLS - 選択レベルを変更できません。\n
42 BLS - ユーザに変更できません: %1$s.\n
43 AFS - セッションを開始する際の setgroups() に失敗しました。\n
44 ユーザ %s のユーザ ID 設定に失敗しました。エラー番号 = %d\n
45 ユーザ %2$s のホーム・ディレクトリ %1$s がありません。/\n
46 セッションの実行に失敗しました。%1$s\n
47 セッションには、コマンド／引き数はありません、\n
48 %s のセッションを開始できません。フォークに失敗しました。\n
49 Dtlogin はクライアントを強制終了できません。\n
50 %1$s を実行できません。\n
51 %1$s を実行するためにフォークできません。\n
52 認証名 "%1$s" が不適切です。AFS、Kerberos、またはデフォルトを使用してください。\n
53 %2$s に対する Dtgreet %1$s を実行できません。\n
54 Dtgreet %1$s を実行するためにフォークできません。\n
55 ポート %1$d のソケット作成が失敗しました。\n
56 エラー割り当てソケット・アドレス %1$d、エラー番号 = %2$d\n
57 makeEnv
58 setEnv
59 parseArgs
60 %1$s ディレクトリのスキャンが %2$d 秒を越えました。スキャンを終了します...\n
61 %s が複数の %1$d グループに属しています。%2$s を無視します。\n
62 audwrite() エラー: 呼び出し側がスーパーユーザではありません。\n
63 audwrite() エラー: 監査レコードに無効なイベント番号があります。\n
64 audwrite() エラー: エラー番号 = %1$d\n
65 AFS 認証に失敗しました: %1$s\n
66 %1$s のアカウントが期限切れです。システム管理者に問い合わせてください。\n
67 サーバが落ちています。%1$s\n
68 ロゴ・ビットマップ・ファイルを開けません: %1$s\n
69 "%1$s" メッセージ・カタログを読めません。デフォルト言語を使用します。
70 ディスプレイ %1$s を初期化できません。サーバがグラブされている可能性があります。\n
71 (%1$s): ログイン・マットの幅がディスプレイの幅を越えています。\n\
デフォルトを使用します。マットの単位はピクセルです。\n
72 (%1$s) ログイン・マットの高さがディスプレイの高さを越えています。\n\
デフォルトを使用します。マットの単位はピクセルです。\n

$set 5
$ set 5 is for other stuff specific to chooser.
1 ログインするサーバを選択する:
$ 1 is the title/instructions for the chooser dialog
2 サーバ名                       サーバ情報
$                                ^
$                                | This needs to start in this column
$ 2 is the headings for the list
3  リストの更新

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/9 1996/11/15 13:52:10 barstow $"

