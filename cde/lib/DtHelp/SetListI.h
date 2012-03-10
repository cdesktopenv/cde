/* $XConsortium: SetListI.h /main/6 1995/12/08 13:00:59 cde-hal $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SetList.h
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
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpSetListI_h
#define _DtHelpSetListI_h

#ifdef __cplusplus
extern "C" {
#endif

extern  XtPointer	_DtHelpDisplayAreaData(
				XtPointer       client_data);
extern	void		_DtHelpDisplayAreaDimensionsReturn (
				XtPointer	 client_data,
				short		*ret_rows,
				short		*ret_columns );
extern	void		_DtHelpDisplayAreaSetList (
				XtPointer	client_data,
				XtPointer	topicHandle,
				Boolean		append_flag,
				int		scroll_percent);
extern  Widget		_DtHelpDisplayAreaWidget(
				XtPointer       client_data);
extern	int		_DtHelpGetScrollbarValue (
				XtPointer       client_data);
extern	Boolean		_DtHelpSetScrollBars (
				XtPointer	client_data,
				Dimension	new_width,
				Dimension	new_height );

#ifdef __cplusplus
}
#endif
#endif /* _DtHelpSetListI_h */
