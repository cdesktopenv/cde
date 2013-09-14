$ $XConsortium: dtksh.msg /main/4 1996/10/16 03:16:20 cde-hit $
$set 1
1 DtkshCvtStringToPixel:カラー '%s' は定義されていません。

$set 5
1 リソース・パラメータの指定が多すぎます。'%s' をスキップします。
2 リソース指定が不正です。形式は '名前:値' です : %s
3 リソース・セパレータはコロン ':' ですが、等号 '=' を使用しています : %s
4 ツールキットはすでに初期化されています。
5 使用方法: XtInitialize 変数 アプリケーション名 アプリケーションクラス [引き数 ...]
6 使用方法: %s 変数 名前 クラス 親プロセス [引き数:値 ...]
7 使用方法: XtCreateApplicationShell 変数 名前 クラス [引き数:値 ...]
8 使用方法: XtPopup ウィジェット GrabNone|GrabNonexclusive|GrabExclusive
9 グラブ・タイプ '%s' は認識できません。'GrabNone' を使用します。
10 使用方法: XtIsShell ウィジェット
11 使用方法: XtMapWidget ウィジェット
12 使用方法: XtUnmapWidget ウィジェット
13 使用方法: %s 変数 ウィジェット
14 使用方法: XtScreen 変数 ウィジェット
15 使用方法: XtWindow 変数 ウィジェット
16 使用方法: XtCallCallbacks ウィジェット コールバック名
17 使用方法: XtHasCallbacks 変数 ウィジェット コールバック名
18 使用方法: XtAddCallback ウィジェット コールバック名 ksh-コマンド
19 使用方法: XtRemoveCallback ウィジェット コールバック名 ksh-コマンド
20 次のコールバック・ハンドルは無効です: %s
21 指定のコールバックは登録されていません。
22 使用方法: XtAddEventHandler ウィジェット マスク nonMaskable ksh-コマンド
23 使用方法: XtRemoveEventHandler ウィジェット マスク nonMaskable ksh-コマンド
24 指定のイベント・ハンドラは登録されていません。
25 使用方法: XtGetValues ウィジェット リソース:変数 ...
26 次のリソース・パラメータの書式が正しくありません: %s
27 使用方法: XtSetValues ウィジェット 引き数:値 ...
28 使用方法: XtAddWorkProc 変数 コマンド
29 使用方法: XtRemoveWorkProc workProcId
30 workProcId パラメータは 16 進数でなければなりません: %s
31 使用方法: XtAddTimeOut 変数 ミリ秒 コマンド
32 ミリ秒パラメータは 0 より大きくなければなりません。
33 使用方法: XtRemoveTimeOut intervalId
34 intervalId パラメータは 16 進数でなければなりません: %s
35 使用方法: %s ウィジェット ...
36 使用方法: XBell ディスプレイ 音量
37 使用方法: %s 変数 画面
38 画面パラメータは無効です: %s
39 使用方法: XDefineCursor ディスプレイ ウィンドウ cursorId
40 cursorId パラメータは無効です: %s
41 使用方法: XUndefineCursor ディスプレイ ウィンドウ
42 使用方法: XtRemoveAllCallbacks ウィジェット コールバック名
43 使用方法: XTextWidth 変数 フォント名 文字列
44 使用方法: %s ディスプレイ drawable [引き数 ...]
45 drawable パラメータは無効です: %s
46 destination パラメータは無効です: %s
47 グラフィック機能名を認識できません: %s
48 行スタイルを認識できません: %s
49 描画オプションを認識できません: %s
50 残りのポイントは無視します。
51 使用方法: XCopyArea ディスプレイ ソース dest sourceX sourceY 幅 高さ destX destY [引き数 ...]
52 リソース・タイプ '%s' を 'String' に変換できません。
53 リソース・タイプ 'String' を '%s' に変換できません。
54 使用方法: XtAddInput 変数 [-r] fileDescriptor kshCommand
56 使用方法: XtRemoveInput inputId
57 inputId パラメータは 16 進数でなければなりません: %s
59 使用方法: XFlush ディスプレイ
60 使用方法: XSync ディスプレイ discard
61 使用方法: XRaiseWindow ディスプレイ ウィンドウ
62 使用方法: %s ウィジェット [True|False]
63 使用方法: %s ウィジェット translations
64 使用方法: XtUninstallTranslations ウィジェット
65 使用方法: XtParent 変数 ウィジェット
66 使用方法: XtLastTimestampProcessed 変数 ディスプレイ
67 使用方法: DtSessionRestorePath ウィジェット パス変数 保存ファイル
68 使用方法: DtSessionSavePath ウィジェット パス変数 ファイル変数
69 使用方法: DtShellIsIconified ウィジェット
70 使用方法: DtSetStartupCommand ウィジェット コマンド
71 使用方法: DtSetIconifyHint ウィジェット ブール
73 rootWindow パラメータは無効です: %s
74 使用方法: DtWsmSetCurrentWorkspace ウィジェット アトム
75 ワークスペース・アトムは無効です: %s
78 使用方法: DtWsmSetWorkspacesOccupied ディスプレイ ウィンドウ ワークスペース・リスト
79 使用方法: DtGetHourGlassCursor 変数 ディスプレイ
80 使用方法: DtWsmAddCurrentWorkspaceCallback 変数 ウィジェット ksh-コマンド
81 使用方法: DtWsmRemoveWorkspaceCallback ハンドル
82 使用方法: XtNameToWidget 変数 referenceWidget 名前
83 使用方法: DtDbLoad
84 使用方法: DtDbReloadNotify ksh-コマンド
85 使用方法: DtActionExists アクション名
86 使用方法: DtActionLabel 変数 アクション名
87 使用方法: DtActionDescription 変数 アクション名
88 使用方法: DtActionInvoke ウィジェット アクション名 termParms 実行ホスト コンテキスト・ディレクトリ useIndicator ksh-コマンド ["FILE" ファイル名] ...
89 使用方法: DtDtsLoadDataTypes
90 使用方法: DtDtsFileToDataType 変数 ファイル名
91 使用方法: DtDtsFileToAttributeValue 変数 ファイル名 属性名
92 使用方法: DtDtsFileToAttributeList 変数 ファイル名
93 使用方法: DtDtsDataTypeToAttributeValue 変数 データ型 属性名 オプション名
94 使用方法: DtDtsDataTypeToAttributeList 変数 データ型 オプション名
95 使用方法: DtDtsFindAttribute 変数 名前 値
96 使用方法: DtDtsDataTypeNames 変数
97 使用方法: DtDtsSetDataType 変数 ファイル名 データ型 override
98 使用方法: DtDtsDataTypeIsAction データ型
99 使用方法: ttdt_open 変数 ステータス 変数2 ツール名 メーカー バージョン sendStarted
100 使用方法: tttk_Xt_input_handler procId ソース id
101 ソース・パラメータは整数でなければなりません: %s
102 id パラメータは 16 進数でなければなりません: %s
103 使用方法: ttdt_close ステータス procId newProcId sendStopped
104 使用方法: ttdt_session_join 変数 ステータス sessId shellWidgetHandle join
105 使用方法: ttdt_session_quit ステータス sessId sessPatterns quit
106 sessPatterns パラメータは無効です: %s
107 使用方法: ttdt_file_event ステータス op パターン send
108 使用方法: ttdt_file_quit ステータス パターン quit
109 使用方法: ttdt_Get_Modified パス名 範囲 タイムアウト
110 使用方法: %s ステータス パス名 範囲 タイムアウト
112 使用方法: tt_error_pointer 変数 ttStatus
113 使用方法: %s ステータス msg
114 使用方法: %s ステータス msg msgStatus msgStatusString destroy
115 使用方法: ttdt_file_join 変数 ステータス パス名 範囲 join ksh-コマンド
116 使用方法: tt_file_netfile 変数 ステータス ファイル名
117 使用方法: tt_netfile_file 変数 ステータス ネットワークファイル名
118 使用方法: tt_host_file_netfile 変数 ステータス ホスト ファイル名
119 使用方法: tt_host_netfile_file 変数 ステータス ホスト ネットワークファイル名
120 使用方法: XtIsSubclass ウィジェット クラス
121 %s は有効なウィジェット・クラス名ではありません。
122 使用方法: XtClass 変数 ウィジェット

$set 6
1 DtkshCvtWindowToString: 'from' 値のサイズは無効です。
2 DtkshCvtHexIntToString: 'from' 値のサイズは無効です。
3 DtkshCvtIntToString: 'from' 値のサイズは無効です。
4 DtkshCvtBooleanToString: 'from' 値のサイズは無効です。
5 DtkshCvtStringToWidget: 'from' 値のサイズは無効です。
6 DtkshCvtStringToWidget: '%s' という名前のウィジェットは見つかりません。
7 DtkshCvtStringToCallback: 'from' 値のサイズは無効です。
8 DtkshCvtCallbackToString: 'from' 値のサイズは無効です。
9 DtkshCvtWidgetToString: 'from' 値のサイズは無効です。
10 DtkshCvtWidgetToString: ウィジェット名が見つかりません。
11 DtkshCvtWidgetClassToString: 'from' 値のサイズは無効です。
12 DtkshCvtWidgetClassToString: 未知のウィジェット・クラスです。
13 DtkshCvtStringToWidgetClass: 未知のウィジェット・クラス名です。
14 DtkshCvtScreenToString: 'from' 値のサイズは無効です。

$set 11
1 '%s' という名前のフィールドは、ストラクチャ '%s' 内では見つかりません。
2 DtkshCvtCallbackToString: 内部変換バッファがオーバーフローしました。
3 ウィジェット・クラス '%s' のリソース '%s' のハッシング障害です。
4 機能名が指定されていません。
5 コマンドを実行できません。ツールキットが初期化されていません。
6 ウィジェット '%s' の作成に失敗しました。
7 ディスプレイ・パラメータは無効です: %s
8 次の属性アトムは無効です: %s
9 次のリソースは、ウィジェット '%s' に定義されていません: %s
10 指定の位置は無効です: %s
11 指定のフォントは無効です: %s
12 ウィジェット・ハンドル '%s' は、既存のウィジェットを参照しません。
13 ウィジェットは、'command' ウィジェットでなければなりません。
14 ウィジェットは、'mainWindow' ウィジェットでなければなりません。
15 ウィジェットは、'scale' ウィジェットでなければなりません。
16 ウィジェットは、'scrollBar' ウィジェットでなければなりません。
17 ウィジェットは、'toggleButton' ウィジェットまたはガジェットでなければなりません。
18 ウィンドウ・パラメータは無効です: %s
19 必須メモリを割り当てられません。終了します。
20 親ウィジェットを見つけられません。
21 シンボル '%s' を位置付けられません。
22 タイプ '%s' を位置付けられません。
23 宣言 '%s' を構文解析できません。'unsigned long' を使用します。
24 未知の子タイプ: %s
25 認識できないオプション・フラグ: %s
26 使用方法: %s ウィジェット 位置
27 使用方法: %s ウィジェット 時間
28 使用方法: %s ウィジェット
29 dtksh エラー
30 dtksh 警告
31 使用方法: %s ディスプレイ ウィンドウ
32 使用方法: %s ディスプレイ ウィンドウ 変数
33 使用方法: %s ディスプレイ rootWindow 変数
34 タイムアウト・パラメータは無効です: %s
35 msg パラメータは無効です: %s
36 パターン・パラメータは無効です: %s

$set 14
1 内部ハッシュ・テーブル障害がウィジェット・クラスの初期化中に発生しました。終了します。
2 '%s' という名前のウィジェット・クラスが見つかりません。
3 識別子 '%s' は有効なウィジェット・ハンドルではありません。
4 ウィジェット・クラスを見つけられません。
5 使用方法: DtLoadWidget ウィジェット・クラス名 ウィジェット・クラス・レコード名
6 '%s' という名前のウィジェット・クラス・レコードを位置付けられません。
7 内部ハッシュ・テーブル障害がウィジェット・クラス '%s' の初期化中に発生しました。
8 \n%sRESOURCES FOR %s%s%s:\n
9 制約条件
10 R
11 M
12 S
13 %-15s %-6s %-6s %-18s %-6s %s\n
14 環境変数    親クラスを処理              ステータス名\n
15 \n使用方法:\tDtWidgetInfo [ウィジェット・ハンドル]\n\tDtWidgetInfo -r <ウィジェット・ハンドル|クラス名>\n\tDtWidgetInfo -R <ウィジェット・ハンドル|クラス名>\n\tDtWidgetInfo -c [クラス名]\n\tDtWidgetInfo -h [ウィジェット・ハンドル]

$set 15
1 ツールキットを初期化できません。
2 ウィジェット作成時に、次のリソースを\n\
設定できませんでした。作成後、%s の代わりに\n\
XtSetValues を使用してください。
3 使用方法: %s 変数 親 名前 [引き数:値 ...]
4 使用方法: DtHelpReturnSelectedWidgetId 変数 ウィジェット 変数
5 使用方法: DtHelpSetCatalogName カタログ名
6 使用方法: DtHelpQuickDialogGetChild 変数 quickHelpWidget 子プロセス
7 ウィジェットは、'quickHelp' ウィジェットでなければなりません。
8 ウィジェットは、'list' ウィジェットでなければなりません。
9 使用方法: %s ウィジェット 位置 項目
10 使用方法: %s ウィジェット 位置 項目リスト
11 使用方法: %s ウィジェット 項目
12 使用方法: XmListDeleteItemsPos ウィジェット カウント 位置
13 使用方法: XmListDeleteItems ウィジェット 項目リスト
14 使用方法: XmListDeletePositions ウィジェット 位置リスト
15 使用方法: XmListGetKbdItemPos 変数 ウィジェット
16 使用方法: XmListItemExists ウィジェット 項目
17 使用方法: XmListItemPos 変数 ウィジェット 項目
18 使用方法: XmListPosSelected ウィジェット 位置
19 使用方法: XmListPosToBounds ウィジェット 位置 変数 変数 変数 変数
20 使用方法: %s ウィジェット 位置 通知フラグ
21 使用方法: %s ウィジェット 項目 通知フラグ
22 使用方法: XmListSetAddMode ウィジェット boolean
23 使用方法: XmListSetKbdItemPos ウィジェット 位置
24 使用方法: XmMainWindowSetAreas メイン・ウィンドウ メニュー コマンド hscroll vscroll work
25 'mainWindow' ハンドルが NULL です。
26 使用方法: %s 変数 メイン・ウィンドウ
27 使用方法: XmProcessTraversal ウィジェット 方向
28 未知のトラバーサルの方向です: %s
29 使用方法: XmInternAtom 変数 ディスプレイ 名前 onlyIfExists
30 使用方法: XmGetAtomName 変数 ディスプレイ アトム
31 指定のアトムは無効です: %s
32 使用方法: XmGetColors ウィジェット バックグラウンド・カラー フォアグラウンド・カラー トップシャドウ ボトムシャドウ 選択
33 バックグラウンド・ピクセルは無効です: %s
34 使用方法: XmUpdateDisplay ウィジェット
35 使用方法: %s ウィジェット プロトコル [プロトコル ...]
36 指定のアトムは無効です: %s
37 使用方法: XmAddWMProtocolCallback ウィジェット プロトコル ksh-コマンド
38 使用方法: XmRemoveWMProtocolCallback ウィジェット プロトコル ksh-コマンド
39 使用方法: XmMenuPosition メニュー イベント
40 指定のイベントは無効です: %s
41 使用方法: XmCommandAppendValue コマンド・ウィジェット 文字列
42 使用方法: XmCommandError コマンド・ウィジェット エラー・メッセージ
43 使用方法: XmCommandSetValue コマンド・ウィジェット コマンド
44 使用方法: XmCommandGetChild 変数 コマンド・ウィジェット 子プロセス
45 使用方法: XmMessageBoxGetChild 変数 コマンド・ウィジェット 子プロセス
46 ウィジェットは、'messageBox' ウィジェットでなければなりません。
47 使用方法: XmFileSelectionBoxGetChild 変数 ウィジェット 子プロセス
48 ウィジェットは、'file selection box' ウィジェットでなければなりません。
49 使用方法: XmSelectionBoxGetChild 変数 ウィジェット 子プロセス
50 ウィジェットは、'selection box' ウィジェットでなければなりません。
51 使用方法: XmScaleGetValue スケール・ウィジェット 変数
52 使用方法: XmScaleSetValue スケール・ウィジェット 値
53 指定のスケール値は無効です: %s
54 使用方法: XmScrollBarGetValues スクロール・バー 変数 変数 変数 変数
55 使用方法: XmScrollBarSetValues スクロール・バー 値 スライダ・サイズ 増加 ページ増加 通知
56 指定の値は無効です: %s
57 指定のスライダ・サイズは無効です: %s
58 指定の増加は無効です: %s
59 指定のページ増加は無効です: %s
60 使用方法: XmScrollVisible scrolledWin ウィジェット 左右マージン 上下マージン
61 ウィジェットは、'scrolledWindow' ウィジェットでなければなりません。
62 表示しようとするウィジェットは存在しません。
63 指定の左／右マージンは無効です: %s
64 指定の上／下マージンは無効です: %s
65 使用方法: %s ウィジェット 状態 通知
66 使用方法: catopen 変数 catName
67 使用方法: catclose catId
68 使用方法: catgets 変数 catId setNum msgNum dftMsg
69 ウィジェットは、'text' または 'textField' ウィジェットでなければなりません。
70 使用方法: %s 変数 ウィジェット
71 使用方法: %s ウィジェット ブール
72 指定のパラメータは無効です: %s
73 使用方法: %s ウィジェット 行
74 使用方法: %s ウィジェット 最長
75 使用方法: %s ウィジェット 文字列
76 使用方法: %s ウィジェット 変数 変数
77 使用方法: %s ウィジェット 位置 文字列
78 使用方法: %s ウィジェット 位置 変数 変数
79 使用方法: %s ウィジェット fromPosition toPosition 文字列
80 指定の 'from' 位置は無効です: %s
81 指定の 'to' 位置は無効です: %s
82 使用方法: %s ウィジェット 最初の位置 最後の位置 時間
83 指定の最初の位置は無効です: %s
84 指定の最後の位置は無効です: %s
85 指定の時間は無効です: %s
86 使用方法: %s 変数 ウィジェット x y
87 指定の x 位置は無効です: %s
88 指定の y 位置は無効です: %s
89 使用方法: %s ウィジェット 左 右 モード
90 指定の左位置は無効です: %s
91 指定の右位置は無効です: %s
92 使用方法: %s ウィジェット 開始 文字列 方向 変数
93 指定の開始位置は無効です: %s
94 使用方法: XmListGetSelectedPos 変数 ウィジェット
95 使用方法: XmListGetMatchPos 変数 ウィジェット 項目
96 使用方法: XmOptionLabelGadget 変数 ウィジェット
97 使用方法: XmOptionButtonGadget 変数 ウィジェット
98 使用方法: XmGetVisibility 変数 ウィジェット
99 使用方法: XmGetTearOffControl 変数 ウィジェット
100 使用方法: XmGetTabGroup 変数 ウィジェット
101 使用方法: XmGetPostedFromWidget 変数 ウィジェット
102 使用方法: XmGetFocusWidget 変数 ウィジェット
103 使用方法: XmFileSelectionDoSearch ウィジェット ディレクトリ・マスク

$set 16
1 DtkshCvtNamedValueToString: 一致するものはありません。
2 DtkshCvtStringToNamedValue: 文字列 '%s' を変換できません。


$ The following messages are for ksh93 itself. message numbers must
$ match those specified in the MsgStr array allmsgs in 
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.
$set 25

1  完了
3  実行中
4  [-n] [引き数...]
5  [引き数...]
6  [ディレクトリ] [リスト]
7  [ジョブ...]
8  [n]
9  [名前 [パス名] ]
10  [名前]
11  [トップ] [ベース]
12  expr...
13  書式 [引き数...]
14  機能です。
15  キーワードです。
16  内蔵シェルです。
17  エクスポート機能です。
18  未定義の機能です。
19  名前 [引き数...]
20  optstring 名前 [引き数...]
21  秒
23 ${HOME:-.}/.profile
24 %c: 表現に無効な文字があります - %s
25 %c: 未知の形式指示子です。
26 %d-%d: 範囲が無効です。
27 %d: 無効なバイナリ・スクリプト・バージョンです。
28 %s は次の別名です。
29 %s は、次のエクスポートされた別名です。
30 %s がありません。
31 %s 未知のベース
32 %s: ':' が '?' オペレータにエクスポートされました。
33 %s: 曖昧です。
34 %s: 引き数は %ジョブ ID またはプロセス ID でなければなりません。
35 %s: 別名が見つかりません。\n
36 %s: 算術構文エラー
37 %s: 割り当てには lvalue が必要です。
38 %s: ファイル・ユニット番号が不正です。
39 %s: 書式が不正です。
40 %s: 番号が不正です。
41 %s: オプションが不正です。
42 %s: 置換が不正です。
43 %s: トラップが不正です。
44 %s: 作成できません。
45 %s: 実行できません。
46 %s: 開けません。
47 %s: 0 で除算しました。
48 %s: ドメイン例外です。
49 %s: %s に失敗しました。
50 %s: ファイルがすでに存在します。
51 %s: 機能名が不正です。
52 %s: 別名が無効です。
53 %s: 制御機能が無効です。
54 %s: エクスポート名が無効です。
55 %s: 機能名が無効です。
56 %s: 名前が無効です。
57 %s: 正規表現が無効です。
58 %s: セルフ・リファレンスが無効です。
59 %s: 次の不正使用です:
60 %s: 変数名が無効です。
61 %s: 識別子ではありません。
62 %s: 読み取り専用です。
63 %s: ラベルが実装されていません。
64 %s: 制限を越えています。
65 %s: もっと多くのトークンが予想されます。
66 %s: 親がありません。
67 %s: リファレンス名がありません。
68 %s: 見つかりません。
69 %s: 実装されていません。
70 %s: オペランドが非互換タイプです。
71 %s: オーバーフロー例外
72 %s: パラメータが設定されていません。
73 %s: パラメータが NULL であるか、設定されていません。
74 %s: 再帰が深すぎます。
75 %s: リファレンス変数を配列できません。
76 %s: パス名引き数が必要です。
77 %s: 制限されています。
78 %s: 単一性例外
79 %s: サブスクリプトは対象外です。
80 %s: 括弧が釣り合いません。
81 %s: 未知の機能です。
82 %s: 未知のロケールです。
83 %s: 未知のオペレータです。
84 %s: 未知のシグナル名です。
85 %s: ループが発生します。
86 (コアダンプ)
87 -c には、引き数が必要です。
88 -e - には、単一の引き数が必要です。
89 /vpix
90 <未知のコマンド>
91 ACE#?F#?H:[ 名前 ]L#?R#?Z#?fi#?[ ベース ]lnprtux [ 名前 =[ 値 ]...]
92 AE#?F#?HL#?R#?Z#?fi#?[ ベース ]lnprtux [ 名前 =[ 値 ]...]
93 中止
94 Ad:[delim]prst#[ タイムアウト ]u#[ ファイル番号 ] [ 名前...]
95 アラーム呼び出し
96 root ノード指定が不正です。
97 システム・コールが不正です。
98 壊れたパイプ
99 バス・エラー
100 ジョブ・コントロールを起動できません。
101 現在のオプション設定
102 DIL シグナル
103 子が落ちました。
104 DircabefhkmnpstuvxCI:[ ファイル ]o:?[ オプション ]A:[ 名前 ] [ 引き数...]
105 DircabefhkmnpstuvxCo:?[ オプション ]A:[ 名前 ] [ 引き数...]
106 EMT トラップ
107 CPU タイムの制限を越えました。
108 ファイル・サイズの制限を越えました。
109 フローティング例外
110 HSacdfmnstv [ 制限 ]
111 ハングアップ
112 IO シグナル
113 指示が不正です。
114 割り込み
115 強制終了しました。
116 LP [ ディレクトリ ] [ 変更 ]
117 メモリ障害
118 プロセスの移行
119 ジョブ制御なし
120 電話回線割り込み
121 ポーリング・アラーム
122 電源異常
123 プロファイルイング・タイム・アラーム
124 終了
125 リソースが失われました。
126 旧 tty ドライバに戻します...
127 S [ マスク ]
128 SIGAPOLLO
129 セキュリティ・ラベルを変更しました。
130 ソケット割り込み
131 サウンドが完了しました。
132 中止しました。(シグナル)
133 中止しました。(tty 入力)
134 プロセスの中止を継続しました。
135 中止しました。
136 中止しました。(tty 出力)
137 新規 tty ドライバに切り替えます...
138 まもなくシステムがクラッシュします。
139 終了しました。
140 トレース/BPT トラップ
141 認識できないバージョンです。
142 このシェルを終了するには、'exit' を使用してください。
143 ユーザ・シグナル 1
144 ユーザ・シグナル 2
145 バージョンが定義されていません。
146 仮想タイム・アラーム
147 ウィンドウ・サイズ変更
148 実行中のジョブがあります。
149 中止したジョブがあります。
150 [_[:alpha:]]*([_[:alnum:]])
151 \n@(#)Version 12/28/93\n
152 \n@(#)Version M-12/28/93\n
153 \nreal
154 \r\n\007活動停止が原因で、シェルは 60 秒後にタイムアウトします。
155 a 名前...
156 a:c [ コマンド [ 引き数...] ]
157 afpv 名前...
158 alarm %s %.3f\n
159 alarm -r %s +%.3g\n
160 引き数が必要です。
161 ディレクトリが不正です。
162 ファイル・ユニット番号が不正です。
163 置換が不正です。
164 親ディレクトリにアクセスできません。
165 パイプを作成できません。
166 一時ファイルを作成できません。
167 フォークできません。
168 %s を獲得できません。
169 %s を設定できません。
170 アラームを設定できません。
171 必須条件
172 dsf:[ ライブラリ ] [ 名前...]
173 e:[ エディタ ]lnrsN# [ 最初 ] [ 最後 ]
174 ファイルの終わり (EOF)
175 f:[ 書式 ]nprsu:[ ファイル番号 ] [ 引き数...]
176 fnv 名前...
177 hist -e "${VISUAL:-${EDITOR:-vi}}" 
178 ヒストリ・ファイルを開けません。
179 構文が正しくありません。
180 引き数のタイプ %c は無効です。
181 シェル内蔵バージョンです。
182 別名をトラックします。
183 強制終了します。
184 %d 行: \\ は $ を進めません。
185 %d 行: ${} の中の %c は引用符で括らなければなりません。
186 %d 行: %s は未知のラベルです。
187 %d 行: [[...]] の中に %s を入れるのは旧式です。((...)) を使用してください。
188 %d 行: '=' は旧式です。'==' を使用してください。
189 %d 行: -a は旧式です。-e を使用してください。
190 %d 行: %c の前の \\ は将来使用するために予約されています。
191 %d 行: `...` は旧式です。$(...) を使用してください。
192 %d 行: 曖昧性を解消するには、%c をエスケープします。
193 %d 行: ラベル %s を無視します。
194 %d 行: 曖昧性を解消するには、%c を引用符で括ってください。
195 %d 行: セット %s は旧式です。
196 %d 行: 入れ子サブシェルにはスペースが必要です。
197 %d 行: $id[...] との曖昧性を解消するには、中括弧を使用してください。
198 %d 行: %c 演算子と %c 演算子を分けるには、スペースまたはタブを使用してください。
199 ln#[ シグニチャ番号 ]s:[ シグニチャ名 ] シグニチャ...
200 ログイン setuid/setgid シェルは禁止されています。
201 マッピング
202 改行
203 nlp [ ジョブ...]
204 ヒストリ・ファイルがありません。
205 照会プロセスはありません。
206 そのようなジョブはありません。
207 そのようなプロセスはありません。
208 サポートしていません。
209 オフ
210 オン
211 開くことができるファイル数の制限を越えました。
212 メモリがなくなりました。
213 p [ アクション 条件...]
214 p [ 名前 [= 値]...]
215 パラメータが設定されていません。
216 アクセス権が拒否されました。
217 プロセスがすでに存在します。
218 ptx [ 名前 =[ 値]...]
219 pvV 名前 [ 引き数 ]...
220 r [ 変数名 秒数 ]
221 %d 行に構文エラーがあります: `%s' %s
222 %d 行に構文エラーがあります: ラベル %s が重複しています。
223 構文エラー: `%s' %s
224 sys
225 入力待ちがタイムアウトしました。
226 予想外です。
227 ユニバースはアクセス可能ではありません。
228 無制限です。
229 一致しません。
230 ユーザ
231 バージョン
232 %d への書き込みが失敗しました。
233 $_ にメールがあります。
234 ゼロ・バイト
