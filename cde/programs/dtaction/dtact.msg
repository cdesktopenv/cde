$ $XConsortium: dtact.msg /main/3 1995/11/01 10:52:05 rswiston $
$ ******************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be thre types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$    These messages are the default, and they should ALL BE LOCALIZED.
$    Note that these messages do NOT have any identification (see the
$    comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$    The localization of these messages is OPTIONAL.  These message are
$    identified by the following:
$ 
$    MESSAGES xx-yy IN SET ZZ WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$    These messages are identified by the following:
$ 
$    DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END  (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************


$ ******************************************************************************
$ 
$ The following are the messages for the dtaction client.  Here are 
$ instructions for displaying each dialog:
$ 
$ 1) To display the dialog prompting for a user password, run dtaction
$    with the '-user <name>' option, where <name> is a valid login name
$    on your system.
$ 
$ 2) To display the invalid password dialog, follow the steps in (1), and
$    enter a bogus password; select the 'Ok' button.
$ 
$ 3) To display the unknown user dialog, run dtaction with the
$    '-user <name>' option, but specify a bogus <name>; i.e. one which
$    is not listed in /etc/passwd.
$ 
$ ******************************************************************************

$set 1

$ This is the format string used when logging a change of user to the
$ file /usr/adm/sulog; it is written to the sulog file anytime the '-user'
$ option is used.  The fields are:
$  <appl Name> <month>/<day> <hour>:<min> <success> <tty> <old Name> <new Name>
$ The only fields which need to be customized are the date and time fields.
1 dtaction %1$.2d/%2$.2d %3$.2d:%4$.2d %5$1.1s %6$s %7$s-%8$s\n

$ This is the label for the Ok button in the dialogs which collect
$ the user's password, the invalid password dialog, and the unknown user
$ error dialog.
2 OK

$ This is the label for the error dialog which occurs when the user enters
$ and invalid password into the password dialog.
3 The password you entered does not match\nthe password for user %s.\n\nPlease reenter the password, or select the\nCancel button to terminate the operation.

$ This is the title for the password error dialog
4 Action Invoker - Password Error

$ This is the label displayed in the prompt dialog, used to collect the
$ user's password.
5 Enter password for user %s:

$ This is the title for the prompt dialog collecting the user's password.
6 Action Invoker - Password

$ This is the label for the Cancel button in the dialog which is used to
$ collect the user's password.
7 Cancel

$ This is the message displayed in the 'unknown user' dialog
8 The user '%s' is an unknown user name.\n\nThe requested action will not be executed.

$ This is the title for the 'unknown user' dialog
9 Action Invoker - Unknown User

$ This is for when the user has failed to supply an action name
10 No action name specified.\n

$ This is for when we are unable to open the display
11 Can't open display.
