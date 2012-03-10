$ $XConsortium: dtprintinfo.msg /main/2 1996/07/24 19:01:49 drk $
$quote "
$set DTPRINTER_SET

$ ----- These are the titles that appear on dialogs and the main window -----
ApplicationName1L  "打印管理器"
ApplicationName2L  "打印作业"
$ Do not translate ApplicationName3L for this release
ApplicationName3L  "Desktop Printers"

$ ----- Common Dialog Buttons -----
YesL     "是"
NoL      "否"
OKL      "确认"
ApplyL   "作用"
CancelL  "取消"
HelpL    "提示"

$ ----- Find dialog -----
FindTitleL        "%s - 查找打印作业"
MyJobNameL        "我的作业名"
OnlyRootJobsL     "仅根作业"
IgnoreCaseL       "忽略大小写"
ExactMatchL       "确切匹配"
GotoL             "转至"
CancelPrintJobsL  "取消打印作业"
PositionL         "位置"
CloseL            "关闭"
StartFindL        "开始查找"
StopFindL         "停止查找"
SearchL           "\
正在搜索 %s...\n\
\n\
按‘停止’中断操作"

$ Find Information dialog messages
NoMatchesL             "没有匹配的打印作业"

NotFoundMessageL       "\
该打印作业不再存在或者\n\
打印机关闭了。"

$ ----- Modify Show Dialog -----
ModifyShowTitleL         "%s - 修改显示列表"
SelectPrintersToBeShowL  "选择要显示的打印机"
SelectAllL               "选择所有的"
DeselectAllL             "去除所有的选择"

$ ----- Set Options Dialog -----
SetOptionsTitleL      "%s - 设置选项"
RepresentationL       "表示"
LargeIconL            "大图标"
SmallIconL            "小图标"
NameOnlyL             "仅名字"
DetailsL              "细节"
ShowLabelsL           "显示标号"
JobsToShowL           "要显示的作业"
OnlyMineL             "仅我的"
EveryoneL             "所有人的"
StatusL               "状态"
ProblemFlagL          "问题标志"
InformationLineL      "消息行"
UpdateIntervalL       "更新"
IntervalL             "间隔 (按秒计)"

$ ----- Print Job Properties Dialog -----
PrintJobPropsTitleL  "%s - 打印作业特性"
PrinterL             "打印机"
JobName1L            "作业名"
SubmittedL           "提交的"
BytesL               "字节"

Position1L           "位置:"
JobNameL             "作业名:"
JobNumberL           "作业号:"
OwnerL               "属主:"
DateL                "日期:"
TimeL                "时间:"
SizeL                "大小:"

$ ----- Printer Properties Dialog -----
PrinterPropsTitleL    "%s - 打印机特性"
IconLabelL            "图标标号:"
IconGroupTitleL       "图标:"
FindSetL              "查找设置..."
DescriptionL          "描述:"
PrinterStatusL        "状态:"
PrintQueueL           "打印机队列:"
DeviceL               "设备名:"
DeviceNL              "设备 %d:"
UpL                   "上 - 在服务"
DownL                 "下 - 不在服务"
PrinterOnServerL      "%1$s 在 %2$s 上"
DefaultDescriptionL   "这是 %s 打印机队列。"
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"
$ This is the value of the device field when the printer is on a remote system

$ ----- Find Set Dialog -----
FindSetTitleL       "%s - 查找设置"
IconFoldersL        "图标文件夹:"
IconTitleL          "图标"
LoadingIconsL       "装载图标 %1$d 个（共 %2$d 个）..."
$ %1 is a counter and %2 is the total number of icons

$ ----- "Cancel" Print Job Confirmation Dialog -----
CancelTitleL     "%s - 取消 %s"
CancelQuestionL  "你确实要取消 %s 吗ⅶ"

$ ----- Printer "Status" Dialog -----
PrinterStatusTitleL  "%s - 打印机状态"
PrinterDownL         "%s 打印机队列不在服务。"
DeviceDownL          "%s 设备不在服务。"

$ ----- Failed Action Dialog -----
FailedActionTitleL  "%s - %s 失败"
FailedMessageL      "不能将动作‘%s’作用到\n\
  %s。参见下面的原因。\n\n%s"

$ ----- Main Window Information Line Messages -----
GettingHelpL      "获得联机提示..."
SearchingL        "搜索打印机..."
ShowingMyJobsL    "仅显示我的作业"
PrintersHiddenL   "%d 个打印机 %d 个隐藏"
JobsL             "%d 个作业"
UpdatingActionsL  "更新动作数据库..."
UpdatingL         "更新 %s..."
$ Do not translate EditLangL for this release
EditLangL         "Editing in /etc/dt/appconfig/types/%s"
$ %s is the name of the printer

$ ----- Printer Menu Messages -----
PrinterMenuL         "打印机[P]"
PrinterAcceleratorL  "P"
FindChoiceL          "查找[F]..."
FindMnemonicL        "F"
FindAcceleratorL     "Ctrl+F"
ExitChoiceL          "退出[x]"
ExitMnemonicL        "x"
ExitAcceleratorL     "Alt+F4"

$ ----- Selected Menu Messages -----
SelectedMenuL           "选择的[S]"
SelectedAcceleratorL    "S"

$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.
$                       This choice pops up the "Cancel" Print Job Confirmation
$                       Dialog.
CancelChoiceL           "取消[C]"
CancelMnemonicL         "C"
CancelAcceleratorL      "Del"

$ For Printers Only
OpenChoiceL             "打开[O]"
OpenMnemonicL           "O"
CloseChoiceL            "关闭[C]"
CloseMnemonicL          "C"
HideChoiceL             "隐藏[e]"
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
PropertiesChoiceL       "特性[P]..."
PropertiesMnemonicL     "P"
PropertiesAcceleratorL  "Ctrl+退格键"
HelpChoiceL             "提示[H]"
HelpMnemonicL           "H"

$ ----- View Menu Messages -----
ViewMenuL             "观看[V]"
ViewAcceleratorL      "V"
SetOptionsChoiceL     "设置选项[O]..."
SetOptionsMnemonicL   "O"
ModifyShowChoiceL     "修改显示列表[M]..."
ModifyShowMnemonicL   "M"

$ ----- Help Menu Messages -----
$ "Help" Menu uses the "HelpChoiceL" message
$ "Help" Accelerator uses the "HelpMnemonicL" message
OverviewChoiceL     "概述[v]"
OverviewMnemonicL   "v"
TaskChoiceL         "任务[T]"
TaskMnemonicL       "T"
ReferenceChoiceL    "参考[R]"
ReferenceMnemonicL  "R"
OnItemChoiceL       "针对项[O]"
OnItemMnemonicL     "O"
UsingHelpChoiceL    "使用提示[U]"
UsingHelpMnemonicL  "U"
AboutChoice1L       "关于打印管理器"
AboutChoice2L       "关于打印机作业"
$ Do not translate AboutChoice3L for this release
AboutChoice3L       "About Desktop Printers"
AboutMnemonicL      "A"

$ ----- Misc Messages -----
EmptyL            "[ 空 ]"
FoundNoPrintersL  "没有找到打印机。"
NotAvailableL     "无可用的"
NoDefaultPrinter1L  "没有系统默认打印机。"
NoDefaultPrinter2L  "\
参见〈用户指南〉如何设置\n\
默认打印机 LPDEST 环境。"
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.
RootUserL "%1$s: 你必须是根才可使用 %2$s 选项。"


$ ----- Command Line Help -----
$ This message is show when the user enters 'dtprintinfo -help'
$ on the command line. The %s is a printer list generated by the program.

CommandLineHelpL "\
dtprintinfo\n\
\n\
	启动 dtprintinfo 不带任何选项，将显示 LPDEST 打印机，\n\
	或者在 LPDEST 没有设置时显示系统默认打印机。\n\
\n\
dtprintinfo -p <打印机>\n\
\n\
	要查看一个特定的打印机，使用 -p 选项和下列打印\n\
	队列名之一：\n\
\n\
%s\
\n\
dtprintinfo -all\n\
\n\
	要查看所有的打印机，使用 -all 选项。\n\
\n\
dtprintinfo -populate\n\
\n\
	只有根才可使用 -populate 选项。此选项对在\n\
	/etc/dt/appconfig/types/LANG 中的所有的打印机\n\
	创建默认打印机动作。如果 LANG 没有定义，动作就\n\
	创建到 /etc/dt/appconfig/types/C 中。"
