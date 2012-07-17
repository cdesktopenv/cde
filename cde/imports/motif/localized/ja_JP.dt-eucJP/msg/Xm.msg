$ $XConsortium: Xm.msg /main/9 1996/12/01 23:21:59 cde-hit $
$
$  @OSF_COPYRIGHT@
$  COPYRIGHT NOTICE
$  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
$  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
$  the full copyright text.
$

$ This message catalog contains messages issued by Motif toolkit library.
$ Consult X-Window systems, OSF/MOTIF Programmer's Guide and OSF/MOTIF
$ Programmer's Reference Manual for technical terms if you have any
$ doubts about their meanings.
$ Do not translate variables such as %s, %d %s ... etc.
$ Do not translate Motif resource name, such as XmNlabelType.
$ Special terms with capital letter(s) should not be translated.
$ "False" and "True" are keywords. Do not translate.
 
$quote " 


$set MS_ArrowButton

MSG_ArrowButton_1000 "矢印の向きが正しくありません。"
$ MSG_ArrowButton_1000 message is obsolete - DO NOT localize this message.

$set MS_BulletinB

MSG_BulletinB_1000 "サイズ変更ポリシーが正しくありません。"
$ MSG_BulletinB_1000 message is obsolete - DO NOT localize this message.

$ MSG_BulletinB_0001 "ダイアログ・スタイルが正しくありません。"
$ "NEW-"
MSG_BulletinB_0001 "ダイアログ・スタイルは XmDIALOG_MODELESS でなければなりません。"

MSG_BulletinB_1001 "シャドウ・タイプが正しくありません。"
$ MSG_BulletinB_1001 message is obsolete - DO NOT localize this message.

MSG_BulletinB_1002 "NULL フォント・リスト(ベンダ・シェル・デフォルトがありません。)"
$ MSG_BulletinB_1002 message is obsolete - DO NOT localize this message.


$set MS_CascadeB

MSG_CascadeB_1000 "XmCascadeButton は XmRowColumnWidgetClass の\n\
         親のタイプが正しくなければなりません。"
$ MSG_CascadeB_1000 message is obsolete - DO NOT localize this message.

MSG_CascadeB_0001 "XmMENU_PULLDOWN XmRowColumnWidgetだけが\n\
submenus になることができます。"
$ please do not translate submenus. 

$ "NEW-"
MSG_CascadeB_0002 "XmNmappingDelay は 0 よりも大きいか等しくなければなりません。"

$ "NEW-"
MSG_CascadeB_0000 "XmCascadeButton[Gadget] は XmNrowColumnType が XmMENU_PULLDOWN, XmMENU_POPUP, XmMENU_BAR または XmMENU_OPTION の XmRowColumn の親をもたなければなりません。"

MSG_CascadeB_0003 "XtGrabPointer が失敗しました。"

MSG_RowColText_0024 "XtGrabKeyboard が失敗しました。"


$set MS_Command

MSG_Command_0000 "ダイアログ・タイプは XmDIALOG_COMMAND でなければなりません。"

MSG_Command_0001 "子のタイプが無効です。\n\
Command ウィジェットには、このタイプの子を指定できません。"
$ please do not translate Command.

$ "NEW-"
MSG_Command_0002 "NULL または空の XmString です。"

$ "NEW-"
MSG_Command_0003 "NULL または空の XmString が XmCommandAppendValue に渡されました。"

MSG_Command_0004 "Command ウィジェットに対する XmNmustMatch は常に False です。"
$ please do not translate False or Command. 

MSG_Command_0005 "XmNhistoryMaxItems は 1 以上の正の整数でなければなりません。"


$set MS_CutPaste

MSG_CutPaste_0000 "XmClipboardCopy() を呼び出す前に、\n\
XmClipboardStartCopy() を呼び出さなければなりません。"

MSG_CutPaste_0001 "XmClipboardEndCopy() を呼び出す前に、\n\
XmClipboardStartCopy() を呼び出さなければなりません。"

MSG_CutPaste_0002 "XmClipboardCopy() にフォーマットが多すぎます。"

MSG_CutPaste_0003 "ClipboardBadDataType"
$ please do not translate ClipboardBadDataType.

$ "NEW-"
MSG_CutPaste_0004 "不正なデータ型です。"

MSG_CutPaste_0005 "ClipboardCorrupt"
$ please do not translate ClipboardCorrupt.

$ "NEW-"
MSG_CutPaste_0006 "内部エラーです。データ構造に矛盾があります。"

MSG_CutPaste_0007 "ClipboardBadFormat"
$ please do not translate ClipboardBadFormat

$ "NEW-"
MSG_CutPaste_0008 "エラーです。登録するフォーマットの長さは 8、16、または 32 でなければなりません。"

$ "NEW-"
MSG_CutPaste_0009 "エラーです。登録するフォーマット名は null であってはなりません。"


$set MS_DialogS

MSG_DialogS_0000 "DialogShell ウィジェットは RectObj の子を\n\
1 つサポートするだけです。"
$ please do not translate DialogShell and RectObj.

MSG_DialogS_1000 "ガジェットはシェルでは使用できません。"
$ MSG_DialogS_1000 message is obsolete - DO NOT localize this message.


$set MS_DrawingA

MSG_DrawingA_1001 "マージン幅または高さは負にはできません。"
$ MSG_DrawingA_1001 message is obsolete - DO NOT localize this message.

MSG_DrawingA_1002 "サイズ変更ポリシーが正しくありません。"
$ MSG_DrawingA_1002 message is obsolete - DO NOT localize this message.


$set MS_Form

MSG_Form_0000 "小数部分は 0 にはできません。"

MSG_Form_1000 "フォーム・アタッチメント・タイプが正しくありません。"
$ MSG_Form_1000 message is obsolete - DO NOT localize this message.

MSG_Form_1001 "非サイズ変更ウィジェットに制約条件を設定できません。"
$ MSG_Form_1001 message is obsolete - DO NOT localize this message.

MSG_Form_1002 "Attachment ウィジェットは NULL ではいけません。"
$ MSG_Form_1002 message is obsolete - DO NOT localize this message.
$ please do not translate Attachment

$ "NEW-"
MSG_Form_0002 "Form ウィジェットの子に循環する依存性があります。\n\
子の間の循環するアタッチメントをチェックしてください。"
$ please do not translate Form. 

MSG_Form_1006 "ウィジェットにエッジが接続されましたが、\n\
ウィジェットが指定されていません。"
$ MSG_Form_1006 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_Form_0003 "10000 の繰り返しの後エッジ合わせは放棄されました。\n\
この Form ウィジェットの子の矛盾したコンストレイントをチェックしてください。"
$ please do not translate Form.

MSG_Form_0004 "Attachment ウィジェットは、ウィジェットと\n\
同一の親を持たなければなりません。"
$ please do not translate Attachment
$ MSG_Form_0004 message is obsolete - DO NOT localize this message.

$set MS_Frame

MSG_Frame_1000 "枠には子を 1 つしか挿入できません。"
$ please do not translate Frame.
$ MSG_Frame_1000 message is obsolete - DO NOT localize this message.

MSG_Frame_1001 "マージン幅が無効です。"
$ MSG_Frame_1001 message is obsolete - DO NOT localize this message.

MSG_Frame_1002 "マージン高さが無効です。"
$ MSG_Frame_1002 message is obsolete - DO NOT localize this message.


$set MS_Gadget

MSG_Gadget_1000 "強調表示の厚みが無効です。"
$ MSG_Gadget_1000 message is obsolete - DO NOT localize this message.

MSG_Gadget_1001 "ユニット型が正しくありません。"
$ MSG_Gadget_1001 message is obsolete - DO NOT localize this message.

MSG_Gadget_1002 "シャドウの厚みが無効です。"
$ MSG_Gadget_1002 message is obsolete - DO NOT localize this message.

MSG_Gadget_1003 "ピックスマップ・リソースを無指定に設定できません。"
$ MSG_Gadget_1003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_Gadget_0000 "初期化後に XmNlayoutDirection を変更することはできません。"


$set MS_Label

MSG_Label_1001 "XmNlabeType が無効です。"
$ MSG_Label_1001 message is obsolete - DO NOT localize this message.

MSG_Label_1002 "XmNalignment に無効な値があります。"
$ MSG_Label_1002 message is obsolete - DO NOT localize this message.

MSG_Label_1003 "XmNstringDirection に無効な値があります。"
$ MSG_Label_1003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_Label_0003 "XmNlabelString は Compound String でなければなりません。"
$ please do not translate Compound String.

$ "NEW-"
MSG_Label_0004 "XmNacceleratorText は Compound String でなければなりません。"
$ please do not translate Compound String.


$set MS_List

$ "NEW-"
MSG_List_0000 "変更する場合、XmNvisibleItemCount は少くとも  1 でなければなりません。"

MSG_List_1001 "選択ポリシーが無効です。"
$ MSG_List_1001 message is obsolete - DO NOT localize this message.

MSG_List_1002 "サイズ・ポリシーが無効です。"
$ MSG_List_1002 message is obsolete - DO NOT localize this message.

MSG_List_1003 "スクロールバーの表示ポリシーが無効です。"
$ MSG_List_1003 message is obsolete - DO NOT localize this message.

MSG_List_1004 "文字列指示が無効です。"
$ MSG_List_1004 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_List_0005 "初期化後に XmNlistSizePolicy を変更することはできません。"

$ "NEW-"
MSG_List_0006 "変更する場合、XmNitemCount は非負の数値でなければなりません。"
$ non-negative means zero or positive.

MSG_List_1007 "SetValues の NULL フォントを無視しました。"
$ MSG_List_1007 message is obsolete - DO NOT localize this message.
$ please do not translate SetValues

$ "NEW-"
MSG_List_0007 "削除する項目がリスト中にありません。"

MSG_List_1009 "水平スクロールバーが設定されていません。"
$ MSG_List_1009 message is obsolete - DO NOT localize this message.

MSG_List_1010 "マージン設定が無効です。"
$ MSG_List_1010 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_List_0008 "XmNlistSpacing は非負の数値でなければなりません。"

$ "NEW-"
MSG_List_0009 "XmNitemCount が正の数値の場合、XmNitems を NULL に設定することができません。"

$ "NEW-"
MSG_List_0010 "XmNselectedItemCount は負の数値であってはなりません。"

$ "NEW-"
MSG_List_0011 "XmNselectedItemCount が正の数値の場合、XmNselectedItems を NULL に設定することはできません。"

$ "NEW-"
MSG_List_0012 "XmNtopItemPosition は非負の数値でなければなりません。"

MSG_List_0013 "XmNitems と XmNitemCount が一致しません。"

MSG_List_1017 "複数選択で追加モードのままにすることはできません。"
$ MSG_List_1017 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_List_0014 "XmNselectedPositionCount は負の数値であってはなりません。"

$ "NEW-"
MSG_List_0015 "XmNselectedPositionCount が正の数値の場合、XmNselectedPositions を NULL に設定することはできません。"


$set MS_MainW

$ "NEW-"
MSG_MainW_0000 "MenuBar を NULL に変更することはできません。"
$ please do not translate MenuBar.

$ "NEW-"
MSG_MainW_0001 "CommandWindow を NULL に変更することはできません。"
$ please do not translate CommandWindow.

MSG_MainW_1003 "負のマージン値を無視しました。"
$ MSG_MainW_1003 message is obsolete - DO NOT localize this message.


$set MS_MenuShell

$ "NEW-"
MSG_MenuShell_0000 "MenuShell ウィジェットは 子として XmRowColumn のみを受け付けます。"
$ please do not translate MenuShell.

$ "NEW-"
MSG_MenuShell_0001 "カスケード・ボタンにアタッチされていないプルダウン・メニューを管理しようとしています。"

$ "NEW-"
MSG_MenuShell_0002 "XmPopup には shellWidgetClass のサブクラスが必要です。"
$ please do not translate shellWidgetClass.

$ "NEW-"
MSG_MenuShell_0003 "XmPopdown には shellWidgetClass のサブクラスが必要です。"
$ please do not translate shellWidgetClass.

$ "NEW-"
MSG_MenuShell_0004 "XtMenuPopup はただひとつの引数が必要です。"

$ "NEW-"
MSG_MenuShell_0005 "XtMenuPopup ButtonPress, KeyPress および EnterNotify イベントのみをサポートします。"
$ please do not translate ButtonPress, KeyPress or EnterNotify.

$ "NEW-"
MSG_MenuShell_0006 "XtMenuPopup にポップアップ・ウィジェット \"%s\" が見つかりません。"

$ "NEW-"
MSG_MenuShell_0007 "XtMenuPopdown にポップアップ・ウィジェット \"%s\" が見つかりません。"

$ "NEW-"
MSG_MenuShell_0008 "XtMenuPopdown が複数の引数で呼び出されました。"

$ "NEW-"
MSG_MenuShell_0009 "初期化後に XmNlayoutDirection を変更することはできません。"


$set MS_MessageB

MSG_MessageB_1001 "ダイアログ・タイプが無効です。"
$ MSG_MessageB_1001 message is obsolete - DO NOT localize this message.

MSG_MessageB_1002 "デフォルト・ボタン・タイプが無効です。"
$ MSG_MessageB_1002 message is obsolete - DO NOT localize this message.

MSG_MessageB_1003 "配列タイプが無効です。"
$ MSG_MessageB_1003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_MessageB_0003 "無効な子の型です。ウィジェットはこの子をもちません。"

$ "NEW-"
MSG_MessageB_0004 "[取消し]ボタンは変更できません。"

MSG_MessageB_1006 "XmNdefaultButtonType を使用して\n\
MessageBox デフォルト・ボタンを設定してください。"
$ MSG_MessageB_1006 message is obsolete - DO NOT localize this message.


$set MS_PanedW

$ "NEW-"
MSG_PanedW_0000 "最小値は 0 よりも大きくなければなりません。"

$ "NEW-"
MSG_PanedW_0001 "最大値は 0 よりも大きくなければなりません。"

$ "NEW-"
MSG_PanedW_0002 "最小値は最大値よりも小さくなければなりません。"

MSG_PanedW_0003 "制約条件は適切なサイズを許可していません。"
$ MSG_PanedW_0003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_PanedW_0004 "サッシのコールバックのパラメータが少な過ぎます。"

$ "NEW-"
MSG_PanedW_0005 "サッシのコールバックの最初のパラメータが無効です。"


$set MS_PWidget

MSG_PWI_1001 "fontList が定義されていません。"
$ MSG_PWI_1001 message is obsolete - DO NOT localize this message.
$ please do not translate fontList.


$set MS_Protocols

$ "NEW-"
MSG_Protocols_0000 "ウィジェットは VendorShell でなければなりません。"
$ please do not translate VendorShell.

MSG_Protocols_0001 "プロトコル・マネージャはすでに存在しています。"

$ "NEW-"
MSG_Protocols_0002 "プロトコルの数はウィジェットが処理できる限界を超えています。\n\
32 が限界値です。"


$set MS_PushB

MSG_PushB_1001 "メモリが足りません。"
$ MSG_PushB_1001 message is obsolete - DO NOT localize this message.


$set MS_RowColumn

$ In this set, do not translate the word that starts with "XmN" or "Xm". 

MSG_RowColumn_1001 "幅を 0 に設定しようとしました。\n\
デフォルト値の 16 に設定します。"
$ MSG_RowColumn_1001 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_RowColumn_0000 "幅への 0 の設定は無視されました。"

MSG_RowColumn_1003 "2054-403 高さを 0 に設定しようとしました。\n\
デフォルト値の 16 に設定します。"
$ MSG_RowColumn_1003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_RowColumn_0001 "高さへの 0 の設定は無視されました。"

$ "NEW-"
MSG_RowColumn_0002 "ポップアップ・メニューで使用されていないため XmNhelpWidget には強制的に NULL が設定されました。"

$ "NEW-"
MSG_RowColumn_0003 "プルダウン・メニューで使用されていないため XmNhelpWidget には強制的に NULL が設定されました。"

$ "NEW-"
MSG_RowColumn_0004 "オプション・メニューで使用されていないため XmNhelpWidget には強制的に NULL が設定されました。"

$ "NEW-"
MSG_RowColumn_0005 "ワーク・エリアで使用されていないため XmNhelpWidget には強制的に NULL が設定されました。"

MSG_RowColumn_1009 "未知の XmNrowColumnType の値です。\n\
WorkArea に設定しました。"
$ MSG_RowColumn_1009 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_RowColumn_0007 "この XmNrowColumnType のウィジェット階層として適切ではありません。\n\
デフォルトの XmWORK_AREA が設定されました。"

$ "NEW-"
MSG_RowColumn_0008 "初期化後 XmNrowColumnType の変更は無視されます。"

MSG_RowColumn_1012 "未知の XmNorientation の値です。\n\
デフォルト値を使用しました。"
$ MSG_RowColumn_1012 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1013 "XmNorientation を未知の値に設定しようとしました。\n\
値を無視しました。"
$ MSG_RowColumn_1013 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1014 "未知の XmNpacking の値です。\n\
デフォルト値を使用しました。"
$ MSG_RowColumn_1014 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1015 "XmNpacking を未知の値に設定しようとしました。\n\
値を無視しました。"
$ MSG_RowColumn_1015 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1016 "未知の XmNentryAlignment の値です。\n\
デフォルト値を使用しました。"
$ MSG_RowColumn_1016 message is obsolete - DO NOT localize this message.

MSG_RowColumn_1017 "XmNentryAlignment を未知の値に設定しようとしました。\n\
値を無視しました。"
$ MSG_RowColumn_1017 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_RowColumn_0015 "型 XmMENU_BAR の RowColumn ウィジェットの XmNisHomogenous を FALSE に設定しようとしました。\n\
値を無視しました。"
$ please do not translate FALSE and RowColumn.

$ "NEW-"
MSG_RowColumn_0016 "型 XmMENU_BAR の RowColumn ウィジェットの XmNentryClass を設定しようとしました。\n\
値を無視しました。"
$ please do not translate RowColumn.

$ "NEW-"
MSG_RowColumn_0017 "型 XmMENU_PULLDOWN の RowColumn ウィジェットの XmNwhichButton を XtSetValues で設定しようとしました。\n\
値を無視しました。"
$ please do not translate RowColumn.

$ "NEW-"
MSG_RowColumn_0018 "型 XmMENU_PULLDOWN の RowColumn ウィジェットの XmNmenuPost を XtSetValues で設定しようとしました。\n\
値を無視しました。"
$ please do not translate RowColumn.

$ "NEW-"
MSG_RowColumn_0019 "XmNmenuPost に不正な値を設定しようとしました。値を無視しました。"

$ "NEW-"
MSG_RowColumn_0020 "型 XmMENU_PULLDOWN または XmMENU_POPUP の RowColumn ウィジェットの XmNshadowThickness を設定しようとしました。\n\
値を無視しました。"
$ please do not translate RowColumn.

MSG_RowColumn_1024 "タイプ XmMENU_OPTION の RowColumn ウィジェットの\n\
XmNorientation を変更しようとしました。値を無視しました。"
$ MSG_RowColumn_1024 message is obsolete - DO NOT localize this message.

MSG_RowColumn_0022 "不正なタイプの子をメニュー(すなわち RowColumn)\n\
ウィジェットに追加しようとしました。"

MSG_RowColumn_0023 "不正なタイプの子を同種の RowColumn ウィジェットに\n\
追加しようとしました。"

MSG_RowColumn_0025 "タイプ XmMENU_OPTION の RowColumn ウィジェットの\n\
XmNisHomogeneous を変更する試みを無視しました。"
$ please do not translate RowColumn.

$ "NEW-"
MSG_RowColumn_0026 "共有されたメニュー区画におけるティアオフの許可は推奨されていません。"

MSG_RowColumn_0027 "ニーモニック文字が不正です。\n\
X KEYSYM をキーコードに変換できません。"
$ please do not translate X KEYSYM.


$set MS_Scale

MSG_Scale_0000 "スケール最小値がスケール最大値より大きいです。\n\
あるいはスケール最大値と等しいです。"

MSG_Scale_0001 "指定のスケール値は最小スケール値より小さいです。"

MSG_Scale_0002 "指定のスケール値は最大スケール値より大きいです。"

MSG_Scale_1004 "オリエンテーションが正しくありません。"
$ MSG_Scale_1004 message is obsolete - DO NOT localize this message.

MSG_ScaleScrBar_0004 "処理の向きが正しくありません。"

MSG_Scale_0005 "強調表示の厚みが無効です。"
$ MSG_Scale_0005 message is obsolete - DO NOT localize this message.

$ "NEW-" 
MSG_Scale_0006 "無効な XmNscaleMultiple です。(最大値 - 最小値) よりも大きい値です。"

$ "NEW-"
MSG_Scale_0007 "無効な XmNscaleMultiple です。0 よりも小さい値です。"

MSG_Scale_0008 "(最大 - 最小)は INT_MAX を 2 で割った数より\n\
大きくてはいけません。最小は 0 に設定されています。\n\
最大を (INT_MAX/2) に設定しました。"

$ "NEW-"
MSG_Scale_0009 "XmNshowValue に不正な値が設定されました。"


$set MS_ScrollBar

MSG_ScrollBar_0000 "スクロールバー最小値がスクロールバー最大値より\n\
大きいです。あるいはスクロールバー最大値と等しいです。"

MSG_ScrollBar_0001 "指定のスライダ・サイズは 1 未満です。"

MSG_ScrollBar_0002 "指定のスクロールバー値が\n\
最小スクロールバー値より小さいです。"

MSG_ScrollBar_0003 "指定のスクロールバー値が、最大スクロールバー値から\n\
スクロールバー・スライダ・サイズを引いた値よりも大きいです。"

MSG_ScrollBar_1005 "オリエンテーションが正しくありません。"
$ MSG_ScrollBar_1005 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_1006 "処理の向きが正しくありません。"
$ MSG_ScrollBar_1006 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_0004 "スクロールバー増加量が 1 未満です。"

MSG_ScrollBar_0005 "スクロールバー・ページ増加量が 1 未満です。"

MSG_ScrollBar_0006 "スクロールバー初期遅延が 1 未満です。"

MSG_ScrollBar_0007 "スクロールバー周期遅延が 1 未満です。"

MSG_ScrollBar_1011 "コンテキスト・マネージャにエラーがあります。;\n\
スクロールバー・バックグラウンド・カラーが正しく設定できません。"
$ MSG_ScrollBar_1011 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_1012 "コンテキスト・マネージャにエラーがあります。;\n\
スクロールバー・フォアグラウンド・カラーが正しく設定できません。"
$ MSG_ScrollBar_1012 message is obsolete - DO NOT localize this message.

MSG_ScrollBar_0008 "2054-453 指定のスライダ・サイズが、スクロールバー最大値から\n\
スクロールバー最小値を引いた値よりも大きいです。"


$set MS_ScrolledW

MSG_ScrolledW_1001 "スクロールバー表示ポリシーが無効です。"
$ MSG_ScrolledW_1001 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1002 "スクロール・ポリシーが無効です。"
$ MSG_ScrolledW_1002 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1003 "ビジュアル・ポリシーが無効です。"
$ MSG_ScrolledW_1003 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_1004 "位置付けポリシーが無効です。"
$ MSG_ScrolledW_1004 message is obsolete - DO NOT localize this message.

MSG_ScrolledW_0004 "初期化後に、スクロール・ポリシーを変更できません。"

MSG_ScrolledW_0005 "初期化後に、ビジュアル・ポリシーを変更できません。"

MSG_ScrolledW_0006 "VARIABLE のビジュアル・ポリシーで \n\
AS_NEEDED スクロールバー・ポリシーを設定できません。"
$ please do not translate AS_NEEDED or VARIABLE.

MSG_ScrolledW_0007 "AUTOMATIC モードで、スクロールバー・ウィジェットを\n\
変更できません。"
$ please do not translate AUTOMATIC.

MSG_ScrolledW_0008 "クリップ・ウィンドウを変更できません。"

MSG_ScrolledW_0009 "APPLICATION_DEFINED モードで、CONSTANT のビジュアル・ポリシーを\n\
設定できません。"
$ please do not translate CONSTANT or APPLICATION_DEFINED.

$ "NEW-"
MSG_ScrollVis_0000 "XmScrollVisible 関数に不正なパラメータが渡されました。"


$set MS_SelectioB

MSG_SelectioB_1001 "ダイアログ・タイプが正しくありません。"
$ MSG_SelectioB_1001 message is obsolete - DO NOT localize this message.

MSG_SelectioB_0001 "Dialog タイプを変更できません。"
$ please do not translate Dialog.

MSG_SelectioB_1003 "ワーク領域の子は 1 つだけ使用できます。"
$ MSG_SelectioB_1003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_SelectioB_0002 "ウィジェットはこの子の型をサポートしていません。"


$set MS_Separator

MSG_Separator_1001 "セパレータ・タイプが無効です。"
$ MSG_Separator_1001 message is obsolete - DO NOT localize this message.

MSG_Separator_1002 "オリエンテーションが無効です。"
$ MSG_Separator_1002 message is obsolete - DO NOT localize this message.


$set MS_Text

$ "NEW-"
MSG_Text_0000 "不正なソース・テキストを無視しました。"

MSG_Text_1003 "編集モードが無効です。"
$ MSG_Text_1003 message is obsolete - DO NOT localize this message.

MSG_Text_0002 "Text widget is editable; XmNtraversalOn must be True."
$ please do not translate XmNtraversalOn and True.
$ MSG_Text_0002 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_TextIn_0000 "前の行の移動で位置が見つかりません。"

$ "NEW-"
MSG_TextOut_0000 "行数は 0 よりも大きくなければなりません。"

MSG_TextF_0002 "XmFontListInitFontContext が失敗しました。"
$ please do not translate XmFontListInitFontContext.

MSG_TextF_0003 "XmFontListGetNextFont が失敗しました。"
$ please do not translate XmFontListGetNextFont.

$ "NEW-"
MSG_TextF_0004 "文字 '%s' はフォントでサポートしていません。無視します。"

$ "NEW-"
MSG_TextFWcs_0000 "文字 '%s' はフォントでサポートしていません。無視します。"

MSG_TextFWcs_0001 "Cannot use multibyte locale without a fontset.  Value discarded."
$ MSG_TextFWcs_0001 message is obsolete - DO NOT localize this message.

$set MS_TextF

$ "NEW-"
MSG_TextF_0000 "カーソル位置は 0 よりも大きいか等しくなければなりません。"

$ "NEW-" 
MSG_TextF_0001 "列数は 0 よりも大きくなければなりません。"

MSG_TextF_0005 "XmNtraversalOn must always be True."
$ please do not translate XmNtraversalOn and True.
$ MSG_TextF_0005 message is obsolete - DO NOT localize this message.

$ "NEW-" 
MSG_TextF_0006 "列数は 0 よりも大きいか等しくなければなりません。"


$set MS_ToggleB

MSG_ToggleB_1001 "インジケータ・タイプは XmONE_OF_MANY \n\
または XmN_OF_MANY です。"
$ MSG_ToggleB_1001 message is obsolete - DO NOT localize this message.


$set MS_Traversal

MSG_Traversal_1001 "navigation_type の値が無効です。"
$ MSG_Traversal_1001 message is obsolete - DO NOT localize this message.

MSG_Traversal_1002 "navigation_type に旧式で不正な値があります。!!"
$ MSG_Traversal_1002 message is obsolete - DO NOT localize this message.

MSG_Traversal_1003 "トラバーサル・ブートストラップ状況に\n\
不正なパラメータがあります。"
$ MSG_Traversal_1003 message is obsolete - DO NOT localize this message.

MSG_Traversal_1004 "不正なパラメータを使用して、新規タブを\n\
トラバースしようとしました。"
$ MSG_Traversal_1004 message is obsolete - DO NOT localize this message.

MSG_Traversal_1005 "startWidget が子リストにありません。"
$ MSG_Traversal_1005 message is obsolete - DO NOT localize this message.
$ please do not translate startWidget

MSG_Traversal_1006 "TraverseToChild への不正なパラメータがあります。"
$ MSG_Traversal_1006 message is obsolete - DO NOT localize this message.
$ please do not translate TraverseToChild.


$set MS_Vendor

$ "NEW-"
MSG_Vendor_0000 "XmNdeleteResponse の無効な値です。"

MSG_Vendor_1002 "XmNpreeditType が無効です。OverTheSpot に設定しました。"
$ MSG_Vendor_1002 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_Vendor_0001 "XmNinputPolicy の無効な値です。"

$ "NEW-"
MSG_Vendor_0002 "XmNlayoutDirection は変更できません。"

$ "NEW-"
MSG_Vendor_0003 "致命的なエラーです。\n\
_XmGetDefaultDisplay は VendorS.Initialize よりも先に使用することはできません。NULL を返します。"


$set MS_VendorE

MSG_VendorE_1001 "FetchUnitType: 不正なウィジェット・クラス"
$ MSG_VendorE_1001 message is obsolete - DO NOT localize this message.

MSG_VendorE_0000 "String to no-op conversion needs no extra arguments"
$ MSG_VendorE_0000 message is obsolete - DO NOT localize this message.

MSG_VendorE_0005 "参照するウィジェットなしで、FetchUnitType が\n\
呼び出されました。"
$ please do not translate FetchUnitType.
$ MSG_VendorE_0005 message is obsolete - DO NOT localize this message.

$set MS_VirtKeys

MSG_VirtKey_1001 "仮想割り当て初期化が呼び出されませんでした。"
$ MSG_VirtKey_1001 message is obsolete - DO NOT localize this message.


$set MS_Visual

MSG_Visual_0000 "_XmAccessColorData で無効なカラー要求がありました。"

$ "NEW-"
MSG_Visual_0001 "デフォルトのバックグラウンドのカラーマップ・エントリを確保できません。"

$ "NEW-"
MSG_Visual_0002 "デフォルトのバックグラウンドのカラー指定を解析できません。"


$set MS_XmIm

$ "NEW-"
MSG_XmIm_0000 "入力メソッドをオープンできません。XLookupString を使用します。"

MSG_XmIm_1002 "入力方式オブジェクトを作成できません。"
$ MSG_XmIm_1002 message is obsolete - DO NOT localize this message.

MSG_XmIm_1003 "NULL ウィジェットで XmIMFocus が起動されました。"
$ MSG_XmIm_1003 message is obsolete - DO NOT localize this message.

MSG_XmIm_1004 "入力方式フォーカスなしで XmIMMove が起動されました。"
$ MSG_XmIm_1004 message is obsolete - DO NOT localize this message.

  
$set MS_Resources
  
MSG_Resource_0001 "了解"
$ "OK" as in "Go ahead"/"Confirmed to proceed as instructed"/"Fine"

MSG_Resource_0002 "取消し"
$ ... as in "Never mind"/"abort this operation"/"Stop current request"

MSG_Resource_0003 "選択"
$ ... as in "choosing from a list of items"

MSG_Resource_0004 "適用"
$ ... as in "To do something with the things selected"

MSG_Resource_0005 "ヘルプ"
$ ... as in "Need more guidance on how to deal with this dialog box"

MSG_Resource_0006 "フィルタ"
$ ... the noun "Filter", as in "a sifter/a filter/a qualifier to screen out"
$ files with

MSG_Resource_0007 "ファイル"
$ ... as in "a list of the files"

MSG_Resource_0008 "ディレクトリ"
$ ... as in "a list of the directories" 

MSG_Resource_0009 "項目"
$ ... as in "a list of things to choose from"

MSG_Resource_0010 "フィルタ"
$ ... the verb "Filter", as in "To screen out some files"
$ Note this has a slightly different semantics from MSG_Res_1006.
$ In English, it is the same word, but MSG_Res_1006 means "a sifter", and
$ MSG_Res_1010 means "Push this button to filter out some files"
$ MSG_Res_1006 is a label indicating "entry below is qualifier to filter out" 
$ files with

$ "NEW-"
MSG_Resource_0011 "ディレクトリ。"

MSG_Resource_0012 ">"
$ command-line prompt in the Command widget. 


$set MS_BaseClass

MSG_BaseClass_0000 "拡張子のコンテキストが見つかりません。"

MSG_BaseClass_0001 "_XmPopWidgetExtData: XFindContext で拡張子が見つかりませんでした。"

MSG_BaseClass_0002 "XmFreeWidgetExtData はサポート・ルーチンではありません。"
$ MSG_BaseClass_0002 message is obsolete - DO NOT localize this message.

MSG_GetSecRes_0000 "getLabelSecResData: メモリが足りません。"
$ please do not translate getLabelSecResData.
$ MSG_GetSecRes_0000 message is obsolete - DO NOT localize this message.


$set MS_Display

MSG_Display_0001 "同一Ｘディスプレイに複数の XmDisplay を作成しています。\n\
特定のＸディスプレイに作成された最初の XmDisplay だけを\n\
XmGetXmDisplay への呼び出しで参照できます。"
$ please do not translate XmDisplay or XmGetXmDisplay

MSG_Display_0002 "アクティブ DragContext がない TOP_LEVEL_LEAVE を\n\
受信しました。"
$ please do not translate TOP_LEVEL_LEAVE or DragContext

MSG_Display_0003 "XmDisplay クラスを\n\
設定できません。"
$ please do not translate XmDisplay


$set MS_DragBS

MSG_DragBS_0000 "_MOTIF_DRAG_WINDOW が破棄されました。"
$ please do not translate _MOTIF_DRAG_WINDOW

MSG_DragBS_0001 "プロトコル・バージョン・レベルが一致しません。"

MSG_DragBS_0002 "ディスプレイを開くことができません。"

MSG_DragBS_0003 "アトム・テーブルが空です。"

MSG_DragBS_0004 "ターゲット・テーブルが空です。"

MSG_DragBS_0005 "ターゲット・テーブルに一貫性がない属性があります。"

MSG_DragBS_0006 "ターゲット・テーブル索引が無効です。"


$set MS_DragC

MSG_DragC_0001 "GenerateCallback は理由として XmCR_DROP_SITE_ENTER を\n\
予期していません。"
$ please do not translate GenerateCallback or XmCR_DROP_SITE_ENTER

MSG_DragC_0002 "DropConvertCallback の選択が無効です。"
$ please do not translate DropConvertCallback

MSG_DragC_0003 "ドロップ選択が失われました。"

MSG_DragC_0004 "XGrabPointer が失敗しました。"

MSG_DragC_0005 "ExternalNotifyHandler:  コールバック・リーズンを\n\
受け入れられません。"
$ please do not translate ExternalNotifyHandler

$ "NEW-"
MSG_DragC_0006 "XmDragStart はボタン・プレスまたはモーション・イベントの結果として呼び出さなければなりません。"
$ please do not translate XmDragStart


$set MS_DragICC

MSG_DragICC_0000 "未知のドラッグ＆ドロップ・メッセージ・タイプです。"

$ "NEW-"
MSG_DragICC_0001 "プロトコルのバージョン・レベルが一致しません。"


$set MS_DragIcon

MSG_DragIcon_0000 "dragIcon ピックスマップにジオメトリが指定されていません。"
$ please do not translate dragIcon

MSG_DragIcon_0001 "dragIcon をピックスマップなしで作成しました。"
$ please do not translate dragIcon

MSG_DragIcon_0002 "String to Bitmap converter needs Screen argument"
$ MSG_DragIcon_0002 message is obsolete - DO NOT localize this message.


$set MS_DragOverS

MSG_DragOverS_0000 "深さが一致しません。"

MSG_DragOverS_0001 "未知のアイコン・アタッチメントです。"

MSG_DragOverS_0002 "未知のドラッグ状態です。"

MSG_DragOverS_0003 "未知の XmNblendModel です。"
$ please do not translate XmNblendModel 


$set MS_DragUnder

MSG_DragUnder_0000 "dropSite ウィンドウ・ジオメトリを獲得できません。"

MSG_DragUnder_0001 "animationPixmapDepth が無効です。"
$ please do not translate animationPixmapDepth


$set MS_DropSMgr

MSG_DropSMgr_0001 "シンプルなドロップ領域の子のドロップ領域を作成できません。"

MSG_DropSMgr_0002 "アクティブなドラッグ・コンテキストなしの\n\
モーション・イベントを受信しています。"

MSG_DropSMgr_0003 "アクティブなドラッグ・コンテキストなしで変更された\n\
操作を受信しています。"

MSG_DropSMgr_0004 "ドロップ・プロシージャなしのアクティブ・ドロップ\n\
領域を作成中です。"

MSG_DropSMgr_0005 "作成ドロップ領域の矩形または矩形番号を設定できません。"

MSG_DropSMgr_0006 "シーケンスにないドロップ領域として、ウィジェットを\n\
登録しました。子孫を登録する前に、先祖を登録しなければなりません。"

MSG_DropSMgr_0007 "2 回以上ドロップ領域として、ウィジェットを登録できません。"

MSG_DropSMgr_0008 "DropSite タイプは作成時にのみ設定できます。"
$ please do not translate DropSite

MSG_DropSMgr_0009 "非シンプル・ドロップ領域の矩形を変更できません。"

$ "NEW-"
MSG_DropSMgr_0010 "Shell をドロップ・サイトとして登録できません。"

MSG_DropSMgrI_0001 "シンプル・ドロップ領域の子孫のドロップ領域を\n\
登録できません。"

MSG_DropSMgrI_0002 "作成ドロップ領域の不連続の子のリストを作成できません。"

MSG_DropSMgrI_0003 "%s は %s の子のドロップ領域ではありません。"


$set MS_GeoUtils

MSG_GeoUtils_0000 "Failure of geometry request to \"almost\" reply"
$ please do not translate "almost"
$ MSG_GeoUtils_0000 message is obsolete - DO NOT localize this message.

MSG_GeoUtils_0001 "Invalid layout of children found"
$ MSG_GeoUtils_0001 message is obsolete - DO NOT localize this message.

MSG_GeoUtils_0002 "Invalid order found in XmSelectionBox"
$ MSG_GeoUtils_0002 message is obsolete - DO NOT localize this message.


$set MS_Region

MSG_Region_0000 "メモリ・エラー"


$set MS_RepType

MSG_RepType_0001 "リプレゼンテーション型識別番号が不正です。"

MSG_RepType_0002 "%d はリプレゼンテーション型 XmR%s の不正な値です。"
$ please do not translate XmR%s

$ "NEW-"
MSG_RepType_0000 "の逆変換"


$set MS_ResConvert

MSG_ResConvert_0000 "FetchUnitType: 不正なウィジェット・クラスです。"
$ please do not translate FetchUnitType.
$ MSG_ResConvert_0000 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_ResConvert_0001 "不適切に定義されたデフォルトのリストです。終了します。"

$ "NEW-"
MSG_ResConvert_0002 "フォント文字列 \"%s\" にコロンがありません。\"%s\" リストの他のフォントは解析しません。"

$ "NEW-"
MSG_ResConvert_0003 "タグ \"%s\" に無効なデリミタがあります。リストの他のフォントは解析しません。"

MSG_ResConvert_0004 "Unmatched quotation marks in string \"%s\"; any remaining fonts in list unparsed"
$ MSG_ResConvert_0004 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_ResConvert_0005 "タグ \"%s\" の引用符が対応しません。 リストの他のフォントは解析しません。"

$ "NEW-"
MSG_ResConvert_0006 "型 %s に変換するとき Null のタグが見つかりました。リストの他のフォントは解析しません。"

$ "NEW-"
MSG_ResConvert_0007 "XmString をコンパウンド・テキストに変換できません。"

$ "NEW-"
MSG_ResConvert_0008 "XmbTextListToTextProperty のためのメモリーが不十分です。"

$ "NEW-"
MSG_ResConvert_0009 "XmbTextListToTextProperty のためのロケールはサポートされていません。"

$ "NEW-"
MSG_ResConvert_0010 "XmbTextListToTextProperty が失敗しました。"

$ "NEW-"
MSG_ResConvert_0011 "ウィジェット名を Widget に変換できません。"
$ please do not translate Widget.

$ "NEW-"
MSG_ResConvert_0012 "コンパウンド・テキストを XmString に変換できません。"

$ "NEW-"
MSG_ResConvert_0013 "XmString をコンパウンド・テキストに変換できません。"

MSG_ResConvert_0014 "FetchUnitType called without a widget to reference"
$ please do not translate FetchUnitType.
$ MSG_ResConvert_0014 message is obsolete - DO NOT localize this message

MSG_ResConvert_0015 "FetchDisplayArg called without a widget to reference"
$ please do not translate FetchDisplayArg.
$ MSG_ResConvert_0015 message is obsolete - DO NOT localize this message

MSG_ResConvert_0016 "FetchWidgetArg called without a widget to reference"
$ please do not translate FetchWidgetArg.
$ MSG_ResConvert_0016 message is obsolete - DO NOT localize this message


$set MS_Screen

MSG_Screen_0000 "アイコン画面が一致しません。"

MSG_Screen_0001 "XmDisplay が見つからないので、XmScreen を獲得できません。"


$set MS_ColObj

MSG_ColObj_0001 "カラー・オブジェクト・データのメモリを割り当てられません。"

MSG_ColObj_0002 "カラー・サーバ選択からの不正な画面番号があります。"

 
$set MS_IG
 
MSG_IG_1 "配列が正しくありません。"
$ MSG_IG_1 message is obsolete - DO NOT localize this message.

MSG_IG_2 "動作が正しくありません。"
$ MSG_IG_2 message is obsolete - DO NOT localize this message.

MSG_IG_3 "塗りつぶしモードが正しくありません。"
$ MSG_IG_3 message is obsolete - DO NOT localize this message.

MSG_IG_4 "文字列またはピックスマップが正しくありません。"
$ MSG_IG_4 message is obsolete - DO NOT localize this message.

MSG_IG_5 "マージン幅または高さが正しくありません。"
$ MSG_IG_5 message is obsolete - DO NOT localize this message.

MSG_IG_6 "シャドウ・タイプが正しくありません。"
$ MSG_IG_6 message is obsolete - DO NOT localize this message.


$set MS_ComboBox

$ "NEW-"
MSG_ComboBox_0000 "アプリケーションは XmComboBox ウィジェットに子を追加することはできません。"

$ "NEW-"
MSG_ComboBox_0001 "XmNcomboBoxType リソースは XtSetValues で変更できません。"

MSG_ComboBox_0002 "XmFontListGetNextFont failed."
$ MSG_ComboBox_0002 message is obsolete - DO NOT localize this message.

MSG_ComboBox_0003 "XmFontListInitFontContext failed."
$ MSG_ComboBox_0003 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_ComboBox_0004 "内部ウィジェットが破棄されました。動作は未定義です。"

$ "NEW-"
MSG_ComboBox_0005 "内部ウィジェットが管理からはずされました。動作は未定義です。"

$ "NEW-"
MSG_ComboBox_0006 "XmQUICK_NAVIGATE は XmNcomboBoxType が XmDROP_DOWN_LIST の ComboBox でのみ有効です。"

$ "NEW-"
MSG_ComboBox_0007 "不正なパラメータ数でアクションが起動されました。"

$ "NEW-"
MSG_ComboBox_0008 "アクション・ルーチンが ComboBox の子孫ではないウィジェットから呼び出されました。"

$ "NEW-"
MSG_ComboBox_0009 "XmComboBoxSelectItem が ComboBox にない項目で呼び出されました。"

$ "NEW-"
MSG_ComboBox_0010 "XmComboBoxSetItem が ComboBox にない項目で呼び出されました。"

$ "NEW-"
MSG_ComboBox_0011 "XmComboBoxDeletePos が無効な位置で呼び出されました。"

$ "NEW-"
MSG_ComboBox_0012 "XmComboBox ユーティリティ・ルーチンが無効なウィジェットで呼び出されました。"

$ "NEW-"
MSG_ComboBox_0013 "アプリケーションは自動的な XmComboBox ウィジェットの子を設定できません。"

$ "NEW-"
MSG_ComboBox_0014 "XmComboBox の positionMode を作成後に変更することはできません。"
$ please do not translate positionMode.


$set MS_Container

$ "NEW-"
MSG_Container_0000 "アクションが不正なパラメータ数で起動されました。"

$ "NEW-"
MSG_Container_0001 "XmNdetailColumnHeading と XmNdetailColumnHeadingCount が一致しません。"


$set MS_CSText

MSG_CSText_0000 "Invalid margin height; must be greater than or equal to 0."
$ MSG_CSText_0000 message is obsolete - DO NOT localize this message.

MSG_CSText_0001 "Invalid margin width; must be greater than or equal to 0."
$ MSG_CSText_0001 message is obsolete - DO NOT localize this message.

MSG_CSText_0002 "Invalid edit mode."
$ MSG_CSText_0002 message is obsolete - DO NOT localize this message.

MSG_CSText_0003 "XmNtraversalOn must always be true."
$ MSG_CSText_0003 message is obsolete - DO NOT localize this message.

MSG_CSText_0004 "Cannot change XmNscrollHorizontal after initialization."
$ MSG_CSText_0004 message is obsolete - DO NOT localize this message.

MSG_CSText_0005 "Cannot change XmNscrollVertical after initialization."
$ MSG_CSText_0005 message is obsolete - DO NOT localize this message.

MSG_CSText_0006 "Cannot change XmNscrollTopSide after initialization."
$ MSG_CSText_0006 message is obsolete - DO NOT localize this message.

MSG_CSText_0007 "Cannot change XmNscrollLeftSide after initialization."
$ MSG_CSText_0007 message is obsolete - DO NOT localize this message.


$set MS_GrabShell

MSG_GrabS_0000 "XmPopup requires a subclass of shellWidgetClass."
$ please do not translate shellWidgetClass.
$ MSG_GrabS_0000 message is obsolete - DO NOT localize this message.


$set MS_Manager

$ "NEW-"
MSG_Manager_0000 "ウィジェット・クラス %s に無効な CompositeClassExtension レコードがあります。"
$ please do not translate CompositeClassExtension.

$ "NEW-"
MSG_Manager_0001 "初期化後に XmNlayoutDirection や XmNstringDirection を変更することはできません。"


$set MS_NavigMap

MSG_NavigMap_0000 "_XmNavigate called with invalid direction"
$ MSG_NavigMap_0000 message is obsolete - DO NOT localize this message.


$set MS_Notebook

$ "NEW-"
MSG_Notebook_0000 "XmNnotebookChildType リソースは XtSetValues で設定できません。"


$set MS_PixConv

$ "NEW-"
MSG_PixConv_0000 "CvtStringToPixmap のパラメータ数が不正です。"
$ please do not translate CvtStringToPixmap.


$set MS_Primitive

$ "NEW-"
MSG_Primitive_0000 "初期化後に XmNlayoutDirection を変更することはできません。"


$set MS_ScrollFrameT

$ "NEW-"
MSG_ScrollFrameT_0000 "AssocNavigator にはナビゲータ・トレイトが必要です。"
$ please do not translate AssocNavigator.

$ "NEW-"
MSG_ScrollFrameT_0001 "DeAssocNavigator にはナビゲータ・トレイトが必要です。"
$ please do not translate DeAssocNavigator.


$set MS_SpinB

MSG_SpinB_0001 "Invalid value for XmNarrowLayout."
$ MSG_SpinB_0001 message is obsolete - DO NOT localize this message.

MSG_SpinB_0002 "XmNminimumValue equals XmNmaximumValue."
$ MSG_SpinB_0002 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_SpinB_0003 "XmSTRING の子に項目がありません。"

$ "NEW-"
MSG_SpinB_0004 "XmNincrementValue は 0 であってはなりません。値 1 を使用します。"

$ "NEW-"
MSG_SpinB_0005 "XmNincrementValue で指定されたスピン方向を、指定された XmNminimumValue と XmNmaximumValue と一致させるため反転しました。"

$ "NEW-"
MSG_SpinB_0006 "XmNposition は範囲外です。最小の XmNposition を使用します。"

$ "NEW-"
MSG_SpinB_0007 "XmNposition は範囲外です。最大の XmNposition を使用します。"

$ "NEW-"
MSG_SpinB_0008 "XmNpositionType の無効な値です。デフォルト値を使用します。"

MSG_SpinB_0009 "XmNpositionType resource can only be set at creation time."
$ MSG_SpinB_0009 message is obsolete - DO NOT localize this message.


$set MS_Transfer

$ "NEW-"
MSG_Transfer_0000 "転送が終了すべきときの SelectionCallbackWrapper が呼び出しです。"
$ please do not translate SelectionCallbackWrapper.

MSG_Transfer_0001 "Cannot lock the clipboard; aborting transfer"
$ MSG_Transfer_0001 message is obsolete - DO NOT localize this message.

$ "NEW-"
MSG_Transfer_0002 "コールバックが提供したデータのフォーマットと型はマージされるデータと一致しません。"

$ "NEW-"
MSG_Transfer_0003 "XmConvertCallbackStruct のステータスは XmCONVERT_MERGE ではありません。"

$ "NEW-"
MSG_Transfer_0004 "CONVERT_MORE はサポートしていません。"
$ please do not translate CONVERT_MORE.

$ "NEW-"
MSG_Transfer_0005 "不正なアトム値が見つかりました。"

$ "NEW-"
MSG_Transfer_0006 "警告: 転送実行中に MULTIPLE 転送を開始しようとしました。"
$ please do not translate MULTIPLE.

$ "NEW-"
MSG_Transfer_0007 "警告: 転送未実行時に MULTIPLE 転送を送信しようとしました。"
$ please do not translate MULTIPLE.


$set MS_VaSimple

$ "NEW-"
MSG_VaSimple_0000 "XtVaTypedArg の変換には null でないウィジェット・ハンドルが必要です。"

$ "NEW-"
MSG_VaSimple_0001 "変換のためのリソースの型がみつか見つかりません。"

$ "NEW-"
MSG_VaSimple_0002 "型の変換に失敗しました。"


$set MS_Xm

$ "NEW-"
MSG_Motif_0000 "\n名前: %s\nクラス: %s\n"

$ "NEW-"
MSG_Motif_0001 "アクションが不正なパラメータ数で起動されました。"


$set MS_XmRenderT

$ "NEW-"
MSG_XmRenderT_0000 "XmNtag を NULL にすることはできません。空の文字列を設定します。"

$ "NEW-"
MSG_XmRenderT_0001 "ディスプレイが NULL です。フォントをロードできません。"

$ "NEW-"
MSG_XmRenderT_0002 "XmNfontType が無効です。フォントをロードできません。"

$ "NEW-"
MSG_XmRenderT_0003 "変換に失敗しました。フォントをロードできませせん。"

$ "NEW-"
MSG_XmRenderT_0004 "XmNfontType が XmAS_IS に設定されました。フォントをロードできません。"

$ "NEW-"
MSG_XmRenderT_0005 "XmNloadModel が XmLOAD_IMMEDIATE に設定されていますが XmNfont と XmNfontName が指定されていません。\n\
フォントをロードできません。"


$set MS_XmSelect

MSG_XmSelect_0000 "Internal error: no selection property context for display"
$ MSG_XmSelect_0000 message is obsolete - DO NOT localize this message.

MSG_XmSelect_0001 "Selection owner returned type INCR property with format != 32"
$ please do not translate INCR.
$ MSG_XmSelect_0001 message is obsolete - DO NOT localize this message.

MSG_XmSelect_0002 "XtGetSelectionRequest called for widget \"%s\" outside of ConvertSelection proc"
$ please do not translate ConvertSelection.
$ MSG_XmSelect_0002 message is obsolete - DO NOT localize this message.


$set MS_XmString

$ "NEW-"
MSG_XmString_0000 "フォントが見つかりません。"


$set MS_XmTabList

$ "NEW-"
MSG_XmTabList_0000 "タブの値を負の数値にすることはできません。"


$set MS_SSpinB

$ "NEW-"
MSG_SSpinB_0001 "XmNtextField リソースを設定することはできません。"

$ "NEW-"
MSG_SSpinB_0002 "XmNpositionType リソースは作成時のみ設定することができます。"

$ "NEW-"
MSG_SSpinB_0003 "項目が存在しません。XmNposition は変更しませんでした。"
