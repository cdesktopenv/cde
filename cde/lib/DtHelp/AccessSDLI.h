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
/* $XConsortium: AccessSDLI.h /main/6 1995/12/18 16:30:06 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessSDLI.h
 **
 **   Project:     Run Time Project File Access
 **
 **   Description: Header file for Access.h
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
#ifndef _DtHelpAccessSDLI_h
#define _DtHelpAccessSDLI_h

/****************************************************************************
 *			Semi-Private Routines
 ****************************************************************************/
extern	void		 _DtHelpCeCleanSdlVolume(
				_DtHelpVolumeHdl	volume );
extern	void		 _DtHelpCeCloseSdlVolume (
				_DtHelpVolumeHdl	volume);
extern	int		 _DtHelpCeFindSdlId (
				_DtHelpVolumeHdl  vol,
				char		 *target_id,
				int		  fd,
				char		**ret_name,
				int		 *ret_offset );
extern	int		 _DtHelpCeGetSdlDocStamp (
				_DtHelpVolumeHdl  volume,
				char		**ret_doc,
				char		**ret_time);
extern	char		*_DtHelpCeGetSdlHomeTopicId (_DtHelpVolumeHdl vol);
extern	int		 _DtHelpCeGetSdlIdPath (
				_DtHelpVolumeHdl   volume,
				char		  *target_id,
				char		***ret_ids);
extern	_DtCvSegment	*_DtHelpCeMapSdlIdToElement(
				_DtHelpVolumeHdl  vol,
				char		 *target_id,
				int		  fd);
extern	int		 _DtHelpCeGetSdlKeywordList (
				_DtHelpVolumeHdl	volume);
extern	int		 _DtHelpCeGetSdlTopicChildren(
				_DtHelpVolumeHdl  volume,
				char		 *target_id,
				char		***ret_ids);
extern	const char	*_DtHelpCeGetSdlVolCharSet (
				_DtHelpVolumeHdl	volume);
extern	char		*_DtHelpCeGetSdlVolLanguage (
				_DtHelpVolumeHdl	volume);
extern	char		*_DtHelpCeGetSdlVolumeAsciiAbstract (
				_DtHelpVolumeHdl	volume);
extern	char		*_DtHelpCeGetSdlVolumeLocale (
				_DtHelpVolumeHdl	volume);
extern	CESDLVolume	*_DtHelpCeGetSdlVolumePtr(
				_DtHelpVolumeHdl	volume );
extern	_DtCvSegment	*_DtHelpCeMapSdlIdToSegment(
				_DtHelpVolumeHdl  vol,
				const char	 *target_id,
				int		  fd);
extern	void		 _DtHelpCeInitSdlVolume(
				_DtHelpVolumeHdl	volume );
extern	int		 _DtHelpCeMapIdToSdlTopicId(
				_DtHelpVolumeHdl  vol,
				const char	 *target_id,
				char		**ret_id);
extern	int		 _DtHelpCeOpenSdlVolume(
				_DtHelpVolumeHdl	volume );
extern	int		 _DtHelpCeRereadSdlVolume(
				_DtHelpVolumeHdl	volume );
#endif /* _DtHelpAccessSDLI_h */
