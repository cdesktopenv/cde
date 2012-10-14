$ $XConsortium: TtIpcMgr.msg /main/4 1996/08/14 14:30:36 rcs $
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
$set	Set_TtIpcMgr
$ The messages in this set will only appear in the users
$   "errorlog" file.
$
$ ******************************************************************************
$ 
$ messages specific to tooltalk interface and operations.
$ Note that many errors defined by tooltalk are passed through
$ and therefore not repeated here.
$

$ 1     "Can't get tt session for X display %s"
1 Can't get tt session for X display %s

$ 2     "Could not set default tooltalk session."
2 Could not set default tooltalk session.

$ 3 Failed to open ToolTalk connection.
3 Failed to open ToolTalk connection.

$ 4     "Could not declare ptype DtInfo."
4 Could not declare ptype DtInfo.

$ Messages 5-10: These messages result from an error in a tt_* function
$    call.
$
5 The ttsession(1) process is not running and the ToolTalk\n\
service cannot restart it.
6 The current default process identifier is out of date or invalid.
7 The specified ToolTalk session is out of date or invalid.

$
$ Message 8: Do not localize the word DtInfo.
$
8 The process type 'DtInfo' is not the\n\
name of an installed process type.

$
$ Message 9: Do not localize the word DtInfoPrint.
$
9 The process type 'DtInfoPrint' is not the\n\
name of an installed process type.

10 The error number is not recognized.

