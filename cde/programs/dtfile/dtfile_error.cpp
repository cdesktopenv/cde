XCOMM! CDE_INSTALLATION_TOP/bin/dtksh
XCOMM $XConsortium: dtfile_error.cpp /main/3 1996/04/21 19:29:14 drk $
XCOMM ##################################################################
XCOMM  Display an error dialog using the argument(s) as the message
XCOMM
XCOMM  This script can be used by applications to display an error
XCOMM  dialog when it would be difficult or impossible to do in the
XCOMM  context of the executing program. For example, it can be used
XCOMM  when exec fails in a child process or if an error is detected
XCOMM  before an applications main window can be realized.
XCOMM
XCOMM (c) Copyright 1996 Digital Equipment Corporation.
XCOMM (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
XCOMM (c) Copyright 1993,1994,1996 International Business Machines Corp.
XCOMM (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
XCOMM (c) Copyright 1996 Novell, Inc. 
XCOMM (c) Copyright 1996 FUJITSU LIMITED.
XCOMM (c) Copyright 1996 Hitachi.
XCOMM (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
XCOMM     Novell, Inc.
XCOMM ###################################################################

XCOMM --------------------
XCOMM  OK button callback
XCOMM --------------------

okCallback()
{
   exit
}


XCOMM ------
XCOMM  main
XCOMM ------

MESSAGE="$@"
#ifdef _AIX
MSGCAT=/usr/dt/lib/nls/msg/$LANG/dtfile.cat
TITLE=`dspmsg $MSGCAT -s 32 1 "dt Error"`
#else
TITLE="dt Error"
#endif /* _AIX */

XtInitialize TOP dtError DtError "$@"

XmCreateErrorDialog DIALOG \
                    $TOP \
                    "Test" \
                    dialogTitle:"$TITLE" \
                    messageString:"$MESSAGE"

XCOMM remove cancel and help buttons
XmMessageBoxGetChild CANCEL $DIALOG DIALOG_CANCEL_BUTTON
XmMessageBoxGetChild HELP   $DIALOG DIALOG_HELP_BUTTON
XtUnmanageChild $CANCEL
XtUnmanageChild $HELP

XtAddCallback $DIALOG okCallback "okCallback"

XtManageChild $DIALOG

XtMainLoop
