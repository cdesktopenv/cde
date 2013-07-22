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
/* $XConsortium: FontI.h /main/6 1995/12/08 13:00:51 cde-hal $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FontI.h
 **
 **   Project:     TextGraphic Display routines
 **
 **   Description: Header file for Font.c
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
#ifndef _DtFontI_h
#define _DtFontI_h

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 *			Semi Public Defines
 ****************************************************************************/
/*
 * resource database types
 */
#define _DtHelpXrmInt      0
#define _DtHelpXrmQuark    1

/*
 * font quarks
 */
#define _DT_HELP_FONT_CHAR_SET       6
#define _DT_HELP_FONT_LANG_TER       5
#define _DT_HELP_FONT_TYPE           4
#define _DT_HELP_FONT_WEIGHT         3
#define _DT_HELP_FONT_ANGLE          2
#define _DT_HELP_FONT_SIZE           1
#define _DT_HELP_FONT_SPACING        0

#define _DT_HELP_FONT_END            7

#define _DtHelpFontQuarkNumber     8

/****************************************************************************
 *			Semi Public Structures
 ****************************************************************************/
/****************************************************************************
 *			Semi Public Routines
 ****************************************************************************/
extern	void		 _DtHelpCopyDefaultList(XrmName *xrm_list);
extern	long		 __DtHelpDefaultFontIndexGet (
				DtHelpDispAreaStruct	*pDAS);
extern	void		 _DtHelpGetStringQuarks(XrmName *xrm_list);
extern	int		 __DtHelpFontCharSetQuarkGet(
				DtHelpDispAreaStruct	*pDAS,
				long			 font_index,
				XrmQuark		*ret_quark);
extern	void		 __DtHelpFontDatabaseInit (
				DtHelpDispAreaStruct	*pDAS,
				XtPointer		 default_font,
				XmFontType		 entry_type,
				XFontStruct		*user_font);
extern	int		 __DtHelpFontIndexGet (
				DtHelpDispAreaStruct	*pDAS,
				XrmQuarkList		 xrm_list,
				long			*ret_idx);
extern	int		 __DtHelpFontLangQuarkGet(
				DtHelpDispAreaStruct	*pDAS,
				long			 font_index,
				XrmQuark		*ret_quark);
extern	void		 __DtHelpFontMetrics (
				DtHelpDAFontInfo	 font_info,
				long			 font_index,
				_DtCvUnit		*ret_ascent,
				_DtCvUnit		*ret_descent,
				_DtCvUnit		*ret_char_width,
				_DtCvUnit		*ret_super,
				_DtCvUnit		*ret_sub);
extern	XFontSet	 __DtHelpFontSetGet (
				DtHelpDAFontInfo	 font_info,
				long			 font_index );
extern	XFontStruct	*__DtHelpFontStructGet (
				DtHelpDAFontInfo	 font_info,
				long			 font_index);
extern	int		 _DtHelpGetExactFontIndex(
				DtHelpDispAreaStruct	*pDAS,
				const char		*lang,
				const char		*char_set,
				char			*xlfd_spec,
				long			*ret_idx);

#ifdef __cplusplus
}
#endif
#endif /* _DtHelpFontI_h */
