$ $XConsortium: dtsearch.msg /main/6 1996/12/01 23:39:15 cde-hit $
$ 
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.

$   COMPONENT_NAME: austext
$ 
$   FUNCTIONS: database
$ 
$   ORIGINS: 27
$ 
$   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$   combined with the aggregated modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1994,1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.

$ ********************** DTSEARCH.MSG ************************
$ $Id: dtsearch.msg /main/6 1996/12/01 23:39:15 cde-hit $
$ December 1994.
$ Messages specific to DtSearch.
$ Symbolic msg sets replaced with numbers for portability.
$ Basically there is a msgset for each module.

$ $Log$
$ Revision 2.15  1996/04/10  22:54:07  miker
$ Revision 2.14  1996/04/10  19:47:18  miker
$ Revision 2.13  1996/03/20  19:32:13  miker
$ Revision 2.12  1996/03/13  22:53:07  miker
$ Revision 2.11  1996/03/05  16:04:30  miker
$ Revision 2.10  1996/02/01  17:16:48  miker
$ Revision 2.9  1995/12/27  21:17:19  miker
$ Revision 2.8  1995/12/27  16:04:46  miker
$ Revision 2.7  1995/12/07  23:26:31  miker
$ Revision 2.6  1995/12/01  16:10:25  miker
$ Revision 2.5  1995/10/26  16:21:21  miker
$ Revision 2.4  1995/10/20  21:30:42  miker
$ Revision 2.3  1995/10/19  20:52:30  miker
$ Revision 2.2  1995/10/03  21:37:17  miker
$ Revision 2.1  1995/09/22  17:54:34  miker

$quote "

$ General comments about all messages in this file:
$ - Any message that begins with a %1$s or %s is using that variable
$   to represent the name of the executable which is causing the
$   message to be output.
$ - Any variables specified in Usage messages should be translated
$   consistently.

$ ****************************************************
$ set 1 formerly set MS_misc, used many places
$set 1

1 "<null>"
$ message 2 can be translated to match local conventions.  It is used
$ in output files for information.
2 "%Y/%m/%d,%H:%M:%S"
3 "*** %1$s %2$s で %3$lu バイトメモリ不足です! ***\n"
4 "%1$s バージョン %2$s.\n"
$ message 6 refers to any value which is null when printing information.
6 "<unknown>"
8 "%1$s 有効でないデータベースが残っています。"
9 "%1$s 無効なコマンドライン引き数です。 '%2$s'\n"
10 "%1$s 未知のコマンドライン引き数 '%2$s' を無視します。\n"
11 "%1$s 無効なデータベース名 です。'%2$s'\n"
$ In message 13, do not translate "dtsrcreate." It is an executable name.
13 "%1$s '%2$s' データベースにDBが登録されていません。\n\
  通常の場合、データベースの初期化に失敗しています。(dtsrccreate実行)"
14 "%1$s プログラム設計バージョン '%2$s' はデータベース'%3$s'バージョン \
'%4$s' に矛盾します。\n"
15 "%1$s 中止シグナルを受け取りました。%2$d.\n"
16 "<環境変数がありません。>"
17 "<サーバ・デーモンがありません。>"
18 "%1$s データベース名が指定されていません。\n"
19 " ,\t\n"
21 "%1$s 終了コード = %2$d.\n"
$ Message 22 can be translated.  It represents a human-readable date like
$ "Thursday, Jan 5, 1996, 6:30 pm".  The "%5$I" is 12 hour clock time, but
$ it could be changed to "%5$H" for 24 hour clock time.
22 "%1$A, %2$b %3$d %4$Y, %5$I:%6$M %7$p"
23 "%1$s: バージョン %2$s.  実行 %3$s.\n"
$ Message 24 should not translate ".fzk".
24 "%1$s: 現在のワーキング・ディレクトリ = '%2$s', .fzk ファイル = '%3$s'\n"
25 "%1$s プログラム・エラーのため中止します。\n"
27 "%1$s: %2$s: ファイル・フォーマット不正です。"
96 "%s Hitlistはありません。"
301 "%1$s 入力テキストは %2$s です。"
$ Msg 362 is a generic system message.  The format is
$ "<program name> <filename>: <system error>" where <system error>
$ is a system errno which will already be translated.
362 "%1$s %2$s: %3$s"
$ Msg 400 is like "<Program name>: <file name>, line <line number>:..."
400 "%1$s: %2$s, line %3$ld: Invalid characters in word '%4$s'."
$ Msg 423 is like "<Program name> Word <word> in <file name> is a duplicate."
423 "%1$s 単語 '%2$s' に %3$s が2つあります。"
1596 "%1$s %2$s 監査ファイルを開けません。 %3$s: %4$s"

$ ****************************************************
$ set 2 formerly set MS_ausapi (ausapi.*)
$set 2

1 "%1$s データベース名 '%2$s'は不正または有効ではありません。"
$ Msg 7 should not translate "dbnames" or "dbcount."
7 "%s dbnames 指定されましたが dbcount ではありません。"
20 "%1$s search_type '%2$c'は不正です。"
30 "%s クエリーがNullです。検索を実行できません。"
$ Msg 37 should not translate "DtSearchInit."  It is a function name.
37 "%s はじめにAPI関数が DtSearchInit() をコールしなければなりません。\n"
40 "%1$s Nullでテキストを消去します。ハイライトは実行しません。"
$ Msg 115 must have the date formatted in the way specified.
115 "%1$s '%2$s' は日付の文字列として無効または不完全です。\n\
正確なフォーマットは '[yy]yy [mm [dd]]'です。"
$ Msg 187 must not translate "LOGNAME."  It is an environment variable.
187 "%1$s userid と環境変数 LOGNAME の両方ともありません。"
198 "%s '以前'の日付は '以後'の日付と同じかその後です。\n\
  レコードは戻りません。"
216 "\n%1$s %2$s シグナル %3$d を見つけました。\n"
294 "%1$s DtSearchResultsToTextでプログラム・ エラー"
437 "%s クエリーは無効なナビゲータ文字列を含みます。"
492 "%1$s 現在のhitlistはありません。"
498 "%1$s DtSearchResultsToText に無効な引数です。"
621 "%1$s %2$s はすでに初期化されています。"
806 "%1$s クエリーが不十分、または検索を始めるデータベース '%2$s' が\
検索オプションと矛盾しています。"
$ Msg 1072 "stem" is the root of a word.
1072 "%1$s プログラム・ エラー: 語幹数 (%2$d) は最大値 (%3$s) より大きいです。"
1342 "%1$s 検索エンジン・エラー %2$s ハイライト・リクエスト\n\
%3$s データベース、単語一致数=%4$ld, 検索タイプ='%5$c', テキスト='%6$.30s'"


$ ****************************************************
$ set 5 formerly set MS_chandel, used in dtsrhan.c
$set 5

3 "出力ファイル '%1$s' はすでに存在します。\n"
$ Msg 4 needs to use the same 2 characters from messages 4 and 5.
$ The "q" is not needed because it is assumed that any character
$ other than "a" or "o" means to quit the process.
4 "追加(a), 上書き(o), または 終了(q) ? [a,o,q] "
$ Msg 5 is the shortcut key for "a" or Append from message 4.
5 "a"
$ Msg 6 is the shortcut key for "o" or overwrite from message 4.
6 "o"
7 "出力ファイル '%1$s' を開けません\n"
11 "\nエラー - プロファイル '%1$s' ファイルを開けません。\n"
12 "エラー行 %1$d: 無効な識別子 '%2$s' です。\n"
13 "エラー行 %1$d - 識別子 '%2$s' は値がありません。\n"
14 "エラー行 %1$d - '%2$s' はゼロまたは不正な値です。\n\
攻撃的なトークン : %3$s \n"
$ Msg 15 should not translate "ANY."
15 "エラー行 %1$d - 識別子 '%2$s'、サイン文字列を確認できませんが、\n\
カラムはいくらでも設定されます。 \n"
16 "エラー行 %1$d - '%2$s' はゼロまたは不正な値です。\n\
攻撃的なトークン : %3$s \n"
18 "エラー行 %1$d -識別子 '%2$s' の値がありません。\n"
23 "警告行 %1$d: 再定義の範囲を定めます。\n"
25 "エラー行 %1$d: delimiter not specified as 'top' or 'bottom'.\n"
26 "エラー行 %1$d: 無効なトークン '%1$s'です。n"
27 "エラー行 %1$d: 無効なトークン '%2$s'です。n"
$ Msg 34 needs to match the 'all' from msgs 36 and 37.
34 "all"
$ Msg 35 needs to match the 'none' from msgs 36 and 37.
35 "none"
$ Msg 36 needs 'all' to match msg 34 and 'none' to match msg 35.
36 "エラー行 %1$d: イメージ・モードは'すべて'か'なし'でなくてはならない。 -'%2$s' は認めない。\n"
$ Msg 37 needs "all" to match msg 34 and "none" to match msg 35.
37 "エラー行 %1$d: テキスト・モードは'すべて'か'なし'でなくてはならない。 -'%2$s' は認めない。\n"
38 "警告行 %1$d: キー文字を再定義します。\n"
39 "エラー line %1$d: 無効なキー文字:'%2$c' です。\n"
40 "警告行 %1$d - キー・フィールドを再定義します。\n"
43 "エラー行 %1$d - キーには不正な識別子 '%2$s' です。\n"
$ Msg 45 should not translate "discard."
45 "エラー行 %1$d: 'discard'には未知のオプションです。: '%2$s'\n"
$ Msg 46 should not translate "delblanklines."
46 "エラー行 %1$d: 'delblanklines'には未知のオプションです。: '%2$s'\n"
47 "エラー行 %1$d: アブストラクトには未知のオプションです。:'%2$s'\n"
48 "エラー行 %1$d -未知の識別子のタイプ '%2$s' です。\n"
49 "エラー - 定義されていない範囲です。\n"
50 "エラー - キータイプ文字は定義されていません。\n"
51 "エラー - キーは定義しません。\n"
52 "エラー - '%1$s' の様に定義する範囲は見つかりません。\n"
53 "エラー - レコード中の物理的ラインを参照する '%1$s' の様に範囲を定義しました。\n\
   物理的ラインの範囲を定義してから\n\
  物理的ラインとして参照することができます。\n"
54 "エラー - フィールド '%1$s' 、 '%2$s'に匹敵すると確認されるラインはありません。\n"
55 "エラー - include/exclude フィールドのリストは\n\
  定義されていない '%1$s' フィールドを含みます。\n"
56 "エラー - include/exclude イメージのリストは\n\
  定義されていない '%1$s'行を含みます。\n"
57 "エラー - include/exclude テキストのリストは\n\
  定義されていない '%1$s'行を含みます。\n"
58 "エラー - キー定義参照 '%1$s' フィールドは、\n\
  定義されていません。\n"
59 "エラー - アブストラクト定義参照 '%1$s'フィールドは\n\
  定義されていません。\n"
68 "警告 - キーに必要なフィールドが見つかりません。\n\
  %1$ld レコードを捨てて、始めました。: %2$s\n"
69 "警告 - レコード %1$ld は無意味と思います。捨てます...\n\
  レコードを始めました。: %2$.60s\n"
70 " 入力ファイル '%1$s' を開けません。n"
$ Msg 71 should not translate ".fzk."
71 "\n\
使用方法: %1$s [options] <profile> <infile> [<outfile>]\n\
  -m         エラー・メッセージを除いてはすべてを止めます。\n\
  -wN        ターゲットス・クリーン幅を <N> に変更します。\n\
  -oo        認可前に出力ファイルに上書きします。\n\
  -oa        認可前に出力ファイルに付加します。\n\
  <profile>  プロセスになるレコードのプロファイルを\n\
             含む入力ファイル。\n\
  <infile>   実際のレコードを含む入力ファイル。\n\
  <outfile>  出力ファイル名。.fzk フォーマット(%2$s)。\n\n"
72 "無効なスクリーン幅を指定しました。\n"
75 "'%1$s' は無効な出力モードです。\n"
76 "未知のコマンドライン引き数 '%1$s' です。\n"
77 "プロファイル-ファイル名の要求はありません。\n"
78 "入力ファイル名の要求はありません。\n"
79 "エラー - 入力として stdin を使う。出力ファイル名を要求する!\n"
80 "プロファイルファイルと入力ファイルは同じ名前です。:'%1$s'\n"
81 "入力ファイルと出力ファイルは同じ名前です。:'%1$s'\n"
82 "プロファイルファイルと出力ファイルは同じ名前です。:'%1$s'\n"
83 "  プロファイルファイル: %1$s\n"
$ Msg 84 should not translate "stdin."
84 "          入力ファイル: stdin\n"
85 "          入力ファイル: %1$s\n"
$ Msg 86 should not translate "stdout."
86 "          出力ファイル: stdout\n"
87 "          出力ファイル: %1$s\n"
88 "%1$s バージョン %2$s. %3$s テキスト・フィルタ \n"
89 "\n  どのドットも %ld レコードです...\n"
90 "\n出力ファイルはエラーで終わります。- ディスクがいっぱいではないですか?\n"
91 "プロファイルファイルのエラーのため中止します。\n"
92 "\n%1$s: 通常の完成です。%2$ld レコードを処理しました。Exit code = 0.\n"
93 "エラー行 %1$d - '%2$s' 文字列はダブル・クォーテーションで囲まれていません。\n"
110 "警告行 %d - データ・フィールドを再定義しています。\n"
111 "エラー行 %1$d - データに不正な識別子です '%2$s'。\n"
115 "%1$s デフォルト・オブジェクト・データは '%2$s' になるでしょう。\n"
116 "エラー - データ再定義はフィールド '%s' に定義されません。\n"
124 "%1$s 出力ファイル '%2$s' に書き込めません。:\n %3$s\n"
$ Msg 125 should not be translated.
125 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"
130 "%1$s 言語加工ファイルを読み込めません。:\n%2$s\n"
133 "\n\
警告 - '%1$s' は無効なデータ指定です。\n\
  '%2$s' データを レコード数 %3$ld に使い、始めました。: %4$.30s\n"


$ ****************************************************
$ set 6 formerly set MS_ve
$set 6

26 "dummy_workproc() をコールしています。\n"
142 "%1$s クライアント・エラー: 不正なデータベース・アドレス %2$ld \n\
(%3$d:%4$ld, x'%5$08.8lx')をデータベース'%6$s'に登録しようとしています。"
156 "データベース '%1$s' を使用した圧縮テーブルは矛盾しています。\n\
  データベースは %2$ld で圧縮され、非圧縮エンジンは %3$ldです。\n"
157 "DTSVE157 クライアント・プログラム・エラー: usrblk.dba中のデータベース・アドレスはNullです。"
167 "%1$s データベース '%2$s' 中にデータがありません。"
178 "%1$sデータベース '%2$s' バージョン '%3$s'は \n\
エンジン・バージョン '%4$s' に矛盾しています。"
183 "%1$s ユーザに示すシグナルファイル '%2$s': %3$s を開けません。\n"
185 "%1$s データベース '%2$s' 最大単語サイズ %3$d は短過ぎます。"
199 "%1$s ユーザに示すシグナル '%1$s' %2$d 試みた範囲で得られません。\n"
230 "%1$s ユーザに示すバックアップファイル '%2$s': %3$s が開けません。"
231 "%1$s データベース '%2$s' は理論サイズ %3$d が無効です。" 
251 "%1$s データベース '%2$s' は変造されています。:\n\
レコード数とデータベース・アドレスが矛盾しています。\n"
$ Msgs 310 and 311 can be appended to msg 309 to produce output.
$ For example, one message might be
$ "<Programname> User notes disabled for entire Engine."
309 "%1$s ユーザに不可能を知らせます。 "
$ Msg 310 can be appended to msg 309 to produce output.
310 "完全なエンジンで"
$ Msg 311 can be appended to msg 309 to produce output.
311 "データベース '%1$s' で"
$ Msg 332 can be translated to local timestamp.  It currently outputs
$ something like "1996/03/04 at 16:40 CDT"
332 "%Y/%m/%d の %H:%M %Z"
333 "\n <ユーザに '%1$s' の%2$s を付加することを示す。>\n"
398 "DTSRVE398 クエリー文字列がNullです。"

$ ****************************************************
$ set 7 formerly set MS_uiutil, no longer used in uiutil/dtsrutil.c.

$ ****************************************************
$ set 8 formerly set MS_loadocf, for module ocf.c.

$ All words in set 8 which are capitalized should not be translated.
$ They represent keywords in a configuration file.  Words like PATH,
$ SEARCH, MAXHITS, FILEIO, KEYTYPE, FILEPATH, KEYTYPES, KEYTYPE,
$ UPPERKEYS, FALSE, and HOME.

$set 8

$ Msg 1 is "<Programname><filename>: <keyword> keyword is obsolete."
1 "%1$s%2$s: %3$s キーワードは 時代遅れです。"
$ Msg 140 "(missing)" refers to a token which is expected to be found but
$ is missing.  The token is from an input string of words being indexed.
140 "(missing)"
$ Msgs 142-168 are combined into "<token>: <error>" where msg 142 or a
$ default file name can be <token> and msgs 143, 165, 166, and 168
$ can be <error>.
142 "残りの行は"
143 "ありません"
165 "未知のidです。"
166 "前に指定しています。"
168 "パスの指定がされていません。"
184 "%1$s %2$s: データベースの名前'%3$.16s'が8文字より多いです。\n"
$ Msg 221 should not translate "PATH."
221 "%1$s %2$s: データベース '%3$s', PATHが指定されていません。\
ステートメントは無視されます。\n"
$ Msg 275 should not translate "SEARCH."
275 "%1$s %2$s: 不正な SEARCH 値は無視されます。"
$ Msg 304 should not translate "MAXHITS."
304 "%1$s %2$s: データベース '%3$s', MAXHITSの値が不正です。\n"
352 "%1$s %2$s: %3$sの値が不正です。行を無視します。\n"
365 "%1$s %2$s: %3$s 値がありません。"
414 "%1$s 構成ファイル '%2$s'が開けません。:\n %3$s"
457 "'%1$c' レコード"
477 "%1$s %2$s: キーワード '%3$s' の後、データベースの名前が有りません。\n"
495 "%1$s %2$s: '%3$.16s' 不正なキーワードです。%4$d 行めを無視します。\n"
$ Msg 607 should not translate "FILEIO."
607 "%1$s %2$s: FILEIOの値が不正です。ユーザファイルは読み取りも書き込みも\
禁止です。"
610 "%1$s %2$s: %3$s の値は '%4$s'です。 正の整数にしてください。\
行を無視します。"
$ Msg 684 should not translate "KEYTYPE."
684 "%1$s %2$s: データベース '%3$s', KEYTYPE文字 '%4$c'が無効です。"
732 "%1$s %2$s: データベース '%3$s', キータイプに許されたのはたった %4$d です。\n"
$ Msg 903 should not translate "FILEPATH."
903 "%1$s %2$s: FILEPATH '%3$s' %4$s。 行を無視します。"
929 "%1$s 古いスタイル構成ファイルを開きます '%2$s'。"
$ Msg 986 should not translate "KEYTYPES."
986 "%1$s %2$s: KEYTYPES がデータベース '%3$s' から抜けています。"
$ Msg 1002 should not translate "KEYTYPE."
1002 "%1$s %2$s: データベース '%3$s', KEYTYPE 文字 '%4$c'は、全く同じです。\n"
1007 "%1$s%2$s: %3$s 文字列がなく、不正です。"
$ Msg 1011 should not translate "KEYTYPE," "UPPERKEYS," or "FALSE."
1011 "%1$s %2$s: データベース '%3$s': 小文字のKEYTYPE文字 '%4$c' の登録はできません。\n\
  小文字のキーにするつもりならば、UPPERKEYS = FALSE を設定してください。"
1404 "%1$s ステータスは構成ファイル '%2$s': %3$sで無効です。"
$ Msg 1643 should not translate "DtSrOCFPATH," "HOME," "/etc/inittab,"
$ or "/etc/inetd.conf."
1643 "%1$s 以下の位置には構成ファイル'%2$s'が見つからずに開けません。\n\
  DtSrOCFPATH ディレクトリ =     %3$s,\n\
  現在の作業ディレクトリ = %4$s,\n\
  HOME ディレクトリ =            %5$s,\n\
  /etc/inittab ディレクトリ =    %6$s.\n\
  /etc/inetd.conf ディレクトリ = %7$s."
1869 "%1$s %2$s: 構成ファイルには少くとも1つのデータの指定が必要です。"

$ ****************************************************
$ set 9 formerly set MS_oeinit, used in oeinit.c and opendblk.c.
$set 9

$ Msg 99 should not translate "open_dblk".  It is a function name.
99 "%s プログラミング エラー: open_dblk()は,無効な入力です。"
137 "%1$s ユーザ・インターフェース・バージョン%2$s とエンジン・バージョン %3$s \
は、矛盾しています。"
202 "%s 構成ファイル中のエラーのために初期化に失敗しました。"
265 "%s 有効なデータベースは残っていません。"
266 "%1$s データベース、言語、または関係するファイルのために\n\
初期化に失敗しました"
317 "%1$s データベースファイル '%2$s' を開けません。\n\
  Errno %3$d = %4$s\n\
  %5$s は '%6$s' を有効なデータベースのリストから削除しました。"
318 "\n  構成ファイルのある有効なデータベースパスを指定することで、\n\
  誤りを訂正できます。"
1404 "%1$s データベース '%2$s' を有効なデータベースのリストから削除します。\
ファイル%3$s: %4$sが無効な状態のためです。"

$ ****************************************************
$ set 10 formerly set MS_oe, used in oe.c and dbchange.c.
$set 10
48 "OE48 リクエスト中止: '%1$s' データベースはこの位置で利用できません。"
71 "OE71 %1$s は終了します。"
$ Msg 76 should not translate "dblk".  It is a structure name.
76 "OE76 プログラム・エラー: dblk がデータベース '%1$s' で開けません。"
217 "OE217 プログラム・エラー: レコードテキストが長過ぎます。"
367 "OE367: ユーザ・インタフェース・エラー 。%1$d は不正なリクエスト・コードです。\n"
429 "OE429 削除ファイル '%1$s': %2$s は開けません。リクエストは無効です。"
440 "OE440 データベース '%1$s' 中に選択されたキータイプのレコードはありません。"
490 "OE490 %1$s エンジンは永久に無力です。"
516 "OE516 検索文字には %1$d 文字しか許しません。"
523 "OE523 リクエスト拒否: 第一のリクエストはエンジン初期化です。"
753 "%1$s 意味カテゴリはデータベース '%2$s'を定義していません。"
1300 "%1$s データベース・ファイル '%2$s': %3$sのステータスにアクセスできません。"
1313 "%1$s *** リクエスト キャンセル *** %2$s エンジン再初期化\n\
  エンジンは、たぶん１つかそれ以上のデータベースの更新によって引き起こされる\n\
%3$s ファイルの正当の変更を再初期化します。"


$ ****************************************************
$ set 11 formerly set MS_cravel, used in dtsrload.c.
$set 11
1 "\n\
使用方法: %1$s -d<dbname> [options] <infile>\n\
       無効にできるデフォルト・ファイル名の拡張子をリストします。\n\
  -d<dbname>  1 - 8 文字のデータベース名、オプショナルパス接頭辞を含みます。\n\
              ファイル名拡張子は自動的に付加されます。\n\
  -t<etxstr>  範囲を定めた文字列のテキスト・ドキュメントの終わり。デフォルト '\\f\\n'.\n\
  -c          数えるレコードによって数えられたデータベース・レコードを初期化します。\n\
  -p<N>       各Nレコード(デフォルト %2$d) の進捗点を印刷する。\n\
  -h<N>       重複したrec idのハッシュ・テーブルサイズを %3$ld から N に変更します。\n\
              -h0 重複がないことを意味し、それらを調べません。\n\
  -e<path>    huffman エンコード・テーブル(デフォルト %4$s) のパス-ファイル名 \n\
  <infile>    入力 [パス] ファイル名。デフォルト拡張子 %5%s \n"
2 "%1$s: 終了コード = %2$d\n"
13 "%1$s 重複したレコードidは調べられません。\n"
14 "\n%1$s 未知のコマンド・ライン引き数 '%2$s'です。\n"
15 "\n入力ファイルメニュー名が命じられません。\07"
16 "\nデータベース名が指定されていません。(-d 引き数)\07"
17 "%1$s 実際数えられたデータベース中の総レコード数を初期化します...\n"
18 "%1$s: '%2$s' スキーマ・バージョン = %3$s、レコード数 = %4$ld、last slot = %5$ld.\n"
19 "%1$s: 最終的なデータベースレコード count = %2$ld, last slot = %3$ld.\n"
20 "%1$s %2$ld 入力レコードは %3$ldm %4$lds で処理されました。(%5$ld%%).\n\
  %6$ld 重複, %7$ld 新規, %8$ld 更新.\n"
29 "%1$s %2$s: %3$s のステータスを得られません。\n"
30 "%1$s ファイル %2$s は空です。\n"
31 "%1$s: どの dot = %2$d レコードも処理しました。\n"
33 "%1$s 捨てたレコード数 %2$ld:  長過ぎるキー:\n  '%3$s'.\n"
34 "%1$s メモリ不足です!  -h 引き数に小さな数を設定するか\n\
  入力レコードの数を下げて下さい。"
35 "%1$s: 捨てた重複したレコード数 %2$ld '%3$s'\n"
246 "\n%1$s '%2$s' は無効なパス/ディレクトリ名です。\n"
522 "%s %s のこのバージョンは意味データベースをサポートしていません。\n"
$ Msg 579 should not translate ".fzk."
579 "%1$s rec #%2$ld を捨てました。:  無効な .fzk ファイル・フォーマットです。\n"
582 "%1$s dot 引き数は、無視された無効な処理をします。 '%2$s'.\n"
666 "%1$sプログラミング・エラー  未知の state = %2$d です。\n"
717 "%1$s 取り消します: 非圧縮テキストはまだサポートしていません。\n"
844 "%1$s %2$ld の警告で取り消します。\
  警告数はコマンド・ラインで指定されています。\n"
926 "%1$s 引数 -m 無視します: %2$s: %3$s。\n"
927 "%1$s 捨てたレコード数 %2$ld :  1文字めの(keytype)キー \n\
  '%3$s' は英数字ではありません。\n"
1048 "%1$sデータベース・オブジェクトは実際に入力ファイルが空なので、変更しません。\n"
1086 "%1$s レコード '%2$s' は不正なデータ・フォーマットです。--実行データを使います。\n"
1153 "%1$s '%2$s'中の圧縮テーブルidの現在のデータは is %3$ld です。\n\
  データベース '%4$s' は以前、テーブル %5$ld で圧縮しています。\n"
1215 "%1$s 警告。  レコード '%2$s' はテキストがありません。\n"


$ ****************************************************
$ set 12 formerly set MS_initausd, used in dtsrcreate.c.
$set 12

$ Msg 3 should not translate "dtsearch.dbd."
3 "\n\
使用方法: %1$s [-options] <dbname>\n\
  DtSearch/AusTextデータベース・ファイルを作成し、初期化します。\n\
  -q       インフォメーション・メッセージを印刷しません。\n\
  -o       先在しているデータベースの上書きを了解します。\n\
  -a<N>    最大抽象サイズを <N> に設定します。(デフォルト フレーバーによって)\n\
  -d<dir>  データベース名ディレクトリで見つからない場合、dtsearch.dbdファイルの検索する場所。\n\
  -wn<N>   最小単語サイズを <N> に変更します。 デフォルトは %2$dです。\n\
  -wx<N>   最大単語サイズを <N> に変更します。 デフォルトは言語によります。\n\
  ---------- データベース フレーバー ----------\n\
  -fd      DtSearch フレーバー。ドキュメントがありません。ドキュメントはリファレンスです(defaelt)。\n\
  -fa      AusText フレーバー.  ドキュメントは中心のサーバの貯蔵所に蓄えています。\n\
  ------------ 言語のサポート ------------\n\
  -l<N>  言語ナンバーに <N> を設定します。デフォルトは0です。サポートされた値は:\n\
           0 英語-ASCII\n\
           1 英語-Latin1\n\
           2 スペイン語\n\
           3 フランス語\n\
           4 イタリア語\n\
           5 ドイツ語\n\
           6 日本語-autoknj\n\
           7 日本語-knjlist\n\
  <dbname> オプション・パス接頭字、 1 - 8 文字以上\n"

5 "DTSRCREATE 最小単語サイズ %1$d は最大単語サイズ %2$d より大きい。\n"
6 "DTSRCREATE 最小単語サイズを %1$d に調整してください。\n"
8 "DTSRCREATE 最大単語サイズを %1$d に調整してください。\n"
9 "DTSRCREATE 小さい最小単語サイズの指定は 意味ありげに増えた索引が\n\
  要求物を貯蔵するように要求するかも知れない。\n"
10 "DTSRCREATE 大きい最大単語サイズの指定は要求物の貯蔵を意味ありげに\n\
増やすかも知れない。 \n"
$ Msg 12 needs option "y" for "yes" to be translated the same as in msg 15.
$ Option "n" does not need a translation because the code assumes anything
$ other than the translated "y" option means "no."
12 "\nファイル '%s' はすでに存在しています。\n\
他のデータベース・ファイルに上書きしてもいいですか? [y,n] "
$ Msg 13 should not translate "load_custom_language."
13 "%s 警告! サポートされていない言語習慣を指定しました。 custom language.\n\
  You will have to provide your own language loaders at run time\n\
  in user function 'load_custom_language' to access this database.\07\n"

14 "\n\
%s semantic processing is only available for English language databases.\n\07"
$ Msg 15 needs "y" to match the "yes" option in msg 12.
15 "y"
24 "DTSRCREATE データベース '%1$s' の初期化に成功しました。\n"
98 "DTSRCREATE %1$s Invalid Huffman 圧縮テーブル id."
186 "DTSRCREATE186 '%1$s' を '%2$s'に名前を変更することはできません。:\n  %3$s\n"
205 "DTSRCREATE205 '%1$s'アクセス権の変更はできません。: %2$s\n"
213 "%1$s '%2$s' を開けません。:\n  %3$s.\07\n"
214 "%1$s '%2$s' を '%3$s'にコピーできません。:\n  %4$s\n"
230 "DTSRCREATE230 データベース '%1$s' を開けません。\n"
239 "DTSRCREATE239 データベース '%1$s' を初期化できません。\n"
244 "DTSRCREATE244 '%1$s' を削除できません。:\n  %2$s\n"
326 "DTSRCREATE326 '%1$s' のアクセス権を変更できません。:\n  %2$s\n\
  すべてのデータベースファイルを読み込み書き込み可能にしなければなりません。\n\
  データベースファイルを開けないか、初期化できていないかもしれません。\n"
327 "  しかしながら、すべてのデータベースファイルが\n\
  読み込み書き込み権限を持たないならば、便利ではないかもしれない。"
433 "%1$s抽象サイズは %2$hd 以上に調整してください。\n"
509 "DTSRCREATE509 データベース・ヘッダ・レコードを初期化できません。\n"

$ ****************************************************
$ set 13 formerly set MS_vista.
$ Msg numbers 1-100 and 900-1000 match
$ user_error[] and system_error[] respectively in dberr.h.
$ Used in dberr.c, ausdopen.c, and opendblk.c.

$ Msgs 1-100 and 900-1000 in set 13 are generated during database
$ input/output.  They are all fatal errors and should be treated as
$ standalone messages.  They are not combined with other messages to
$ produce errors.  The messages come from a third party vendor, so we
$ do not intend to modify the vendor code and make complete sentences
$ out of them.

$set 13

$ For msgs 1-100, see comments at top of set 13.
1 "データベースを開けません。"
2 "無効な設定です。"
3 "無効なレコードです。"
4 "無効なデータベースです。"
5 "無効なフィールド名です。"
6 "無効な db_addressです。"
7 "現在のレコードではありません。"
8 "現在の所有者の設定がありません。"
9 "現在のメンバーの設定がありません。"
10 "キー値が必要です。"
11 "無効なロック値です。"
12 "レコードは空でないセットの所有者です。"
13 "レコードはセットのメンバーです。"
14 "メンバーはすでに所有しています。"
15 "フィールドはコンパウンドキーです。"
16 "レコードはセットに接続していません。"
17 "フィールドは有効なキーではありません。"
18 "レコードはセットの正当な所有者ではありません。"
19 "レコードはセットの正当なメンバーではありません。"
20 "d_setpagesでエラーです。 (データベースが開けないか bad param )"
21 "辞書ファイルに矛盾します。"
22 "システムのレコードを削除することは不法な試みです。"
23 "ロックされたセット/レコードをロックすることは不法な試みです。"
24 "ロックされていないセット/レコードにアクセスすることを試みます。"
25 "トランザクション id は供給されていません。"
26 "トランザクションはすでに起動しています。"
27 "トランザクションは起動していません。"
28 "トランザクションはロックされたファイルのため始められません。"
29 "トランザクションの範囲でロックを解放できません。"
30 "データベース・ページの変更が多過ぎます。"
31 "データベース外のトランザクションを更新できません。"
32 "アクセス要求を除外します。"
33 "静的ファイルで禁止されているロックを書き込みます。"
34 "ユーザ id を重複または指定していません。"
35 "データベース、パスまたはファイルの名前が長過ぎます。"
36 "無効なファイル数は d_renfile となります。"
37 "フィールドはオプショナル・キー・フィールドではありません。"
38 "フィールドは願剤のレコード・タイプとして定義されていません。"
39 "レコード/フィールドはコンパウンド・キーです。"
40 "無効なレコードまたはセット数です。"
41 "レコード/セット timestamped ではありません。"
42 "不正な DBUSERID です。(英数字以外を含みます)"
43 "NetBIOS はマシンにインストールされていません。"
44 "DBDPATHの要素の番号が不正です。"
45 "DBFPATHの要素の番号が不正です。"
46 "現在のレコード・タイプではありません。"
47 "現在のテーブルをソートする文字列は無効です。"
48 "データベースは閉じていません。"
$ Msg 301 should not translate "errno."
301 "\n*** システム I/O エラー %1$d = %2$s\n"
302 "\n\
*** たいていこの種類のエラーはデータベース・ファイルが見つからないか、\n\
*** 読み取り専用です。さもなければ、いくつかのシステムでは、\n\
*** ファイルを開く数の限界を超えています。"
303 "(ロケーションを指定していません。)"
$ Msg 304 should not translate "db_VISTA."
304 "\n*** db_VISTA プロセスを自動回復します...\n"
$ Msg 311 should not translate "db_status."
311 "*** %1$s データベース・エラー、 db_status = %2$d: %3$n"
312 "プログラミング・エラーです。"
313 "未知のエラーです。"
315 "他のユーザによって使用されているため、データベースを利用できません。"
348 "%1$s 無効なデータベース名です。 '%2$s'"
378 "%1$s 以下のデータベース名を開けません。:\n  '%2$s'"
379 "%1$s データベース '%2$s' を開けません。:\n  %3$s."

$ For msgs 900-1000, see comments at top of set 13.
900 "ファイルにスペースがありません。"
901 "システム・エラー"
902 "ページ・フォルト"
$ Msg 903 should not translate "dio."
903 "dio 中のワーキング・ファイルがありません。"
904 "十分なメモリをアロケートできません。"
905 "エラー オープニング・ファイル"
906 "データベース taf ファイルにアクセスを得られません。"
907 "データベース taf/log ファイルエラー"
908 "データベース・ロックが矛盾してます。"
909 "ファイル・レコードは限度を超えています。"
910 "キー・ファイル矛盾"
911 "最大同時ユーザが限界を超えました。"
912 "データベース・ファイル中のエラーを捜します。"
913 "指定されたファイルは無効です。"
914 "データベース/オーバーフローのファイルからエラーを読み込みました。"
915 "マネージャの動機エラーをロックします。"
916 "デバッグ・チェックを中断します。"
917 "ネットワーク・コミュニケーション・エラー"
918 "プロセスをオート・リカバリーします。"
919 "データベース/オーバーフローのファイルにエラーを書き込みました。"
920 "インストールされたマネージャはロックされていません。"
921 "DBUSERID はすでに使われています。"
922 "ロック・マネージャはビジーです。"


$ ****************************************************
$ set 14 formerly set MS_cborodin, used in dtsrindex.c.
$set 14

1 "%1$s.  実行 %2$s 。\n"
2 "%1$s: 警告が見つけられました。"
$ Msg 6 should not be translated.
6 "put_addrs_2_dtbs_addr_file() nitems=%1$d, batchsz=%2$ld\n"
$ Msg 9 should not translate "d99."
9 "\n\
\n%1$s Corrupted d99 file for word '%2$s',\n\
  database address %3$ld @ file position %4$ld => bitvector[%5$ld],\n\
  but maximum bitvector allocation = %6$ld.\n"
15 "%1$s 入力が2バイトより少いです。\n"
17 "\n\
使用方法:  %1$s -d<dbname> [options] <infile>\n\
        上書き可能なデフォルト・ファイル名の拡張をリストします。\n\
  -d<dbname>  1 - 8文字のデータベース名、オプショナル・パス接頭語を含みます。\n\
  -t<etxstr>  ドキュメント範囲の文字列のテキストの終わり。デフォルト '\\f\\n'.\n\
  -r<N>       %2$d から <N> に1レコードパードットパスを変更します。\n\
  -b<N>       %3$ld から <N> に最大バッチサイズを変更します。\n\
  -c<N>       %4$ld 1K ページから <N> 1K ページにデータベース・ページング・キャッシュを変更します。.\n\
              N >= 16 by powers of 2.  初めに小さな変更のみを試みます。\n\
  -i<N>       デフォルト %5$d から <N> に(I)nput バッファサイズを変更します。\n\
  -h<N>       %6$ld から <N> に2重のrec id のハッシュ・テーブル・サイズを変更します。\n\
              -h0 は重複がないことを意味し、それらを調べません。\n\
   <infile>   入力[パス]ファイル名。デフォルト拡張 %7$s\n"
21 "\n\
\n%s '%1$s' レコードが単語カウンタ配列を超えました。 \n\
  レコード数 %2$ld > maxdba %3$ld, dba=%4$ld, sld00=%5$d, offs=%6$d.\n"
22 "\n\
\n%1$s '%2$s' データベース中のレコード (dba=%3$ld)\n\
  が bitvector allocation (%4$ld >= %5$ld)を超えました。\n"
33 "\n%1$s 入ってくるレコードの数が %2$d を超えました。\n\
  これは、Pass 2 のエラーとデータベースの変造で 'ページング空間の不足' \n\
  の結果です。どちらもレコードを数え -b オプションを使い、\n\
  縮めるため入ってくるファイルを分割し、戻します。\n"
40 "%1$ld 重複したレコードを無視します。"
41 "重複したレコードはありません。"
164 "\n%1$s: %2$s シグナル %3$d のため取り消します。 データベース %4$s は\n\
  おそらく不正です。バックアップ・データベースを復元してください。\n"
206 "%1$s: Exit code = %2$d, 経過時間は %3$ldm %4$lds です。\n"
$ Msg 280 should not be translated.
280 "\n%1$s num_addrs_for_word (%2$d) >= batchsz (%3$ld).\n"
288 "%1$s 取り消します。入力ファイル %2$s に単語がありません。\n"
374 "\n%1$s メモリ不足です ! \n\
  入ってきたファイルをいくつかの小さなファイルに分割し、再度試みます。\n"
539 "%1$s 重複したレコード id の照合は無力です。\n"
550 "%1$s '%2$s' はパス/データベース名を無効とします。\n"
558 "%1$s 入力バッファサイズ '%2$s' は無効です。\n"
567 "%1$s 未知のコマンドライン引き数です。'%2$s'\n"
577 "%1$s 引き数 '%2$s'は無効です。デフォルト -r%3$d を使います。\n"
580 "%1$s 入力ファイル名の要求がありません。\n"
589 "%1$s データベース名の指定 (-d 引き数) がありません。\07\n"
595 "%1$s バッチ・サイズ引き数 '%2$s' は無効です。\n"
600 "%1$sキャッシュ・サイズは %2$d に再調整します。\n"
659 "\n%1$s: %2$s 不用なレコードです。キーが長過ぎます。:\n  '%3$s'.\n"
713 "\n%1$s 単語 '%2$s' は %3$ld レコードにあります。\n\
  しかしデータベース中の %4$ld レコードのみです !\n\
  (これはストップリストの良い候補に成るでしょう。).\n"
727 "\n%1$s dbaddr レコード数 (%2$ld) \
 bitvector アロケーション (%3$ld) を超えます。\n"
$ Msg 776 should not translate "d99."
776 "%1$s 書き込み失敗 d99 ファイル: %2$s\n"
$ Msg 848 should not translate "d99."
848 "\n%1$s %2$ld バイト (%3$ld dba's) d99 ファイルの解放が\n\
  オフセット %4$ld でできません。dbaの読み取り数 (return code) = %5$ld\n"
849 "\n%1$s: 単語数 %2$ld, %3$.0f%% しました。Est %4$lum %5$02lus to completion.\n"
$ Msg 875 should not translate "fseek" or "d99."
875 "\n%1$s オフセット %2$ld に d99 ファイルをfseekできません。\n"
1068 "%1$s 新しい逆索引ファイル '%2$s' が開けません。: %3$s\n"
1083 "%1$s 入力ファイル '%2$s' が読み取れません。: %3$s\n"
1097 "%1$s 言語ファイルの読み込みエラーのため取り消します。\n"
1108 "%1$s: Pass 1 の初めに、'%2$s' からレコードを読み取ります。\n\
   Each dot = %3$d レコード.\n"
$ Msg 1129 should not translate ".fzk."
1129 "%1$s: %2$s 無効な .fzk フォーマットです。\n"
1168 "%1$s: %2$s '%3$s' を捨てます。キーはデータベースにありません。\n"
1190 "\n%1$s: レコード数 %2$lu, %3$.0f%% done.  Est %4$lum %5$02lus to end Pass 1.\n"
1225 "%1$s: Pass 1 %2$lum %3$lusで完成し、%4$lu レコード読み込みました。\n\
  %5$s, parsed %6$lu 単語です。\n"
1233 "%1$s: Pass 2 初め: バッチ索引トラバーサルとデータベース更新。\n\
  Each dot = %2$d 単語\n"
1246 "%1$s: Pass 2 %2$lum %3$lusで完成し、%4$lu 単語を更新しました。\n"
1402 "%1$s: 重複したレコード数 %2$lu '%3$s' を捨てます。\n"


$ ****************************************************
$ set 15 formerly set MS_delsfx, used in lang.c.
$set 15

$ Msg 4 should not translate "dblk."  It is a structure name.
4 "%s dblk は初期化されていません。\n"
$ Msg 5 should not translate "parg."
5 "%s プログラム・エラー : parg はファイルも文字列も含みません。\n"

8 "\n\
%1$s '%2$.*s...' はデータベース '%3$s' の\n\
最大単語サイズより大きいです。"

10 "%s プログラム・エラー : 未知のパーサ状態です。\n"
12 "%1$s '%2$.120s' はデータベース '%3$s' で有効な単語ではありません。"

17 "\n\
%1$s '%2$s' はデータベース '%3$s' の\n\
最小単語サイズより小さいです。"

19 "%1$s 単語 '%2$s' はデータベース '%3$s' 中で索引に載せられていません。"
20 "%1$s プログラム・エラー: 未知の jstate %2$d です。\n"
21 "%1$s '%2$.*s' は有効な日本語ではありません。"
23 "%1$s 単語 '%2$s' %3$ld 行めのは短過ぎます。"
24 "%1$s 単語ファイル '%2$s' に日本語の単語がありません。"
25 "%1$s データベース '%2$s' はパスがながすぎます: '%3$s'。"
31 "%s Stemmer 接頭辞ファイルは読み込まれません。\n"
$ Msg 41 should not translate "Paice."
41 "%1$s %2$s: 無効な Paice ルールは '%3$s' で始まります。"
$ Msg 42 should not translate "Paice."
42 "%1$s %2$s: Paice ルールは '%3$s' で始まるには具合が悪いです。"
45 "%s メモリ不足です。\n"
50 "英語-ASCII"
$ Msg 51 refers to the Latin 1 codeset.
51 "英語-Latin1"
52 "スペイン語"
53 "フランス語"
54 "イタリア語"
55 "ドイツ語"
56 "日本語-comp"
57 "日本語-.knj"


$ ****************************************************
$ set 16 formerly set MS_vewords, used in boolpars.c and boolsrch.c.
$set 16

$ Msg 2 is like "<program name> Part of ... term <word> which is
$ not in database <database name>."
2 "\n\
%1$s 語の配置の部分の操作は\n\
データベース '%3$s' 中にない\n\
ターミナル '%2$s' を含みます。"
$ Msg 6, variable 3 is the database name.
6 "\n\
%1$s データベース・エラー。単語 '%2$s' は\n\
データベース '%3$s' 中にリストされています。しかし索引レコードにはありません。"
$ Msg , variable 3 is the database name.
11 "%1$s 単語 '%2$s' は データベース '%3$s' 中に見つかりません。"
$ Msg 14 should not translate "WHITLIM."  The message is like
$ "<Program name> <word in query> has more than <some number> hits."
14 "\n\
%1$s '%2$s' は %3$ld より多いです。\n\
それをクエリーまたは検索エンジン構成ファイルの WHITLIM 値\n\
をたてたものから削除してください。"
15 "\n\
%s このクエリー要求はクエリー単語を持たない\n\
データベース中のすべてのドキュメントを回復します。\n\
この検索のタイプはたいてい長い時間が掛かります。"

$ Msg 28 should not translate "d99."  Variable 2 is the database name.
28 "%1$s Database Read Error in %2$s.d99."
$ Msg 32 should not translate "d99."  The message is formed using either
$ msg 33 or 34 as variable 2, and it is like "<program name> Database
$ Error: Word <word> in database <database name> has invalid d99 record
$ number <number>."
32 "\n\
%1$s データベース・エラー: %2$s '%3$s'\n\
データベース'%4$s'中で d99 レコード数 %5$ld に無効があります。"
$ Msg 33 is used for variable 2 of message 32.
33 "単語"
$ Msg 34 is used for variable 2 of message 32.
34 "語幹"
$ Msg 35 should not translate "stemct," and "pmsz."
35 "%1$s プログラム・エラー: stemct=%2$d pmsz=%3$d\n"

$ ****************************************************
$ set 17 formerly set MS_vestatis
$set 17

104 "共有メモリが分離できません。"
107 "Qsort スタックはオーバーフローしました。\n"
110 "共有メモリを消せません。"
115 "共有メモリを利用できません。"
310 "メモリ範囲外です。%1$ld バイト必要です。\n"
314 "メモリ範囲外です。\n"
421 "総適合数 = %1$d. 最大指定数以上の適合を切り捨てます。"
$ Msg 437 should not be translated.
437 "db_status = %1$d, dba = %2$d:%3$ld (x'%4$08.8lx'), vistano = %5$d.\n"
504 "cur_dittoを割り当てられません。"
677 "クエリー・フィールドはありません。"
990 "システムは現在ソート中です。もうしばらくお待ち下さい。"


$ ****************************************************
$ set 23 formerly set MS_dbrec

$ Set 23 has several words in parenthesis which should not be translated.
$ For example, msg 3: "objkey,"  msg 5: "fzkeysz," msg 12: "abstrsz," 
$ msg 24: "minwordsz," msg 26: "maxwordsz," ...  The single words in
$ parenthesis should not be translated.
$ Msgs like 10, where the entire text is in parentheses should be translated.

$set 23

1 "---------- データベースのシステム値 '%1$s' ----------\n"
2 "スキーマ・バージョン・ナンバー (バージョン) は '%1$s'です。\n"
$ Msg 3 should not translate "sizeof" or "objkey."
3 "オブジェクト・キー・サイズの最大値(sizeof(objkey)) は %1$ld バイトです。\n"
$ Msg 4 "Huge" means very, very large.
4 "オプション 'Huge' キーは使用可能です。\n"
5 "意味カテゴリ(fzkeysz)の最大数は %1$d です。\n"
10 "  (オブジェクトへの意味アクセスは不可能です。)"
12 "抽象的な文字列の最長値 (abstrsz) は %1$d です。\n"
14 "  (抽象名詞はこのデータベースでは使われません。)"
20 "抽象は %1$s 圧縮されています。\n"
22 "言語を %1$d 個に解剖します。%2$s\n"
24 "単語の最短値 (minwordsz) は %1$d です。\n"
26 "単語の最長値 (maxwordsz) は %1$d です。\n"
30 "オブジェクト毎の .d00 slots の数 (recslots) は %1$d です。\n"
36 "  (データベース・オブジェクトの最大数は %1$ldです。)\n"
40 "Huffman 圧縮テーブルid (hufid) は %1$ld です。\n"
42 "  (圧縮はこのデータベースでは使用できません。)"
44 "  (指定した圧縮テーブルはまだ決定していません。)"
50 "エンジンはオブジェクトからオブジェクトへ変更するデータを手に入れやすい。"
54 "データ・オブジェクトはエンジンからすぐには手に入りません。"
56 "データ・オブジェクトは圧縮されたテキストとして内部に蓄積されます。"
60 "オブジェクトを参照するサーバ・ファイルのみが圧縮されたテキストに蓄積されます。"
64 "オブジェクトを参照するクライアント・ファイルのみが圧縮されたテキストに蓄積されます。"
68 "オブジェクト・キーは、オブジェクトを参照するサーバ・ファイルです。"
72 "オブジェクト・キーは、オブジェクトを参照するクライアント・ファイルです。"
74 "オブジェクトを参照するサーバ・ファイルは 'huge' キーに蓄積されます。."
80 "オブジェクトを参照するサーバ・ファイルは 抽象的に蓄積されます。."
86 "オブジェクトを参照するクライアント・ファイルは 抽象的に蓄積されます。."
90 "エラー: or_dbaccess値(%1$hd) の意味はわかりません。\n"
100 "圧縮されたテキストの倉庫は %1$s 圧縮されています。\n"
110 "圧縮されたテキストの倉庫はこのデータベースでは使用されません。"
120 "データベース・スイッチ (dbflags) は 0x%1$lx:です。\n"
$ Msgs 124 and 125 are used in the variable in messages 130 and 140.
$ For example: "Inverted index INCLUDES words exactly as parsed."
124 "INCLUDES"
125 "EXCLUDES"
$ Msgs 126 and 127 are used in the variable in messages 160, 162, 164.
$ For example, "Use of optional 'huge' keys is ENABLED."
126 "ENABLED"
127 "DISABLED"
$ Msg 130 uses msgs 124 and 125 in the variable.
130 "  逆索引 %1$s 単語は正確にパースのようです。\n"
$ Msg 140 uses msgs 124 and 125 in the variable.
140 "  逆索引 %1$s 単語は語幹です。.\n"
$ Msg 160 uses msgs 126 and 127 in the variable.
160 "  オプショナル 'huge' キーの使用は %1$s です。\n"
$ Msg 162 uses msgs 126 and 127 in the variable.
162 "  Mark-for-deletion は %1$s です。n"
$ Msg 164 uses msgs 126 and 127 in the variable.
164 "  加えられユーザに示される %1$s です。\n"
$ Msg 170 is followed by either msg 172 or msg 174.
$ For example, "Text characters are a SINGLE byte wide."
170 "  テキストの文字列は %1$s です。\n"
172 "2バイト"
174 "1バイト"
200 "現在のデータベース・オブジェクト数 (reccount) は %1$ld です。\n"
210 "最後のスロット数(maxdba)の現用は %1$ld です。\n"
310 "\n\
使用方法: %1$s <dbname>\n"
330 "'%1$s' データベースを開けません。\n%2$s\n"
340 "データベース '%1$s' に dbrec レコードがありません。\n"
350 "データベース '%1$s' の dbrec レコードが読み込めません。\n%2$s\n"

$ ****************************************************
$ set MS_dtsrkdump is set 25.  5/20/96.  dtsrkdump.c
$set 25

1 "%s プログラム・エラー取り消し。\07\n"
$ Msg 2 should not be translated
2 "%1$c dba=%2$d:%3$-7ld ofs=%4$-9ld adr=%5$-6ld fre=%6$ld\n"
$ Msg 3 is like "<program name> <RCS revision number>, engine
$ <DtSearch version>.  <time stamp>."
3 "%1$s %2$s, エンジン %3$s.  %4$s.\n"
$ Msg 4, option "-p" can include decimal points -- this is handled by
$ using sscanf, so if sscanf can read numbers with commas in them, then
$ the "-p" option will process that correctly.  atof is used on what
$ sscanf reads in.
4 "\n\
使用方法: %1$s -o|w|ow [-v] [-t# | -p#] <dbname>\n\
          DtSearch キーファイルを読み取り、レポート要約を印刷します。\n\
  -o        キーの調査は OBJECT レコードキーです。\n\
  -w        キーの調査は 逆索引 WORDS です。\n\
  -v        VERBOSE モード、 すべてのキーをリストします。\n\
  -t<N>     しきい値。w と v オプションを設定し、単語のみをリストします。\n\
            with >= # アドレス。  n = 1 ならばすべての単語をリストします。\n\
  -p<N>     別のしきい値。#を除いて -t と同じ全データベースのパーセントです。\n\
            (# は小数点を含むでしょう。)\n\
            例えば -p99.9 は少くともそのレコードの99.9%%に起こる\n\
            すべての単語を出力します。 -- ストップリストのための候補を見つける\n\
            優れた方法です。\n\
            もし w と v がしきい値なしで設定されていたならば、デフォルトは -t%2$d です。\n\
  <dbname>  1 - 8 文字のオプショナルパス接頭辞とともにデータベース名です。\n"
5 "%1$s  パーセント値 %2$lf は無効です。\07\n"
53 "%1$s しきい値は無効です。\07\n"
55 "%1$s 未知のコマンドライン引き数です。'%2$c'\07\n"
56 "%1$s データベース名の要求がありません。\07\n"
57 "%1$s -o または -w のどちらも 指定されなければなりません。\07\n"
58 "%1$s データベース名 '%2$s' は無効です。\07\n"
$ Msg 60 should not be translated.
60 "%1$s: '%2$s' reccount=%3$ld maxdba=%4$ld recslots=%5$hd minw=%6$hd maxw=%7$hd\n"
70 "%1$s %2$ld、少くともレコード中にある単語をリストするのみでしょう。\n"
80 "%1$s: データベース中のすべての単語をリストします。\n"
$ Msg 100 should not be translated.
100 "dba x%1$08lx, %2$6ld\n"
$ Msg 110 is like "in <directory name>"
110 " %1$s 中"
120 "'%1$s' のオブジェクト要約です。%2$s:\n"
130 "キータイプによるオブジェクト数:"
160 "総オブジェクト数 = %1$ld\n"
$ Msg 170 should not translate "DBA."
170 "最大オブジェクト DBA  = %1$ld\n"
180 "%1$s: * アスタリスクでマークした単語はすべてのレコードにあります。\n"
200 "'%1$s' の単語要です。%2$s:\n"
210 "総単語数 = %1$ld\n"

$ ****************************************************
$ set MS_dtsrclean is set 26.  5/21/96.  dtsrclean.c
$set 26

1 "\n\
使用方法: %1$s [options] <dbname> <newpath>\n\
       d99 スペースを使わず、d00-d99 リンクを有効とし、圧縮します。\n\
  -p<N>     進捗ドットをすべての <N> レコードにプリントします(デフォルト %lu)。\n\
            完了進捗メッセージがすべての %d ドットにプリントされます。\n\
  -oy       d99/d2<N> ファイルが新しいパスに現われる以前のオーバレイを認めます。\n\
  -on       d99/d2<N> ファイルが新しいパスに現われる以前ならば終了させます。\n\
  -v        d99 と d00 リンクを有効とし、d99 ファイルを変造せず、正確な\n\
            レコード数を保証します。-c0 もまた完全なデータベースを変造しません。\n\
  -v<fname> -v と同じですが、.fzk ファイル・フォーマットに引き出されるに\n\
            ふさわしいフォーマット中の fname に d99 によって参照されない\n\
            すべての d00 レコードを書きもします。\n\
  -c<N>     <N> 変造/不完了リンク以上ならば存在します (デフォルト %d)。\n\
            変造制限は -c0 によって止まります。\n\
  <dbname>  1 - 8 文字のデータベース名 = 古い d99/d2<N> ファイルは更新されます。\n\
            ファイルはローカル・ディレクトリで見つかります。\n\
  <newpath> 新しい d99/d2<N> ファイルのある場所を指定します。\n\
            一文字めがスラッシュでないならば、パスはローカル・ディレクトリに関係します。\n\
終了コード:\n\
  0: 正常終了。  1: 警告。  2: ジョブが開始していません。\n\
  3-49: ジョブが早まって終了し、古いファイルは間違いなく、新しいファイルは使われていません。\n\
  50-99: 致命的エラー、 古いデータベースは変造されているかもしれません。\n\
  100+: Ctrl-C, kill, and all other signal interrupts cause premature\n\
     終了、新しいファイルは使われていないかもしれません。 シグナル = 終了コード  - 100.\n"
2 "%1$s: %2$s 圧縮 %3$d%% (約 %4$lu KB) %5$ld:%6$02ld 分:秒.\n"
$ Msg 3 should not translate "d99."  "WORDS" should be translated.
3 "%1$s: 総数 %2$ld WORDS %3$s.d99 中で。\n"
4 "%1$s: 見つけたリンクは変造されていません。\n"
5 "%1$s: 終了コード = %2$d.\n"
$ Msg 6 should not be translated.
6 "  DBA = %1$d:%2$ld (x%3$02x:%4$06lx),  orig addr val = x%5$08lx\n\
  Word='%6$c%7$s' offset=%8$ld addrs=%9$ld free=%10$d\n"
7 "%1$s: 古い d2<N> ファイルから %2$s にコピー中です...\n"
$ Msg 11 is like "<program name> Version <version number>. Run <date program
$ was run>."
11 "%1$s バージョン %2$s.  実行 %3$s.\n"
$ Msg 14 should not translate "d99."
14 "%1%s: 古いファイル: '%2$s.d2x, .d99'.\n"
$ Msg 15 should not translate "d99."
15 "%1$s: 新しいファイル: '%2$s.d2x, .d99'.\n"
24 "%1$s: 1つかそれ以上の新しいファイルがすでに存在します。\n"
$ Msg 25 should not be translated.
25 "%1$s: curr reccnt=%2$ld, mxdba=%3$ld, sl/rec=%4$ld, tot#adr=%5$ld.\n"
26 "%1$s: d00 ファイルを読み込み中です。 それぞれのドットはおおよそ %2$ld データベース・ドキュメントです...\n"
$ Msg 27 should translate DOCUMENTS.
27 "%1$s: %2$ld DOCUMENTS %3$s.d00 で確かめます。\n"
$ Msg 28 should translate DOCUMENTS.
28 "%1$s: %2$ld DOCUMENTS 実際に %3$s.d00 ではない = %4$ld そこに蓄えた数。\n\
  新しい d00 ファイルに訂正された数を数えます。\n"
29 "%1$s: %2$sに圧縮中です。  それぞれのドットはおおよそ %3$lu 単語です...\n"
$ Msg 45 needs the "y" for "yes" to match msg 46.
45 "    新しいディレクトリ中のオーバレイ・ファイルですか? [y/n] "
$ Msg 46 must match the "yes" option from msg 45.
46 "y"
66 "%1$s 中断シグナル %2$d の後中止しました。\n"
$ Msg 111 should not translate "d99."
111 "*** %1$s DBA in d99 = -1.  d2x ファイルで間違った値のため\n\
  拡張領域を超えたらしいです。\n"
159 "%1$s 未知の引数です。: '%2$s'.\n"
177 "%1$s 無効な %2$.2s 引数です。\n"
193 "%1$s 変造されたリンク  %2$ld で中止します。\n"
194 "%1$s %2$s %3$ld は変造/不完了リンクを見つけました。\n"
210 "%1$s dbname 引数の要求がありません。\n"
211 "%1$s newpath 引数の要求がありません。\n"
212 "%1$s 引数が多過ぎます。\n"
$ Msg 222 should not translate "d99" or "DBA."
222 "*** %1$s DBA in d99 not in d00, スロット > ドキュメントの最大数\n"
229 "%1$s 無効なデータベース名です '%2$s'。\n"
301 "%1$s -m 引数はもはや必要ではありません。\n"
$ Msg 333 should not translate "d99" or "d00."
333 "*** %1$s DBA in d99 d00 に存在しません。\n"
393 "%1$s 古いのと新しいディレクトリは全く同じです: '%2$s'。\n"
439 "%1$s %2$sを開けません。: %3$s\n"
440 "%1$s %2$sのステータスにアクセスできません。: %3$s\n"
$ Msg 444 should not translate "d99."  Do not translate starting with "DBA = ..."
444 "*** %1$s d00 レコード '%2$s' は d99 を参照しません。\n\
  DBA = %3$d:%4$ld (x%5$02x:%6$06lx).\n"
463 "%1$s コマンド・ライン引数はファイルのオーバレイを許しません。\n"
$ Msg 465 "Validate" should match the description for the "-v" option in msg 1.
465 "%1$s 警告: ビット・ベクトルのメモリをアロケートできません。\n\
  'Validate' モードはスイッチを切りました。\n"
489 "%1$s %2$s で書き込みエラーです。: %3$s.\n"
499 "%1$s %2$s は空です。\n"
517 "%1$s %2$s で読み込みエラーです。: %3$s.\n"
$ Msg 561 should not translate "DBA," "maxdba," or "dbrec."
561 "%1$s DBA '%2$d:%3$ld' (レコード数 %4$ld) d00 でアドレスの総数を超えました。;\n\
  dbrec 中の maxdba %6$ld が間違っているため、ビット・ベクトルがオーバフローしました。\n"
$ Msg 657 is like "<program name> Read error on <filename>: <system errno>."
657 "%1$s %2$s で読み込みエラーです。: %3$s.\n"
$ Msg 665 is like "<program name> Write error on <filename>: <system errno>."
665 "%1$s %2$s で書き込みエラーです。: %3$s.\n"
$ Msg 802 is like "<program name> Unable to open <filename> to output
$ unreferenced d00 records: <system errno>."  Do not translate "d00."
802 "%1$s d00 レコードを参照しない出力に '%2$s' を開けません。:\n  %3$s\n"

$ ****************************************************
$ set MS_dui is set 27.  5/21/96.  dui.c
$set 27
1 "%1$s DtSearch バージョン %2$s.\n\
USAGE: %3$s [-options] <query>...\n\
       オプションは相互に排他的な同じ文字で始まります。\n\
  -ss         検索タイプ S (単語の語幹) はデフォルトの検索の統計の代わりです。\n\
  -sw         検索タイプ W (正確な単語) はデフォルトの検索の統計の代わりです。\n\
  -pa         摘要を印刷 (デフォルト)。\n\
  -pk         レコード・キーを印刷。\n\
  -pb         摘要とレコード・キーの両方を印刷。\n\
  -p<N>       hitlistから <N> 番めのレコードを印刷し、回復します。\n\
  -k<C>       <C> 文字で開始するキーの制限検索。\n\
  -d<dbname>  1 - 8 文字のデータベース名。デフォルトは\n\
              構成ファイル側で最初のデータベースです。\n\
  -m<N>       戻された一致の最大数。デフォルトは %4$d。\n\
  <query>     コマンド・ラインの思い出させるもので検索単語の設定。\n\
              クエリーはブール演算子を含むクォーテーションで囲まれるかもしれません。\n"
4 "'%1$s' データベースを検索中です...\n"
9 "<使用可能な摘要はありません。>"
117 "%1$s 無効なコマンド・ライン引数です '%2$s'。\07\n"
125 "%1$s クエリーの指定がありません。\07\n"
165 "%1$s DtSearch 初期化に失敗しました。:\n  %2$s\n"
190 "%1$s 一致が見つかりませんでした。\n"
195 "%1$s 検索が失敗しました。\07\n"
200 "%1$s テキストはドキュメント '%2$s' を利用できません。\n"
203 "%1$s 利用できるキータイプを決定できません。  %2$s\n"
224 "%1$s '%2$c' で始まるキーのレコードがデータベース '%3$s' 中にありません。\n"
247 "%1$s ----- 初期化後のエンジン・メッセージ -----\n%2$s\n"
248 "%1$s ----- エンジン初期化メッセージの終わり -----\n"

$ ****************************************************
$ set MS_boolpars is set 28.   5/22/96.
$set 28

1 "\n\
%1$s この検索文字列は無効なブール・クエリーです。\n\
修正して、再試行してください。"

2 "%1$s クエリー・フィールドは空です。"

$ Msg 3 should not translate "@".
3 "\n\
%1$s 並置操作は (@) \n\
2つの単語野間に位置されるのみかもしれない。"

$ Msg 4 variable 2 is <database name>.
4 "\n\
%1$s 1つかそれ以上の単語がOne or more words in the\n\
クエリー中でデータベース '%2$s' に蓄えられません。"

$ Msg 5 "AND" should be the local equivalent to the Boolean Operator
$ for "AND."  "&" should not be translated.
5 "\n\
%1$s ブール演算子は単語または語句の間に\n\
位置付けられなければなりません。操作以外に\n\
2つの引き続いて起こる単語は AND 操作 (&) によって\n\
分けられるように解釈されます。"

6 "%1$s 丸括孤中の語句はありません。"

$ Msg 5 "NOT" should be the local equivalent to the Boolean Operator
$ for "NOT."  "~" should not be translated.
7 "\n\
%1$s NOT 操作 (~) は単語または語句の左に\n\
位置付けられ、修飾します。\n"

8 "%1$s 無効なブール・クエリーです。シンタクス・エラー数 %2$d"

9 "%1$s ブール・クエリーに端末が多過ぎます。"

$ Msg 10 variable 2 is <database name>.
10 "%1$s 並置検索はデータベース '%2$s' を利用できません。"

$ Msg 11 should not translate "@."
11 "\n\
%1$s 並置操作 '%2$.*s' は無効です。\n\
正確なフォーマットは '@<N>' <N> は0より大きいところです。"

$ Msg 12 should not translate "@."
12 "\n\
%1$s 並置操作 (@) は全く同じ\n\
単語の間では許されません。"

13 "%1$s 単語 '%2$.*s' は無効です。"

14 "%1$s 文字列 '%2$.*s' は単一単語ではありません。"

15 "\n\
%1$s このクエリーは論理上どんなレコードも\n\
戻せません。修正して再試行してください。"

16 "\n\
%1$s このクエリーは完全なデータベース '%2$s' を戻します。\n\
修正して再試行してください。"


$ ****************************************************
$ set 29 symbolically named MS_tomita, used in tomita.c (dtsrdelete)
$set 29

1 "\n\
\n%1$s 割り込み %2$d を受け取りました。\n\
  プログラムは現在のバッチの削除の後中止します。\n"

3 "%1$s: 終了コード = %2$d.\n"
4 "%1$s データベース '%2$s' が見つかりません。\n"
5 "利用できる選択は:"
8 "%1$s 無効な入力フォーマットです。: %2$.30s...\n"
$ Msg 24 is like <Program name> Database <dbname>, <record id> not found."
24 "%1$s データベース %2$s, '%3$s' は見つかりません。\n"

25 "\n\
DO NOT CONTINUE 以下の状況の元では:\n\
-> 削除されるレコード id のリストの入力ファイルが '%1$s' と\n\
      名づけられている場合。\n\
-> ユーザがデータベースに影響を与えるアクセスをまだ行なっている場合。\n\
-> データベース・ファイルがバックアップされる場合。\n\
削除し始める準備が確かならば、'y' を入力してください... "

$ Msg 26 is "<program name> <number of files> read, <number> deleted,
$ <time> elapsed. Database <dbname>: Current record count = <number>,
$ Batch size = <number of records>."
26 "%1$s %2$ld 読み、 %3$ld 削除、 %4$ldh %5$2ldm %6$2lds 経過しました。\n\
  データベース '%7$s': 現在のレコード数 = %8$ld, バッチ・サイズ = %9$d.\n"
$ Msg 27 is "<program name> <number of records> records read from input file.
$ <number> were deleted and <number> were not found in <time>." 
27 "%1$s 入力ファイルから %2$ld レコード読み込みました。 %3$ld は削除され、\n\
  %4$ld は %5$ld 時 %6$ld 分 %7$ld 秒で見つかりませんでした。\n"

28 "  または レコードあたり平均 %1$ld 分 %2$ld 秒 削除しました。\n"
$ Msg 29 is "<program name>. Run <time stamp>."
29 "%1$s.  実行 %2$s.\n"

$ Msg 30 should not translate "-y".
30 "\n\
使用方法: %1$s [options]\n\
  -i    ファイル名入力。指定されていなければ、%2$s をデフォルトとします。\n\
  -d[v] デバッグ文をプリントします。\n\
        -dv 冗長な (record-by-record) デバッグをつけます。\n\
  -t<N> 実行時間の最大要求秒数。\n\
        Ctrl-C/Break は次のレコードで削除を中止もするでしょう。\n\
  -n<N> %5$d から <N> にバッチ中のレコード野変更数。\n\
  -y    自動的に削除モードの確認プロンプトに 'yes' と答えます。\n\
  -d    削除操作をトレースします。\n"

31 "%1$s 未知の引数です。'%2$s'.\n"
32 "%1$s 入力ファイル '%2$s' を開けません。\n"
33 "\n%1$s 出力ファイル '%2$s' を開けません。\n"
34 "検索エンジン初期化中...\n"
36 "通常エンジンをシャットダウン\n"


$ ****************************************************
$ set 30 named MS_huff has messages for hencode.c, hdecode.c, huffcode.c
$set 30

$ Msg 1 variable 2 is <filename>, variable 3 is <system errno>.
1 "%1$s: huffman encode ファイルを開けません。'%2$s': '%3$s' 終了コード = 2.\n"
2 "\n\
使用方法: %1$s <filename>\n"
3 "%1$s 入力ファイル '%2$s を開けません。\n"
4 "%1$s 総文字数 = %2$ld.\n"

10 "%1$s メモリ不足です。\n"
$ Msg 11 variable 2 is <tree file name>, variable 3 is <system errno>.
11 "%1$s ツリー・ファイル '%2$s' を開けません。: %3$s.\n"
$ Msg 12 variable 2 is <tree file name>.  Do not translate "hctree."
12 "%1$s 無効なフォーマットです。hctree '%2$s'.\n"
$ Msg 13 should not translate "hctree_ids."
13 "%1$s hctree_ids に矛盾します。\n"
14 "%1$s cypherfile '%2$s' を開けません。\n"
$ Msg 15 should not translate "EOF."
15 "%1$s 予期しない EOF '%2$s'です。\n"

30 "183 ビット文字列が大きくなり過ぎました。多分、文字どおり\n\
文字数をひどくアンバランスにし、消しました。"
31 "191 プログラミング・エラー:  ルートを作成した後で\n\
Huffman コード・ツリーの構築を試みます。\n"
32 "(nonascii character, high bit set)"
33 "366 %1$s で無効なファイルフォーマットです。\n"
34 "424 ファイル '%1$s'は書き込みのために開くのに失敗しました。読み込み専用ではないことを確かめます。\n"
35 "\n\
使用方法: huffcode [-l<N> | -l-] [-o] <huffname> [<infile>]\n\
  -l<N>      '文字'の初めの数を指定します。<= <N> 回発生するどんな文字も \n\
             文字でコード化されるでしょう。デフォルトは -l0 で、n\
             ゼロの数の bytes のみ文字コーディングです。\n\
  -l-        文字コーディングを消します。EXTREMELY LONG ビット文字コードに\n\
             導かれるバランスの取れないツリー中の文字コーディングを消します。\n\
             --よくバランスの取れたバイナリファイルとして知られる入力の他は\n\
             しません。\n\
  -o         現在のデコード・ファイルの存在の上書きを前もって認めます。\n\
  <huffname> Huffman コード・ファイルのファイル名の接頭辞です。\n\
             エンコード・ファイル (%1$s) が既に存在するならば、それに\n\
             加えられる infile からバイト数を別な方法で新たに\n\
             作成されるでしょう。\n\
             デコード・ファイル (%2$s) は実行時毎にいつも新たに作成されます。\n\
  <infile>   数えられるバイトを含む入力ファイルです。\n\
             エンコード・ファイルがすでに存在するならば省略されるかもしれません。\n"
36 "'%1$s' は無効な引数です。\n"
37 "576 Huffman コード・ファイル名接頭辞がありません。\n"
$ Msg 38 needs to have "y" translated to match msg 39"
38 "デコードファイル '%s' は既に存在します。上書きしますか? [y/n]\n"
$ Msg 39 needs "y" to match the "yes" option in msg 38.
39 "y"
40 "HUFFCODE バージョン %1$s\n"
41 "Huffman コード・テーブルは新たに作成されるでしょう。\n"
42 "テーブル '%1$s' は既に以前の実行から %2$ld Kbytes 含みます。\n"
43 "645 入力ファイルは指定されていません。そして '%sテーブル・ファイルは\n\
存在しません。-- 何もしません。\n"
44 "入力ファイル '%1$s' ｌ・を開けないか、ステータス にアクセスにできません。: %2$s\n"
45 "入力ファイル '%1$s' は約 %2$ld Kbytes 含みます。\n"
46 "\n%1$ld%% 終了。 %2$2ld Kbytes 読み込みました。概算で %3$3ld 秒で終了しました。\n"
$ Msg 47 variable 2 is a time stamp, variable 2 is from either msg 48 or 49,
$ variables3 and 4 are filenames.
47 "タイムスタンプ特性は '%1$ld' になるでしょう。\n\
%2$s Huffman コード・テーブル '%3$s' と '%4$s'中の...\n"
$ Msg 48 is used in msg 47 as variable 2.
48 "作成"
$ Msg 49 is used in msg 47 as variable 2.
49 "再構築"

$ ********************** DTSEARCH.MSG ************************
