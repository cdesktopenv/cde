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
/* $XConsortium: AccessCCDFI.h /main/6 1995/12/18 16:29:46 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessCCDFI.h
 **
 **   Project:     Run Time Project File Access
 **
 **   Description: Header file for AccessCCDF.h
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
#ifndef _DtHelpAccessCcdfI_h
#define _DtHelpAccessCcdfI_h

/******************************************************************************
 *                   Resource Database Access Routines
 ******************************************************************************/
extern	char		 *_DtHelpCeGetResourceString(
					XrmDatabase	 db,
					char		*topic,
					char		*res_class,
					char		*res_name);
extern	char		**_DtHelpCeGetResourceStringArray(
					XrmDatabase	 db,
					char		*topic,
					char		*res_class,
					char		*res_name);

/******************************************************************************
 *                   CCDF Access Functions
 ******************************************************************************/
extern  void		 _DtHelpCeCloseCcdfVolume(
				_DtHelpVolume      vol);
extern  int		 _DtHelpCeFindCcdfId(
				_DtHelpVolume      vol,
				char              *target_id,
				char		 **ret_name,
				int		  *ret_offset);
extern  int		 _DtHelpCeGetCcdfDocStamp(
				_DtHelpVolume      vol,
				char		 **ret_doc,
				char		 **ret_time);
extern  int		 _DtHelpCeFindCcdfKeyword(
				_DtHelpVolume      vol,
				char              *target_id,
				char            ***ret_ids );
extern  int		 _DtHelpCeGetCcdfIdPath(
				_DtHelpVolume      vol,
				char              *target_id,
				char            ***ret_ids );
extern	int		 _DtHelpCeGetCcdfKeywordList (
				_DtHelpVolume      vol);
extern	int		 _DtHelpCeGetCcdfTopicChildren (
				_DtHelpVolume      vol,
				char		  *topic_id,
				char		***retTopics);
extern	int		 _DtHelpCeGetCcdfTopicParent (
				_DtHelpVolume      vol,
				char		  *topic_id,
				char		 **retTopics);
extern	int		 _DtHelpCeGetCcdfTopTopic (
				_DtHelpVolume      vol,
				char		 **ret_topic);
extern	int		 _DtHelpCeGetCcdfVolIdList (
				_DtHelpVolume      vol,
				char		***retTopics);
extern	char		*_DtHelpCeGetCcdfVolLocale (
				_DtHelpVolume      vol);
extern	char		*_DtHelpCeGetCcdfVolTitle(
				_DtHelpVolume      vol);
extern	int		 _DtHelpCeGetCcdfVolumeAbstract (
				_DtHelpVolume      vol,
				char		 **ret_abs);
extern	int		 _DtHelpCeGetCcdfVolumeTitle(
				_DtHelpVolume      vol,
				char		 **ret_title);
extern	int		 _DtHelpCeMapCcdfTargetToId (
				_DtHelpVolume      vol,
				const char        *target_id,
				char             **ret_id);
extern	int		 _DtHelpCeOpenCcdfVolume (
				_DtHelpVolume      vol);
extern	int		 _DtHelpCeRereadCcdfVolume (
				_DtHelpVolume      vol);
#endif /* _DtHelpAccessCcdfI_h */
