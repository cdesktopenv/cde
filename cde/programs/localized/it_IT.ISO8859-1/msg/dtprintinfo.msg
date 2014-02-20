$ $XConsortium: dtprintinfo.msg /main/5 1996/10/19 16:11:43 rws $
$ 
$   COMPONENT_NAME: desktop
$ 
$   FUNCTIONS: none
$ 
$   ORIGINS: 27,118,119,120,121
$ 
$   This module contains IBM CONFIDENTIAL code. -- (IBM
$   Confidential Restricted when combined with the aggregated
$   modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ 
$ +SNOTICE
$ 
$  $XConsortium: dtprintinfo.msg /main/5 1996/10/19 16:11:43 rws $
$ 
$  RESTRICTED CONFIDENTIAL INFORMATION:
$ 
$  The information in this document is subject to special
$  restrictions in a confidential disclosure agreement between
$  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
$  document outside HP, IBM, Sun, USL, SCO, or Univel without
$  SunXSQUOTEs specific written approval.  This document and all copies
$  and derivative works thereof must be returned or destroyed at
$  SunXSQUOTEs request.
$ 
$  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
$ 
$ +ENOTICE
$ 
$		     Common Desktop Environment
$ 
$   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
$   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
$   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
$   (c) Copyright 1993, 1994, 1995 Novell, Inc.
$   (c) Copyright 1995 Digital Equipment Corp.
$   (c) Copyright 1995 Fujitsu Limited
$   (c) Copyright 1995 Hitachi, Ltd.
$                                                                   
$ 
$                     RESTRICTED RIGHTS LEGEND                              
$ 
$ Use, duplication, or disclosure by the U.S. Government is subject to
$ restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
$ Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
$ for non-DOD U.S. Government Departments and Agencies are as set forth in
$ FAR 52.227-19(c)(1,2).
$ 
$ Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
$ International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
$ Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
$ Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
$ Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
$ Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
$ Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan

$quote XDQUOTE

$set DTPRINTER_SET

$ ----- These are the titles that appear on dialogs and the main window -----
ApplicationName1L  "Gestione della stampa"
ApplicationName2L  "Lavori della stampante"
$ Do not translate ApplicationName3L for this release
ApplicationName3L  "Desktop Printers"

$ ----- Common Dialog Buttons -----
YesL     "Sì"
NoL      "No"
OKL      "OK"
ApplyL   "Applicare"
CancelL  "Annullare"
HelpL    "Aiuto"

$ ----- Find dialog -----
FindTitleL        "%s - Ricerca dei lavori di stampa"
MyJobNameL        "Nome del lavoro"
OnlyRootJobsL     "Solo i lavori del superutente"
IgnoreCaseL       "Ignorare maiuscole/minuscole"
ExactMatchL       "Corrispondenza esatta"
GotoL             "Andare a"
CancelPrintJobsL  "Annullare i lavori di stampa"
PositionL         "Posizione"
CloseL            "Chiudere"
StartFindL        "Iniziare la ricerca"
StopFindL         "Interrompere"
SearchL           "\
Ricerca di %s in corso...\n\
\n\
Premere Interrompere per terminare l'operazione."

$ Find Information dialog messages
NoMatchesL             "Nessun lavoro corrispondente allo schema di ricerca"

NotFoundMessageL       "\
Il lavoro di stampa non esiste più,\n\
oppure la stampante non è attiva."

$ ----- Modify Show Dialog -----
ModifyShowTitleL         "%s - Modifica dell'elenco delle stampanti"
SelectPrintersToBeShowL  "Selezionare le stampanti da visualizzare"
SelectAllL               "Selezionare tutto"
DeselectAllL             "Deselezionare tutto"

$ ----- Set Options Dialog -----
SetOptionsTitleL      "%s - Impostazione delle opzioni"
RepresentationL       "Ripresentazione"
LargeIconL            "Icona grande"
SmallIconL            "Icona piccola"
NameOnlyL             "Solo nome"
DetailsL              "Dettagli"
ShowLabelsL           "Mostrare etichette"
JobsToShowL           "Lavori da mostrare"
OnlyMineL             "Personali"
EveryoneL             "Tutti"
StatusL               "Stato"
ProblemFlagL          "Indicatore di problemi"
InformationLineL      "Riga messaggi"
UpdateIntervalL       "Aggiornamenti"
IntervalL             "Intervallo (in secondi)"

$ ----- Print Job Properties Dialog -----
PrintJobPropsTitleL  "%s - Caratteristiche del lavoro di stampa"
PrinterL             "Stampante"
JobName1L            "Nome del lavoro"
SubmittedL           "Data e ora della richiesta"
BytesL               "byte"

Position1L           "Posizione:"
JobNameL             "Nome:"
JobNumberL           "Numero:"
OwnerL               "Proprietario:"
DateL                "Data:"
TimeL                "Ora:"
SizeL                "Dimensione:"

$ ----- Printer Properties Dialog -----
PrinterPropsTitleL    "%s - Caratteristiche della stampante"
IconLabelL            "Etichetta dell'icona:"
IconGroupTitleL       "Icone:"
FindSetL              "Ricercare icone..."
DescriptionL          "Descrizione:"
PrinterStatusL        "Stato:"
PrintQueueL           "Coda della stampante:"
DeviceL               "Nome dell'unità:"
DeviceNL              "Unità %d:"
UpL                   "Attiva - In servizio"
DownL                 "Non attiva - Fuori servizio"
PrinterOnServerL      "%1$s su %2$s"
DefaultDescriptionL   "Questa è la coda della stampante %s."
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"
$ This is the value of the device field when the printer is on a remote system

$ ----- Find Set Dialog -----
FindSetTitleL       "%s - Ricerca delle icone"
IconFoldersL        "Cartelle con icone:"
IconTitleL          "Icone"
LoadingIconsL       "Caricamento dell'icona %1$d di %2$d..."
$ %1 is a counter and %2 is the total number of icons

$ ----- "Cancel" Print Job Confirmation Dialog -----
CancelTitleL     "%s - Annullamento di %s"
CancelQuestionL  "Confermare l'annullamento di %s?"

$ ----- Printer "Status" Dialog -----
PrinterStatusTitleL  "%s - Stato della stampante"
PrinterDownL         "La coda della stampante %s non è attiva."
DeviceDownL          "L'unità %s non è attiva."

$ ----- Failed Action Dialog -----
FailedActionTitleL  "%s - %s non riuscita"
FailedMessageL      "Impossibile applicare l'azione '%s' a\n\
  %s. Vedere qui sotto le possibili cause del problema.\n\n%s"

$ ----- Main Window Information Line Messages -----
GettingHelpL      "Richiamo dell'aiuto online..."
SearchingL        "Ricerca delle stampanti in corso..."
ShowingMyJobsL    "Mostrare solo i lavori dell'utente"
PrintersHiddenL   "%d stampanti %d nascoste"
JobsL             "%d lavori"
UpdatingActionsL  "Aggiornamento del database delle azioni..."
UpdatingL         "Aggiornamento di %s..."
$ Do not translate EditLangL for this release
EditLangL         "Editing in /etc/dt/appconfig/types/%s"
$ %s is the name of the printer

$ ----- Printer Menu Messages -----
PrinterMenuL         "Stampanti"
PrinterAcceleratorL  "S"
FindChoiceL          "Ricercare..."
FindMnemonicL        "R"
FindAcceleratorL     "Ctrl+R"
ExitChoiceL          "Chiudere"
ExitMnemonicL        "C"
ExitAcceleratorL     "Alt+F4"

$ ----- Selected Menu Messages -----
SelectedMenuL           "Selezioni"
SelectedAcceleratorL    "l"

$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.
$                       This choice pops up the "Cancel" Print Job Confirmation
$                       Dialog. 
CancelChoiceL           "Annullare"
CancelMnemonicL         "A"
CancelAcceleratorL      "Del"

$ For Printers Only
OpenChoiceL             "Aprire"
OpenMnemonicL           "A"
CloseChoiceL            "Chiudere"
CloseMnemonicL          "C"
HideChoiceL             "Nascondere"
HideMnemonicL           "N"
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
PropertiesChoiceL       "Caratteristiche..."
PropertiesMnemonicL     "t"
PropertiesAcceleratorL  "Ctrl+Backspace"
HelpChoiceL             "Aiuto"
HelpMnemonicL           "u"

$ ----- View Menu Messages -----
ViewMenuL             "Visualizzare"
ViewAcceleratorL      "V"
SetOptionsChoiceL     "Impostare opzioni..."
SetOptionsMnemonicL   "o"
ModifyShowChoiceL     "Modificare l'elenco delle stampanti..."
ModifyShowMnemonicL   "M"

$ ----- Help Menu Messages -----
$ "Help" Menu uses the "HelpChoiceL" message
$ "Help" Accelerator uses the "HelpMnemonicL" message
OverviewChoiceL     "Panoramica"
OverviewMnemonicL   "P"
TaskChoiceL         "Attività"
TaskMnemonicL       "A"
ReferenceChoiceL    "Riferimenti"
ReferenceMnemonicL  "R"
OnItemChoiceL       "Sull'elemento"
OnItemMnemonicL     "S"
UsingHelpChoiceL    "Uso dell'aiuto"
UsingHelpMnemonicL  "U"
AboutChoice1L       "Informazioni sulla Gestione della stampa"
AboutChoice2L       "Informazioni sui Lavori della stampante"
$ Do not translate AboutChoice3L for this release
AboutChoice3L       "About Desktop Printers"
AboutMnemonicL      "A"

$ ----- Misc Messages -----
EmptyL            "[ Vuota ]"
FoundNoPrintersL  "Nessuna stampante trovata."
NotAvailableL     "non disponibile"
NoDefaultPrinter1L  "Il sistema non possiede stampanti predefinite."
NoDefaultPrinter2L  "\
Per informazioni su come impostare l'ambiente LPDEST\n\
per la stampante predefinita, vedere il Manuale dell'utente."
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.
RootUserL "%1$s: Per usare l'opzione %2$s è necessario essere superutente."


$ ----- Command Line Help -----
$ This message is show when the user enters 'dtprintinfo -help'
$ on the command line. The %s is a printer list generated by the program.

CommandLineHelpL "\
dtprintinfo\n\
\n\
    Avviando dtprintinfo senza opzioni viene mostrata la \n\
    stampante LPDEST o la stampante predefinita del sistema.\n\
\n\
dtprintinfo -p <stampante>\n\
\n\
    Per visualizzare una stampante specifica, usare l'opzione -p\n\
    e specificare una delle code di stampa seguenti:\n\
\n\
%s\
\n\
dtprintinfo -all\n\
\n\
    Per visualizzare tutte le stampanti, usare l'opzione -all.\n\
\n\
dtprintinfo -populate\n\
\n\
    L'opzione -populate è disponibile solo per il superutente. Essa\n\
    crea le azioni della stampante predefinita per tutte le stampanti\n\
    incluse in /etc/dt/appconfig/types/LANG. Se la variabile LANG non è\n\
    definita, le azioni vengono create in /etc/dt/appconfig/types/C."

NoRemoteCancelL "Il server di stampa non ha fornito il numero del lavoro di stampa.\nAlcuni server di stampa non permettono l'annullamento remoto\ndei lavori di stampa. Rivolgersi all'amministratore stampanti."
FindAcceleratorKeySeq "Ctrl<Tasto>f"
