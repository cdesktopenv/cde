$ $XConsortium: dthelpprint.msg /main/4 1996/10/16 03:15:20 cde-hit $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelpprint.msg
$ **
$ **  Project:     Cde1 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthelpprint
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in printed output
$ **        These messages are the default and they should all be localized.
$ **        These messages are marked with LOCALIZE THESE MESSAGES.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 7/11/94 for dthelpprint
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ Use the double quote char around all messages
$quote "

$set 2
$ *****  Module: PrintUtil.c *****
$ 

$set 3
$ *****  Module: PrintTopics.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s エラー: helpType はトピックですが、helpVolume が指定されていません。\n"
2 "%s エラー: ヘルプ・ボリューム %s を検出することができません。\n"
3 "%s エラー: ヘルプ・ボリューム %s を処理中に問題が発生しました。\n"
4 "%s エラー: メモリの割り当てに失敗しました。\n"
5 "%s エラー: トピック情報を獲得できません:\n\
ボリューム %s、位置 ID %s\n"
6 "%s エラー: 一時ファイル %s を開くことができません。\n"
$ String used to replace $SECTNUM when printing the index section
10 "索引"
$ String used to replace $SECTNUM when printing the table of contents section
11 "目次"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ index entry, page number
20 "%s, %d\n"
$ index subentry, page number
21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint
30 "__GENERATED-INDEX"
$ string used by can't get a topic title associated with an index subentry
31 ""

$set 4
$ *****  Module: PrintManStrFile.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s エラー: helpType は文字列ですが、stringData が指定されていません。\n"
2 "%s エラー: helpType は動的文字列ですが、stringData が指定されていません。\n"
3 "%s エラー: helpType はマニュアル・ページですが、manPage が指定されていません。\n"
4 "%s エラー: helpType はファイルですが、helpFile が指定されていません。\n"
5 "%s エラー: メモリの割り当てに失敗しました。\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint - ヘルプ用印刷プログラム\n\n"
2 "使用方法: dthelpprint [オプション]\n"
3 "印刷方法を制御するオプション:\n"
4 "\t-printer プリンタ名     使用するプリンタ\n"
5 "\t-copies 数値            印刷部数\n"
6 "\t-outputFile ファイル名  このファイルに出力を書き込みます。\n"
7 "\t-paperSize サイズ       この用紙サイズに合わせてに内容を書式化します。\n"
$ DO NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx
8 "\t\tsize = { help_papersize_letter|help_papersize_legal|\n"
9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display ディスプレイ名  リソースを獲得するディスプレイ\n"
11 "\t-name                    リソースを獲得する際に使用するプログラム名\n"
12 "\t-class                   リソースを獲得する際に使用するクラス名\n"
13 "\t-xrm リソース文字列      追加リソース\n"
14 "印刷対象を制御するオプション:\n"
15 "\t-helpType 型             ヘルプ・データの型\n"
16 "\t\t型 = 0 (ヘルプ・ボリューム)、 1 (文字列)、2 (マニュアル・ページ)、\
3 (ヘルプ・ファイル)\n"
17 "\t-helpVolume ボリューム   ヘルプ・ボリューム・ファイルの絶対パス\n"
18 "\t-locationId 位置         ボリュームのヘルプ・トピック名\n"
19 "\t-all                     ヘルプ・ボリュームのすべてのトピック、目次、\
および索引を印刷\n"
20 "\t-sub                     トピックの locationId とすべてのサブトピックを\
印刷\n"
21 "\t-one                           トピックの locationId を印刷\n"
22 "\t-toc                           ヘルプ・ボリュームの目次を印刷\n"
23 "\t-index                         ヘルプ・ボリュームの索引を印刷\n"
24 "\t-frontMatter                   ヘルプ・ボリュームの前付を印刷\n"
25 "\t-manPage マニュアル・ページ名  マニュアル・ページ名\n"
26 "\t-stringData 文字列             印刷するヘルプ・テキスト\n"
27 "\t-helpFile ファイル名           ヘルプ・テキストのファイル\n"
28 "\t-jobTitle タイトル             印刷ジョブ用のタイトル文字列\n"
29 "\t-topicTitle タイトル           ヘルプ・テキスト用のタイトル文字列\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s 警告: 用紙サイズ '%s' が不正です。help_papersize_letter を使用します。\n"
41 "%s 警告: 用紙サイズ、幅、または高さの値がありません。help_papersize_letter を使用します。\n"
42 "%s 警告: ディスプレイ %s を開けません。\n"
45 "警告: 一時ファイル用のメモリを割り当てることができません。\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s エラー: helpType %d が不正です。\n"



