/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: vgmsg.h /main/5 1998/03/04 19:29:00 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vgmsg.h
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Header file for Dtgreet application.
 **
 **                _DtMessage catalog string numbers and default strings
 **		   are specified here. The default strings should match
 **		   the strings in the "C" message catalog.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _VGMSG_H
#define _VGMSG_H


/****************************************************************************
 *
 *  Defines
 *
 ****************************************************************************/

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif

#ifdef sun
#define	FBCONSOLE		"/usr/openwin/bin/fbconsole"
#define OWPATH_ENV		"OPENWINHOME=/usr/openwin"
#endif

/*
 *  message catalog id numbers...
 */
#define MC_LABEL_SET		1
#define MC_LOGIN_LABEL		1
#define MC_PASSWD_LABEL		2
#define MC_OK_LABEL		3
#define MC_CLEAR_LABEL		4
#define MC_OPTIONS_LABEL	5
#define MC_HELP_LABEL		6
#define MC_RS_LABEL		7
#define MC_NW_LABEL		8
#define MC_COPY_LABEL		9
/* #define MC_FS_LABEL		10 */
#define MC_LANG_LABEL		11
#define MC_START_LABEL		12
#define MC_CANCEL_LABEL		13
#define MC_HELP_LABEL_2		14
#define MC_DTLITE_LABEL	15
#define MC_GREET_LABEL		16
#define MC_DT_LABEL		17
#define MC_SUS1_LABEL		18
#define MC_SUS2_LABEL		19
#define MC_SUS3_LABEL		20
#define MC_RES_LABEL		21
#define MC_START_LBLDPY		22
#define MC_WAIT_LABEL		23
#define MC_X_LABEL		24
#define MC_SES_LABEL		25
#define MC_PERS_GREET_LABEL     26
#define MC_FS_LABEL		27

/*
 *  Sun specific message catalog id numbers in MC_LABEL_SET messages.
 *  (Starting at a random uneven higher number to allow some room  
 *  for some shared sample source message additions. 
 */
#define MC_USER_DT_LABEL	73
#define MC_TO_LABEL		74


#define MC_ERROR_SET		2
#define MC_LOGIN		1
#define MC_HOME			2
#define MC_MAX_USERS		3
#define MC_PASSWD_EXPIRED	4
#define MC_BAD_UID		5
#define MC_BAD_GID		6
#define MC_BAD_AID		7
#define MC_BAD_AFLAG		8
#define MC_NO_LOGIN		9
#define MC_BAD_ROOT		10
#define MC_LOG_REBUILD		11
#define MC_LOG_RESCAN		12
#define MC_LOG_REREADCFG	13
#define MC_LOG_REREADSRV	14
#define MC_LOG_REREADACC	15
#define MC_NO_MEMORY		16

#define MC_HELP_SET		3
#define MC_HELP			1
#define MC_SYSTEM		2
#define MC_HELP_CHOOSER         3

#define MC_LOG_SET		4
#define MC_LOG_ACC_FILE		1
#define MC_LOG_HOST_ENT		2
#define MC_LOG_ACC_DPY		3
#define MC_LOG_ACC_CTL		4
#define MC_LOG_DOMAIN		5
#define MC_LOG_SRV_OPEN		6
#define MC_LOG_SRV_WRT		7
#define MC_LOG_SAVE_ADDR	8
#define MC_LOG_NET_CFG		9
#define MC_LOG_SET_AUTH		10
#define MC_LOG_LCK_AUTH		11
#define MC_LOG_NOT_AUTH		12
#define MC_LOG_NO_CREATE	13
#define MC_LOG_NO_LOCK		14
#define MC_LOG_NO_SRVACC	15
#define MC_LOG_NEW_DPY		16
#define MC_LOG_NO_ERRLOG	17
#define MC_LOG_MAX_LOGFILE	18
#define MC_LOG_MISS_NAME	19
#define MC_LOG_MISS_TYPE	20
#define MC_LOG_INV_HOSTNM	21
#define MC_LOG_BAD_DPYTYPE	22
#define MC_LOG_PSEUDO		23
#define MC_LOG_GET_RSC		24
#define MC_LOG_NO_SPACE		25
#define MC_LOG_NO_OPENCFG	26
#define MC_LOG_EXTRA_ARG	27
#define MC_LOG_NO_ARGS		28
#define MC_LOG_NO_EXESRV	29
#define MC_LOG_FAIL_FORK	30
#define MC_LOG_SRV_DIED		31
#define MC_LOG_HUNG_DPY		32
#define MC_LOG_FAIL_SRVOPEN	33
#define MC_LOG_FATAL_IO		34
#define MC_LOG_X_ERR		35
#define MC_LOG_FAIL_START	36
#define MC_LOG_NO_SECDPY	37
#define MC_LOG_NO_SECKEY	38
#define MC_LOG_NO_BLSACCT	39
#define MC_LOG_NO_BLSPACCT	40
#define MC_LOG_NO_VFYLVL	41
#define MC_LOG_NO_BLSUSR	42
#define MC_LOG_AFS_FAIL		43
#define MC_LOG_FAIL_SETUID	44
#define MC_LOG_NO_HMDIR		45
#define MC_LOG_SES_EXEFAIL	46
#define MC_LOG_NO_CMDARG	47
#define MC_LOG_NO_SESFORK	48
#define MC_LOG_NO_KILLCL	49
#define MC_LOG_NO_EXE		50
#define MC_LOG_NO_FORK		51
#define MC_LOG_IMPROP_AUTH	52
#define MC_LOG_NO_DTGREET	53
#define MC_LOG_NO_FORKCG	54
#define MC_LOG_FAIL_SOCK	55
#define MC_LOG_ERR_BIND		56
#define MC_LOG_MAKEENV		57
#define MC_LOG_SETENV		58
#define MC_LOG_PARSEARGS	59
#define MC_LOG_NO_SCAN		60
#define MC_LOG_MORE_GRP		61
#define MC_LOG_NOT_SUSER	62
#define MC_LOG_INV_EVENT	63
#define MC_LOG_ERR_ERRNO	64
#define MC_LOG_AFS_FAILATH	65
#define MC_LOG_ACC_EXP		66
#define MC_LOG_DEADSRV		67
#define MC_LOG_NO_LOGOBIT	68
#define MC_LOG_NO_MSGCAT	69
#define MC_LOG_NO_DPYINIT	70
#define MC_LOG_BAD_MATTE	71
#define MC_LOG_BAD_HMATTE	72

#define MC_CHOOSER_SET          5
#define MC_CHOOSER_TITLE        1
#define MC_CHOOSER_HEADING      2
#define MC_UPDATE_LABEL         3


/*
 *  message catalog default strings...
 */

#define MC_DEF_LOGIN_LABEL	"Please enter your user name"
#define MC_DEF_PASSWD_LABEL	"Please enter your password"
#define MC_DEF_OK_LABEL		"OK"
#define MC_DEF_CLEAR_LABEL	"Start Over"
#define MC_DEF_OPTIONS_LABEL	"Options"
#define MC_DEF_HELP_LABEL	"Help"
#define MC_DEF_RS_LABEL		"Reset Login Screen"
#define MC_DEF_NW_LABEL		"Command Line Login"
#define MC_DEF_COPY_LABEL	"Version..."
#define MC_DEF_FS_LABEL		"Failsafe Session"
#define MC_DEF_USER_DT_LABEL	"User's Last Desktop"
#define MC_DEF_LANG_LABEL	"Language"
#define MC_DEF_TO_LABEL		"to"
#define MC_DEF_START_LABEL	"Start DT"
#define MC_DEF_CANCEL_LABEL	"Cancel"
#define MC_DEF_DTLITE_LABEL	"DT Lite Session"
#define MC_DEF_GREET_LABEL	"Welcome to"
#define MC_DEF_PERS_GREET_LABEL	"Welcome %s"
#define MC_DEF_DT_LABEL		"Regular Desktop"
#define MC_DEF_SES_LABEL	"Session"

#define MC_DEF_LOGIN		"Login incorrect; please try again."
#define MC_DEF_HOME		"Unable to change to home directory."
#define MC_DEF_MAX_USERS	"Sorry. Maximum number of users already logged in."
#define MC_DEF_BAD_UID		"Login error, invalid user id."
#define MC_DEF_BAD_GID		"Login error, invalid group id."
#define MC_DEF_BAD_AID		"Login error, invalid audit id."
#define MC_DEF_BAD_AFLAG	"Login error, invalid audit flag."
#define MC_DEF_NO_LOGIN		"Logins are currently disabled."


#define MC_DEF_PASSWD_EXPIRED	\
"\n\
You need to choose an initial password for your account,\n\
or your current password has expired.\n\
\n\
After you set your password, you must log in again\n\
using the new password.\
"

    
/*
 *  default help message...
 */

#define MC_DEF_HELP 	\
"\n\
Login Help:\n\
\n\
Use the login screen to identify yourself to the computer.\n\
\n\
To log in:\n\
       1) Type your user name and press <Enter> or click [OK].\n\
          (if you have not created a user, enter \"root\" as the\n\
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
Then find the Login Manager help volume.\n\
"

/*
 *  system name set to "unknown" message...
 */
#define MC_DEF_SYSTEM	\
"\n\
Your workstation does not have a unique name. It is currently\n\
set to \"unknown\". It is recommended that you name this\n\
workstation.\n\
"	   
#define MC_DEF_HELP_CHOOSER \
"\n\
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
"
 

#define MC_DEF_SUS1_LABEL	"* Suspending Desktop Login...\r\n*\r\n"
#define MC_DEF_SUS2_LABEL	"* Press [Enter] for a login prompt.\r\n*\r\n"
#define MC_DEF_SUS3_LABEL	"* Log in. Desktop Login will resume shortly after you log out.\r\n*\r\n"
#define MC_DEF_RES_LABEL	"* Resuming Desktop Login...\r\n*\r\n"
#define MC_DEF_START_LBLDPY	"* Starting Desktop Login on display %1$s...\r\n*\r\n"
#define MC_DEF_WAIT_LABEL	"* Wait for the Desktop Login screen before logging in.\r\n*\r\n"
#define MC_DEF_X_LABEL		"* The X-server can not be started on display %1$s...\r\n*\r\n"

#define MC_DEF_BAD_ROOT		"Only root wants to run %1$s\n"
#define MC_DEF_LOG_REBUILD	"Rebuilding default language list from %1$s\n"
#define MC_DEF_LOG_RESCAN	"Rescanning both config and servers files\n"
#define MC_DEF_LOG_REREADCFG	"Rereading configuration file %1$s\n"
#define MC_DEF_LOG_REREADSRV	"Rereading servers file %1$s\n"
#define MC_DEF_LOG_REREADACC	"Rereading access file %1$s\n"
#define MC_DEF_NO_MEMORY	"dtlogin: out of memory in routine\n"


#define MC_DEF_LOG_ACC_FILE	"Access file \"%1$s\", host \"%2$s\" not found\n"
#define MC_DEF_LOG_HOST_ENT	"ReadHostEntry\n"
#define MC_DEF_LOG_ACC_DPY	"Access file %1$2s, display %1$2s unknown\n"
#define MC_DEF_LOG_ACC_CTL	"Cannot open access control file %1$s, no XDMCP requests will be granted\n"
#define MC_DEF_LOG_DOMAIN	"Domain/OS authentication error: %1$s (%2$s/%3$s)"
#define MC_DEF_LOG_SRV_OPEN	"Can not open server authorization file %1$s\n"
#define MC_DEF_LOG_SRV_WRT	"Cannot write server authorization file %1$s\n"
#define MC_DEF_LOG_SAVE_ADDR	"saveAddr"
#define MC_DEF_LOG_NET_CFG	"Trouble getting network interface configuration"
#define MC_DEF_LOG_SET_AUTH	"setAuthNumber"
#define MC_DEF_LOG_LCK_AUTH	"Can not lock authorization file %1$s or backup %2$s\n"
#define MC_DEF_LOG_NOT_AUTH	"Can not move authorization into place\n"
#define MC_DEF_LOG_NO_CREATE	"Can not create/lock pid file %1$s\n"
#define MC_DEF_LOG_NO_LOCK	"Can not lock pid file %1$s, another Dtlogin is running (pid %2$d)\n"
#define MC_DEF_LOG_NO_SRVACC	"Can not access servers file %1$s\n"
#define MC_DEF_LOG_NEW_DPY	"NewDisplay"
#define MC_DEF_LOG_NO_ERRLOG	"Can not open errorLogFile %1$s\n"
#define MC_DEF_LOG_MAX_LOGFILE	"TrimErrorLog(): maximum size of error log file is 200Kb.\n"
#define MC_DEF_LOG_MISS_NAME	"Missing display name in servers file\n"
#define MC_DEF_LOG_MISS_TYPE	"Missing display type for %1$s\n"
#define MC_DEF_LOG_INV_HOSTNM	"Hostname %1$s is invalid. Setting DISPLAY to ':0'\n"
#define MC_DEF_LOG_BAD_DPYTYPE	"Unacceptable display type %1$s for display %2$s\n"
#define MC_DEF_LOG_PSEUDO	"Pseudo reset timed out.\n"
#define MC_DEF_LOG_GET_RSC	"GetResource"
#define MC_DEF_LOG_NO_SPACE	"no space for argument realloc\n"
#define MC_DEF_LOG_NO_OPENCFG	"Can not open configuration file %1$s\n"
#define MC_DEF_LOG_EXTRA_ARG	"Extra arguments on command line:"
#define MC_DEF_LOG_NO_ARGS	"StartServer(): no arguments\n"
#define MC_DEF_LOG_NO_EXESRV	"Server %1$s cannot be executed\n"
#define MC_DEF_LOG_FAIL_FORK	"StartServer() fork failed, sleeping\n"
#define MC_DEF_LOG_SRV_DIED	"Server unexpectedly died\n"
#define MC_DEF_LOG_HUNG_DPY	"Hung in XOpenDisplay(%1$s) attempt #%2$d, aborting.\n"
#define MC_DEF_LOG_FAIL_SRVOPEN	"Server open attempt #%1$d failed for %2$s, giving up\n"
#define MC_DEF_LOG_FATAL_IO	"Fatal IO error %d (%1$s)\n"
#define MC_DEF_LOG_X_ERR	"X error\n"
#define MC_DEF_LOG_FAIL_START	"Session start failed\n"
#define MC_DEF_LOG_NO_SECDPY	"WARNING: display %1$s could not be secured\n"
#define MC_DEF_LOG_NO_SECKEY	"WARNING: keyboard on display %1$s could not be secured\n"
#define MC_DEF_LOG_NO_BLSACCT	"BLS - Could not obtain account information.\n"
#define MC_DEF_LOG_NO_BLSPACCT	"BLS - Could not obtain protected account information.\n"
#define MC_DEF_LOG_NO_VFYLVL	"BLS - Could not verify sensitivity level.\n"
#define MC_DEF_LOG_NO_BLSUSR	"BLS - Could not change to user: %1$s.\n"
#define MC_DEF_LOG_AFS_FAIL	"AFS - setgroups() failure when starting session\n"
#define MC_DEF_LOG_FAIL_SETUID	"Setuid failed for user %s, errno = %d\n"
#define MC_DEF_LOG_NO_HMDIR	"No home directory %1$s for user %2$s, using /\n"
#define MC_DEF_LOG_SES_EXEFAIL	"Session execution failed %1$s\n"
#define MC_DEF_LOG_NO_CMDARG	"Session has no command/arguments\n"
#define MC_DEF_LOG_NO_SESFORK	"Can not start session for %s, fork failed\n"
#define MC_DEF_LOG_NO_KILLCL	"Dtlogin can not kill client\n"
#define MC_DEF_LOG_NO_EXE	"Can not execute %1$s\n"
#define MC_DEF_LOG_NO_FORK	"Can not fork to execute %1$s\n"
#define MC_DEF_LOG_IMPROP_AUTH	"Improper authentication name \"%1$s\". Use AFS, Kerberos or the default.\n"
#define MC_DEF_LOG_NO_DTGREET	"Can not execute Dtgreet %1$s for %2$s\n"
#define MC_DEF_LOG_NO_FORKCG	"Can not fork to execute Dtgreet %1$s\n"
#define MC_DEF_LOG_FAIL_SOCK	"Socket creation failed on port %1$d\n"
#define MC_DEF_LOG_ERR_BIND	"error binding socket address %1$d, errno = %2$d\n"
#define MC_DEF_LOG_MAKEENV	"makeEnv"
#define MC_DEF_LOG_SETENV	"setEnv"
#define MC_DEF_LOG_PARSEARGS	"parseArgs"
#define MC_DEF_LOG_NO_SCAN	"Scan of %1$s directory exceeded %2$d seconds. Terminating scan...\n"
#define MC_DEF_LOG_MORE_GRP	"%s belongs to more than %1$d groups, %2$s ignored\n"
#define MC_DEF_LOG_NOT_SUSER	"audwrite() error: caller is not superuser\n"
#define MC_DEF_LOG_INV_EVENT	"audwrite() error: invalid event number in audit record.\n"
#define MC_DEF_LOG_ERR_ERRNO	"audwrite() error: errno = %1$d\n"
#define MC_DEF_LOG_AFS_FAILATH	"AFS authentication failure: %1$s\n"
#define MC_DEF_LOG_ACC_EXP	"Account for %1$s has expired. Please see your System Administrator.\n"
#define MC_DEF_LOG_DEADSRV	"Server dead  %1$s\n"
#define MC_DEF_LOG_NO_LOGOBIT	"Can't open logo bitmap file: %1$s\n"
#define MC_DEF_LOG_NO_MSGCAT	"cannot read \"%1$s\" message catalog. Using default language."
#define MC_DEF_LOG_NO_DPYINIT	"display %1$s could not be initialized, server may be grabbed.\n"
#define MC_DEF_LOG_BAD_MATTE	"(%1$s): Login matte width exceeds display, using default. Matte units are pixels.\n"
#define MC_DEF_LOG_BAD_HMATTE	"(%1$s) Login matte height exceeds display, using default.  Matte units are pixels.\n"

#define MC_DEF_CHOOSER_TITLE  "Select a server to log into:"
#define MC_DEF_CHOOSER_HEADING        "Server Name                    Server Information"
#define MC_DEF_UPDATE_LABEL   " Update List "

#endif /* _VGMSG_H */
