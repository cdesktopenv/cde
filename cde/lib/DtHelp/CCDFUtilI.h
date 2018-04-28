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
/* $XConsortium: CCDFUtilI.h /main/5 1995/10/26 12:17:26 rswiston $ */
/*************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        CCDFUtilI.h
 **
 **   Project:     Un*x Desktop Help
 **
 **   Description: Header file for CCDFUtil.c
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
#ifndef _CECCDFUtilI_h
#define _CECCDFUtilI_h

/*****************************************************************************
 *  Parsing flags and macros
 *****************************************************************************/
#define CCDF_ABBREV_CMD          (1 <<  0)
#define CCDF_FIGURE_CMD          (1 <<  1)
#define CCDF_FONT_CMD            (1 <<  2)
#define CCDF_FORMAT_END          (1 <<  3)
#define CCDF_GRAPHIC_CMD         (1 <<  4)
#define CCDF_ID_CMD              (1 <<  5)
#define CCDF_LABEL_CMD           (1 <<  6)
#define CCDF_LINK_CMD            (1 <<  7)
#define CCDF_NEWLINE_CMD         (1 <<  8)
#define CCDF_OCTAL_CMD           (1 <<  9)
#define CCDF_PARAGRAPH_CMD       (1 << 10)
#define CCDF_TITLE_CMD           (1 << 11)
#define CCDF_TOPIC_CMD           (1 << 12)

#define CCDF_NO_CMDS                 0
#define CCDF_ALL_CMDS              ~(0)
#define CCDF_ALLOW_CMD(x, y)             ((x) & (y))
#define CCDF_NOT_ALLOW_CMD(x, y)         ((~(x)) & (y))

/****************************************************************************
 *			Semi Public Routines
 ****************************************************************************/
extern	int	_DtHelpCeGetCcdfAbbrevCmd(
			BufFilePtr	 in_file,
			char		*in_buf,
			int		 in_size,
			int		 cur_mb_len,
			char		**in_ptr,
			char		**ret_string);
extern	int	_DtHelpCeSkipCcdfAbbrev(
			BufFilePtr	 in_file,
			char		*in_buf,
			char		**in_ptr,
			int		 in_size,
			int		 cur_mb_len);
extern	int	_DtHelpCeCheckNextCcdfCmd(
			char		*token,
			BufFilePtr	 in_file,
			char		*in_buf,
			int		 in_size,
			int		 cur_mb_len,
			char		**in_ptr);
extern	int	_DtHelpCeGetCcdfEndMark(
			BufFilePtr	  file,
			char		 *buffer,
			char		**buf_ptr,
			int		  buf_size,
			int		  cur_mb_len);
extern	int	_DtHelpCeGetCcdfFontType(
			char		*code);
extern	int	_DtHelpCeGetCcdfValueParam(
			BufFilePtr	 in_file,
			char		*in_buf,
			int		 in_size,
			char		**in_ptr,
			_DtCvValue	 flag,
			int		 cur_mb_len,
			int		*ret_value);
extern	int	_DtHelpCeGetCcdfStrParam(
			BufFilePtr	 in_file,
			char		*in_buf,
			int		 in_size,
			int		 cur_mb_len,
			char		**in_ptr,
			_DtCvValue	 flag,
			_DtCvValue	 eat_escape,
			_DtCvValue	 ignore_quotes,
			_DtCvValue	 less_test,
			char		**ret_string);
extern	int	_DtHelpCeGetCcdfTopicAbbrev(
			void		*dpy,
			BufFilePtr	 in_file,
			char		*in_buf,
			char		**in_ptr,
			int		 in_size,
			int		 cur_mb_len,
			char		**ret_title,
			char		**ret_charSet,
			char		**ret_abbrev);
extern	int	_DtHelpCeGetCcdfTopicCmd(
			void		*dpy,
			BufFilePtr	 in_file,
			char		*in_buf,
			char		**in_ptr,
			int		 in_size,
			int		 cur_mb_len,
			char		**ret_charSet);
extern	int	_DtHelpCeGetCcdfCmd(
			int		 current,
			char		*in_buf,
			char		**in_ptr,
			BufFilePtr	 in_file,
			int		 size,
			int		 allowed);
extern	int	_DtHelpCeSkipToNextCcdfToken(
			BufFilePtr	 in_file,
			char		*in_buf,
			int		 in_size,
			int		 cur_mb_len,
			char		**in_ptr,
			_DtCvValue	 flag );

#endif /* _CECCDFUtilI_h */
