$ $TOG: dtinfo_start.msg /main/2 1999/09/20 13:27:31 mgreess $
$
$ (c) Copyright 1996 Digital Equipment Corporation.
$ (c) Copyright 1996 Hewlett-Packard Company.
$ (c) Copyright 1996 International Business Machines Corp.
$ (c) Copyright 1996 Sun Microsystems, Inc.
$ (c) Copyright 1996 Novell, Inc. 
$ (c) Copyright 1996 FUJITSU LIMITED.
$ (c) Copyright 1996 Hitachi.
$
$ ******************************************************************************
$
$set 1
$ The messages in this set will only appear in the users
$   "errorlog" file.
$
$ ******************************************************************************
$
$ Messages 1-5: These messages result from an error in a tt_* function
$    call.
$
1 The ttsession(1) process is not running and the ToolTalk\n\
service cannot restart it.
2 The current default process identifier is out of date or invalid.
3 The specified ToolTalk session is out of date or invalid.

$
$ Message 4: Do not localize the word DtInfo_Start.
$
4 The process type 'DtInfo_Start' is not the\n\
name of an installed process type.

5 The error number is not recognized.

6 Cannot invoke an action because malloc(1) failed.

$
$ Message 7: Do not localize the word DISPLAY.  Do not localize the
$   word XtOpenDisplay().
$
7 XtOpenDisplay() failed.  Perhaps the DISPLAY environment\n\
variable is not set or is invalid.

$
$ Message 8: Do not localize the word DtAppInitialize().
$
8 DtAppInitialize() failed.  Perhaps the desktop environment\n\
is not properly installed.

$
$ Message 9: Do NOT localize the word DTINFOLIBDEFAULT
$
9 An InfoLib was not provided in the '%s' message.\n\
The value of the environment variable DTINFOLIBDEFAULT\n\
(%s) will be used for the InfoLib.

$
$ Message 10: Do NOT localize the word DTINFOLIBDEFAULT
$
10 An InfoLib was not provided in the '%s' message\n\
and the environment variable DTINFOLIBDEFAULT was not defined.

$
$ Messages 11-16: These message are the result of the dtinfo_start
$   getting an error while trying to parse a ToolTalk message.
$
$   Do NOT localize the word NULL
$
11 The message operation '%d' is not supported.

12 Message '%s' does not have the required\n\
number of arguments '%d'.

13 Message '%s' specified a NULL action name.\n\
Action '%s' will be invoked.

14 Message '%s' specified a NULL execution host.

15 Message '%s' specified a NULL locale.

16 Message '%s' specified a NULL topic.

$set 2
$ *****************************************************************************
$ **
$ ** Message set for  DieFromToolTalkError Dialog
$ **
$ *****************************************************************************
$ These are the messages for the ToolTalk error dialog
$ "-NEW"
1 Dtinfo: Error
$ "-NEW"
2 "Could not connect to ToolTalk:\n%s\n"
$ "-NEW"
3 OK
