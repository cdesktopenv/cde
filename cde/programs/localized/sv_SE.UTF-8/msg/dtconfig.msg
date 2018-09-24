$ $XConsortium: dtconfig.msg /main/3 1995/11/08 13:32:35 rswiston $
$ *****************************************************************************
$ **
$ **  File:        dtconfig.msg
$ **
$ **  Project:     CDE Session Management (dtconfig)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtconfig
$ **
$ **  Message set 10: IBM
$ **  Message set 20: Sun
$ **
$ *****************************************************************************

$ *****************************************************************************
$ **  Message set 10: For IBM platforms
$ *****************************************************************************

$set 10
1 can only be run by the super user. \n
$ 
2 The CDE environment is now set as the default user interface. This\n\
interface will appear on login for all users of the system.\n\
To see this change take effect you must shutdown and restart your system.\n
$ 
3 The Command line is now set as the default user interface. This\n\
interface will appear on login for all users of the system.\n\
To see this change take effect you must shutdown and restart your system.\n
4 The CDE environment is Already set as the default User Interface.\n
$ 
5 The Command line is already set as the default User Interface.\n
$ 
6 usage: %s\n\
  -e (enable auto-start of dtlogin)\n\
  -d (disable auto-start of dtlogin)\n\
  -kill (kill dtlogin)\n\
  -reset (reset dtlogin - reread configuration files)\n
$ 
7 Too many arguments \n
$ 
8  Needs one argument \n
$ 
9  Could not disable xdm, Please refer to xdm man pages to disable\n\
xdm and try again. \n 
$ 
10 Dtlogin is not running \n

$ *****************************************************************************
$ **  Message set 20: For Sun Platforms
$ *****************************************************************************

$set 20
$ 
1  CDE configuration utility
$ 
2   %s -d     (disable auto-start)
3   %s -e     (enable auto-start)
4   %s -kill  (kill dtlogin)
5   %s -reset (reset dtlogin)
6   %s -p     (printer action update)
$ 
$	In above messages 2 through 6, only translate words between
$	left "(" and right ").
$ 
7 %s: this script must be run as root
8 Usage: Too many arguments
9 Usage: Needs one argument
10 option failed
11 could not execute %s
12 disable failed
13 could not remove %s
14 done
15 desktop auto-start disabled.
16 enable failed
17 could not find %s
18 could not create %s
19 could not create %s
20 desktop auto-start enabled.
21 dtlogin kill complete.
22 dtlogin config resources reloaded.
23 printer action update is complete
24 Usage:
