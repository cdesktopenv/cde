$ $XConsortium: dtconfig.msg /main/5 1996/10/16 03:14:20 cde-hit $
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *
$ *****************************************************************************
$ **
$ **  File:        dtconfig.msg
$ **
$ **  Project:     CDE Session Management (dtconfig)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtconfig
$ **
$ **  Message set 10: IBM
$ **  Message set 20: Sun
$ **
$ *****************************************************************************

$ *****************************************************************************
$ **  Message set 10: For IBM platforms
$ *****************************************************************************

$set 10
1 スーパーユーザのみが実行できます。\n
$ 
2 CDE 環境を、デフォルトのユーザ・インタフェースとして設定しました。\n\
このインタフェースは、システムのすべてのユーザがログインした時に\n\
適用されます。変更の効果を確認するには、シャットダウンして、\n\
システムを再起動してください。\n
$ 
3 コマンド行を、デフォルトのユーザ・インタフェースとして設定しました。\n\
このインタフェースは、システムのすべてのユーザがログインした時に\n\
適用されます。変更の効果を確認するには、シャットダウンして、\n\
システムを再起動してください。\n
4 CDE 環境は、すでにデフォルトのユーザ・インタフェースとして\n\
設定されています。\n
$ 
5 コマンド行は、すでにデフォルトのユーザ・インタフェースとして\n\
設定されています。\n
$ 
6 使用方法: %s\n\
  -e (dtlogin の自動起動を使用可能にします。)\n\
  -d (dtlogin の自動起動を使用不可にします。)\n\
  -kill (dtlogin を強制終了します。)\n\
  -reset (dtlogin をリセットします。構成ファイルを再度読み取ります。)\n
$ 
7 引き数が多すぎます。\n
$ 
8  引き数を一つ指定してください。\n
$ 
9  xdm を使用不可にできません。xdm を使用不可にするには、xdm の\n\
マニュアル・ページを参照してから、再度行ってください。\n
$ 
10 Dtlogin を実行していません。\n

$ *****************************************************************************
$ **  Message set 20: For Sun Platforms
$ *****************************************************************************

$set 20
$ 
1  CDE 構成ユーティリティ
$ 
2   %s -d        (自動起動を使用不可にします。)
3   %s -e        (自動起動を使用可能にします。)
4   %s -kill     (dtlogin を強制終了します。)
5   %s -reset    (dtlogin をリセットします。)
6   %s -p        (プリンタ・アクションを更新します。)
$ 
$	In above messages 2 through 6, only translate words between
$	left "(" and right ").
$ 
7 %s: このスクリプトは、root で実行しなければなりません。
8 使用方法: 引き数が多すぎます。
9 使用方法: 引き数を一つ指定してください。
10 オプションが失敗しました。
11 %s を実行できません。
12 使用不可にすることに失敗しました。
13 %s を削除できません。
14 完了しました。
15 デスクトップの自動起動を使用不可にしました。
16 使用可能にすることに失敗しました。
17 %s が見つかりません。
18 %s を作成できません。
19 %s を作成できません。
20 デスクトップの自動起動を使用可能にしました。
21 dtlogin を強制終了しました。
22 dtlogin の構成リソースを再度読み込みました。
23 プリンタ・アクションの更新が完了しました。
24 使用方法:
25  %s -inetd    (inetd.conf /usr/dt デーモン)
26  %s -inetd.ow (inetd.conf /usr/openwin デーモン)
$ 
$       In above messages 25 and 26, only translate words between
$       left "(" and right ").
$ 
27 次のシステム起動時に、次のものは /usr/dt/bin から実行されます。
$ 
28     rpc.ttdbserverd         (ToolTalk)
29     rpc.cmsd                (カレンダ・マネージャ)
30     dtspcd                  (サブプロセス・コントロール)
$ 
$  Keep column allignment on messages 28 thru 30.
$  Only translate what's beteen ( )'s
$ 
31 次のシステム起動時に、次のものは /usr/openwin/bin から実行されます。
$ 
32     rpc.ttdbserverd         (ToolTalk)
33     rpc.cmsd                (カレンダ・マネージャ)
$ 
$  Keep column allignment on messages 32 thru 33.
$  Only translate what's beteen ( )'s
$ 
