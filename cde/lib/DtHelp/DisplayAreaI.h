/* $XConsortium: DisplayAreaI.h /main/2 1996/05/09 03:41:47 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        DisplayAreaI.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Defines the Display Area structures and defines.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtHelpDisplayAreaI_h
#define	_DtHelpDisplayAreaI_h

typedef	struct {
	int	 reason;
	XEvent	*event;
	Window	 window;
	char	*specification;
	int	 hyper_type;
	int	 window_hint;
} DtHelpHyperTextStruct;

#endif /* _DtHelpDisplayAreaI_h */
