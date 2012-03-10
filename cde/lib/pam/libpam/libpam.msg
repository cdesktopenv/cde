$ $XConsortium: libpam.msg /main/3 1996/07/24 19:03:44 drk $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        libpam.msg
$ **
$ **  Project:     libpam
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for libpam library.
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1995 Sun Microsystems, Inc.
$ **     (c) Copyright 1995 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
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

$set 2

$ PAM_SUCCESS 
1 Success

$ PAM_OPEN_ERR 
2 Dlopen failure

$ PAM_SYMBOL_ERR 
3 Symbol not found

$ PAM_SERVICE_ERR 
4 Error in underlying service module

$ PAM_SYSTEM_ERR 
5 System error

$ PAM_BUF_ERR 
6 Memory buffer error

$ PAM_CONV_ERR 
7 Conversation failure

$ PAM_PERM_DENIED 
8 Permission denied

$ PAM_MAXTRIES 
9 Maximum number of attempts exceeded

$ PAM_AUTH_ERR 
10 Authentication failed

$ PAM_AUTHTOKEN_REQD 
11 Get new authentication token

$ PAM_CRED_INSUFFICIENT 
12 Insufficient credentials

$ PAM_AUTHINFO_UNAVAIL 
13 Can not retrieve authentication info

$ PAM_USER_UNKNOWN 
14 No account present for user

$ PAM_CRED_UNAVAIL 
15 Can not retrieve user credentials

$ PAM_CRED_EXPIRED 
16 User credentials have expired

$ PAM_CRED_ERR 
17 Failure setting user credentials

$ PAM_ACCT_EXPIRED 
18 User account has expired

$ PAM_AUTHTOK_EXPIRED 
19 User password has expired

$ PAM_SESSION_ERR 
20 Can not make/remove entry for session

$ PAM_AUTHTOK_ERR 
21 Authentication token manipulation error

$ PAM_AUTHTOK_RECOVERY_ERR 
22 Authentication token can not be recovered

$ PAM_AUTHTOK_LOCK_BUSY 
23 Authentication token lock busy

$ PAM_AUTHTOK_DISABLE_AGING 
24 Authentication token aging disabled

$ PAM_NO_MODULE_DATA 
25 Module specific data not found

$ PAM_IGNORE 
26 Ignore module

$ PAM_ABORT 
27 General PAM failure

$ Unknown Error
28 Unknown Error

$ Following messages are from pam_framework_util.c
29 pam_sm_setcred: %s
30 setproc_cred: %s
31 password: 

$ from pam_framework.c, default prompt
32 Please enter user name: 
