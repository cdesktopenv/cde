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
/* $XConsortium: ObsoleteP.h /main/3 1996/05/09 03:44:18 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ObsoleteP.h
 **
 **   Project:     Obsolete private routines.
 **
 **   Description: Header file for Obsolete.h
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
#ifndef _DtHelpObsoleteP_h
#define _DtHelpObsoleteP_h

/****************************************************************************
 *			Public Defines
 ****************************************************************************/
#ifndef	True
#define	True	1
#endif
#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	False
#define	False	0
#endif
#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef _DtHelpGenUtilsP_h
typedef void*   CanvasHandle;
typedef void*   VolumeHandle;
#endif

/****************************************************************************
 *			Semi-Public Externals
 ****************************************************************************/
extern	int	 _DtHelpCeCloseVolume (
				CanvasHandle	 canvas_handle,
				VolumeHandle	 retVol );
extern	void	 _DtHelpCeDestroyCanvas (CanvasHandle canvas);
extern	int	 _DtHelpCeGetTopicTitle (
				CanvasHandle          canvas,
				VolumeHandle          volume,
				char                 *id,
				char                **ret_title);
extern	char	*_DtHelpCeGetVolumeLocale (
				VolumeHandle	 volume);
extern	int	 _DtHelpCeOpenVolume (
				CanvasHandle	 canvas_handle,
				char		*volFile,
				VolumeHandle	*retVol);
#endif /* _DtHelpObsoleteP_h */
