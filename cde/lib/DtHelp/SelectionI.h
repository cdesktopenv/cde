/* $XConsortium: SelectionI.h /main/9 1995/12/27 16:56:32 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   SelectionI.h
 **
 **  Project:
 **
 **  Description:  Private Header file for Selection.c
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
#ifndef __DtCvSelectionI_h
#define __DtCvSelectionI_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Private Defines Declarations    ********/

/********    Private Typedef Declarations    ********/

/********    Private Structures Declarations    ********/

/********    Private Macro Declarations    ********/

/********    Private Function Declarations    ********/
extern	_DtCvStatus	_DtCvGetMarkSegs(
				_DtCanvasStruct	  *canvas,
				_DtCvPointInfo	***ret_info);
extern	void		_DtCvDrawAreaWithFlags(
				_DtCanvasStruct	  *canvas,
				_DtCvSelectData	   start,
				_DtCvSelectData	   end,
				_DtCvFlags	   old_flag,
				_DtCvFlags	   new_flag,
				_DtCvElemType	   trav_type,
				_DtCvPointer	   trav_data);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* __DtCvSelectionI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
