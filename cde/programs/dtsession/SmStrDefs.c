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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: SmStrDefs.c /main/9 1996/02/02 15:14:33 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmStrDef.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  Contains all resource names and classes for the session manager
 **
 **
 *******************************************************************
 **  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
 **  reserved.  Copying or other reproduction of this program      
 **  except for archival purposes is prohibited without prior      
 **  written consent of Hewlett-Packard Company.                     
 ********************************************************************
 **
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/

/*
 * Global Resource Names
 */
char SmNsessionVersion[] = "sessionVersion";

/*
 * Session Manager Resource Names
 */
/* Resources names for session manager behavior*/
char SmNwmStartup[] = "wmStartupCommand";
char SmNquerySettings[] = "queryServerSettings";
char SmNkeys[] = "keys";
char SmNalarmTime[] = "passwordTimeout";
char SmNmemThreshold[] = "memThreshold";
char SmNcontManagement[] = "contManagement";
char SmNwaitClientTimeout[] = "waitClientTimeout";
char SmNwaitWmTimeout[] = "waitWmTimeout";
char SmNuseMessaging[] = "useMessaging";
char SmNsaveFontPath[] = "saveFontPath";
char SmNcycleTimeout[] = "cycleTimeout";
char SmNlockTimeout[] = "lockTimeout";
char SmNsaverTimeout[] = "saverTimeout";
char SmNrandom[] = "random";
char SmNsaverList[] = "saverList";
char SmNsaveYourselfTimeout[] = "saveYourselfTimeout";
char SmNmergeXdefaults[] = "mergeXdefaults";
char SmNnumSessionsBackedup[] = "numSessionsBackedup";
char SmNignoreEnvironment[] = "ignoreEnvironment";
char SmNxineramaPreferredScreen[] = "xineramaPreferredScreen";


/* Resource names for settings information */

char SmNkeyClick[] = "keyClickPercent";
char SmNbellPercent[] = "bellPercent";
char SmNbellPitch[] = "bellPitch";
char SmNbellDuration[] = "bellDuration";
char SmNledMask[] = "ledMask";
char SmNglobalRepeats[] = "globalAutoRepeats";
char SmNautoRepeats[] = "autoRepeats";
char SmNaccelNum[] = "accelNum";
char SmNaccelDenom[] = "accelDenom";
char SmNthreshold[] = "threshold";
char SmNtimeout[] = "timeout";
char SmNinterval[] = "interval";
char SmNpreferBlank[] = "preferBlank";
char SmNallowExp[] = "allowExp";
char SmNfontPath[] = "fontPath";
char SmNkeySymsPerKey[] = "keySymsPerKeyCode";
char SmNnumKeyCode[] = "numKeyCode";
char SmNkeySyms[] = "keySyms";
char SmNmaxKeyPerMod[] = "maxKeyPerMod";
char SmNmodMap[] = "modMap";
char SmNbuttonMap[] = "buttonMap";
char SmNhostState[] = "hostState";
char SmNhostNames[] = "hostName";
char SmNdidQuerySettings[] = "didQuery";
char SmNshutDownState[] = "shutDownState";
char SmNshutDownMode[] = "shutDownMode";
char SmNdisplayResolution[] = "displayResolution";
char SmNsessionLang[] = "sessionLanguage";

/*
 * Global resource classes
 */
char SmCsessionVersion[] = "SessionVersion";

/*
 * Session Manager Resource Classes
 */
/* Resources classes for session manager behavior*/
char SmCwmStartup[] = "WmStartupCommand";
char SmCquerySettings[] = "QueryServerSettings";
char SmCkeys[] = "Keys";
char SmCalarmTime[] = "PasswordTimeout";
char SmCmemThreshold[] = "MemThreshold";
char SmCContManagement[] = "ContManagement";
char SmCWaitClientTimeout[] = "WaitClientTimeout";
char SmCWaitWmTimeout[] = "WaitWmTimeout";
char SmCUseMessaging[] = "UseMessaging";
char SmCsaveFontPath[] = "SaveFontPath";
char SmCcycleTimeout[] = "CycleTimeout";
char SmClockTimeout[] = "LockTimeout";
char SmCsaverTimeout[] = "SaverTimeout";
char SmCrandom[] = "Random";
char SmCsaverList[] = "SaverList";
char SmCsaveYourselfTimeout[] = "SaveYourselfTimeout";
char SmCmergeXdefaults[] = "MergeXdefaults";
char SmCnumSessionsBackedup[] = "NumSessionsBackedup";
char SmCignoreEnvironment[] = "IgnoreEnvironment";
char SmCxineramaPreferredScreen[] = "XineramaPreferredScreen";

/*
 * Class names for session settings information
 */
char SmCkeyClick[] = "KeyClickPercent";
char SmCbellPercent[] = "BellPercent";
char SmCbellPitch[] = "BellPitch";
char SmCbellDuration[] = "BellDuration";
char SmCledMask[] ="LedMask";
char SmCglobalRepeats[] = "GlobalAutoRepeats";
char SmCautoRepeats[] = "AutoRepeats";
char SmCaccelNum[] = "AccelNum";
char SmCaccelDenom[] = "AccelDenom";
char SmCthreshold[] = "Threshold";
char SmCtimeout[] = "Timeout";
char SmCinterval[] = "Interval";
char SmCpreferBlank[] = "PreferBlank";
char SmCallowExp[] = "AllowExp";
char SmCfontPath[] = "FontPath";
char SmCkeySymsPerKey[] = "KeySymsPerKeyCode";
char SmCnumKeyCode[] = "NumKeyCode";
char SmCkeySyms[] = "KeySyms";
char SmCmaxKeyPerMod[] = "MaxKeyPerMod";
char SmCmodMap[] = "ModMap";
char SmCbuttonMap[] = "ButtonMap";
char SmChostState[] = "HostState";
char SmChostNames[] = "HostName";
char SmCdidQuerySettings[] = "DidQuery";
char SmCshutDownState[] = "ShutDownState";
char SmCshutDownMode[] = "ShutDownMode";
char SmCdisplayResolution[] = "DisplayResolution";
char SmCsessionLang[] = "SessionLanguage";


/*************************************<->*************************************
 *
 *  Session manager resource converter names ...
 *
 *
 *  Description:
 *  -----------
 *
 *************************************<->***********************************/

char SmRContManagement[] = "ContManagement";


/* from Sm.h */

char SM_CLIENT_FILE[] = "dt.session";
char SM_CLIENT_FILE2[] = "dtsession.db";
char SM_RESOURCE_FILE[] = "dt.resources";
char SM_FONT_FILE[] = "dt.font";
char SM_LOW_RES_EXT[] = "l";
char SM_MED_RES_EXT[] = "m";
char SM_HIGH_RES_EXT[] = "h";
char SM_SETTING_FILE[] = "dt.settings";
char SM_CONVERSION_FILE[] = "*CONVERTED*";
char SM_SYSTEM_CLIENT_FILE[] = "sys.session";
char SM_SYSTEM_RESOURCE_FILE[] = "sys.resources";
char SM_SYSTEM_FONT_FILE[] = "sys.font";
char SM_SYSTEM_PATH[] = CDE_CONFIGURATION_TOP "/config";
char SM_SECURE_PATH[] = "/.secure/etc/passwd";
char SM_CURRENT_DIRECTORY[] = "current";
char SM_CURRENT_FONT_DIRECTORY[] = "current.font";
char SM_HOME_FONT_DIRECTORY[] = "home.font";
char SM_HOME_DIRECTORY[] = "home";
char SM_OLD_EXTENSION[] = "old";
char SM_SYSTEM_DIRECTORY[] = "system";
char smEtcFile[] = "sessionetc";
char smExitFile[] = "sessionexit";
char SM_RESOURCE_CLASS[] = "Dtsession";
char SM_RESOURCE_NAME[] = "dtsession";
char SM_HELP_VOLUME[] = "FPanel";
char SM_RUNNING_LOCK[] = "dtsession_running";
char SM_SCREEN_SAVER_NAME[] = "screensaver";

/*
 * Strings for the XSMP
 */
char SM_SESSION_MANAGER[] 	= "SESSION_MANAGER";
char SM_VENDOR_NAME[]		= "DtSession";
char SM_RELEASE_NAME[]		= "1.0";
char SM_CLIENT_ID[]		= "SM_CLIENT_ID";

/*
 * ToolTalk operation names
 */
char SM_DISPLAY_LOCK[]		= "Display_Lock";
char SM_XSESSION_EXIT[]		= "XSession_Exit";
char SM_RESOURCES_RELOAD[] 	= "Resources_Reload";
char SM_SAVE_SESSION[]		= "Save_Session";

