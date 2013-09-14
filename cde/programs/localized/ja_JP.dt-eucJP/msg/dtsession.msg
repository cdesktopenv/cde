$ "$TOG: dtsession.msg /main/18 1999/09/20 13:50:16 mgreess $";
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtsession.msg
$ **
$ **  Project:     HP DT  Session Manager (dtsession)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtsession
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * Do not localize sets 2, 4, 6, 8, 10, 12, 14, 16.
$ * Do not localize sets 20, 22, 24, 26, 28, 99.  
$ * They only appear in the errorlog.
$ *
$ * DO localize sets 18 and 29. 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMain.c --
$ These are errorlog messages
$ DO NOT TRANSLATE THIS SET.

2 別の dtsession が現在実行中です。- 終了します。
3 このサーバで利用できないスリーンセーバをつけた時、スクリーンをロックします。スクリーンセーバがつけられた時、スクリーンはロックされないでしょう。

$set 4
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmGlobals.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 無効なディスプレイ名です。- 終了します。
2 デフォルトファイルがありません。アプリケーションを再起動できません。
3 デフォルトファイルがありません。アプリケーションを再起動できません。
4 セッション。ディレクトリを削除できません。あらかじめ $HOME/.dt ディレクトリに\
書き込み権限をつけて下さい。無効なセッションファイルは削除されないでしょう。
5 メモリの malloc 操作ができません。
6 ディスプレイをロックできません。他のアプリケーションがポインタまたは\
キーボードをグラブしているかも知れません。
7 セッションファイルを開けません。起動されるクライアントはありません。
8 クライアントプロセスをフォークできません。
9 ディレクトリを作成できません。ホーム・ディレクトリのアクセス権限を確認してください。
10 セキュリティ制限のためディスプレイをロックできません。
11 一時ファイルを作成できません。:%s \
あらかじめこのファイルに書き込み権限をつけて下さい。\
セッションマネージャを終了します。
$ This message is added to the error log if a required temporary
$ file can not be created by the session manager.
$ The insert is the name of the temporary file.

$set 6
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMsgSrv.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 "Could not connect to ToolTalk message server:\n%sExiting ...\n"
2 DT メッセージ・サーバは終了しました。


$set 8
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmError.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 サーバへの接続をなくしました。- 終了します。


$set 10
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProperty.c --


$set 12
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProtocol.c --


$set 14
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmSave.c --


$set 16
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmRestore.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 プロセス /usr/bin/X11/xrdb を exec できません。セッション・リソースは\
復元されません。
2 クライアント設定ファイルが無効です。設定は復元されません。
3 %s を exec できません。
4 プロセス CDE_INSTALLATION_TOP/bin/dtwm を exec できません。ウィンドウ・マネージャは開始されないでしょう。
5 許されるリモート・クライアント数の最大値となりました。これ以上のリモート・クライアントは復元されないでしょう。
6 無効なディスプレイ/言語非依存なリソース・ファイル。ディスプレイ/言語非依存なリソースは復元されません。
7 wmStartupCommand リソース設定 :\n\n\
      %s\n\n\
このファイルは存在しないか、実行形式ではありません。\n\
その代わりに CDE_INSTALLATION_TOP/bin/dtwm は開始されないでしょう。\n
8 ホスト "%s" でコマンド(DT リモート実行プロセスを使用)\n\
の復元に失敗しました。\n\n\
      %s\n\n\
以下の実行文字列で試してください。:\n\n\
   %s\n\n
9 ウィンドウ・マネージャが起動できません。:\n\n\
      %s\n\n\
This message indicates you tried to start a\n\
window manager that is built into an X terminal.\n\
This will only work with X terminals that support this protocol.\n\
CDE_INSTALLATION_TOP/bin/dtwm will be started instead.\n


$set 18
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmUI.c -- 
$ LOCALIZE Set 18
$ Deleted Messages:  13, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
$						29, 30, 31, 32, 33

$ Message 1 tells the user who currently has the display locked.
1 ユーザ %s がディスプレイをロックしました。

$ Message 2 tells the user to enter the password for the above user to unlock
$ the display.
2 ロック解除するには、パスワードを入力してください。

$ Message 3 is a prompt for the user to enter a password.
3 パスワード:

$ Message 4 is the logout message that appears when dtsession is going
$ to save the Current session information. 
$ Access this message by pressing the logout button.
$ 4 Exiting the desktop session...\n\n\
$ Application updates you have not saved may be lost.\n\n\
$ Your Current session will be saved and returned to \n\
$ upon your next login.  For more detail, select Help.\n\n\
$ Items in the Trash Can will be permanently shredded.\n\n\
$ Continue Logout?
$ 4 NEW-
4 デスクトップ・セッションを終了中です...\n\n\
ログインすると、現在のセッションを復元します。\n\
保存していないアプリケーションの変更内容は失われる可能性があります。\n\n\
ログアウトしますか?

$ Message 5,6 is the logout message that appears when dtsession is not going
$ to save the Current session information - but return to the Home session info
$ 5 Exiting the desktop session...\n\n\
$ Application updates you have not saved may be lost.\n\n\
$ Your Home session will be restored upon login.  Your current\n\
$ session will not be saved.  For more detail, select Help.\n\n\
$ Items in the Trash Can will be permanently shredded.\n\n\
$ Continue Logout?
$ 5 NEW-
5 デスクトップ・セッションを終了中です...\n\n\
ログインすると、ユーザのホームセッションを復元します。\n\
保存していないアプリケーションの変更内容は失われる可能性があります。\n\n\
ログアウトしますか?

$ Message 6 is the title of the logout dialog box.
6 ログアウト確認

$ Messages 7,8,9,10 have been obsoleted.
$ Do NOT translate messages 7, 8, 9 and 10
$ the user has requested to have the choice
$ to either save the Current session or return to the Home session.
$ 7 OBS- Exiting the desktop session...\n\n\
$ Application updates you have not saved may be lost.\n\n\
$ You can either save your Current session and return to it upon login,\n\
$ or return to your Home session upon login.  For more detail, select Help.\n\n\
$ Items in the Trash Can will be permanently shredded.\n\n\
$ Continue Logout?
$ 8 OBS- OK\nSave Current
$ 9 OBS- OK\nReturn Home
$ Message 10 is the title of the logout dialog box.
$ 10 OBS- Logout Confirmation

$ Message 11,12 are part of the error dialog that appears when the DT
$ messaging system dies.
$ These messages are generated when you kill the Message
$ Server, which is /usr/dt/bin/ttsession.
$ Message 11 is obsolete.  Message 38 replaces message 11

$ 11 DT Messaging System Inoperative\n\n\
$ To restart:\n\n\
$ 1) Save all open data files.\n\
$ 2) Press the reset key sequence to return to the login screen.\n\
$ 3) Log in again.\n\n\
$ Note: The Current session will not be saved.\n\n\
$ When you are ready to begin the restart process, click [OK] and\n\
$ proceed to save your files.

12 Message 障害

$ Messages 15, 16, 17 are used for identical purpose as messages 1, 2,3
$ in this set.
15 ユーザ %s がディスプレイをロックしました。
16 ロック解除するには、パスワードを入力してください。
17 パスワード:

$ Message 34, 35 are messages that appear when the user started up in 
$ compatibility mode, and tries to log out.  You can only exit out of 
$ compatibility mode by using the reset key sequence.
$ To generate this message, start in x11 (not DT) and then run dtsession.
$ Refer to the dtsession manpage.  Then try to log out.
34 このセッションは、Ｘウィンドウ・システムの起動スクリプトから起動しました。\n\n\
セッション情報は保存しません。\n\n\
ログアウトするには、リセット・キー・シーケンスを使用してください。
35 ログアウト Message

$ Messages 36,37 are part of the error dialog that appears when dtsession is
$ unable to start the DT messaging system, and therefore not able to bring
$ up the system at all.
$ To generate this, move the message server from
$ /usr/softbench/bin/softmsgsrv to some other directory, then start Dt.
$ 36 The DT messaging system could not be started.\n\n\
$ To correct the problem:\n\n\
$ 1.  Choose [OK] to return to the login screen.\n\n\
$ 2.  Select Failsafe Session from the login screen's option\n\
$      menu and log in.\n\n\
$ 3.  Check to see that the hostname is correct in these locations:\n\
$      /etc/src.sh\n\
$      /etc/hosts\n\
$      /usr/adm/inetd.sec\n\n\
$ For additional information, see the DT User's Guide.
$ 36 NEW-
36 DT メッセージ・システムを起動できません。\n\n\
問題を解決するには:\n\n\
1.  [了解] を選択して、ログイン画面に戻ります。\n\n\
2.  ログイン画面のオプション・メニューで、[フェイルセーフ・セッション] を\n\
選択して、ログインします。\n\n\
3.  次の場所のホスト名が正しいかどうか確認します:\n\
     /etc/src.sh\n\
     /etc/hosts\n\
     /usr/adm/inetd.sec\n\n\
詳細は、DT ユーザーズ・ガイドを参照してください。
37 必要なアクション

$ Message 38 is part of the error dialog that appears when the DT
$ messaging system dies.
$ This messages are generated when you kill the Message
$ Server), which is /usr/dt/bin/ttsession.
38 メッセージ・システムが動作していません。\n\n\
再起動するには:\n\n\
1) 開いているデータ・ファイルをすべて保存します。\n\
2) ログアウトします。\n\
3) 再度ログインします。\n\n\
注意: 現在のセッションは保存しません。\n\n\
プロセスの再起動を開始する用意ができたら、[了解] をクリックして、\n\
ファイルを保存してください。

$ 
$ Messages 39-41 are displayed on the logout and/or session selection
$  confirmation dialogs.  These message are for dialog buttons
$ 
39 了解
40 取消し
41 ヘルプ

$ 
$ Messages 50-53 are for the display-specific session confirmation dialog
$ 
50 Home
51 Current
$ 
$ The first '%s' in message 52 will be replace with message 50 or 51.
$ The second '%'s in message 52 will be replace with the value of
$ $DISPLAY (e.g. hostname:0).
52 ディスプレイ指定 '%s' セッションが '%s' ディスプレイで\n\
選択されています。しかしそれは存在しません。\n\n\
続けるならば、新しいディスプレイ指定セッションが\n\
作成されます。
53 警告

$ 
$ Message 55-56 are for the Session Save error dialog
$   55 is the dialog title
$   56 is the message in the dialog - do NOT localize 'save_state'
$ 
55 セッション失敗保存
56 セッションは 'save_type' パラメータ要求が\n\
メッセージ中にないため、保存されません。\n\
セッションは保存されません。

$ 
$ Messages 70-71 refer to the name of the System Administration manual
$ 
70 これ以上の情報は以下を参照してください。:
71 CDE 上級ユーザ及びシステム管理者ガイド

$ 
$ Message 75 is used during logout if the user started a Current
$   session but used the Style Manager to return to a Home session.
$ 
75 デスクトップ・セッションを終了中です...\n\n\
現在のセッションを保存しますが、ログインすると、\n\
ユーザのホームセッションを復元します。\n\n\
保存していないアプリケーションの変更内容は失われる可能性があります。\n\n\
ログアウトしますか?

$ 
$ Message 76 is used at the same time message 36 is used except that
$   message 76 is for Digital/OSF only and is slightly different than
$   message 36.
$ 
76 DT メッセージ・システムが起動していません。\n\n\
問題を直すには:\n\n\
1.  ログイン・スクリーンに戻り、[了解]を選択します。\n\n\
2.  ログイン・スクリーンのオプション・メニューから\n\
         復旧セッションを選択しログインします。\n\n\
3.  ネットワークがすでに構成されているならば、\n\
     ホスト名が /etc/hosts に正確にあるか確認してください。\n\
4. ネットワークがまだ構成されていないならば、/etc/hosts \n\
     に以下のエントリをあらかじめ加えて下さい。:\n\
     127.0.0.1 localhost \n\n\
これ以上の情報は、ユーザーズ・ガイドを参照してください。

$ Message 99 is displayed when a user logs in for the first time
 
99 ホーム・ディレクトリに新しいファイル '.dtprofile' を追加しました。\n\
\n\
'.login' または '.profile' ファイルが起動され、CDE と影響し合い、\n\
再ログインした時このファイル中の命令に従います。\n\
\n\
これ以上の情報は CDE 上級ユーザ及びシステム管理者ガイドを参照してください。

$set 20
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmWindow.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 クライアント・メッセージが失敗しました。クライアント情報は保存されません。
2 ルート・ウィンドウが無効です。クライアント情報は保存できません。
3 セッション復元情報はクライアントのため更新されません。\
無効な情報を保存したかも知れません。
4 セッション復元情報はクライアント %s のため更新されません。\
無効な情報を保存したかも知れません。


$set 22
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmLock.c --


$set 24
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- version.c --


$set 26
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvPalette.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 カラー・サーバ警告:選択の所有権が得られません。
2 カラー・サーバ警告:選択の所有権を失います。
3 選択を失うべきではありません。
4 警告、より多くのピクセルを見つけ、そのうえ利用可能です。
5 警告、十分にピクセルをアロケートできません。


$set 28
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvFile_io.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.
$ Messages 1,2,6,7,8,9 are obsolete

$ 1 Color Server Warning, the size of file '
$ 2 ' is invalid\nremoving file and trying again.
$ 6 Color Server FatalError, could not make directory
$ 7 Color Server FatalError: couldn't open or create
$ 8 Color Server FatalError, couldn't open file
$ 9 Color Server FatalError, couldn't open system file

3 カラー・サーバ警告、ファイルのサイズは無効です。
4 カラー・サーバ警告、無効な情報 '
5 ' ファイルを削除し、再起動します。
10 カラー・サーバ致命的エラー、パレットの構造を作成するのに十分なメモリがありません。
11 警告: カラー・サーバのシステム・デフォルト・ファイルは壊れています。\n
12 カラー・サーバ警告、ファイルが読めません。%s\n


$set 29
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmHelp.c -- 
$ LOCALIZE Set 29
1 セッション・マネージャ・ヘルプ


$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$ 
$ Set 40 is for messages related to the X11R6 Session Management Protocol
$ 
$   o Do NOT localize the string '%s' in any of the messages in this set.
$ 
$set 40
$ Messages 1-7 are used when a session cannot be started.
$ Message 1 - this message will preface messages 2-7
1 セッションは以下のエラーのため起動できません。:
2 SmsInitialize は失敗しました。
3 IceListenForConnections は失敗しました。

$ 
$ Do not remove the whitespace at the beginning of lines 2 and 3
$   for message number 4
$ 
4 認証ファイル ~/.ICEauthority が作成されません。\n\
   ~/.ICEauthority-c と ~/.ICEauthority-l ファイルがあるならば、\n\
   セッションを開始する前に削除しなければなりません。

5 IceAddConnectionWatch は失敗しました。
6 IceComposeNetworkIds は失敗しました。
$ Message 7 - Do NOT translate "SESSION_MANAGER".
7 SESSION_MANAGER 環境変数を作成できません。
$ 
$ Messages 8-9 occur when a session database cannot be opened.
$ 
$ Message 8 - Do NOT remove the whitespace before the last '%s'.
8 以下のセッション・データベースを開けません。:\n\n\
   '%s'
$ Message 9 - Do NOT remove the whitespace before the last '%s'.
9 以下のセッション・データベースは要求されたリソース\n\n\
'%s' を含みません。:\n\n\
   %s
$ 
$ Messages 10-11 are used to report errors when reading a session database
$ 
$ Message 10 - Do NOT remove the whitespace before the last '%s'.
10 要求されたプロパティ '%s' は '%s' ファイル中のクライアント\n\n\
    %s \n\n\
に省略されています。
11 要求されたリソース '%s' は '%s' ファイル中のクライアント %d \n\n\
に省略されています。
$ 
$ Messages 12-14 used if errors occur when a session is restarted
$ 
$ Message 12 - Do NOT remove the whitespace before the last '%s'.
12 ディレクトリ '%s' \n\
はアプリケーション '%s' で利用できません。\n\n\
以下のディレクトリが使用されるでしょう。:\n\n\
   %s
$ Message 13 - Do NOT remove the whitespace before the last '%s'.
13 ホスト '%s' からこのホストに達せないため、\n\
13 ホスト '%s' で以下のアプリケーションが起動されません。:\n\n\
   %s
$ Message 14 - Do NOT remove the whitespace before the third '%s'.
$ Message 14 - "cannonical form" in this context means an internal
$   format.
14 ホスト '%s' で以下のディレクトリが\n\
   %s\n\
から正典に変えられないため、アプリケーション '%s' の起動に失敗しました。\n\n\
[%s]

$ 
$ Message 15 - used if the -session command line option has no value
$ 
15 コマンド行オプション -session でセッション名が提供されていません。
$ 
$ Message 16 - used if the Save_Session message contains an unrecognized
$    argument
$ 
16 セッション保存メッセージは認められない引き数 '%s' を含みます。

17 'ウィンドウ・マネージャ'からクライアント・リストを得るのに失敗しました。
$ 
$ Message 18 - the client database cannot be opened for writing.
$ Do NOT remove the whitespace before the last '%s'.
$ 
18 以下のデータベース・ファイルは書き込みのために開けません。:\n\n\
   %s\n\n\
セッションは保存されません。
$ 
$ Message 19 - used if an application cannot be saved
$ 
19 アプリケーション '%s' は保存されません。
$ 
$ Message 20 - used if the IceAcceptConnection function fails:
$ 
20 IceAcceptConnection は失敗しました。
$ 
$ Message 21 - used if a Save Session is requested and one is already
$    in progress
21 セッション保存が進行中のため、セッションは保存されません。

22 セッションのシャットダウンはアプリケーション '%s' によって取り消されました。

$ 
$ Messages 23-25 are used when a session is saved and an application 
$    has not set its required properties
$ 
$ Message 23 - Do NOT remove the whitespace before the last '%s'.
23 以下のアプリケーションは要求されたプロパティ '%s' \n\
を設定できません。\n\n\
   %s\n\n\
このアプリケーションは保存されません。

24 アプリケーション '%s' は、要求されたプロパティ '%s' が\n\
設定されなしため保存されないでしょう。

25 アプリケーション '%s' は要求されたプロパティ '%s' を設定しません。
$ 
$ Messages 26-27 are used when the DiscardCommand properties are
$    invoked - before an old session directory is removed.
$ Message 26 - do NOT remove the whitespace before the '%s'.
$ Message 26 - do NOT translate the word 'DiscardCommand' in
$    this message
26 以下の位案痔クライアント・データベースは開けません。:\n\n\
   %s\n\n\
'DiscardCommand' プロパティは実行されません。

$ 
$ Message 27 - do NOT translate the word 'DiscardCommand' in
$    this message
27 アプリケーション '%s' の失敗によって、'DiscardCommand' \n\
プロパティを実行します。

$ 
$ Messages 28-29 - used when an application exists and it wants the
$    Session Manager to post a dialog with a message that describes
$    the reason the client exited.
$ Message 29 is the title for the dialog
$ 
28 アプリケーション '%s' を以下の理由で終了します。:\n\n\
%s
29 アプリケーションを閉じる理由


$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $TOG: dtsession.msg /main/18 1999/09/20 13:50:16 mgreess $"

