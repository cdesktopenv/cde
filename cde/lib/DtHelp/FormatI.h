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
/* $XConsortium: FormatI.h /main/5 1995/10/26 12:20:54 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FormatI.h
 **
 **   Project:     TextGraphic Display routines
 **
 **   Description: Header file for Format.c
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
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpFormatI_h
#define _DtHelpFormatI_h

extern	int		 _DtHelpFormatAsciiFile (
				XtPointer	  client_data,
				char		 *filename,
				XtPointer	 *ret_topic);
extern	int		 _DtHelpFormatAsciiString (
				XtPointer	  client_data,
				char		*input_string,
				XtPointer	 *ret_topic);
extern	int		 _DtHelpFormatAsciiStringDynamic (
				XtPointer	  client_data,
				char		*input_string,
				XtPointer	 *ret_topic);
extern	int		 _DtFormatGetParagraphList(
				XtPointer	 *ret_topic);
extern	int		 _DtHelpFormatIndexEntries (
				XtPointer	  client_data,
				_DtHelpVolumeHdl     volume,
				int		 *ret_cnt,
				XmString	**ret_words,
				XmFontList	 *ret_list,
				Boolean		 *ret_mod);
extern	void		 _DtFormatInitializeFontQuarks(
				char	**font_attr );
extern	int		 _DtFormatInitializeVariables(
				char             *input_string,
				int               grow_size );
extern	int		 _DtFormatProcessString (
				char	**input_string,
				int       seg_type,
				char	**font_attr );
extern	int		 _DtHelpFormatToc (
				XtPointer	  client_data,
				_DtHelpVolumeHdl     volume,
				char		*id_string,
				char		*ret_id,
				XtPointer	*ret_handle);
extern	int		 _DtHelpFormatTopic (
				XtPointer	  client_data,
				_DtHelpVolumeHdl     volume,
				char		*id_string,
				Boolean		 look_at_id,
				XtPointer	*ret_handle);
extern	int		 _DtHelpFormatTopicTitle (
				XtPointer	  client_data,
				_DtHelpVolumeHdl     volume,
				char		 *location_id,
				XmString	 *ret_title,
				XmFontList	 *ret_list,
				Boolean		 *ret_mod);
extern	int		 _DtHelpFormatVolumeTitle (
				XtPointer	  client_data,
				_DtHelpVolumeHdl     volume,
				XmString	 *ret_title,
				XmFontList	 *ret_list,
				Boolean		 *ret_mod);
extern int		_DtHelpGetAsciiVolumeTitle (
				XtPointer            client_data,
				_DtHelpVolumeHdl      volume,
				char                **ret_title);

#endif /* _DtHelpFormatI_h */
