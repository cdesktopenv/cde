$ $XConsortium: pam_dce.msg /main/2 1996/07/24 19:04:00 drk $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        pam_dce.msg
$ **
$ **  Project:     pam_dce
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for pam_unix module.
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1996 Sun Microsystems, Inc.
$ **     All Rights reserved
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *****************************************************************************
$
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$
$ There may be three types of messages in this file:
$
$  1. Messages that appear in dialogs or are displayed to the user.
$
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$
$  2. Messages that only appear in the error log file. 
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

$set 1

$ dce_acct_mgmt.c 
1 Warning: Your DCE %s has expired.
2 Warning: Your DCE %s will expire within %d hour%s.
3 Warning: Your DCE %s will expire in 2 days.\n
4 Warning: Your DCE passwd has expired.

$ dce_authenticate.c
10 Password: 
11 DCE Password: 
12 Error: Your DCE Account has expired.\n

$ dce_password.c
20 Old DCE password: 
21 New DCE password:
22 Re-enter new DCE password: 

$ dce_setcred.c
30 warning: Unable to obtain DCE credentials.\n

