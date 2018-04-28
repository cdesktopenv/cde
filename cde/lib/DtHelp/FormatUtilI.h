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
/* $XConsortium: FormatUtilI.h /main/9 1996/05/09 03:42:14 drk $ */
/*************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FormatUtilI.h
 **
 **   Project:     TextGraphic Display routines
 **
 **  
 **   Description: Header file for FormatUtil.c
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
#ifndef _DtHelpFormatUtilI_h
#define _DtHelpFormatUtilI_h

/********    Semi-Private Structure Declarations    ********/
typedef	struct	sdlMatchData {
	int	clan;
	int	level;
	char	*ssi;
} SdlMatchData;

typedef struct  _frmtUiInfo {
	_DtCvValue	(*load_graphic)();
	_DtCvValue	(*resolve_spc)();
	void            (*load_font)();
	int	        (*exec_filter)();
	void	        (*destroy_region)();
	_DtCvPointer    client_data;
	_DtCvUnit	line_width;
	_DtCvUnit	line_height;
	_DtCvUnit	leading;
	int             avg_char;
	short           nl_to_space;
} _FrmtUiInfo;

typedef	struct	_frmtPrivateInfo {
	char		 top_block;
	char		 dup_flag;
	int		 sdl_el_type;
	int		 interp;
	void		*match_info;
	void		*id_info;
	void		*doc_info;
	void		*entry;
	void		*toss;
	void		*lang_char;
	char		*abbrev;
} FrmtPrivateInfo;

/********    Semi-Private Function Declarations    ********/
extern	int	_DtHelpCeAddCharToBuf (
			char	**src,
			char	**dst,
			int	 *dst_size,
			int	 *dst_max,
			int	  grow_size );
extern	int	_DtHelpCeAddOctalToBuf(
			char     *src,
			char    **dst,
			int      *dst_size,
			int      *dst_max,
			int       grow_size );
extern	int	_DtHelpCeAddStrToBuf (
			char	**src,
			char	**dst,
			int	 *dst_size,
			int	 *dst_max,
			int	  copy_size,
			int	  grow_size );
extern _DtCvSegment *_DtHelpAllocateSegments (
			int          malloc_size);
extern	int	_DtHelpCeGetMbLen(
			char	*lang,
			char	*char_set);
extern	int	_DtHelpCeGetNxtBuf(
			BufFilePtr	  file,
			char		 *dst,
			char		**src,
			int		  max_size);
extern	char *	_DtHelpGetNxtToken (
			char		 *str,
			char		**retToken);
extern	int	_DtHelpCeReadBuf(
			BufFilePtr	 file,
			char		*buffer,
			int		 size);
extern	int	_DtHelpFmtFindBreak(
			char		*ptr,
			int		 mb_len,
			int		*num_chars);
extern	void	_DtHelpLoadMultiInfo(
			wchar_t		**cant_begin_chars,
			wchar_t		**cant_end_chars,
			short		 *nl_to_space);

/********    Semi-Private Macro Declarations    ********/
#define	FrmtPrivInfoPtr(x)	((FrmtPrivateInfo *) (x)->client_use)

#endif /* _DtHelpFormatUtilI_h */
