$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        DtPrint.msg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  library.
$ **
$ **
$ *****************************************************************************
$ **
$ **  (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994, 1996 Novell, Inc.
$ **  (c) Copyright 1996 Digital Equipment Corporation.
$ **  (c) Copyright 1996 FUJITSU LIMITED.
$ **  (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: DtPrint.msg /main/18 1996/10/31 02:08:59 cde-hp $
$ *****************************************************************************
$ *****************************************************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There MAY be three types of messages in this file:
$ 
$  1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$  2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$  3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************

$quote "

$set 1
$ *************************************************************
$ ** 							     ** 
$ ** PrintSetupBox messages and labels 			     **
$ ** 							     ** 
$ *************************************************************
$ Warning messages
$ Note: do not localize the chars %s
$ 
$quote
1 Invalid workAreaLocation resource setting.

$ Label values
$ 
2 More Printers...
3 Print to File
4 Select File...
5 Cancel
6 Copies:
7 Help
8 Print
9 Printer Name:
10 Setup...
11 Printer Description:

$ Warning messages
$ Note: do not localize the chars %s
12 Copy count out of range.
13 Invalid print setup mode.
14 Print setup mode cannot be changed.

$ Label values
$ 
15 Info...

$ Warning messages
$ Note: do not localize the chars %s
16 Invalid Printer
17 Printer %s not found on print server %s
18 Unable to connect to print server %s
19 X server %s does not support printing
20 A default printer could not be determined
21 An X print server for %s could not be found
22 Printer name missing from specifier: %s
23 Invalid print destination.
24 Unable to convert from compound text.
25 Arguments not needed for resource conversion.

$ Label values
$ 
26 Select Printer Error

$ Warning messages
$ Note: do not localize the chars %s
27 Unable to find any printers.

$ Label values
$ 
28 description unavailable
29 Select File
30 Printer Information
31 Format:
32 Printer Model:
33 Printers:
34 More Printers
35 Print to Printer
36 File Name:
37 Print Setup - Help

$set 2
$ *************************************************************
$ ** 							     ** 
$ ** _DtPrint Frame GUI messages			     **
$ ** 							     ** 
$ *************************************************************
$ 
$quote "

$ Label values for Margin Frame
$ 
1 "Margins"
2 "Top:"
3 "Right:"
4 "Bottom:"
5 "Left:"
$ 
$ Label values for Header/Footer Frame
$ 
6 "Headers and Footers"
7 "Top Left:"
8 "Top Right:"
9 "Bottom Left:"
10 "Bottom Right:"

$set 3
$ *************************************************************
$ ** 							     ** 
$ ** Dt Print Dialog Manager Job Options GUI messages        **
$ ** 							     ** 
$ *************************************************************
$ 
$quote

$ Label values for Margin Frame
1 Send Mail When Done
2 Banner Page Title:
3 Print Command Options:
