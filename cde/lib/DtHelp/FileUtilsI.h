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
/* $XConsortium: FileUtilsI.h /main/5 1995/10/26 12:19:41 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FileUtilsI.h
 **
 **   Project:     DtHelp Project
 **
 **   Description: File locating and handling utilities
 ** 
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpFileUtilsI_h
#define _DtHelpFileUtilsI_h

#define DtHelpVOLUME_TYPE "volumes"      /* dir for %T in XtResolvePathname */
#define DtHelpCCDF_VOL_SUFFIX ".hv"      /* possible basename suffix */
#define DtHelpSDL_VOL_SUFFIX ".sdl"      /* possible basename suffix */

/* array indices and size for use with _DtHelpFileGetSearchPaths() */
#define _DtHELP_FILE_USER_PATH 0
#define _DtHELP_FILE_SYS_PATH  1
#define _DtHELP_FILE_HOME_PATH 2
#define _DtHELP_FILE_NUM_PATHS 3

typedef struct _dtHelpCeDirStruct {
	char    *dir_name;
	int      type;
	int      user_flag;
	struct _dtHelpCeDirStruct *next_dir;
} _DtHelpCeDirStruct;

/* list of suffixes that help volumes may have */
extern const char * _DtHelpFileSuffixList[];

extern	int			 _DtHelpCeCheckAndCacheDir(char *dir);
extern	void			 _DtHelpFileGetSearchPaths(
				         char *  paths[],
				         Boolean searchHomeDir);
extern	char			 *_DtHelpFileLocate (
				        char *       type,
				        char *       base,
				        const char * suffixList[],
				        Boolean      searchCurDir,
				        int          accessMode);
extern	Boolean			 _DtHelpFileTraceLinks (
					char * *  pPathName);
extern	Boolean			 _DtHelpFileTraceToFile (
				        char * *  pPathName,
				        int       accessMode,
				        char * *  pFoundPath);

#endif /* _DtHelpFileUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

