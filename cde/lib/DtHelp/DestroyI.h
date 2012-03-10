/* $XConsortium: DestroyI.h /main/5 1995/10/26 12:18:47 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        DestroyTG.h
 **
 **   Project:     TextGraphic Display routines
 **
 **   Description: Header file for SetListTG.h
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
#ifndef _DtHelpDestroyI_h
#define _DtHelpDestroyI_h

extern	void	_DtHelpDisplayAreaClean (
			XtPointer  client_data);
extern	void	_DtHelpDisplayAreaDestroyCB (
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data);

#endif /* _DtHelpDestroyI_h */
