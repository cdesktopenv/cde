/* $XConsortium: LinkMgrP.h /main/2 1996/05/09 03:43:50 drk $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   LinkMgrP.h
 **
 **  Project:
 **
 **  Description:  Public Header file for the Ling Manager
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtCvLinkMgrP_h
#define _DtCvLinkMgrP_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Typedef Structures Declarations    ********/
#if !defined(_DtCanvasI_h) && !defined(_DtCvLinkMgrI_h)
typedef	struct _dtCvLinkDb*	_DtCvLinkDb;
#endif

/********    Public Function Declarations    ********/

extern	int		_DtLinkDbAddLink (
				_DtCvLinkDb	 link_db,
				char		*id,
				char		*spec,
				int		 type,
				int		 hint,
				char		*description);
extern	int		_DtLinkDbAddSwitch (
				_DtCvLinkDb	 link_db,
				char		*id,
				char		*interp,
				char		*cmd,
				char		*branches);
extern	_DtCvLinkDb	_DtLinkDbCreate (void);
extern	void		_DtLinkDbDestroy (_DtCvLinkDb link_db);
extern  void		_DtLinkDbRemoveLink(
				_DtCvLinkDb      link_data,
				int	      link_index);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtCvLinkMgrP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
