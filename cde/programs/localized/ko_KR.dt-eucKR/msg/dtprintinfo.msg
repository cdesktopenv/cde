$ $XConsortium: dtprintinfo.msg /main/2 1996/07/24 18:58:04 drk $
$quote "
$set DTPRINTER_SET

$ ----- These are the titles that appear on dialogs and the main window -----
ApplicationName1L  "인쇄 관리자"
ApplicationName2L  "프린터 작업"
$ Do not translate ApplicationName3L for this release
ApplicationName3L  "데스크탑 프린터"

$ ----- Common Dialog Buttons -----
YesL     "예"
NoL      "아니오"
OKL      "예"
ApplyL   "적용"
CancelL  "취소"
HelpL    "도움말"

$ ----- Find dialog -----
FindTitleL        "%s - 인쇄 작업 찾기"
MyJobNameL        "작업 이름"
OnlyRootJobsL     "루트 작업만"
IgnoreCaseL       "대/소문자 무시"
ExactMatchL       "정확하게 일치"
GotoL             "찾아가기"
CancelPrintJobsL  "인쇄 작업 취소"
PositionL         "위치"
CloseL            "닫기"
StartFindL        "찾기 시작"
StopFindL         "찾기 멈춤"
SearchL           "\
Searching %s...\n\
\n\
중단하려면 멈춤을 누르십시오."

$ Find Information dialog messages
NoMatchesL             "맞는 인쇄 작업이 없습니다."

NotFoundMessageL       "\
해당 인쇄 작업이 더 이상 없거나\n\
프린터가 닫혔습니다."

$ ----- Modify Show Dialog -----
ModifyShowTitleL         "%s - 보기 목록 수정"
SelectPrintersToBeShowL  "보일 프린터 선택"
SelectAllL               "모두 선택"
DeselectAllL             "모두 해제"

$ ----- Set Options Dialog -----
SetOptionsTitleL      "%s - 선택사항 설정"
RepresentationL       "표현"
LargeIconL            "큰 아이콘"
SmallIconL            "작은 아이콘"
NameOnlyL             "이름만"
DetailsL              "세부사항"
ShowLabelsL           "레이블 보기"
JobsToShowL           "보일 작업"
OnlyMineL             "사용자작업만"
EveryoneL             "모든작업"
StatusL               "상태"
ProblemFlagL          "문제 플래그"
InformationLineL      "메세지 줄"
UpdateIntervalL       "새로 그리기"
IntervalL             "간격 (초단위)"

$ ----- Print Job Properties Dialog -----
PrintJobPropsTitleL  "%s - 인쇄 작업 특성"
PrinterL             "프린터"
JobName1L            "작업 이름"
SubmittedL           "제출된"
BytesL               "바이트수"

Position1L           "위치:"
JobNameL             "작업 이름:"
JobNumberL           "작업 번호:"
OwnerL               "소유자:"
DateL                "날짜:"
TimeL                "시간:"
SizeL                "크기:"

$ ----- Printer Properties Dialog -----
PrinterPropsTitleL    "%s - 프린터 특성"
IconLabelL            "아이콘 레이블:"
IconGroupTitleL       "아이콘:"
FindSetL              "세트 찾기..."
DescriptionL          "설명:"
PrinterStatusL        "상태:"
PrintQueueL           "프린터 대기행렬:"
DeviceL               "장치 이름:"
DeviceNL              "장치 %d:"
UpL                   "위쪽 - 서비스중"
DownL                 "아래쪽 - 서비스 완료"
PrinterOnServerL      "서버 %2$s에 있는 %1$s"
DefaultDescriptionL   "이것은 %s 프린터 대기행렬입니다."
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"
$ This is the value of the device field when the printer is on a remote system

$ ----- Find Set Dialog -----
FindSetTitleL       "%s - 찾기 설정"
IconFoldersL        "아이콘 폴더:"
IconTitleL          "아이콘"
LoadingIconsL       "%2$d개 아이콘 중 %1$d개를 로드하는 중..."
$ %1 is a counter and %2 is the total number of icons

$ ----- "Cancel" Print Job Confirmation Dialog -----
CancelTitleL     "%s - 취소 %s"
CancelQuestionL  "확실히 %s을(를) 취소하겠습니까?"

$ ----- Printer "Status" Dialog -----
PrinterStatusTitleL  "%s - 프린터 상태"
PrinterDownL         "%s 프린터 대기행렬은 서비스가 끝났습니다."
DeviceDownL          "%s 장치는 서비스가 끝났습니다."

$ ----- Failed Action Dialog -----
FailedActionTitleL  "%s - %s 실패"
FailedMessageL      "'%s' 활동을 %s에 적용할 수 없습니다.\n\
 다음 원인을 참조하십시오.\n\n%s"

$ ----- Main Window Information Line Messages -----
GettingHelpL      "온라인 도움말을 가져오는 중입니다..."
SearchingL        "프린터를 찾는 중입니다..."
ShowingMyJobsL    "내 작업만 보기"
PrintersHiddenL   "%d대 프린터 %d대 숨김"
JobsL             "%d개 작업"
UpdatingActionsL  "활동 데이타베이스를 새로 그리는중입니다..."
UpdatingL         "%s을(를) 새로 그리는중입니다..."
$ Do not translate EditLangL for this release
EditLangL         "/etc/dt/appconfig/types/%s에서 편집"
$ %s is the name of the printer

$ ----- Printer Menu Messages -----
PrinterMenuL         "프린터P"
PrinterAcceleratorL  "P"
FindChoiceL          "찾기F..."
FindMnemonicL        "F"
FindAcceleratorL     "Ctrl+F"
ExitChoiceL          "마침x"
ExitMnemonicL        "x"
ExitAcceleratorL     "Alt+F4"

$ ----- Selected Menu Messages -----
SelectedMenuL           "선택S"
SelectedAcceleratorL    "S"

$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.
$	                This choice pops up the "Cancel" Print Job Confirmation
$	                Dialog.
CancelChoiceL           "취소C"
CancelMnemonicL         "C"
CancelAcceleratorL      "Del"

$ For Printers Only
OpenChoiceL             "열기O"
OpenMnemonicL           "O"
CloseChoiceL            "닫기C"
CloseMnemonicL          "C"
HideChoiceL             "숨기기e"
HideMnemonicL           "e"
$ Do not translate RenameChoiceL for this release
RenameChoiceL           "이름 바꾸기R"
$ Do not translate RenameMnemonicL for this release
RenameMnemonicL         "R"
$ Do not translate StartChoiceL for this release
StartChoiceL            "대기행렬에 넣기 시작S"
$ Do not translate StartMnemonicL for this release
StartMnemonicL          "S"
$ Do not translate StopChoiceL for this release
StopChoiceL             "대기행렬에 넣기 멈춤t"
$ Do not translate StopMnemonicL for this release
StopMnemonicL           "t"
$ Do not translate EnableChoiceL for this release
EnableChoiceL           "인쇄 가능E"
$ Do not translate EnableMnemonicL for this release
EnableMnemonicL         "E"
$ Do not translate DisableChoiceL for this release
DisableChoiceL          "인쇄 불가D"
$ Do not translate DisableMnemonicL for this release
DisableMnemonicL        "D"

$ For Both Printers and Print Jobs
PropertiesChoiceL       "특성P..."
PropertiesMnemonicL     "P"
PropertiesAcceleratorL  "Ctrl+Backspace"
HelpChoiceL             "도움말H"
HelpMnemonicL           "H"

$ ----- View Menu Messages -----
ViewMenuL             "열람V"
ViewAcceleratorL      "V"
SetOptionsChoiceL     "선택사항 설정O..."
SetOptionsMnemonicL   "O"
ModifyShowChoiceL     "보기 목록 수정M..."
ModifyShowMnemonicL   "M"

$ ----- Help Menu Messages -----
$ "Help" Menu uses the "HelpChoiceL" message
$ "Help" Accelerator uses the "HelpMnemonicL" message
OverviewChoiceL     "개요v..."
OverviewMnemonicL   "v"
TaskChoiceL         "작업T..."
TaskMnemonicL       "T"
ReferenceChoiceL    "참조R..."
ReferenceMnemonicL  "R"
OnItemChoiceL       "항목O"
OnItemMnemonicL     "O"
UsingHelpChoiceL    "도움말 사용U..."
UsingHelpMnemonicL  "U"
AboutChoice1L       "인쇄 관리자에 관하여A"
AboutChoice2L       "프린터 작업에 관하여A"
$ Do not translate AboutChoice3L for this release
AboutChoice3L       "데스크탑 프린터에 관하여A"
AboutMnemonicL      "A"

$ ----- Misc Messages -----
EmptyL            "[비어있음]"
FoundNoPrintersL  "프린터를 찾을 수 없습니다."
NotAvailableL     "사용할 수 없음"
NoDefaultPrinter1L  "시스템 기본 프린터가 없습니다."
NoDefaultPrinter2L  "\
기본 프린터 LPDEST 환경을 설정하는 방법에 대한 설명은 \n\사용 설명서를 참조하십시오."
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.
RootUserL "%1$s: %2$s 선택사항을 사용하려면 루트 사용자이어야 합니다."


$ ----- Command Line Help -----
$ This message is show when the user enters 'dtprintinfo -help'
$ on the command line. The %s is a printer list generated by the program.

CommandLineHelpL "\
dtprintinfo\n\
\n\
    선택사항 없이 dtprintinfo를 시작하면, LPDEST가 설정되지 않을 경우 \n\LPDEST 프린터 또는 시스템 기본값이 나타납니다.\n\
\n\
dtprintinfo -p <printer>\n\
\n\
    특정 프린터를 보려면 -p 선택사항과 다음 인쇄 대기행렬 이름 중\n\
    하나를 사용하십시오.\n\
\n\
%s\
\n\
dtprintinfo -all\n\
\n\
    모든 프린터를 보려면 -all 선택사항을 사용하십시오.\n\
\n\
dtprintinfo -populate\n\
\n\
    -populate 선택사항은 루트에 대해서만 사용할 수 있습니다.  이\n\
    선택사항은 모든 프린터에 대한 기본 프린터 활동을\n\
    /etc/dt/appconfig/types/LANG에 만들어 놓습니다. LANG이 정의되어 있지 않으면,\n\
    활동은 /etc/dt/appconfig/types/C에 만들어집니다."
