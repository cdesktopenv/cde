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
/* $XConsortium: SmResource.h /main/5 1996/01/22 20:06:16 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmResource.h
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

#ifndef _smresource_h 
#define _smresource_h

/*
 * Default resource valuse
 */
#define DEFAULT_NUM_SESSIONS_BACKED_UP		2

/*
 * Global resource names
 */
extern char SmNsessionVersion[];

/*
 * Session Manager Resource Names
 */
extern char SmNwmStartup[];
extern char SmNquerySettings[];
extern char SmNkeys[];
extern char SmNalarmTime[];
extern char SmNmemThreshold[];
extern char SmNcontManagement[];
extern char SmNwaitClientTimeout[];
extern char SmNwaitWmTimeout[];
extern char SmNuseMessaging[];
extern char SmNcycleTimeout[];
extern char SmNlockTimeout[];
extern char SmNsaverTimeout[];
extern char SmNrandom[];
extern char SmNsaverList[];
extern char SmNsaveYourselfTimeout[];
extern char SmNmergeXdefaults[];
extern char SmNnumSessionsBackedup[];
extern char SmNignoreEnvironment[];
extern char SmNxineramaPreferredScreen[];

/* 
 * Resource names for settings information 
 */
extern char SmNkeyClick[];
extern char SmNbellPercent[];
extern char SmNbellPitch[];
extern char SmNbellDuration[];
extern char SmNledMask[];
extern char SmNglobalRepeats[];
extern char SmNautoRepeats[];
extern char SmNaccelNum[];
extern char SmNaccelDenom[];
extern char SmNthreshold[];
extern char SmNtimeout[];
extern char SmNinterval[];
extern char SmNpreferBlank[];
extern char SmNallowExp[];
extern char SmNfontPath[];
extern char SmNkeySymsPerKey[];
extern char SmNnumKeyCode[];
extern char SmNkeySyms[];
extern char SmNmaxKeyPerMod[];
extern char SmNmodMap[];
extern char SmNbuttonMap[];
extern char SmNhostState[];
extern char SmNhostNames[];
extern char SmNdidQuerySettings[];
extern char SmNshutDownState[];
extern char SmNshutDownMode[];
extern char SmNdisplayResolution[];
extern char SmNsessionLang[];
extern char SmNsaveFontPath[];

/*
 * Global resource classes
 */
extern char SmCsessionVersion[];

/*
 * Session Manager Resource Classes
 */
extern char SmCwmStartup[];
extern char SmCquerySettings[];
extern char SmCkeys[];
extern char SmCalarmTime[];
extern char SmCmemThreshold[];
extern char SmCContManagement[];
extern char SmRContManagement[];
extern char SmCWaitClientTimeout[];
extern char SmCWaitWmTimeout[];
extern char SmCUseMessaging[];
extern char SmCcycleTimeout[];
extern char SmClockTimeout[];
extern char SmCsaverTimeout[];
extern char SmCrandom[];
extern char SmCsaverList[];
extern char SmCsaveYourselfTimeout[];
extern char SmCmergeXdefaults[];
extern char SmCnumSessionsBackedup[];
extern char SmCignoreEnvironment[];
extern char SmCxineramaPreferredScreen[];


/*
 * Class names for session settings information
 */
extern char SmCkeyClick[];
extern char SmCbellPercent[];
extern char SmCbellPitch[];
extern char SmCbellDuration[];
extern char SmCledMask[];
extern char SmCglobalRepeats[];
extern char SmCautoRepeats[];
extern char SmCaccelNum[];
extern char SmCaccelDenom[];
extern char SmCthreshold[];
extern char SmCtimeout[];
extern char SmCinterval[];
extern char SmCpreferBlank[];
extern char SmCallowExp[];
extern char SmCfontPath[];
extern char SmCkeySymsPerKey[];
extern char SmCnumKeyCode[];
extern char SmCkeySyms[];
extern char SmCmaxKeyPerMod[];
extern char SmCmodMap[];
extern char SmCbuttonMap[];
extern char SmChostState[];
extern char SmChostNames[];
extern char SmCdidQuerySettings[];
extern char SmCshutDownState[];
extern char SmCshutDownMode[];
extern char SmCdisplayResolution[];
extern char SmCsessionLang[];
extern char SmCsaveFontPath[];

#endif /* _smresource_h */
