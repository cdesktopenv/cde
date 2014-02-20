$ $XConsortium: dtprintinfo.msg /main/6 1996/10/19 14:27:30 rws $
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
$  $XConsortium: dtprintinfo.msg /main/6 1996/10/19 14:27:30 rws $
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

ApplicationName1L "Druckmanager"

ApplicationName2L "Druckerjobs"
$ Do not translate ApplicationName3L for this release

ApplicationName3L "Desktop-Drucker"
$ ----- Common Dialog Buttons -----

YesL "Ja"

NoL "Nein"

OKL "OK"

ApplyL "Anwenden"

CancelL "Abbrechen"

HelpL "Hilfe"
$ ----- Find dialog -----

FindTitleL "%s - Druckjobs suchen"

MyJobNameL "Name des eigenen Druckjobs"

OnlyRootJobsL "Nur Jobs von Root"

IgnoreCaseL "Groß-/Kleinschreibung ignorieren"

ExactMatchL "Exakte Übereinstimmung"

GotoL "Gehe zu"

CancelPrintJobsL "Druckjobs abbrechen"

PositionL "Position"

CloseL "Schließen"

StartFindL "Suche starten"

StopFindL "Suche stoppen"

SearchL "Suche %s...\n\n\
Zum Unterbrechen 'Stop' drücken"
$ Find Information dialog messages

NoMatchesL "Keine übereinstimmenden Druckjobs gefunden"

NotFoundMessageL "Druckjobs ist nicht mehr vorhanden\n\
oder Drucker ist geschlossen."
$ ----- Modify Show Dialog -----

ModifyShowTitleL "%s - Anzeigeliste ändern"

SelectPrintersToBeShowL "Drucker auswählen, die angezeigt werden sollen"

SelectAllL "Alles auswählen"

DeselectAllL "Alles abwählen"
$ ----- Set Options Dialog -----

SetOptionsTitleL "%s - Optionen festlegen"

RepresentationL "Darstellung"

LargeIconL "Großes Symbol"

SmallIconL "Kleines Symbol"

NameOnlyL "Nur Name"

DetailsL "Details"

ShowLabelsL "Kennungen anzeigen"

JobsToShowL "Anzuzeigende Jobs"

OnlyMineL "Nur meine"

EveryoneL "Von allen"

StatusL "Status"

ProblemFlagL "Problem-Option"

InformationLineL "Meldungszeile"

UpdateIntervalL "Aktualisierungen"

IntervalL "Intervall (in Sekunden)"
$ ----- Print Job Properties Dialog -----

PrintJobPropsTitleL "%s - Druckjob-Eigenschaften"

PrinterL "Drucker"

JobName1L "Jobname"

SubmittedL "abgesetzt"

BytesL "Byte"

Position1L "Position:"

JobNameL "Jobname:"

JobNumberL "Jobnummer:"

OwnerL "Eigner:"

DateL "Datum:"

TimeL "Uhrzeit:"

SizeL "Größe:"
$ ----- Printer Properties Dialog -----

PrinterPropsTitleL "%s - Druckereigenschaften"

IconLabelL "Symbolkennung:"

IconGroupTitleL "Symbole:"

FindSetL "Gruppe finden..."

DescriptionL "Beschreibung:"

PrinterStatusL "Status:"

PrintQueueL "Druckerwarteschlange:"

DeviceL "Einheitenname:"

DeviceNL "Einheit %d:"

UpL "Aktiv - In Betrieb"

DownL "Inaktiv - Außer Betrieb"

PrinterOnServerL "%1$s an %2$s"

DefaultDescriptionL "Hierbei handelt es sich um die %s Warteschlange."
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"
$ This is the value of the device field when the printer is on a remote system
$ ----- Find Set Dialog -----

FindSetTitleL "%s - Gruppe finden"

IconFoldersL "Symbolordner:"

IconTitleL "Symbole"

LoadingIconsL "Laden der Symbole %1$d von %2$d..."
$ %1 is a counter and %2 is the total number of icons
$ ----- "Cancel" Print Job Confirmation Dialog -----

CancelTitleL "%s - %s abbrechen"

CancelQuestionL "Soll %s tatsächlich abgebrochen werden?"
$ ----- Printer "Status" Dialog -----

PrinterStatusTitleL "%s - Druckerstatus"

PrinterDownL "Die Druckerwarteschlange %s ist außer Betrieb."

DeviceDownL "Die Einheit %s ist außer Betrieb."
$ ----- Failed Action Dialog -----

FailedActionTitleL "%s - %s gescheitert"

FailedMessageL "Die Aktion '%s' konnte nicht angewendet\n\
werden auf %s. Ursache ist nachfolgend angegeben.\n\n\
%s"
$ ----- Main Window Information Line Messages -----

GettingHelpL "Aufrufen der Online-Hilfe..."

SearchingL "Suchen nach Druckern..."

ShowingMyJobsL "Nur meine Jobs anzeigen"

PrintersHiddenL "%d Drucker %d verdeckt"

JobsL "%d Jobs"

UpdatingActionsL "Aktualisieren der Aktionsdatenbank..."

UpdatingL "%s wird aktualisiert..."
$ Do not translate EditLangL for this release

EditLangL "Editieren in /etc/dt/appconfig/types/%s"
$ %s is the name of the printer
$ ----- Printer Menu Messages -----

PrinterMenuL "Drucker"

PrinterAcceleratorL "P"

FindChoiceL "Suchen..."

FindMnemonicL "S"

FindAcceleratorL "Ctrl+F"

ExitChoiceL "Ende"

ExitMnemonicL "E"

ExitAcceleratorL "Alt+F4"
$ ----- Selected Menu Messages -----

SelectedMenuL "Ausgewählt"

SelectedAcceleratorL "S"
$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.
$                       This choice pops up the "Cancel" Print Job Confirmation
$                       Dialog.

CancelChoiceL "Abbrechen"

CancelMnemonicL "A"

CancelAcceleratorL "Del"
$ For Printers Only

OpenChoiceL "Öffnen"

OpenMnemonicL "f"

CloseChoiceL "Schließen"

CloseMnemonicL "c"

HideChoiceL "Verdecken"

HideMnemonicL "e"
$ Do not translate RenameChoiceL for this release

RenameChoiceL "Umbenennen"
$ Do not translate RenameMnemonicL for this release

RenameMnemonicL "U"
$ Do not translate StartChoiceL for this release

StartChoiceL "Einreihen in Warteschlange starten"
$ Do not translate StartMnemonicL for this release

StartMnemonicL "S"
$ Do not translate StopChoiceL for this release

StopChoiceL "Einreihen in Warteschlange stoppen"
$ Do not translate StopMnemonicL for this release

StopMnemonicL "t"
$ Do not translate EnableChoiceL for this release

EnableChoiceL "Drucken aktivieren"
$ Do not translate EnableMnemonicL for this release

EnableMnemonicL "a"
$ Do not translate DisableChoiceL for this release

DisableChoiceL "Drucken inaktivieren"
$ Do not translate DisableMnemonicL for this release

DisableMnemonicL "i"
$ For Both Printers and Print Jobs

PropertiesChoiceL "Eigenschaften..."

PropertiesMnemonicL "E"

PropertiesAcceleratorL "Ctrl+Backspace"

HelpChoiceL "Hilfe"

HelpMnemonicL "H"
$ ----- View Menu Messages -----

ViewMenuL "Ansicht"

ViewAcceleratorL "A"

SetOptionsChoiceL "Optionen festlegen..."

SetOptionsMnemonicL "O"

ModifyShowChoiceL "Anzeigeliste ändern..."

ModifyShowMnemonicL "A"
$ ----- Help Menu Messages -----
$ "Help" Menu uses the "HelpChoiceL" message
$ "Help" Accelerator uses the "HelpMnemonicL" message

OverviewChoiceL "Überblick"

OverviewMnemonicL "b"

TaskChoiceL "Aufgaben"

TaskMnemonicL "A"

ReferenceChoiceL "Referenzinformationen"

ReferenceMnemonicL "R"

OnItemChoiceL "Kontexthilfe"

OnItemMnemonicL "K"

UsingHelpChoiceL "Hilfe für Hilfe"

UsingHelpMnemonicL "H"

AboutChoice1L "Informationen zu Druckmanager"

AboutChoice2L "Informationen zu Druckerjobs"
$ Do not translate AboutChoice3L for this release

AboutChoice3L "Informationen zu Desktop-Druckern"

AboutMnemonicL "A"
$ ----- Misc Messages -----

EmptyL "[ Empty ]"

FoundNoPrintersL "Keine Drucker gefunden."

NotAvailableL "nicht verfügbar"

NoDefaultPrinter1L "Es gibt keinen Systemstandarddrucker."

NoDefaultPrinter2L "Im Benutzerhandbuch ist beschrieben, wie\n\
die LPDEST-Umgebung des Standarddruckers festgelegt wird."
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.

RootUserL "%1$s: Zur Verwendung der Option %2$s muß root-Berechtigung vorliegen."
$ ----- Command Line Help -----
$ This message is show when the user enters 'dtprintinfo -help'
$ on the command line. The %s is a printer list generated by the program.

CommandLineHelpL "dtprintinfo\n\n\
    Wenn dtprintinfo ohne Optionen aufgerufen wird, wird der LPDEST-Drucker\n\
    bzw. der Systemstandarddrucker angezeigt (wenn LPDEST nicht definiert ist).\n\n\
dtprintinfo -p <Drucker>\n\n\
    Zum Anzeigen eines bestimmten Druckers die Option -p und einen der\n\
    folgenden Warteschlangenamen angeben:\n\n\
%s\n\
dtprintinfo -all\n\n\
    Zum Anzeigen aller Drucker die Option -all angeben.\n\n\
dtprintinfo -populate\n\n\
    Die Option -populate steht nur für root zur Verfügung.\n\
    Durch diese Option werden Standarddruckeraktionen für alle Drucker\n\
    in /etc/dt/appconfig/types/LANG erstellt. Wenn LANG nicht definiert\n\
    ist, werden die Aktionen in /etc/dt/appconfig/types/C erstellt."

NoRemoteCancelL "Der Druckserver hat keine Druckjobnummer angegeben.\nEinige Druckserver erlauben nicht das entfernte Abbrechen von\nDruckjobs. Wenn Sie wissen, daß Ihr Drucker dies zuläßt, informieren Sie bitte\nIhren Druckserver-Verwalter."
FindAcceleratorKeySeq "Strg<Taste>f"
