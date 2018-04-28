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
 * File:         DtPStrings.h $XConsortium: DtPStrings.h /main/5 1995/12/14 11:02:36 barstow $
 * Language:     C
 */
#ifndef _DtPStrings_h
#define _DtPStrings_h

/* 
 * DT applications should use this name to look up DT-global 
 * resources instead of argv[0].
 */
#define DtDT_PROG_NAME 		"dt"	

/* 
 * DT applications should use this class name to look up DT-global
 * resources.
 */
#define DtDT_PROG_CLASS		"Dt"

/* 
 * The following string constants define the standard DT configuration
 * directories. 
 */
#define DtPERSONAL_CONFIG_DIRECTORY		".dt"
#define DtSM_SESSION_DIRECTORY			"sessions"
#define DtSM_SESSION_DISPLAY_DIRECTORY		"display"
#define DtCURRENT_DT_VERSION			"3.0"

/* 
 * If you change the following two #defines, you must also change the
 * related one below (DtDB_DIRS_DEFAULT). 
 */
#define DtPERSONAL_DB_DIRECTORY		".dt/types"
#define DtPERSONAL_TMP_DIRECTORY	".dt/tmp"

/*
 * Names for the message log files
 */
#define DtERRORLOG_FILE			"errorlog"
#define DtOLD_ERRORLOG_FILE		"errorlog.old"
#define DtOLDER_ERRORLOG_FILE   	"errorlog.older"

/* 
 * Strings for default types and icons
 */
#define DtDEFAULT_DATA_FT_NAME				"DATA"

/*
 * The following string constants define the resource name,
 * resource class and default values for the action bitmaps.
 */
#define DtACTION_ICON_RESOURCE_NAME			"actionIcon"
#define DtACTION_ICON_RESOURCE_CLASS			"ActionIcon"
#define DtACTION_ICON_DEFAULT				"Dtactn"

/*
 * The following string constants define the resource name,
 * resource class and default values for the Dt tmp directory path.
 */
#define DtACTION_DTTMPDIR_RESOURCE_NAME			"dtTmpDir"
#define DtACTION_DTTMPDIR_RESOURCE_CLASS		"DtTmpDir"
#define DtACTION_DTTMPDIR_DEFAULT			".dt/tmp"

/* Do not add anything after this endif. */
#endif /* _DtPStrings_h */
