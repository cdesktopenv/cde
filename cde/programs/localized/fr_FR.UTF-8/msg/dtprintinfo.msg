$ $XConsortium: dtprintinfo.msg /main/6 1996/10/19 15:27:14 rws $
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
$  $XConsortium: dtprintinfo.msg /main/6 1996/10/19 15:27:14 rws $
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

ApplicationName1L "Gestionnaire d'impression"

ApplicationName2L "Travaux d'impression"
$ Do not translate ApplicationName3L for this release  

ApplicationName3L "Imprimantes bureau"
$ ----- Common Dialog Buttons -----  

YesL "Oui"

NoL "Non"

OKL "OK"

ApplyL "Appliquer"

CancelL "Annuler"

HelpL "Aide"
$ ----- Find dialog -----  

FindTitleL "%s - Rechercher travaux d'impression"

MyJobNameL "Nom travail personnel"

OnlyRootJobsL "Travaux root uniquement"

IgnoreCaseL "Pas de distinction maj-min"

ExactMatchL "Correspondance exacte"

GotoL "Aller"

CancelPrintJobsL "Annuler travaux d'impression"

PositionL "Position"

CloseL "Fermeture"

StartFindL "Démarrer recherche"

StopFindL "Arrêter recherche"

SearchL "Recherche de %s...\n\n\
Appuyez sur Arrêter pour interrompre"
$ Find Information dialog messages  

NoMatchesL "Pas de travail correspondant"

NotFoundMessageL "Le travail n'existe plus ou \n\
l'imprimante est arrêtée."
$ ----- Modify Show Dialog -----  

ModifyShowTitleL "%s - Modifier la liste d'affichage"

SelectPrintersToBeShowL "Sélectionner imprimantes à afficher"

SelectAllL "Tout sélectionner"

DeselectAllL "Tout désélectionner"
$ ----- Set Options Dialog -----  

SetOptionsTitleL "%s - Définir les options"

RepresentationL "Représentation"

LargeIconL "Grandes icônes"

SmallIconL "Petites icônes"

NameOnlyL "Nom uniquement"

DetailsL "Informations"

ShowLabelsL "Affichage libellés"

JobsToShowL "Travaux à afficher"

OnlyMineL "Travaux personnels uniquement"

EveryoneL "Tous les travaux"

StatusL "Etat"

ProblemFlagL "Signal d'erreur"

InformationLineL "Ligne de message"

UpdateIntervalL "Mises à jour"

IntervalL "Intervalle (en secondes)"
$ ----- Print Job Properties Dialog -----  

PrintJobPropsTitleL "%s - Caractéristiques travail d'impression"

PrinterL "Imprimante"

JobName1L "Nom du travail"

SubmittedL "Soumis"

BytesL "octets"

Position1L "Position:"

JobNameL "Nom de travail:"

JobNumberL "Numéro de travail:"

OwnerL "Propriétaire:"

DateL "Date:"

TimeL "Heure:"

SizeL "Taille:"
$ ----- Printer Properties Dialog -----  

PrinterPropsTitleL "%s - Caractéristiques d'imprimante"

IconLabelL "Libellé d'icône:"

IconGroupTitleL "Icônes:"

FindSetL "Rechercher ensemble..."

DescriptionL "Description:"

PrinterStatusL "Etat:"

PrintQueueL "File d'attente d'impression:"

DeviceL "Nom d'unité:"

DeviceNL "Unité %d:"

UpL "EF - En fonction"

DownL "HF - Hors fonction"

PrinterOnServerL "%1$s de %2$s"

DefaultDescriptionL "File d'attente %s."
$ %1 is the printer name and %2 is the server name, ex: "lp0 on warpspeed"  
$ This is the value of the device field when the printer is on a remote system  
$ ----- Find Set Dialog -----  

FindSetTitleL "%s - Rechercher ensemble"

IconFoldersL "Dossiers icônes:"

IconTitleL "Icônes"

LoadingIconsL "Chargement des icônes %1$d de %2$d..."
$ %1 is a counter and %2 is the total number of icons  
$ ----- "Cancel" Print Job Confirmation Dialog -----  

CancelTitleL "%s - Annulation de %s"

CancelQuestionL "Souhaitez-vous annuler %s?"
$ ----- Printer "Status" Dialog -----  

PrinterStatusTitleL "%s - Etat imprimante"

PrinterDownL "La file d'attente d'impression %s est hors fonction."

DeviceDownL "L'unité %s est hors fonction."
$ ----- Failed Action Dialog -----  

FailedActionTitleL "%s - %s n'a pas abouti"

FailedMessageL "Impossible d'appliquer l'action '%s' à\n\
  %s. Reportez-vous au code cause ci-dessous.\n\n\
%s"
$ ----- Main Window Information Line Messages -----  

GettingHelpL "Obtention de l'aide en ligne..."

SearchingL "Recherche des imprimantes..."

ShowingMyJobsL "Affichage des travaux personnels uniquement"

PrintersHiddenL "%d Imprimantes %d Cachés"

JobsL "%d Travaux"

UpdatingActionsL "Mise à jour de la base de données d'actions..."

UpdatingL "Mise à jour de %s..."
$ Do not translate EditLangL for this release  

EditLangL "Edition de /etc/dt/appconfig/types/%s"
$ %s is the name of the printer  
$ ----- Printer Menu Messages -----  

PrinterMenuL "Imprimantes"

PrinterAcceleratorL "I"

FindChoiceL "Recherche..."

FindMnemonicL "R"

FindAcceleratorL "Ctrl+R"

ExitChoiceL "Quitter"

ExitMnemonicL "t"

ExitAcceleratorL "Alt+F4"
$ ----- Selected Menu Messages -----  

SelectedMenuL "Sélectionné(s)"

SelectedAcceleratorL "S"
$ For Print Jobs Only - "Cancel" means to remove a print job from the printer.  
$                       This choice pops up the "Cancel" Print Job Confirmation  
$                       Dialog.   

CancelChoiceL "Annuler"

CancelMnemonicL "A"

CancelAcceleratorL "Suppr"
$ For Printers Only  

OpenChoiceL "Ouvrir"

OpenMnemonicL "O"

CloseChoiceL "Fermer"

CloseMnemonicL "F"

HideChoiceL "Cacher"

HideMnemonicL "r"
$ Do not translate RenameChoiceL for this release  

RenameChoiceL "Renommer"
$ Do not translate RenameMnemonicL for this release  

RenameMnemonicL "m"
$ Do not translate StartChoiceL for this release  

StartChoiceL "Démarrer mise en file d'attente"
$ Do not translate StartMnemonicL for this release  

StartMnemonicL "D"
$ Do not translate StopChoiceL for this release  

StopChoiceL "Arrêter mise en file d'attente"
$ Do not translate StopMnemonicL for this release  

StopMnemonicL "t"
$ Do not translate EnableChoiceL for this release  

EnableChoiceL "Activer impression"
$ Do not translate EnableMnemonicL for this release  

EnableMnemonicL "i"
$ Do not translate DisableChoiceL for this release  

DisableChoiceL "Désactiver impression"
$ Do not translate DisableMnemonicL for this release  

DisableMnemonicL "D"
$ For Both Printers and Print Jobs  

PropertiesChoiceL "Caractéristiques..."

PropertiesMnemonicL "C"

PropertiesAcceleratorL "Ctrl+Retour"

HelpChoiceL "Aide"

HelpMnemonicL "i"
$ ----- View Menu Messages -----  

ViewMenuL "Visualiser"

ViewAcceleratorL "V"

SetOptionsChoiceL "Définir les options..."

SetOptionsMnemonicL "o"

ModifyShowChoiceL "Modifier la liste d'affichage..."

ModifyShowMnemonicL "M"
$ ----- Help Menu Messages -----  
$ "Help" Menu uses the "HelpChoiceL" message  
$ "Help" Accelerator uses the "HelpMnemonicL" message  

OverviewChoiceL "Généralités"

OverviewMnemonicL "l"

TaskChoiceL "Tâches"

TaskMnemonicL "T"

ReferenceChoiceL "Référence"

ReferenceMnemonicL "R"

OnItemChoiceL "Sur l'élément"

OnItemMnemonicL "u"

UsingHelpChoiceL "Aide sur l'aide"

UsingHelpMnemonicL "d"

AboutChoice1L "A propos du Gestionnaire d'impression"

AboutChoice2L "A propos des travaux d'impression"
$ Do not translate AboutChoice3L for this release  

AboutChoice3L "A propos des imprimantes bureau"

AboutMnemonicL "m"
$ ----- Misc Messages -----  

EmptyL "[ Vide ]"

FoundNoPrintersL "Aucune imprimante trouvée."

NotAvailableL "non disponible"

NoDefaultPrinter1L "Il n'existe pas d'imprimante système par défaut."

NoDefaultPrinter2L "Reportez-vous au guide d'utilisation pour plus de détails\n\
sur la définition de l'environnement d'impression par défaut LPDEST."
$ %1 is the name of the program, and %2 is either '-edit' or '-populate'.  

RootUserL "%1$s: Vous devez être root pour utiliser l'option %2$s."
$ ----- Command Line Help -----  
$ This message is show when the user enters 'dtprintinfo -help'  
$ on the command line. The %s is a printer list generated by the program.  

CommandLineHelpL "dtprintinfo\n\n\
    Le démarrage de dtprintinfo sans options affiche l'imprimante LPDEST\n\
    ou l'imprimante par défaut si LPDEST n'est pas définie.\n\n\
dtprintinfo -p <imprimante>\n\n\
    Pour visualiser une imprimante spécifique, utilisez l'option -p et\n\
    l'un des noms de files d'attente suivants:\n\n\
%s\n\
dtprintinfo -all\n\n\
    Pour visualiser toutes les imprimantes, utilisez l'option -all.\n\n\
dtprintinfo -populate\n\n\
    L'option -populate n'est disponible que pour root. Cette\n\
    option crée des actions imprimante par défaut pour les imprimantes\n\
    /etc/dt/appconfig/types/LANG. Si LANG n'est pas défini, les actions\n\
    sont créées dans /etc/dt/appconfig/types/C."

NoRemoteCancelL "Le serveur d'impression n'a pas donné le numéro de la tâche d'impression.\nCertains serveurs n'autorisent pas l'annulation éloignée.\n des tâches d'impression. Si c'est le cas, contactez \n l'administrateur du serveur."
FindAcceleratorKeySeq "Ctrl+f"
