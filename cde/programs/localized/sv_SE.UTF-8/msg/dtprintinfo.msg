$ $XConsortium: dtprintinfo.msg /main/3 1996/07/24 18:59:54 drk $
$quote "
$set DTPRINTER_SET
$ (kommentar nästa rad och lagt till '$set 1' på raden efter 22.11.94 Kenny)
$ (lagt till DTPRINTER_SET efter 'set 1' 30.12.94 silvio)
$ set DTPRINTER_SET
$ ----- These are the titles that appear on dialogs and the main window -----
ApplicationName1L  "Utskriftshanterare"
ApplicationName2L  "Utskriftsjobb"
$ Do not translate ApplicationName3L for this release
ApplicationName3L  "Desktop Printers"

$ ----- Common Dialog Buttons -----
YesL     "Ja"
NoL      "Nej"
OKL      "OK"
ApplyL   "Tillämpa"
CancelL  "Avbryt"
HelpL    "Hjälp"

$ ----- Find dialog -----
FindTitleL        "%s - Sök utskriftsjobb"
MyJobNameL        "Mitt jobbs namn"
OnlyRootJobsL     "Endast rot-jobb"
IgnoreCaseL       "Ignorera STOR-liten bokstav"
ExactMatchL       "Exakt träff"
GotoL             "Gå till"
CancelPrintJobsL  "Avbryt utskriftsjobb"
PositionL         "Position"
CloseL            "Stäng"
StartFindL        "Starta sökning"
StopFindL         "Avbryt sökning"
SearchL           "\
Searching %s...\n\
\n\
Tryck på Stopp för att avbryta"

$ Find Information dialog messages
NoMatchesL             "Inget sökt jobb hittades"

NotFoundMessageL       "\
Utskriftsjobbet finns inte längre\n\
eller så har skrivaren stängts."

$ ----- Modify Show Dialog -----
ModifyShowTitleL         "%s - Ändra listan"
SelectPrintersToBeShowL  "Markera skrivare som ska visas"
SelectAllL               "Markera alla"
DeselectAllL             "Avmarkera alla"

$ ----- Set Options Dialog -----
SetOptionsTitleL      "%s - Välj alternativ"
RepresentationL       "Visa som"
LargeIconL            "Stor ikon"
SmallIconL            "Liten ikon"
NameOnlyL             "Endast namn"
DetailsL              "Detaljer"
ShowLabelsL           "Visa namn"
JobsToShowL           "Jobb som ska visas"
OnlyMineL             "Bara mina"
EveryoneL             "Allas"
StatusL               "Status"
ProblemFlagL          "Problemflagga"
InformationLineL      "Meddelanderad"
UpdateIntervalL       "Uppdateringar"
IntervalL             "Intervall (i sekunder)"

$ ----- Print Job Properties Dialog -----
PrintJobPropsTitleL  "%s - Egenskaper för utskriftsjobb"
PrinterL             "Skrivare"
JobName1L            "Jobbnamn"
SubmittedL           "Startat av"
BytesL               "byte"
Position1L           "Position:"
JobNameL             "Jobbnamn:"
JobNumberL           "Jobbnummer:"
OwnerL               "Ägare:"
DateL                "Datum:"
TimeL                "Klockslag:"
SizeL                "Storlek:"

$ ----- Printer Properties Dialog -----
PrinterPropsTitleL    "%s - Egenskaper för skrivare"
IconLabelL            "Ikonnamn:"
IconGroupTitleL       "Ikoner:"
FindSetL              "Sök set..."
DescriptionL          "Beskrivning:"
PrinterStatusL        "Status:"
PrintQueueL           "Skrivarkö:"
DeviceL               "Enhetsnamn:"
DeviceNL              "enhet %d:"
UpL                   "Uppe - fungerar"
DownL                 "Nere - fungerar inte"
PrinterOnServerL      "%1$s på %2$s"
DefaultDescriptionL   "Det här är skrivarkön %s."
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"
$ This is the value of the device field when the printer is on a remote system

$ ----- Find Set Dialog -----
FindSetTitleL       "%s - Sök set"
IconFoldersL        "Ikonmapp:"
IconTitleL          "Ikoner"
LoadingIconsL       "Laddar %1$d ikoner av %2$d..."
$ %1 is a counter and %2 is the total number of icons

$ ----- "Cancel" Print Job Confirmation Dialog -----
CancelTitleL     "%s - Ta bort %s"
CancelQuestionL  "Är du säker på att du vill ta bort %s?"

$ ----- Printer "Status" Dialog -----
PrinterStatusTitleL  "%s - Skrivarstatus"
PrinterDownL         "Skrivarkön %s fungerar inte."
DeviceDownL          "Enheten %s fungerar inte."

$ ----- Failed Action Dialog -----
FailedActionTitleL  "%s - %s misslyckades"
FailedMessageL      "det gick inte att tillämpa funktionen '%s' på\n\
  %s. Se nedan för eventuella orsaker.\n\n%s"

$ ----- Main Window Information Line Messages -----
GettingHelpL      "Hämtar hjälp..."
SearchingL        "Skrivare söks..."
ShowingMyJobsL    "Visar endast mina jobb"
PrintersHiddenL   "%d Skrivare %d Dold"
JobsL             "%d Jobb"
UpdatingActionsL  "Funktionsdatabasen uppdateras..."
UpdatingL         "%s uppdateras..."
$ Do not translate EditLangL for this release
EditLangL         "Editing in /etc/dt/appconfig/types/%s"
$ %s is the name of the printer

$ ----- Printer Menu Messages -----
PrinterMenuL         "Skrivare"
PrinterAcceleratorL  "P"
FindChoiceL          "Sök..."
FindMnemonicL        "S"
FindAcceleratorL     "Ctrl+S"
ExitChoiceL          "Avsluta"
ExitMnemonicL        "x"
ExitAcceleratorL     "Alt+F4"

$ ----- Selected Menu Messages -----
SelectedMenuL           "Markerad"
SelectedAccelerator     "M"

$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.
$                       This choice pops up the "Cancel" Print Job Confirmation
$                       Dialog. 
CancelChoiceL           "Ta bort"
CancelMnemonicL         "b"
CancelAcceleratorL      "Del"

$ For Printers Only
OpenChoiceL             "Öppna"
OpenMnemonicL           "Ö"
CloseChoiceL            "Stäng"
CloseMnemonicL          "S"
HideChoiceL             "Dölj"
HideMnemonicL           "e"
$ Do not translate RenameChoiceL for this release
RenameChoiceL           "Ren"
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
PropertiesChoiceL       "Egenskaper..."
PropertiesMnemonicL     "E"
PropertiesAcceleratorL  "Ctrl+Backsteg"
HelpChoiceL             "Hjälp"
HelpMnemonicL           "H"

$ ----- View Menu Messages -----
ViewMenuL             "Visa"
ViewAcceleratorL      "V"
SetOptionsChoiceL     "Välj alternativ..."
SetOptionsMnemonicL   "v"
ModifyShowChoiceL     "Ändra listan..."
ModifyShowMnemonicL   "Ä"

$ ----- Help Menu Messages -----
$ "Help" Menu uses the "HelpChoiceL" message
$ "Help" Accelerator uses the "HelpMnemonicL" message
OverviewChoiceL     "Översikt"
OverviewMnemonicL   "v"
TaskChoiceL         "Steg-för-steg-anvisningar"
TaskMnemonicL       "t"
ReferenceChoiceL    "Referens"
ReferenceMnemonicL  "R"
OnItemChoiceL       "Objekthjälp"
OnItemMnemonicL     "O"
UsingHelpChoiceL    "Om hjälpfunktionen"
UsingHelpMnemonicL  "j"
AboutChoice1L       "Om Utskriftshanterare"
AboutChoice2L       "Om Utskriftsjobb"
$ Do not translate AboutChoice3L for this release
AboutChoice3L       "About Desktop Printers"
AboutMnemonicL      "A"

$ ----- Misc Messages -----
EmptyL            "[ Tom ]"
FoundNoPrintersL  "Inga skrivare hittades."
NotAvailableL     "inte tillgänglig"
NoDefaultPrinter1L  "Det finns ingen standardsystemskrivare."
NoDefaultPrinter2L  "\
I användarhandboken finns information om hur man ställer\n\
in miljövariabel för standardsystemskrivare (LPDEST)."
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.
RootUserL "%1$s: Du måste vara användaren root för att använda tillägget %2$s."


$ ----- Command Line Help -----
$ This message is show when the user enters 'dtprintinfo -help'
$ on the command line. The %s is a printer list generated by the program.

CommandLineHelpL "\
dtprintinfo\n\
\n\
    Om du startar dtprintinfo utan tillägg visas skrivaren LPDEST.\n\
    Om LPDEST inte är inställd visas systemskrivaren.\n\
\n\
dtprintinfo -p <skrivare>\n\
\n\
    Använd tillägget -p och\n\
    ett av följande könamn:\n\
\n\
%s\
\n\
dtprintinfo -all\n\
\n\
    Använd tillägget -all för att se alla skrivare.\n\
\n\
dtprintinfo -populate\n\
\n\
    Tillägget -populate är bara tillgängligt för användaren root.\n\
    Tillägget skapar standardskrivarfunktioner för alla skrivare\n\
    i /etc/dt/appconfig/types/LANG. Om LANG inte är definierad\n\
    skapas funktionen i /etc/dt/appconfig/types/C."
