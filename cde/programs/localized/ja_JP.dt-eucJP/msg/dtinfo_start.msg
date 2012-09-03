$ $TOG: dtinfo_start.msg /main/3 1999/09/20 13:29:53 mgreess $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.
$ 
$ ******************************************************************************
$ 
$set 1
$ The messages in this set will only appear in the users
$   "errorlog" file.
$ 
$ ******************************************************************************
$ 
$ Messages 1-5: These messages result from an error in a tt_* function
$    call.
$ 
1 ttsession(1)プロセスは実行していません。\n\
ToolTalkサービスは再起動できません。
2 現在のデフォルトプロセス確認は古いか不正です。
3 指定されたToolTalkセッションは古いが不正です。

$ 
$ Message 4: Do not localize the word DtInfo_Start.
$ 
4 プロセスタイプ 'DtInfo_Start'は\n\
インストールしたプロセスタイプではありません。

5 エラーナンバーは認められません。

6 malloc(1)が失敗したためアクションを起動できません。

$ 
$ Message 7: Do not localize the word DISPLAY.  Do not localize the
$   word XtOpenDisplay().
$ 
7 XrOpenDisplay()が失敗しました。おそらく環境変数DISPLAYが設定されていないか\n\
不正となっています。

$ 
$ Message 8: Do not localize the word DtAppInitialize().
$ 
8 DtAppInitialize()が失敗しました。おそらくデスクトップ環境が\n\
正しくインストールされていません。

$ 
$ Message 9: Do NOT localize the word DTINFOLIBDEFAULT
$ 
9 InfiLibには'%s'メッセージはありません。\n\
環境変数DTINFOLIBDEFAULTの値(%s)はInfoLibに使われていないでしょう。

$ 
$ Message 10: Do NOT localize the word DTINFOLIBDEFAULT
$ 
10 InfiLibには'%s'メッセージはありません。\n\
環境変数DTINFOLIBDEFAULTの値は定義されていません。

$ 
$ Messages 11-16: These message are the result of the dtinfo_start
$   getting an error while trying to parse a ToolTalk message.
$ 
$   Do NOT localize the word NULL
$   
11 メッセージ操作'%d'はサポートされていません。

12 メッセージ'%s'は要求された引き数の数'%d'を持ちません。

13 メッセージ'%s'はアクション名にNULLを指定しています。\n\
アクション'%s'は起動しません。

14 メッセージ'%s'は実行ホストにNULLを指定しています。

15 メッセージ'%s'はロケールにNULLを指定しています。

16 メッセージ'%s'はトピックにNULLを指定しています。

$set 2
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Dtinfo: Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\nExiting ..."
$ "-NEW"
3 OK
