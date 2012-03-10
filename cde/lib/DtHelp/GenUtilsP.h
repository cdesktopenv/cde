/* $XConsortium: GenUtilsP.h /main/3 1996/05/09 03:42:42 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        GenUtilsP.h
 **
 **   Project:     CDEnext Info Access
 **
 **   Description: Header file for GenUtils.c
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
#ifndef _DtHelpGenUtilsP_h
#define _DtHelpGenUtilsP_h

/****************************************************************************
 *			Public Defines
 ****************************************************************************/
#ifndef	_DtHelpObsoleteP_h
typedef	void*	CanvasHandle;
typedef	void*	VolumeHandle;
#endif

/****************************************************************************
 *			Semi-Public Externals
 ****************************************************************************/
extern	CanvasHandle	_DtHelpCeCreateDefCanvas(void);
extern  int      _DtHelpCeGetAsciiVolumeAbstract (
			CanvasHandle	canvas,
			VolumeHandle	volume,
                        char            **retAbs);
extern  char    *_DtHelpCeGetNxtToken (
			char             *str,
			char            **retToken);
extern	int	 _DtHelpCeGetVolumeTitle(
			CanvasHandle	canvas,
			VolumeHandle	volume,
			char          **ret_title);
#endif /* _DtHelpGenUtilsP_h */
