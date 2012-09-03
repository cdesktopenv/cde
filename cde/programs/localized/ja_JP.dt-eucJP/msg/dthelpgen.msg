$ $XConsortium: dthelpgen.msg /main/4 1996/10/16 03:15:11 cde-hit $
$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
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
$ 
$set 1
$ 
$ _DtMessage 1 is the usage message
$ 
$ Localize only the words 'directory', 'name', and 'language'.
$ 'directory' means the directory name in which the resulting files
$ will be placed. 'name' mean what base name to give the files placed
$ in 'directory'. 'language' means which localized versions of help files
$ to look for.
$ 
1 %s -dir <ディレクトリ> [-generate] [-file <ファイル名>] [-lang <言語>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: %s の要素はディレクトリではありません。\n
3 %s: ディレクトリ %s へのアクセスが拒否されました。\n\
スーパーユーザで試行しますか?\n
4 %s: %s の要素が存在しません。\n
5 %s: %s があるファイル・システムがいっぱいです。\n
6 %s: %s にアクセスできません。エラー・ステータス番号 %d\n
7 %s: %s があるファイル・システムは読み取り専用です。\n
8 %s: %s に書き込むには、root のアクセス権が必要です。\n
9 %s: %s への書き込みは無効です。\n
10 %s: 検索パスが空です。\n
11 %s: 'title' リソースがありません。\n
12 %s: 'abstract' リソースがありません。\n
13 %s: 'volumes' リソースがありません。\n
14 %s: 'character' 設定リソースがありません。\n
15 %s: 出力先ディレクトリがありません。\n
16 %s: ゼロ・ファミリ・ファイルがあります。\n
17 %s: ゼロ・ボリューム・ファイルがあります。\n
18 %s: 現在の作業ディレクトリにアクセスできません。エラー・ステータス番号 %d\n
19 %s: メモリを割り当てられません。\n
20 %s: 指定の %s システム言語は無効です。\n

$ 
$set 2
$ 
$ Specifies the character set used to create this file
$ 
$ This must be localized for the various languages. That is
$ for Japanese shift JIS, it would be 'ja_JP.SJIS'; for Japanese 
$ EUC it would be 'ja_JP.eucJP'. For files written using
$ HP Roman8 character set it would be '<lang+terr>.HP-ROMAN8', etc.
$ The set of allowable locale strings can be found in
$ /usr/dt/config/svc/CDE.lcx
$ 
1 ja_JP.HI-SJIS

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 ヘルプ・マネージャの使用法

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>ヘルプ・マネージャの使用法</ABBREV>\
<PARAGRAPH>以下の各タイトルは、インストールされ、オンライン・ヘルプに\n\
登録されている<ANGLE italic>プロダクト・ファミリ</>を示しています。\n\
各タイトル(およびアイコン)は、ファミリ内のヘルプをリストで表示する\n\
ハイパーリンクです。</>\
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">\
プロダクト・ファミリで使用可能なヘルプのリストを表示するには、タイトル\n\
(下線付きのテキスト)またはアイコンを選択してください。</PARAGRAPH>\
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">\
プロダクト・ファミリ内で、表示したいヘルプを検索して、そのタイトルを\n\
選択してください。</PARAGRAPH>\
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">\
ヘルプ・ウィンドウを使用中にヘルプが必要な場合は、F1 キーを押してください。</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 ヘルプ - トップレベル

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>ヘルプ・マネージャの使用法</ABBREV>\
<LINK 0 "Help4Help How-To-Register-Help">\
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>\
注意:\ \ \ ヘルプは登録されていません。\
</SIZE></WEIGHT></TYPE></></LINK>\
<PARAGRAPH leftindent 3 firstindent 3>\
<WEIGHT bold>プロダクト・ファミリには登録済みのブラウズ用オンライン・\n\
ヘルプはありません。</>ヘルプは、アプリケーション内でヘルプ・コマンドを\n\
直接選択することによって、そのアプリケーションで使用可能になります。</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 ブラウザ情報を作成中です。しばらくお待ちください。

$ 
$set 3
$ 
$ ******* Text Formatting Templates ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ 
1 <TOPIC charset %s>
2 <TITLE><TYPE serif><WEIGHT bold><SIZE 14>%s</SIZE></WEIGHT></TYPE></TITLE>
3 <PARAGRAPH before 1 first 1 left 1>
4 <CHARACTERSET %s>
5 <TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>%s</></></></>
6 <PARAGRAPH before 1 first 1 left 1 graphic %s glink %s gtypelink 0>
7 <PARAGRAPH before 1 first 1 left 1 graphic %s glink "%s %s" gtypelink 1>


