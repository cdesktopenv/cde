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
/* $XConsortium: AccessI.h /main/7 1995/12/18 16:29:56 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessI.h
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
#ifndef _DtHelpAccessI_h
#define _DtHelpAccessI_h


#ifndef	_XtIntrinsic_h
/*
 * typedef Boolean
 */
#ifdef CRAY
typedef long            Boolean;
#else
typedef char            Boolean;
#endif
#endif

#ifndef	_XLIB_H_
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
#endif

/******************************************************************************
 *                   Semi-Public Structures
 ******************************************************************************/
typedef struct {
	char  match;
	char *substitution;
} _DtSubstitutionRec;

typedef	struct _dtHelpCeLockInfo {
	int		fd;
	_DtHelpVolumeHdl	volume;
} _DtHelpCeLockInfo;

/******************************************************************************
 *                   Semi-Public Access Functions
 ******************************************************************************/
extern	char	*_DtHelpCeExpandPathname (
			char		   *spec,
			char		   *filename,
			char		   *type,
			char		   *suffix,
			char		   *lang,
			_DtSubstitutionRec *subs,
			int		    num );
extern	int	 _DtHelpCeFileOpenAndSeek(
			char		*filename,
			int		 offset,
			int		 fd,
			BufFilePtr	*ret_file,
			time_t		*ret_time);
extern	int	 _DtHelpCeGetDocStamp (
			_DtHelpVolumeHdl  volume,
			char		**ret_doc,
			char		**ret_time);
extern	int	 _DtHelpCeGetLangSubParts (
			char		 *lang,
			char		**subLang,
			char		**subTer,
			char		**subCodeSet );
extern	int	 _DtHelpCeGetTopicChildren(
			_DtHelpVolumeHdl  volume,
			char		 *topic_id,
			char		***ret_childs);
extern	int	 _DtHelpGetTopicTitle(
			_DtHelpVolumeHdl  volume,
			char		 *target_id,
			char		**ret_title);
extern	int	 _DtHelpCeGetUncompressedFileName (
			char		  *name,
			char		 **ret_name);
extern	const char *_DtHelpCeGetVolumeCharSet (
			_DtHelpVolumeHdl  volume);
extern	int	 _DtHelpCeGetVolumeFlag (
			_DtHelpVolumeHdl  volume);
extern	int	 _DtHelpCeIsTopTopic(
			_DtHelpVolumeHdl   volume,
			const char	  *id);
extern	int	 _DtHelpCeLockVolume(
			_DtHelpVolumeHdl   volume,
			_DtHelpCeLockInfo *ret_info);
extern	int	 _DtHelpCeMapTargetToId (
			_DtHelpVolumeHdl  volume,
			char		 *target_id,
			char		**ret_id);
extern	int	 _DtHelpCeUnlockVolume(_DtHelpCeLockInfo lock_info);
#endif /* _DtHelpAccessI_h */
