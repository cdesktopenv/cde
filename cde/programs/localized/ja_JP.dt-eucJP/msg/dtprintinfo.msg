$ $XConsortium: dtprintinfo.msg /main/7 1996/10/22 04:08:33 cde-hit $
$quote "
$set DTPRINTER_SET

$ ----- These are the titles that appear on dialogs and the main window -----
ApplicationName1L  "印刷マネージャ"
ApplicationName2L  "プリンタ・ジョブ"
$ Do not translate ApplicationName3L for this release
ApplicationName3L  "Desktop Printers"

$ ----- Common Dialog Buttons -----
YesL     "はい"
NoL      "いいえ"
OKL      "了解"
ApplyL   "適用"
CancelL  "取消し"
HelpL    "ヘルプ"

$ ----- Find dialog -----
FindTitleL        "%s - 印刷ジョブ検索"
MyJobNameL        "ユーザのジョブの名前"
OnlyRootJobsL     "root のジョブのみ"
IgnoreCaseL       "大／小文字の区別なし"
ExactMatchL       "区別あり"
GotoL             "行先指定"
CancelPrintJobsL  "印刷ジョブの取消し"
PositionL         "位置"
CloseL            "閉じる"
StartFindL        "検索開始"
StopFindL         "検索中止"
SearchL           "\
%s を検索中です...\n\
\n\
中止するには、中止を押してください。"

$ Find Information dialog messages
NoMatchesL             "一致する印刷ジョブはありません。"

NotFoundMessageL       "\
もはや印刷ジョブは、ありません。\n\
または、プリンタが動いていません。"

$ ----- Modify Show Dialog -----
ModifyShowTitleL         "%s - 表示リストの変更"
SelectPrintersToBeShowL  "表示するプリンタを選択してください。"
SelectAllL               "すべてを選択"
DeselectAllL             "選択をすべて解除"

$ ----- Set Options Dialog -----
SetOptionsTitleL      "%s - オプションの設定"
RepresentationL       "表示方法"
LargeIconL            "大型アイコン"
SmallIconL            "小型アイコン"
NameOnlyL             "名前のみ"
DetailsL              "詳細"
ShowLabelsL           "ラベルを表示"
JobsToShowL           "表示するジョブ"
OnlyMineL             "ユーザのジョブのみ"
EveryoneL             "全員のジョブ"
StatusL               "ステータス"
ProblemFlagL          "障害フラグ"
InformationLineL      "メッセージ行"
UpdateIntervalL       "更新"
IntervalL             "間隔(秒単位)"

$ ----- Print Job Properties Dialog -----
PrintJobPropsTitleL  "%s - 印刷ジョブ属性"
PrinterL             "プリンタ"
JobName1L            "ジョブ名"
SubmittedL           "送信"
BytesL               "バイト数"
Position1L           "位置:"
JobNameL             "ジョブ名:"
JobNumberL           "ジョブ番号:"
OwnerL               "所有者:"
DateL                "日付:"
TimeL                "時間:"
SizeL                "サイズ:"

$ ----- Printer Properties Dialog -----
PrinterPropsTitleL    "%s - プリンタ属性"
IconLabelL            "アイコン・ラベル:"
IconGroupTitleL       "アイコン:"
FindSetL              "アイコンセット検索..."
DescriptionL          "説明:"
PrinterStatusL        "ステータス:"
PrintQueueL           "プリンタ・キュー:"
DeviceL               "デバイス名:"
DeviceNL              "デバイス %d:"
UpL                   "Up - サービス中"
DownL                 "Down - サービス停止中"
PrinterOnServerL      "%2$s 上の %1$s"
DefaultDescriptionL   "これは %s プリンタ・キューです。"
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"
$ This is the value of the device field when the printer is on a remote system

$ ----- Find Set Dialog -----
FindSetTitleL       "%s - アイコンセット検索"
IconFoldersL        "アイコン・フォルダ:"
IconTitleL          "アイコン"
LoadingIconsL       "%2$d のアイコン %1$d を読み込んでいます..."
$ %1 is a counter and %2 is the total number of icons

$ ----- "Cancel" Print Job Confirmation Dialog -----
CancelTitleL        "%s - %s の取消し"
CancelQuestionL     "%s を取り消してもいいですか?"

$ ----- Printer "Status" Dialog -----
PrinterStatusTitleL  "%s - プリンタ・ステータス"
PrinterDownL         "%s プリンタ・キューはサービスしていません。"
DeviceDownL          "%s デバイスはサービスしていません。"

$ ----- Failed Action Dialog -----
FailedActionTitleL  "%s - %s が異常終了しました。"
FailedMessageL      "%s に '%s' アクションを適用できません。\n\
  以下の理由を参照してください。\n\n%s"

$ ----- Main Window Information Line Messages -----
GettingHelpL      "オンライン・ヘルプ..."
SearchingL        "プリンタの検索..."
ShowingMyJobsL    "ユーザのジョブのみ表示"
PrintersHiddenL   "プリンタ %d 個  隠しファイル %d 個"
JobsL             "ジョブ %d 個"
UpdatingActionsL  "アクション・データベースの更新..."
UpdatingL         "%s を更新中です..."
$ Do not translate EditLangL for this release
EditLangL         "Editing in /etc/dt/appconfig/types/%s"
$ %s is the name of the printer

$ ----- Printer Menu Messages -----
PrinterMenuL         "プリンタ(P)"
PrinterAcceleratorL  "P"
FindChoiceL          "検索(F)..."
FindMnemonicL        "F"
FindAcceleratorL     "Ctrl+F"
ExitChoiceL          "終了(x)"
ExitMnemonicL        "x"
ExitAcceleratorL     "Alt+F4"

$ ----- Selected Menu Messages -----
SelectedMenuL           "選択(S)"
SelectedAcceleratorL    "S"

$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.
$			This choice pops up the "Cancel" Print Job Confirmation
$                       Dialog. 
CancelChoiceL           "取消し(C)"
CancelMnemonicL         "C"
CancelAcceleratorL      "Del"

$ For Printers Only
OpenChoiceL             "開く(O)"
OpenMnemonicL           "O"
CloseChoiceL            "閉じる(C)"
CloseMnemonicL          "C"
HideChoiceL             "隠す(e)"
HideMnemonicL           "e"
$ Do not translate RenameChoiceL for this release
RenameChoiceL           "Rename"
$ Do not translate RenameMnemonicL for this release
RenameMnemonicL         "R"
$ Do not translate StartChoiceL for this release
StartChoiceL            "Start queuing"
$ Do not translate StartMnemonicL for this release
StartMnemonicL          "S"
$ Do not translate StopChoiceL for this release
StopChoiceL             "Stop queuing"
$ Do not translate StopMnemonicL for this release
StopMnemonicL           "t"
$ Do not translate EnableChoiceL for this release
EnableChoiceL           "Enable printing"
$ Do not translate EnableMnemonicL for this release
EnableMnemonicL         "E"
$ Do not translate DisableChoiceL for this release
DisableChoiceL          "Disable printing"
$ Do not translate DisableMnemonicL for this release
DisableMnemonicL        "D"

$ For Both Printers and Print Jobs
PropertiesChoiceL       "属性(P)..."
PropertiesMnemonicL     "P"
PropertiesAcceleratorL  "Ctrl+Backspace"
HelpChoiceL             "ヘルプ(H)"
HelpMnemonicL           "H"

$ ----- View Menu Messages -----
ViewMenuL             "表示(V)"
ViewAcceleratorL      "V"
SetOptionsChoiceL     "オプションの設定(O)..."
SetOptionsMnemonicL   "O"
ModifyShowChoiceL     "表示リストの変更(M)..."
ModifyShowMnemonicL   "M"

$ ----- Help Menu Messages -----
$ "Help" Menu uses the "HelpChoiceL" message
$ "Help" Accelerator uses the "HelpMnemonicL" message
OverviewChoiceL     "概要(v)"
OverviewMnemonicL   "v"
TaskChoiceL         "使い方(T)"
TaskMnemonicL       "T"
ReferenceChoiceL    "リファレンス(R)"
ReferenceMnemonicL  "R"
OnItemChoiceL       "アイテムヘルプ(O)"
OnItemMnemonicL     "O"
UsingHelpChoiceL    "ヘルプの使い方(U)"
UsingHelpMnemonicL  "U"
AboutChoice1L       "印刷マネージャについて(A)"
AboutChoice2L       "プリンタ・ジョブについて(A)"
$ Do not translate AboutChoice3L for this release
AboutChoice3L       "About Desktop Printers"
AboutMnemonicL      "A"

$ ----- Misc Messages -----
EmptyL              "[ 空 ]"
FoundNoPrintersL    "プリンタが見つかりません。"
NotAvailableL       "使用できません。"
NoDefaultPrinter1L  "システムのデフォルト・プリンタはありません。"
NoDefaultPrinter2L  "\
デフォルト・プリンタの LPDEST 環境の設定方法については、\n\
Users Guide を参照してください。"
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.
RootUserL "%1$s: %2$s オプションを使用するには、\n\
root ユーザでなければなりません。"


$ ----- Command Line Help -----
$ This message is show when the user enters 'dtprintinfo -help'
$ on the command line. The %s is a printer list generated by the program.

CommandLineHelpL "\
dtprintinfo\n\
\n\
    dtprintinfo をオプションなしで起動すると、LPDEST プリンタ\n\
    または、LPDEST が設定されていない場合にはシステムのデフォルト\n\
    を表示します。\n\
\n\
dtprintinfo -p <printer>\n\
\n\
    特定のプリンタを表示するには、-p オプションと\n\
    以下の印刷キュー名の 1 つを使用してください:\n\
\n\
%s\
\n\
dtprintinfo -all\n\
\n\
    すべてのプリンタを表示するには、-all オプションを使用してください。\n\
\n\
dtprintinfo -populate\n\
\n\
    -populate オプションは、root ユーザだけが使用できます。\n\
    このオプションは、すべてのプリンタのデフォルトのプリンタ・アクションを\n\
    /etc/dt/appconfig/types/LANG に作成します。LANG が定義されていない場合\n\
    には、アクションは/etc/dt/appconfig/types/C に作成されます。"

NoRemoteCancelL "印刷サーバは印刷ジョブ番号を供給していません。\n\
いくつかの印刷サーバは印刷ジョブのリモート取消しを許しません。\n\
もしこのやり方を知りたいのならば、印刷サーバ管理者に連絡をとってください。"

FindAcceleratorKeySeq "Ctrl<Key>f"
