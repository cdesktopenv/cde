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
/* $XConsortium: UtilSDLI.h /main/8 1996/01/29 12:21:37 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        UtilSDLI.h
 **
 **  Project:     Cde Help System
 **
 **  Description: Internal header file for UtilSDL.c
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _DtHelpUtilSDLI_h
#define _DtHelpUtilSDLI_h


#ifdef __cplusplus
extern "C" {
#endif

/********     Private Define Declarations    ********/
#define	DTD_NAMELEN	64
/******** End Private Define Declarations    ********/

enum    _sdlFontMode
  {
    _SdlFontModeResolve,
    _SdlFontModeSave,
    _SdlFontModeNone
  };

typedef enum _sdlFontMode  _SdlFontMode;

/********    Private Macro Declarations    ********/

#define	_SdlContainerPtrOfSeg(x)	(&((x)->handle.container))

/********    Private Function Declarations    ********/

extern	int	_DtHelpCeAllocateFontStruct (
			int		  num,
			_DtHelpFontHints **ret_ptr);
extern	_DtCvString *_DtHelpCeAllocStrStruct(void);
extern	void	_DtHelpCeAddSegToList(
			_DtCvSegment	   *seg_ptr,
			_DtCvSegment	  **seg_list,
			_DtCvSegment	  **last_seg);
extern	int	_DtHelpCeAllocSegment (
			int		    malloc_size,
			int		   *alloc_size,
			_DtCvSegment	  **next_seg,
			_DtCvSegment	  **ret_seg);
extern	int	_DtHelpCeFindSkipSdlElementEnd(
			BufFilePtr	  f);
extern	int	_DtHelpCeGetSdlAttribute (
			BufFilePtr	 f,
			int		 max_len,
			char		*attribute_name);
extern	int	_DtHelpCeGetSdlAttributeCdata (
			BufFilePtr	  f,
			_DtCvValue	  limit,
			char		**string);
extern	int	_DtHelpCeGetSdlCdata (
			BufFilePtr	  f,
			SdlOption	  type,
			int		  char_len,
			_DtCvValue	  nl_to_space,
			_DtCvValue	 *cur_space,
			_DtCvValue	 *cur_nl,
			_DtCvValue	 *cur_mb,
			char		 *non_break_char,
			char		**string,
			int		 *str_max);
extern	int	_DtHelpCeGetSdlId (
			BufFilePtr	  f,
			char		**id_value);
extern	int	_DtHelpCeGetSdlNumber (
			BufFilePtr	 f,
			char		*number_string);
extern	int	_DtHelpCeMatchSdlElement (
			BufFilePtr	 f,
			const char	*element_str,
			int		 sig_chars);
extern	_DtCvSegment *_DtHelpCeMatchSemanticStyle (
			_DtCvSegment	*toss,
			SdlOption	 clan,
			int		 level,
			char		*ssi);
extern	void	_DtHelpCeMergeSdlAttribInfo(
			_DtCvSegment           *src_toss,
			_DtCvSegment	    *dst_contain,
			_DtHelpFontHints    *dst_fonts,
			void		    *el_info,
			unsigned long	    *str1_values,
			unsigned long       *str2_values);
extern	int	_DtHelpCeReturnSdlElement (
			BufFilePtr	  f,
			const SDLElementInfo *el_list,
			_DtCvValue	  cdata_flag,
			enum SdlElement	*ret_el_define,
			char		**ret_data,
			_DtCvValue	 *ret_end_flag);
extern	int	_DtHelpCeSaveString (
			_DtCvPointer	    client_data,
			_DtCvSegment	  **seg_list,
			_DtCvSegment	  **last_seg,
			_DtCvSegment	  **prev_seg,
			char		   *string,
			_DtHelpFontHints   *font_specs,
			int		    link_index,
			int		    multi_len,
			int		    flags,
			void		   (*load_font)(),
			_SdlFontMode	    resolve_font,
			_DtCvValue	    newline);
extern	int	_DtHelpCeSkipCdata (
			BufFilePtr	f,
			_DtCvValue	flag);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtHelpUtilSDLI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
