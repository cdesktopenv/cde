
$ "$XConsortium: dtlp.msg /main/5 1996/08/11 17:26:52 pascale $";

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


$  This is the message that appears in the "Printer Name:" field when no
$  printer name has been specified and no there is no default printer.
10 "Default"

$  This is the label that appears in the title bar of the print dialog.
20 "Print"

$  This is the label for the field indicating the file that will be printed.
30 "File:"

$  This is the label for the field indicating the name of the printer.
40 "Printer:"

$  This is the message that will appear in the "Printer:" field if no printer
$  has been specified.
50 "Default"

$  This is the label for the field indicating the number of copies to print.
60 "Copies:"

$  This is the label for the field indicating the title for the banner page.
70 "Banner Page Title:"

$  This is the label for the field indicating if page numbers are to be added.
80 "Print Page Numbers"

$  This is thef label for the field indicating additional options for the
$  "lp" command.
90 "Print Command Options:"

$  This is the label on the left button. Pushing this button causes the
$  dialog to disappear and the print operation to start.
100 "Print"

$  This is the label on the center button. Pushing this button causes the
$  dialog to disappear and cancels the print operation.
110 "Cancel"

$  This is the label on the right button. Pushing this button causes a
$  help dialog to be posted.
120 "Help"

$ 
$  -- The following are error messages. --
$ 

$  This message is posted when there is a problem with the file specified. 
$  The file name will be appended to the message. One additional message
$  from the list below will also be included in the dialog to provide
$  more details.
130 "Sorry--Unable to print:"

$  This message is added to the error dialog when the file is not printable 
$  or does not exist.
132 "This is not a printable file or doesn't exist."

$  This message is added to the error dialog when the file is not readable 
$  by the user invoking dtlp.
134 "You don't have permission to read that file."

$  This message is added to the error dialog when there is a general failure 
$  in setting up to print.
136 "Please check whether this file exists."

$  This is the label that appears in the title bar of the error dialog.
160 "Print Information"

$ 
$  -- Strings used in the Print dialog and Select Printer dialog in CDEnext --
$ 
$  This is the label for the field indicating the file(s) that will be printed.
170 "File Name(s): "

$  This is used as the window title of the Select Printer dialog and a button
$  in that dialog.
180 "Select Printer"

$  This is the label for the printer list in the Select Printer dialog.
190 "Available Printers:"

$  This is the label for the text box in the Select Printer dialog.
200 "Printer Information:"

$ 
$  -- The following are error messages added for CDEnext --
$ 
$  File is not printable or does not exist.
210 "That file is either not printable or it does not exist."

$  File is not printable or does not exist.
220 "There is either no printer defined for this system or no printer is enabled."

$  Printer does not exist.
230 "The printer does not exist."

$  Printer is not enabled.
240 "The printer is not enabled."

$  No default printer.
250 "There is not default printer destination."

$  Printer not accepting requests.
260 "The printer is currently not accepting requests."

$  Number of copies is not a positive integer.
270 "Number of copies must be a positive number."


$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/5 1996/08/11 17:26:52 pascale $
