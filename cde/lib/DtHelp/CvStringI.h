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
/* $XConsortium: CvStringI.h /main/3 1996/08/30 13:37:59 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        CvStringI.h
 **
 **   Project:     Cde DtHelp
 **
 **   Description: Header file for CvString.c
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
#ifndef _DtCvStringI_h
#define _DtCvStringI_h

/*****************************************************************************
 *           Macro defines
 *****************************************************************************/
#define _DtCvToLower(c) (('A' <= (c) && (c) <= 'Z') ? (c) + 'a' - 'A' : (c))

/*****************************************************************************
 *           Extern defines
 *****************************************************************************/
#if !defined(_AIX)
#define _DtCvStrCaseCmp(s1,s2)      strcasecmp(s1,s2)
#endif /* _AIX */

extern	void	 **_DtCvAddPtrToArray (
			void		**array,
			void		 *ptr);
extern	wchar_t	   _DtCvChar (
			const void	*p1,
			int		 type,
			int		 count);
extern	int	   _DtCvFreeArray (void **array);
extern	int	   _DtCvStrcspn(
			const void	*s1,
			const char	*s2,
			int		 type,
			int		*ret_len);
extern	int	  _DtCvStrLen (
			const void	*p1,
			int		 type);
extern	void	 *_DtCvStrPtr (
			const void	*p1,
			int		 type,
			int		 count);

#if     defined(_AIX)
extern  int     _DtCvStrCaseCmp(
			const char      *s1,
			const char      *s2);
#endif /* _AIX */
extern  int     _DtCvStrCaseCmpLatin1(
			const char      *s1,
			const char      *s2);
extern  int     _DtCvStrNCaseCmpLatin1(
			const char      *s1,
			const char      *s2,
			size_t           n);

#endif /* _DtCvStringI_h */
