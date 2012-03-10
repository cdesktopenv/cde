/* $XConsortium: CvtToArrayP.h /main/3 1996/05/09 03:41:33 drk $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:         CvtToArrayP.h
 **
 **  Project:      Cde Help System
 **
 **  Description:  Private header file. Contains defines for converting
 **                a format specific file to a chunked string.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _DtHelpCvtToArrayP_h
#define _DtHelpCvtToArrayP_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Public Defines Declarations    ********/
/*********************************
 * Title Chunk types
 *********************************/
#define DT_HELP_CE_END		0
#define DT_HELP_CE_STRING	(1 << 0)
#define DT_HELP_CE_CHARSET	(1 << 1)
#define DT_HELP_CE_FONT_PTR	(1 << 2)
#define DT_HELP_CE_SPC		(1 << 3)
#define DT_HELP_CE_NEWLINE	(1 << 4)

/********    Public Typedef Declarations    ********/
/********    Public Enum Declarations    ********/
/********    Public Structures Declarations    ********/
/********    Public Prototype Procedures    ********/

/********    Public Macro Declarations    ********/

/********    Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtHelpCvtToArrayP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
