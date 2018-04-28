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
/* $XConsortium: Access.h /main/7 1995/12/18 16:29:36 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Access.h
 **
 **   Project:     Run Time Project File Access
 **
 **   Description: Header file for Access.h
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpAccess_h
#define _DtHelpAccess_h

/****************************************************************************
 *			Public Defines
 ****************************************************************************/
#ifndef	True
#define	True	1
#endif
#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	False
#define	False	0
#endif
#ifndef	FALSE
#define	FALSE	0
#endif

typedef	void*	_DtHelpVolumeHdl;

/****************************************************************************
 *			Semi-Private Externals
 ****************************************************************************/
extern	int	 _DtHelpCeCompressPathname ( char *basePath );
extern	char	*_DtHelpCeTraceFilenamePath ( char *file_path );
extern	char	*_DtHelpCeTracePathName ( char *path );

/****************************************************************************
 *			Semi-Public Externals
 ****************************************************************************/
extern	int	 _DtHelpCloseVolume (
				_DtHelpVolumeHdl vol );
extern	int	 _DtHelpCeFindId (
				_DtHelpVolumeHdl  vol,
				char		 *target_id,
				int		  fd,
				char		**ret_name,
				int		 *ret_offset );
extern	int	 _DtHelpCeFindKeyword (
				_DtHelpVolumeHdl   vol,
				char		  *target,
				char		***ret_ids );
extern	int	 _DtHelpCeGetKeywordList (
				_DtHelpVolumeHdl   vol,
				char		***ret_keywords );
extern	int	 _DtHelpCeGetTopTopicId (
				_DtHelpVolumeHdl  vol,
				char		**ret_idString );
extern	char	*_DtHelpCeGetVolumeName(
				_DtHelpVolumeHdl   vol);
extern  char    *_DtHelpGetVolumeLocale (
				_DtHelpVolumeHdl  volume);
extern	int	 _DtHelpOpenVolume (
				char		*volFile,
				_DtHelpVolumeHdl*retVol );
extern	int	 _DtHelpCeUpVolumeOpenCnt (
				_DtHelpVolumeHdl   volume);
#endif /* _DtHelpAccess_h */
