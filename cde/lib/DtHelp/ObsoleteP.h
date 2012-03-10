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
