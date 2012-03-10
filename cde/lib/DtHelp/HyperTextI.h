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
