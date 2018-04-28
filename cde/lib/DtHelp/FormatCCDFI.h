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
/* $XConsortium: FormatCCDFI.h /main/5 1995/10/26 12:20:47 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FormatCCDFI.h
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
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpFormatCCDFI_h
#define _DtHelpFormatCCDFI_h

typedef	void*	VarHandle;

extern	int		_DtHelpCeGetCcdfTitleChunks(
				_DtHelpVolumeHdl	volume,
				char		*loc_id,
				_FrmtUiInfo		*ui_info,
				void		***ret_chunks);
extern	int		_DtHelpCeFrmtCcdfPathAndChildren(
				_DtHelpVolumeHdl         volume,
				char                 *loc_id,
				_FrmtUiInfo		*ui_info,
				_DtCvTopicPtr          *ret_handle );
extern	int		_DtHelpCeGetCcdfVolTitleChunks(
				_DtHelpVolumeHdl	 volume,
				_FrmtUiInfo		*ui_info,
				void		***ret_chunks);
extern	int		_DtHelpCeFrmtCcdfTopic (
				_DtHelpVolumeHdl volume,
				char          *filename,
				int            offset,
				char          *id_string,
				_FrmtUiInfo	*ui_info,
				_DtCvTopicPtr	*ret_topic);
extern	int		__DtHelpCeGetParagraphList (
				VarHandle	 var_handle,
				int		 make_cont,
				_DtCvFrmtOption	 type,
				_DtCvTopicPtr	*ret_handle);
extern	int		__DtHelpCeProcessString(
				VarHandle	  var_handle,
				BufFilePtr	  my_file,
				_DtCvFrmtOption	  frmt_type,
				char		 *scan_string,
				char		 *in_string,
				int		  in_size,
				int		  fnt_flags,
				int		  ret_on_nl,
				_DtHelpFontHints *font_attr);
extern	VarHandle	__DtHelpCeSetUpVars (
				char		*lang,
				char		*code_set,
				_FrmtUiInfo	*ui_info);

#endif /* _DtHelpFormatCCDFI_h */
