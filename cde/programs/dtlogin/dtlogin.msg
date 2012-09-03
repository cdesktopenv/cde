$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *

$ "$XConsortium: dtlogin.msg /main/6 1996/11/15 08:46:31 barstow $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlogin.msg
$ **
$ **  Project:     CDE dtlogin
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlogin
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *
$ * 
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 1
$ set 1 is labels
$ 
$ messages 1 through 6 are button labels on the login screen

1 Please enter your user name
2 Please enter your password
3 OK
4 Start Over
5 Options
6 Help

$ 
$ messages 7 through 11 are options button menu items
$ 

7 Reset Login Screen
8 Command Line Login
9 Version...
$ message 10 is replaced by 27
10 Single-window Session
11 Language

$ 
$ messages 12 through 14 are miscellaneous button labels
$ 
12 Start DT
13 Cancel
14 Help


$ 
$ message 15, 17 are more option button menu items 
$ message 16 is the welcome message
$ 
15 DT Lite Session
16 Welcome to
17 Regular Desktop

$ 
$ message 18-24
$ 
18 * Suspending Desktop Login...\r\n*\r\n
19 * Press [Enter] for a login prompt.\r\n*\r\n
20 * Log in. Desktop Login will resume shortly after you log out.\r\n*\r\n
21 * Resuming Desktop Login...\r\n*\r\n
22 * Starting Desktop Login on display %1$s...\r\n*\r\n
23 * Wait for the Desktop Login screen before logging in.\r\n*\r\n
24 * The X-server can not be started on display %1$s...\r\n*\r\n

$ messages 25 is a options button menu item
25 Session

$ 
$ 26 replaces 16 as the greeting after a user name has been 
$ entered. The %s is replaced by the user name.
$ 
26 Welcome %s
$ 
$ 27 replaces 10 as the label for the failsafe option menu item
$ 
27 Failsafe Session

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room  
$  for some shared sample source message additions. 
$ 
73 User's Last Desktop
$ 
$  Following used in Language menu.  
$  "to" used in phrases like "ja to zh" locales
74 to

$set 2
$ set 2 is error messages
$ 

1 Login incorrect; please try again.
2 Unable to change to home directory.
3 Sorry. Maximum number of users already logged in.
$ message 4 is a lengthy message that should be localized to correspond to 
$ the language.  
4 \n\
You need to choose an initial password for your account,\n\
or your current password has expired.\n\
\n\
After you set your password, you must log in again\n\
using the new password.
5 Login error, invalid user id.
6 Login error, invalid group id.
7 Login error, invalid audit id.
8 Login error, invalid audit flag.
9 Logins are currently disabled.
10 Only root wants to run %1$s\n
11 Rebuilding default language list from %1$s\n
12 Rescanning both config and servers files\n
13 Rereading configuration file %1$s\n
14 Rereading servers file %1$s\n
15 Rereading access file %1$s\n
16 dtlogin: out of memory in routine\n


$set 3
$ set 3 is help text
$ 

$ message 1 is generated when the help button on the login screen is clicked.
$ 1 \n\
$ Login Help:\n\
$ \n\
$ Use the login screen to identify yourself to the computer.\n\
$ \n\
$ To log in:\n\
$        1) Type your user name and press <Enter> or click [OK].\n\
$           (if you have not created a user, enter "root" as the\n\
$           user name)\n\
$        2) Type your password and press <Enter> or click [OK].\n\
$ \n\
$ [Start Over]\n\
$    Erases your entries so you can re-enter your user name and\n\
$    try again.\n\
$ \n\
$ [Options]\n\
$    Allows you to change session options:\n\
$ \n\
$    With the mouse pointer over the [Options] button, press and\n\
$    hold mouse button 1.  Drag the pointer through the choices.\n\
$    Releasing the mouse button with the pointer on an item will\n\
$    select that item. You may choose:\n\
$ \n\
$   Language - to change the session language\n\
$   Session - to select preferred user Desktop\n\
$     OpenWindows Desktop - for OW \n\
$     Common Desktop Desktop - for CDE \n\
$     User's Last Desktop - from previous login \n\
$     FailSafe Session - for a single terminal window\n\
$   Command Line Login - to switch to a command line environment\n\
$   Reset Login Screen - to restart the X-server\n\
$ \n\
$ Click anywhere on the background with any mouse button to\n\
$ refresh the screen, for example, if system messages make the\n\
$ screen unreadable.\n\
$ \n\
$ For more help information, log in first, then click on the Help\n\
$ Manager icon in the Front Panel (Books with a question mark).\n\
$ Then find the Login Manager help volume.\n\

$ 
$ 1 NEW-
$ 
$ Under [Options] there is an item called "Session".  Under
$ Session there are five items and these items have been
$ translated in other files.  To be consistent with the other
$ translations, the translations used below should be the same
$ as the other translations.  The items and their file are
$ listed below:
$ 
$   1. Current   - _common.session.tmsg ; set 1, message 2
$ 
$   2. Home      - _common.session.tmsg ; set 1, message 1
$ 
$   3. <display-name> - Current   - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Current" should
$         be the same as _common.session.tmsg ; set 1, message 2
$ 
$   4. <display-name> - Home      - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Home" should
$         be the same as _common.session.tmsg ; set 1, message 1
$ 

1 \n\
Login Help:\n\
\n\
Use the login screen to identify yourself to the computer.\n\
\n\
To log in:\n\
       1) Type your user name and press <Enter> or click [OK].\n\
          (if you have not created a user, enter "root" as the\n\
          user name)\n\
       2) Type your password and press <Enter> or click [OK].\n\
\n\
[Start Over]\n\
   Erases your entries so you can re-enter your user name and\n\
   try again.\n\
\n\
[Options]\n\
   Allows you to change session options:\n\
\n\
   With the mouse pointer over the [Options] button, press and\n\
   hold mouse button 1.  Drag the pointer through the choices.\n\
   Releasing the mouse button with the pointer on an item will\n\
   select that item. You may choose:\n\
\n\
   Language - to change the session language\n\
   Session - to select preferred user Desktop\n\
      Current - Starts your most recent session\n\
      Home - Starts your home session (if you set one)\n\
      <display-name> - Current - starts your most recent session for\n\
          the given display\n\
      <display-name> - Home - starts your home session for\n\
          the given display\n\
      FailSafe Session - Starts a failsafe session.\n\
   Command Line Login - to switch to a command line environment\n\
   Reset Login Screen - to restart the X-server\n\
\n\
Click anywhere on the background with any mouse button to\n\
refresh the screen, for example, if system messages make the\n\
screen unreadable.\n\
\n\
For more help information, log in first, then click on the Help\n\
Manager icon that is in the Information Manager slide-up menu\n\
in the Front Panel (Book with an information symbol).\n\
Then find the Login Manager help volume.\n


$ message 2 is generated when you attempt to start a system whose id is
$ set to unknown or if it is not configured for networking.
2 \n\
Your workstation does not have a unique name. It is currently\n\
set to "unknown". It is recommended that you name this\n\
workstation.\n\
\n\

$ message 3 is generated when the help button on the chooser screen is clicked.
3 \n\
Chooser Help:\n\
\n\
Use the chooser screen to choose which server to log into.\n\
\n\
The chooser screen contains a list that has 2 parts for each\n\
entry.  The first part (Server Name) contains the network\n\
name for each available server.  The second part (Server\n\
Information) will contain different information depending on\n\
what login manager is being run on each server.  If a server\n\
is running the CDE Desktop, it should provide information\n\
about the number of users (or tty devices) currently in use,\n\
and load averages (average CPU usage) for the last 5, 10, and\n\
15 minutes.\n\
\n\
To choose a server to log into, just select the line in the\n\
list that contains the name of the server you wish.\n\
\n\
Once you have made your selection, press the ok button.\n\
This will bring up a login screen from the server you selected.\n\
\n\
[Update List]\n\
   This will clear the list and rebuild it by querying the\n\
   servers on the network to see if they will allow logins.\n\
\n\
[Options]\n\
   Allows you to change session options:\n\
\n\
   With the mouse pointer over the [Options] button, press and\n\
   hold mouse button 1.  Drag the pointer through the choices.\n\
   Releasing the mouse button with the pointer on an item will\n\
   select that item. You may choose:\n\
\n\
   Language - to change the session language\n\
   Reset Login Screen - to restart the X-server\n\
\n\
Click anywhere on the background with any mouse button to\n\
refresh the screen, for example, if system messages make the\n\
screen unreadable.\n\
\n\


$set 4
$ set 4 is log error messages
$ 

1 Access file \"%1$s\", host \"%2$s\" not found\n
2 ReadHostEntry\n
3 Access file %1$s, display %2$s unknown\n
4 Cannot open access control file %1$s, no XDMCP reqeusts will be granted\n
5 Domain/OS authentication error: %1$s (%2$s/%3$s)
6 Can not open server authorization file %1$s\n
7 Cannot write server authorization file %1$s\n
8 saveAddr
9 Trouble getting network interface configuration
10 setAuthNumber
11 Can not lock authorization file %1$s or backup %2$s\n
12 Can not move authorization into place\n
13 Can not create/lock pid file %1$s\n
14 Can not lock pid file %1$s, another Dtlogin is running (pid %2$d)\n
15 Can not access servers file %1$s\n
16 NewDisplay
17 Can not open errorLogFile %1$s\n
18 TrimErrorLog(): maximum size of error log file is 200Kb.\n
19 Missing display name in servers file\n
20 Missing display type for %1$s\n
21 Hostname %1$s is invalid. Setting DISPLAY to ':0'\n
22 Unacceptable display type %1$s for display %2$s\n
23 Pseudo reset timed out.\n
24 GetResource
25 no space for argument realloc\n
26 Can not open configuration file %1$s\n
27 Extra arguments on command line:
28 StartServer(): no arguments\n
29 Server %1$s cannot be executed\n
30 StartServer() fork failed, sleeping\n
31 Server unexpectedly died\n
32 Hung in XOpenDisplay(%1$s) attempt #%2$d, aborting.\n
33 Server open attempt #%1$d failed for %2$s, giving up\n
34 Fatal IO error %d (%1$s)\n
35 X error\n
36 Session start failed\n
37 WARNING: display %1$s could not be secured\n
38 WARNING: keyboard on display %1$s could not be secured\n
39 BLS - Could not obtain account information.\n
40 BLS - Could not obtain protected account information.\n
41 BLS - Could not verify sensitivity level.\n
42 BLS - Could not change to user: %1$s.\n
43 AFS - setgroups() failure when starting session\n
44 Setuid failed for user %s, errno = %d\n
45 No home directory %1$s for user %2$s, using /\n
46 Session execution failed %1$s\n
47 Session has no command/arguments\n
48 Can not start session for %s, fork failed\n
49 Dtlogin can not kill client\n
50 Can not execute %1$s\n
51 Can not fork to execute %1$s\n
52 Improper authentication name \"%1$s\". Use AFS, Kerberos or the default.\n
53 Can not execute Dtgreet %1$s for %2$s\n
54 Can not fork to execute Dtgreet %1$s\n
55 Socket creation failed on port %1$d\n
56 error binding socket address %1$d, errno = %2$d\n
57 makeEnv
58 setEnv
59 parseArgs
60 Scan of %1$s directory exceeded %2$d seconds. Terminating scan...\n
61 %s belongs to more than %1$d groups, %2$s ignored\n
62 audwrite() error: caller is not superuser\n
63 audwrite() error: invalid event number in audit record.\n
64 audwrite() error: errno = %1$d\n
65 AFS authentication failure: %1$s\n
66 Account for %1$s has expired. Please see your System Administrator.\n
67 Server dead  %1$s\n
68 Can't open logo bitmap file: %1$s\n
69 cannot read \"%1$s\" message catalog. Using default language.
70 display %1$s could not be initialized, server may be grabbed.\n
71 (%1$s): Login matte width exceeds display, using default. Matte units are pixels.\n
72 (%1$s) Login matte height exceeds display, using default.  Matte units are pixels.\n

$set 5
$ set 5 is for other stuff specific to chooser.
1 Select a server to log into:
$ 1 is the title/instructions for the chooser dialog
2 Server Name                    Server Information
$                                ^
$                                | This needs to start in this column
$ 2 is the headings for the list
3  Update List 

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/6 1996/11/15 08:46:31 barstow $
