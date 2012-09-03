
$ "$XConsortium: dtlp.msg /main/6 1996/10/19 16:11:27 rws $";

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
$ -- The first group of messages all appear on the Print dialog --
$ 


$  This is the message that appears in the "Printer:" field when no
$  printer name has been specified.
10 "Predefinita"

$  This is the label that appears in the title bar of the print dialog.
20 "Stampa"

$  This is the label for the field indicating the file that will be printed.
30 "File:"

$  This is the label for the field indicating the name of the printer.
40 "Stampante:"

$  This is the message that will appear in the "Printer:" field if no printer
$  has been specified.
50 "Predefinita"

$  This is the label for the field indicating the number of copies to print.
60 "Copie:"

$  This is the label for the field indicating the title for the banner page.
70 "Titolo pagina di testa:"

$  This is the label for the field indicating if page numbers are to be added.
80 "Stampare i numeri di pagina"

$  This is thef label for the field indicating additional options for the
$  "lp" command.
90 "Opzioni del comando di stampa:"

$  This is the label on the left button. Pushing this button causes the
$  dialog to disappear and the print operation to start.
100 "Stampare"

$  This is the label on the center button. Pushing this button causes the
$  dialog to disappear and cancels the print operation.
110 "Annullare"

$  This is the label on the right button. Pushing this button causes a
$  help dialog to be posted.
120 "Aiuto"

$ 
$ -- The following are error messages. --
$ 

$  This message is posted when there is a problem with the file specified. 
$  The file name will be appended to the message. One additional message
$  from the list below will also be included in the dialog to provide
$  more details.
130 "Spiacente--Impossibile stampare il file:"

$  This message is added to the error dialog when the file is not printable 
$  or does not exist.
132 "Il file non può essere stampato o non esiste."

$  This message is added to the error dialog when the file is not readable 
$  by the user invoking dtlp.
134 "Autorizzazioni insufficienti per leggere il file."

$  This message is added to the error dialog when there is a general failure 
$  in setting up to print.
136 "Verificare che il file esista."

$  This is the label that appears in the title bar of the error dialog.
160 "Informazioni sulla stampa"

$ 
$  -- Strings used in the Print dialog and Select Printer dialog in CDEnext --
$ 
$  This is the label for the field indicating the file(s) that will be printed.
170 "Nomi dei file: "

$  This is used as the window title of the Select Printer dialog and a button
$  in that dialog.
180 "Selezione della stampante"

$  This is the label for the printer list in the Select Printer dialog.
190 "Stampanti disponibili:"

$  This is the label for the text box in the Select Printer dialog.
200 "Descrizione della stampante:"

$ 
$  -- The following are error messages added for CDEnext --
$ 
$  File is not printable or does not exist.
210 "Il file non può essere stampato o non esiste."

$  File is not printable or does not exist.
220 "Nessuna stampante definita per questo sistema o nessuna stampante abilitata."

$  Printer does not exist.
230 "La stampante non esiste."

$  Printer is not enabled.
240 "La stampante non è abilitata."

$  No default printer.
250 "Nessuna stampante predefinita."

$  Printer not accepting requests.
260 "La stampante non accetta richieste."

$  Number of copies is not a positive integer.
270 "Il valore Copie deve essere positivo."




$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/6 1996/10/19 16:11:27 rws $
