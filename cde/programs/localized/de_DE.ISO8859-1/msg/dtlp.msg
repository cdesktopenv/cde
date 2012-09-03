$ "$XConsortium: dtlp.msg /main/5 1996/10/19 14:27:12 rws $";
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlp.msg
$ **
$ **  Project:     DT printer utility (dtlp)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlp
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1990, 1992, 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1994 International Business Machines Corp.
$ ** (c) Copyright 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1994 Novell, Inc.
$ **
$ **     All Rights reserved
$ **
$ *****************************************************************************
$ *************************************<+>*************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * You don't need to localize the following sets:
$ *
$ * Please localize the following sets:
$ *  sets 1
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************
$quote "

$set 1
$ 
$  -- The first group of messages all appear on the Print dialog --
$ 
$  This is the message that appears in the "Printer:" field when no
$  printer name has been specified.

10 "Standard"
$  This is the label that appears in the title bar of the print dialog.

20 "Drucken"
$  This is the label for the field indicating the file that will be printed.

30 "Datei:"
$  This is the label for the field indicating the name of the printer.

40 "Drucker:"
$  This is the message that will appear in the "Printer:" field if no printer
$  has been specified.

50 "Standard"
$  This is the label for the field indicating the number of copies to print.

60 "Exemplare:"
$  This is the label for the field indicating the title for the banner page.

70 "Titel des Deckblatts:"
$  This is the label for the field indicating if page numbers are to be added.

80 "Seitenzahlen drucken"
$   ???SL: please check
$   This is thef label for the field indicating additional options for the
$  "lp" command.

90 "Befehlsoptionen drucken:"
$   ???SL: please check
$  This is the label on the left button. Pushing this button causes the
$  dialog to disappear and the print operation to start.

100 "Drucken"
$  This is the label on the center button. Pushing this button causes the
$  dialog to disappear and cancels the print operation.

110 "Abbrechen"
$  This is the label on the right button. Pushing this button causes a
$  help dialog to be posted.

120 "Hilfe"
$ 
$  -- The following are error messages. --
$ 
$  This message is posted when there is a problem with the file specified.
$  The file name will be appended to the message. One additional message
$  from the list below will also be included in the dialog to provide
$  more details.

130 "Drucken nicht möglich:"
$  This message is added to the error dialog when the file is not printable
$  or does not exist.

132 "Datei nicht druckbar oder nicht vorhanden."
$  This message is added to the error dialog when the file is not readable
$  by the user invoking dtlp.

134 "Keine Berechtigung zum Lesen dieser Datei."
$  This message is added to the error dialog when there is a general failure
$  in setting up to print.

136 "Bitte prüfen, ob diese Datei vorhanden ist."
$  This is the label that appears in the title bar of the error dialog.

160 "Information drucken"
$   ???SL: please check

$ 
$  -- Strings used in the Print dialog and Select Printer dialog in CDEnext --
$ 
$  This is the label for the field indicating the file(s) that will be printed.
170 "Dateiname(n): "

$  This is used as the window title of the Select Printer dialog and a button
$  in that dialog.
180 "Drucker auswählen"

$  This is the label for the printer list in the Select Printer dialog.
190 "Verfügbare Drucker:"

$  This is the label for the text box in the Select Printer dialog.
200 "Druckerinformation:"

$ 
$  -- The following are error messages added for CDEnext --
$ 
$  File is not printable or does not exist.
210 "Diese Datei ist entweder nicht druckbar oder sie existiert nicht."

$  File is not printable or does not exist.
220 "Es ist entweder kein Drucker für dieses System definiert, oder kein Drucker ist aktiviert."

$  Printer does not exist.
230 "Der Drucker existiert nicht."

$  Printer is not enabled.
240 "Der Drucker ist nicht aktiviert."

$  No default printer.
250 "Es existiert kein Standard-Druckerziel."

$  Printer not accepting requests.
260 "Der Drucker akzeptiert derzeit keine Anforderungen."

$  Number of copies is not a positive integer.
270 "Die Anzahl der Kopien muß eine positive Zahl sein."


$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/5 1996/10/19 14:27:12 rws $"
