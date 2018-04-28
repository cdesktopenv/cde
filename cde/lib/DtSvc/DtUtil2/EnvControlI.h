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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/****************************<+>*************************************
 **
 **   File:     EnvControlI.h
 **
 **   RCS:	$TOG: EnvControlI.h /main/6 1998/07/30 12:14:02 mgreess $
 **
 **   Project:  DT Runtime Library  -- Private header
 **
 **   Description: Defines structures, and parameters used
 **                for communication with the environment
 **
 **   (c) Copyright 1992-94 by Hewlett-Packard Company
 **
 ****************************<+>*************************************/
/*******************************************************************
	The environment variables
*******************************************************************/
#define BIN_PATH_ENVIRON "PATH"
#define NLS_PATH_ENVIRON "NLSPATH"
#define SYSTEM_APPL_PATH_ENVIRON "XFILESEARCHPATH"
#define PM_PATH_ENVIRON	"XMICONSEARCHPATH"
#define BM_PATH_ENVIRON	"XMICONBMSEARCHPATH"

/*******************************************************************
	The default DT path strings, architecture-dependent
*******************************************************************/
#define BIN_PATH_STRING	 CDE_INSTALLATION_TOP "/bin"

#define NLS_PATH_STRING  CDE_INSTALLATION_TOP "/lib/nls/msg/%L/%N.cat:" \
                         CDE_INSTALLATION_TOP "/lib/nls/msg/C/%N.cat"

#if defined(sun)
#define X_BIN_PATH_STRING "/usr/openwin/bin"
#else
#define X_BIN_PATH_STRING "/usr/bin/X11"
#endif

/*
 *  Some notes on the behavior and use of the XFILESEARCHPATH component, as
 *  defined through the SYSTEM_APPL_PATH_STRING definition below:
 *
 *      Its precedence in the lookup of X resources is fairly low--it can
 *	be overridden by XRM resources, by resources specified in
 *      the user's $HOME/.Xdefaults-<hostname> file, and by resources
 *      found using the $XUSERFILESEARCHPATH setting.
 *
 *      The order of pathnames in the XFILESEARCHPATH is such that the first
 *      match satisfies the lookup, and the lookup stops there.
 *
 *      We place the pathname components in our XFILESEARCHPATH such that
 *      the lookup goes, from first match attempt to last match attempt, as
 *      follows:
 *
 *      - Custom resources: /etc/../$LANG
 *      - Custom resources: /etc/../C
 *      - Factory defaults: /opt/../$LANG --shipped with every localized system
 *      - Factory defaults: /opt/../C     --shipped with every system
 *
 *      These resources are used ONLY for the DT components themselves
 *      (not, for example, for MIT client resources).
 *
 *      The CDE vendors retain the right to alter, remove, append to, and
 *      ignore any settings in the factory defaults locations.  The vendors
 *      will not modify the settings in the "custom resources" locations.
 *
 */
#define SYSTEM_APPL_PATH_STRING  CDE_CONFIGURATION_TOP "/app-defaults/%L/%N:" \
                                 CDE_CONFIGURATION_TOP "/app-defaults/C/%N:" \
                                 CDE_INSTALLATION_TOP "/app-defaults/%L/%N:" \
                                 CDE_INSTALLATION_TOP "/app-defaults/C/%N"


/**********************************************************************
 * Data representation of the user's DT environment
 **********************************************************************/

typedef struct environStruct {
	char * pmPath;
	char * binPath;
	char * nlsPath;
	char * sysApplPath;
	char * bmPath;
} _environStruct;

/**********************************************************************
 * Miscellaneous
 **********************************************************************/
#define BV_BINPATH                      (1<<0)
#define BV_SYSAPPLPATH                  (1<<1)
#define BV_NLSPATH                      (1<<2)
#define BV_PMPATH                       (1<<3)
#define BV_BMPATH                       (1<<4)

#define MAX_ENV_STRING			(2*BUFSIZ)

/****************************        eof       **********************/
