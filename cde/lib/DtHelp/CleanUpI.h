/* $XConsortium: CleanUpI.h /main/8 1996/01/29 12:19:46 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **  File:   CleanUpI.h
 **  Project: Common Desktop Environment
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpCleanUpI_h
#define _DtHelpCleanUpI_h

/********    Public Function Declarations    ********/
extern	void	_DtHelpFreeSegments(
				_DtCvSegment	*seg_list,
				_DtCvStatus	 unresolved,
				void		(*destroy_region)(),
				_DtCvPointer	 client_data);
extern	void	_DtHelpDestroyTopicData(
				_DtCvTopicPtr	 topic,
				void 		(*destroy_region)(),
				_DtCvPointer	 client_data);
/********    End Public Function Declarations    ********/

#endif /* _DtHelpCleanUpI_h */
