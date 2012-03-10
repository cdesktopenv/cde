/* $XConsortium: AsciiSpcI.h /main/4 1995/10/26 12:17:10 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   AsciiSpcI.h
 **
 **  Project:
 **
 **  Description:  Public Header file for AsciiSpc.c
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtHelpAsciiSpc_h
#define _DtHelpAsciiSpc_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Public Defines Declarations    ********/

/********    Public Typedef Declarations    ********/

/********    Public Structures Declarations    ********/

/********    Public Macro Declarations    ********/

/********    Semi-Private Function Declarations    ********/
extern	const char	*_DtHelpCeResolveSpcToAscii(
				const char	*spc_symbol);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _DtHelpAsciiSpc_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
