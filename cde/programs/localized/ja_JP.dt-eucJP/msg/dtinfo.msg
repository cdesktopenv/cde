
$quote "
$ $TOG: dtinfo.msg /main/25 1999/09/20 13:35:52 mgreess $
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

$set	1
$ 
$ messages specific to message-catalog manager
$ 
1 "カタログ中にデフォルトのメッセージが見つかりません。"


$set	2
$ 
$ messages specific to UAS/DtSearch modules
$ 
1 "検索機能 ``%s'' はサポートされていません。"
2 "quoted phrase"
3 "weight"
4 "proximity"
5 "xor"


$set	4
$ 
$ messages specific to booklist window
$ 
3 "Dtinfo: Infolibのブックリスト"
63 "Dtinfoへようこそ!"
65 "現在のセクション"


$set	5
$ 
$ messages specific to browser window
$ 
1 "Dtinfo: ブラウザ"
2 "リンク先をプレビューするには、リンクをクリックし、ボタンを押したままにしてください。"
3 "インフォメーション・ライブラリ"

$       4       "The locator for this section is %s"
$       This message is the result of the "Show Locator" operation.
$ "NEW-"
4 "このセクションのロケータは %s です。"

$ line-break rules (reserved)
5 "、。，．：；？！’”）〕］｝〉》」』】々ーぁぃぅぇぉっゃゅょゎァィゥェォッャュョヮヵヶ゛゜ヽヾゝゞ―‐°′″℃％"
6 "‘“（〔［｛〈《「『【￥＄¢£＠§〒♯"

$set	6
$ 
$ messages specific to add information library dialog
$ Note: "Infolib" is a CDE glossary term.
$ 

1 "Dtinfo: ドキュメント・ライブラリの追加"

$ 2     "Please enter an absolute path to the Infolib."
$       (Path must exist and include "/".)
2 "Infolibへの絶対パスを入力してください。"

$ 3     "Infolib open failed. Check the path specified."
$	(Path passed basic format check, but could not open target found.)
3 "Infolibのオープンに失敗しました。指定したパスを確認してください。"

$ 4     "Infolib open failed. Check entry syntax or Infolib integrity."
$       (An unidentified error occurred on attempt to open.)
4 "Infolibのオープンに失敗しました。エントリのシンタクスまたはInfolibが完全かどうか確認してください。"

$ 5     "Infolib specification format error."
5 "Infolibの仕様の書式エラー"

$ 6     "No Infolib specified."
6 "Infolibが指定されていません。"

$ 7     "No Infolib found or specified."
7 "Infolibが見つからないか、指定されていません。"

$ 10    "Dtinfo: Infolib Selection"
$       (For title to the File Selection Dialog)
$ "NEW-"
10 "Dtinfo: Infolibの選択"

$ 11    "No Infolibs Present"
$       (This string will be shown in a directory listing where there
$        are no Infolibs found in the directory.)
$ "NEW-"
11 "Infolibがありません。"

$set	7
$ 
$ dtinfo GUI label strings
$ 
1 "ファイル(F)"
2 "F"
4 "表示(V)"
5 "V"

$	6	"Clone"
$ "Clone" is applied to the selected entry in the booklist, and
$ has the effect of opening another (secondary) window with a
$ copy of the booklist hierarchy below the selection, and rooted
$ (based) at the selected item.
$ [Currently, this functionality is deferred beyond CDE 2.0.]
6 "Clone"
$	7	"N" for "N" in "Clone"
7 "N"

8 "印刷(P)"
9 "P"
10 "印刷(r)..."
11 "r"
12 "閉じる(C)"
13 "C"
14 "終了(x)"
15 "x"
16 "編集(E)"
17 "E"
18 "コピー(C)"
19 "C"
20 "追加(A)..."
21 "A"
22 "削除(R)"
23 "R"
24 "オプション(O)"
25 "O"
26 "設定の変更(P)..."
27 "P"
28 "自動トラック(A"
29 "A"
30 "ツール・バーの表示(T)"
31 "T"
32 "検索領域(S)"
33 "S"
34 "ウィンドウ(W)"
35 "W"
36 "グラフィカル・マップ(G)"
37 "G"
38 "検索ヒストリ(H)"
39 "H"
40 "マーク・リスト"
41 "M"

$	42	"Search History " presents a dialog listing
$ a history of prior search results, as provided in the Search
$ Results Dialog.
42 "検索ヒストリ(S)"
43 "S"
44 "クエリー・エディタ(Q)"
45 "Q"
46 "範囲エディタ(c)"
47 "c"
48 "ヘルプ(H)"
49 "H"
50 "`アイテムヘルプ(O)..."
51 "O"
59 "検索:"
61 "範囲エディタ"

$  Messages 62-77 for "Browser" pulldown menu entries
$ 
62 "ブラウザ(B)"
63 "B"
64 "新しいウィンドウ(N)"
65 "N"
66 "ロケータを開く(O)..."
67 "O"
68 "ロケータを表示(S)"
69 "S"
70 "印刷(P)"
71 "P"
72 "印刷(P)..."
73 "r"
74 "閉じる(C)"
75 "C"
76 "終了(x)"
77 "x"

$	78	"On Selection", where selection is a text
$ selection that will be used directly as a search query string
78 "選択テキスト(O)"
79 "O"

80 "クエリー・エディタ(Q)"
81 "Q"
82 "範囲エディタ(S)"
83 "S"
84 "検索ヒストリ(H)"
85 "H"
86 "検索結果の消去(C)"
87 "C"
88 "後 (ヒストリ)(B)"
89 "B"
90 "前 (ヒストリ)(F)"
91 "F"
92 "ヒストリ(H)..."
93 "H"
94 "前のセクション(P)"
95 "P"
96 "次のセクション(N)"
97 "N"

$   Messages 98-101 are for traversal of the document view in order
$ to present and highlight the Previous or Next "search hit" (if any)
$ in the section. Presumably, all search hits have already been
$ highlighted to some degree, and this will add the equivalent of a
$ keyboard traversal highlight to the appropriate one when brought
$ into view.
98 "前の検索(r)"
99 "r"
100 "次の検索(x)"
101 "x"

102 "検索(S)"
103 "S"

$	104	"Go" is a pulldown menu label, where menu items are
$ various methods by which the user may "go" to a specific document.
104 "行先(G)"
105 "G"

$   Message 106-119 are for the "Marks" pulldown menu
106 "マーク(M)"
107 "M"
108 "ブックマークの作成(B)"
109 "B"
110 "注記の作成(N)..."
111 "N"

$	112	"List Marks" means to show all saved Marks as a list
$ in a separate dialog window.
112 "マークの一覧表示(L)"
113 "L"

114 "選択マークの編集(E)..."
115 "E"
116 "選択マークの移動(M)"
117 "M"
118 "選択マークの削除(D)"
119 "D"

$   Messages 120-129 are for "Options" pulldown menu entries
120 "オプション(O)"
121 "O"
122 "設定の変更(P)..."
123 "P"

$	124	"Detach Graphic" removes a selected graphic from the
$ document flow and places it in its own window. A "placeholder"
$ remains in the document where the graphic originally appeared.
124 "グラフィックの分離(D)"
125 "D"

126 "ツール・バーの表示(T)"
127 "T"
128 "検索領域の表示(S)"
129 "S"

$   Messages 130-135 are for "Windows" pulldown menu entries
130 "ウィンドウ(W)"
131 "W"
132 "ブックリスト(B)"
133 "B"
134 "グラフィカル・マップ(G)"
135 "G"

$   Messages 136-142	are believed to be *unused*/obsolete
136 "所有者(A)"
137 "A"
138 "ロケータの取得"
139 "デバッグ(D)"
140 "D"
141 "にせのInfobaseリストを作成"
142 "コマンド・プロセッサ"

$   Messages 143-158 are Help pulldown menu entries, and
$ should of course read the same across all CDE applications,
$ within a localization. They always mean the same thing:
$ provide "help On []"
143 "ヘルプ(H)"
144 "H"
145 "アイテムヘルプ..."
146 "C"
147 "ウィンドウ(W)"
148 "W"
149 "アプリケーション(S)"
150 "A"
151 "索引(I)"
152 "I"
153 "ヘルプ(H)"
154 "H"
155 "キー(K)"
156 "K"
157 "バージョン(V)"
158 "V"

$	159	"Retain" is believed to be unused
159 "保存"

160 "Infolib パス:"
161 "了解"
162 "取消し"
163 "名前"
164 "注記"
165 "表示"

$	166	"Panner" is a graphical user interface control for
$ selecting that part of a larger area which the associated view
$ window is to display.
166 "パナー"

167 "接続"
168 "セクションの表示"

$   Messages 169-182 are for the window which is used to present a
$ detached graphic.
$	169	Fit >graphic< to window
169 "ウィンドウに合わせる"
$	170	Fit >window< to graphic
170 "グラフィックに合わせる"
171 "25 %"
172 "50 %"
173 "100 %"
174 "125 %"
175 "150 %"
176 "200 %"
$	177	"Custom" value entry for re-scaling of graphic
177 "カスタム..."

$   Messages 178-182 refer to where the panner is to be placed within
$ the detached graphic dialog window. Is a "panner" masculine or
$ feminine ? "Beats" me ;-).
178 "表示しない"
179 "左上に表示"
180 "右上に表示"
181 "左下に表示"
182 "右下に表示"

183 "削除"
184 "ブック"
185 "セクション"
186 "設定の変更..."
187 "ブラウズ"
188 "マップ"
189 "検索"
190 "ブラウサのジオメトリ"
191 ". . ."
192 "フォント・スケール"

$	193	"Pin []" is a verb
193 "ウィンドウをピンで固定"
194 "バグをファイル"
195 "ジオメトリをマップ"
196 "自動更新"
197 "セクション・ヒストリのサイズ"
198 "検索ヒストリのサイズ"
199 "検索一致の最大数"
200 "最初の一致の自動表示"
201 "適用"
202 "リセット"
203 "オン"
204 "オフ"
205 "セクション数"
206 "選択済み:"
207 "印刷予定:"
208 "エラーです。"
209 "印刷内容"
210 "セクション"
211 "階層"
212 "範囲名"
213 "名前なし"
214 "ブック"
215 "コンポーネント"
216 "新規"
217 "保存"
218 "名前の変更"
219 "復帰:"
220 "範囲:"
221 "クエリー:"
222 "一意的なロケータ:"
223 "消去"
224 "カット"
225 "ペースト"
226 "グループ化"
227 "グループ化解除"
228 "元に戻す"
229 "再実行"
230 "クエリー"
231 "すべて消去"

$   Messages 232-242 relate to operators available in a query
$ expression, and appear in the Query Editor.
232 "Contains"
233 "Doesn\'t Contain"
234 "Start With"
235 "Doesn\'t Start With"
$	236	Boolean operator OR
236 "Or"
$	237	"Xor" is for Boolean logic "Exclusive OR"
237 "Xor"
$	238	Boolean operator AND
238 "And"
239 "Near"
240 "Before"
241 "Weight"
242 "Proximity"
243 "Scope"
244 "Yes"
245 "No"
246 "Maches"
247 "紹介(I)"
248 "I"
249 "使い方(T)"
250 "T"
251 "リファレンス(R)"
252 "R"
253 "ヘルプの使い方(U)"
254 "U"
255 "インフォメーション・マネージャについて(A)"
256 "A"
257 "概要(v)"
258 "v"
$ "NEW-"
259 "o"
$ "NEW-"
260 "ヒストリ"
$ "NEW-"
261 "ヒストリ・リスト"
$ "NEW-"
262 "セクションは有効ではありません。"
$ "NEW-"
263 "デタッチするにはグラフィックをクリックしてください。"
$ "NEW-"
264 "グラフィックのレイズ"
$ "NEW-"
265 "グラフィックのアタッチ"

$       270  "..." is a button label indicating a list from which an
$            entry may be selected to fill a preceeding text field.
$            In this case, the user may enter a path into a text field
$            or activate the button with "..." to be presented with
$            a File Selection Box. The object is to select an Infolib
$            path.
$ "NEW-"
270 "..."


$set	8
$ 
$ dtinfo quick help messages (to be exhibited on status display area)
$ 
1 "ヒストリ中の前のセクションに移動します"
2 "ヒストリ中の次のセクションに移動します"
3 "ドキュメント中の前のセクションに移動します"
4 "ドキュメント中の次のセクションに移動します"

$ A search "match" is the same as a search "hit". "Match" is more
$ descriptive.
5 "前の検索の一致箇所に移動します"
6 "次の検索の一致箇所に移動します"

7 "現在のセクションを印刷します"
8 "ブックリストを表示します"
9 "グラフィカル・マップを表示します"
10 "新たなドキュメントがこのウィンドウを使用することを防ぐ(許す)には、ピンで固定して(ピンを抜いて)ください。"
11 "範囲エディタで使用する検索範囲を指定してください"
52 "選択されたセクションを表示します"
53 "独立したブックリストに選択されたセクションを表示します"
54 "選択されたセクションを印刷します"
55 "他のインフォメーション・ライブラリを開きます"
56 "選択されたインフォメーション・ライブラリを閉じます"
57 "クエリー・エディタを使用して検索を実行します"
58 "ブックマークと注記のリストを表示します"
60 "検索範囲を指定して下さい"
62 "範囲エディタを使用して検索範囲を指定してください"
64 "%s を検索します"


$set	9
$ 
$ messages specific to bookmark edit dialog
$ 
1 "Dtinfo: ブックマーク・エディタ"
2 "ブックマークの更新に失敗しました。"


$set	10
$ 
$ messages specific to detached graphic window
$ 
1 "Dtinfo: 分離グラフィック"
2 "分離グラフィック"
3 "ラスタ・イラスト (非スケーラブル)"
4 "ベクタ・イラスト (スケーラブル)"
5 "イラストをスケーリング中です、お待ちください..."
$ 
$ Dtinfo*SetZoom
$ 
6 "新しいスケーリングの比率を入力してください。"
$ 
$ Dtinfo*SetZoomTitle
$ 
7 "Dtinfo: スケールの設定"
$ 
8 "Dtinfo: "
9 "ラスタ・イラスト (スケーラブル)"


$set	11
$ 
$ messages specific to mark list window
$ 

$	1 is the title of a window containing a list of the
$ user's bookmarks
1 "Dtinfo: ユーザのマーク・リスト"


$set	12
$ 
$ messages specific to mark selection dialog
$ 
$   Messages 1 & 2: "Select" and "Edit" are verbs
1 "Dtinfo: マークの選択"
2 "Dtinfo: マークの編集"


$set	13
$ 
$ messages specific to section history window
$ 
1 "Dtinfo: セクション・ヒストリ"


$set	14
$ 
$ messages specific to preferences window
$ 
1 "Dtinfo: 設定"
2 "Dtinfo: ジオメトリの取得"


$set	15
$ 
$ messages specific to printing
$ 
1 "Dtinfo: 印刷"
$          Messages 2 - 5 are obsolete, and have been replaced by 12
2 環境中に印刷ダイアログ・マネージャが見つかりません
3 印刷ダイアログ・マネージャを起動できません
4 印刷ダイアログ・マネージャには画像ディスプレイに接続する権限がありません
5 印刷ダイアログ・マネージャには印刷ディスプレイに接続する権限がありません
6 印刷ダイアログ・マネージャを表示できません
7 デフォルトの印刷データをコピーできません
8 ファイルに印刷中にエラーが発生しました
9 印刷ジョブはすでに実行中です
10 Dtinfo: 印刷ステータス
$       Message 12 is used to handle errors from the PDM
12	印刷・ダイアログ・マネージャ(PDM) エラー - 設定失敗.


$set	16
$ 
$ messages specofic to search scope editor window
$ 
$	1	Edit the Scope for for use in a Search
1 "Dtinfo: 検索範囲エディタ"
$ 
$ Dtinfo*範囲保存"
$ 
2 "新しい範囲名を入力してください:"
$ 
$ Dtinfo*SaveScopeTitle
$ 
3 "Dtinfo: 範囲の保存"
$ 
4 "すべてのInfolib"


$set	17
$ 
$ messages specific to search result window
$ 
$	1	"Search Results" is for the results of a search
1 "Dtinfo: 検索結果"


$set	18
$ 
$ messages specific to open locator dialog, and locator
$ input formats in general.
$ 
$ 1	"Dtinfo: Open Locator"
1 "Dtinfo: ロケータを開く"

$ 2     "Locator specification format error."
2 "ロケータの仕様の書式エラーです。"


$ 3     "Locator resolution failed."
3 "ロケータの決定に失敗しました。"

$ 4     "Locator resolution failed within the loaded infolibs."
4 "読み込まれたInfilibの中で、ロケータの決定に失敗しました。"

$ 5     "Document not found."
5 "ドキュメントが見つかりません。"

$ 6     "No locator value provided."
6 "ロケータの値が与えられていません。"


$set	19
$ 
$ messages specific to query editor window
$ 
1 "Dtinfo: クエリー・エディタ"


$set	20
$ 
$ dtinfo general messages
$ 
$ 
$ Dtinfo*PixmapAllocationError
$ 
1 "グラフィックのレンダリングに必要な\n\
サーバのメモリがありません。"
$ 
$ Dtinfo*AbandonChanges
$ 
2 "変更内容を破棄しますか?"
$ 
$ Dtinfo*NoHelp
$ 
3 "ヘルプ情報が見つかりません。"
$ 
$ Dtinfo*NoSelection
$ 
4 "選択されたテキストがありません。\n\
テキストを選択してから、再試行してください。"
$ 
$ Dtinfo*AbandonBookmarks
$ 
5 "保存されていない、開かれたブックマークがあります。\n\
保存しないで中止しますか?"
$ 
$ Dtinfo*ConfirmQuit
$ 
6 "Dtinfoを中止しますか?"
$ 
$ Dtinfo*BadTocHierarchy
$ 
7 "バグをファイル"
$ 
$ Dtinfo*LinkToBookTitle
$ 
8 "%s: %s へリンク"
$ 
$ Dtinfo*LinkToTitle
$ 
9 "%s へリンク"
$ 
$ Dtinfo*MarkCreateFailed
$ 
10 "Dtinfo はこのブックマークを作成できません。"
$ 
$ Dtinfo*MarkDeleteFailed
$ 
11 "Dtinfo はこのブックマークを削除できません。"
$ 
$ Dtinfo*MarkMoveFailed
$ 
12 "Dtinfo はこのブックマークを削除できません。"
$ 
$ Dtinfo*MaxDepthExceeded
$ 
13 "バグをファイル"
$ 
$ Dtinfo*MarkIconInvisible
$ 
14 "選択されたブックマークを表示できません。\n\
このセクションの表示可能範囲外にあります。"
$ 
$ Dtinfo*MViewNoBase
$ 
15 "選択されているマークは\n\
アクセス可能なドキュメント上にありません。\n\
適切なInfolibを読み込んでください。"
$ 
$ Dtinfo*GetGeometry
$ 
16 "デフォルトとして保存するサイズをもつ\n\
Dtinfoウィンドウの内側をクリックしてください。"
$ 
$ Dtinfo*SaveModifiedScope
$ 
17 "変更内容を現在の範囲に保存しますか?"
$ 
$ Dtinfo*BadScopeName
$ 
18 "コロン(:)、コンマ(,)、\n\
セミコロン(\;)は、検索範囲名では\n\
許されません。"
$ 
$ Dtinfo*EmptyScopeName
$ 
19 "有効な範囲名を入力してください。"
$ 
$ Dtinfo*ScopeNameTooLong
$ 
20 "検索範囲名は、30文字を超えてはなりません。\n\
もっと短い範囲名を入力してください。"
$ 
$ Dtinfo*DuplicateScopeName
$ 
21 "入力された名前はすでに使用中です。\n\
一意的な範囲名を入力してください。"
$ 
$ Dtinfo*Everything
$	22	"Everything" refers to section components like
$		titles, body, and related.
$ 
22 "すべて"
$ 
$ Dtinfo*Titles
$ 
23 "タイトル"
$ 
$ Dtinfo*Body
$	24	"Body" as in the body of text.
$ 
24 "本文"
$ 
$ Dtinfo*Examples
$ 
25 "例"
$ 
$ Dtinfo*Index
$ 
26 "索引"
$ 
$ Dtinfo*Tables
$ 
27 "表"
$ 
$ Dtinfo*Graphics
$ 
28 "グラフィック"
$ 
$ Dtinfo*ScopeDeleteConfirm
$ 
29 "表示された範囲を削除しますか?"
$ 
$ Dtinfo*CurrentSection
$ 
30 "現在のセクション"
$ 
$ Dtinfo*IgnoredScopes
$ 
31 "このInfolibに有効ではない\n\
保存された範囲を無視します。"
$ 
$ Dtinfo*MarkBaseOpenFailed
$ 
32 "ユーザのマーク・データベースを開けません。"
$ 
$ Dtinfo*NoOpenMarkBases
$ 
33 "Dtinfoのマーク・データベースが見つかりませんでした。\n\
Dtinfo はブックマークを作成できません。"
$ 
$ Dtinfo*MarkBaseReadOnly
$ 
34 "現在のマーク・データベースは読み取り専用です。"
$ 
35 "ドキュメントで未知のエラーが発生しました: プロダクト・サポートに知らせてください。"

36 "検索エンジン(UAS_SearchEngine内)から、\n\
予期しないメッセージを受信しました:\n\
プロダクト・サポートに知らせてください。"
$ 
$ Dtinfo*TooManyIbases
$ 
37 "検索エンジンの制限のため、現在の範囲の最初の %d 個の\n\
ブックケースのみ、検索されます。\n\
継続しますか?"
$ 
$ Dtinfo*DemoFailure
$ 
38 "これはブラウザの評価バージョンです。\n\
このInfolibにアクセスする権限はありません。"
$ 
$ Dtinfo*StyleSheetSyntaxError
$ 
39 "このドキュメントのスタイル・シートにエラーがあります。\n\
デフォルトのスタイルが適用されます。"
$ 
$ Dtinfo*NoHits
$ 
40 "このクエリーでの一致はありません。"
$ 
$ Dtinfo*BadQuery
$ 
41 "Dtinfoはクエリーを理解できません。\n\
有効なクエリーを入力してください。"
$ 
$ Dtinfo*SearchError
$ 
42 "検索エンジンの内部エラーが検出されました。\n\
このブラウザ・セッションで今後おこなわれるすべての検索は、失敗するでしょう。\n\n\
この問題は記録され、ブラウザの今後のリリースで修正されます。\n\
一時的な回避策として、この状況を引き起こすようなクエリーの入力を避けてください。\n\
再び検索を有効にするには、ブラウザを中止し再起動してください。"
$ 
$ Dtinfo*KernelError
$ 
43 "検索エンジンの内部エラーが検出されました。\n\
おそらく、このマシンの/usr/tmpディレクトリが\n\
tmpfsファイル・システム上にあります(またはそこにリンクされています)。\n\
このブラウザ・セッションで今後おこなわれるすべての検索は、失敗するでしょう。\n\n\
この問題は認知され、記録されました。\n\
回避策は、/usr/tmpディレクトリをtmpfsファイル・システム上に置かない\n\
(またはそこにリンクしない)ように、このマシンの/usr/tmpディレクトリを再構成する事です。\n\
その後ブラウザを再起動すれば、検索は再び有効となり、正しく動作するはずです。\n\
詳しくは、システム管理者に問い合わせてください。"
$ 
$ Dtinfo*FulcrumNoMemory
$ 
44 "検索エンジンは、メモリを割り当てられなかったために、\n\
検索要求を完了できませんでした。再び検索を試みる前に、\n\
他のアプリケーションを終了してみるか、システムの物理メモリや\n\
や仮想メモリを増やしてください。"
$ 
$ Dtinfo*FulcrumError
$ 
45 "検索エンジンのエラーが発生しました。\n\
プロダクト・サポートに、検索エラー %d を報告してください。"
$ 
$ Dtinfo*BadHelp
$ 
46 "このボタンを押すと、現在読み込まれているInfolibには存在しない\n\
Dtinfoドキュメントへ、リンクします。"
$ 
$ Dtinfo*query_is_read_only
$ 
47 "これは、表示目的のフィールドです。クエリーを編集するには、\n\
クエリー・エディタ、または [閲覧] ウィンドウか [ブックリスト] ウィンドウの、\n\
[クイック検索] フィールドを使用してください。"
$ 
$ Dtinfo*ExceedRdWindows
$ 
48 "ユーザの設定で設定された、アクティブな [閲覧] ウィンドウ\n\
の最大数 (%d)に、既に達しました。"
$ 
$ Dtinfo*NoPrefBase
$ 
49 "利用可能なユーザの設定データベースがありません。"
$ 
$ Dtinfo*PrefBaseReadOnly
$ 
50 "ユーザの設定データベースは、読み取り専用モードで開かれています。\n\
新しい設定は、ディスクに保存されません。"
$ 
$ Dtinfo*DeadEndLink
$ 
51 "要求されたドキュメントまたはセクションを参照できません。"
$ 
$ Dtinfo*MarkPositionFailed
$ 
52 "Dtinfoは、\42%s\42という名前のマークを\n\
表示できません。\n\
それを結び付けていたパラグラフが、\n\
もはや存在しないためです。"
$ 
$ Dtinfo*ScopeChangeAbandon
$ 
53 "現在の範囲に対する変更内容を、破棄しますか?"
$ 
$ Dtinfo*InformationLibrary
$ 
63 "インフォメーション・ライブラリ"
$ 
$ Dtinfo*On
$ 
64 "オン"
$ 
$ Dtinfo*Off
$ 
65 "オフ"
$ 
67 "Infolibを作成できませんでした。" 

68 "Infolibの作成に失敗しました。"
$ 
69 "検索結果の重み付けは、利用可能ではありません。"

$	70	The "qualified locator" is a "fully qualified
$		locator" as opposed to the simple format.
70 "完全なロケータ(URL)によるアクセス方式が無効です。"

71 "ドキュメントの作成に失敗しました。"

$ "NEW-"
72 "Dtinfo: グラフィックのデタッチ"
$ "NEW-"
73 "デタッチするにはグラフィックをクリックしてください。"

$set	21
$ 
$ messages specific to message dialogs
$ 

$ Dtinfo*ErrorDialogTitle
$ 
1 "Dtinfo: エラー"

$ Dtinfo*WarningDialogTitle
$ 
2 "Dtinfo: 警告"

$ Dtinfo*InfoDialogTitle
$ 
3 "Dtinfo: 情報"

$ Dtinfo*ProgressDialogTitle
$ 
$	4	"Progress" is a level of completion in regards
$		 to a task.
4 "Dtinfo: 経過"

$ Dtinfo*QuitDialogTitle
$ 
5 "Dtinfo: 中止の確認"

$ Dtinfo*QuestionDialogTitle
$ 
6 "Dtinfo: 質問"
$ 
7 "Dtinfo: バグをファイル"


$set	22
$ 
$ dtinfo version information
$ 
$ 
$	Dtinfo*Version
$ 
1 "CDE Dtinfo Browser\n\
\n\
Copyright (c) 1996 Chisholm Technologies, Inc.,\n\
Austin, TX 78758 USA\n\
\n\
Copyright (c) 1994, 1995 HAL Computer Systems, Inc.\n\
\n\
All Rights Reserved.\n\
\n\
U. S. Government Restricted Rights\n\
Use, duplication or disclosure by the Government is subject\n\
to restrictions set forth in subparagraphs (a) through (d)\n\
of the Commercial Computer - Restricted Rights clause at FAR\n\
52.227-19 when applicable, or in subparagraph (c)(1)(ii) of\n\
the Rights in Technical Data and Computer Software clause at\n\
DFARS 252.227-7013, and in similar clauses in the NASA FAR\n\
Supplement. Contractor/manufacturer is HAL Computer\n\
Systems, 1315 Dell Avenue, Campbell, CA 95008 USA."


$set	23
$ 
$ messages specific to CanvasRenderer (dtinfo I/F to DtCanvas/DtHelp)
$ 


$set	24
$ 
$ messages specific to UAS/Base modules
$ 
1 "URLで提供されているアクセス部分がありません。"
2 "作成方法が分かりません %s: object"
3 "範囲エラーです: UAS_ObjList<T>::operator[]"
4 "範囲エラーです: UAS_PtrList<T>::operator[]"


$set	27
$ 
$ messages specific to UAS/MMDB modules
$ 
1 "ブックケースは有効ではありません:"
2 "無効な MMDB infolib パスです: %s"
3 "無効な MMDB infolib パスを無視します: %s"
4 "MMDB ライブラリ %d"
5 "MMDB Infolib: "
6 "Infolib %s"


$set	28
$ 
$ messages specific to graphical map window
$ 
1 "Dtinfo: ローカル・マップ"
2 "Dtinfo: "


$set	29
$ 
$ messages specific to search history window
$ 
$	1	"Search Results History" is a history of the
$ search results already obtained under the dtinfo session, for
$ prior queries.
1 "Dtinfo: 検索結果ヒストリ"


$set	30
$ 
$ messages specific to window system module
$ 
1 "Dtinfo: 致命的エラー"
2 "終了します..."
3 ディスプレイを開けません。\n

$set	31
$ 
$ messages specific to printing
$ 
1 "ページ・ヘッダ"
2 "ページ・フッタ - %d ページ"


$set	40
$ 
$ messages specific to environmental interface or program context
$ 

$ 1     ""
1 "実装されていません。"


$set	41
$ The messages in this set will only appear in the users
$   "errorlog" file.
$ 
$ ******************************************************************************
$ 
$ messages specific to tooltalk interface and operations.
$ Note that many errors defined by tooltalk are passed through
$ and therefore not repeated here.
$ 

$ 1     "Can't get tt session for X display %s"
1 Xディスプレイ s の tt session を得られません。:\n%s

$ 2     "Could not set default tooltalk session."
2 デフォルトの tooltalk session を設定できませんでした。:\n%s

$ 3 Failed to open ToolTalk connection.
3 ToolTalk接続を開くのに失敗しました。:\n%s

$ 4     "Could not declare ptype DtInfo."
4 ptype DtInfoを宣言できませんでした。:\n%s

$ Messages 5-10: These messages result from an error in a tt_* function
$    call.
$ 
5 ttsession(1)プロセスが実行中ではなく、また\n\
ToolTalkサービスはそれを再起動できません。
6 現在のデフォルトのプロセス識別子は、期限切れか無効です。
7 指定されたToolTalkセッションは、期限切れか無効です。

$ 
$ Message 8: Do not localize the word DtInfo.
$ 
8 プロセス・タイプ'DtInfo'は、\n\
インストールされたプロセス・タイプの名前ではありません。

$ 
$ Message 9: Do not localize the word DtInfoPrint.
$ 
9 プロセス・タイプ'DtInfoPrint'は、\n\
インストールされたプロセス・タイプの名前ではありません。

10 エラー番号は認知されません。

$set    42
$ 
$ messages specific to UAS/DtSR_SearchEngine modules
$ 
1 "DtSearchはクエリーをサポートしていません。"
