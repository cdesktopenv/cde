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
/* $XConsortium: HyperTextI.h /main/5 1995/10/26 12:28:01 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HyperText.h
 **
 **   Project:     TextGraphic Display routines
 **
 **   Description: Header file for HyperText.h
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
#ifndef _DtHyperTextI_h
#define _DtHyperTextI_h

/*****************************************************************************
 * Semi-private routines
 *****************************************************************************/
extern	int	 __DtLinkAddToList (
			char *link,
			int   type,
			char *description);
extern	void	 __DtLinkRemoveFromList ( int link_index );
extern	char	*__DtLinkStringReturn ( int link_index );
extern	int	 __DtLinkTypeReturn ( int link_index );
extern	void	_DtHelpExecProcedure (
			XtPointer  client_data,
			char *cmd );
extern	void	_DtHelpProcessHyperSelection (
			XtPointer        client_data,
			int              downX,
			int              downY,
			XEvent		*event );

#endif /* _DtHyperTextI_h */
